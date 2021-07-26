#ifndef QUICKJS_QUICKJS_GENERATOR_H
#define QUICKJS_QUICKJS_GENERATOR_H
typedef enum JSGeneratorStateEnum {
    JS_GENERATOR_STATE_SUSPENDED_START,
    JS_GENERATOR_STATE_SUSPENDED_YIELD,
    JS_GENERATOR_STATE_SUSPENDED_YIELD_STAR,
    JS_GENERATOR_STATE_EXECUTING,
    JS_GENERATOR_STATE_COMPLETED,
} JSGeneratorStateEnum;

typedef struct JSGeneratorData {
    JSGeneratorStateEnum state;
    JSAsyncFunctionState func_state;
} JSGeneratorData;

static void free_generator_stack_rt(JSRuntime *rt, JSGeneratorData *s)
{
    if (s->state == JS_GENERATOR_STATE_COMPLETED)
        return;
    async_func_free(rt, &s->func_state);
    s->state = JS_GENERATOR_STATE_COMPLETED;
}

static void js_generator_finalizer(JSRuntime *rt, JSValue obj)
{
    JSGeneratorData *s = JS_GetOpaque(obj, JS_CLASS_GENERATOR);

    if (s) {
        free_generator_stack_rt(rt, s);
        js_free_rt(rt, s);
    }
}

static void free_generator_stack(JSContext *ctx, JSGeneratorData *s)
{
    free_generator_stack_rt(ctx->rt, s);
}

static void js_generator_mark(JSRuntime *rt, JSValueConst val,
                              JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSGeneratorData *s = p->u.generator_data;

    if (!s || s->state == JS_GENERATOR_STATE_COMPLETED)
        return;
    async_func_mark(rt, &s->func_state, mark_func);
}

/* XXX: use enum */
#define GEN_MAGIC_NEXT   0
#define GEN_MAGIC_RETURN 1
#define GEN_MAGIC_THROW  2

static JSValue js_generator_next(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv,
                                 BOOL *pdone, int magic)
{
    JSGeneratorData *s = JS_GetOpaque(this_val, JS_CLASS_GENERATOR);
    JSStackFrame *sf;
    JSValue ret, func_ret;
    JSValueConst iter_args[1];

    *pdone = TRUE;
    if (!s)
        return JS_ThrowTypeError(ctx, "not a generator");
    sf = &s->func_state.frame;
    redo:
    switch(s->state) {
        default:
        case JS_GENERATOR_STATE_SUSPENDED_START:
            if (magic == GEN_MAGIC_NEXT) {
                goto exec_no_arg;
            } else {
                free_generator_stack(ctx, s);
                goto done;
            }
            break;
        case JS_GENERATOR_STATE_SUSPENDED_YIELD_STAR:
        {
            int done;
            JSValue method, iter_obj;

            iter_obj = sf->cur_sp[-2];
            if (magic == GEN_MAGIC_NEXT) {
                method = JS_DupValue(ctx, sf->cur_sp[-1]);
            } else {
                method = JS_GetProperty(ctx, iter_obj,
                                        magic == GEN_MAGIC_RETURN ?
                                        JS_ATOM_return : JS_ATOM_throw);
                if (JS_IsException(method))
                    goto iter_exception;
            }
            if (magic != GEN_MAGIC_NEXT &&
                (JS_IsUndefined(method) || JS_IsNull(method))) {
                /* default action */
                if (magic == GEN_MAGIC_RETURN) {
                    ret = JS_DupValue(ctx, argv[0]);
                    goto iter_done;
                } else {
                    if (JS_IteratorClose(ctx, iter_obj, FALSE))
                        goto iter_exception;
                    JS_ThrowTypeError(ctx, "iterator does not have a throw method");
                    goto iter_exception;
                }
            }
            ret = JS_IteratorNext2(ctx, iter_obj, method, argc, argv, &done);
            JS_FreeValue(ctx, method);
            if (JS_IsException(ret)) {
                iter_exception:
                goto exec_throw;
            }
            /* if not done, the iterator returns the exact object
               returned by 'method' */
            if (done == 2) {
                JSValue done_val, value;
                done_val = JS_GetProperty(ctx, ret, JS_ATOM_done);
                if (JS_IsException(done_val)) {
                    JS_FreeValue(ctx, ret);
                    goto iter_exception;
                }
                done = JS_ToBoolFree(ctx, done_val);
                if (done) {
                    value = JS_GetProperty(ctx, ret, JS_ATOM_value);
                    JS_FreeValue(ctx, ret);
                    if (JS_IsException(value))
                        goto iter_exception;
                    ret = value;
                    goto iter_done;
                } else {
                    *pdone = 2;
                }
            } else {
                if (done) {
                    /* 'yield *' returns the value associated to done = true */
                    iter_done:
                    JS_FreeValue(ctx, sf->cur_sp[-2]);
                    JS_FreeValue(ctx, sf->cur_sp[-1]);
                    sf->cur_sp--;
                    goto exec_arg;
                } else {
                    *pdone = FALSE;
                }
            }
            break;
        }
            break;
        case JS_GENERATOR_STATE_SUSPENDED_YIELD:
            /* cur_sp[-1] was set to JS_UNDEFINED in the previous call */
            ret = JS_DupValue(ctx, argv[0]);
            if (magic == GEN_MAGIC_THROW) {
                JS_Throw(ctx, ret);
                exec_throw:
                s->func_state.throw_flag = TRUE;
            } else {
                exec_arg:
                sf->cur_sp[-1] = ret;
                sf->cur_sp[0] = JS_NewBool(ctx, (magic == GEN_MAGIC_RETURN));
                sf->cur_sp++;
                exec_no_arg:
                s->func_state.throw_flag = FALSE;
            }
            s->state = JS_GENERATOR_STATE_EXECUTING;
            func_ret = async_func_resume(ctx, &s->func_state);
            s->state = JS_GENERATOR_STATE_SUSPENDED_YIELD;
            if (JS_IsException(func_ret)) {
                /* finalize the execution in case of exception */
                free_generator_stack(ctx, s);
                return func_ret;
            }
            if (JS_VALUE_GET_TAG(func_ret) == JS_TAG_INT) {
                if (JS_VALUE_GET_INT(func_ret) == FUNC_RET_YIELD_STAR) {
                    /* 'yield *' */
                    s->state = JS_GENERATOR_STATE_SUSPENDED_YIELD_STAR;
                    iter_args[0] = JS_UNDEFINED;
                    argc = 1;
                    argv = iter_args;
                    goto redo;
                } else {
                    /* get the return the yield value at the top of the stack */
                    ret = sf->cur_sp[-1];
                    sf->cur_sp[-1] = JS_UNDEFINED;
                    *pdone = FALSE;
                }
            } else {
                /* end of iterator */
                ret = sf->cur_sp[-1];
                sf->cur_sp[-1] = JS_UNDEFINED;
                JS_FreeValue(ctx, func_ret);
                free_generator_stack(ctx, s);
            }
            break;
        case JS_GENERATOR_STATE_COMPLETED:
        done:
            /* execution is finished */
            switch(magic) {
                default:
                case GEN_MAGIC_NEXT:
                    ret = JS_UNDEFINED;
                    break;
                case GEN_MAGIC_RETURN:
                    ret = JS_DupValue(ctx, argv[0]);
                    break;
                case GEN_MAGIC_THROW:
                    ret = JS_Throw(ctx, JS_DupValue(ctx, argv[0]));
                    break;
            }
            break;
        case JS_GENERATOR_STATE_EXECUTING:
            ret = JS_ThrowTypeError(ctx, "cannot invoke a running generator");
            break;
    }
    return ret;
}

static JSValue js_generator_function_call(JSContext *ctx, JSValueConst func_obj,
                                          JSValueConst this_obj,
                                          int argc, JSValueConst *argv,
                                          int flags)
{
    JSValue obj, func_ret;
    JSGeneratorData *s;

    s = js_mallocz(ctx, sizeof(*s));
    if (!s)
        return JS_EXCEPTION;
    s->state = JS_GENERATOR_STATE_SUSPENDED_START;
    if (async_func_init(ctx, &s->func_state, func_obj, this_obj, argc, argv)) {
        s->state = JS_GENERATOR_STATE_COMPLETED;
        goto fail;
    }

    /* execute the function up to 'OP_initial_yield' */
    func_ret = async_func_resume(ctx, &s->func_state);
    if (JS_IsException(func_ret))
        goto fail;
    JS_FreeValue(ctx, func_ret);

    obj = js_create_from_ctor(ctx, func_obj, JS_CLASS_GENERATOR);
    if (JS_IsException(obj))
        goto fail;
    JS_SetOpaque(obj, s);
    return obj;
    fail:
    free_generator_stack_rt(ctx->rt, s);
    js_free(ctx, s);
    return JS_EXCEPTION;
}
#endif //QUICKJS_QUICKJS_GENERATOR_H
