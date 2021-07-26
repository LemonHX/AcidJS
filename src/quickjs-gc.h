#ifndef QUICKJS_QUICKJS_GC_H
#define QUICKJS_QUICKJS_GC_H
static void add_gc_object(JSRuntime *rt, JSGCObjectHeader *h,
                          JSGCObjectTypeEnum type)
{
    h->mark = 0;
    h->gc_obj_type = type;
    list_add_tail(&h->link, &rt->gc_obj_list);
}

static void remove_gc_object(JSGCObjectHeader *h)
{
    list_del(&h->link);
}

void JS_MarkValue(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func)
{
    if (JS_VALUE_HAS_REF_COUNT(val)) {
        switch(JS_VALUE_GET_TAG(val)) {
            case JS_TAG_OBJECT:
            case JS_TAG_FUNCTION_BYTECODE:
                mark_func(rt, JS_VALUE_GET_PTR(val));
                break;
            default:
                break;
        }
    }
}

static void mark_children(JSRuntime *rt, JSGCObjectHeader *gp,
                          JS_MarkFunc *mark_func)
{
    switch(gp->gc_obj_type) {
        case JS_GC_OBJ_TYPE_JS_OBJECT:
        {
            JSObject *p = (JSObject *)gp;
            JSShapeProperty *prs;
            JSShape *sh;
            int i;
            sh = p->shape;
            mark_func(rt, &sh->header);
            /* mark all the fields */
            prs = get_shape_prop(sh);
            for(i = 0; i < sh->prop_count; i++) {
                JSProperty *pr = &p->prop[i];
                if (prs->atom != JS_ATOM_NULL) {
                    if (prs->flags & JS_PROP_TMASK) {
                        if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                            if (pr->u.getset.getter)
                                mark_func(rt, &pr->u.getset.getter->header);
                            if (pr->u.getset.setter)
                                mark_func(rt, &pr->u.getset.setter->header);
                        } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                            if (pr->u.var_ref->is_detached) {
                                /* Note: the tag does not matter
                                   provided it is a GC object */
                                mark_func(rt, &pr->u.var_ref->header);
                            }
                        } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                            js_autoinit_mark(rt, pr, mark_func);
                        }
                    } else {
                        JS_MarkValue(rt, pr->u.value, mark_func);
                    }
                }
                prs++;
            }

            if (p->class_id != JS_CLASS_OBJECT) {
                JSClassGCMark *gc_mark;
                gc_mark = rt->class_array[p->class_id].gc_mark;
                if (gc_mark)
                    gc_mark(rt, JS_MKPTR(JS_TAG_OBJECT, p), mark_func);
            }
        }
            break;
        case JS_GC_OBJ_TYPE_FUNCTION_BYTECODE:
            /* the template objects can be part of a cycle */
        {
            JSFunctionBytecode *b = (JSFunctionBytecode *)gp;
            int i;
            for(i = 0; i < b->cpool_count; i++) {
                JS_MarkValue(rt, b->cpool[i], mark_func);
            }
            if (b->realm)
                mark_func(rt, &b->realm->header);
        }
            break;
        case JS_GC_OBJ_TYPE_VAR_REF:
        {
            JSVarRef *var_ref = (JSVarRef *)gp;
            /* only detached variable referenced are taken into account */
            assert(var_ref->is_detached);
            JS_MarkValue(rt, *var_ref->pvalue, mark_func);
        }
            break;
        case JS_GC_OBJ_TYPE_ASYNC_FUNCTION:
        {
            JSAsyncFunctionData *s = (JSAsyncFunctionData *)gp;
            if (s->is_active)
                async_func_mark(rt, &s->func_state, mark_func);
            JS_MarkValue(rt, s->resolving_funcs[0], mark_func);
            JS_MarkValue(rt, s->resolving_funcs[1], mark_func);
        }
            break;
        case JS_GC_OBJ_TYPE_SHAPE:
        {
            JSShape *sh = (JSShape *)gp;
            if (sh->proto != NULL) {
                mark_func(rt, &sh->proto->header);
            }
        }
            break;
        case JS_GC_OBJ_TYPE_JS_CONTEXT:
        {
            JSContext *ctx = (JSContext *)gp;
            JS_MarkContext(rt, ctx, mark_func);
        }
            break;
        default:
            abort();
    }
}

static void gc_decref_child(JSRuntime *rt, JSGCObjectHeader *p)
{
    assert(p->ref_count > 0);
    p->ref_count--;
    if (p->ref_count == 0 && p->mark == 1) {
        list_del(&p->link);
        list_add_tail(&p->link, &rt->tmp_obj_list);
    }
}

static void gc_decref(JSRuntime *rt)
{
    struct list_head *el, *el1;
    JSGCObjectHeader *p;

    init_list_head(&rt->tmp_obj_list);

    /* decrement the refcount of all the children of all the GC
       objects and move the GC objects with zero refcount to
       tmp_obj_list */
    list_for_each_safe(el, el1, &rt->gc_obj_list) {
        p = list_entry(el, JSGCObjectHeader, link);
        assert(p->mark == 0);
        mark_children(rt, p, gc_decref_child);
        p->mark = 1;
        if (p->ref_count == 0) {
            list_del(&p->link);
            list_add_tail(&p->link, &rt->tmp_obj_list);
        }
    }
}

static void gc_scan_incref_child(JSRuntime *rt, JSGCObjectHeader *p)
{
    p->ref_count++;
    if (p->ref_count == 1) {
        /* ref_count was 0: remove from tmp_obj_list and add at the
           end of gc_obj_list */
        list_del(&p->link);
        list_add_tail(&p->link, &rt->gc_obj_list);
        p->mark = 0; /* reset the mark for the next GC call */
    }
}

static void gc_scan_incref_child2(JSRuntime *rt, JSGCObjectHeader *p)
{
    p->ref_count++;
}

static void gc_scan(JSRuntime *rt)
{
    struct list_head *el;
    JSGCObjectHeader *p;

    /* keep the objects with a refcount > 0 and their children. */
    list_for_each(el, &rt->gc_obj_list) {
        p = list_entry(el, JSGCObjectHeader, link);
        assert(p->ref_count > 0);
        p->mark = 0; /* reset the mark for the next GC call */
        mark_children(rt, p, gc_scan_incref_child);
    }

    /* restore the refcount of the objects to be deleted. */
    list_for_each(el, &rt->tmp_obj_list) {
        p = list_entry(el, JSGCObjectHeader, link);
        mark_children(rt, p, gc_scan_incref_child2);
    }
}

static void gc_free_cycles(JSRuntime *rt)
{
    struct list_head *el, *el1;
    JSGCObjectHeader *p;
#ifdef DUMP_GC_FREE
    BOOL header_done = FALSE;
#endif

    rt->gc_phase = JS_GC_PHASE_REMOVE_CYCLES;

    for(;;) {
        el = rt->tmp_obj_list.next;
        if (el == &rt->tmp_obj_list)
            break;
        p = list_entry(el, JSGCObjectHeader, link);
        /* Only need to free the GC object associated with JS
           values. The rest will be automatically removed because they
           must be referenced by them. */
        switch(p->gc_obj_type) {
            case JS_GC_OBJ_TYPE_JS_OBJECT:
            case JS_GC_OBJ_TYPE_FUNCTION_BYTECODE:
#ifdef DUMP_GC_FREE
                if (!header_done) {
                printf("Freeing cycles:\n");
                JS_DumpObjectHeader(rt);
                header_done = TRUE;
            }
            JS_DumpGCObject(rt, p);
#endif
                free_gc_object(rt, p);
                break;
            default:
                list_del(&p->link);
                list_add_tail(&p->link, &rt->gc_zero_ref_count_list);
                break;
        }
    }
    rt->gc_phase = JS_GC_PHASE_NONE;

    list_for_each_safe(el, el1, &rt->gc_zero_ref_count_list) {
        p = list_entry(el, JSGCObjectHeader, link);
        assert(p->gc_obj_type == JS_GC_OBJ_TYPE_JS_OBJECT ||
               p->gc_obj_type == JS_GC_OBJ_TYPE_FUNCTION_BYTECODE);
        js_free_rt(rt, p);
    }

    init_list_head(&rt->gc_zero_ref_count_list);
}

void JS_RunGC(JSRuntime *rt)
{
    /* decrement the reference of the children of each object. mark =
       1 after this pass. */
    gc_decref(rt);

    /* keep the GC objects with a non zero refcount and their childs */
    gc_scan(rt);

    /* free the GC objects in a cycle */
    gc_free_cycles(rt);
}

/* Return false if not an object or if the object has already been
   freed (zombie objects are visible in finalizers when freeing
   cycles). */
BOOL JS_IsLiveObject(JSRuntime *rt, JSValueConst obj)
{
    JSObject *p;
    if (!JS_IsObject(obj))
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    return !p->free_mark;
}

/* Compute memory used by various object types */
/* XXX: poor man's approach to handling multiply referenced objects */
typedef struct JSMemoryUsage_helper {
    double memory_used_count;
    double str_count;
    double str_size;
    int64_t js_func_count;
    double js_func_size;
    int64_t js_func_code_size;
    int64_t js_func_pc2line_count;
    int64_t js_func_pc2line_size;
} JSMemoryUsage_helper;

static void compute_value_size(JSValueConst val, JSMemoryUsage_helper *hp);

static void compute_jsstring_size(JSString *str, JSMemoryUsage_helper *hp)
{
    if (!str->atom_type) {  /* atoms are handled separately */
        double s_ref_count = str->header.ref_count;
        hp->str_count += 1 / s_ref_count;
        hp->str_size += ((sizeof(*str) + (str->len << str->is_wide_char) +
                          1 - str->is_wide_char) / s_ref_count);
    }
}

static void compute_bytecode_size(JSFunctionBytecode *b, JSMemoryUsage_helper *hp)
{
    int memory_used_count, js_func_size, i;

    memory_used_count = 0;
    js_func_size = offsetof(JSFunctionBytecode, debug);
    if (b->vardefs) {
        js_func_size += (b->arg_count + b->var_count) * sizeof(*b->vardefs);
    }
    if (b->cpool) {
        js_func_size += b->cpool_count * sizeof(*b->cpool);
        for (i = 0; i < b->cpool_count; i++) {
            JSValueConst val = b->cpool[i];
            compute_value_size(val, hp);
        }
    }
    if (b->closure_var) {
        js_func_size += b->closure_var_count * sizeof(*b->closure_var);
    }
    if (!b->read_only_bytecode && b->byte_code_buf) {
        hp->js_func_code_size += b->byte_code_len;
    }
    if (b->has_debug) {
        js_func_size += sizeof(*b) - offsetof(JSFunctionBytecode, debug);
        if (b->debug.source) {
            memory_used_count++;
            js_func_size += b->debug.source_len + 1;
        }
        if (b->debug.pc2line_len) {
            memory_used_count++;
            hp->js_func_pc2line_count += 1;
            hp->js_func_pc2line_size += b->debug.pc2line_len;
        }
    }
    hp->js_func_size += js_func_size;
    hp->js_func_count += 1;
    hp->memory_used_count += memory_used_count;
}

static void compute_value_size(JSValueConst val, JSMemoryUsage_helper *hp)
{
    switch(JS_VALUE_GET_TAG(val)) {
        case JS_TAG_STRING:
            compute_jsstring_size(JS_VALUE_GET_STRING(val), hp);
            break;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_INT:
    case JS_TAG_BIG_FLOAT:
    case JS_TAG_BIG_DECIMAL:
        /* should track JSBigFloat usage */
        break;
#endif
    }
}

void JS_ComputeMemoryUsage(JSRuntime *rt, JSMemoryUsage *s)
{
    struct list_head *el, *el1;
    int i;
    JSMemoryUsage_helper mem = { 0 }, *hp = &mem;

    memset(s, 0, sizeof(*s));
    s->malloc_count = rt->malloc_state.malloc_count;
    s->malloc_size = rt->malloc_state.malloc_size;
    s->malloc_limit = rt->malloc_state.malloc_limit;

    s->memory_used_count = 2; /* rt + rt->class_array */
    s->memory_used_size = sizeof(JSRuntime) + sizeof(JSValue) * rt->class_count;

    list_for_each(el, &rt->context_list) {
        JSContext *ctx = list_entry(el, JSContext, link);
        JSShape *sh = ctx->array_shape;
        s->memory_used_count += 2; /* ctx + ctx->class_proto */
        s->memory_used_size += sizeof(JSContext) +
                               sizeof(JSValue) * rt->class_count;
        s->binary_object_count += ctx->binary_object_count;
        s->binary_object_size += ctx->binary_object_size;

        /* the hashed shapes are counted separately */
        if (sh && !sh->is_hashed) {
            int hash_size = sh->prop_hash_mask + 1;
            s->shape_count++;
            s->shape_size += get_shape_size(hash_size, sh->prop_size);
        }
        list_for_each(el1, &ctx->loaded_modules) {
            JSModuleDef *m = list_entry(el1, JSModuleDef, link);
            s->memory_used_count += 1;
            s->memory_used_size += sizeof(*m);
            if (m->req_module_entries) {
                s->memory_used_count += 1;
                s->memory_used_size += m->req_module_entries_count * sizeof(*m->req_module_entries);
            }
            if (m->export_entries) {
                s->memory_used_count += 1;
                s->memory_used_size += m->export_entries_count * sizeof(*m->export_entries);
                for (i = 0; i < m->export_entries_count; i++) {
                    JSExportEntry *me = &m->export_entries[i];
                    if (me->export_type == JS_EXPORT_TYPE_LOCAL && me->u.local.var_ref) {
                        /* potential multiple count */
                        s->memory_used_count += 1;
                        compute_value_size(me->u.local.var_ref->value, hp);
                    }
                }
            }
            if (m->star_export_entries) {
                s->memory_used_count += 1;
                s->memory_used_size += m->star_export_entries_count * sizeof(*m->star_export_entries);
            }
            if (m->import_entries) {
                s->memory_used_count += 1;
                s->memory_used_size += m->import_entries_count * sizeof(*m->import_entries);
            }
            compute_value_size(m->module_ns, hp);
            compute_value_size(m->func_obj, hp);
        }
    }

    list_for_each(el, &rt->gc_obj_list) {
        JSGCObjectHeader *gp = list_entry(el, JSGCObjectHeader, link);
        JSObject *p;
        JSShape *sh;
        JSShapeProperty *prs;

        /* XXX: could count the other GC object types too */
        if (gp->gc_obj_type == JS_GC_OBJ_TYPE_FUNCTION_BYTECODE) {
            compute_bytecode_size((JSFunctionBytecode *)gp, hp);
            continue;
        } else if (gp->gc_obj_type != JS_GC_OBJ_TYPE_JS_OBJECT) {
            continue;
        }
        p = (JSObject *)gp;
        sh = p->shape;
        s->obj_count++;
        if (p->prop) {
            s->memory_used_count++;
            s->prop_size += sh->prop_size * sizeof(*p->prop);
            s->prop_count += sh->prop_count;
            prs = get_shape_prop(sh);
            for(i = 0; i < sh->prop_count; i++) {
                JSProperty *pr = &p->prop[i];
                if (prs->atom != JS_ATOM_NULL && !(prs->flags & JS_PROP_TMASK)) {
                    compute_value_size(pr->u.value, hp);
                }
                prs++;
            }
        }
        /* the hashed shapes are counted separately */
        if (!sh->is_hashed) {
            int hash_size = sh->prop_hash_mask + 1;
            s->shape_count++;
            s->shape_size += get_shape_size(hash_size, sh->prop_size);
        }

        switch(p->class_id) {
            case JS_CLASS_ARRAY:             /* u.array | length */
            case JS_CLASS_ARGUMENTS:         /* u.array | length */
                s->array_count++;
                if (p->fast_array) {
                    s->fast_array_count++;
                    if (p->u.array.u.values) {
                        s->memory_used_count++;
                        s->memory_used_size += p->u.array.count *
                                               sizeof(*p->u.array.u.values);
                        s->fast_array_elements += p->u.array.count;
                        for (i = 0; i < p->u.array.count; i++) {
                            compute_value_size(p->u.array.u.values[i], hp);
                        }
                    }
                }
                break;
            case JS_CLASS_NUMBER:            /* u.object_data */
            case JS_CLASS_STRING:            /* u.object_data */
            case JS_CLASS_BOOLEAN:           /* u.object_data */
            case JS_CLASS_SYMBOL:            /* u.object_data */
            case JS_CLASS_DATE:              /* u.object_data */
#ifdef CONFIG_BIGNUM
                case JS_CLASS_BIG_INT:           /* u.object_data */
        case JS_CLASS_BIG_FLOAT:         /* u.object_data */
        case JS_CLASS_BIG_DECIMAL:         /* u.object_data */
#endif
                compute_value_size(p->u.object_data, hp);
                break;
            case JS_CLASS_C_FUNCTION:        /* u.cfunc */
                s->c_func_count++;
                break;
            case JS_CLASS_BYTECODE_FUNCTION: /* u.func */
            {
                JSFunctionBytecode *b = p->u.func.function_bytecode;
                JSVarRef **var_refs = p->u.func.var_refs;
                /* home_object: object will be accounted for in list scan */
                if (var_refs) {
                    s->memory_used_count++;
                    s->js_func_size += b->closure_var_count * sizeof(*var_refs);
                    for (i = 0; i < b->closure_var_count; i++) {
                        if (var_refs[i]) {
                            double ref_count = var_refs[i]->header.ref_count;
                            s->memory_used_count += 1 / ref_count;
                            s->js_func_size += sizeof(*var_refs[i]) / ref_count;
                            /* handle non object closed values */
                            if (var_refs[i]->pvalue == &var_refs[i]->value) {
                                /* potential multiple count */
                                compute_value_size(var_refs[i]->value, hp);
                            }
                        }
                    }
                }
            }
                break;
            case JS_CLASS_BOUND_FUNCTION:    /* u.bound_function */
            {
                JSBoundFunction *bf = p->u.bound_function;
                /* func_obj and this_val are objects */
                for (i = 0; i < bf->argc; i++) {
                    compute_value_size(bf->argv[i], hp);
                }
                s->memory_used_count += 1;
                s->memory_used_size += sizeof(*bf) + bf->argc * sizeof(*bf->argv);
            }
                break;
            case JS_CLASS_C_FUNCTION_DATA:   /* u.c_function_data_record */
            {
                JSCFunctionDataRecord *fd = p->u.c_function_data_record;
                if (fd) {
                    for (i = 0; i < fd->data_len; i++) {
                        compute_value_size(fd->data[i], hp);
                    }
                    s->memory_used_count += 1;
                    s->memory_used_size += sizeof(*fd) + fd->data_len * sizeof(*fd->data);
                }
            }
                break;
            case JS_CLASS_REGEXP:            /* u.regexp */
                compute_jsstring_size(p->u.regexp.pattern, hp);
                compute_jsstring_size(p->u.regexp.bytecode, hp);
                break;

            case JS_CLASS_FOR_IN_ITERATOR:   /* u.for_in_iterator */
            {
                JSForInIterator *it = p->u.for_in_iterator;
                if (it) {
                    compute_value_size(it->obj, hp);
                    s->memory_used_count += 1;
                    s->memory_used_size += sizeof(*it);
                }
            }
                break;
            case JS_CLASS_ARRAY_BUFFER:      /* u.array_buffer */
            case JS_CLASS_SHARED_ARRAY_BUFFER: /* u.array_buffer */
            {
                JSArrayBuffer *abuf = p->u.array_buffer;
                if (abuf) {
                    s->memory_used_count += 1;
                    s->memory_used_size += sizeof(*abuf);
                    if (abuf->data) {
                        s->memory_used_count += 1;
                        s->memory_used_size += abuf->byte_length;
                    }
                }
            }
                break;
            case JS_CLASS_GENERATOR:         /* u.generator_data */
            case JS_CLASS_UINT8C_ARRAY:      /* u.typed_array / u.array */
            case JS_CLASS_INT8_ARRAY:        /* u.typed_array / u.array */
            case JS_CLASS_UINT8_ARRAY:       /* u.typed_array / u.array */
            case JS_CLASS_INT16_ARRAY:       /* u.typed_array / u.array */
            case JS_CLASS_UINT16_ARRAY:      /* u.typed_array / u.array */
            case JS_CLASS_INT32_ARRAY:       /* u.typed_array / u.array */
            case JS_CLASS_UINT32_ARRAY:      /* u.typed_array / u.array */
#ifdef CONFIG_BIGNUM
                case JS_CLASS_BIG_INT64_ARRAY:   /* u.typed_array / u.array */
        case JS_CLASS_BIG_UINT64_ARRAY:  /* u.typed_array / u.array */
#endif
            case JS_CLASS_FLOAT32_ARRAY:     /* u.typed_array / u.array */
            case JS_CLASS_FLOAT64_ARRAY:     /* u.typed_array / u.array */
            case JS_CLASS_DATAVIEW:          /* u.typed_array */
#ifdef CONFIG_BIGNUM
                case JS_CLASS_FLOAT_ENV:         /* u.float_env */
#endif
            case JS_CLASS_MAP:               /* u.map_state */
            case JS_CLASS_SET:               /* u.map_state */
            case JS_CLASS_WEAKMAP:           /* u.map_state */
            case JS_CLASS_WEAKSET:           /* u.map_state */
            case JS_CLASS_MAP_ITERATOR:      /* u.map_iterator_data */
            case JS_CLASS_SET_ITERATOR:      /* u.map_iterator_data */
            case JS_CLASS_ARRAY_ITERATOR:    /* u.array_iterator_data */
            case JS_CLASS_STRING_ITERATOR:   /* u.array_iterator_data */
            case JS_CLASS_PROXY:             /* u.proxy_data */
            case JS_CLASS_PROMISE:           /* u.promise_data */
            case JS_CLASS_PROMISE_RESOLVE_FUNCTION:  /* u.promise_function_data */
            case JS_CLASS_PROMISE_REJECT_FUNCTION:   /* u.promise_function_data */
            case JS_CLASS_ASYNC_FUNCTION_RESOLVE:    /* u.async_function_data */
            case JS_CLASS_ASYNC_FUNCTION_REJECT:     /* u.async_function_data */
            case JS_CLASS_ASYNC_FROM_SYNC_ITERATOR:  /* u.async_from_sync_iterator_data */
            case JS_CLASS_ASYNC_GENERATOR:   /* u.async_generator_data */
                /* TODO */
            default:
                /* XXX: class definition should have an opaque block size */
                if (p->u.opaque) {
                    s->memory_used_count += 1;
                }
                break;
        }
    }
    s->obj_size += s->obj_count * sizeof(JSObject);

    /* hashed shapes */
    s->memory_used_count++; /* rt->shape_hash */
    s->memory_used_size += sizeof(rt->shape_hash[0]) * rt->shape_hash_size;
    for(i = 0; i < rt->shape_hash_size; i++) {
        JSShape *sh;
        for(sh = rt->shape_hash[i]; sh != NULL; sh = sh->shape_hash_next) {
            int hash_size = sh->prop_hash_mask + 1;
            s->shape_count++;
            s->shape_size += get_shape_size(hash_size, sh->prop_size);
        }
    }

    /* atoms */
    s->memory_used_count += 2; /* rt->atom_array, rt->atom_hash */
    s->atom_count = rt->atom_count;
    s->atom_size = sizeof(rt->atom_array[0]) * rt->atom_size +
                   sizeof(rt->atom_hash[0]) * rt->atom_hash_size;
    for(i = 0; i < rt->atom_size; i++) {
        JSAtomStruct *p = rt->atom_array[i];
        if (!atom_is_free(p)) {
            s->atom_size += (sizeof(*p) + (p->len << p->is_wide_char) +
                             1 - p->is_wide_char);
        }
    }
    s->str_count = round(mem.str_count);
    s->str_size = round(mem.str_size);
    s->js_func_count = mem.js_func_count;
    s->js_func_size = round(mem.js_func_size);
    s->js_func_code_size = mem.js_func_code_size;
    s->js_func_pc2line_count = mem.js_func_pc2line_count;
    s->js_func_pc2line_size = mem.js_func_pc2line_size;
    s->memory_used_count += round(mem.memory_used_count) +
                            s->atom_count + s->str_count +
                            s->obj_count + s->shape_count +
                            s->js_func_count + s->js_func_pc2line_count;
    s->memory_used_size += s->atom_size + s->str_size +
                           s->obj_size + s->prop_size + s->shape_size +
                           s->js_func_size + s->js_func_code_size + s->js_func_pc2line_size;
}

void JS_DumpMemoryUsage(FILE *fp, const JSMemoryUsage *s, JSRuntime *rt)
{
    fprintf(fp, "QuickJS memory usage -- "
                #ifdef CONFIG_BIGNUM
                "BigNum "
                #endif
                CONFIG_VERSION " version, %d-bit, malloc limit: %"PRId64"\n\n",
            (int)sizeof(void *) * 8, (int64_t)(ssize_t)s->malloc_limit);
#if 1
    if (rt) {
        static const struct {
            const char *name;
            size_t size;
        } object_types[] = {
                { "JSRuntime", sizeof(JSRuntime) },
                { "JSContext", sizeof(JSContext) },
                { "JSObject", sizeof(JSObject) },
                { "JSString", sizeof(JSString) },
                { "JSFunctionBytecode", sizeof(JSFunctionBytecode) },
        };
        int i, usage_size_ok = 0;
        for(i = 0; i < countof(object_types); i++) {
            unsigned int size = object_types[i].size;
            void *p = js_malloc_rt(rt, size);
            if (p) {
                unsigned int size1 = js_malloc_usable_size_rt(rt, p);
                if (size1 >= size) {
                    usage_size_ok = 1;
                    fprintf(fp, "  %3u + %-2u  %s\n",
                            size, size1 - size, object_types[i].name);
                }
                js_free_rt(rt, p);
            }
        }
        if (!usage_size_ok) {
            fprintf(fp, "  malloc_usable_size unavailable\n");
        }
        {
            int obj_classes[JS_CLASS_INIT_COUNT + 1] = { 0 };
            int class_id;
            struct list_head *el;
            list_for_each(el, &rt->gc_obj_list) {
                JSGCObjectHeader *gp = list_entry(el, JSGCObjectHeader, link);
                JSObject *p;
                if (gp->gc_obj_type == JS_GC_OBJ_TYPE_JS_OBJECT) {
                    p = (JSObject *)gp;
                    obj_classes[min_uint32(p->class_id, JS_CLASS_INIT_COUNT)]++;
                }
            }
            fprintf(fp, "\n" "JSObject classes\n");
            if (obj_classes[0])
                fprintf(fp, "  %5d  %2.0d %s\n", obj_classes[0], 0, "none");
            for (class_id = 1; class_id < JS_CLASS_INIT_COUNT; class_id++) {
                if (obj_classes[class_id]) {
                    char buf[ATOM_GET_STR_BUF_SIZE];
                    fprintf(fp, "  %5d  %2.0d %s\n", obj_classes[class_id], class_id,
                            JS_AtomGetStrRT(rt, buf, sizeof(buf), js_std_class_def[class_id - 1].class_name));
                }
            }
            if (obj_classes[JS_CLASS_INIT_COUNT])
                fprintf(fp, "  %5d  %2.0d %s\n", obj_classes[JS_CLASS_INIT_COUNT], 0, "other");
        }
        fprintf(fp, "\n");
    }
#endif
    fprintf(fp, "%-20s %8s %8s\n", "NAME", "COUNT", "SIZE");

    if (s->malloc_count) {
        fprintf(fp, "%-20s %8"PRId64" %8"PRId64"  (%0.1f per block)\n",
                "memory allocated", s->malloc_count, s->malloc_size,
                (double)s->malloc_size / s->malloc_count);
        fprintf(fp, "%-20s %8"PRId64" %8"PRId64"  (%d overhead, %0.1f average slack)\n",
                "memory used", s->memory_used_count, s->memory_used_size,
                MALLOC_OVERHEAD, ((double)(s->malloc_size - s->memory_used_size) /
                                  s->memory_used_count));
    }
    if (s->atom_count) {
        fprintf(fp, "%-20s %8"PRId64" %8"PRId64"  (%0.1f per atom)\n",
                "atoms", s->atom_count, s->atom_size,
                (double)s->atom_size / s->atom_count);
    }
    if (s->str_count) {
        fprintf(fp, "%-20s %8"PRId64" %8"PRId64"  (%0.1f per string)\n",
                "strings", s->str_count, s->str_size,
                (double)s->str_size / s->str_count);
    }
    if (s->obj_count) {
        fprintf(fp, "%-20s %8"PRId64" %8"PRId64"  (%0.1f per object)\n",
                "objects", s->obj_count, s->obj_size,
                (double)s->obj_size / s->obj_count);
        fprintf(fp, "%-20s %8"PRId64" %8"PRId64"  (%0.1f per object)\n",
                "  properties", s->prop_count, s->prop_size,
                (double)s->prop_count / s->obj_count);
        fprintf(fp, "%-20s %8"PRId64" %8"PRId64"  (%0.1f per shape)\n",
                "  shapes", s->shape_count, s->shape_size,
                (double)s->shape_size / s->shape_count);
    }
    if (s->js_func_count) {
        fprintf(fp, "%-20s %8"PRId64" %8"PRId64"\n",
                "bytecode functions", s->js_func_count, s->js_func_size);
        fprintf(fp, "%-20s %8"PRId64" %8"PRId64"  (%0.1f per function)\n",
                "  bytecode", s->js_func_count, s->js_func_code_size,
                (double)s->js_func_code_size / s->js_func_count);
        if (s->js_func_pc2line_count) {
            fprintf(fp, "%-20s %8"PRId64" %8"PRId64"  (%0.1f per function)\n",
                    "  pc2line", s->js_func_pc2line_count,
                    s->js_func_pc2line_size,
                    (double)s->js_func_pc2line_size / s->js_func_pc2line_count);
        }
    }
    if (s->c_func_count) {
        fprintf(fp, "%-20s %8"PRId64"\n", "C functions", s->c_func_count);
    }
    if (s->array_count) {
        fprintf(fp, "%-20s %8"PRId64"\n", "arrays", s->array_count);
        if (s->fast_array_count) {
            fprintf(fp, "%-20s %8"PRId64"\n", "  fast arrays", s->fast_array_count);
            fprintf(fp, "%-20s %8"PRId64" %8"PRId64"  (%0.1f per fast array)\n",
                    "  elements", s->fast_array_elements,
                    s->fast_array_elements * (int)sizeof(JSValue),
                    (double)s->fast_array_elements / s->fast_array_count);
        }
    }
    if (s->binary_object_count) {
        fprintf(fp, "%-20s %8"PRId64" %8"PRId64"\n",
                "binary objects", s->binary_object_count, s->binary_object_size);
    }
}

JSValue JS_GetGlobalObject(JSContext *ctx)
{
    return JS_DupValue(ctx, ctx->global_obj);
}

/* WARNING: obj is freed */
JSValue JS_Throw(JSContext *ctx, JSValue obj)
{
    JSRuntime *rt = ctx->rt;
    JS_FreeValue(ctx, rt->current_exception);
    rt->current_exception = obj;
    return JS_EXCEPTION;
}

/* return the pending exception (cannot be called twice). */
JSValue JS_GetException(JSContext *ctx)
{
    JSValue val;
    JSRuntime *rt = ctx->rt;
    val = rt->current_exception;
    rt->current_exception = JS_NULL;
    return val;
}

static void dbuf_put_leb128(DynBuf *s, uint32_t v)
{
    uint32_t a;
    for(;;) {
        a = v & 0x7f;
        v >>= 7;
        if (v != 0) {
            dbuf_putc(s, a | 0x80);
        } else {
            dbuf_putc(s, a);
            break;
        }
    }
}

static void dbuf_put_sleb128(DynBuf *s, int32_t v1)
{
    uint32_t v = v1;
    dbuf_put_leb128(s, (2 * v) ^ -(v >> 31));
}

static int get_leb128(uint32_t *pval, const uint8_t *buf,
                      const uint8_t *buf_end)
{
    const uint8_t *ptr = buf;
    uint32_t v, a, i;
    v = 0;
    for(i = 0; i < 5; i++) {
        if (unlikely(ptr >= buf_end))
            break;
        a = *ptr++;
        v |= (a & 0x7f) << (i * 7);
        if (!(a & 0x80)) {
            *pval = v;
            return ptr - buf;
        }
    }
    *pval = 0;
    return -1;
}

static int get_sleb128(int32_t *pval, const uint8_t *buf,
                       const uint8_t *buf_end)
{
    int ret;
    uint32_t val;
    ret = get_leb128(&val, buf, buf_end);
    if (ret < 0) {
        *pval = 0;
        return -1;
    }
    *pval = (val >> 1) ^ -(val & 1);
    return ret;
}

static int find_line_num(JSContext *ctx, JSFunctionBytecode *b,
                         uint32_t pc_value)
{
    const uint8_t *p_end, *p;
    int new_line_num, line_num, pc, v, ret;
    unsigned int op;

    if (!b->has_debug || !b->debug.pc2line_buf) {
        /* function was stripped */
        return -1;
    }

    p = b->debug.pc2line_buf;
    p_end = p + b->debug.pc2line_len;
    pc = 0;
    line_num = b->debug.line_num;
    while (p < p_end) {
        op = *p++;
        if (op == 0) {
            uint32_t val;
            ret = get_leb128(&val, p, p_end);
            if (ret < 0)
                goto fail;
            pc += val;
            p += ret;
            ret = get_sleb128(&v, p, p_end);
            if (ret < 0) {
                fail:
                /* should never happen */
                return b->debug.line_num;
            }
            p += ret;
            new_line_num = line_num + v;
        } else {
            op -= PC2LINE_OP_FIRST;
            pc += (op / PC2LINE_RANGE);
            new_line_num = line_num + (op % PC2LINE_RANGE) + PC2LINE_BASE;
        }
        if (pc_value < pc)
            return line_num;
        line_num = new_line_num;
    }
    return line_num;
}

/* in order to avoid executing arbitrary code during the stack trace
   generation, we only look at simple 'name' properties containing a
   string. */
static const char *get_func_name(JSContext *ctx, JSValueConst func)
{
    JSProperty *pr;
    JSShapeProperty *prs;
    JSValueConst val;

    if (JS_VALUE_GET_TAG(func) != JS_TAG_OBJECT)
        return NULL;
    prs = find_own_property(&pr, JS_VALUE_GET_OBJ(func), JS_ATOM_name);
    if (!prs)
        return NULL;
    if ((prs->flags & JS_PROP_TMASK) != JS_PROP_NORMAL)
        return NULL;
    val = pr->u.value;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_STRING)
        return NULL;
    return JS_ToCString(ctx, val);
}

#define JS_BACKTRACE_FLAG_SKIP_FIRST_LEVEL (1 << 0)
/* only taken into account if filename is provided */
#define JS_BACKTRACE_FLAG_SINGLE_LEVEL     (1 << 1)

/* if filename != NULL, an additional level is added with the filename
   and line number information (used for parse error). */
static void build_backtrace(JSContext *ctx, JSValueConst error_obj,
                            const char *filename, int line_num,
                            int backtrace_flags)
{
    JSStackFrame *sf;
    JSValue str;
    DynBuf dbuf;
    const char *func_name_str;
    const char *str1;
    JSObject *p;
    BOOL backtrace_barrier;

    js_dbuf_init(ctx, &dbuf);
    if (filename) {
        dbuf_printf(&dbuf, "    at %s", filename);
        if (line_num != -1)
            dbuf_printf(&dbuf, ":%d", line_num);
        dbuf_putc(&dbuf, '\n');
        str = JS_NewString(ctx, filename);
        JS_DefinePropertyValue(ctx, error_obj, JS_ATOM_fileName, str,
                               JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
        JS_DefinePropertyValue(ctx, error_obj, JS_ATOM_lineNumber, JS_NewInt32(ctx, line_num),
                               JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
        if (backtrace_flags & JS_BACKTRACE_FLAG_SINGLE_LEVEL)
            goto done;
    }
    for(sf = ctx->rt->current_stack_frame; sf != NULL; sf = sf->prev_frame) {
        if (backtrace_flags & JS_BACKTRACE_FLAG_SKIP_FIRST_LEVEL) {
            backtrace_flags &= ~JS_BACKTRACE_FLAG_SKIP_FIRST_LEVEL;
            continue;
        }
        func_name_str = get_func_name(ctx, sf->cur_func);
        if (!func_name_str || func_name_str[0] == '\0')
            str1 = "<anonymous>";
        else
            str1 = func_name_str;
        dbuf_printf(&dbuf, "    at %s", str1);
        JS_FreeCString(ctx, func_name_str);

        p = JS_VALUE_GET_OBJ(sf->cur_func);
        backtrace_barrier = FALSE;
        if (js_class_has_bytecode(p->class_id)) {
            JSFunctionBytecode *b;
            const char *atom_str;
            int line_num1;

            b = p->u.func.function_bytecode;
            backtrace_barrier = b->backtrace_barrier;
            if (b->has_debug) {
                line_num1 = find_line_num(ctx, b,
                                          sf->cur_pc - b->byte_code_buf - 1);
                atom_str = JS_AtomToCString(ctx, b->debug.filename);
                dbuf_printf(&dbuf, " (%s",
                            atom_str ? atom_str : "<null>");
                JS_FreeCString(ctx, atom_str);
                if (line_num1 != -1)
                    dbuf_printf(&dbuf, ":%d", line_num1);
                dbuf_putc(&dbuf, ')');
            }
        } else {
            dbuf_printf(&dbuf, " (native)");
        }
        dbuf_putc(&dbuf, '\n');
        /* stop backtrace if JS_EVAL_FLAG_BACKTRACE_BARRIER was used */
        if (backtrace_barrier)
            break;
    }
    done:
    dbuf_putc(&dbuf, '\0');
    if (dbuf_error(&dbuf))
        str = JS_NULL;
    else
        str = JS_NewString(ctx, (char *)dbuf.buf);
    dbuf_free(&dbuf);
    JS_DefinePropertyValue(ctx, error_obj, JS_ATOM_stack, str,
                           JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
}

/* Note: it is important that no exception is returned by this function */
static BOOL is_backtrace_needed(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    if (p->class_id != JS_CLASS_ERROR)
        return FALSE;
    if (find_own_property1(p, JS_ATOM_stack))
        return FALSE;
    return TRUE;
}

JSValue JS_NewError(JSContext *ctx)
{
    return JS_NewObjectClass(ctx, JS_CLASS_ERROR);
}

static JSValue JS_ThrowError2(JSContext *ctx, JSErrorEnum error_num,
                              const char *fmt, va_list ap, BOOL add_backtrace)
{
    char buf[256];
    JSValue obj, ret;

    vsnprintf(buf, sizeof(buf), fmt, ap);
    obj = JS_NewObjectProtoClass(ctx, ctx->native_error_proto[error_num],
                                 JS_CLASS_ERROR);
    if (unlikely(JS_IsException(obj))) {
        /* out of memory: throw JS_NULL to avoid recursing */
        obj = JS_NULL;
    } else {
        JS_DefinePropertyValue(ctx, obj, JS_ATOM_message,
                               JS_NewString(ctx, buf),
                               JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    }
    if (add_backtrace) {
        build_backtrace(ctx, obj, NULL, 0, 0);
    }
    ret = JS_Throw(ctx, obj);
    return ret;
}

static JSValue JS_ThrowError(JSContext *ctx, JSErrorEnum error_num,
                             const char *fmt, va_list ap)
{
    JSRuntime *rt = ctx->rt;
    JSStackFrame *sf;
    BOOL add_backtrace;

    /* the backtrace is added later if called from a bytecode function */
    sf = rt->current_stack_frame;
    add_backtrace = !rt->in_out_of_memory &&
                    (!sf || (JS_GetFunctionBytecode(sf->cur_func) == NULL));
    return JS_ThrowError2(ctx, error_num, fmt, ap, add_backtrace);
}

JSValue __js_printf_like(2, 3) JS_ThrowSyntaxError(JSContext *ctx, const char *fmt, ...)
{
JSValue val;
va_list ap;

va_start(ap, fmt);
val = JS_ThrowError(ctx, JS_SYNTAX_ERROR, fmt, ap);
va_end(ap);
return val;
}

JSValue __js_printf_like(2, 3) JS_ThrowTypeError(JSContext *ctx, const char *fmt, ...)
{
JSValue val;
va_list ap;

va_start(ap, fmt);
val = JS_ThrowError(ctx, JS_TYPE_ERROR, fmt, ap);
va_end(ap);
return val;
}

static int __js_printf_like(3, 4) JS_ThrowTypeErrorOrFalse(JSContext *ctx, int flags, const char *fmt, ...)
{
va_list ap;

if ((flags & JS_PROP_THROW) ||
((flags & JS_PROP_THROW_STRICT) && is_strict_mode(ctx))) {
va_start(ap, fmt);
JS_ThrowError(ctx, JS_TYPE_ERROR, fmt, ap);
va_end(ap);
return -1;
} else {
return FALSE;
}
}

/* never use it directly */
static JSValue __js_printf_like(3, 4) __JS_ThrowTypeErrorAtom(JSContext *ctx, JSAtom atom, const char *fmt, ...)
{
char buf[ATOM_GET_STR_BUF_SIZE];
return JS_ThrowTypeError(ctx, fmt,
        JS_AtomGetStr(ctx, buf, sizeof(buf), atom));
}

/* never use it directly */
static JSValue __js_printf_like(3, 4) __JS_ThrowSyntaxErrorAtom(JSContext *ctx, JSAtom atom, const char *fmt, ...)
{
char buf[ATOM_GET_STR_BUF_SIZE];
return JS_ThrowSyntaxError(ctx, fmt,
        JS_AtomGetStr(ctx, buf, sizeof(buf), atom));
}

/* %s is replaced by 'atom'. The macro is used so that gcc can check
    the format string. */
#define JS_ThrowTypeErrorAtom(ctx, fmt, atom) __JS_ThrowTypeErrorAtom(ctx, atom, fmt, "")
#define JS_ThrowSyntaxErrorAtom(ctx, fmt, atom) __JS_ThrowSyntaxErrorAtom(ctx, atom, fmt, "")

static int JS_ThrowTypeErrorReadOnly(JSContext *ctx, int flags, JSAtom atom)
{
    if ((flags & JS_PROP_THROW) ||
        ((flags & JS_PROP_THROW_STRICT) && is_strict_mode(ctx))) {
        JS_ThrowTypeErrorAtom(ctx, "'%s' is read-only", atom);
        return -1;
    } else {
        return FALSE;
    }
}

JSValue __js_printf_like(2, 3) JS_ThrowReferenceError(JSContext *ctx, const char *fmt, ...)
{
JSValue val;
va_list ap;

va_start(ap, fmt);
val = JS_ThrowError(ctx, JS_REFERENCE_ERROR, fmt, ap);
va_end(ap);
return val;
}

JSValue __js_printf_like(2, 3) JS_ThrowRangeError(JSContext *ctx, const char *fmt, ...)
{
JSValue val;
va_list ap;

va_start(ap, fmt);
val = JS_ThrowError(ctx, JS_RANGE_ERROR, fmt, ap);
va_end(ap);
return val;
}

JSValue __js_printf_like(2, 3) JS_ThrowInternalError(JSContext *ctx, const char *fmt, ...)
{
JSValue val;
va_list ap;

va_start(ap, fmt);
val = JS_ThrowError(ctx, JS_INTERNAL_ERROR, fmt, ap);
va_end(ap);
return val;
}

JSValue JS_ThrowOutOfMemory(JSContext *ctx)
{
    JSRuntime *rt = ctx->rt;
    if (!rt->in_out_of_memory) {
        rt->in_out_of_memory = TRUE;
        JS_ThrowInternalError(ctx, "out of memory");
        rt->in_out_of_memory = FALSE;
    }
    return JS_EXCEPTION;
}

static JSValue JS_ThrowStackOverflow(JSContext *ctx)
{
    return JS_ThrowInternalError(ctx, "stack overflow");
}

static JSValue JS_ThrowTypeErrorNotAnObject(JSContext *ctx)
{
    return JS_ThrowTypeError(ctx, "not an object");
}

static JSValue JS_ThrowTypeErrorNotASymbol(JSContext *ctx)
{
    return JS_ThrowTypeError(ctx, "not a symbol");
}

static JSValue JS_ThrowReferenceErrorNotDefined(JSContext *ctx, JSAtom name)
{
    char buf[ATOM_GET_STR_BUF_SIZE];
    return JS_ThrowReferenceError(ctx, "'%s' is not defined",
                                  JS_AtomGetStr(ctx, buf, sizeof(buf), name));
}

static JSValue JS_ThrowReferenceErrorUninitialized(JSContext *ctx, JSAtom name)
{
    char buf[ATOM_GET_STR_BUF_SIZE];
    return JS_ThrowReferenceError(ctx, "%s is not initialized",
                                  name == JS_ATOM_NULL ? "lexical variable" :
                                  JS_AtomGetStr(ctx, buf, sizeof(buf), name));
}

static JSValue JS_ThrowTypeErrorInvalidClass(JSContext *ctx, int class_id)
{
    JSRuntime *rt = ctx->rt;
    JSAtom name;
    name = rt->class_array[class_id].class_name;
    return JS_ThrowTypeErrorAtom(ctx, "%s object expected", name);
}

static no_inline __exception int __js_poll_interrupts(JSContext *ctx)
{
    JSRuntime *rt = ctx->rt;
    ctx->interrupt_counter = JS_INTERRUPT_COUNTER_INIT;
    if (rt->interrupt_handler) {
        if (rt->interrupt_handler(rt, rt->interrupt_opaque)) {
            /* XXX: should set a specific flag to avoid catching */
            JS_ThrowInternalError(ctx, "interrupted");
            JS_SetUncatchableError(ctx, ctx->rt->current_exception, TRUE);
            return -1;
        }
    }
    return 0;
}

static inline __exception int js_poll_interrupts(JSContext *ctx)
{
    if (unlikely(--ctx->interrupt_counter <= 0)) {
        return __js_poll_interrupts(ctx);
    } else {
        return 0;
    }
}

/* return -1 (exception) or TRUE/FALSE */
static int JS_SetPrototypeInternal(JSContext *ctx, JSValueConst obj,
                                   JSValueConst proto_val,
                                   BOOL throw_flag)
{
    JSObject *proto, *p, *p1;
    JSShape *sh;

    if (throw_flag) {
        if (JS_VALUE_GET_TAG(obj) == JS_TAG_NULL ||
            JS_VALUE_GET_TAG(obj) == JS_TAG_UNDEFINED)
            goto not_obj;
    } else {
        if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
            goto not_obj;
    }
    p = JS_VALUE_GET_OBJ(obj);
    if (JS_VALUE_GET_TAG(proto_val) != JS_TAG_OBJECT) {
        if (JS_VALUE_GET_TAG(proto_val) != JS_TAG_NULL) {
            not_obj:
            JS_ThrowTypeErrorNotAnObject(ctx);
            return -1;
        }
        proto = NULL;
    } else {
        proto = JS_VALUE_GET_OBJ(proto_val);
    }

    if (throw_flag && JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        return TRUE;

    if (unlikely(p->class_id == JS_CLASS_PROXY))
        return js_proxy_setPrototypeOf(ctx, obj, proto_val, throw_flag);
    sh = p->shape;
    if (sh->proto == proto)
        return TRUE;
    if (!p->extensible) {
        if (throw_flag) {
            JS_ThrowTypeError(ctx, "object is not extensible");
            return -1;
        } else {
            return FALSE;
        }
    }
    if (proto) {
        /* check if there is a cycle */
        p1 = proto;
        do {
            if (p1 == p) {
                if (throw_flag) {
                    JS_ThrowTypeError(ctx, "circular prototype chain");
                    return -1;
                } else {
                    return FALSE;
                }
            }
            /* Note: for Proxy objects, proto is NULL */
            p1 = p1->shape->proto;
        } while (p1 != NULL);
        JS_DupValue(ctx, proto_val);
    }

    if (js_shape_prepare_update(ctx, p, NULL))
        return -1;
    sh = p->shape;
    if (sh->proto)
        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, sh->proto));
    sh->proto = proto;
    return TRUE;
}

/* return -1 (exception) or TRUE/FALSE */
int JS_SetPrototype(JSContext *ctx, JSValueConst obj, JSValueConst proto_val)
{
    return JS_SetPrototypeInternal(ctx, obj, proto_val, TRUE);
}

/* Only works for primitive types, otherwise return JS_NULL. */
static JSValueConst JS_GetPrototypePrimitive(JSContext *ctx, JSValueConst val)
{
    switch(JS_VALUE_GET_NORM_TAG(val)) {
#ifdef CONFIG_BIGNUM
        case JS_TAG_BIG_INT:
        val = ctx->class_proto[JS_CLASS_BIG_INT];
        break;
    case JS_TAG_BIG_FLOAT:
        val = ctx->class_proto[JS_CLASS_BIG_FLOAT];
        break;
    case JS_TAG_BIG_DECIMAL:
        val = ctx->class_proto[JS_CLASS_BIG_DECIMAL];
        break;
#endif
        case JS_TAG_INT:
        case JS_TAG_FLOAT64:
            val = ctx->class_proto[JS_CLASS_NUMBER];
            break;
        case JS_TAG_BOOL:
            val = ctx->class_proto[JS_CLASS_BOOLEAN];
            break;
        case JS_TAG_STRING:
            val = ctx->class_proto[JS_CLASS_STRING];
            break;
        case JS_TAG_SYMBOL:
            val = ctx->class_proto[JS_CLASS_SYMBOL];
            break;
        case JS_TAG_OBJECT:
        case JS_TAG_NULL:
        case JS_TAG_UNDEFINED:
        default:
            val = JS_NULL;
            break;
    }
    return val;
}

/* Return an Object, JS_NULL or JS_EXCEPTION in case of Proxy object. */
JSValue JS_GetPrototype(JSContext *ctx, JSValueConst obj)
{
    JSValue val;
    if (JS_VALUE_GET_TAG(obj) == JS_TAG_OBJECT) {
        JSObject *p;
        p = JS_VALUE_GET_OBJ(obj);
        if (unlikely(p->class_id == JS_CLASS_PROXY)) {
            val = js_proxy_getPrototypeOf(ctx, obj);
        } else {
            p = p->shape->proto;
            if (!p)
                val = JS_NULL;
            else
                val = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
        }
    } else {
        val = JS_DupValue(ctx, JS_GetPrototypePrimitive(ctx, obj));
    }
    return val;
}

static JSValue JS_GetPrototypeFree(JSContext *ctx, JSValue obj)
{
    JSValue obj1;
    obj1 = JS_GetPrototype(ctx, obj);
    JS_FreeValue(ctx, obj);
    return obj1;
}

/* return TRUE, FALSE or (-1) in case of exception */
static int JS_OrdinaryIsInstanceOf(JSContext *ctx, JSValueConst val,
                                   JSValueConst obj)
{
    JSValue obj_proto;
    JSObject *proto;
    const JSObject *p, *proto1;
    BOOL ret;

    if (!JS_IsFunction(ctx, obj))
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    if (p->class_id == JS_CLASS_BOUND_FUNCTION) {
        JSBoundFunction *s = p->u.bound_function;
        return JS_IsInstanceOf(ctx, val, s->func_obj);
    }

    /* Only explicitly boxed values are instances of constructors */
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    obj_proto = JS_GetProperty(ctx, obj, JS_ATOM_prototype);
    if (JS_VALUE_GET_TAG(obj_proto) != JS_TAG_OBJECT) {
        if (!JS_IsException(obj_proto))
            JS_ThrowTypeError(ctx, "operand 'prototype' property is not an object");
        ret = -1;
        goto done;
    }
    proto = JS_VALUE_GET_OBJ(obj_proto);
    p = JS_VALUE_GET_OBJ(val);
    for(;;) {
        proto1 = p->shape->proto;
        if (!proto1) {
            /* slow case if proxy in the prototype chain */
            if (unlikely(p->class_id == JS_CLASS_PROXY)) {
                JSValue obj1;
                obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, (JSObject *)p));
                for(;;) {
                    obj1 = JS_GetPrototypeFree(ctx, obj1);
                    if (JS_IsException(obj1)) {
                        ret = -1;
                        break;
                    }
                    if (JS_IsNull(obj1)) {
                        ret = FALSE;
                        break;
                    }
                    if (proto == JS_VALUE_GET_OBJ(obj1)) {
                        JS_FreeValue(ctx, obj1);
                        ret = TRUE;
                        break;
                    }
                    /* must check for timeout to avoid infinite loop */
                    if (js_poll_interrupts(ctx)) {
                        JS_FreeValue(ctx, obj1);
                        ret = -1;
                        break;
                    }
                }
            } else {
                ret = FALSE;
            }
            break;
        }
        p = proto1;
        if (proto == p) {
            ret = TRUE;
            break;
        }
    }
    done:
    JS_FreeValue(ctx, obj_proto);
    return ret;
}

/* return TRUE, FALSE or (-1) in case of exception */
int JS_IsInstanceOf(JSContext *ctx, JSValueConst val, JSValueConst obj)
{
    JSValue method;

    if (!JS_IsObject(obj))
        goto fail;
    method = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_hasInstance);
    if (JS_IsException(method))
        return -1;
    if (!JS_IsNull(method) && !JS_IsUndefined(method)) {
        JSValue ret;
        ret = JS_CallFree(ctx, method, obj, 1, &val);
        return JS_ToBoolFree(ctx, ret);
    }

    /* legacy case */
    if (!JS_IsFunction(ctx, obj)) {
        fail:
        JS_ThrowTypeError(ctx, "invalid 'instanceof' right operand");
        return -1;
    }
    return JS_OrdinaryIsInstanceOf(ctx, val, obj);
}

typedef int JSAutoInitFunc(JSContext *ctx, JSObject *p, JSAtom atom, void *opaque);

static JSAutoInitFunc *js_autoinit_func_table[] = {
        js_instantiate_prototype, /* JS_AUTOINIT_ID_PROTOTYPE */
        js_module_ns_autoinit, /* JS_AUTOINIT_ID_MODULE_NS */
        JS_InstantiateFunctionListItem, /* JS_AUTOINIT_ID_PROP */
};

static int JS_AutoInitProperty(JSContext *ctx, JSObject *p, JSAtom prop,
                               JSProperty *pr)
{
    int ret;
    JSContext *realm;
    JSAutoInitFunc *func;

    realm = js_autoinit_get_realm(pr);
    func = js_autoinit_func_table[js_autoinit_get_id(pr)];
    ret = func(realm, p, prop, pr->u.init.opaque);
    return ret;
}

JSValue JS_GetPropertyInternal(JSContext *ctx, JSValueConst obj,
                               JSAtom prop, JSValueConst this_obj,
                               BOOL throw_ref_error)
{
    JSObject *p;
    JSProperty *pr;
    JSShapeProperty *prs;
    uint32_t tag;

    tag = JS_VALUE_GET_TAG(obj);
    if (unlikely(tag != JS_TAG_OBJECT)) {
        switch(tag) {
            case JS_TAG_NULL:
                return JS_ThrowTypeErrorAtom(ctx, "cannot read property '%s' of null", prop);
            case JS_TAG_UNDEFINED:
                return JS_ThrowTypeErrorAtom(ctx, "cannot read property '%s' of undefined", prop);
            case JS_TAG_EXCEPTION:
                return JS_EXCEPTION;
            case JS_TAG_STRING:
            {
                JSString *p1 = JS_VALUE_GET_STRING(obj);
                if (__JS_AtomIsTaggedInt(prop)) {
                    uint32_t idx, ch;
                    idx = __JS_AtomToUInt32(prop);
                    if (idx < p1->len) {
                        if (p1->is_wide_char)
                            ch = p1->u.str16[idx];
                        else
                            ch = p1->u.str8[idx];
                        return js_new_string_char(ctx, ch);
                    }
                } else if (prop == JS_ATOM_length) {
                    return JS_NewInt32(ctx, p1->len);
                }
            }
                break;
            default:
                break;
        }
        /* cannot raise an exception */
        p = JS_VALUE_GET_OBJ(JS_GetPrototypePrimitive(ctx, obj));
        if (!p)
            return JS_UNDEFINED;
    } else {
        p = JS_VALUE_GET_OBJ(obj);
    }

    for(;;) {
        prs = find_own_property(&pr, p, prop);
        if (prs) {
            /* found */
            if (unlikely(prs->flags & JS_PROP_TMASK)) {
                if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                    if (unlikely(!pr->u.getset.getter)) {
                        return JS_UNDEFINED;
                    } else {
                        JSValue func = JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.getter);
                        /* Note: the field could be removed in the getter */
                        func = JS_DupValue(ctx, func);
                        return JS_CallFree(ctx, func, this_obj, 0, NULL);
                    }
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                    JSValue val = *pr->u.var_ref->pvalue;
                    if (unlikely(JS_IsUninitialized(val)))
                        return JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
                    return JS_DupValue(ctx, val);
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                    /* Instantiate property and retry */
                    if (JS_AutoInitProperty(ctx, p, prop, pr))
                        return JS_EXCEPTION;
                    continue;
                }
            } else {
                return JS_DupValue(ctx, pr->u.value);
            }
        }
        if (unlikely(p->is_exotic)) {
            /* exotic behaviors */
            if (p->fast_array) {
                if (__JS_AtomIsTaggedInt(prop)) {
                    uint32_t idx = __JS_AtomToUInt32(prop);
                    if (idx < p->u.array.count) {
                        /* we avoid duplicating the code */
                        return JS_GetPropertyUint32(ctx, JS_MKPTR(JS_TAG_OBJECT, p), idx);
                    } else if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                               p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                        goto typed_array_oob;
                    }
                } else if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                           p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                    int ret;
                    ret = JS_AtomIsNumericIndex(ctx, prop);
                    if (ret != 0) {
                        if (ret < 0)
                            return JS_EXCEPTION;
                        typed_array_oob:
                        if (typed_array_is_detached(ctx, p))
                            return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
                        return JS_UNDEFINED;
                    }
                }
            } else {
                const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
                if (em) {
                    if (em->get_property) {
                        JSValue obj1, retval;
                        /* XXX: should pass throw_ref_error */
                        /* Note: if 'p' is a prototype, it can be
                           freed in the called function */
                        obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
                        retval = em->get_property(ctx, obj1, prop, this_obj);
                        JS_FreeValue(ctx, obj1);
                        return retval;
                    }
                    if (em->get_own_property) {
                        JSPropertyDescriptor desc;
                        int ret;
                        JSValue obj1;

                        /* Note: if 'p' is a prototype, it can be
                           freed in the called function */
                        obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
                        ret = em->get_own_property(ctx, &desc, obj1, prop);
                        JS_FreeValue(ctx, obj1);
                        if (ret < 0)
                            return JS_EXCEPTION;
                        if (ret) {
                            if (desc.flags & JS_PROP_GETSET) {
                                JS_FreeValue(ctx, desc.setter);
                                return JS_CallFree(ctx, desc.getter, this_obj, 0, NULL);
                            } else {
                                return desc.value;
                            }
                        }
                    }
                }
            }
        }
        p = p->shape->proto;
        if (!p)
            break;
    }
    if (unlikely(throw_ref_error)) {
        return JS_ThrowReferenceErrorNotDefined(ctx, prop);
    } else {
        return JS_UNDEFINED;
    }
}

static JSValue JS_ThrowTypeErrorPrivateNotFound(JSContext *ctx, JSAtom atom)
{
    return JS_ThrowTypeErrorAtom(ctx, "private class field '%s' does not exist",
                                 atom);
}

/* Private fields can be added even on non extensible objects or
   Proxies */
static int JS_DefinePrivateField(JSContext *ctx, JSValueConst obj,
                                 JSValueConst name, JSValue val)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSAtom prop;

    if (unlikely(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        goto fail;
    }
    /* safety check */
    if (unlikely(JS_VALUE_GET_TAG(name) != JS_TAG_SYMBOL)) {
        JS_ThrowTypeErrorNotASymbol(ctx);
        goto fail;
    }
    prop = js_symbol_to_atom(ctx, name);
    p = JS_VALUE_GET_OBJ(obj);
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        JS_ThrowTypeErrorAtom(ctx, "private class field '%s' already exists",
                              prop);
        goto fail;
    }
    pr = add_property(ctx, p, prop, JS_PROP_C_W_E);
    if (unlikely(!pr)) {
        fail:
        JS_FreeValue(ctx, val);
        return -1;
    }
    pr->u.value = val;
    return 0;
}

static JSValue JS_GetPrivateField(JSContext *ctx, JSValueConst obj,
                                  JSValueConst name)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSAtom prop;

    if (unlikely(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT))
        return JS_ThrowTypeErrorNotAnObject(ctx);
    /* safety check */
    if (unlikely(JS_VALUE_GET_TAG(name) != JS_TAG_SYMBOL))
        return JS_ThrowTypeErrorNotASymbol(ctx);
    prop = js_symbol_to_atom(ctx, name);
    p = JS_VALUE_GET_OBJ(obj);
    prs = find_own_property(&pr, p, prop);
    if (!prs) {
        JS_ThrowTypeErrorPrivateNotFound(ctx, prop);
        return JS_EXCEPTION;
    }
    return JS_DupValue(ctx, pr->u.value);
}

static int JS_SetPrivateField(JSContext *ctx, JSValueConst obj,
                              JSValueConst name, JSValue val)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSAtom prop;

    if (unlikely(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        goto fail;
    }
    /* safety check */
    if (unlikely(JS_VALUE_GET_TAG(name) != JS_TAG_SYMBOL)) {
        JS_ThrowTypeErrorNotASymbol(ctx);
        goto fail;
    }
    prop = js_symbol_to_atom(ctx, name);
    p = JS_VALUE_GET_OBJ(obj);
    prs = find_own_property(&pr, p, prop);
    if (!prs) {
        JS_ThrowTypeErrorPrivateNotFound(ctx, prop);
        fail:
        JS_FreeValue(ctx, val);
        return -1;
    }
    set_value(ctx, &pr->u.value, val);
    return 0;
}

static int JS_AddBrand(JSContext *ctx, JSValueConst obj, JSValueConst home_obj)
{
    JSObject *p, *p1;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSValue brand;
    JSAtom brand_atom;

    if (unlikely(JS_VALUE_GET_TAG(home_obj) != JS_TAG_OBJECT)) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    p = JS_VALUE_GET_OBJ(home_obj);
    prs = find_own_property(&pr, p, JS_ATOM_Private_brand);
    if (!prs) {
        brand = JS_NewSymbolFromAtom(ctx, JS_ATOM_brand, JS_ATOM_TYPE_PRIVATE);
        if (JS_IsException(brand))
            return -1;
        /* if the brand is not present, add it */
        pr = add_property(ctx, p, JS_ATOM_Private_brand, JS_PROP_C_W_E);
        if (!pr) {
            JS_FreeValue(ctx, brand);
            return -1;
        }
        pr->u.value = JS_DupValue(ctx, brand);
    } else {
        brand = JS_DupValue(ctx, pr->u.value);
    }
    brand_atom = js_symbol_to_atom(ctx, brand);

    if (unlikely(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        JS_FreeAtom(ctx, brand_atom);
        return -1;
    }
    p1 = JS_VALUE_GET_OBJ(obj);
    pr = add_property(ctx, p1, brand_atom, JS_PROP_C_W_E);
    JS_FreeAtom(ctx, brand_atom);
    if (!pr)
        return -1;
    pr->u.value = JS_UNDEFINED;
    return 0;
}

static int JS_CheckBrand(JSContext *ctx, JSValueConst obj, JSValueConst func)
{
    JSObject *p, *p1, *home_obj;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSValueConst brand;

    /* get the home object of 'func' */
    if (unlikely(JS_VALUE_GET_TAG(func) != JS_TAG_OBJECT)) {
        not_obj:
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    p1 = JS_VALUE_GET_OBJ(func);
    if (!js_class_has_bytecode(p1->class_id))
        goto not_obj;
    home_obj = p1->u.func.home_object;
    if (!home_obj)
        goto not_obj;
    prs = find_own_property(&pr, home_obj, JS_ATOM_Private_brand);
    if (!prs) {
        JS_ThrowTypeError(ctx, "expecting <brand> private field");
        return -1;
    }
    brand = pr->u.value;
    /* safety check */
    if (unlikely(JS_VALUE_GET_TAG(brand) != JS_TAG_SYMBOL))
        goto not_obj;

    /* get the brand array of 'obj' */
    if (unlikely(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT))
        goto not_obj;
    p = JS_VALUE_GET_OBJ(obj);
    prs = find_own_property(&pr, p, js_symbol_to_atom(ctx, brand));
    if (!prs) {
        JS_ThrowTypeError(ctx, "invalid brand on object");
        return -1;
    }
    return 0;
}

static int num_keys_cmp(const void *p1, const void *p2, void *opaque)
{
    JSContext *ctx = opaque;
    JSAtom atom1 = ((const JSPropertyEnum *)p1)->atom;
    JSAtom atom2 = ((const JSPropertyEnum *)p2)->atom;
    uint32_t v1, v2;
    BOOL atom1_is_integer, atom2_is_integer;

    atom1_is_integer = JS_AtomIsArrayIndex(ctx, &v1, atom1);
    atom2_is_integer = JS_AtomIsArrayIndex(ctx, &v2, atom2);
    assert(atom1_is_integer && atom2_is_integer);
    if (v1 < v2)
        return -1;
    else if (v1 == v2)
        return 0;
    else
        return 1;
}

static void js_free_prop_enum(JSContext *ctx, JSPropertyEnum *tab, uint32_t len)
{
    uint32_t i;
    if (tab) {
        for(i = 0; i < len; i++)
            JS_FreeAtom(ctx, tab[i].atom);
        js_free(ctx, tab);
    }
}

/* return < 0 in case if exception, 0 if OK. ptab and its atoms must
   be freed by the user. */
static int __exception JS_GetOwnPropertyNamesInternal(JSContext *ctx,
JSPropertyEnum **ptab,
        uint32_t *plen,
JSObject *p, int flags)
{
int i, j;
JSShape *sh;
JSShapeProperty *prs;
JSPropertyEnum *tab_atom, *tab_exotic;
JSAtom atom;
uint32_t num_keys_count, str_keys_count, sym_keys_count, atom_count;
uint32_t num_index, str_index, sym_index, exotic_count;
BOOL is_enumerable, num_sorted;
uint32_t num_key;
JSAtomKindEnum kind;

/* clear pointer for consistency in case of failure */
*ptab = NULL;
*plen = 0;

/* compute the number of returned properties */
num_keys_count = 0;
str_keys_count = 0;
sym_keys_count = 0;
exotic_count = 0;
tab_exotic = NULL;
sh = p->shape;
for(i = 0, prs = get_shape_prop(sh); i < sh->prop_count; i++, prs++) {
atom = prs->atom;
if (atom != JS_ATOM_NULL) {
is_enumerable = ((prs->flags & JS_PROP_ENUMERABLE) != 0);
kind = JS_AtomGetKind(ctx, atom);
if ((!(flags & JS_GPN_ENUM_ONLY) || is_enumerable) &&
((flags >> kind) & 1) != 0) {
/* need to raise an exception in case of the module
   name space (implicit GetOwnProperty) */
if (unlikely((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) &&
(flags & (JS_GPN_SET_ENUM | JS_GPN_ENUM_ONLY))) {
JSVarRef *var_ref = p->prop[i].u.var_ref;
if (unlikely(JS_IsUninitialized(*var_ref->pvalue))) {
JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
return -1;
}
}
if (JS_AtomIsArrayIndex(ctx, &num_key, atom)) {
num_keys_count++;
} else if (kind == JS_ATOM_KIND_STRING) {
str_keys_count++;
} else {
sym_keys_count++;
}
}
}
}

if (p->is_exotic) {
if (p->fast_array) {
/* the implicit GetOwnProperty raises an exception if the
   typed array is detached */
if ((flags & (JS_GPN_SET_ENUM | JS_GPN_ENUM_ONLY)) &&
(p->class_id >= JS_CLASS_UINT8C_ARRAY &&
p->class_id <= JS_CLASS_FLOAT64_ARRAY) &&
typed_array_is_detached(ctx, p) &&
typed_array_get_length(ctx, p) != 0) {
JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
return -1;
}
if (flags & JS_GPN_STRING_MASK) {
num_keys_count += p->u.array.count;
}
} else {
const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
if (em && em->get_own_property_names) {
if (em->get_own_property_names(ctx, &tab_exotic, &exotic_count,
JS_MKPTR(JS_TAG_OBJECT, p)))
return -1;
for(i = 0; i < exotic_count; i++) {
atom = tab_exotic[i].atom;
kind = JS_AtomGetKind(ctx, atom);
if (((flags >> kind) & 1) != 0) {
is_enumerable = FALSE;
if (flags & (JS_GPN_SET_ENUM | JS_GPN_ENUM_ONLY)) {
JSPropertyDescriptor desc;
int res;
/* set the "is_enumerable" field if necessary */
res = JS_GetOwnPropertyInternal(ctx, &desc, p, atom);
if (res < 0) {
js_free_prop_enum(ctx, tab_exotic, exotic_count);
return -1;
}
if (res) {
is_enumerable =
((desc.flags & JS_PROP_ENUMERABLE) != 0);
js_free_desc(ctx, &desc);
}
tab_exotic[i].is_enumerable = is_enumerable;
}
if (!(flags & JS_GPN_ENUM_ONLY) || is_enumerable) {
if (JS_AtomIsArrayIndex(ctx, &num_key, atom)) {
num_keys_count++;
} else if (kind == JS_ATOM_KIND_STRING) {
str_keys_count++;
} else {
sym_keys_count++;
}
}
}
}
}
}
}

/* fill them */

atom_count = num_keys_count + str_keys_count + sym_keys_count;
/* avoid allocating 0 bytes */
tab_atom = js_malloc(ctx, sizeof(tab_atom[0]) * max_int(atom_count, 1));
if (!tab_atom) {
js_free_prop_enum(ctx, tab_exotic, exotic_count);
return -1;
}

num_index = 0;
str_index = num_keys_count;
sym_index = str_index + str_keys_count;

num_sorted = TRUE;
sh = p->shape;
for(i = 0, prs = get_shape_prop(sh); i < sh->prop_count; i++, prs++) {
atom = prs->atom;
if (atom != JS_ATOM_NULL) {
is_enumerable = ((prs->flags & JS_PROP_ENUMERABLE) != 0);
kind = JS_AtomGetKind(ctx, atom);
if ((!(flags & JS_GPN_ENUM_ONLY) || is_enumerable) &&
((flags >> kind) & 1) != 0) {
if (JS_AtomIsArrayIndex(ctx, &num_key, atom)) {
j = num_index++;
num_sorted = FALSE;
} else if (kind == JS_ATOM_KIND_STRING) {
j = str_index++;
} else {
j = sym_index++;
}
tab_atom[j].atom = JS_DupAtom(ctx, atom);
tab_atom[j].is_enumerable = is_enumerable;
}
}
}

if (p->is_exotic) {
if (p->fast_array) {
if (flags & JS_GPN_STRING_MASK) {
for(i = 0; i < p->u.array.count; i++) {
tab_atom[num_index].atom = __JS_AtomFromUInt32(i);
if (tab_atom[num_index].atom == JS_ATOM_NULL) {
js_free_prop_enum(ctx, tab_exotic, exotic_count);
js_free_prop_enum(ctx, tab_atom, num_index);
return -1;
}
tab_atom[num_index].is_enumerable = TRUE;
num_index++;
}
}
}
if (exotic_count > 0) {
for(i = 0; i < exotic_count; i++) {
atom = tab_exotic[i].atom;
is_enumerable = tab_exotic[i].is_enumerable;
kind = JS_AtomGetKind(ctx, atom);
if ((!(flags & JS_GPN_ENUM_ONLY) || is_enumerable) &&
((flags >> kind) & 1) != 0) {
if (JS_AtomIsArrayIndex(ctx, &num_key, atom)) {
j = num_index++;
num_sorted = FALSE;
} else if (kind == JS_ATOM_KIND_STRING) {
j = str_index++;
} else {
j = sym_index++;
}
tab_atom[j].atom = atom;
tab_atom[j].is_enumerable = is_enumerable;
} else {
JS_FreeAtom(ctx, atom);
}
}
}
js_free(ctx, tab_exotic);
}

assert(num_index == num_keys_count);
assert(str_index == num_keys_count + str_keys_count);
assert(sym_index == atom_count);

if (num_keys_count != 0 && !num_sorted) {
rqsort(tab_atom, num_keys_count, sizeof(tab_atom[0]), num_keys_cmp,
ctx);
}
*ptab = tab_atom;
*plen = atom_count;
return 0;
}

int JS_GetOwnPropertyNames(JSContext *ctx, JSPropertyEnum **ptab,
                           uint32_t *plen, JSValueConst obj, int flags)
{
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    return JS_GetOwnPropertyNamesInternal(ctx, ptab, plen,
                                          JS_VALUE_GET_OBJ(obj), flags);
}

/* Return -1 if exception,
   FALSE if the property does not exist, TRUE if it exists. If TRUE is
   returned, the property descriptor 'desc' is filled present. */
static int JS_GetOwnPropertyInternal(JSContext *ctx, JSPropertyDescriptor *desc,
                                     JSObject *p, JSAtom prop)
{
    JSShapeProperty *prs;
    JSProperty *pr;

    retry:
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        if (desc) {
            desc->flags = prs->flags & JS_PROP_C_W_E;
            desc->getter = JS_UNDEFINED;
            desc->setter = JS_UNDEFINED;
            desc->value = JS_UNDEFINED;
            if (unlikely(prs->flags & JS_PROP_TMASK)) {
                if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                    desc->flags |= JS_PROP_GETSET;
                    if (pr->u.getset.getter)
                        desc->getter = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.getter));
                    if (pr->u.getset.setter)
                        desc->setter = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.setter));
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                    JSValue val = *pr->u.var_ref->pvalue;
                    if (unlikely(JS_IsUninitialized(val))) {
                        JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
                        return -1;
                    }
                    desc->value = JS_DupValue(ctx, val);
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                    /* Instantiate property and retry */
                    if (JS_AutoInitProperty(ctx, p, prop, pr))
                        return -1;
                    goto retry;
                }
            } else {
                desc->value = JS_DupValue(ctx, pr->u.value);
            }
        } else {
            /* for consistency, send the exception even if desc is NULL */
            if (unlikely((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF)) {
                if (unlikely(JS_IsUninitialized(*pr->u.var_ref->pvalue))) {
                    JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
                    return -1;
                }
            } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                /* nothing to do: delay instantiation until actual value and/or attributes are read */
            }
        }
        return TRUE;
    }
    if (p->is_exotic) {
        if (p->fast_array) {
            /* specific case for fast arrays */
            if (__JS_AtomIsTaggedInt(prop)) {
                uint32_t idx;
                idx = __JS_AtomToUInt32(prop);
                if (idx < p->u.array.count) {
                    if (desc) {
                        desc->flags = JS_PROP_WRITABLE | JS_PROP_ENUMERABLE;
                        if (p->class_id == JS_CLASS_ARRAY ||
                            p->class_id == JS_CLASS_ARGUMENTS)
                            desc->flags |= JS_PROP_CONFIGURABLE;
                        desc->getter = JS_UNDEFINED;
                        desc->setter = JS_UNDEFINED;
                        desc->value = JS_GetPropertyUint32(ctx, JS_MKPTR(JS_TAG_OBJECT, p), idx);
                    }
                    return TRUE;
                }
            }
            if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                int ret;
                ret = JS_AtomIsNumericIndex(ctx, prop);
                if (ret != 0) {
                    if (ret < 0)
                        return -1;
                    if (typed_array_is_detached(ctx, p)) {
                        JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
                        return -1;
                    }
                }
            }
        } else {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em && em->get_own_property) {
                return em->get_own_property(ctx, desc,
                                            JS_MKPTR(JS_TAG_OBJECT, p), prop);
            }
        }
    }
    return FALSE;
}

int JS_GetOwnProperty(JSContext *ctx, JSPropertyDescriptor *desc,
                      JSValueConst obj, JSAtom prop)
{
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    return JS_GetOwnPropertyInternal(ctx, desc, JS_VALUE_GET_OBJ(obj), prop);
}

/* return -1 if exception (Proxy object only) or TRUE/FALSE */
int JS_IsExtensible(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;

    if (unlikely(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT))
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    if (unlikely(p->class_id == JS_CLASS_PROXY))
        return js_proxy_isExtensible(ctx, obj);
    else
        return p->extensible;
}

/* return -1 if exception (Proxy object only) or TRUE/FALSE */
int JS_PreventExtensions(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;

    if (unlikely(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT))
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    if (unlikely(p->class_id == JS_CLASS_PROXY))
        return js_proxy_preventExtensions(ctx, obj);
    p->extensible = FALSE;
    return TRUE;
}

/* return -1 if exception otherwise TRUE or FALSE */
int JS_HasProperty(JSContext *ctx, JSValueConst obj, JSAtom prop)
{
    JSObject *p;
    int ret;
    JSValue obj1;

    if (unlikely(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT))
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    for(;;) {
        if (p->is_exotic) {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em && em->has_property) {
                /* has_property can free the prototype */
                obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
                ret = em->has_property(ctx, obj1, prop);
                JS_FreeValue(ctx, obj1);
                return ret;
            }
        }
        /* JS_GetOwnPropertyInternal can free the prototype */
        JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
        ret = JS_GetOwnPropertyInternal(ctx, NULL, p, prop);
        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
        if (ret != 0)
            return ret;
        if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
            p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
            ret = JS_AtomIsNumericIndex(ctx, prop);
            if (ret != 0) {
                if (ret < 0)
                    return -1;
                /* the detached array test was done in
                   JS_GetOwnPropertyInternal() */
                return FALSE;
            }
        }
        p = p->shape->proto;
        if (!p)
            break;
    }
    return FALSE;
}

/* val must be a symbol */
static JSAtom js_symbol_to_atom(JSContext *ctx, JSValue val)
{
    JSAtomStruct *p = JS_VALUE_GET_PTR(val);
    return js_get_atom_index(ctx->rt, p);
}

/* return JS_ATOM_NULL in case of exception */
JSAtom JS_ValueToAtom(JSContext *ctx, JSValueConst val)
{
    JSAtom atom;
    uint32_t tag;
    tag = JS_VALUE_GET_TAG(val);
    if (tag == JS_TAG_INT &&
        (uint32_t)JS_VALUE_GET_INT(val) <= JS_ATOM_MAX_INT) {
        /* fast path for integer values */
        atom = __JS_AtomFromUInt32(JS_VALUE_GET_INT(val));
    } else if (tag == JS_TAG_SYMBOL) {
        JSAtomStruct *p = JS_VALUE_GET_PTR(val);
        atom = JS_DupAtom(ctx, js_get_atom_index(ctx->rt, p));
    } else {
        JSValue str;
        str = JS_ToPropertyKey(ctx, val);
        if (JS_IsException(str))
            return JS_ATOM_NULL;
        if (JS_VALUE_GET_TAG(str) == JS_TAG_SYMBOL) {
            atom = js_symbol_to_atom(ctx, str);
        } else {
            atom = JS_NewAtomStr(ctx, JS_VALUE_GET_STRING(str));
        }
    }
    return atom;
}

static JSValue JS_GetPropertyValue(JSContext *ctx, JSValueConst this_obj,
                                   JSValue prop)
{
    JSAtom atom;
    JSValue ret;

    if (likely(JS_VALUE_GET_TAG(this_obj) == JS_TAG_OBJECT &&
               JS_VALUE_GET_TAG(prop) == JS_TAG_INT)) {
        JSObject *p;
        uint32_t idx, len;
        /* fast path for array access */
        p = JS_VALUE_GET_OBJ(this_obj);
        idx = JS_VALUE_GET_INT(prop);
        len = (uint32_t)p->u.array.count;
        if (unlikely(idx >= len))
            goto slow_path;
        switch(p->class_id) {
            case JS_CLASS_ARRAY:
            case JS_CLASS_ARGUMENTS:
                return JS_DupValue(ctx, p->u.array.u.values[idx]);
            case JS_CLASS_INT8_ARRAY:
                return JS_NewInt32(ctx, p->u.array.u.int8_ptr[idx]);
            case JS_CLASS_UINT8C_ARRAY:
            case JS_CLASS_UINT8_ARRAY:
                return JS_NewInt32(ctx, p->u.array.u.uint8_ptr[idx]);
            case JS_CLASS_INT16_ARRAY:
                return JS_NewInt32(ctx, p->u.array.u.int16_ptr[idx]);
            case JS_CLASS_UINT16_ARRAY:
                return JS_NewInt32(ctx, p->u.array.u.uint16_ptr[idx]);
            case JS_CLASS_INT32_ARRAY:
                return JS_NewInt32(ctx, p->u.array.u.int32_ptr[idx]);
            case JS_CLASS_UINT32_ARRAY:
                return JS_NewUint32(ctx, p->u.array.u.uint32_ptr[idx]);
#ifdef CONFIG_BIGNUM
                case JS_CLASS_BIG_INT64_ARRAY:
            return JS_NewBigInt64(ctx, p->u.array.u.int64_ptr[idx]);
        case JS_CLASS_BIG_UINT64_ARRAY:
            return JS_NewBigUint64(ctx, p->u.array.u.uint64_ptr[idx]);
#endif
            case JS_CLASS_FLOAT32_ARRAY:
                return __JS_NewFloat64(ctx, p->u.array.u.float_ptr[idx]);
            case JS_CLASS_FLOAT64_ARRAY:
                return __JS_NewFloat64(ctx, p->u.array.u.double_ptr[idx]);
            default:
                goto slow_path;
        }
    } else {
        slow_path:
        atom = JS_ValueToAtom(ctx, prop);
        JS_FreeValue(ctx, prop);
        if (unlikely(atom == JS_ATOM_NULL))
            return JS_EXCEPTION;
        ret = JS_GetProperty(ctx, this_obj, atom);
        JS_FreeAtom(ctx, atom);
        return ret;
    }
}

JSValue JS_GetPropertyUint32(JSContext *ctx, JSValueConst this_obj,
                             uint32_t idx)
{
    return JS_GetPropertyValue(ctx, this_obj, JS_NewUint32(ctx, idx));
}

/* Check if an object has a generalized numeric property. Return value:
   -1 for exception,
   TRUE if property exists, stored into *pval,
   FALSE if proprty does not exist.
 */
static int JS_TryGetPropertyInt64(JSContext *ctx, JSValueConst obj, int64_t idx, JSValue *pval)
{
    JSValue val = JS_UNDEFINED;
    JSAtom prop;
    int present;

    if (likely((uint64_t)idx <= JS_ATOM_MAX_INT)) {
        /* fast path */
        present = JS_HasProperty(ctx, obj, __JS_AtomFromUInt32(idx));
        if (present > 0) {
            val = JS_GetPropertyValue(ctx, obj, JS_NewInt32(ctx, idx));
            if (unlikely(JS_IsException(val)))
                present = -1;
        }
    } else {
        prop = JS_NewAtomInt64(ctx, idx);
        present = -1;
        if (likely(prop != JS_ATOM_NULL)) {
            present = JS_HasProperty(ctx, obj, prop);
            if (present > 0) {
                val = JS_GetProperty(ctx, obj, prop);
                if (unlikely(JS_IsException(val)))
                    present = -1;
            }
            JS_FreeAtom(ctx, prop);
        }
    }
    *pval = val;
    return present;
}

static JSValue JS_GetPropertyInt64(JSContext *ctx, JSValueConst obj, int64_t idx)
{
    JSAtom prop;
    JSValue val;

    if ((uint64_t)idx <= INT32_MAX) {
        /* fast path for fast arrays */
        return JS_GetPropertyValue(ctx, obj, JS_NewInt32(ctx, idx));
    }
    prop = JS_NewAtomInt64(ctx, idx);
    if (prop == JS_ATOM_NULL)
        return JS_EXCEPTION;

    val = JS_GetProperty(ctx, obj, prop);
    JS_FreeAtom(ctx, prop);
    return val;
}

JSValue JS_GetPropertyStr(JSContext *ctx, JSValueConst this_obj,
                          const char *prop)
{
    JSAtom atom;
    JSValue ret;
    atom = JS_NewAtom(ctx, prop);
    ret = JS_GetProperty(ctx, this_obj, atom);
    JS_FreeAtom(ctx, atom);
    return ret;
}

/* Note: the property value is not initialized. Return NULL if memory
   error. */
static JSProperty *add_property(JSContext *ctx,
                                JSObject *p, JSAtom prop, int prop_flags)
{
    JSShape *sh, *new_sh;

    sh = p->shape;
    if (sh->is_hashed) {
        /* try to find an existing shape */
        new_sh = find_hashed_shape_prop(ctx->rt, sh, prop, prop_flags);
        if (new_sh) {
            /* matching shape found: use it */
            /*  the property array may need to be resized */
            if (new_sh->prop_size != sh->prop_size) {
                JSProperty *new_prop;
                new_prop = js_realloc(ctx, p->prop, sizeof(p->prop[0]) *
                                                    new_sh->prop_size);
                if (!new_prop)
                    return NULL;
                p->prop = new_prop;
            }
            p->shape = js_dup_shape(new_sh);
            js_free_shape(ctx->rt, sh);
            return &p->prop[new_sh->prop_count - 1];
        } else if (sh->header.ref_count != 1) {
            /* if the shape is shared, clone it */
            new_sh = js_clone_shape(ctx, sh);
            if (!new_sh)
                return NULL;
            /* hash the cloned shape */
            new_sh->is_hashed = TRUE;
            js_shape_hash_link(ctx->rt, new_sh);
            js_free_shape(ctx->rt, p->shape);
            p->shape = new_sh;
        }
    }
    assert(p->shape->header.ref_count == 1);
    if (add_shape_property(ctx, &p->shape, p, prop, prop_flags))
        return NULL;
    return &p->prop[p->shape->prop_count - 1];
}

/* can be called on Array or Arguments objects. return < 0 if
   memory alloc error. */
static no_inline __exception int convert_fast_array_to_array(JSContext *ctx,
                                                             JSObject *p)
{
    JSProperty *pr;
    JSShape *sh;
    JSValue *tab;
    uint32_t i, len, new_count;

    if (js_shape_prepare_update(ctx, p, NULL))
        return -1;
    len = p->u.array.count;
    /* resize the properties once to simplify the error handling */
    sh = p->shape;
    new_count = sh->prop_count + len;
    if (new_count > sh->prop_size) {
        if (resize_properties(ctx, &p->shape, p, new_count))
            return -1;
    }

    tab = p->u.array.u.values;
    for(i = 0; i < len; i++) {
        /* add_property cannot fail here but
           __JS_AtomFromUInt32(i) fails for i > INT32_MAX */
        pr = add_property(ctx, p, __JS_AtomFromUInt32(i), JS_PROP_C_W_E);
        pr->u.value = *tab++;
    }
    js_free(ctx, p->u.array.u.values);
    p->u.array.count = 0;
    p->u.array.u.values = NULL; /* fail safe */
    p->u.array.u1.size = 0;
    p->fast_array = 0;
    return 0;
}

static int delete_property(JSContext *ctx, JSObject *p, JSAtom atom)
{
    JSShape *sh;
    JSShapeProperty *pr, *lpr, *prop;
    JSProperty *pr1;
    uint32_t lpr_idx;
    intptr_t h, h1;

    redo:
    sh = p->shape;
    h1 = atom & sh->prop_hash_mask;
    h = *JSSHAPE_HASH_OFFSET(sh, -h1 - 1);
    prop = get_shape_prop(sh);
    lpr = NULL;
    lpr_idx = 0;   /* prevent warning */
    while (h != 0) {
        pr = &prop[h - 1];
        if (likely(pr->atom == atom)) {
            /* found ! */
            if (!(pr->flags & JS_PROP_CONFIGURABLE))
                return FALSE;
            /* realloc the shape if needed */
            if (lpr)
                lpr_idx = lpr - get_shape_prop(sh);
            if (js_shape_prepare_update(ctx, p, &pr))
                return -1;
            sh = p->shape;
            /* remove property */
            if (lpr) {
                lpr = get_shape_prop(sh) + lpr_idx;
                lpr->hash_next = pr->hash_next;
            } else {
                *JSSHAPE_HASH_OFFSET(sh, -h1 - 1) = pr->hash_next;
            }
            sh->deleted_prop_count++;
            /* free the entry */
            pr1 = &p->prop[h - 1];
            free_property(ctx->rt, pr1, pr->flags);
            JS_FreeAtom(ctx, pr->atom);
            /* put default values */
            pr->flags = 0;
            pr->atom = JS_ATOM_NULL;
            pr1->u.value = JS_UNDEFINED;

            /* compact the properties if too many deleted properties */
            if (sh->deleted_prop_count >= 8 &&
                sh->deleted_prop_count >= ((unsigned)sh->prop_count / 2)) {
                compact_properties(ctx, p);
            }
            return TRUE;
        }
        lpr = pr;
        h = pr->hash_next;
    }

    if (p->is_exotic) {
        if (p->fast_array) {
            uint32_t idx;
            if (JS_AtomIsArrayIndex(ctx, &idx, atom) &&
                idx < p->u.array.count) {
                if (p->class_id == JS_CLASS_ARRAY ||
                    p->class_id == JS_CLASS_ARGUMENTS) {
                    /* Special case deleting the last element of a fast Array */
                    if (idx == p->u.array.count - 1) {
                        JS_FreeValue(ctx, p->u.array.u.values[idx]);
                        p->u.array.count = idx;
                        return TRUE;
                    }
                    if (convert_fast_array_to_array(ctx, p))
                        return -1;
                    goto redo;
                } else {
                    return FALSE; /* not configurable */
                }
            }
        } else {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em && em->delete_property) {
                return em->delete_property(ctx, JS_MKPTR(JS_TAG_OBJECT, p), atom);
            }
        }
    }
    /* not found */
    return TRUE;
}

static int call_setter(JSContext *ctx, JSObject *setter,
                       JSValueConst this_obj, JSValue val, int flags)
{
    JSValue ret, func;
    if (likely(setter)) {
        func = JS_MKPTR(JS_TAG_OBJECT, setter);
        /* Note: the field could be removed in the setter */
        func = JS_DupValue(ctx, func);
        ret = JS_CallFree(ctx, func, this_obj, 1, (JSValueConst *)&val);
        JS_FreeValue(ctx, val);
        if (JS_IsException(ret))
            return -1;
        JS_FreeValue(ctx, ret);
        return TRUE;
    } else {
        JS_FreeValue(ctx, val);
        if ((flags & JS_PROP_THROW) ||
            ((flags & JS_PROP_THROW_STRICT) && is_strict_mode(ctx))) {
            JS_ThrowTypeError(ctx, "no setter for property");
            return -1;
        }
        return FALSE;
    }
}

/* set the array length and remove the array elements if necessary. */
static int set_array_length(JSContext *ctx, JSObject *p, JSProperty *prop,
                            JSValue val, int flags)
{
    uint32_t len, idx, cur_len;
    int i, ret;

    ret = JS_ToArrayLengthFree(ctx, &len, val);
    if (ret)
        return -1;
    if (likely(p->fast_array)) {
        uint32_t old_len = p->u.array.count;
        if (len < old_len) {
            for(i = len; i < old_len; i++) {
                JS_FreeValue(ctx, p->u.array.u.values[i]);
            }
            p->u.array.count = len;
        }
        prop->u.value = JS_NewUint32(ctx, len);
    } else {
        /* Note: length is always a uint32 because the object is an
           array */
        JS_ToUint32(ctx, &cur_len, prop->u.value);
        if (len < cur_len) {
            uint32_t d;
            JSShape *sh;
            JSShapeProperty *pr;

            d = cur_len - len;
            sh = p->shape;
            if (d <= sh->prop_count) {
                JSAtom atom;

                /* faster to iterate */
                while (cur_len > len) {
                    atom = JS_NewAtomUInt32(ctx, cur_len - 1);
                    ret = delete_property(ctx, p, atom);
                    JS_FreeAtom(ctx, atom);
                    if (unlikely(!ret)) {
                        /* unlikely case: property is not
                           configurable */
                        break;
                    }
                    cur_len--;
                }
            } else {
                /* faster to iterate thru all the properties. Need two
                   passes in case one of the property is not
                   configurable */
                cur_len = len;
                for(i = 0, pr = get_shape_prop(sh); i < sh->prop_count;
                    i++, pr++) {
                    if (pr->atom != JS_ATOM_NULL &&
                        JS_AtomIsArrayIndex(ctx, &idx, pr->atom)) {
                        if (idx >= cur_len &&
                            !(pr->flags & JS_PROP_CONFIGURABLE)) {
                            cur_len = idx + 1;
                        }
                    }
                }

                for(i = 0, pr = get_shape_prop(sh); i < sh->prop_count;
                    i++, pr++) {
                    if (pr->atom != JS_ATOM_NULL &&
                        JS_AtomIsArrayIndex(ctx, &idx, pr->atom)) {
                        if (idx >= cur_len) {
                            /* remove the property */
                            delete_property(ctx, p, pr->atom);
                            /* WARNING: the shape may have been modified */
                            sh = p->shape;
                            pr = get_shape_prop(sh) + i;
                        }
                    }
                }
            }
        } else {
            cur_len = len;
        }
        set_value(ctx, &p->prop[0].u.value, JS_NewUint32(ctx, cur_len));
        if (unlikely(cur_len > len)) {
            return JS_ThrowTypeErrorOrFalse(ctx, flags, "not configurable");
        }
    }
    return TRUE;
}

/* Preconditions: 'p' must be of class JS_CLASS_ARRAY, p->fast_array =
   TRUE and p->extensible = TRUE */
static int add_fast_array_element(JSContext *ctx, JSObject *p,
                                  JSValue val, int flags)
{
    uint32_t new_len, array_len;
    /* extend the array by one */
    /* XXX: convert to slow array if new_len > 2^31-1 elements */
    new_len = p->u.array.count + 1;
    /* update the length if necessary. We assume that if the length is
       not an integer, then if it >= 2^31.  */
    if (likely(JS_VALUE_GET_TAG(p->prop[0].u.value) == JS_TAG_INT)) {
        array_len = JS_VALUE_GET_INT(p->prop[0].u.value);
        if (new_len > array_len) {
            if (unlikely(!(get_shape_prop(p->shape)->flags & JS_PROP_WRITABLE))) {
                JS_FreeValue(ctx, val);
                return JS_ThrowTypeErrorReadOnly(ctx, flags, JS_ATOM_length);
            }
            p->prop[0].u.value = JS_NewInt32(ctx, new_len);
        }
    }
    if (unlikely(new_len > p->u.array.u1.size)) {
        uint32_t new_size;
        size_t slack;
        JSValue *new_array_prop;
        /* XXX: potential arithmetic overflow */
        new_size = max_int(new_len, p->u.array.u1.size * 3 / 2);
        new_array_prop = js_realloc2(ctx, p->u.array.u.values, sizeof(JSValue) * new_size, &slack);
        if (!new_array_prop) {
            JS_FreeValue(ctx, val);
            return -1;
        }
        new_size += slack / sizeof(*new_array_prop);
        p->u.array.u.values = new_array_prop;
        p->u.array.u1.size = new_size;
    }
    p->u.array.u.values[new_len - 1] = val;
    p->u.array.count = new_len;
    return TRUE;
}

static void js_free_desc(JSContext *ctx, JSPropertyDescriptor *desc)
{
    JS_FreeValue(ctx, desc->getter);
    JS_FreeValue(ctx, desc->setter);
    JS_FreeValue(ctx, desc->value);
}

/* generic (and slower) version of JS_SetProperty() for Reflect.set() */
static int JS_SetPropertyGeneric(JSContext *ctx,
                                 JSObject *p, JSAtom prop,
                                 JSValue val, JSValueConst this_obj,
                                 int flags)
{
    int ret;
    JSPropertyDescriptor desc;

    while (p != NULL) {
        if (p->is_exotic) {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em && em->set_property) {
                JSValue obj1;
                /* set_property can free the prototype */
                obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
                ret = em->set_property(ctx, obj1, prop,
                                       val, this_obj, flags);
                JS_FreeValue(ctx, obj1);
                JS_FreeValue(ctx, val);
                return ret;
            }
        }

        ret = JS_GetOwnPropertyInternal(ctx, &desc, p, prop);
        if (ret < 0)
            return ret;
        if (ret) {
            if (desc.flags & JS_PROP_GETSET) {
                JSObject *setter;
                if (JS_IsUndefined(desc.setter))
                    setter = NULL;
                else
                    setter = JS_VALUE_GET_OBJ(desc.setter);
                ret = call_setter(ctx, setter, this_obj, val, flags);
                JS_FreeValue(ctx, desc.getter);
                JS_FreeValue(ctx, desc.setter);
                return ret;
            } else {
                JS_FreeValue(ctx, desc.value);
                if (!(desc.flags & JS_PROP_WRITABLE)) {
                    goto read_only_error;
                }
            }
            break;
        }
        p = p->shape->proto;
    }

    if (!JS_IsObject(this_obj))
        return JS_ThrowTypeErrorOrFalse(ctx, flags, "receiver is not an object");

    p = JS_VALUE_GET_OBJ(this_obj);

    /* modify the property in this_obj if it already exists */
    ret = JS_GetOwnPropertyInternal(ctx, &desc, p, prop);
    if (ret < 0)
        return ret;
    if (ret) {
        if (desc.flags & JS_PROP_GETSET) {
            JS_FreeValue(ctx, desc.getter);
            JS_FreeValue(ctx, desc.setter);
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeErrorOrFalse(ctx, flags, "setter is forbidden");
        } else {
            JS_FreeValue(ctx, desc.value);
            if (!(desc.flags & JS_PROP_WRITABLE) ||
                p->class_id == JS_CLASS_MODULE_NS) {
                read_only_error:
                JS_FreeValue(ctx, val);
                return JS_ThrowTypeErrorReadOnly(ctx, flags, prop);
            }
        }
        ret = JS_DefineProperty(ctx, this_obj, prop, val,
                                JS_UNDEFINED, JS_UNDEFINED,
                                JS_PROP_HAS_VALUE);
        JS_FreeValue(ctx, val);
        return ret;
    }

    ret = JS_CreateProperty(ctx, p, prop, val, JS_UNDEFINED, JS_UNDEFINED,
                            flags |
                            JS_PROP_HAS_VALUE |
                            JS_PROP_HAS_ENUMERABLE |
                            JS_PROP_HAS_WRITABLE |
                            JS_PROP_HAS_CONFIGURABLE |
                            JS_PROP_C_W_E);
    JS_FreeValue(ctx, val);
    return ret;
}

/* return -1 in case of exception or TRUE or FALSE. Warning: 'val' is
   freed by the function. 'flags' is a bitmask of JS_PROP_NO_ADD,
   JS_PROP_THROW or JS_PROP_THROW_STRICT. If JS_PROP_NO_ADD is set,
   the new property is not added and an error is raised. */
int JS_SetPropertyInternal(JSContext *ctx, JSValueConst this_obj,
                           JSAtom prop, JSValue val, int flags)
{
    JSObject *p, *p1;
    JSShapeProperty *prs;
    JSProperty *pr;
    uint32_t tag;
    JSPropertyDescriptor desc;
    int ret;
#if 0
    printf("JS_SetPropertyInternal: "); print_atom(ctx, prop); printf("\n");
#endif
    tag = JS_VALUE_GET_TAG(this_obj);
    if (unlikely(tag != JS_TAG_OBJECT)) {
        switch(tag) {
            case JS_TAG_NULL:
                JS_FreeValue(ctx, val);
                JS_ThrowTypeErrorAtom(ctx, "cannot set property '%s' of null", prop);
                return -1;
            case JS_TAG_UNDEFINED:
                JS_FreeValue(ctx, val);
                JS_ThrowTypeErrorAtom(ctx, "cannot set property '%s' of undefined", prop);
                return -1;
            default:
                /* even on a primitive type we can have setters on the prototype */
                p = NULL;
                p1 = JS_VALUE_GET_OBJ(JS_GetPrototypePrimitive(ctx, this_obj));
                goto prototype_lookup;
        }
    }
    p = JS_VALUE_GET_OBJ(this_obj);
    retry:
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        if (likely((prs->flags & (JS_PROP_TMASK | JS_PROP_WRITABLE |
                                  JS_PROP_LENGTH)) == JS_PROP_WRITABLE)) {
            /* fast case */
            set_value(ctx, &pr->u.value, val);
            return TRUE;
        } else if ((prs->flags & (JS_PROP_LENGTH | JS_PROP_WRITABLE)) ==
                   (JS_PROP_LENGTH | JS_PROP_WRITABLE)) {
            assert(p->class_id == JS_CLASS_ARRAY);
            assert(prop == JS_ATOM_length);
            return set_array_length(ctx, p, pr, val, flags);
        } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
            return call_setter(ctx, pr->u.getset.setter, this_obj, val, flags);
        } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
            /* JS_PROP_WRITABLE is always true for variable
               references, but they are write protected in module name
               spaces. */
            if (p->class_id == JS_CLASS_MODULE_NS)
                goto read_only_prop;
            set_value(ctx, pr->u.var_ref->pvalue, val);
            return TRUE;
        } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
            /* Instantiate property and retry (potentially useless) */
            if (JS_AutoInitProperty(ctx, p, prop, pr)) {
                JS_FreeValue(ctx, val);
                return -1;
            }
            goto retry;
        } else {
            goto read_only_prop;
        }
    }

    p1 = p;
    for(;;) {
        if (p1->is_exotic) {
            if (p1->fast_array) {
                if (__JS_AtomIsTaggedInt(prop)) {
                    uint32_t idx = __JS_AtomToUInt32(prop);
                    if (idx < p1->u.array.count) {
                        if (unlikely(p == p1))
                            return JS_SetPropertyValue(ctx, this_obj, JS_NewInt32(ctx, idx), val, flags);
                        else
                            break;
                    } else if (p1->class_id >= JS_CLASS_UINT8C_ARRAY &&
                               p1->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                        goto typed_array_oob;
                    }
                } else if (p1->class_id >= JS_CLASS_UINT8C_ARRAY &&
                           p1->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                    ret = JS_AtomIsNumericIndex(ctx, prop);
                    if (ret != 0) {
                        if (ret < 0) {
                            JS_FreeValue(ctx, val);
                            return -1;
                        }
                        typed_array_oob:
                        val = JS_ToNumberFree(ctx, val);
                        JS_FreeValue(ctx, val);
                        if (JS_IsException(val))
                            return -1;
                        if (typed_array_is_detached(ctx, p1)) {
                            JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
                            return -1;
                        }
                        return JS_ThrowTypeErrorOrFalse(ctx, flags, "out-of-bound numeric index");
                    }
                }
            } else {
                const JSClassExoticMethods *em = ctx->rt->class_array[p1->class_id].exotic;
                if (em) {
                    JSValue obj1;
                    if (em->set_property) {
                        /* set_property can free the prototype */
                        obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p1));
                        ret = em->set_property(ctx, obj1, prop,
                                               val, this_obj, flags);
                        JS_FreeValue(ctx, obj1);
                        JS_FreeValue(ctx, val);
                        return ret;
                    }
                    if (em->get_own_property) {
                        /* get_own_property can free the prototype */
                        obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p1));
                        ret = em->get_own_property(ctx, &desc,
                                                   obj1, prop);
                        JS_FreeValue(ctx, obj1);
                        if (ret < 0) {
                            JS_FreeValue(ctx, val);
                            return ret;
                        }
                        if (ret) {
                            if (desc.flags & JS_PROP_GETSET) {
                                JSObject *setter;
                                if (JS_IsUndefined(desc.setter))
                                    setter = NULL;
                                else
                                    setter = JS_VALUE_GET_OBJ(desc.setter);
                                ret = call_setter(ctx, setter, this_obj, val, flags);
                                JS_FreeValue(ctx, desc.getter);
                                JS_FreeValue(ctx, desc.setter);
                                return ret;
                            } else {
                                JS_FreeValue(ctx, desc.value);
                                if (!(desc.flags & JS_PROP_WRITABLE))
                                    goto read_only_prop;
                                if (likely(p == p1)) {
                                    ret = JS_DefineProperty(ctx, this_obj, prop, val,
                                                            JS_UNDEFINED, JS_UNDEFINED,
                                                            JS_PROP_HAS_VALUE);
                                    JS_FreeValue(ctx, val);
                                    return ret;
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        p1 = p1->shape->proto;
        prototype_lookup:
        if (!p1)
            break;

        retry2:
        prs = find_own_property(&pr, p1, prop);
        if (prs) {
            if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                return call_setter(ctx, pr->u.getset.setter, this_obj, val, flags);
            } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                /* Instantiate property and retry (potentially useless) */
                if (JS_AutoInitProperty(ctx, p1, prop, pr))
                    return -1;
                goto retry2;
            } else if (!(prs->flags & JS_PROP_WRITABLE)) {
                read_only_prop:
                JS_FreeValue(ctx, val);
                return JS_ThrowTypeErrorReadOnly(ctx, flags, prop);
            }
        }
    }

    if (unlikely(flags & JS_PROP_NO_ADD)) {
        JS_FreeValue(ctx, val);
        JS_ThrowReferenceErrorNotDefined(ctx, prop);
        return -1;
    }

    if (unlikely(!p)) {
        JS_FreeValue(ctx, val);
        return JS_ThrowTypeErrorOrFalse(ctx, flags, "not an object");
    }

    if (unlikely(!p->extensible)) {
        JS_FreeValue(ctx, val);
        return JS_ThrowTypeErrorOrFalse(ctx, flags, "object is not extensible");
    }

    if (p->is_exotic) {
        if (p->class_id == JS_CLASS_ARRAY && p->fast_array &&
            __JS_AtomIsTaggedInt(prop)) {
            uint32_t idx = __JS_AtomToUInt32(prop);
            if (idx == p->u.array.count) {
                /* fast case */
                return add_fast_array_element(ctx, p, val, flags);
            } else {
                goto generic_create_prop;
            }
        } else {
            generic_create_prop:
            ret = JS_CreateProperty(ctx, p, prop, val, JS_UNDEFINED, JS_UNDEFINED,
                                    flags |
                                    JS_PROP_HAS_VALUE |
                                    JS_PROP_HAS_ENUMERABLE |
                                    JS_PROP_HAS_WRITABLE |
                                    JS_PROP_HAS_CONFIGURABLE |
                                    JS_PROP_C_W_E);
            JS_FreeValue(ctx, val);
            return ret;
        }
    }

    pr = add_property(ctx, p, prop, JS_PROP_C_W_E);
    if (unlikely(!pr)) {
        JS_FreeValue(ctx, val);
        return -1;
    }
    pr->u.value = val;
    return TRUE;
}

/* flags can be JS_PROP_THROW or JS_PROP_THROW_STRICT */
static int JS_SetPropertyValue(JSContext *ctx, JSValueConst this_obj,
                               JSValue prop, JSValue val, int flags)
{
    if (likely(JS_VALUE_GET_TAG(this_obj) == JS_TAG_OBJECT &&
               JS_VALUE_GET_TAG(prop) == JS_TAG_INT)) {
        JSObject *p;
        uint32_t idx;
        double d;
        int32_t v;

        /* fast path for array access */
        p = JS_VALUE_GET_OBJ(this_obj);
        idx = JS_VALUE_GET_INT(prop);
        switch(p->class_id) {
            case JS_CLASS_ARRAY:
                if (unlikely(idx >= (uint32_t)p->u.array.count)) {
            JSObject *p1;
            JSShape *sh1;

            /* fast path to add an element to the array */
            if (idx != (uint32_t)p->u.array.count ||
                                 !p->fast_array || !p->extensible)
            goto slow_path;
            /* check if prototype chain has a numeric property */
            p1 = p->shape->proto;
            while (p1 != NULL) {
                sh1 = p1->shape;
                if (p1->class_id == JS_CLASS_ARRAY) {
                    if (unlikely(!p1->fast_array))
                        goto slow_path;
                } else if (p1->class_id == JS_CLASS_OBJECT) {
                    if (unlikely(sh1->has_small_array_index))
                        goto slow_path;
                } else {
                    goto slow_path;
                }
                p1 = sh1->proto;
            }
            /* add element */
            return add_fast_array_element(ctx, p, val, flags);
        }
                set_value(ctx, &p->u.array.u.values[idx], val);
                break;
            case JS_CLASS_ARGUMENTS:
                if (unlikely(idx >= (uint32_t)p->u.array.count))
                goto slow_path;
                set_value(ctx, &p->u.array.u.values[idx], val);
                break;
            case JS_CLASS_UINT8C_ARRAY:
                if (JS_ToUint8ClampFree(ctx, &v, val))
                    return -1;
                /* Note: the conversion can detach the typed array, so the
                   array bound check must be done after */
                if (unlikely(idx >= (uint32_t)p->u.array.count))
                goto ta_out_of_bound;
                p->u.array.u.uint8_ptr[idx] = v;
                break;
            case JS_CLASS_INT8_ARRAY:
            case JS_CLASS_UINT8_ARRAY:
                if (JS_ToInt32Free(ctx, &v, val))
                    return -1;
                if (unlikely(idx >= (uint32_t)p->u.array.count))
                goto ta_out_of_bound;
                p->u.array.u.uint8_ptr[idx] = v;
                break;
            case JS_CLASS_INT16_ARRAY:
            case JS_CLASS_UINT16_ARRAY:
                if (JS_ToInt32Free(ctx, &v, val))
                    return -1;
                if (unlikely(idx >= (uint32_t)p->u.array.count))
                goto ta_out_of_bound;
                p->u.array.u.uint16_ptr[idx] = v;
                break;
            case JS_CLASS_INT32_ARRAY:
            case JS_CLASS_UINT32_ARRAY:
                if (JS_ToInt32Free(ctx, &v, val))
                    return -1;
                if (unlikely(idx >= (uint32_t)p->u.array.count))
                goto ta_out_of_bound;
                p->u.array.u.uint32_ptr[idx] = v;
                break;
#ifdef CONFIG_BIGNUM
                case JS_CLASS_BIG_INT64_ARRAY:
        case JS_CLASS_BIG_UINT64_ARRAY:
            /* XXX: need specific conversion function */
            {
                int64_t v;
                if (JS_ToBigInt64Free(ctx, &v, val))
                    return -1;
                if (unlikely(idx >= (uint32_t)p->u.array.count))
                    goto ta_out_of_bound;
                p->u.array.u.uint64_ptr[idx] = v;
            }
            break;
#endif
            case JS_CLASS_FLOAT32_ARRAY:
                if (JS_ToFloat64Free(ctx, &d, val))
                    return -1;
                if (unlikely(idx >= (uint32_t)p->u.array.count))
                goto ta_out_of_bound;
                p->u.array.u.float_ptr[idx] = d;
                break;
            case JS_CLASS_FLOAT64_ARRAY:
                if (JS_ToFloat64Free(ctx, &d, val))
                    return -1;
                if (unlikely(idx >= (uint32_t)p->u.array.count)) {
            ta_out_of_bound:
            if (typed_array_is_detached(ctx, p)) {
                JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
                return -1;
            } else {
                return JS_ThrowTypeErrorOrFalse(ctx, flags, "out-of-bound numeric index");
            }
        }
                p->u.array.u.double_ptr[idx] = d;
                break;
            default:
                goto slow_path;
        }
        return TRUE;
    } else {
        JSAtom atom;
        int ret;
        slow_path:
        atom = JS_ValueToAtom(ctx, prop);
        JS_FreeValue(ctx, prop);
        if (unlikely(atom == JS_ATOM_NULL)) {
            JS_FreeValue(ctx, val);
            return -1;
        }
        ret = JS_SetPropertyInternal(ctx, this_obj, atom, val, flags);
        JS_FreeAtom(ctx, atom);
        return ret;
    }
}

int JS_SetPropertyUint32(JSContext *ctx, JSValueConst this_obj,
                         uint32_t idx, JSValue val)
{
    return JS_SetPropertyValue(ctx, this_obj, JS_NewUint32(ctx, idx), val,
                               JS_PROP_THROW);
}

int JS_SetPropertyInt64(JSContext *ctx, JSValueConst this_obj,
                        int64_t idx, JSValue val)
{
    JSAtom prop;
    int res;

    if ((uint64_t)idx <= INT32_MAX) {
        /* fast path for fast arrays */
        return JS_SetPropertyValue(ctx, this_obj, JS_NewInt32(ctx, idx), val,
                                   JS_PROP_THROW);
    }
    prop = JS_NewAtomInt64(ctx, idx);
    if (prop == JS_ATOM_NULL) {
        JS_FreeValue(ctx, val);
        return -1;
    }
    res = JS_SetProperty(ctx, this_obj, prop, val);
    JS_FreeAtom(ctx, prop);
    return res;
}

int JS_SetPropertyStr(JSContext *ctx, JSValueConst this_obj,
                      const char *prop, JSValue val)
{
    JSAtom atom;
    int ret;
    atom = JS_NewAtom(ctx, prop);
    ret = JS_SetPropertyInternal(ctx, this_obj, atom, val, JS_PROP_THROW);
    JS_FreeAtom(ctx, atom);
    return ret;
}

/* compute the property flags. For each flag: (JS_PROP_HAS_x forces
   it, otherwise def_flags is used)
   Note: makes assumption about the bit pattern of the flags
*/
static int get_prop_flags(int flags, int def_flags)
{
    int mask;
    mask = (flags >> JS_PROP_HAS_SHIFT) & JS_PROP_C_W_E;
    return (flags & mask) | (def_flags & ~mask);
}

static int JS_CreateProperty(JSContext *ctx, JSObject *p,
                             JSAtom prop, JSValueConst val,
                             JSValueConst getter, JSValueConst setter,
                             int flags)
{
    JSProperty *pr;
    int ret, prop_flags;

    /* add a new property or modify an existing exotic one */
    if (p->is_exotic) {
        if (p->class_id == JS_CLASS_ARRAY) {
            uint32_t idx, len;

            if (p->fast_array) {
                if (__JS_AtomIsTaggedInt(prop)) {
                    idx = __JS_AtomToUInt32(prop);
                    if (idx == p->u.array.count) {
                        if (!p->extensible)
                            goto not_extensible;
                        if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET))
                            goto convert_to_array;
                        prop_flags = get_prop_flags(flags, 0);
                        if (prop_flags != JS_PROP_C_W_E)
                            goto convert_to_array;
                        return add_fast_array_element(ctx, p,
                                                      JS_DupValue(ctx, val), flags);
                    } else {
                        goto convert_to_array;
                    }
                } else if (JS_AtomIsArrayIndex(ctx, &idx, prop)) {
                    /* convert the fast array to normal array */
                    convert_to_array:
                    if (convert_fast_array_to_array(ctx, p))
                        return -1;
                    goto generic_array;
                }
            } else if (JS_AtomIsArrayIndex(ctx, &idx, prop)) {
                JSProperty *plen;
                JSShapeProperty *pslen;
                generic_array:
                /* update the length field */
                plen = &p->prop[0];
                JS_ToUint32(ctx, &len, plen->u.value);
                if ((idx + 1) > len) {
                    pslen = get_shape_prop(p->shape);
                    if (unlikely(!(pslen->flags & JS_PROP_WRITABLE)))
                        return JS_ThrowTypeErrorReadOnly(ctx, flags, JS_ATOM_length);
                    /* XXX: should update the length after defining
                       the property */
                    len = idx + 1;
                    set_value(ctx, &plen->u.value, JS_NewUint32(ctx, len));
                }
            }
        } else if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                   p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
            ret = JS_AtomIsNumericIndex(ctx, prop);
            if (ret != 0) {
                if (ret < 0)
                    return -1;
                return JS_ThrowTypeErrorOrFalse(ctx, flags, "cannot create numeric index in typed array");
            }
        } else if (!(flags & JS_PROP_NO_EXOTIC)) {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em) {
                if (em->define_own_property) {
                    return em->define_own_property(ctx, JS_MKPTR(JS_TAG_OBJECT, p),
                                                   prop, val, getter, setter, flags);
                }
                ret = JS_IsExtensible(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
                if (ret < 0)
                    return -1;
                if (!ret)
                    goto not_extensible;
            }
        }
    }

    if (!p->extensible) {
        not_extensible:
        return JS_ThrowTypeErrorOrFalse(ctx, flags, "object is not extensible");
    }

    if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
        prop_flags = (flags & (JS_PROP_CONFIGURABLE | JS_PROP_ENUMERABLE)) |
                     JS_PROP_GETSET;
    } else {
        prop_flags = flags & JS_PROP_C_W_E;
    }
    pr = add_property(ctx, p, prop, prop_flags);
    if (unlikely(!pr))
        return -1;
    if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
        pr->u.getset.getter = NULL;
        if ((flags & JS_PROP_HAS_GET) && JS_IsFunction(ctx, getter)) {
            pr->u.getset.getter =
                    JS_VALUE_GET_OBJ(JS_DupValue(ctx, getter));
        }
        pr->u.getset.setter = NULL;
        if ((flags & JS_PROP_HAS_SET) && JS_IsFunction(ctx, setter)) {
            pr->u.getset.setter =
                    JS_VALUE_GET_OBJ(JS_DupValue(ctx, setter));
        }
    } else {
        if (flags & JS_PROP_HAS_VALUE) {
            pr->u.value = JS_DupValue(ctx, val);
        } else {
            pr->u.value = JS_UNDEFINED;
        }
    }
    return TRUE;
}

/* return FALSE if not OK */
static BOOL check_define_prop_flags(int prop_flags, int flags)
{
    BOOL has_accessor, is_getset;

    if (!(prop_flags & JS_PROP_CONFIGURABLE)) {
        if ((flags & (JS_PROP_HAS_CONFIGURABLE | JS_PROP_CONFIGURABLE)) ==
            (JS_PROP_HAS_CONFIGURABLE | JS_PROP_CONFIGURABLE)) {
            return FALSE;
        }
        if ((flags & JS_PROP_HAS_ENUMERABLE) &&
            (flags & JS_PROP_ENUMERABLE) != (prop_flags & JS_PROP_ENUMERABLE))
            return FALSE;
    }
    if (flags & (JS_PROP_HAS_VALUE | JS_PROP_HAS_WRITABLE |
                 JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
        if (!(prop_flags & JS_PROP_CONFIGURABLE)) {
            has_accessor = ((flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET)) != 0);
            is_getset = ((prop_flags & JS_PROP_TMASK) == JS_PROP_GETSET);
            if (has_accessor != is_getset)
                return FALSE;
            if (!has_accessor && !is_getset && !(prop_flags & JS_PROP_WRITABLE)) {
                /* not writable: cannot set the writable bit */
                if ((flags & (JS_PROP_HAS_WRITABLE | JS_PROP_WRITABLE)) ==
                    (JS_PROP_HAS_WRITABLE | JS_PROP_WRITABLE))
                    return FALSE;
            }
        }
    }
    return TRUE;
}

/* ensure that the shape can be safely modified */
static int js_shape_prepare_update(JSContext *ctx, JSObject *p,
                                   JSShapeProperty **pprs)
{
    JSShape *sh;
    uint32_t idx = 0;    /* prevent warning */

    sh = p->shape;
    if (sh->is_hashed) {
        if (sh->header.ref_count != 1) {
            if (pprs)
                idx = *pprs - get_shape_prop(sh);
            /* clone the shape (the resulting one is no longer hashed) */
            sh = js_clone_shape(ctx, sh);
            if (!sh)
                return -1;
            js_free_shape(ctx->rt, p->shape);
            p->shape = sh;
            if (pprs)
                *pprs = get_shape_prop(sh) + idx;
        } else {
            js_shape_hash_unlink(ctx->rt, sh);
            sh->is_hashed = FALSE;
        }
    }
    return 0;
}

static int js_update_property_flags(JSContext *ctx, JSObject *p,
                                    JSShapeProperty **pprs, int flags)
{
    if (flags != (*pprs)->flags) {
        if (js_shape_prepare_update(ctx, p, pprs))
            return -1;
        (*pprs)->flags = flags;
    }
    return 0;
}

/* allowed flags:
   JS_PROP_CONFIGURABLE, JS_PROP_WRITABLE, JS_PROP_ENUMERABLE
   JS_PROP_HAS_GET, JS_PROP_HAS_SET, JS_PROP_HAS_VALUE,
   JS_PROP_HAS_CONFIGURABLE, JS_PROP_HAS_WRITABLE, JS_PROP_HAS_ENUMERABLE,
   JS_PROP_THROW, JS_PROP_NO_EXOTIC.
   If JS_PROP_THROW is set, return an exception instead of FALSE.
   if JS_PROP_NO_EXOTIC is set, do not call the exotic
   define_own_property callback.
   return -1 (exception), FALSE or TRUE.
*/
int JS_DefineProperty(JSContext *ctx, JSValueConst this_obj,
                      JSAtom prop, JSValueConst val,
                      JSValueConst getter, JSValueConst setter, int flags)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    int mask, res;

    if (JS_VALUE_GET_TAG(this_obj) != JS_TAG_OBJECT) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    p = JS_VALUE_GET_OBJ(this_obj);

    redo_prop_update:
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        /* property already exists */
        if (!check_define_prop_flags(prs->flags, flags)) {
            not_configurable:
            return JS_ThrowTypeErrorOrFalse(ctx, flags, "property is not configurable");
        }

        retry:
        if (flags & (JS_PROP_HAS_VALUE | JS_PROP_HAS_WRITABLE |
                     JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
            if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
                JSObject *new_getter, *new_setter;

                if (JS_IsFunction(ctx, getter)) {
                    new_getter = JS_VALUE_GET_OBJ(getter);
                } else {
                    new_getter = NULL;
                }
                if (JS_IsFunction(ctx, setter)) {
                    new_setter = JS_VALUE_GET_OBJ(setter);
                } else {
                    new_setter = NULL;
                }

                if ((prs->flags & JS_PROP_TMASK) != JS_PROP_GETSET) {
                    if (js_shape_prepare_update(ctx, p, &prs))
                        return -1;
                    /* convert to getset */
                    if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                        free_var_ref(ctx->rt, pr->u.var_ref);
                    } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                        /* clear property and update */
                        if (js_shape_prepare_update(ctx, p, &prs))
                            return -1;
                        js_autoinit_free(ctx->rt, pr);
                        prs->flags &= ~JS_PROP_TMASK;
                        pr->u.value = JS_UNDEFINED;
                        goto retry;
                    } else {
                        JS_FreeValue(ctx, pr->u.value);
                    }
                    prs->flags = (prs->flags &
                                  (JS_PROP_CONFIGURABLE | JS_PROP_ENUMERABLE)) |
                                 JS_PROP_GETSET;
                    pr->u.getset.getter = NULL;
                    pr->u.getset.setter = NULL;
                } else {
                    if (!(prs->flags & JS_PROP_CONFIGURABLE)) {
                        if ((flags & JS_PROP_HAS_GET) &&
                            new_getter != pr->u.getset.getter) {
                            goto not_configurable;
                        }
                        if ((flags & JS_PROP_HAS_SET) &&
                            new_setter != pr->u.getset.setter) {
                            goto not_configurable;
                        }
                    }
                }
                if (flags & JS_PROP_HAS_GET) {
                    if (pr->u.getset.getter)
                        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.getter));
                    if (new_getter)
                        JS_DupValue(ctx, getter);
                    pr->u.getset.getter = new_getter;
                }
                if (flags & JS_PROP_HAS_SET) {
                    if (pr->u.getset.setter)
                        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.setter));
                    if (new_setter)
                        JS_DupValue(ctx, setter);
                    pr->u.getset.setter = new_setter;
                }
            } else {
                if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                    /* convert to data descriptor */
                    if (js_shape_prepare_update(ctx, p, &prs))
                        return -1;
                    if (pr->u.getset.getter)
                        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.getter));
                    if (pr->u.getset.setter)
                        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.setter));
                    prs->flags &= ~(JS_PROP_TMASK | JS_PROP_WRITABLE);
                    pr->u.value = JS_UNDEFINED;
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                    /* Note: JS_PROP_VARREF is always writable */
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                    /* clear property and update */
                    if (js_shape_prepare_update(ctx, p, &prs))
                        return -1;
                    js_autoinit_free(ctx->rt, pr);
                    prs->flags &= ~JS_PROP_TMASK;
                    pr->u.value = JS_UNDEFINED;
                } else {
                    if ((prs->flags & (JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE)) == 0 &&
                        (flags & JS_PROP_HAS_VALUE) &&
                        !js_same_value(ctx, val, pr->u.value)) {
                        goto not_configurable;
                    }
                }
                if (prs->flags & JS_PROP_LENGTH) {
                    if (flags & JS_PROP_HAS_VALUE) {
                        res = set_array_length(ctx, p, pr, JS_DupValue(ctx, val),
                                               flags);
                    } else {
                        res = TRUE;
                    }
                    /* still need to reset the writable flag if needed.
                       The JS_PROP_LENGTH is reset to have the correct
                       read-only behavior in JS_SetProperty(). */
                    if ((flags & (JS_PROP_HAS_WRITABLE | JS_PROP_WRITABLE)) ==
                        JS_PROP_HAS_WRITABLE) {
                        prs = get_shape_prop(p->shape);
                        if (js_update_property_flags(ctx, p, &prs,
                                                     prs->flags & ~(JS_PROP_WRITABLE | JS_PROP_LENGTH)))
                            return -1;
                    }
                    return res;
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                    if (flags & JS_PROP_HAS_VALUE) {
                        if (p->class_id == JS_CLASS_MODULE_NS) {
                            /* JS_PROP_WRITABLE is always true for variable
                               references, but they are write protected in module name
                               spaces. */
                            if (!js_same_value(ctx, val, *pr->u.var_ref->pvalue))
                                goto not_configurable;
                        }
                        /* update the reference */
                        set_value(ctx, pr->u.var_ref->pvalue,
                                  JS_DupValue(ctx, val));
                    }
                    /* if writable is set to false, no longer a
                       reference (for mapped arguments) */
                    if ((flags & (JS_PROP_HAS_WRITABLE | JS_PROP_WRITABLE)) == JS_PROP_HAS_WRITABLE) {
                        JSValue val1;
                        if (js_shape_prepare_update(ctx, p, &prs))
                            return -1;
                        val1 = JS_DupValue(ctx, *pr->u.var_ref->pvalue);
                        free_var_ref(ctx->rt, pr->u.var_ref);
                        pr->u.value = val1;
                        prs->flags &= ~(JS_PROP_TMASK | JS_PROP_WRITABLE);
                    }
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                    /* XXX: should never happen, type was reset above */
                    abort();
                } else {
                    if (flags & JS_PROP_HAS_VALUE) {
                        JS_FreeValue(ctx, pr->u.value);
                        pr->u.value = JS_DupValue(ctx, val);
                    }
                    if (flags & JS_PROP_HAS_WRITABLE) {
                        if (js_update_property_flags(ctx, p, &prs,
                                                     (prs->flags & ~JS_PROP_WRITABLE) |
                                                     (flags & JS_PROP_WRITABLE)))
                            return -1;
                    }
                }
            }
        }
        mask = 0;
        if (flags & JS_PROP_HAS_CONFIGURABLE)
            mask |= JS_PROP_CONFIGURABLE;
        if (flags & JS_PROP_HAS_ENUMERABLE)
            mask |= JS_PROP_ENUMERABLE;
        if (js_update_property_flags(ctx, p, &prs,
                                     (prs->flags & ~mask) | (flags & mask)))
            return -1;
        return TRUE;
    }

    /* handle modification of fast array elements */
    if (p->fast_array) {
        uint32_t idx;
        uint32_t prop_flags;
        if (p->class_id == JS_CLASS_ARRAY) {
            if (__JS_AtomIsTaggedInt(prop)) {
                idx = __JS_AtomToUInt32(prop);
                if (idx < p->u.array.count) {
                    prop_flags = get_prop_flags(flags, JS_PROP_C_W_E);
                    if (prop_flags != JS_PROP_C_W_E)
                        goto convert_to_slow_array;
                    if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
                        convert_to_slow_array:
                        if (convert_fast_array_to_array(ctx, p))
                            return -1;
                        else
                            goto redo_prop_update;
                    }
                    if (flags & JS_PROP_HAS_VALUE) {
                        set_value(ctx, &p->u.array.u.values[idx], JS_DupValue(ctx, val));
                    }
                    return TRUE;
                }
            }
        } else if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                   p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
            JSValue num;
            int ret;

            if (!__JS_AtomIsTaggedInt(prop)) {
                /* slow path with to handle all numeric indexes */
                num = JS_AtomIsNumericIndex1(ctx, prop);
                if (JS_IsUndefined(num))
                    goto typed_array_done;
                if (JS_IsException(num))
                    return -1;
                ret = JS_NumberIsInteger(ctx, num);
                if (ret < 0) {
                    JS_FreeValue(ctx, num);
                    return -1;
                }
                if (!ret) {
                    JS_FreeValue(ctx, num);
                    return JS_ThrowTypeErrorOrFalse(ctx, flags, "non integer index in typed array");
                }
                ret = JS_NumberIsNegativeOrMinusZero(ctx, num);
                JS_FreeValue(ctx, num);
                if (ret) {
                    return JS_ThrowTypeErrorOrFalse(ctx, flags, "negative index in typed array");
                }
                if (!__JS_AtomIsTaggedInt(prop))
                    goto typed_array_oob;
            }
            idx = __JS_AtomToUInt32(prop);
            /* if the typed array is detached, p->u.array.count = 0 */
            if (idx >= typed_array_get_length(ctx, p)) {
                typed_array_oob:
                return JS_ThrowTypeErrorOrFalse(ctx, flags, "out-of-bound index in typed array");
            }
            prop_flags = get_prop_flags(flags, JS_PROP_ENUMERABLE | JS_PROP_WRITABLE);
            if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET) ||
                prop_flags != (JS_PROP_ENUMERABLE | JS_PROP_WRITABLE)) {
                return JS_ThrowTypeErrorOrFalse(ctx, flags, "invalid descriptor flags");
            }
            if (flags & JS_PROP_HAS_VALUE) {
                return JS_SetPropertyValue(ctx, this_obj, JS_NewInt32(ctx, idx), JS_DupValue(ctx, val), flags);
            }
            return TRUE;
            typed_array_done: ;
        }
    }

    return JS_CreateProperty(ctx, p, prop, val, getter, setter, flags);
}

static int JS_DefineAutoInitProperty(JSContext *ctx, JSValueConst this_obj,
                                     JSAtom prop, JSAutoInitIDEnum id,
                                     void *opaque, int flags)
{
    JSObject *p;
    JSProperty *pr;

    if (JS_VALUE_GET_TAG(this_obj) != JS_TAG_OBJECT)
        return FALSE;

    p = JS_VALUE_GET_OBJ(this_obj);

    if (find_own_property(&pr, p, prop)) {
        /* property already exists */
        abort();
        return FALSE;
    }

    /* Specialized CreateProperty */
    pr = add_property(ctx, p, prop, (flags & JS_PROP_C_W_E) | JS_PROP_AUTOINIT);
    if (unlikely(!pr))
        return -1;
    pr->u.init.realm_and_id = (uintptr_t)JS_DupContext(ctx);
    assert((pr->u.init.realm_and_id & 3) == 0);
    assert(id <= 3);
    pr->u.init.realm_and_id |= id;
    pr->u.init.opaque = opaque;
    return TRUE;
}

/* shortcut to add or redefine a new property value */
int JS_DefinePropertyValue(JSContext *ctx, JSValueConst this_obj,
                           JSAtom prop, JSValue val, int flags)
{
    int ret;
    ret = JS_DefineProperty(ctx, this_obj, prop, val, JS_UNDEFINED, JS_UNDEFINED,
                            flags | JS_PROP_HAS_VALUE | JS_PROP_HAS_CONFIGURABLE | JS_PROP_HAS_WRITABLE | JS_PROP_HAS_ENUMERABLE);
    JS_FreeValue(ctx, val);
    return ret;
}

int JS_DefinePropertyValueValue(JSContext *ctx, JSValueConst this_obj,
                                JSValue prop, JSValue val, int flags)
{
    JSAtom atom;
    int ret;
    atom = JS_ValueToAtom(ctx, prop);
    JS_FreeValue(ctx, prop);
    if (unlikely(atom == JS_ATOM_NULL)) {
        JS_FreeValue(ctx, val);
        return -1;
    }
    ret = JS_DefinePropertyValue(ctx, this_obj, atom, val, flags);
    JS_FreeAtom(ctx, atom);
    return ret;
}

int JS_DefinePropertyValueUint32(JSContext *ctx, JSValueConst this_obj,
                                 uint32_t idx, JSValue val, int flags)
{
    return JS_DefinePropertyValueValue(ctx, this_obj, JS_NewUint32(ctx, idx),
                                       val, flags);
}

int JS_DefinePropertyValueInt64(JSContext *ctx, JSValueConst this_obj,
                                int64_t idx, JSValue val, int flags)
{
    return JS_DefinePropertyValueValue(ctx, this_obj, JS_NewInt64(ctx, idx),
                                       val, flags);
}

int JS_DefinePropertyValueStr(JSContext *ctx, JSValueConst this_obj,
                              const char *prop, JSValue val, int flags)
{
    JSAtom atom;
    int ret;
    atom = JS_NewAtom(ctx, prop);
    ret = JS_DefinePropertyValue(ctx, this_obj, atom, val, flags);
    JS_FreeAtom(ctx, atom);
    return ret;
}

/* shortcut to add getter & setter */
int JS_DefinePropertyGetSet(JSContext *ctx, JSValueConst this_obj,
                            JSAtom prop, JSValue getter, JSValue setter,
                            int flags)
{
    int ret;
    ret = JS_DefineProperty(ctx, this_obj, prop, JS_UNDEFINED, getter, setter,
                            flags | JS_PROP_HAS_GET | JS_PROP_HAS_SET |
                            JS_PROP_HAS_CONFIGURABLE | JS_PROP_HAS_ENUMERABLE);
    JS_FreeValue(ctx, getter);
    JS_FreeValue(ctx, setter);
    return ret;
}

static int JS_CreateDataPropertyUint32(JSContext *ctx, JSValueConst this_obj,
                                       int64_t idx, JSValue val, int flags)
{
    return JS_DefinePropertyValueValue(ctx, this_obj, JS_NewInt64(ctx, idx),
                                       val, flags | JS_PROP_CONFIGURABLE |
                                            JS_PROP_ENUMERABLE | JS_PROP_WRITABLE);
}


/* return TRUE if 'obj' has a non empty 'name' string */
static BOOL js_object_has_name(JSContext *ctx, JSValueConst obj)
{
    JSProperty *pr;
    JSShapeProperty *prs;
    JSValueConst val;
    JSString *p;

    prs = find_own_property(&pr, JS_VALUE_GET_OBJ(obj), JS_ATOM_name);
    if (!prs)
        return FALSE;
    if ((prs->flags & JS_PROP_TMASK) != JS_PROP_NORMAL)
        return TRUE;
    val = pr->u.value;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_STRING)
        return TRUE;
    p = JS_VALUE_GET_STRING(val);
    return (p->len != 0);
}

static int JS_DefineObjectName(JSContext *ctx, JSValueConst obj,
                               JSAtom name, int flags)
{
    if (name != JS_ATOM_NULL
        &&  JS_IsObject(obj)
        &&  !js_object_has_name(ctx, obj)
        &&  JS_DefinePropertyValue(ctx, obj, JS_ATOM_name, JS_AtomToString(ctx, name), flags) < 0) {
        return -1;
    }
    return 0;
}

static int JS_DefineObjectNameComputed(JSContext *ctx, JSValueConst obj,
                                       JSValueConst str, int flags)
{
    if (JS_IsObject(obj) &&
        !js_object_has_name(ctx, obj)) {
        JSAtom prop;
        JSValue name_str;
        prop = JS_ValueToAtom(ctx, str);
        if (prop == JS_ATOM_NULL)
            return -1;
        name_str = js_get_function_name(ctx, prop);
        JS_FreeAtom(ctx, prop);
        if (JS_IsException(name_str))
            return -1;
        if (JS_DefinePropertyValue(ctx, obj, JS_ATOM_name, name_str, flags) < 0)
            return -1;
    }
    return 0;
}

#define DEFINE_GLOBAL_LEX_VAR (1 << 7)
#define DEFINE_GLOBAL_FUNC_VAR (1 << 6)

static JSValue JS_ThrowSyntaxErrorVarRedeclaration(JSContext *ctx, JSAtom prop)
{
    return JS_ThrowSyntaxErrorAtom(ctx, "redeclaration of '%s'", prop);
}

/* flags is 0, DEFINE_GLOBAL_LEX_VAR or DEFINE_GLOBAL_FUNC_VAR */
/* XXX: could support exotic global object. */
static int JS_CheckDefineGlobalVar(JSContext *ctx, JSAtom prop, int flags)
{
    JSObject *p;
    JSShapeProperty *prs;

    p = JS_VALUE_GET_OBJ(ctx->global_obj);
    prs = find_own_property1(p, prop);
    /* XXX: should handle JS_PROP_AUTOINIT */
    if (flags & DEFINE_GLOBAL_LEX_VAR) {
        if (prs && !(prs->flags & JS_PROP_CONFIGURABLE))
            goto fail_redeclaration;
    } else {
        if (!prs && !p->extensible)
            goto define_error;
        if (flags & DEFINE_GLOBAL_FUNC_VAR) {
            if (prs) {
                if (!(prs->flags & JS_PROP_CONFIGURABLE) &&
                    ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET ||
                     ((prs->flags & (JS_PROP_WRITABLE | JS_PROP_ENUMERABLE)) !=
                      (JS_PROP_WRITABLE | JS_PROP_ENUMERABLE)))) {
                    define_error:
                    JS_ThrowTypeErrorAtom(ctx, "cannot define variable '%s'",
                                          prop);
                    return -1;
                }
            }
        }
    }
    /* check if there already is a lexical declaration */
    p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
    prs = find_own_property1(p, prop);
    if (prs) {
        fail_redeclaration:
        JS_ThrowSyntaxErrorVarRedeclaration(ctx, prop);
        return -1;
    }
    return 0;
}

/* def_flags is (0, DEFINE_GLOBAL_LEX_VAR) |
   JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE */
/* XXX: could support exotic global object. */
static int JS_DefineGlobalVar(JSContext *ctx, JSAtom prop, int def_flags)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSValue val;
    int flags;

    if (def_flags & DEFINE_GLOBAL_LEX_VAR) {
        p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
        flags = JS_PROP_ENUMERABLE | (def_flags & JS_PROP_WRITABLE) |
                JS_PROP_CONFIGURABLE;
        val = JS_UNINITIALIZED;
    } else {
        p = JS_VALUE_GET_OBJ(ctx->global_obj);
        flags = JS_PROP_ENUMERABLE | JS_PROP_WRITABLE |
                (def_flags & JS_PROP_CONFIGURABLE);
        val = JS_UNDEFINED;
    }
    prs = find_own_property1(p, prop);
    if (prs)
        return 0;
    if (!p->extensible)
        return 0;
    pr = add_property(ctx, p, prop, flags);
    if (unlikely(!pr))
        return -1;
    pr->u.value = val;
    return 0;
}

/* 'def_flags' is 0 or JS_PROP_CONFIGURABLE. */
/* XXX: could support exotic global object. */
static int JS_DefineGlobalFunction(JSContext *ctx, JSAtom prop,
                                   JSValueConst func, int def_flags)
{

    JSObject *p;
    JSShapeProperty *prs;
    int flags;

    p = JS_VALUE_GET_OBJ(ctx->global_obj);
    prs = find_own_property1(p, prop);
    flags = JS_PROP_HAS_VALUE | JS_PROP_THROW;
    if (!prs || (prs->flags & JS_PROP_CONFIGURABLE)) {
        flags |= JS_PROP_ENUMERABLE | JS_PROP_WRITABLE | def_flags |
                 JS_PROP_HAS_CONFIGURABLE | JS_PROP_HAS_WRITABLE | JS_PROP_HAS_ENUMERABLE;
    }
    if (JS_DefineProperty(ctx, ctx->global_obj, prop, func,
                          JS_UNDEFINED, JS_UNDEFINED, flags) < 0)
        return -1;
    return 0;
}

static JSValue JS_GetGlobalVar(JSContext *ctx, JSAtom prop,
                               BOOL throw_ref_error)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;

    /* no exotic behavior is possible in global_var_obj */
    p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        /* XXX: should handle JS_PROP_TMASK properties */
        if (unlikely(JS_IsUninitialized(pr->u.value)))
            return JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
        return JS_DupValue(ctx, pr->u.value);
    }
    return JS_GetPropertyInternal(ctx, ctx->global_obj, prop,
                                  ctx->global_obj, throw_ref_error);
}

/* construct a reference to a global variable */
static int JS_GetGlobalVarRef(JSContext *ctx, JSAtom prop, JSValue *sp)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;

    /* no exotic behavior is possible in global_var_obj */
    p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        /* XXX: should handle JS_PROP_AUTOINIT properties? */
        /* XXX: conformance: do these tests in
           OP_put_var_ref/OP_get_var_ref ? */
        if (unlikely(JS_IsUninitialized(pr->u.value))) {
            JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
            return -1;
        }
        if (unlikely(!(prs->flags & JS_PROP_WRITABLE))) {
            return JS_ThrowTypeErrorReadOnly(ctx, JS_PROP_THROW, prop);
        }
        sp[0] = JS_DupValue(ctx, ctx->global_var_obj);
    } else {
        int ret;
        ret = JS_HasProperty(ctx, ctx->global_obj, prop);
        if (ret < 0)
            return -1;
        if (ret) {
            sp[0] = JS_DupValue(ctx, ctx->global_obj);
        } else {
            sp[0] = JS_UNDEFINED;
        }
    }
    sp[1] = JS_AtomToValue(ctx, prop);
    return 0;
}

/* use for strict variable access: test if the variable exists */
static int JS_CheckGlobalVar(JSContext *ctx, JSAtom prop)
{
    JSObject *p;
    JSShapeProperty *prs;
    int ret;

    /* no exotic behavior is possible in global_var_obj */
    p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
    prs = find_own_property1(p, prop);
    if (prs) {
        ret = TRUE;
    } else {
        ret = JS_HasProperty(ctx, ctx->global_obj, prop);
        if (ret < 0)
            return -1;
    }
    return ret;
}

/* flag = 0: normal variable write
   flag = 1: initialize lexical variable
   flag = 2: normal variable write, strict check was done before
*/
static int JS_SetGlobalVar(JSContext *ctx, JSAtom prop, JSValue val,
                           int flag)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    int flags;

    /* no exotic behavior is possible in global_var_obj */
    p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        /* XXX: should handle JS_PROP_AUTOINIT properties? */
        if (flag != 1) {
            if (unlikely(JS_IsUninitialized(pr->u.value))) {
                JS_FreeValue(ctx, val);
                JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
                return -1;
            }
            if (unlikely(!(prs->flags & JS_PROP_WRITABLE))) {
                JS_FreeValue(ctx, val);
                return JS_ThrowTypeErrorReadOnly(ctx, JS_PROP_THROW, prop);
            }
        }
        set_value(ctx, &pr->u.value, val);
        return 0;
    }

    flags = JS_PROP_THROW_STRICT;
    if (flag != 2 && is_strict_mode(ctx))
        flags |= JS_PROP_NO_ADD;
    return JS_SetPropertyInternal(ctx, ctx->global_obj, prop, val, flags);
}

/* return -1, FALSE or TRUE. return FALSE if not configurable or
   invalid object. return -1 in case of exception.
   flags can be 0, JS_PROP_THROW or JS_PROP_THROW_STRICT */
int JS_DeleteProperty(JSContext *ctx, JSValueConst obj, JSAtom prop, int flags)
{
    JSValue obj1;
    JSObject *p;
    int res;

    obj1 = JS_ToObject(ctx, obj);
    if (JS_IsException(obj1))
        return -1;
    p = JS_VALUE_GET_OBJ(obj1);
    res = delete_property(ctx, p, prop);
    JS_FreeValue(ctx, obj1);
    if (res != FALSE)
        return res;
    if ((flags & JS_PROP_THROW) ||
        ((flags & JS_PROP_THROW_STRICT) && is_strict_mode(ctx))) {
        JS_ThrowTypeError(ctx, "could not delete property");
        return -1;
    }
    return FALSE;
}

int JS_DeletePropertyInt64(JSContext *ctx, JSValueConst obj, int64_t idx, int flags)
{
    JSAtom prop;
    int res;

    if ((uint64_t)idx <= JS_ATOM_MAX_INT) {
        /* fast path for fast arrays */
        return JS_DeleteProperty(ctx, obj, __JS_AtomFromUInt32(idx), flags);
    }
    prop = JS_NewAtomInt64(ctx, idx);
    if (prop == JS_ATOM_NULL)
        return -1;
    res = JS_DeleteProperty(ctx, obj, prop, flags);
    JS_FreeAtom(ctx, prop);
    return res;
}

BOOL JS_IsFunction(JSContext *ctx, JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    switch(p->class_id) {
        case JS_CLASS_BYTECODE_FUNCTION:
            return TRUE;
        case JS_CLASS_PROXY:
            return p->u.proxy_data->is_func;
        default:
            return (ctx->rt->class_array[p->class_id].call != NULL);
    }
}

BOOL JS_IsCFunction(JSContext *ctx, JSValueConst val, JSCFunction *func, int magic)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    if (p->class_id == JS_CLASS_C_FUNCTION)
        return (p->u.cfunc.c_function.generic == func && p->u.cfunc.magic == magic);
    else
        return FALSE;
}

BOOL JS_IsConstructor(JSContext *ctx, JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    return p->is_constructor;
}

BOOL JS_SetConstructorBit(JSContext *ctx, JSValueConst func_obj, BOOL val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(func_obj) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(func_obj);
    p->is_constructor = val;
    return TRUE;
}

BOOL JS_IsError(JSContext *ctx, JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    return (p->class_id == JS_CLASS_ERROR);
}

/* used to avoid catching interrupt exceptions */
BOOL JS_IsUncatchableError(JSContext *ctx, JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    return p->class_id == JS_CLASS_ERROR && p->is_uncatchable_error;
}

void JS_SetUncatchableError(JSContext *ctx, JSValueConst val, BOOL flag)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return;
    p = JS_VALUE_GET_OBJ(val);
    if (p->class_id == JS_CLASS_ERROR)
        p->is_uncatchable_error = flag;
}

void JS_ResetUncatchableError(JSContext *ctx)
{
    JS_SetUncatchableError(ctx, ctx->rt->current_exception, FALSE);
}

void JS_SetOpaque(JSValue obj, void *opaque)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(obj) == JS_TAG_OBJECT) {
        p = JS_VALUE_GET_OBJ(obj);
        p->u.opaque = opaque;
    }
}

/* return NULL if not an object of class class_id */
void *JS_GetOpaque(JSValueConst obj, JSClassID class_id)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        return NULL;
    p = JS_VALUE_GET_OBJ(obj);
    if (p->class_id != class_id)
        return NULL;
    return p->u.opaque;
}

void *JS_GetOpaque2(JSContext *ctx, JSValueConst obj, JSClassID class_id)
{
    void *p = JS_GetOpaque(obj, class_id);
    if (unlikely(!p)) {
        JS_ThrowTypeErrorInvalidClass(ctx, class_id);
    }
    return p;
}

#define HINT_STRING  0
#define HINT_NUMBER  1
#define HINT_NONE    2
/* don't try Symbol.toPrimitive */
#define HINT_FORCE_ORDINARY (1 << 4)

static JSValue JS_ToPrimitiveFree(JSContext *ctx, JSValue val, int hint)
{
    int i;
    BOOL force_ordinary;

    JSAtom method_name;
    JSValue method, ret;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return val;
    force_ordinary = hint & HINT_FORCE_ORDINARY;
    hint &= ~HINT_FORCE_ORDINARY;
    if (!force_ordinary) {
        method = JS_GetProperty(ctx, val, JS_ATOM_Symbol_toPrimitive);
        if (JS_IsException(method))
            goto exception;
        /* ECMA says *If exoticToPrim is not undefined* but tests in
           test262 use null as a non callable converter */
        if (!JS_IsUndefined(method) && !JS_IsNull(method)) {
            JSAtom atom;
            JSValue arg;
            switch(hint) {
                case HINT_STRING:
                    atom = JS_ATOM_string;
                    break;
                case HINT_NUMBER:
                    atom = JS_ATOM_number;
                    break;
                default:
                case HINT_NONE:
                    atom = JS_ATOM_default;
                    break;
            }
            arg = JS_AtomToString(ctx, atom);
            ret = JS_CallFree(ctx, method, val, 1, (JSValueConst *)&arg);
            JS_FreeValue(ctx, arg);
            if (JS_IsException(ret))
                goto exception;
            JS_FreeValue(ctx, val);
            if (JS_VALUE_GET_TAG(ret) != JS_TAG_OBJECT)
                return ret;
            JS_FreeValue(ctx, ret);
            return JS_ThrowTypeError(ctx, "toPrimitive");
        }
    }
    if (hint != HINT_STRING)
        hint = HINT_NUMBER;
    for(i = 0; i < 2; i++) {
        if ((i ^ hint) == 0) {
            method_name = JS_ATOM_toString;
        } else {
            method_name = JS_ATOM_valueOf;
        }
        method = JS_GetProperty(ctx, val, method_name);
        if (JS_IsException(method))
            goto exception;
        if (JS_IsFunction(ctx, method)) {
            ret = JS_CallFree(ctx, method, val, 0, NULL);
            if (JS_IsException(ret))
                goto exception;
            if (JS_VALUE_GET_TAG(ret) != JS_TAG_OBJECT) {
                JS_FreeValue(ctx, val);
                return ret;
            }
            JS_FreeValue(ctx, ret);
        } else {
            JS_FreeValue(ctx, method);
        }
    }
    JS_ThrowTypeError(ctx, "toPrimitive");
    exception:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static JSValue JS_ToPrimitive(JSContext *ctx, JSValueConst val, int hint)
{
    return JS_ToPrimitiveFree(ctx, JS_DupValue(ctx, val), hint);
}

static int JS_ToBoolFree(JSContext *ctx, JSValue val)
{
    uint32_t tag = JS_VALUE_GET_TAG(val);
    switch(tag) {
        case JS_TAG_INT:
            return JS_VALUE_GET_INT(val) != 0;
        case JS_TAG_BOOL:
        case JS_TAG_NULL:
        case JS_TAG_UNDEFINED:
            return JS_VALUE_GET_INT(val);
        case JS_TAG_EXCEPTION:
            return -1;
        case JS_TAG_STRING:
        {
            BOOL ret = JS_VALUE_GET_STRING(val)->len != 0;
            JS_FreeValue(ctx, val);
            return ret;
        }
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_INT:
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            BOOL ret;
            ret = p->num.expn != BF_EXP_ZERO && p->num.expn != BF_EXP_NAN;
            JS_FreeValue(ctx, val);
            return ret;
        }
    case JS_TAG_BIG_DECIMAL:
        {
            JSBigDecimal *p = JS_VALUE_GET_PTR(val);
            BOOL ret;
            ret = p->num.expn != BF_EXP_ZERO && p->num.expn != BF_EXP_NAN;
            JS_FreeValue(ctx, val);
            return ret;
        }
#endif
        default:
            if (JS_TAG_IS_FLOAT64(tag)) {
                double d = JS_VALUE_GET_FLOAT64(val);
                return !isnan(d) && d != 0;
            } else {
                JS_FreeValue(ctx, val);
                return TRUE;
            }
    }
}

int JS_ToBool(JSContext *ctx, JSValueConst val)
{
    return JS_ToBoolFree(ctx, JS_DupValue(ctx, val));
}

static int skip_spaces(const char *pc)
{
    const uint8_t *p, *p_next, *p_start;
    uint32_t c;

    p = p_start = (const uint8_t *)pc;
    for (;;) {
        c = *p;
        if (c < 128) {
            if (!((c >= 0x09 && c <= 0x0d) || (c == 0x20)))
                break;
            p++;
        } else {
            c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p_next);
            if (!lre_is_space(c))
                break;
            p = p_next;
        }
    }
    return p - p_start;
}

static inline int to_digit(int c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    else
        return 36;
}

/* XXX: remove */
static double js_strtod(const char *p, int radix, BOOL is_float)
{
    double d;
    int c;

    if (!is_float || radix != 10) {
        uint64_t n_max, n;
        int int_exp, is_neg;

        is_neg = 0;
        if (*p == '-') {
            is_neg = 1;
            p++;
        }

        /* skip leading zeros */
        while (*p == '0')
            p++;
        n = 0;
        if (radix == 10)
            n_max = ((uint64_t)-1 - 9) / 10; /* most common case */
        else
            n_max = ((uint64_t)-1 - (radix - 1)) / radix;
        /* XXX: could be more precise */
        int_exp = 0;
        while (*p != '\0') {
            c = to_digit((uint8_t)*p);
            if (c >= radix)
                break;
            if (n <= n_max) {
                n = n * radix + c;
            } else {
                int_exp++;
            }
            p++;
        }
        d = n;
        if (int_exp != 0) {
            d *= pow(radix, int_exp);
        }
        if (is_neg)
            d = -d;
    } else {
        d = strtod(p, NULL);
    }
    return d;
}

#define ATOD_INT_ONLY        (1 << 0)
/* accept Oo and Ob prefixes in addition to 0x prefix if radix = 0 */
#define ATOD_ACCEPT_BIN_OCT  (1 << 2)
/* accept O prefix as octal if radix == 0 and properly formed (Annex B) */
#define ATOD_ACCEPT_LEGACY_OCTAL  (1 << 4)
/* accept _ between digits as a digit separator */
#define ATOD_ACCEPT_UNDERSCORES  (1 << 5)
/* allow a suffix to override the type */
#define ATOD_ACCEPT_SUFFIX    (1 << 6)
/* default type */
#define ATOD_TYPE_MASK        (3 << 7)
#define ATOD_TYPE_FLOAT64     (0 << 7)
#define ATOD_TYPE_BIG_INT     (1 << 7)
#define ATOD_TYPE_BIG_FLOAT   (2 << 7)
#define ATOD_TYPE_BIG_DECIMAL (3 << 7)
/* assume bigint mode: floats are parsed as integers if no decimal
   point nor exponent */
#define ATOD_MODE_BIGINT      (1 << 9)
/* accept -0x1 */
#define ATOD_ACCEPT_PREFIX_AFTER_SIGN (1 << 10)

#ifdef CONFIG_BIGNUM
static JSValue js_string_to_bigint(JSContext *ctx, const char *buf,
                                   int radix, int flags, slimb_t *pexponent)
{
    bf_t a_s, *a = &a_s;
    int ret;
    JSValue val;
    val = JS_NewBigInt(ctx);
    if (JS_IsException(val))
        return val;
    a = JS_GetBigInt(val);
    ret = bf_atof(a, buf, NULL, radix, BF_PREC_INF, BF_RNDZ);
    if (ret & BF_ST_MEM_ERROR) {
        JS_FreeValue(ctx, val);
        return JS_ThrowOutOfMemory(ctx);
    }
    val = JS_CompactBigInt1(ctx, val, (flags & ATOD_MODE_BIGINT) != 0);
    return val;
}

static JSValue js_string_to_bigfloat(JSContext *ctx, const char *buf,
                                     int radix, int flags, slimb_t *pexponent)
{
    bf_t *a;
    int ret;
    JSValue val;

    val = JS_NewBigFloat(ctx);
    if (JS_IsException(val))
        return val;
    a = JS_GetBigFloat(val);
    if (flags & ATOD_ACCEPT_SUFFIX) {
        /* return the exponent to get infinite precision */
        ret = bf_atof2(a, pexponent, buf, NULL, radix, BF_PREC_INF,
                       BF_RNDZ | BF_ATOF_EXPONENT);
    } else {
        ret = bf_atof(a, buf, NULL, radix, ctx->fp_env.prec,
                      ctx->fp_env.flags);
    }
    if (ret & BF_ST_MEM_ERROR) {
        JS_FreeValue(ctx, val);
        return JS_ThrowOutOfMemory(ctx);
    }
    return val;
}

static JSValue js_string_to_bigdecimal(JSContext *ctx, const char *buf,
                                       int radix, int flags, slimb_t *pexponent)
{
    bfdec_t *a;
    int ret;
    JSValue val;

    val = JS_NewBigDecimal(ctx);
    if (JS_IsException(val))
        return val;
    a = JS_GetBigDecimal(val);
    ret = bfdec_atof(a, buf, NULL, BF_PREC_INF,
                     BF_RNDZ | BF_ATOF_NO_NAN_INF);
    if (ret & BF_ST_MEM_ERROR) {
        JS_FreeValue(ctx, val);
        return JS_ThrowOutOfMemory(ctx);
    }
    return val;
}

#endif

/* return an exception in case of memory error. Return JS_NAN if
   invalid syntax */
#ifdef CONFIG_BIGNUM
static JSValue js_atof2(JSContext *ctx, const char *str, const char **pp,
                        int radix, int flags, slimb_t *pexponent)
#else
static JSValue js_atof(JSContext *ctx, const char *str, const char **pp,
                       int radix, int flags)
#endif
{
    const char *p, *p_start;
    int sep, is_neg;
    BOOL is_float, has_legacy_octal;
    int atod_type = flags & ATOD_TYPE_MASK;
    char buf1[64], *buf;
    int i, j, len;
    BOOL buf_allocated = FALSE;
    JSValue val;

    /* optional separator between digits */
    sep = (flags & ATOD_ACCEPT_UNDERSCORES) ? '_' : 256;
    has_legacy_octal = FALSE;

    p = str;
    p_start = p;
    is_neg = 0;
    if (p[0] == '+') {
        p++;
        p_start++;
        if (!(flags & ATOD_ACCEPT_PREFIX_AFTER_SIGN))
            goto no_radix_prefix;
    } else if (p[0] == '-') {
        p++;
        p_start++;
        is_neg = 1;
        if (!(flags & ATOD_ACCEPT_PREFIX_AFTER_SIGN))
            goto no_radix_prefix;
    }
    if (p[0] == '0') {
        if ((p[1] == 'x' || p[1] == 'X') &&
            (radix == 0 || radix == 16)) {
            p += 2;
            radix = 16;
        } else if ((p[1] == 'o' || p[1] == 'O') &&
                   radix == 0 && (flags & ATOD_ACCEPT_BIN_OCT)) {
            p += 2;
            radix = 8;
        } else if ((p[1] == 'b' || p[1] == 'B') &&
                   radix == 0 && (flags & ATOD_ACCEPT_BIN_OCT)) {
            p += 2;
            radix = 2;
        } else if ((p[1] >= '0' && p[1] <= '9') &&
                   radix == 0 && (flags & ATOD_ACCEPT_LEGACY_OCTAL)) {
            int i;
            has_legacy_octal = TRUE;
            sep = 256;
            for (i = 1; (p[i] >= '0' && p[i] <= '7'); i++)
                continue;
            if (p[i] == '8' || p[i] == '9')
                goto no_prefix;
            p += 1;
            radix = 8;
        } else {
            goto no_prefix;
        }
        /* there must be a digit after the prefix */
        if (to_digit((uint8_t)*p) >= radix)
            goto fail;
        no_prefix: ;
    } else {
        no_radix_prefix:
        if (!(flags & ATOD_INT_ONLY) &&
            (atod_type == ATOD_TYPE_FLOAT64 ||
             atod_type == ATOD_TYPE_BIG_FLOAT) &&
            strstart(p, "Infinity", &p)) {
#ifdef CONFIG_BIGNUM
            if (atod_type == ATOD_TYPE_BIG_FLOAT) {
                bf_t *a;
                val = JS_NewBigFloat(ctx);
                if (JS_IsException(val))
                    goto done;
                a = JS_GetBigFloat(val);
                bf_set_inf(a, is_neg);
            } else
#endif
            {
                double d;
#ifdef _MSC_VER
                d = INFINITY;
#else
                d = 1.0 / 0.0;
#endif
                if (is_neg)
                    d = -d;
                val = JS_NewFloat64(ctx, d);
            }
            goto done;
        }
    }
    if (radix == 0)
        radix = 10;
    is_float = FALSE;
    p_start = p;
    while (to_digit((uint8_t)*p) < radix
           ||  (*p == sep && (radix != 10 ||
                              p != p_start + 1 || p[-1] != '0') &&
                to_digit((uint8_t)p[1]) < radix)) {
        p++;
    }
    if (!(flags & ATOD_INT_ONLY)) {
        if (*p == '.' && (p > p_start || to_digit((uint8_t)p[1]) < radix)) {
            is_float = TRUE;
            p++;
            if (*p == sep)
                goto fail;
            while (to_digit((uint8_t)*p) < radix ||
                   (*p == sep && to_digit((uint8_t)p[1]) < radix))
                p++;
        }
        if (p > p_start &&
            (((*p == 'e' || *p == 'E') && radix == 10) ||
             ((*p == 'p' || *p == 'P') && (radix == 2 || radix == 8 || radix == 16)))) {
            const char *p1 = p + 1;
            is_float = TRUE;
            if (*p1 == '+') {
                p1++;
            } else if (*p1 == '-') {
                p1++;
            }
            if (is_digit((uint8_t)*p1)) {
                p = p1 + 1;
                while (is_digit((uint8_t)*p) || (*p == sep && is_digit((uint8_t)p[1])))
                    p++;
            }
        }
    }
    if (p == p_start)
        goto fail;

    buf = buf1;
    buf_allocated = FALSE;
    len = p - p_start;
    if (unlikely((len + 2) > sizeof(buf1))) {
        buf = js_malloc_rt(ctx->rt, len + 2); /* no exception raised */
        if (!buf)
            goto mem_error;
        buf_allocated = TRUE;
    }
    /* remove the separators and the radix prefixes */
    j = 0;
    if (is_neg)
        buf[j++] = '-';
    for (i = 0; i < len; i++) {
        if (p_start[i] != '_')
            buf[j++] = p_start[i];
    }
    buf[j] = '\0';

#ifdef CONFIG_BIGNUM
    if (flags & ATOD_ACCEPT_SUFFIX) {
        if (*p == 'n') {
            p++;
            atod_type = ATOD_TYPE_BIG_INT;
        } else if (*p == 'l') {
            p++;
            atod_type = ATOD_TYPE_BIG_FLOAT;
        } else if (*p == 'm') {
            p++;
            atod_type = ATOD_TYPE_BIG_DECIMAL;
        } else {
            if (flags & ATOD_MODE_BIGINT) {
                if (!is_float)
                    atod_type = ATOD_TYPE_BIG_INT;
                if (has_legacy_octal)
                    goto fail;
            } else {
                if (is_float && radix != 10)
                    goto fail;
            }
        }
    } else {
        if (atod_type == ATOD_TYPE_FLOAT64) {
            if (flags & ATOD_MODE_BIGINT) {
                if (!is_float)
                    atod_type = ATOD_TYPE_BIG_INT;
                if (has_legacy_octal)
                    goto fail;
            } else {
                if (is_float && radix != 10)
                    goto fail;
            }
        }
    }

    switch(atod_type) {
    case ATOD_TYPE_FLOAT64:
        {
            double d;
            d = js_strtod(buf, radix, is_float);
            /* return int or float64 */
            val = JS_NewFloat64(ctx, d);
        }
        break;
    case ATOD_TYPE_BIG_INT:
        if (has_legacy_octal || is_float)
            goto fail;
        val = ctx->rt->bigint_ops.from_string(ctx, buf, radix, flags, NULL);
        break;
    case ATOD_TYPE_BIG_FLOAT:
        if (has_legacy_octal)
            goto fail;
        val = ctx->rt->bigfloat_ops.from_string(ctx, buf, radix, flags,
                                                pexponent);
        break;
    case ATOD_TYPE_BIG_DECIMAL:
        if (radix != 10)
            goto fail;
        val = ctx->rt->bigdecimal_ops.from_string(ctx, buf, radix, flags, NULL);
        break;
    default:
        abort();
    }
#else
    {
        double d;
        (void)has_legacy_octal;
        if (is_float && radix != 10)
            goto fail;
        d = js_strtod(buf, radix, is_float);
        val = JS_NewFloat64(ctx, d);
    }
#endif

    done:
    if (buf_allocated)
        js_free_rt(ctx->rt, buf);
    if (pp)
        *pp = p;
    return val;
    fail:
    val = JS_NAN;
    goto done;
    mem_error:
    val = JS_ThrowOutOfMemory(ctx);
    goto done;
}

#ifdef CONFIG_BIGNUM
static JSValue js_atof(JSContext *ctx, const char *str, const char **pp,
                       int radix, int flags)
{
    return js_atof2(ctx, str, pp, radix, flags, NULL);
}
#endif

typedef enum JSToNumberHintEnum {
    TON_FLAG_NUMBER,
    TON_FLAG_NUMERIC,
} JSToNumberHintEnum;

static JSValue JS_ToNumberHintFree(JSContext *ctx, JSValue val,
                                   JSToNumberHintEnum flag)
{
    uint32_t tag;
    JSValue ret;

    redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
#ifdef CONFIG_BIGNUM
        case JS_TAG_BIG_DECIMAL:
        if (flag != TON_FLAG_NUMERIC) {
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeError(ctx, "cannot convert bigdecimal to number");
        }
        ret = val;
        break;
    case JS_TAG_BIG_INT:
        if (flag != TON_FLAG_NUMERIC) {
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeError(ctx, "cannot convert bigint to number");
        }
        ret = val;
        break;
    case JS_TAG_BIG_FLOAT:
        if (flag != TON_FLAG_NUMERIC) {
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeError(ctx, "cannot convert bigfloat to number");
        }
        ret = val;
        break;
#endif
        case JS_TAG_FLOAT64:
        case JS_TAG_INT:
        case JS_TAG_EXCEPTION:
            ret = val;
            break;
        case JS_TAG_BOOL:
        case JS_TAG_NULL:
            ret = JS_NewInt32(ctx, JS_VALUE_GET_INT(val));
            break;
        case JS_TAG_UNDEFINED:
            ret = JS_NAN;
            break;
        case JS_TAG_OBJECT:
            val = JS_ToPrimitiveFree(ctx, val, HINT_NUMBER);
            if (JS_IsException(val))
                return JS_EXCEPTION;
            goto redo;
        case JS_TAG_STRING:
        {
            const char *str;
            const char *p;
            size_t len;

            str = JS_ToCStringLen(ctx, &len, val);
            JS_FreeValue(ctx, val);
            if (!str)
                return JS_EXCEPTION;
            p = str;
            p += skip_spaces(p);
            if ((p - str) == len) {
                ret = JS_NewInt32(ctx, 0);
            } else {
                int flags = ATOD_ACCEPT_BIN_OCT;
                ret = js_atof(ctx, p, &p, 0, flags);
                if (!JS_IsException(ret)) {
                    p += skip_spaces(p);
                    if ((p - str) != len) {
                        JS_FreeValue(ctx, ret);
                        ret = JS_NAN;
                    }
                }
            }
            JS_FreeCString(ctx, str);
        }
            break;
        case JS_TAG_SYMBOL:
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeError(ctx, "cannot convert symbol to number");
        default:
            JS_FreeValue(ctx, val);
            ret = JS_NAN;
            break;
    }
    return ret;
}

static JSValue JS_ToNumberFree(JSContext *ctx, JSValue val)
{
    return JS_ToNumberHintFree(ctx, val, TON_FLAG_NUMBER);
}

static JSValue JS_ToNumericFree(JSContext *ctx, JSValue val)
{
    return JS_ToNumberHintFree(ctx, val, TON_FLAG_NUMERIC);
}

static JSValue JS_ToNumeric(JSContext *ctx, JSValueConst val)
{
    return JS_ToNumericFree(ctx, JS_DupValue(ctx, val));
}

static __exception int __JS_ToFloat64Free(JSContext *ctx, double *pres,
                                          JSValue val)
{
    double d;
    uint32_t tag;

    val = JS_ToNumberFree(ctx, val);
    if (JS_IsException(val)) {
        *pres = JS_FLOAT64_NAN;
        return -1;
    }
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
        case JS_TAG_INT:
            d = JS_VALUE_GET_INT(val);
            break;
        case JS_TAG_FLOAT64:
            d = JS_VALUE_GET_FLOAT64(val);
            break;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_INT:
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            /* XXX: there can be a double rounding issue with some
               primitives (such as JS_ToUint8ClampFree()), but it is
               not critical to fix it. */
            bf_get_float64(&p->num, &d, BF_RNDN);
            JS_FreeValue(ctx, val);
        }
        break;
#endif
        default:
            abort();
    }
    *pres = d;
    return 0;
}

static inline int JS_ToFloat64Free(JSContext *ctx, double *pres, JSValue val)
{
    uint32_t tag;

    tag = JS_VALUE_GET_TAG(val);
    if (tag <= JS_TAG_NULL) {
        *pres = JS_VALUE_GET_INT(val);
        return 0;
    } else if (JS_TAG_IS_FLOAT64(tag)) {
        *pres = JS_VALUE_GET_FLOAT64(val);
        return 0;
    } else {
        return __JS_ToFloat64Free(ctx, pres, val);
    }
}

int JS_ToFloat64(JSContext *ctx, double *pres, JSValueConst val)
{
    return JS_ToFloat64Free(ctx, pres, JS_DupValue(ctx, val));
}

static JSValue JS_ToNumber(JSContext *ctx, JSValueConst val)
{
    return JS_ToNumberFree(ctx, JS_DupValue(ctx, val));
}

/* same as JS_ToNumber() but return 0 in case of NaN/Undefined */
static __maybe_unused JSValue JS_ToIntegerFree(JSContext *ctx, JSValue val)
{
uint32_t tag;
JSValue ret;

redo:
tag = JS_VALUE_GET_NORM_TAG(val);
switch(tag) {
case JS_TAG_INT:
case JS_TAG_BOOL:
case JS_TAG_NULL:
case JS_TAG_UNDEFINED:
ret = JS_NewInt32(ctx, JS_VALUE_GET_INT(val));
break;
case JS_TAG_FLOAT64:
{
double d = JS_VALUE_GET_FLOAT64(val);
if (isnan(d)) {
ret = JS_NewInt32(ctx, 0);
} else {
/* convert -0 to +0 */
d = trunc(d) + 0.0;
ret = JS_NewFloat64(ctx, d);
}
}
break;
#ifdef CONFIG_BIGNUM
case JS_TAG_BIG_FLOAT:
        {
            bf_t a_s, *a, r_s, *r = &r_s;
            BOOL is_nan;

            a = JS_ToBigFloat(ctx, &a_s, val);
            if (!bf_is_finite(a)) {
                is_nan = bf_is_nan(a);
                if (is_nan)
                    ret = JS_NewInt32(ctx, 0);
                else
                    ret = JS_DupValue(ctx, val);
            } else {
                ret = JS_NewBigInt(ctx);
                if (!JS_IsException(ret)) {
                    r = JS_GetBigInt(ret);
                    bf_set(r, a);
                    bf_rint(r, BF_RNDZ);
                    ret = JS_CompactBigInt(ctx, ret);
                }
            }
            if (a == &a_s)
                bf_delete(a);
            JS_FreeValue(ctx, val);
        }
        break;
#endif
default:
val = JS_ToNumberFree(ctx, val);
if (JS_IsException(val))
return val;
goto redo;
}
return ret;
}

/* Note: the integer value is satured to 32 bits */
static int JS_ToInt32SatFree(JSContext *ctx, int *pres, JSValue val)
{
    uint32_t tag;
    int ret;

    redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
        case JS_TAG_INT:
        case JS_TAG_BOOL:
        case JS_TAG_NULL:
        case JS_TAG_UNDEFINED:
            ret = JS_VALUE_GET_INT(val);
            break;
        case JS_TAG_EXCEPTION:
            *pres = 0;
            return -1;
        case JS_TAG_FLOAT64:
        {
            double d = JS_VALUE_GET_FLOAT64(val);
            if (isnan(d)) {
                ret = 0;
            } else {
                if (d < INT32_MIN)
                    ret = INT32_MIN;
                else if (d > INT32_MAX)
                    ret = INT32_MAX;
                else
                    ret = (int)d;
            }
        }
            break;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_get_int32(&ret, &p->num, 0);
            JS_FreeValue(ctx, val);
        }
        break;
#endif
        default:
            val = JS_ToNumberFree(ctx, val);
            if (JS_IsException(val)) {
                *pres = 0;
                return -1;
            }
            goto redo;
    }
    *pres = ret;
    return 0;
}

int JS_ToInt32Sat(JSContext *ctx, int *pres, JSValueConst val)
{
    return JS_ToInt32SatFree(ctx, pres, JS_DupValue(ctx, val));
}

int JS_ToInt32Clamp(JSContext *ctx, int *pres, JSValueConst val,
                    int min, int max, int min_offset)
{
    int res = JS_ToInt32SatFree(ctx, pres, JS_DupValue(ctx, val));
    if (res == 0) {
        if (*pres < min) {
            *pres += min_offset;
            if (*pres < min)
                *pres = min;
        } else {
            if (*pres > max)
                *pres = max;
        }
    }
    return res;
}

static int JS_ToInt64SatFree(JSContext *ctx, int64_t *pres, JSValue val)
{
    uint32_t tag;

    redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
        case JS_TAG_INT:
        case JS_TAG_BOOL:
        case JS_TAG_NULL:
        case JS_TAG_UNDEFINED:
            *pres = JS_VALUE_GET_INT(val);
            return 0;
        case JS_TAG_EXCEPTION:
            *pres = 0;
            return -1;
        case JS_TAG_FLOAT64:
        {
            double d = JS_VALUE_GET_FLOAT64(val);
            if (isnan(d)) {
                *pres = 0;
            } else {
                if (d < INT64_MIN)
                    *pres = INT64_MIN;
                else if (d > INT64_MAX)
                    *pres = INT64_MAX;
                else
                    *pres = (int64_t)d;
            }
        }
            return 0;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_get_int64(pres, &p->num, 0);
            JS_FreeValue(ctx, val);
        }
        return 0;
#endif
        default:
            val = JS_ToNumberFree(ctx, val);
            if (JS_IsException(val)) {
                *pres = 0;
                return -1;
            }
            goto redo;
    }
}

int JS_ToInt64Sat(JSContext *ctx, int64_t *pres, JSValueConst val)
{
    return JS_ToInt64SatFree(ctx, pres, JS_DupValue(ctx, val));
}

int JS_ToInt64Clamp(JSContext *ctx, int64_t *pres, JSValueConst val,
                    int64_t min, int64_t max, int64_t neg_offset)
{
    int res = JS_ToInt64SatFree(ctx, pres, JS_DupValue(ctx, val));
    if (res == 0) {
        if (*pres < 0)
            *pres += neg_offset;
        if (*pres < min)
            *pres = min;
        else if (*pres > max)
            *pres = max;
    }
    return res;
}

/* Same as JS_ToInt32Free() but with a 64 bit result. Return (<0, 0)
   in case of exception */
static int JS_ToInt64Free(JSContext *ctx, int64_t *pres, JSValue val)
{
    uint32_t tag;
    int64_t ret;

    redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
        case JS_TAG_INT:
        case JS_TAG_BOOL:
        case JS_TAG_NULL:
        case JS_TAG_UNDEFINED:
            ret = JS_VALUE_GET_INT(val);
            break;
        case JS_TAG_FLOAT64:
        {
            JSFloat64Union u;
            double d;
            int e;
            d = JS_VALUE_GET_FLOAT64(val);
            u.d = d;
            /* we avoid doing fmod(x, 2^64) */
            e = (u.u64 >> 52) & 0x7ff;
            if (likely(e <= (1023 + 62))) {
                /* fast case */
                ret = (int64_t)d;
            } else if (e <= (1023 + 62 + 53)) {
                uint64_t v;
                /* remainder modulo 2^64 */
                v = (u.u64 & (((uint64_t)1 << 52) - 1)) | ((uint64_t)1 << 52);
                ret = v << ((e - 1023) - 52);
                /* take the sign into account */
                if (u.u64 >> 63)
                    ret = -ret;
            } else {
                ret = 0; /* also handles NaN and +inf */
            }
        }
            break;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_get_int64(&ret, &p->num, BF_GET_INT_MOD);
            JS_FreeValue(ctx, val);
        }
        break;
#endif
        default:
            val = JS_ToNumberFree(ctx, val);
            if (JS_IsException(val)) {
                *pres = 0;
                return -1;
            }
            goto redo;
    }
    *pres = ret;
    return 0;
}

int JS_ToInt64(JSContext *ctx, int64_t *pres, JSValueConst val)
{
    return JS_ToInt64Free(ctx, pres, JS_DupValue(ctx, val));
}

int JS_ToInt64Ext(JSContext *ctx, int64_t *pres, JSValueConst val)
{
    if (JS_IsBigInt(ctx, val))
        return JS_ToBigInt64(ctx, pres, val);
    else
        return JS_ToInt64(ctx, pres, val);
}

/* return (<0, 0) in case of exception */
static int JS_ToInt32Free(JSContext *ctx, int32_t *pres, JSValue val)
{
    uint32_t tag;
    int32_t ret;

    redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
        case JS_TAG_INT:
        case JS_TAG_BOOL:
        case JS_TAG_NULL:
        case JS_TAG_UNDEFINED:
            ret = JS_VALUE_GET_INT(val);
            break;
        case JS_TAG_FLOAT64:
        {
            JSFloat64Union u;
            double d;
            int e;
            d = JS_VALUE_GET_FLOAT64(val);
            u.d = d;
            /* we avoid doing fmod(x, 2^32) */
            e = (u.u64 >> 52) & 0x7ff;
            if (likely(e <= (1023 + 30))) {
                /* fast case */
                ret = (int32_t)d;
            } else if (e <= (1023 + 30 + 53)) {
                uint64_t v;
                /* remainder modulo 2^32 */
                v = (u.u64 & (((uint64_t)1 << 52) - 1)) | ((uint64_t)1 << 52);
                v = v << ((e - 1023) - 52 + 32);
                ret = v >> 32;
                /* take the sign into account */
                if (u.u64 >> 63)
                    ret = -ret;
            } else {
                ret = 0; /* also handles NaN and +inf */
            }
        }
            break;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_get_int32(&ret, &p->num, BF_GET_INT_MOD);
            JS_FreeValue(ctx, val);
        }
        break;
#endif
        default:
            val = JS_ToNumberFree(ctx, val);
            if (JS_IsException(val)) {
                *pres = 0;
                return -1;
            }
            goto redo;
    }
    *pres = ret;
    return 0;
}

int JS_ToInt32(JSContext *ctx, int32_t *pres, JSValueConst val)
{
    return JS_ToInt32Free(ctx, pres, JS_DupValue(ctx, val));
}

static inline int JS_ToUint32Free(JSContext *ctx, uint32_t *pres, JSValue val)
{
    return JS_ToInt32Free(ctx, (int32_t *)pres, val);
}

static int JS_ToUint8ClampFree(JSContext *ctx, int32_t *pres, JSValue val)
{
    uint32_t tag;
    int res;

    redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
        case JS_TAG_INT:
        case JS_TAG_BOOL:
        case JS_TAG_NULL:
        case JS_TAG_UNDEFINED:
            res = JS_VALUE_GET_INT(val);
#ifdef CONFIG_BIGNUM
            int_clamp:
#endif
            res = max_int(0, min_int(255, res));
            break;
        case JS_TAG_FLOAT64:
        {
            double d = JS_VALUE_GET_FLOAT64(val);
            if (isnan(d)) {
                res = 0;
            } else {
                if (d < 0)
                    res = 0;
                else if (d > 255)
                    res = 255;
                else
                    res = lrint(d);
            }
        }
            break;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_t r_s, *r = &r_s;
            bf_init(ctx->bf_ctx, r);
            bf_set(r, &p->num);
            bf_rint(r, BF_RNDN);
            bf_get_int32(&res, r, 0);
            bf_delete(r);
            JS_FreeValue(ctx, val);
        }
        goto int_clamp;
#endif
        default:
            val = JS_ToNumberFree(ctx, val);
            if (JS_IsException(val)) {
                *pres = 0;
                return -1;
            }
            goto redo;
    }
    *pres = res;
    return 0;
}

static __exception int JS_ToArrayLengthFree(JSContext *ctx, uint32_t *plen,
                                            JSValue val)
{
    uint32_t tag, len;

    redo:
    tag = JS_VALUE_GET_TAG(val);
    switch(tag) {
        case JS_TAG_INT:
        case JS_TAG_BOOL:
        case JS_TAG_NULL:
        {
            int v;
            v = JS_VALUE_GET_INT(val);
            if (v < 0)
                goto fail;
            len = v;
        }
            break;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_INT:
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_t a;
            BOOL res;
            bf_get_int32((int32_t *)&len, &p->num, BF_GET_INT_MOD);
            bf_init(ctx->bf_ctx, &a);
            bf_set_ui(&a, len);
            res = bf_cmp_eq(&a, &p->num);
            bf_delete(&a);
            JS_FreeValue(ctx, val);
            if (!res)
                goto fail;
        }
        break;
#endif
        default:
            if (JS_TAG_IS_FLOAT64(tag)) {
                double d;
                d = JS_VALUE_GET_FLOAT64(val);
                len = (uint32_t)d;
                if (len != d) {
                    fail:
                    JS_ThrowRangeError(ctx, "invalid array length");
                    return -1;
                }
            } else {
                val = JS_ToNumberFree(ctx, val);
                if (JS_IsException(val))
                    return -1;
                goto redo;
            }
            break;
    }
    *plen = len;
    return 0;
}

#define MAX_SAFE_INTEGER (((int64_t)1 << 53) - 1)

static BOOL is_safe_integer(double d)
{
    return isfinite(d) && floor(d) == d &&
           fabs(d) <= (double)MAX_SAFE_INTEGER;
}

int JS_ToIndex(JSContext *ctx, uint64_t *plen, JSValueConst val)
{
    int64_t v;
    if (JS_ToInt64Sat(ctx, &v, val))
        return -1;
    if (v < 0 || v > MAX_SAFE_INTEGER) {
        JS_ThrowRangeError(ctx, "invalid array index");
        *plen = 0;
        return -1;
    }
    *plen = v;
    return 0;
}

/* convert a value to a length between 0 and MAX_SAFE_INTEGER.
   return -1 for exception */
static __exception int JS_ToLengthFree(JSContext *ctx, int64_t *plen,
                                       JSValue val)
{
    int res = JS_ToInt64Clamp(ctx, plen, val, 0, MAX_SAFE_INTEGER, 0);
    JS_FreeValue(ctx, val);
    return res;
}

/* Note: can return an exception */
static int JS_NumberIsInteger(JSContext *ctx, JSValueConst val)
{
    double d;
    if (!JS_IsNumber(val))
        return FALSE;
    if (unlikely(JS_ToFloat64(ctx, &d, val)))
        return -1;
    return isfinite(d) && floor(d) == d;
}

static BOOL JS_NumberIsNegativeOrMinusZero(JSContext *ctx, JSValueConst val)
{
    uint32_t tag;

    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
        case JS_TAG_INT:
        {
            int v;
            v = JS_VALUE_GET_INT(val);
            return (v < 0);
        }
        case JS_TAG_FLOAT64:
        {
            JSFloat64Union u;
            u.d = JS_VALUE_GET_FLOAT64(val);
            return (u.u64 >> 63);
        }
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_INT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            /* Note: integer zeros are not necessarily positive */
            return p->num.sign && !bf_is_zero(&p->num);
        }
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            return p->num.sign;
        }
        break;
    case JS_TAG_BIG_DECIMAL:
        {
            JSBigDecimal *p = JS_VALUE_GET_PTR(val);
            return p->num.sign;
        }
        break;
#endif
        default:
            return FALSE;
    }
}

#ifdef CONFIG_BIGNUM

static JSValue js_bigint_to_string1(JSContext *ctx, JSValueConst val, int radix)
{
    JSValue ret;
    bf_t a_s, *a;
    char *str;
    int saved_sign;

    a = JS_ToBigInt(ctx, &a_s, val);
    if (!a)
        return JS_EXCEPTION;
    saved_sign = a->sign;
    if (a->expn == BF_EXP_ZERO)
        a->sign = 0;
    str = bf_ftoa(NULL, a, radix, 0, BF_RNDZ | BF_FTOA_FORMAT_FRAC |
                  BF_FTOA_JS_QUIRKS);
    a->sign = saved_sign;
    JS_FreeBigInt(ctx, a, &a_s);
    if (!str)
        return JS_ThrowOutOfMemory(ctx);
    ret = JS_NewString(ctx, str);
    bf_free(ctx->bf_ctx, str);
    return ret;
}

static JSValue js_bigint_to_string(JSContext *ctx, JSValueConst val)
{
    return js_bigint_to_string1(ctx, val, 10);
}

static JSValue js_ftoa(JSContext *ctx, JSValueConst val1, int radix,
                       limb_t prec, bf_flags_t flags)
{
    JSValue val, ret;
    bf_t a_s, *a;
    char *str;
    int saved_sign;

    val = JS_ToNumeric(ctx, val1);
    if (JS_IsException(val))
        return val;
    a = JS_ToBigFloat(ctx, &a_s, val);
    saved_sign = a->sign;
    if (a->expn == BF_EXP_ZERO)
        a->sign = 0;
    flags |= BF_FTOA_JS_QUIRKS;
    if ((flags & BF_FTOA_FORMAT_MASK) == BF_FTOA_FORMAT_FREE_MIN) {
        /* Note: for floating point numbers with a radix which is not
           a power of two, the current precision is used to compute
           the number of digits. */
        if ((radix & (radix - 1)) != 0) {
            bf_t r_s, *r = &r_s;
            int prec, flags1;
            /* must round first */
            if (JS_VALUE_GET_TAG(val) == JS_TAG_BIG_FLOAT) {
                prec = ctx->fp_env.prec;
                flags1 = ctx->fp_env.flags &
                    (BF_FLAG_SUBNORMAL | (BF_EXP_BITS_MASK << BF_EXP_BITS_SHIFT));
            } else {
                prec = 53;
                flags1 = bf_set_exp_bits(11) | BF_FLAG_SUBNORMAL;
            }
            bf_init(ctx->bf_ctx, r);
            bf_set(r, a);
            bf_round(r, prec, flags1 | BF_RNDN);
            str = bf_ftoa(NULL, r, radix, prec, flags1 | flags);
            bf_delete(r);
        } else {
            str = bf_ftoa(NULL, a, radix, BF_PREC_INF, flags);
        }
    } else {
        str = bf_ftoa(NULL, a, radix, prec, flags);
    }
    a->sign = saved_sign;
    if (a == &a_s)
        bf_delete(a);
    JS_FreeValue(ctx, val);
    if (!str)
        return JS_ThrowOutOfMemory(ctx);
    ret = JS_NewString(ctx, str);
    bf_free(ctx->bf_ctx, str);
    return ret;
}

static JSValue js_bigfloat_to_string(JSContext *ctx, JSValueConst val)
{
    return js_ftoa(ctx, val, 10, 0, BF_RNDN | BF_FTOA_FORMAT_FREE_MIN);
}

static JSValue js_bigdecimal_to_string1(JSContext *ctx, JSValueConst val,
                                        limb_t prec, int flags)
{
    JSValue ret;
    bfdec_t *a;
    char *str;
    int saved_sign;

    a = JS_ToBigDecimal(ctx, val);
    saved_sign = a->sign;
    if (a->expn == BF_EXP_ZERO)
        a->sign = 0;
    str = bfdec_ftoa(NULL, a, prec, flags | BF_FTOA_JS_QUIRKS);
    a->sign = saved_sign;
    if (!str)
        return JS_ThrowOutOfMemory(ctx);
    ret = JS_NewString(ctx, str);
    bf_free(ctx->bf_ctx, str);
    return ret;
}

static JSValue js_bigdecimal_to_string(JSContext *ctx, JSValueConst val)
{
    return js_bigdecimal_to_string1(ctx, val, 0,
                                    BF_RNDZ | BF_FTOA_FORMAT_FREE);
}

#endif /* CONFIG_BIGNUM */

/* 2 <= base <= 36 */
static char *i64toa(char *buf_end, int64_t n, unsigned int base)
{
    char *q = buf_end;
    int digit, is_neg;

    is_neg = 0;
    if (n < 0) {
        is_neg = 1;
        n = -n;
    }
    *--q = '\0';
    do {
        digit = (uint64_t)n % base;
        n = (uint64_t)n / base;
        if (digit < 10)
            digit += '0';
        else
            digit += 'a' - 10;
        *--q = digit;
    } while (n != 0);
    if (is_neg)
        *--q = '-';
    return q;
}

/* buf1 contains the printf result */
static void js_ecvt1(double d, int n_digits, int *decpt, int *sign, char *buf,
                     int rounding_mode, char *buf1, int buf1_size)
{
    if (rounding_mode != FE_TONEAREST)
        fesetround(rounding_mode);
    snprintf(buf1, buf1_size, "%+.*e", n_digits - 1, d);
    if (rounding_mode != FE_TONEAREST)
        fesetround(FE_TONEAREST);
    *sign = (buf1[0] == '-');
    /* mantissa */
    buf[0] = buf1[1];
    if (n_digits > 1)
        memcpy(buf + 1, buf1 + 3, n_digits - 1);
    buf[n_digits] = '\0';
    /* exponent */
    *decpt = atoi(buf1 + n_digits + 2 + (n_digits > 1)) + 1;
}

/* maximum buffer size for js_dtoa */
#define JS_DTOA_BUF_SIZE 128

/* needed because ecvt usually limits the number of digits to
   17. Return the number of digits. */
static int js_ecvt(double d, int n_digits, int *decpt, int *sign, char *buf,
                   BOOL is_fixed)
{
    int rounding_mode;
    char buf_tmp[JS_DTOA_BUF_SIZE];

    if (!is_fixed) {
        unsigned int n_digits_min, n_digits_max;
        /* find the minimum amount of digits (XXX: inefficient but simple) */
        n_digits_min = 1;
        n_digits_max = 17;
        while (n_digits_min < n_digits_max) {
            n_digits = (n_digits_min + n_digits_max) / 2;
            js_ecvt1(d, n_digits, decpt, sign, buf, FE_TONEAREST,
                     buf_tmp, sizeof(buf_tmp));
            if (strtod(buf_tmp, NULL) == d) {
                /* no need to keep the trailing zeros */
                while (n_digits >= 2 && buf[n_digits - 1] == '0')
                    n_digits--;
                n_digits_max = n_digits;
            } else {
                n_digits_min = n_digits + 1;
            }
        }
        n_digits = n_digits_max;
        rounding_mode = FE_TONEAREST;
    } else {
        rounding_mode = FE_TONEAREST;
#ifdef CONFIG_PRINTF_RNDN
        {
            char buf1[JS_DTOA_BUF_SIZE], buf2[JS_DTOA_BUF_SIZE];
            int decpt1, sign1, decpt2, sign2;
            /* The JS rounding is specified as round to nearest ties away
               from zero (RNDNA), but in printf the "ties" case is not
               specified (for example it is RNDN for glibc, RNDNA for
               Windows), so we must round manually. */
            js_ecvt1(d, n_digits + 1, &decpt1, &sign1, buf1, FE_TONEAREST,
                     buf_tmp, sizeof(buf_tmp));
            /* XXX: could use 2 digits to reduce the average running time */
            if (buf1[n_digits] == '5') {
                js_ecvt1(d, n_digits + 1, &decpt1, &sign1, buf1, FE_DOWNWARD,
                         buf_tmp, sizeof(buf_tmp));
                js_ecvt1(d, n_digits + 1, &decpt2, &sign2, buf2, FE_UPWARD,
                         buf_tmp, sizeof(buf_tmp));
                if (memcmp(buf1, buf2, n_digits + 1) == 0 && decpt1 == decpt2) {
                    /* exact result: round away from zero */
                    if (sign1)
                        rounding_mode = FE_DOWNWARD;
                    else
                        rounding_mode = FE_UPWARD;
                }
            }
        }
#endif /* CONFIG_PRINTF_RNDN */
    }
    js_ecvt1(d, n_digits, decpt, sign, buf, rounding_mode,
             buf_tmp, sizeof(buf_tmp));
    return n_digits;
}

static int js_fcvt1(char *buf, int buf_size, double d, int n_digits,
                    int rounding_mode)
{
    int n;
    if (rounding_mode != FE_TONEAREST)
        fesetround(rounding_mode);
    n = snprintf(buf, buf_size, "%.*f", n_digits, d);
    if (rounding_mode != FE_TONEAREST)
        fesetround(FE_TONEAREST);
    assert(n < buf_size);
    return n;
}

static void js_fcvt(char *buf, int buf_size, double d, int n_digits)
{
    int rounding_mode;
    rounding_mode = FE_TONEAREST;
#ifdef CONFIG_PRINTF_RNDN
    {
        int n1, n2;
        char buf1[JS_DTOA_BUF_SIZE];
        char buf2[JS_DTOA_BUF_SIZE];

        /* The JS rounding is specified as round to nearest ties away from
           zero (RNDNA), but in printf the "ties" case is not specified
           (for example it is RNDN for glibc, RNDNA for Windows), so we
           must round manually. */
        n1 = js_fcvt1(buf1, sizeof(buf1), d, n_digits + 1, FE_TONEAREST);
        rounding_mode = FE_TONEAREST;
        /* XXX: could use 2 digits to reduce the average running time */
        if (buf1[n1 - 1] == '5') {
            n1 = js_fcvt1(buf1, sizeof(buf1), d, n_digits + 1, FE_DOWNWARD);
            n2 = js_fcvt1(buf2, sizeof(buf2), d, n_digits + 1, FE_UPWARD);
            if (n1 == n2 && memcmp(buf1, buf2, n1) == 0) {
                /* exact result: round away from zero */
                if (buf1[0] == '-')
                    rounding_mode = FE_DOWNWARD;
                else
                    rounding_mode = FE_UPWARD;
            }
        }
    }
#endif /* CONFIG_PRINTF_RNDN */
    js_fcvt1(buf, buf_size, d, n_digits, rounding_mode);
}

/* radix != 10 is only supported with flags = JS_DTOA_VAR_FORMAT */
/* use as many digits as necessary */
#define JS_DTOA_VAR_FORMAT   (0 << 0)
/* use n_digits significant digits (1 <= n_digits <= 101) */
#define JS_DTOA_FIXED_FORMAT (1 << 0)
/* force fractional format: [-]dd.dd with n_digits fractional digits */
#define JS_DTOA_FRAC_FORMAT  (2 << 0)
/* force exponential notation either in fixed or variable format */
#define JS_DTOA_FORCE_EXP    (1 << 2)

/* XXX: slow and maybe not fully correct. Use libbf when it is fast enough.
   XXX: radix != 10 is only supported for small integers
*/
static void js_dtoa1(char *buf, double d, int radix, int n_digits, int flags)
{
    char *q;

    if (!isfinite(d)) {
        if (isnan(d)) {
            strcpy(buf, "NaN");
        } else {
            q = buf;
            if (d < 0)
                *q++ = '-';
            strcpy(q, "Infinity");
        }
    } else if (flags == JS_DTOA_VAR_FORMAT) {
        int64_t i64;
        char buf1[70], *ptr;
        i64 = (int64_t)d;
        if (d != i64 || i64 > MAX_SAFE_INTEGER || i64 < -MAX_SAFE_INTEGER)
            goto generic_conv;
        /* fast path for integers */
        ptr = i64toa(buf1 + sizeof(buf1), i64, radix);
        strcpy(buf, ptr);
    } else {
        if (d == 0.0)
            d = 0.0; /* convert -0 to 0 */
        if (flags == JS_DTOA_FRAC_FORMAT) {
            js_fcvt(buf, JS_DTOA_BUF_SIZE, d, n_digits);
        } else {
            char buf1[JS_DTOA_BUF_SIZE];
            int sign, decpt, k, n, i, p, n_max;
            BOOL is_fixed;
            generic_conv:
            is_fixed = ((flags & 3) == JS_DTOA_FIXED_FORMAT);
            if (is_fixed) {
                n_max = n_digits;
            } else {
                n_max = 21;
            }
            /* the number has k digits (k >= 1) */
            k = js_ecvt(d, n_digits, &decpt, &sign, buf1, is_fixed);
            n = decpt; /* d=10^(n-k)*(buf1) i.e. d= < x.yyyy 10^(n-1) */
            q = buf;
            if (sign)
                *q++ = '-';
            if (flags & JS_DTOA_FORCE_EXP)
                goto force_exp;
            if (n >= 1 && n <= n_max) {
                if (k <= n) {
                    memcpy(q, buf1, k);
                    q += k;
                    for(i = 0; i < (n - k); i++)
                        *q++ = '0';
                    *q = '\0';
                } else {
                    /* k > n */
                    memcpy(q, buf1, n);
                    q += n;
                    *q++ = '.';
                    for(i = 0; i < (k - n); i++)
                        *q++ = buf1[n + i];
                    *q = '\0';
                }
            } else if (n >= -5 && n <= 0) {
                *q++ = '0';
                *q++ = '.';
                for(i = 0; i < -n; i++)
                    *q++ = '0';
                memcpy(q, buf1, k);
                q += k;
                *q = '\0';
            } else {
                force_exp:
                /* exponential notation */
                *q++ = buf1[0];
                if (k > 1) {
                    *q++ = '.';
                    for(i = 1; i < k; i++)
                        *q++ = buf1[i];
                }
                *q++ = 'e';
                p = n - 1;
                if (p >= 0)
                    *q++ = '+';
                sprintf(q, "%d", p);
            }
        }
    }
}

static JSValue js_dtoa(JSContext *ctx,
                       double d, int radix, int n_digits, int flags)
{
    char buf[JS_DTOA_BUF_SIZE];
    js_dtoa1(buf, d, radix, n_digits, flags);
    return JS_NewString(ctx, buf);
}

JSValue JS_ToStringInternal(JSContext *ctx, JSValueConst val, BOOL is_ToPropertyKey)
{
    uint32_t tag;
    const char *str;
    char buf[32];

    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
        case JS_TAG_STRING:
            return JS_DupValue(ctx, val);
        case JS_TAG_INT:
            snprintf(buf, sizeof(buf), "%d", JS_VALUE_GET_INT(val));
            str = buf;
            goto new_string;
        case JS_TAG_BOOL:
            return JS_AtomToString(ctx, JS_VALUE_GET_BOOL(val) ?
                                        JS_ATOM_true : JS_ATOM_false);
        case JS_TAG_NULL:
            return JS_AtomToString(ctx, JS_ATOM_null);
        case JS_TAG_UNDEFINED:
            return JS_AtomToString(ctx, JS_ATOM_undefined);
        case JS_TAG_EXCEPTION:
            return JS_EXCEPTION;
        case JS_TAG_OBJECT:
        {
            JSValue val1, ret;
            val1 = JS_ToPrimitive(ctx, val, HINT_STRING);
            if (JS_IsException(val1))
                return val1;
            ret = JS_ToStringInternal(ctx, val1, is_ToPropertyKey);
            JS_FreeValue(ctx, val1);
            return ret;
        }
            break;
        case JS_TAG_FUNCTION_BYTECODE:
            str = "[function bytecode]";
            goto new_string;
        case JS_TAG_SYMBOL:
            if (is_ToPropertyKey) {
                return JS_DupValue(ctx, val);
            } else {
                return JS_ThrowTypeError(ctx, "cannot convert symbol to string");
            }
        case JS_TAG_FLOAT64:
            return js_dtoa(ctx, JS_VALUE_GET_FLOAT64(val), 10, 0,
                           JS_DTOA_VAR_FORMAT);
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_INT:
        return ctx->rt->bigint_ops.to_string(ctx, val);
    case JS_TAG_BIG_FLOAT:
        return ctx->rt->bigfloat_ops.to_string(ctx, val);
    case JS_TAG_BIG_DECIMAL:
        return ctx->rt->bigdecimal_ops.to_string(ctx, val);
#endif
        default:
            str = "[unsupported type]";
        new_string:
            return JS_NewString(ctx, str);
    }
}

JSValue JS_ToString(JSContext *ctx, JSValueConst val)
{
    return JS_ToStringInternal(ctx, val, FALSE);
}

static JSValue JS_ToStringFree(JSContext *ctx, JSValue val)
{
    JSValue ret;
    ret = JS_ToString(ctx, val);
    JS_FreeValue(ctx, val);
    return ret;
}

static JSValue JS_ToLocaleStringFree(JSContext *ctx, JSValue val)
{
    if (JS_IsUndefined(val) || JS_IsNull(val))
        return JS_ToStringFree(ctx, val);
    return JS_InvokeFree(ctx, val, JS_ATOM_toLocaleString, 0, NULL);
}

JSValue JS_ToPropertyKey(JSContext *ctx, JSValueConst val)
{
    return JS_ToStringInternal(ctx, val, TRUE);
}

static JSValue JS_ToStringCheckObject(JSContext *ctx, JSValueConst val)
{
    uint32_t tag = JS_VALUE_GET_TAG(val);
    if (tag == JS_TAG_NULL || tag == JS_TAG_UNDEFINED)
        return JS_ThrowTypeError(ctx, "null or undefined are forbidden");
    return JS_ToString(ctx, val);
}

static JSValue JS_ToQuotedString(JSContext *ctx, JSValueConst val1)
{
    JSValue val;
    JSString *p;
    int i;
    uint32_t c;
    StringBuffer b_s, *b = &b_s;
    char buf[16];

    val = JS_ToStringCheckObject(ctx, val1);
    if (JS_IsException(val))
        return val;
    p = JS_VALUE_GET_STRING(val);

    if (string_buffer_init(ctx, b, p->len + 2))
        goto fail;

    if (string_buffer_putc8(b, '\"'))
        goto fail;
    for(i = 0; i < p->len; ) {
        c = string_getc(p, &i);
        switch(c) {
            case '\t':
                c = 't';
                goto quote;
            case '\r':
                c = 'r';
                goto quote;
            case '\n':
                c = 'n';
                goto quote;
            case '\b':
                c = 'b';
                goto quote;
            case '\f':
                c = 'f';
                goto quote;
            case '\"':
            case '\\':
            quote:
                if (string_buffer_putc8(b, '\\'))
                    goto fail;
                if (string_buffer_putc8(b, c))
                    goto fail;
                break;
            default:
                if (c < 32 || (c >= 0xd800 && c < 0xe000)) {
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    if (string_buffer_puts8(b, buf))
                        goto fail;
                } else {
                    if (string_buffer_putc(b, c))
                        goto fail;
                }
                break;
        }
    }
    if (string_buffer_putc8(b, '\"'))
        goto fail;
    JS_FreeValue(ctx, val);
    return string_buffer_end(b);
    fail:
    JS_FreeValue(ctx, val);
    string_buffer_free(b);
    return JS_EXCEPTION;
}

static __maybe_unused void JS_DumpObjectHeader(JSRuntime *rt)
{
    printf("%14s %4s %4s %14s %10s %s\n",
           "ADDRESS", "REFS", "SHRF", "PROTO", "CLASS", "PROPS");
}

/* for debug only: dump an object without side effect */
static __maybe_unused void JS_DumpObject(JSRuntime *rt, JSObject *p)
{
    uint32_t i;
    char atom_buf[ATOM_GET_STR_BUF_SIZE];
    JSShape *sh;
    JSShapeProperty *prs;
    JSProperty *pr;
    BOOL is_first = TRUE;

    /* XXX: should encode atoms with special characters */
    sh = p->shape; /* the shape can be NULL while freeing an object */
    printf("%14p %4d ",
           (void *)p,
           p->header.ref_count);
    if (sh) {
        printf("%3d%c %14p ",
               sh->header.ref_count,
               " *"[sh->is_hashed],
               (void *)sh->proto);
    } else {
        printf("%3s  %14s ", "-", "-");
    }
    printf("%10s ",
           JS_AtomGetStrRT(rt, atom_buf, sizeof(atom_buf), rt->class_array[p->class_id].class_name));
    if (p->is_exotic && p->fast_array) {
        printf("[ ");
        for(i = 0; i < p->u.array.count; i++) {
            if (i != 0)
                printf(", ");
            switch (p->class_id) {
                case JS_CLASS_ARRAY:
                case JS_CLASS_ARGUMENTS:
                    JS_DumpValueShort(rt, p->u.array.u.values[i]);
                    break;
                case JS_CLASS_UINT8C_ARRAY:
                case JS_CLASS_INT8_ARRAY:
                case JS_CLASS_UINT8_ARRAY:
                case JS_CLASS_INT16_ARRAY:
                case JS_CLASS_UINT16_ARRAY:
                case JS_CLASS_INT32_ARRAY:
                case JS_CLASS_UINT32_ARRAY:
#ifdef CONFIG_BIGNUM
                    case JS_CLASS_BIG_INT64_ARRAY:
    case JS_CLASS_BIG_UINT64_ARRAY:
#endif
                case JS_CLASS_FLOAT32_ARRAY:
                case JS_CLASS_FLOAT64_ARRAY:
                {
                    int size = 1 << typed_array_size_log2(p->class_id);
                    const uint8_t *b = p->u.array.u.uint8_ptr + i * size;
                    while (size-- > 0)
                        printf("%02X", *b++);
                }
                    break;
            }
        }
        printf(" ] ");
    }

    if (sh) {
        printf("{ ");
        for(i = 0, prs = get_shape_prop(sh); i < sh->prop_count; i++, prs++) {
            if (prs->atom != JS_ATOM_NULL) {
                pr = &p->prop[i];
                if (!is_first)
                    printf(", ");
                printf("%s: ",
                       JS_AtomGetStrRT(rt, atom_buf, sizeof(atom_buf), prs->atom));
                if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                    printf("[getset %p %p]", (void *)pr->u.getset.getter,
                           (void *)pr->u.getset.setter);
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                    printf("[varref %p]", (void *)pr->u.var_ref);
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                    printf("[autoinit %p %d %p]",
                           (void *)js_autoinit_get_realm(pr),
                           js_autoinit_get_id(pr),
                           (void *)pr->u.init.opaque);
                } else {
                    JS_DumpValueShort(rt, pr->u.value);
                }
                is_first = FALSE;
            }
        }
        printf(" }");
    }

    if (js_class_has_bytecode(p->class_id)) {
        JSFunctionBytecode *b = p->u.func.function_bytecode;
        JSVarRef **var_refs;
        if (b->closure_var_count) {
            var_refs = p->u.func.var_refs;
            printf(" Closure:");
            for(i = 0; i < b->closure_var_count; i++) {
                printf(" ");
                JS_DumpValueShort(rt, var_refs[i]->value);
            }
            if (p->u.func.home_object) {
                printf(" HomeObject: ");
                JS_DumpValueShort(rt, JS_MKPTR(JS_TAG_OBJECT, p->u.func.home_object));
            }
        }
    }
    printf("\n");
}

static __maybe_unused void JS_DumpGCObject(JSRuntime *rt, JSGCObjectHeader *p)
{
    if (p->gc_obj_type == JS_GC_OBJ_TYPE_JS_OBJECT) {
        JS_DumpObject(rt, (JSObject *)p);
    } else {
        printf("%14p %4d ",
               (void *)p,
               p->ref_count);
        switch(p->gc_obj_type) {
            case JS_GC_OBJ_TYPE_FUNCTION_BYTECODE:
                printf("[function bytecode]");
                break;
            case JS_GC_OBJ_TYPE_SHAPE:
                printf("[shape]");
                break;
            case JS_GC_OBJ_TYPE_VAR_REF:
                printf("[var_ref]");
                break;
            case JS_GC_OBJ_TYPE_ASYNC_FUNCTION:
                printf("[async_function]");
                break;
            case JS_GC_OBJ_TYPE_JS_CONTEXT:
                printf("[js_context]");
                break;
            default:
                printf("[unknown %d]", p->gc_obj_type);
                break;
        }
        printf("\n");
    }
}

static __maybe_unused void JS_DumpValueShort(JSRuntime *rt,
                                             JSValueConst val)
{
    uint32_t tag = JS_VALUE_GET_NORM_TAG(val);
    const char *str;

    switch(tag) {
        case JS_TAG_INT:
            printf("%d", JS_VALUE_GET_INT(val));
            break;
        case JS_TAG_BOOL:
            if (JS_VALUE_GET_BOOL(val))
                str = "true";
            else
                str = "false";
            goto print_str;
        case JS_TAG_NULL:
            str = "null";
            goto print_str;
        case JS_TAG_EXCEPTION:
            str = "exception";
            goto print_str;
        case JS_TAG_UNINITIALIZED:
            str = "uninitialized";
            goto print_str;
        case JS_TAG_UNDEFINED:
            str = "undefined";
        print_str:
            printf("%s", str);
            break;
        case JS_TAG_FLOAT64:
            printf("%.14g", JS_VALUE_GET_FLOAT64(val));
            break;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_INT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            char *str;
            str = bf_ftoa(NULL, &p->num, 10, 0,
                          BF_RNDZ | BF_FTOA_FORMAT_FRAC);
            printf("%sn", str);
            bf_realloc(&rt->bf_ctx, str, 0);
        }
        break;
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            char *str;
            str = bf_ftoa(NULL, &p->num, 16, BF_PREC_INF,
                          BF_RNDZ | BF_FTOA_FORMAT_FREE | BF_FTOA_ADD_PREFIX);
            printf("%sl", str);
            bf_free(&rt->bf_ctx, str);
        }
        break;
    case JS_TAG_BIG_DECIMAL:
        {
            JSBigDecimal *p = JS_VALUE_GET_PTR(val);
            char *str;
            str = bfdec_ftoa(NULL, &p->num, BF_PREC_INF,
                             BF_RNDZ | BF_FTOA_FORMAT_FREE);
            printf("%sm", str);
            bf_free(&rt->bf_ctx, str);
        }
        break;
#endif
        case JS_TAG_STRING:
        {
            JSString *p;
            p = JS_VALUE_GET_STRING(val);
            JS_DumpString(rt, p);
        }
            break;
        case JS_TAG_FUNCTION_BYTECODE:
        {
            JSFunctionBytecode *b = JS_VALUE_GET_PTR(val);
            char buf[ATOM_GET_STR_BUF_SIZE];
            printf("[bytecode %s]", JS_AtomGetStrRT(rt, buf, sizeof(buf), b->func_name));
        }
            break;
        case JS_TAG_OBJECT:
        {
            JSObject *p = JS_VALUE_GET_OBJ(val);
            JSAtom atom = rt->class_array[p->class_id].class_name;
            char atom_buf[ATOM_GET_STR_BUF_SIZE];
            printf("[%s %p]",
                   JS_AtomGetStrRT(rt, atom_buf, sizeof(atom_buf), atom), (void *)p);
        }
            break;
        case JS_TAG_SYMBOL:
        {
            JSAtomStruct *p = JS_VALUE_GET_PTR(val);
            char atom_buf[ATOM_GET_STR_BUF_SIZE];
            printf("Symbol(%s)",
                   JS_AtomGetStrRT(rt, atom_buf, sizeof(atom_buf), js_get_atom_index(rt, p)));
        }
            break;
        case JS_TAG_MODULE:
            printf("[module]");
            break;
        default:
            printf("[unknown tag %d]", tag);
            break;
    }
}

static __maybe_unused void JS_DumpValue(JSContext *ctx,
                                        JSValueConst val)
{
    JS_DumpValueShort(ctx->rt, val);
}

static __maybe_unused void JS_PrintValue(JSContext *ctx,
                                         const char *str,
                                         JSValueConst val)
{
    printf("%s=", str);
    JS_DumpValueShort(ctx->rt, val);
    printf("\n");
}

/* return -1 if exception (proxy case) or TRUE/FALSE */
int JS_IsArray(JSContext *ctx, JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) == JS_TAG_OBJECT) {
        p = JS_VALUE_GET_OBJ(val);
        if (unlikely(p->class_id == JS_CLASS_PROXY))
            return js_proxy_isArray(ctx, val);
        else
            return p->class_id == JS_CLASS_ARRAY;
    } else {
        return FALSE;
    }
}

static double js_pow(double a, double b)
{
    if (unlikely(!isfinite(b)) && fabs(a) == 1) {
        /* not compatible with IEEE 754 */
        return JS_FLOAT64_NAN;
    } else {
        return pow(a, b);
    }
}

#ifdef CONFIG_BIGNUM

JSValue JS_NewBigInt64_1(JSContext *ctx, int64_t v)
{
    JSValue val;
    bf_t *a;
    val = JS_NewBigInt(ctx);
    if (JS_IsException(val))
        return val;
    a = JS_GetBigInt(val);
    if (bf_set_si(a, v)) {
        JS_FreeValue(ctx, val);
        return JS_ThrowOutOfMemory(ctx);
    }
    return val;
}

JSValue JS_NewBigInt64(JSContext *ctx, int64_t v)
{
    if (is_math_mode(ctx) &&
        v >= -MAX_SAFE_INTEGER && v <= MAX_SAFE_INTEGER) {
        return JS_NewInt64(ctx, v);
    } else {
        return JS_NewBigInt64_1(ctx, v);
    }
}

JSValue JS_NewBigUint64(JSContext *ctx, uint64_t v)
{
    JSValue val;
    if (is_math_mode(ctx) && v <= MAX_SAFE_INTEGER) {
        val = JS_NewInt64(ctx, v);
    } else {
        bf_t *a;
        val = JS_NewBigInt(ctx);
        if (JS_IsException(val))
            return val;
        a = JS_GetBigInt(val);
        if (bf_set_ui(a, v)) {
            JS_FreeValue(ctx, val);
            return JS_ThrowOutOfMemory(ctx);
        }
    }
    return val;
}

/* if the returned bigfloat is allocated it is equal to
   'buf'. Otherwise it is a pointer to the bigfloat in 'val'. Return
   NULL in case of error. */
static bf_t *JS_ToBigFloat(JSContext *ctx, bf_t *buf, JSValueConst val)
{
    uint32_t tag;
    bf_t *r;
    JSBigFloat *p;

    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_BOOL:
    case JS_TAG_NULL:
        r = buf;
        bf_init(ctx->bf_ctx, r);
        if (bf_set_si(r, JS_VALUE_GET_INT(val)))
            goto fail;
        break;
    case JS_TAG_FLOAT64:
        r = buf;
        bf_init(ctx->bf_ctx, r);
        if (bf_set_float64(r, JS_VALUE_GET_FLOAT64(val))) {
        fail:
            bf_delete(r);
            return NULL;
        }
        break;
    case JS_TAG_BIG_INT:
    case JS_TAG_BIG_FLOAT:
        p = JS_VALUE_GET_PTR(val);
        r = &p->num;
        break;
    case JS_TAG_UNDEFINED:
    default:
        r = buf;
        bf_init(ctx->bf_ctx, r);
        bf_set_nan(r);
        break;
    }
    return r;
}

/* return NULL if invalid type */
static bfdec_t *JS_ToBigDecimal(JSContext *ctx, JSValueConst val)
{
    uint32_t tag;
    JSBigDecimal *p;
    bfdec_t *r;

    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_BIG_DECIMAL:
        p = JS_VALUE_GET_PTR(val);
        r = &p->num;
        break;
    default:
        JS_ThrowTypeError(ctx, "bigdecimal expected");
        r = NULL;
        break;
    }
    return r;
}

/* return NaN if bad bigint literal */
static JSValue JS_StringToBigInt(JSContext *ctx, JSValue val)
{
    const char *str, *p;
    size_t len;
    int flags;

    str = JS_ToCStringLen(ctx, &len, val);
    JS_FreeValue(ctx, val);
    if (!str)
        return JS_EXCEPTION;
    p = str;
    p += skip_spaces(p);
    if ((p - str) == len) {
        val = JS_NewBigInt64(ctx, 0);
    } else {
        flags = ATOD_INT_ONLY | ATOD_ACCEPT_BIN_OCT | ATOD_TYPE_BIG_INT;
        if (is_math_mode(ctx))
            flags |= ATOD_MODE_BIGINT;
        val = js_atof(ctx, p, &p, 0, flags);
        p += skip_spaces(p);
        if (!JS_IsException(val)) {
            if ((p - str) != len) {
                JS_FreeValue(ctx, val);
                val = JS_NAN;
            }
        }
    }
    JS_FreeCString(ctx, str);
    return val;
}

static JSValue JS_StringToBigIntErr(JSContext *ctx, JSValue val)
{
    val = JS_StringToBigInt(ctx, val);
    if (JS_VALUE_IS_NAN(val))
        return JS_ThrowSyntaxError(ctx, "invalid bigint literal");
    return val;
}

/* if the returned bigfloat is allocated it is equal to
   'buf'. Otherwise it is a pointer to the bigfloat in 'val'. */
static bf_t *JS_ToBigIntFree(JSContext *ctx, bf_t *buf, JSValue val)
{
    uint32_t tag;
    bf_t *r;
    JSBigFloat *p;

 redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_NULL:
    case JS_TAG_UNDEFINED:
        if (!is_math_mode(ctx))
            goto fail;
        /* fall tru */
    case JS_TAG_BOOL:
        r = buf;
        bf_init(ctx->bf_ctx, r);
        bf_set_si(r, JS_VALUE_GET_INT(val));
        break;
    case JS_TAG_FLOAT64:
        {
            double d = JS_VALUE_GET_FLOAT64(val);
            if (!is_math_mode(ctx))
                goto fail;
            if (!isfinite(d))
                goto fail;
            r = buf;
            bf_init(ctx->bf_ctx, r);
            d = trunc(d);
            bf_set_float64(r, d);
        }
        break;
    case JS_TAG_BIG_INT:
        p = JS_VALUE_GET_PTR(val);
        r = &p->num;
        break;
    case JS_TAG_BIG_FLOAT:
        if (!is_math_mode(ctx))
            goto fail;
        p = JS_VALUE_GET_PTR(val);
        if (!bf_is_finite(&p->num))
            goto fail;
        r = buf;
        bf_init(ctx->bf_ctx, r);
        bf_set(r, &p->num);
        bf_rint(r, BF_RNDZ);
        JS_FreeValue(ctx, val);
        break;
    case JS_TAG_STRING:
        val = JS_StringToBigIntErr(ctx, val);
        if (JS_IsException(val))
            return NULL;
        goto redo;
    case JS_TAG_OBJECT:
        val = JS_ToPrimitiveFree(ctx, val, HINT_NUMBER);
        if (JS_IsException(val))
            return NULL;
        goto redo;
    default:
    fail:
        JS_FreeValue(ctx, val);
        JS_ThrowTypeError(ctx, "cannot convert to bigint");
        return NULL;
    }
    return r;
}

static bf_t *JS_ToBigInt(JSContext *ctx, bf_t *buf, JSValueConst val)
{
    return JS_ToBigIntFree(ctx, buf, JS_DupValue(ctx, val));
}

static __maybe_unused JSValue JS_ToBigIntValueFree(JSContext *ctx, JSValue val)
{
    if (JS_VALUE_GET_TAG(val) == JS_TAG_BIG_INT) {
        return val;
    } else {
        bf_t a_s, *a, *r;
        int ret;
        JSValue res;

        res = JS_NewBigInt(ctx);
        if (JS_IsException(res))
            return JS_EXCEPTION;
        a = JS_ToBigIntFree(ctx, &a_s, val);
        if (!a) {
            JS_FreeValue(ctx, res);
            return JS_EXCEPTION;
        }
        r = JS_GetBigInt(res);
        ret = bf_set(r, a);
        JS_FreeBigInt(ctx, a, &a_s);
        if (ret) {
            JS_FreeValue(ctx, res);
            return JS_ThrowOutOfMemory(ctx);
        }
        return JS_CompactBigInt(ctx, res);
    }
}

/* free the bf_t allocated by JS_ToBigInt */
static void JS_FreeBigInt(JSContext *ctx, bf_t *a, bf_t *buf)
{
    if (a == buf) {
        bf_delete(a);
    } else {
        JSBigFloat *p = (JSBigFloat *)((uint8_t *)a -
                                       offsetof(JSBigFloat, num));
        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_BIG_FLOAT, p));
    }
}

/* XXX: merge with JS_ToInt64Free with a specific flag */
static int JS_ToBigInt64Free(JSContext *ctx, int64_t *pres, JSValue val)
{
    bf_t a_s, *a;

    a = JS_ToBigIntFree(ctx, &a_s, val);
    if (!a) {
        *pres = 0;
        return -1;
    }
    bf_get_int64(pres, a, BF_GET_INT_MOD);
    JS_FreeBigInt(ctx, a, &a_s);
    return 0;
}

int JS_ToBigInt64(JSContext *ctx, int64_t *pres, JSValueConst val)
{
    return JS_ToBigInt64Free(ctx, pres, JS_DupValue(ctx, val));
}

static JSBigFloat *js_new_bf(JSContext *ctx)
{
    JSBigFloat *p;
    p = js_malloc(ctx, sizeof(*p));
    if (!p)
        return NULL;
    p->header.ref_count = 1;
    bf_init(ctx->bf_ctx, &p->num);
    return p;
}

static JSValue JS_NewBigFloat(JSContext *ctx)
{
    JSBigFloat *p;
    p = js_malloc(ctx, sizeof(*p));
    if (!p)
        return JS_EXCEPTION;
    p->header.ref_count = 1;
    bf_init(ctx->bf_ctx, &p->num);
    return JS_MKPTR(JS_TAG_BIG_FLOAT, p);
}

static JSValue JS_NewBigDecimal(JSContext *ctx)
{
    JSBigDecimal *p;
    p = js_malloc(ctx, sizeof(*p));
    if (!p)
        return JS_EXCEPTION;
    p->header.ref_count = 1;
    bfdec_init(ctx->bf_ctx, &p->num);
    return JS_MKPTR(JS_TAG_BIG_DECIMAL, p);
}

static JSValue JS_NewBigInt(JSContext *ctx)
{
    JSBigFloat *p;
    p = js_malloc(ctx, sizeof(*p));
    if (!p)
        return JS_EXCEPTION;
    p->header.ref_count = 1;
    bf_init(ctx->bf_ctx, &p->num);
    return JS_MKPTR(JS_TAG_BIG_INT, p);
}

static JSValue JS_CompactBigInt1(JSContext *ctx, JSValue val,
                                 BOOL convert_to_safe_integer)
{
    int64_t v;
    bf_t *a;

    if (JS_VALUE_GET_TAG(val) != JS_TAG_BIG_INT)
        return val; /* fail safe */
    a = JS_GetBigInt(val);
    if (convert_to_safe_integer && bf_get_int64(&v, a, 0) == 0 &&
        v >= -MAX_SAFE_INTEGER && v <= MAX_SAFE_INTEGER) {
        JS_FreeValue(ctx, val);
        return JS_NewInt64(ctx, v);
    } else if (a->expn == BF_EXP_ZERO && a->sign) {
        JSBigFloat *p = JS_VALUE_GET_PTR(val);
        assert(p->header.ref_count == 1);
        a->sign = 0;
    }
    return val;
}

/* Convert the big int to a safe integer if in math mode. normalize
   the zero representation. Could also be used to convert the bigint
   to a short bigint value. The reference count of the value must be
   1. Cannot fail */
static JSValue JS_CompactBigInt(JSContext *ctx, JSValue val)
{
    return JS_CompactBigInt1(ctx, val, is_math_mode(ctx));
}

/* must be kept in sync with JSOverloadableOperatorEnum */
/* XXX: use atoms ? */
static const char js_overloadable_operator_names[JS_OVOP_COUNT][4] = {
    "+",
    "-",
    "*",
    "/",
    "%",
    "**",
    "|",
    "&",
    "^",
    "<<",
    ">>",
    ">>>",
    "==",
    "<",
    "pos",
    "neg",
    "++",
    "--",
    "~",
};

static int get_ovop_from_opcode(OPCodeEnum op)
{
    switch(op) {
    case OP_add:
        return JS_OVOP_ADD;
    case OP_sub:
        return JS_OVOP_SUB;
    case OP_mul:
        return JS_OVOP_MUL;
    case OP_div:
        return JS_OVOP_DIV;
    case OP_mod:
    case OP_math_mod:
        return JS_OVOP_MOD;
    case OP_pow:
        return JS_OVOP_POW;
    case OP_or:
        return JS_OVOP_OR;
    case OP_and:
        return JS_OVOP_AND;
    case OP_xor:
        return JS_OVOP_XOR;
    case OP_shl:
        return JS_OVOP_SHL;
    case OP_sar:
        return JS_OVOP_SAR;
    case OP_shr:
        return JS_OVOP_SHR;
    case OP_eq:
    case OP_neq:
        return JS_OVOP_EQ;
    case OP_lt:
    case OP_lte:
    case OP_gt:
    case OP_gte:
        return JS_OVOP_LESS;
    case OP_plus:
        return JS_OVOP_POS;
    case OP_neg:
        return JS_OVOP_NEG;
    case OP_inc:
        return JS_OVOP_INC;
    case OP_dec:
        return JS_OVOP_DEC;
    default:
        abort();
    }
}

/* return NULL if not present */
static JSObject *find_binary_op(JSBinaryOperatorDef *def,
                                uint32_t operator_index,
                                JSOverloadableOperatorEnum op)
{
    JSBinaryOperatorDefEntry *ent;
    int i;
    for(i = 0; i < def->count; i++) {
        ent = &def->tab[i];
        if (ent->operator_index == operator_index)
            return ent->ops[op];
    }
    return NULL;
}

/* return -1 if exception, 0 if no operator overloading, 1 if
   overloaded operator called */
static __exception int js_call_binary_op_fallback(JSContext *ctx,
                                                  JSValue *pret,
                                                  JSValueConst op1,
                                                  JSValueConst op2,
                                                  OPCodeEnum op,
                                                  BOOL is_numeric,
                                                  int hint)
{
    JSValue opset1_obj, opset2_obj, method, ret, new_op1, new_op2;
    JSOperatorSetData *opset1, *opset2;
    JSOverloadableOperatorEnum ovop;
    JSObject *p;
    JSValueConst args[2];

    if (!ctx->allow_operator_overloading)
        return 0;

    opset2_obj = JS_UNDEFINED;
    opset1_obj = JS_GetProperty(ctx, op1, JS_ATOM_Symbol_operatorSet);
    if (JS_IsException(opset1_obj))
        goto exception;
    if (JS_IsUndefined(opset1_obj))
        return 0;
    opset1 = JS_GetOpaque2(ctx, opset1_obj, JS_CLASS_OPERATOR_SET);
    if (!opset1)
        goto exception;

    opset2_obj = JS_GetProperty(ctx, op2, JS_ATOM_Symbol_operatorSet);
    if (JS_IsException(opset2_obj))
        goto exception;
    if (JS_IsUndefined(opset2_obj)) {
        JS_FreeValue(ctx, opset1_obj);
        return 0;
    }
    opset2 = JS_GetOpaque2(ctx, opset2_obj, JS_CLASS_OPERATOR_SET);
    if (!opset2)
        goto exception;

    if (opset1->is_primitive && opset2->is_primitive) {
        JS_FreeValue(ctx, opset1_obj);
        JS_FreeValue(ctx, opset2_obj);
        return 0;
    }

    ovop = get_ovop_from_opcode(op);

    if (opset1->operator_counter == opset2->operator_counter) {
        p = opset1->self_ops[ovop];
    } else if (opset1->operator_counter > opset2->operator_counter) {
        p = find_binary_op(&opset1->left, opset2->operator_counter, ovop);
    } else {
        p = find_binary_op(&opset2->right, opset1->operator_counter, ovop);
    }
    if (!p) {
        JS_ThrowTypeError(ctx, "operator %s: no function defined",
                          js_overloadable_operator_names[ovop]);
        goto exception;
    }

    if (opset1->is_primitive) {
        if (is_numeric) {
            new_op1 = JS_ToNumeric(ctx, op1);
        } else {
            new_op1 = JS_ToPrimitive(ctx, op1, hint);
        }
        if (JS_IsException(new_op1))
            goto exception;
    } else {
        new_op1 = JS_DupValue(ctx, op1);
    }

    if (opset2->is_primitive) {
        if (is_numeric) {
            new_op2 = JS_ToNumeric(ctx, op2);
        } else {
            new_op2 = JS_ToPrimitive(ctx, op2, hint);
        }
        if (JS_IsException(new_op2)) {
            JS_FreeValue(ctx, new_op1);
            goto exception;
        }
    } else {
        new_op2 = JS_DupValue(ctx, op2);
    }

    /* XXX: could apply JS_ToPrimitive() if primitive type so that the
       operator function does not get a value object */

    method = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
    if (ovop == JS_OVOP_LESS && (op == OP_lte || op == OP_gt)) {
        args[0] = new_op2;
        args[1] = new_op1;
    } else {
        args[0] = new_op1;
        args[1] = new_op2;
    }
    ret = JS_CallFree(ctx, method, JS_UNDEFINED, 2, args);
    JS_FreeValue(ctx, new_op1);
    JS_FreeValue(ctx, new_op2);
    if (JS_IsException(ret))
        goto exception;
    if (ovop == JS_OVOP_EQ) {
        BOOL res = JS_ToBoolFree(ctx, ret);
        if (op == OP_neq)
            res ^= 1;
        ret = JS_NewBool(ctx, res);
    } else if (ovop == JS_OVOP_LESS) {
        if (JS_IsUndefined(ret)) {
            ret = JS_FALSE;
        } else {
            BOOL res = JS_ToBoolFree(ctx, ret);
            if (op == OP_lte || op == OP_gte)
                res ^= 1;
            ret = JS_NewBool(ctx, res);
        }
    }
    JS_FreeValue(ctx, opset1_obj);
    JS_FreeValue(ctx, opset2_obj);
    *pret = ret;
    return 1;
 exception:
    JS_FreeValue(ctx, opset1_obj);
    JS_FreeValue(ctx, opset2_obj);
    *pret = JS_UNDEFINED;
    return -1;
}

/* try to call the operation on the operatorSet field of 'obj'. Only
   used for "/" and "**" on the BigInt prototype in math mode */
static __exception int js_call_binary_op_simple(JSContext *ctx,
                                                JSValue *pret,
                                                JSValueConst obj,
                                                JSValueConst op1,
                                                JSValueConst op2,
                                                OPCodeEnum op)
{
    JSValue opset1_obj, method, ret, new_op1, new_op2;
    JSOperatorSetData *opset1;
    JSOverloadableOperatorEnum ovop;
    JSObject *p;
    JSValueConst args[2];

    opset1_obj = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_operatorSet);
    if (JS_IsException(opset1_obj))
        goto exception;
    if (JS_IsUndefined(opset1_obj))
        return 0;
    opset1 = JS_GetOpaque2(ctx, opset1_obj, JS_CLASS_OPERATOR_SET);
    if (!opset1)
        goto exception;
    ovop = get_ovop_from_opcode(op);

    p = opset1->self_ops[ovop];
    if (!p) {
        JS_FreeValue(ctx, opset1_obj);
        return 0;
    }

    new_op1 = JS_ToNumeric(ctx, op1);
    if (JS_IsException(new_op1))
        goto exception;
    new_op2 = JS_ToNumeric(ctx, op2);
    if (JS_IsException(new_op2)) {
        JS_FreeValue(ctx, new_op1);
        goto exception;
    }

    method = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
    args[0] = new_op1;
    args[1] = new_op2;
    ret = JS_CallFree(ctx, method, JS_UNDEFINED, 2, args);
    JS_FreeValue(ctx, new_op1);
    JS_FreeValue(ctx, new_op2);
    if (JS_IsException(ret))
        goto exception;
    JS_FreeValue(ctx, opset1_obj);
    *pret = ret;
    return 1;
 exception:
    JS_FreeValue(ctx, opset1_obj);
    *pret = JS_UNDEFINED;
    return -1;
}

/* return -1 if exception, 0 if no operator overloading, 1 if
   overloaded operator called */
static __exception int js_call_unary_op_fallback(JSContext *ctx,
                                                 JSValue *pret,
                                                 JSValueConst op1,
                                                 OPCodeEnum op)
{
    JSValue opset1_obj, method, ret;
    JSOperatorSetData *opset1;
    JSOverloadableOperatorEnum ovop;
    JSObject *p;

    if (!ctx->allow_operator_overloading)
        return 0;

    opset1_obj = JS_GetProperty(ctx, op1, JS_ATOM_Symbol_operatorSet);
    if (JS_IsException(opset1_obj))
        goto exception;
    if (JS_IsUndefined(opset1_obj))
        return 0;
    opset1 = JS_GetOpaque2(ctx, opset1_obj, JS_CLASS_OPERATOR_SET);
    if (!opset1)
        goto exception;
    if (opset1->is_primitive) {
        JS_FreeValue(ctx, opset1_obj);
        return 0;
    }

    ovop = get_ovop_from_opcode(op);

    p = opset1->self_ops[ovop];
    if (!p) {
        JS_ThrowTypeError(ctx, "no overloaded operator %s",
                          js_overloadable_operator_names[ovop]);
        goto exception;
    }
    method = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
    ret = JS_CallFree(ctx, method, JS_UNDEFINED, 1, &op1);
    if (JS_IsException(ret))
        goto exception;
    JS_FreeValue(ctx, opset1_obj);
    *pret = ret;
    return 1;
 exception:
    JS_FreeValue(ctx, opset1_obj);
    *pret = JS_UNDEFINED;
    return -1;
}

static JSValue throw_bf_exception(JSContext *ctx, int status)
{
    const char *str;
    if (status & BF_ST_MEM_ERROR)
        return JS_ThrowOutOfMemory(ctx);
    if (status & BF_ST_DIVIDE_ZERO) {
        str = "division by zero";
    } else if (status & BF_ST_INVALID_OP) {
        str = "invalid operation";
    } else {
        str = "integer overflow";
    }
    return JS_ThrowRangeError(ctx, "%s", str);
}

static int js_unary_arith_bigint(JSContext *ctx,
                                 JSValue *pres, OPCodeEnum op, JSValue op1)
{
    bf_t a_s, *r, *a;
    int ret, v;
    JSValue res;

    if (op == OP_plus && !is_math_mode(ctx)) {
        JS_ThrowTypeError(ctx, "bigint argument with unary +");
        JS_FreeValue(ctx, op1);
        return -1;
    }
    res = JS_NewBigInt(ctx);
    if (JS_IsException(res)) {
        JS_FreeValue(ctx, op1);
        return -1;
    }
    r = JS_GetBigInt(res);
    a = JS_ToBigInt(ctx, &a_s, op1);
    ret = 0;
    switch(op) {
    case OP_inc:
    case OP_dec:
        v = 2 * (op - OP_dec) - 1;
        ret = bf_add_si(r, a, v, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_plus:
        ret = bf_set(r, a);
        break;
    case OP_neg:
        ret = bf_set(r, a);
        bf_neg(r);
        break;
    case OP_not:
        ret = bf_add_si(r, a, 1, BF_PREC_INF, BF_RNDZ);
        bf_neg(r);
        break;
    default:
        abort();
    }
    JS_FreeBigInt(ctx, a, &a_s);
    JS_FreeValue(ctx, op1);
    if (unlikely(ret)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    res = JS_CompactBigInt(ctx, res);
    *pres = res;
    return 0;
}

static int js_unary_arith_bigfloat(JSContext *ctx,
                                   JSValue *pres, OPCodeEnum op, JSValue op1)
{
    bf_t a_s, *r, *a;
    int ret, v;
    JSValue res;

    if (op == OP_plus && !is_math_mode(ctx)) {
        JS_ThrowTypeError(ctx, "bigfloat argument with unary +");
        JS_FreeValue(ctx, op1);
        return -1;
    }

    res = JS_NewBigFloat(ctx);
    if (JS_IsException(res)) {
        JS_FreeValue(ctx, op1);
        return -1;
    }
    r = JS_GetBigFloat(res);
    a = JS_ToBigFloat(ctx, &a_s, op1);
    ret = 0;
    switch(op) {
    case OP_inc:
    case OP_dec:
        v = 2 * (op - OP_dec) - 1;
        ret = bf_add_si(r, a, v, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case OP_plus:
        ret = bf_set(r, a);
        break;
    case OP_neg:
        ret = bf_set(r, a);
        bf_neg(r);
        break;
    default:
        abort();
    }
    if (a == &a_s)
        bf_delete(a);
    JS_FreeValue(ctx, op1);
    if (unlikely(ret & BF_ST_MEM_ERROR)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    *pres = res;
    return 0;
}

static int js_unary_arith_bigdecimal(JSContext *ctx,
                                     JSValue *pres, OPCodeEnum op, JSValue op1)
{
    bfdec_t *r, *a;
    int ret, v;
    JSValue res;

    if (op == OP_plus && !is_math_mode(ctx)) {
        JS_ThrowTypeError(ctx, "bigdecimal argument with unary +");
        JS_FreeValue(ctx, op1);
        return -1;
    }

    res = JS_NewBigDecimal(ctx);
    if (JS_IsException(res)) {
        JS_FreeValue(ctx, op1);
        return -1;
    }
    r = JS_GetBigDecimal(res);
    a = JS_ToBigDecimal(ctx, op1);
    ret = 0;
    switch(op) {
    case OP_inc:
    case OP_dec:
        v = 2 * (op - OP_dec) - 1;
        ret = bfdec_add_si(r, a, v, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_plus:
        ret = bfdec_set(r, a);
        break;
    case OP_neg:
        ret = bfdec_set(r, a);
        bfdec_neg(r);
        break;
    default:
        abort();
    }
    JS_FreeValue(ctx, op1);
    if (unlikely(ret)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    *pres = res;
    return 0;
}

static no_inline __exception int js_unary_arith_slow(JSContext *ctx,
                                                     JSValue *sp,
                                                     OPCodeEnum op)
{
    JSValue op1, val;
    int v, ret;
    uint32_t tag;

    op1 = sp[-1];
    /* fast path for float64 */
    if (JS_TAG_IS_FLOAT64(JS_VALUE_GET_TAG(op1)))
        goto handle_float64;
    if (JS_IsObject(op1)) {
        ret = js_call_unary_op_fallback(ctx, &val, op1, op);
        if (ret < 0)
            return -1;
        if (ret) {
            JS_FreeValue(ctx, op1);
            sp[-1] = val;
            return 0;
        }
    }

    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1))
        goto exception;
    tag = JS_VALUE_GET_TAG(op1);
    switch(tag) {
    case JS_TAG_INT:
        {
            int64_t v64;
            v64 = JS_VALUE_GET_INT(op1);
            switch(op) {
            case OP_inc:
            case OP_dec:
                v = 2 * (op - OP_dec) - 1;
                v64 += v;
                break;
            case OP_plus:
                break;
            case OP_neg:
                if (v64 == 0) {
                    sp[-1] = __JS_NewFloat64(ctx, -0.0);
                    return 0;
                } else {
                    v64 = -v64;
                }
                break;
            default:
                abort();
            }
            sp[-1] = JS_NewInt64(ctx, v64);
        }
        break;
    case JS_TAG_BIG_INT:
    handle_bigint:
        if (ctx->rt->bigint_ops.unary_arith(ctx, sp - 1, op, op1))
            goto exception;
        break;
    case JS_TAG_BIG_FLOAT:
        if (ctx->rt->bigfloat_ops.unary_arith(ctx, sp - 1, op, op1))
            goto exception;
        break;
    case JS_TAG_BIG_DECIMAL:
        if (ctx->rt->bigdecimal_ops.unary_arith(ctx, sp - 1, op, op1))
            goto exception;
        break;
    default:
    handle_float64:
        {
            double d;
            if (is_math_mode(ctx))
                goto handle_bigint;
            d = JS_VALUE_GET_FLOAT64(op1);
            switch(op) {
            case OP_inc:
            case OP_dec:
                v = 2 * (op - OP_dec) - 1;
                d += v;
                break;
            case OP_plus:
                break;
            case OP_neg:
                d = -d;
                break;
            default:
                abort();
            }
            sp[-1] = __JS_NewFloat64(ctx, d);
        }
        break;
    }
    return 0;
 exception:
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static __exception int js_post_inc_slow(JSContext *ctx,
                                        JSValue *sp, OPCodeEnum op)
{
    JSValue op1;

    /* XXX: allow custom operators */
    op1 = sp[-1];
    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1)) {
        sp[-1] = JS_UNDEFINED;
        return -1;
    }
    sp[-1] = op1;
    sp[0] = JS_DupValue(ctx, op1);
    return js_unary_arith_slow(ctx, sp + 1, op - OP_post_dec + OP_dec);
}

static no_inline int js_not_slow(JSContext *ctx, JSValue *sp)
{
    JSValue op1, val;
    int ret;

    op1 = sp[-1];
    if (JS_IsObject(op1)) {
        ret = js_call_unary_op_fallback(ctx, &val, op1, OP_not);
        if (ret < 0)
            return -1;
        if (ret) {
            JS_FreeValue(ctx, op1);
            sp[-1] = val;
            return 0;
        }
    }

    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1))
        goto exception;
    if (is_math_mode(ctx) || JS_VALUE_GET_TAG(op1) == JS_TAG_BIG_INT) {
        if (ctx->rt->bigint_ops.unary_arith(ctx, sp - 1, OP_not, op1))
            goto exception;
    } else {
        int32_t v1;
        if (unlikely(JS_ToInt32Free(ctx, &v1, op1)))
            goto exception;
        sp[-1] = JS_NewInt32(ctx, ~v1);
    }
    return 0;
 exception:
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static int js_binary_arith_bigfloat(JSContext *ctx, OPCodeEnum op,
                                    JSValue *pres, JSValue op1, JSValue op2)
{
    bf_t a_s, b_s, *r, *a, *b;
    int ret;
    JSValue res;

    res = JS_NewBigFloat(ctx);
    if (JS_IsException(res)) {
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
        return -1;
    }
    r = JS_GetBigFloat(res);
    a = JS_ToBigFloat(ctx, &a_s, op1);
    b = JS_ToBigFloat(ctx, &b_s, op2);
    bf_init(ctx->bf_ctx, r);
    switch(op) {
    case OP_add:
        ret = bf_add(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case OP_sub:
        ret = bf_sub(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case OP_mul:
        ret = bf_mul(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case OP_div:
        ret = bf_div(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case OP_math_mod:
        /* Euclidian remainder */
        ret = bf_rem(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags,
                     BF_DIVREM_EUCLIDIAN);
        break;
    case OP_mod:
        ret = bf_rem(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags,
                     BF_RNDZ);
        break;
    case OP_pow:
        ret = bf_pow(r, a, b, ctx->fp_env.prec,
                     ctx->fp_env.flags | BF_POW_JS_QUIRKS);
        break;
    default:
        abort();
    }
    if (a == &a_s)
        bf_delete(a);
    if (b == &b_s)
        bf_delete(b);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    if (unlikely(ret & BF_ST_MEM_ERROR)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    *pres = res;
    return 0;
}

static int js_binary_arith_bigint(JSContext *ctx, OPCodeEnum op,
                                  JSValue *pres, JSValue op1, JSValue op2)
{
    bf_t a_s, b_s, *r, *a, *b;
    int ret;
    JSValue res;

    res = JS_NewBigInt(ctx);
    if (JS_IsException(res))
        goto fail;
    a = JS_ToBigInt(ctx, &a_s, op1);
    if (!a)
        goto fail;
    b = JS_ToBigInt(ctx, &b_s, op2);
    if (!b) {
        JS_FreeBigInt(ctx, a, &a_s);
        goto fail;
    }
    r = JS_GetBigInt(res);
    ret = 0;
    switch(op) {
    case OP_add:
        ret = bf_add(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_sub:
        ret = bf_sub(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_mul:
        ret = bf_mul(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_div:
        if (!is_math_mode(ctx)) {
            bf_t rem_s, *rem = &rem_s;
            bf_init(ctx->bf_ctx, rem);
            ret = bf_divrem(r, rem, a, b, BF_PREC_INF, BF_RNDZ,
                            BF_RNDZ);
            bf_delete(rem);
        } else {
            goto math_mode_div_pow;
        }
        break;
    case OP_math_mod:
        /* Euclidian remainder */
        ret = bf_rem(r, a, b, BF_PREC_INF, BF_RNDZ,
                     BF_DIVREM_EUCLIDIAN) & BF_ST_INVALID_OP;
        break;
    case OP_mod:
        ret = bf_rem(r, a, b, BF_PREC_INF, BF_RNDZ,
                     BF_RNDZ) & BF_ST_INVALID_OP;
        break;
    case OP_pow:
        if (b->sign) {
            if (!is_math_mode(ctx)) {
                ret = BF_ST_INVALID_OP;
            } else {
            math_mode_div_pow:
                JS_FreeValue(ctx, res);
                ret = js_call_binary_op_simple(ctx, &res, ctx->class_proto[JS_CLASS_BIG_INT], op1, op2, op);
                if (ret != 0) {
                    JS_FreeBigInt(ctx, a, &a_s);
                    JS_FreeBigInt(ctx, b, &b_s);
                    JS_FreeValue(ctx, op1);
                    JS_FreeValue(ctx, op2);
                    if (ret < 0) {
                        return -1;
                    } else {
                        *pres = res;
                        return 0;
                    }
                }
                /* if no BigInt power operator defined, return a
                   bigfloat */
                res = JS_NewBigFloat(ctx);
                if (JS_IsException(res)) {
                    JS_FreeBigInt(ctx, a, &a_s);
                    JS_FreeBigInt(ctx, b, &b_s);
                    goto fail;
                }
                r = JS_GetBigFloat(res);
                if (op == OP_div) {
                    ret = bf_div(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags) & BF_ST_MEM_ERROR;
                } else {
                    ret = bf_pow(r, a, b, ctx->fp_env.prec,
                                 ctx->fp_env.flags | BF_POW_JS_QUIRKS) & BF_ST_MEM_ERROR;
                }
                JS_FreeBigInt(ctx, a, &a_s);
                JS_FreeBigInt(ctx, b, &b_s);
                JS_FreeValue(ctx, op1);
                JS_FreeValue(ctx, op2);
                if (unlikely(ret)) {
                    JS_FreeValue(ctx, res);
                    throw_bf_exception(ctx, ret);
                    return -1;
                }
                *pres = res;
                return 0;
            }
        } else {
            ret = bf_pow(r, a, b, BF_PREC_INF, BF_RNDZ | BF_POW_JS_QUIRKS);
        }
        break;

        /* logical operations */
    case OP_shl:
    case OP_sar:
        {
            slimb_t v2;
#if LIMB_BITS == 32
            bf_get_int32(&v2, b, 0);
            if (v2 == INT32_MIN)
                v2 = INT32_MIN + 1;
#else
            bf_get_int64(&v2, b, 0);
            if (v2 == INT64_MIN)
                v2 = INT64_MIN + 1;
#endif
            if (op == OP_sar)
                v2 = -v2;
            ret = bf_set(r, a);
            ret |= bf_mul_2exp(r, v2, BF_PREC_INF, BF_RNDZ);
            if (v2 < 0) {
                ret |= bf_rint(r, BF_RNDD) & (BF_ST_OVERFLOW | BF_ST_MEM_ERROR);
            }
        }
        break;
    case OP_and:
        ret = bf_logic_and(r, a, b);
        break;
    case OP_or:
        ret = bf_logic_or(r, a, b);
        break;
    case OP_xor:
        ret = bf_logic_xor(r, a, b);
        break;
    default:
        abort();
    }
    JS_FreeBigInt(ctx, a, &a_s);
    JS_FreeBigInt(ctx, b, &b_s);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    if (unlikely(ret)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    *pres = JS_CompactBigInt(ctx, res);
    return 0;
 fail:
    JS_FreeValue(ctx, res);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    return -1;
}

/* b must be a positive integer */
static int js_bfdec_pow(bfdec_t *r, const bfdec_t *a, const bfdec_t *b)
{
    bfdec_t b1;
    int32_t b2;
    int ret;

    bfdec_init(b->ctx, &b1);
    ret = bfdec_set(&b1, b);
    if (ret) {
        bfdec_delete(&b1);
        return ret;
    }
    ret = bfdec_rint(&b1, BF_RNDZ);
    if (ret) {
        bfdec_delete(&b1);
        return BF_ST_INVALID_OP; /* must be an integer */
    }
    ret = bfdec_get_int32(&b2, &b1);
    bfdec_delete(&b1);
    if (ret)
        return ret; /* overflow */
    if (b2 < 0)
        return BF_ST_INVALID_OP; /* must be positive */
    return bfdec_pow_ui(r, a, b2);
}

static int js_binary_arith_bigdecimal(JSContext *ctx, OPCodeEnum op,
                                      JSValue *pres, JSValue op1, JSValue op2)
{
    bfdec_t *r, *a, *b;
    int ret;
    JSValue res;

    res = JS_NewBigDecimal(ctx);
    if (JS_IsException(res))
        goto fail;
    r = JS_GetBigDecimal(res);

    a = JS_ToBigDecimal(ctx, op1);
    if (!a)
        goto fail;
    b = JS_ToBigDecimal(ctx, op2);
    if (!b)
        goto fail;
    switch(op) {
    case OP_add:
        ret = bfdec_add(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_sub:
        ret = bfdec_sub(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_mul:
        ret = bfdec_mul(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_div:
        ret = bfdec_div(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_math_mod:
        /* Euclidian remainder */
        ret = bfdec_rem(r, a, b, BF_PREC_INF, BF_RNDZ, BF_DIVREM_EUCLIDIAN);
        break;
    case OP_mod:
        ret = bfdec_rem(r, a, b, BF_PREC_INF, BF_RNDZ, BF_RNDZ);
        break;
    case OP_pow:
        ret = js_bfdec_pow(r, a, b);
        break;
    default:
        abort();
    }
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    if (unlikely(ret)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    *pres = res;
    return 0;
 fail:
    JS_FreeValue(ctx, res);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    return -1;
}

static no_inline __exception int js_binary_arith_slow(JSContext *ctx, JSValue *sp,
                                                      OPCodeEnum op)
{
    JSValue op1, op2, res;
    uint32_t tag1, tag2;
    int ret;
    double d1, d2;

    op1 = sp[-2];
    op2 = sp[-1];
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    /* fast path for float operations */
    if (tag1 == JS_TAG_FLOAT64 && tag2 == JS_TAG_FLOAT64) {
        d1 = JS_VALUE_GET_FLOAT64(op1);
        d2 = JS_VALUE_GET_FLOAT64(op2);
        goto handle_float64;
    }

    /* try to call an overloaded operator */
    if ((tag1 == JS_TAG_OBJECT &&
         (tag2 != JS_TAG_NULL && tag2 != JS_TAG_UNDEFINED)) ||
        (tag2 == JS_TAG_OBJECT &&
         (tag1 != JS_TAG_NULL && tag1 != JS_TAG_UNDEFINED))) {
        ret = js_call_binary_op_fallback(ctx, &res, op1, op2, op, TRUE, 0);
        if (ret != 0) {
            JS_FreeValue(ctx, op1);
            JS_FreeValue(ctx, op2);
            if (ret < 0) {
                goto exception;
            } else {
                sp[-2] = res;
                return 0;
            }
        }
    }

    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToNumericFree(ctx, op2);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);

    if (tag1 == JS_TAG_INT && tag2 == JS_TAG_INT) {
        int32_t v1, v2;
        int64_t v;
        v1 = JS_VALUE_GET_INT(op1);
        v2 = JS_VALUE_GET_INT(op2);
        switch(op) {
        case OP_sub:
            v = (int64_t)v1 - (int64_t)v2;
            break;
        case OP_mul:
            v = (int64_t)v1 * (int64_t)v2;
            if (is_math_mode(ctx) &&
                (v < -MAX_SAFE_INTEGER || v > MAX_SAFE_INTEGER))
                goto handle_bigint;
            if (v == 0 && (v1 | v2) < 0) {
                sp[-2] = __JS_NewFloat64(ctx, -0.0);
                return 0;
            }
            break;
        case OP_div:
            if (is_math_mode(ctx))
                goto handle_bigint;
            sp[-2] = __JS_NewFloat64(ctx, (double)v1 / (double)v2);
            return 0;
        case OP_math_mod:
            if (unlikely(v2 == 0)) {
                throw_bf_exception(ctx, BF_ST_DIVIDE_ZERO);
                goto exception;
            }
            v = (int64_t)v1 % (int64_t)v2;
            if (v < 0) {
                if (v2 < 0)
                    v -= v2;
                else
                    v += v2;
            }
            break;
        case OP_mod:
            if (v1 < 0 || v2 <= 0) {
                sp[-2] = JS_NewFloat64(ctx, fmod(v1, v2));
                return 0;
            } else {
                v = (int64_t)v1 % (int64_t)v2;
            }
            break;
        case OP_pow:
            if (!is_math_mode(ctx)) {
                sp[-2] = JS_NewFloat64(ctx, js_pow(v1, v2));
                return 0;
            } else {
                goto handle_bigint;
            }
            break;
        default:
            abort();
        }
        sp[-2] = JS_NewInt64(ctx, v);
    } else if (tag1 == JS_TAG_BIG_DECIMAL || tag2 == JS_TAG_BIG_DECIMAL) {
        if (ctx->rt->bigdecimal_ops.binary_arith(ctx, op, sp - 2, op1, op2))
            goto exception;
    } else if (tag1 == JS_TAG_BIG_FLOAT || tag2 == JS_TAG_BIG_FLOAT) {
        if (ctx->rt->bigfloat_ops.binary_arith(ctx, op, sp - 2, op1, op2))
            goto exception;
    } else if (tag1 == JS_TAG_BIG_INT || tag2 == JS_TAG_BIG_INT) {
    handle_bigint:
        if (ctx->rt->bigint_ops.binary_arith(ctx, op, sp - 2, op1, op2))
            goto exception;
    } else {
        double dr;
        /* float64 result */
        if (JS_ToFloat64Free(ctx, &d1, op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        if (JS_ToFloat64Free(ctx, &d2, op2))
            goto exception;
    handle_float64:
        if (is_math_mode(ctx) && is_safe_integer(d1) && is_safe_integer(d2))
            goto handle_bigint;
        switch(op) {
        case OP_sub:
            dr = d1 - d2;
            break;
        case OP_mul:
            dr = d1 * d2;
            break;
        case OP_div:
            dr = d1 / d2;
            break;
        case OP_mod:
            dr = fmod(d1, d2);
            break;
        case OP_math_mod:
            d2 = fabs(d2);
            dr = fmod(d1, d2);
            /* XXX: loss of accuracy if dr < 0 */
            if (dr < 0)
                dr += d2;
            break;
        case OP_pow:
            dr = js_pow(d1, d2);
            break;
        default:
            abort();
        }
        sp[-2] = __JS_NewFloat64(ctx, dr);
    }
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static no_inline __exception int js_add_slow(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2, res;
    uint32_t tag1, tag2;
    int ret;

    op1 = sp[-2];
    op2 = sp[-1];

    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    /* fast path for float64 */
    if (tag1 == JS_TAG_FLOAT64 && tag2 == JS_TAG_FLOAT64) {
        double d1, d2;
        d1 = JS_VALUE_GET_FLOAT64(op1);
        d2 = JS_VALUE_GET_FLOAT64(op2);
        sp[-2] = __JS_NewFloat64(ctx, d1 + d2);
        return 0;
    }

    if (tag1 == JS_TAG_OBJECT || tag2 == JS_TAG_OBJECT) {
        /* try to call an overloaded operator */
        if ((tag1 == JS_TAG_OBJECT &&
             (tag2 != JS_TAG_NULL && tag2 != JS_TAG_UNDEFINED &&
              tag2 != JS_TAG_STRING)) ||
            (tag2 == JS_TAG_OBJECT &&
             (tag1 != JS_TAG_NULL && tag1 != JS_TAG_UNDEFINED &&
              tag1 != JS_TAG_STRING))) {
            ret = js_call_binary_op_fallback(ctx, &res, op1, op2, OP_add,
                                             FALSE, HINT_NONE);
            if (ret != 0) {
                JS_FreeValue(ctx, op1);
                JS_FreeValue(ctx, op2);
                if (ret < 0) {
                    goto exception;
                } else {
                    sp[-2] = res;
                    return 0;
                }
            }
        }

        op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NONE);
        if (JS_IsException(op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }

        op2 = JS_ToPrimitiveFree(ctx, op2, HINT_NONE);
        if (JS_IsException(op2)) {
            JS_FreeValue(ctx, op1);
            goto exception;
        }
        tag1 = JS_VALUE_GET_NORM_TAG(op1);
        tag2 = JS_VALUE_GET_NORM_TAG(op2);
    }

    if (tag1 == JS_TAG_STRING || tag2 == JS_TAG_STRING) {
        sp[-2] = JS_ConcatString(ctx, op1, op2);
        if (JS_IsException(sp[-2]))
            goto exception;
        return 0;
    }

    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToNumericFree(ctx, op2);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);

    if (tag1 == JS_TAG_INT && tag2 == JS_TAG_INT) {
        int32_t v1, v2;
        int64_t v;
        v1 = JS_VALUE_GET_INT(op1);
        v2 = JS_VALUE_GET_INT(op2);
        v = (int64_t)v1 + (int64_t)v2;
        sp[-2] = JS_NewInt64(ctx, v);
    } else if (tag1 == JS_TAG_BIG_DECIMAL || tag2 == JS_TAG_BIG_DECIMAL) {
        if (ctx->rt->bigdecimal_ops.binary_arith(ctx, OP_add, sp - 2, op1, op2))
            goto exception;
    } else if (tag1 == JS_TAG_BIG_FLOAT || tag2 == JS_TAG_BIG_FLOAT) {
        if (ctx->rt->bigfloat_ops.binary_arith(ctx, OP_add, sp - 2, op1, op2))
            goto exception;
    } else if (tag1 == JS_TAG_BIG_INT || tag2 == JS_TAG_BIG_INT) {
    handle_bigint:
        if (ctx->rt->bigint_ops.binary_arith(ctx, OP_add, sp - 2, op1, op2))
            goto exception;
    } else {
        double d1, d2;
        /* float64 result */
        if (JS_ToFloat64Free(ctx, &d1, op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        if (JS_ToFloat64Free(ctx, &d2, op2))
            goto exception;
        if (is_math_mode(ctx) && is_safe_integer(d1) && is_safe_integer(d2))
            goto handle_bigint;
        sp[-2] = __JS_NewFloat64(ctx, d1 + d2);
    }
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static no_inline __exception int js_binary_logic_slow(JSContext *ctx,
                                                      JSValue *sp,
                                                      OPCodeEnum op)
{
    JSValue op1, op2, res;
    int ret;
    uint32_t tag1, tag2;
    uint32_t v1, v2, r;

    op1 = sp[-2];
    op2 = sp[-1];
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);

    /* try to call an overloaded operator */
    if ((tag1 == JS_TAG_OBJECT &&
         (tag2 != JS_TAG_NULL && tag2 != JS_TAG_UNDEFINED)) ||
        (tag2 == JS_TAG_OBJECT &&
         (tag1 != JS_TAG_NULL && tag1 != JS_TAG_UNDEFINED))) {
        ret = js_call_binary_op_fallback(ctx, &res, op1, op2, op, TRUE, 0);
        if (ret != 0) {
            JS_FreeValue(ctx, op1);
            JS_FreeValue(ctx, op2);
            if (ret < 0) {
                goto exception;
            } else {
                sp[-2] = res;
                return 0;
            }
        }
    }

    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToNumericFree(ctx, op2);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }

    if (is_math_mode(ctx))
        goto bigint_op;

    tag1 = JS_VALUE_GET_TAG(op1);
    tag2 = JS_VALUE_GET_TAG(op2);
    if (tag1 == JS_TAG_BIG_INT || tag2 == JS_TAG_BIG_INT) {
        if (tag1 != tag2) {
            JS_FreeValue(ctx, op1);
            JS_FreeValue(ctx, op2);
            JS_ThrowTypeError(ctx, "both operands must be bigint");
            goto exception;
        } else {
        bigint_op:
            if (ctx->rt->bigint_ops.binary_arith(ctx, op, sp - 2, op1, op2))
                goto exception;
        }
    } else {
        if (unlikely(JS_ToInt32Free(ctx, (int32_t *)&v1, op1))) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        if (unlikely(JS_ToInt32Free(ctx, (int32_t *)&v2, op2)))
            goto exception;
        switch(op) {
        case OP_shl:
            r = v1 << (v2 & 0x1f);
            break;
        case OP_sar:
            r = (int)v1 >> (v2 & 0x1f);
            break;
        case OP_and:
            r = v1 & v2;
            break;
        case OP_or:
            r = v1 | v2;
            break;
        case OP_xor:
            r = v1 ^ v2;
            break;
        default:
            abort();
        }
        sp[-2] = JS_NewInt32(ctx, r);
    }
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

/* Note: also used for bigint */
static int js_compare_bigfloat(JSContext *ctx, OPCodeEnum op,
                               JSValue op1, JSValue op2)
{
    bf_t a_s, b_s, *a, *b;
    int res;

    a = JS_ToBigFloat(ctx, &a_s, op1);
    if (!a) {
        JS_FreeValue(ctx, op2);
        return -1;
    }
    b = JS_ToBigFloat(ctx, &b_s, op2);
    if (!b) {
        if (a == &a_s)
            bf_delete(a);
        JS_FreeValue(ctx, op1);
        return -1;
    }
    switch(op) {
    case OP_lt:
        res = bf_cmp_lt(a, b); /* if NaN return false */
        break;
    case OP_lte:
        res = bf_cmp_le(a, b); /* if NaN return false */
        break;
    case OP_gt:
        res = bf_cmp_lt(b, a); /* if NaN return false */
        break;
    case OP_gte:
        res = bf_cmp_le(b, a); /* if NaN return false */
        break;
    case OP_eq:
        res = bf_cmp_eq(a, b); /* if NaN return false */
        break;
    default:
        abort();
    }
    if (a == &a_s)
        bf_delete(a);
    if (b == &b_s)
        bf_delete(b);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    return res;
}

static int js_compare_bigdecimal(JSContext *ctx, OPCodeEnum op,
                                 JSValue op1, JSValue op2)
{
    bfdec_t *a, *b;
    int res;

    /* Note: binary floats are converted to bigdecimal with
       toString(). It is not mathematically correct but is consistent
       with the BigDecimal() constructor behavior */
    op1 = JS_ToBigDecimalFree(ctx, op1, TRUE);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        return -1;
    }
    op2 = JS_ToBigDecimalFree(ctx, op2, TRUE);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        return -1;
    }
    a = JS_ToBigDecimal(ctx, op1);
    b = JS_ToBigDecimal(ctx, op2);

    switch(op) {
    case OP_lt:
        res = bfdec_cmp_lt(a, b); /* if NaN return false */
        break;
    case OP_lte:
        res = bfdec_cmp_le(a, b); /* if NaN return false */
        break;
    case OP_gt:
        res = bfdec_cmp_lt(b, a); /* if NaN return false */
        break;
    case OP_gte:
        res = bfdec_cmp_le(b, a); /* if NaN return false */
        break;
    case OP_eq:
        res = bfdec_cmp_eq(a, b); /* if NaN return false */
        break;
    default:
        abort();
    }
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    return res;
}

static no_inline int js_relational_slow(JSContext *ctx, JSValue *sp,
                                        OPCodeEnum op)
{
    JSValue op1, op2, ret;
    int res;
    uint32_t tag1, tag2;

    op1 = sp[-2];
    op2 = sp[-1];
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    /* try to call an overloaded operator */
    if ((tag1 == JS_TAG_OBJECT &&
         (tag2 != JS_TAG_NULL && tag2 != JS_TAG_UNDEFINED)) ||
        (tag2 == JS_TAG_OBJECT &&
         (tag1 != JS_TAG_NULL && tag1 != JS_TAG_UNDEFINED))) {
        res = js_call_binary_op_fallback(ctx, &ret, op1, op2, op,
                                         FALSE, HINT_NUMBER);
        if (res != 0) {
            JS_FreeValue(ctx, op1);
            JS_FreeValue(ctx, op2);
            if (res < 0) {
                goto exception;
            } else {
                sp[-2] = ret;
                return 0;
            }
        }
    }
    op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NUMBER);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToPrimitiveFree(ctx, op2, HINT_NUMBER);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);

    if (tag1 == JS_TAG_STRING && tag2 == JS_TAG_STRING) {
        JSString *p1, *p2;
        p1 = JS_VALUE_GET_STRING(op1);
        p2 = JS_VALUE_GET_STRING(op2);
        res = js_string_compare(ctx, p1, p2);
        switch(op) {
        case OP_lt:
            res = (res < 0);
            break;
        case OP_lte:
            res = (res <= 0);
            break;
        case OP_gt:
            res = (res > 0);
            break;
        default:
        case OP_gte:
            res = (res >= 0);
            break;
        }
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
    } else if ((tag1 <= JS_TAG_NULL || tag1 == JS_TAG_FLOAT64) &&
               (tag2 <= JS_TAG_NULL || tag2 == JS_TAG_FLOAT64)) {
        /* can use floating point comparison */
        double d1, d2;
        if (tag1 == JS_TAG_FLOAT64) {
            d1 = JS_VALUE_GET_FLOAT64(op1);
        } else {
            d1 = JS_VALUE_GET_INT(op1);
        }
        if (tag2 == JS_TAG_FLOAT64) {
            d2 = JS_VALUE_GET_FLOAT64(op2);
        } else {
            d2 = JS_VALUE_GET_INT(op2);
        }
        switch(op) {
        case OP_lt:
            res = (d1 < d2); /* if NaN return false */
            break;
        case OP_lte:
            res = (d1 <= d2); /* if NaN return false */
            break;
        case OP_gt:
            res = (d1 > d2); /* if NaN return false */
            break;
        default:
        case OP_gte:
            res = (d1 >= d2); /* if NaN return false */
            break;
        }
    } else {
        if (((tag1 == JS_TAG_BIG_INT && tag2 == JS_TAG_STRING) ||
             (tag2 == JS_TAG_BIG_INT && tag1 == JS_TAG_STRING)) &&
            !is_math_mode(ctx)) {
            if (tag1 == JS_TAG_STRING) {
                op1 = JS_StringToBigInt(ctx, op1);
                if (JS_VALUE_GET_TAG(op1) != JS_TAG_BIG_INT)
                    goto invalid_bigint_string;
            }
            if (tag2 == JS_TAG_STRING) {
                op2 = JS_StringToBigInt(ctx, op2);
                if (JS_VALUE_GET_TAG(op2) != JS_TAG_BIG_INT) {
                invalid_bigint_string:
                    JS_FreeValue(ctx, op1);
                    JS_FreeValue(ctx, op2);
                    res = FALSE;
                    goto done;
                }
            }
        } else {
            op1 = JS_ToNumericFree(ctx, op1);
            if (JS_IsException(op1)) {
                JS_FreeValue(ctx, op2);
                goto exception;
            }
            op2 = JS_ToNumericFree(ctx, op2);
            if (JS_IsException(op2)) {
                JS_FreeValue(ctx, op1);
                goto exception;
            }
        }

        if (JS_VALUE_GET_TAG(op1) == JS_TAG_BIG_DECIMAL ||
            JS_VALUE_GET_TAG(op2) == JS_TAG_BIG_DECIMAL) {
            res = ctx->rt->bigdecimal_ops.compare(ctx, op, op1, op2);
            if (res < 0)
                goto exception;
        } else if (JS_VALUE_GET_TAG(op1) == JS_TAG_BIG_FLOAT ||
                   JS_VALUE_GET_TAG(op2) == JS_TAG_BIG_FLOAT) {
            res = ctx->rt->bigfloat_ops.compare(ctx, op, op1, op2);
            if (res < 0)
                goto exception;
        } else {
            res = ctx->rt->bigint_ops.compare(ctx, op, op1, op2);
            if (res < 0)
                goto exception;
        }
    }
 done:
    sp[-2] = JS_NewBool(ctx, res);
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static BOOL tag_is_number(uint32_t tag)
{
    return (tag == JS_TAG_INT || tag == JS_TAG_BIG_INT ||
            tag == JS_TAG_FLOAT64 || tag == JS_TAG_BIG_FLOAT ||
            tag == JS_TAG_BIG_DECIMAL);
}

static no_inline __exception int js_eq_slow(JSContext *ctx, JSValue *sp,
                                            BOOL is_neq)
{
    JSValue op1, op2, ret;
    int res;
    uint32_t tag1, tag2;

    op1 = sp[-2];
    op2 = sp[-1];
 redo:
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    if (tag_is_number(tag1) && tag_is_number(tag2)) {
        if (tag1 == JS_TAG_INT && tag2 == JS_TAG_INT) {
            res = JS_VALUE_GET_INT(op1) == JS_VALUE_GET_INT(op2);
        } else if ((tag1 == JS_TAG_FLOAT64 &&
                    (tag2 == JS_TAG_INT || tag2 == JS_TAG_FLOAT64)) ||
                   (tag2 == JS_TAG_FLOAT64 &&
                    (tag1 == JS_TAG_INT || tag1 == JS_TAG_FLOAT64))) {
            double d1, d2;
            if (tag1 == JS_TAG_FLOAT64) {
                d1 = JS_VALUE_GET_FLOAT64(op1);
            } else {
                d1 = JS_VALUE_GET_INT(op1);
            }
            if (tag2 == JS_TAG_FLOAT64) {
                d2 = JS_VALUE_GET_FLOAT64(op2);
            } else {
                d2 = JS_VALUE_GET_INT(op2);
            }
            res = (d1 == d2);
        } else if (tag1 == JS_TAG_BIG_DECIMAL || tag2 == JS_TAG_BIG_DECIMAL) {
            res = ctx->rt->bigdecimal_ops.compare(ctx, OP_eq, op1, op2);
            if (res < 0)
                goto exception;
        } else if (tag1 == JS_TAG_BIG_FLOAT || tag2 == JS_TAG_BIG_FLOAT) {
            res = ctx->rt->bigfloat_ops.compare(ctx, OP_eq, op1, op2);
            if (res < 0)
                goto exception;
        } else {
            res = ctx->rt->bigint_ops.compare(ctx, OP_eq, op1, op2);
            if (res < 0)
                goto exception;
        }
    } else if (tag1 == tag2) {
        if (tag1 == JS_TAG_OBJECT) {
            /* try the fallback operator */
            res = js_call_binary_op_fallback(ctx, &ret, op1, op2,
                                             is_neq ? OP_neq : OP_eq,
                                             FALSE, HINT_NONE);
            if (res != 0) {
                JS_FreeValue(ctx, op1);
                JS_FreeValue(ctx, op2);
                if (res < 0) {
                    goto exception;
                } else {
                    sp[-2] = ret;
                    return 0;
                }
            }
        }
        res = js_strict_eq2(ctx, op1, op2, JS_EQ_STRICT);
    } else if ((tag1 == JS_TAG_NULL && tag2 == JS_TAG_UNDEFINED) ||
               (tag2 == JS_TAG_NULL && tag1 == JS_TAG_UNDEFINED)) {
        res = TRUE;
    } else if ((tag1 == JS_TAG_STRING && tag_is_number(tag2)) ||
               (tag2 == JS_TAG_STRING && tag_is_number(tag1))) {

        if ((tag1 == JS_TAG_BIG_INT || tag2 == JS_TAG_BIG_INT) &&
            !is_math_mode(ctx)) {
            if (tag1 == JS_TAG_STRING) {
                op1 = JS_StringToBigInt(ctx, op1);
                if (JS_VALUE_GET_TAG(op1) != JS_TAG_BIG_INT)
                    goto invalid_bigint_string;
            }
            if (tag2 == JS_TAG_STRING) {
                op2 = JS_StringToBigInt(ctx, op2);
                if (JS_VALUE_GET_TAG(op2) != JS_TAG_BIG_INT) {
                invalid_bigint_string:
                    JS_FreeValue(ctx, op1);
                    JS_FreeValue(ctx, op2);
                    res = FALSE;
                    goto done;
                }
            }
        } else {
            op1 = JS_ToNumericFree(ctx, op1);
            if (JS_IsException(op1)) {
                JS_FreeValue(ctx, op2);
                goto exception;
            }
            op2 = JS_ToNumericFree(ctx, op2);
            if (JS_IsException(op2)) {
                JS_FreeValue(ctx, op1);
                goto exception;
            }
        }
        res = js_strict_eq(ctx, op1, op2);
    } else if (tag1 == JS_TAG_BOOL) {
        op1 = JS_NewInt32(ctx, JS_VALUE_GET_INT(op1));
        goto redo;
    } else if (tag2 == JS_TAG_BOOL) {
        op2 = JS_NewInt32(ctx, JS_VALUE_GET_INT(op2));
        goto redo;
    } else if ((tag1 == JS_TAG_OBJECT &&
                (tag_is_number(tag2) || tag2 == JS_TAG_STRING || tag2 == JS_TAG_SYMBOL)) ||
               (tag2 == JS_TAG_OBJECT &&
                (tag_is_number(tag1) || tag1 == JS_TAG_STRING || tag1 == JS_TAG_SYMBOL))) {

        /* try the fallback operator */
        res = js_call_binary_op_fallback(ctx, &ret, op1, op2,
                                         is_neq ? OP_neq : OP_eq,
                                         FALSE, HINT_NONE);
        if (res != 0) {
            JS_FreeValue(ctx, op1);
            JS_FreeValue(ctx, op2);
            if (res < 0) {
                goto exception;
            } else {
                sp[-2] = ret;
                return 0;
            }
        }

        op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NONE);
        if (JS_IsException(op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        op2 = JS_ToPrimitiveFree(ctx, op2, HINT_NONE);
        if (JS_IsException(op2)) {
            JS_FreeValue(ctx, op1);
            goto exception;
        }
        goto redo;
    } else {
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
        res = FALSE;
    }
 done:
    sp[-2] = JS_NewBool(ctx, res ^ is_neq);
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static no_inline int js_shr_slow(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    uint32_t v1, v2, r;

    op1 = sp[-2];
    op2 = sp[-1];
    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToNumericFree(ctx, op2);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    /* XXX: could forbid >>> in bignum mode */
    if (!is_math_mode(ctx) &&
        (JS_VALUE_GET_TAG(op1) == JS_TAG_BIG_INT ||
         JS_VALUE_GET_TAG(op2) == JS_TAG_BIG_INT)) {
        JS_ThrowTypeError(ctx, "bigint operands are forbidden for >>>");
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    /* cannot give an exception */
    JS_ToUint32Free(ctx, &v1, op1);
    JS_ToUint32Free(ctx, &v2, op2);
    r = v1 >> (v2 & 0x1f);
    sp[-2] = JS_NewUint32(ctx, r);
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static JSValue js_mul_pow10_to_float64(JSContext *ctx, const bf_t *a,
                                       int64_t exponent)
{
    bf_t r_s, *r = &r_s;
    double d;
    int ret;

    /* always convert to Float64 */
    bf_init(ctx->bf_ctx, r);
    ret = bf_mul_pow_radix(r, a, 10, exponent,
                           53, bf_set_exp_bits(11) | BF_RNDN |
                           BF_FLAG_SUBNORMAL);
    bf_get_float64(r, &d, BF_RNDN);
    bf_delete(r);
    if (ret & BF_ST_MEM_ERROR)
        return JS_ThrowOutOfMemory(ctx);
    else
        return __JS_NewFloat64(ctx, d);
}

static no_inline int js_mul_pow10(JSContext *ctx, JSValue *sp)
{
    bf_t a_s, *a, *r;
    JSValue op1, op2, res;
    int64_t e;
    int ret;

    res = JS_NewBigFloat(ctx);
    if (JS_IsException(res))
        return -1;
    r = JS_GetBigFloat(res);
    op1 = sp[-2];
    op2 = sp[-1];
    a = JS_ToBigFloat(ctx, &a_s, op1);
    if (!a)
        return -1;
    if (JS_IsBigInt(ctx, op2)) {
        ret = JS_ToBigInt64(ctx, &e, op2);
    } else {
        ret = JS_ToInt64(ctx, &e, op2);
    }
    if (ret) {
        if (a == &a_s)
            bf_delete(a);
        JS_FreeValue(ctx, res);
        return -1;
    }

    bf_mul_pow_radix(r, a, 10, e, ctx->fp_env.prec, ctx->fp_env.flags);
    if (a == &a_s)
        bf_delete(a);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    sp[-2] = res;
    return 0;
}

#else /* !CONFIG_BIGNUM */

static JSValue JS_ThrowUnsupportedBigint(JSContext *ctx)
{
    return JS_ThrowTypeError(ctx, "bigint is not supported");
}

JSValue JS_NewBigInt64(JSContext *ctx, int64_t v)
{
    return JS_ThrowUnsupportedBigint(ctx);
}

JSValue JS_NewBigUint64(JSContext *ctx, uint64_t v)
{
    return JS_ThrowUnsupportedBigint(ctx);
}

int JS_ToBigInt64(JSContext *ctx, int64_t *pres, JSValueConst val)
{
    JS_ThrowUnsupportedBigint(ctx);
    *pres = 0;
    return -1;
}

static no_inline __exception int js_unary_arith_slow(JSContext *ctx,
                                                     JSValue *sp,
                                                     OPCodeEnum op)
{
    JSValue op1;
    double d;

    op1 = sp[-1];
    if (unlikely(JS_ToFloat64Free(ctx, &d, op1))) {
        sp[-1] = JS_UNDEFINED;
        return -1;
    }
    switch(op) {
        case OP_inc:
            d++;
            break;
        case OP_dec:
            d--;
            break;
        case OP_plus:
            break;
        case OP_neg:
            d = -d;
            break;
        default:
            abort();
    }
    sp[-1] = JS_NewFloat64(ctx, d);
    return 0;
}

/* specific case necessary for correct return value semantics */
static __exception int js_post_inc_slow(JSContext *ctx,
                                        JSValue *sp, OPCodeEnum op)
{
    JSValue op1;
    double d, r;

    op1 = sp[-1];
    if (unlikely(JS_ToFloat64Free(ctx, &d, op1))) {
        sp[-1] = JS_UNDEFINED;
        return -1;
    }
    r = d + 2 * (op - OP_post_dec) - 1;
    sp[0] = JS_NewFloat64(ctx, r);
    sp[-1] = JS_NewFloat64(ctx, d);
    return 0;
}

static no_inline __exception int js_binary_arith_slow(JSContext *ctx, JSValue *sp,
                                                      OPCodeEnum op)
{
    JSValue op1, op2;
    double d1, d2, r;

    op1 = sp[-2];
    op2 = sp[-1];
    if (unlikely(JS_ToFloat64Free(ctx, &d1, op1))) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    if (unlikely(JS_ToFloat64Free(ctx, &d2, op2))) {
        goto exception;
    }
    switch(op) {
        case OP_sub:
            r = d1 - d2;
            break;
        case OP_mul:
            r = d1 * d2;
            break;
        case OP_div:
            r = d1 / d2;
            break;
        case OP_mod:
            r = fmod(d1, d2);
            break;
        case OP_pow:
            r = js_pow(d1, d2);
            break;
        default:
            abort();
    }
    sp[-2] = JS_NewFloat64(ctx, r);
    return 0;
    exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static no_inline __exception int js_add_slow(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    uint32_t tag1, tag2;

    op1 = sp[-2];
    op2 = sp[-1];
    tag1 = JS_VALUE_GET_TAG(op1);
    tag2 = JS_VALUE_GET_TAG(op2);
    if ((tag1 == JS_TAG_INT || JS_TAG_IS_FLOAT64(tag1)) &&
        (tag2 == JS_TAG_INT || JS_TAG_IS_FLOAT64(tag2))) {
        goto add_numbers;
    } else {
        op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NONE);
        if (JS_IsException(op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        op2 = JS_ToPrimitiveFree(ctx, op2, HINT_NONE);
        if (JS_IsException(op2)) {
            JS_FreeValue(ctx, op1);
            goto exception;
        }
        tag1 = JS_VALUE_GET_TAG(op1);
        tag2 = JS_VALUE_GET_TAG(op2);
        if (tag1 == JS_TAG_STRING || tag2 == JS_TAG_STRING) {
            sp[-2] = JS_ConcatString(ctx, op1, op2);
            if (JS_IsException(sp[-2]))
                goto exception;
        } else {
            double d1, d2;
            add_numbers:
            if (JS_ToFloat64Free(ctx, &d1, op1)) {
                JS_FreeValue(ctx, op2);
                goto exception;
            }
            if (JS_ToFloat64Free(ctx, &d2, op2))
                goto exception;
            sp[-2] = JS_NewFloat64(ctx, d1 + d2);
        }
    }
    return 0;
    exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static no_inline __exception int js_binary_logic_slow(JSContext *ctx,
                                                      JSValue *sp,
                                                      OPCodeEnum op)
{
    JSValue op1, op2;
    uint32_t v1, v2, r;

    op1 = sp[-2];
    op2 = sp[-1];
    if (unlikely(JS_ToInt32Free(ctx, (int32_t *)&v1, op1))) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    if (unlikely(JS_ToInt32Free(ctx, (int32_t *)&v2, op2)))
        goto exception;
    switch(op) {
        case OP_shl:
            r = v1 << (v2 & 0x1f);
            break;
        case OP_sar:
            r = (int)v1 >> (v2 & 0x1f);
            break;
        case OP_and:
            r = v1 & v2;
            break;
        case OP_or:
            r = v1 | v2;
            break;
        case OP_xor:
            r = v1 ^ v2;
            break;
        default:
            abort();
    }
    sp[-2] = JS_NewInt32(ctx, r);
    return 0;
    exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static no_inline int js_not_slow(JSContext *ctx, JSValue *sp)
{
    int32_t v1;

    if (unlikely(JS_ToInt32Free(ctx, &v1, sp[-1]))) {
        sp[-1] = JS_UNDEFINED;
        return -1;
    }
    sp[-1] = JS_NewInt32(ctx, ~v1);
    return 0;
}

static no_inline int js_relational_slow(JSContext *ctx, JSValue *sp,
                                        OPCodeEnum op)
{
    JSValue op1, op2;
    int res;

    op1 = sp[-2];
    op2 = sp[-1];
    op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NUMBER);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToPrimitiveFree(ctx, op2, HINT_NUMBER);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    if (JS_VALUE_GET_TAG(op1) == JS_TAG_STRING &&
        JS_VALUE_GET_TAG(op2) == JS_TAG_STRING) {
        JSString *p1, *p2;
        p1 = JS_VALUE_GET_STRING(op1);
        p2 = JS_VALUE_GET_STRING(op2);
        res = js_string_compare(ctx, p1, p2);
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
        switch(op) {
            case OP_lt:
                res = (res < 0);
                break;
            case OP_lte:
                res = (res <= 0);
                break;
            case OP_gt:
                res = (res > 0);
                break;
            default:
            case OP_gte:
                res = (res >= 0);
                break;
        }
    } else {
        double d1, d2;
        if (JS_ToFloat64Free(ctx, &d1, op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        if (JS_ToFloat64Free(ctx, &d2, op2))
            goto exception;
        switch(op) {
            case OP_lt:
                res = (d1 < d2); /* if NaN return false */
                break;
            case OP_lte:
                res = (d1 <= d2); /* if NaN return false */
                break;
            case OP_gt:
                res = (d1 > d2); /* if NaN return false */
                break;
            default:
            case OP_gte:
                res = (d1 >= d2); /* if NaN return false */
                break;
        }
    }
    sp[-2] = JS_NewBool(ctx, res);
    return 0;
    exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static no_inline __exception int js_eq_slow(JSContext *ctx, JSValue *sp,
                                            BOOL is_neq)
{
    JSValue op1, op2;
    int tag1, tag2;
    BOOL res;

    op1 = sp[-2];
    op2 = sp[-1];
    redo:
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    if (tag1 == tag2 ||
        (tag1 == JS_TAG_INT && tag2 == JS_TAG_FLOAT64) ||
        (tag2 == JS_TAG_INT && tag1 == JS_TAG_FLOAT64)) {
        res = js_strict_eq(ctx, op1, op2);
    } else if ((tag1 == JS_TAG_NULL && tag2 == JS_TAG_UNDEFINED) ||
               (tag2 == JS_TAG_NULL && tag1 == JS_TAG_UNDEFINED)) {
        res = TRUE;
    } else if ((tag1 == JS_TAG_STRING && (tag2 == JS_TAG_INT ||
                                          tag2 == JS_TAG_FLOAT64)) ||
               (tag2 == JS_TAG_STRING && (tag1 == JS_TAG_INT ||
                                          tag1 == JS_TAG_FLOAT64))) {
        double d1;
        double d2;
        if (JS_ToFloat64Free(ctx, &d1, op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        if (JS_ToFloat64Free(ctx, &d2, op2))
            goto exception;
        res = (d1 == d2);
    } else if (tag1 == JS_TAG_BOOL) {
        op1 = JS_NewInt32(ctx, JS_VALUE_GET_INT(op1));
        goto redo;
    } else if (tag2 == JS_TAG_BOOL) {
        op2 = JS_NewInt32(ctx, JS_VALUE_GET_INT(op2));
        goto redo;
    } else if (tag1 == JS_TAG_OBJECT &&
               (tag2 == JS_TAG_INT || tag2 == JS_TAG_FLOAT64 || tag2 == JS_TAG_STRING || tag2 == JS_TAG_SYMBOL)) {
        op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NONE);
        if (JS_IsException(op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        goto redo;
    } else if (tag2 == JS_TAG_OBJECT &&
               (tag1 == JS_TAG_INT || tag1 == JS_TAG_FLOAT64 || tag1 == JS_TAG_STRING || tag1 == JS_TAG_SYMBOL)) {
        op2 = JS_ToPrimitiveFree(ctx, op2, HINT_NONE);
        if (JS_IsException(op2)) {
            JS_FreeValue(ctx, op1);
            goto exception;
        }
        goto redo;
    } else {
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
        res = FALSE;
    }
    sp[-2] = JS_NewBool(ctx, res ^ is_neq);
    return 0;
    exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static no_inline int js_shr_slow(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    uint32_t v1, v2, r;

    op1 = sp[-2];
    op2 = sp[-1];
    if (unlikely(JS_ToUint32Free(ctx, &v1, op1))) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    if (unlikely(JS_ToUint32Free(ctx, &v2, op2)))
        goto exception;
    r = v1 >> (v2 & 0x1f);
    sp[-2] = JS_NewUint32(ctx, r);
    return 0;
    exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

#endif /* !CONFIG_BIGNUM */

/* XXX: Should take JSValueConst arguments */
static BOOL js_strict_eq2(JSContext *ctx, JSValue op1, JSValue op2,
                          JSStrictEqModeEnum eq_mode)
{
    BOOL res;
    int tag1, tag2;
    double d1, d2;

    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    switch(tag1) {
        case JS_TAG_BOOL:
            if (tag1 != tag2) {
                res = FALSE;
            } else {
                res = JS_VALUE_GET_INT(op1) == JS_VALUE_GET_INT(op2);
                goto done_no_free;
            }
            break;
        case JS_TAG_NULL:
        case JS_TAG_UNDEFINED:
            res = (tag1 == tag2);
            break;
        case JS_TAG_STRING:
        {
            JSString *p1, *p2;
            if (tag1 != tag2) {
                res = FALSE;
            } else {
                p1 = JS_VALUE_GET_STRING(op1);
                p2 = JS_VALUE_GET_STRING(op2);
                res = (js_string_compare(ctx, p1, p2) == 0);
            }
        }
            break;
        case JS_TAG_SYMBOL:
        {
            JSAtomStruct *p1, *p2;
            if (tag1 != tag2) {
                res = FALSE;
            } else {
                p1 = JS_VALUE_GET_PTR(op1);
                p2 = JS_VALUE_GET_PTR(op2);
                res = (p1 == p2);
            }
        }
            break;
        case JS_TAG_OBJECT:
            if (tag1 != tag2)
                res = FALSE;
            else
                res = JS_VALUE_GET_OBJ(op1) == JS_VALUE_GET_OBJ(op2);
            break;
        case JS_TAG_INT:
            d1 = JS_VALUE_GET_INT(op1);
            if (tag2 == JS_TAG_INT) {
                d2 = JS_VALUE_GET_INT(op2);
                goto number_test;
            } else if (tag2 == JS_TAG_FLOAT64) {
                d2 = JS_VALUE_GET_FLOAT64(op2);
                goto number_test;
            } else {
                res = FALSE;
            }
            break;
        case JS_TAG_FLOAT64:
            d1 = JS_VALUE_GET_FLOAT64(op1);
            if (tag2 == JS_TAG_FLOAT64) {
                d2 = JS_VALUE_GET_FLOAT64(op2);
            } else if (tag2 == JS_TAG_INT) {
                d2 = JS_VALUE_GET_INT(op2);
            } else {
                res = FALSE;
                break;
            }
        number_test:
            if (unlikely(eq_mode >= JS_EQ_SAME_VALUE)) {
                JSFloat64Union u1, u2;
                /* NaN is not always normalized, so this test is necessary */
                if (isnan(d1) || isnan(d2)) {
                    res = isnan(d1) == isnan(d2);
                } else if (eq_mode == JS_EQ_SAME_VALUE_ZERO) {
                    res = (d1 == d2); /* +0 == -0 */
                } else {
                    u1.d = d1;
                    u2.d = d2;
                    res = (u1.u64 == u2.u64); /* +0 != -0 */
                }
            } else {
                res = (d1 == d2); /* if NaN return false and +0 == -0 */
            }
            goto done_no_free;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_INT:
        {
            bf_t a_s, *a, b_s, *b;
            if (tag1 != tag2) {
                res = FALSE;
                break;
            }
            a = JS_ToBigFloat(ctx, &a_s, op1);
            b = JS_ToBigFloat(ctx, &b_s, op2);
            res = bf_cmp_eq(a, b);
            if (a == &a_s)
                bf_delete(a);
            if (b == &b_s)
                bf_delete(b);
        }
        break;
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p1, *p2;
            const bf_t *a, *b;
            if (tag1 != tag2) {
                res = FALSE;
                break;
            }
            p1 = JS_VALUE_GET_PTR(op1);
            p2 = JS_VALUE_GET_PTR(op2);
            a = &p1->num;
            b = &p2->num;
            if (unlikely(eq_mode >= JS_EQ_SAME_VALUE)) {
                if (eq_mode == JS_EQ_SAME_VALUE_ZERO &&
                           a->expn == BF_EXP_ZERO && b->expn == BF_EXP_ZERO) {
                    res = TRUE;
                } else {
                    res = (bf_cmp_full(a, b) == 0);
                }
            } else {
                res = bf_cmp_eq(a, b);
            }
        }
        break;
    case JS_TAG_BIG_DECIMAL:
        {
            JSBigDecimal *p1, *p2;
            const bfdec_t *a, *b;
            if (tag1 != tag2) {
                res = FALSE;
                break;
            }
            p1 = JS_VALUE_GET_PTR(op1);
            p2 = JS_VALUE_GET_PTR(op2);
            a = &p1->num;
            b = &p2->num;
            res = bfdec_cmp_eq(a, b);
        }
        break;
#endif
        default:
            res = FALSE;
            break;
    }
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    done_no_free:
    return res;
}

static BOOL js_strict_eq(JSContext *ctx, JSValue op1, JSValue op2)
{
    return js_strict_eq2(ctx, op1, op2, JS_EQ_STRICT);
}

static BOOL js_same_value(JSContext *ctx, JSValueConst op1, JSValueConst op2)
{
    return js_strict_eq2(ctx,
                         JS_DupValue(ctx, op1), JS_DupValue(ctx, op2),
                         JS_EQ_SAME_VALUE);
}

static BOOL js_same_value_zero(JSContext *ctx, JSValueConst op1, JSValueConst op2)
{
    return js_strict_eq2(ctx,
                         JS_DupValue(ctx, op1), JS_DupValue(ctx, op2),
                         JS_EQ_SAME_VALUE_ZERO);
}

static no_inline int js_strict_eq_slow(JSContext *ctx, JSValue *sp,
                                       BOOL is_neq)
{
    BOOL res;
    res = js_strict_eq(ctx, sp[-2], sp[-1]);
    sp[-2] = JS_NewBool(ctx, res ^ is_neq);
    return 0;
}

static __exception int js_operator_in(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    JSAtom atom;
    int ret;

    op1 = sp[-2];
    op2 = sp[-1];

    if (JS_VALUE_GET_TAG(op2) != JS_TAG_OBJECT) {
        JS_ThrowTypeError(ctx, "invalid 'in' operand");
        return -1;
    }
    atom = JS_ValueToAtom(ctx, op1);
    if (unlikely(atom == JS_ATOM_NULL))
        return -1;
    ret = JS_HasProperty(ctx, op2, atom);
    JS_FreeAtom(ctx, atom);
    if (ret < 0)
        return -1;
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    sp[-2] = JS_NewBool(ctx, ret);
    return 0;
}

static __exception int js_has_unscopable(JSContext *ctx, JSValueConst obj,
                                         JSAtom atom)
{
    JSValue arr, val;
    int ret;

    arr = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_unscopables);
    if (JS_IsException(arr))
        return -1;
    ret = 0;
    if (JS_IsObject(arr)) {
        val = JS_GetProperty(ctx, arr, atom);
        ret = JS_ToBoolFree(ctx, val);
    }
    JS_FreeValue(ctx, arr);
    return ret;
}

static __exception int js_operator_instanceof(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    BOOL ret;

    op1 = sp[-2];
    op2 = sp[-1];
    ret = JS_IsInstanceOf(ctx, op1, op2);
    if (ret < 0)
        return ret;
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    sp[-2] = JS_NewBool(ctx, ret);
    return 0;
}

static __exception int js_operator_typeof(JSContext *ctx, JSValue op1)
{
    JSAtom atom;
    uint32_t tag;

    tag = JS_VALUE_GET_NORM_TAG(op1);
    switch(tag) {
#ifdef CONFIG_BIGNUM
        case JS_TAG_BIG_INT:
        atom = JS_ATOM_bigint;
        break;
    case JS_TAG_BIG_FLOAT:
        atom = JS_ATOM_bigfloat;
        break;
    case JS_TAG_BIG_DECIMAL:
        atom = JS_ATOM_bigdecimal;
        break;
#endif
        case JS_TAG_INT:
        case JS_TAG_FLOAT64:
            atom = JS_ATOM_number;
            break;
        case JS_TAG_UNDEFINED:
            atom = JS_ATOM_undefined;
            break;
        case JS_TAG_BOOL:
            atom = JS_ATOM_boolean;
            break;
        case JS_TAG_STRING:
            atom = JS_ATOM_string;
            break;
        case JS_TAG_OBJECT:
            if (JS_IsFunction(ctx, op1))
                atom = JS_ATOM_function;
            else
                goto obj_type;
            break;
        case JS_TAG_NULL:
        obj_type:
            atom = JS_ATOM_object;
            break;
        case JS_TAG_SYMBOL:
            atom = JS_ATOM_symbol;
            break;
        default:
            atom = JS_ATOM_unknown;
            break;
    }
    return atom;
}

static __exception int js_operator_delete(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    JSAtom atom;
    int ret;

    op1 = sp[-2];
    op2 = sp[-1];
    atom = JS_ValueToAtom(ctx, op2);
    if (unlikely(atom == JS_ATOM_NULL))
        return -1;
    ret = JS_DeleteProperty(ctx, op1, atom, JS_PROP_THROW_STRICT);
    JS_FreeAtom(ctx, atom);
    if (unlikely(ret < 0))
        return -1;
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    sp[-2] = JS_NewBool(ctx, ret);
    return 0;
}

static JSValue js_throw_type_error(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    return JS_ThrowTypeError(ctx, "invalid property access");
}

/* XXX: not 100% compatible, but mozilla seems to use a similar
   implementation to ensure that caller in non strict mode does not
   throw (ES5 compatibility) */
static JSValue js_function_proto_caller(JSContext *ctx, JSValueConst this_val,
                                        int argc, JSValueConst *argv)
{
    JSFunctionBytecode *b = JS_GetFunctionBytecode(this_val);
    if (!b || (b->js_mode & JS_MODE_STRICT) || !b->has_prototype) {
        return js_throw_type_error(ctx, this_val, 0, NULL);
    }
    return JS_UNDEFINED;
}

static JSValue js_function_proto_fileName(JSContext *ctx,
                                          JSValueConst this_val)
{
    JSFunctionBytecode *b = JS_GetFunctionBytecode(this_val);
    if (b && b->has_debug) {
        return JS_AtomToString(ctx, b->debug.filename);
    }
    return JS_UNDEFINED;
}

static JSValue js_function_proto_lineNumber(JSContext *ctx,
                                            JSValueConst this_val)
{
    JSFunctionBytecode *b = JS_GetFunctionBytecode(this_val);
    if (b && b->has_debug) {
        return JS_NewInt32(ctx, b->debug.line_num);
    }
    return JS_UNDEFINED;
}

static int js_arguments_define_own_property(JSContext *ctx,
                                            JSValueConst this_obj,
                                            JSAtom prop, JSValueConst val,
                                            JSValueConst getter, JSValueConst setter, int flags)
{
    JSObject *p;
    uint32_t idx;
    p = JS_VALUE_GET_OBJ(this_obj);
    /* convert to normal array when redefining an existing numeric field */
    if (p->fast_array && JS_AtomIsArrayIndex(ctx, &idx, prop) &&
        idx < p->u.array.count) {
        if (convert_fast_array_to_array(ctx, p))
            return -1;
    }
    /* run the default define own property */
    return JS_DefineProperty(ctx, this_obj, prop, val, getter, setter,
                             flags | JS_PROP_NO_EXOTIC);
}

static const JSClassExoticMethods js_arguments_exotic_methods = {
        .define_own_property = js_arguments_define_own_property,
};

static JSValue js_build_arguments(JSContext *ctx, int argc, JSValueConst *argv)
{
    JSValue val, *tab;
    JSProperty *pr;
    JSObject *p;
    int i;

    val = JS_NewObjectProtoClass(ctx, ctx->class_proto[JS_CLASS_OBJECT],
                                 JS_CLASS_ARGUMENTS);
    if (JS_IsException(val))
        return val;
    p = JS_VALUE_GET_OBJ(val);

    /* add the length field (cannot fail) */
    pr = add_property(ctx, p, JS_ATOM_length,
                      JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    pr->u.value = JS_NewInt32(ctx, argc);

    /* initialize the fast array part */
    tab = NULL;
    if (argc > 0) {
        tab = js_malloc(ctx, sizeof(tab[0]) * argc);
        if (!tab) {
            JS_FreeValue(ctx, val);
            return JS_EXCEPTION;
        }
        for(i = 0; i < argc; i++) {
            tab[i] = JS_DupValue(ctx, argv[i]);
        }
    }
    p->u.array.u.values = tab;
    p->u.array.count = argc;

    JS_DefinePropertyValue(ctx, val, JS_ATOM_Symbol_iterator,
                           JS_DupValue(ctx, ctx->array_proto_values),
                           JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE);
    /* add callee property to throw a TypeError in strict mode */
    JS_DefineProperty(ctx, val, JS_ATOM_callee, JS_UNDEFINED,
                      ctx->throw_type_error, ctx->throw_type_error,
                      JS_PROP_HAS_GET | JS_PROP_HAS_SET);
    return val;
}

#define GLOBAL_VAR_OFFSET 0x40000000
#define ARGUMENT_VAR_OFFSET 0x20000000

/* legacy arguments object: add references to the function arguments */
static JSValue js_build_mapped_arguments(JSContext *ctx, int argc,
                                         JSValueConst *argv,
                                         JSStackFrame *sf, int arg_count)
{
    JSValue val;
    JSProperty *pr;
    JSObject *p;
    int i;

    val = JS_NewObjectProtoClass(ctx, ctx->class_proto[JS_CLASS_OBJECT],
                                 JS_CLASS_MAPPED_ARGUMENTS);
    if (JS_IsException(val))
        return val;
    p = JS_VALUE_GET_OBJ(val);

    /* add the length field (cannot fail) */
    pr = add_property(ctx, p, JS_ATOM_length,
                      JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    pr->u.value = JS_NewInt32(ctx, argc);

    for(i = 0; i < arg_count; i++) {
        JSVarRef *var_ref;
        var_ref = get_var_ref(ctx, sf, i, TRUE);
        if (!var_ref)
            goto fail;
        pr = add_property(ctx, p, __JS_AtomFromUInt32(i), JS_PROP_C_W_E | JS_PROP_VARREF);
        if (!pr) {
            free_var_ref(ctx->rt, var_ref);
            goto fail;
        }
        pr->u.var_ref = var_ref;
    }

    /* the arguments not mapped to the arguments of the function can
       be normal properties */
    for(i = arg_count; i < argc; i++) {
        if (JS_DefinePropertyValueUint32(ctx, val, i,
                                         JS_DupValue(ctx, argv[i]),
                                         JS_PROP_C_W_E) < 0)
            goto fail;
    }

    JS_DefinePropertyValue(ctx, val, JS_ATOM_Symbol_iterator,
                           JS_DupValue(ctx, ctx->array_proto_values),
                           JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE);
    /* callee returns this function in non strict mode */
    JS_DefinePropertyValue(ctx, val, JS_ATOM_callee,
                           JS_DupValue(ctx, ctx->rt->current_stack_frame->cur_func),
                           JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE);
    return val;
    fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static JSValue js_build_rest(JSContext *ctx, int first, int argc, JSValueConst *argv)
{
    JSValue val;
    int i, ret;

    val = JS_NewArray(ctx);
    if (JS_IsException(val))
        return val;
    for (i = first; i < argc; i++) {
        ret = JS_DefinePropertyValueUint32(ctx, val, i - first,
                                           JS_DupValue(ctx, argv[i]),
                                           JS_PROP_C_W_E);
        if (ret < 0) {
            JS_FreeValue(ctx, val);
            return JS_EXCEPTION;
        }
    }
    return val;
}

static JSValue build_for_in_iterator(JSContext *ctx, JSValue obj)
{
    JSObject *p, *p1;
    JSPropertyEnum *tab_atom;
    int i;
    JSValue enum_obj;
    JSForInIterator *it;
    uint32_t tag, tab_atom_count;

    tag = JS_VALUE_GET_TAG(obj);
    if (tag != JS_TAG_OBJECT && tag != JS_TAG_NULL && tag != JS_TAG_UNDEFINED) {
        obj = JS_ToObjectFree(ctx, obj);
    }

    it = js_malloc(ctx, sizeof(*it));
    if (!it) {
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    enum_obj = JS_NewObjectProtoClass(ctx, JS_NULL, JS_CLASS_FOR_IN_ITERATOR);
    if (JS_IsException(enum_obj)) {
        js_free(ctx, it);
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    it->is_array = FALSE;
    it->obj = obj;
    it->idx = 0;
    p = JS_VALUE_GET_OBJ(enum_obj);
    p->u.for_in_iterator = it;

    if (tag == JS_TAG_NULL || tag == JS_TAG_UNDEFINED)
        return enum_obj;

    p = JS_VALUE_GET_OBJ(obj);

    /* fast path: assume no enumerable properties in the prototype chain */
    p1 = p->shape->proto;
    while (p1 != NULL) {
        if (JS_GetOwnPropertyNamesInternal(ctx, &tab_atom, &tab_atom_count, p1,
                                           JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY))
            goto fail;
        js_free_prop_enum(ctx, tab_atom, tab_atom_count);
        if (tab_atom_count != 0) {
            goto slow_path;
        }
        p1 = p1->shape->proto;
    }
    if (p->fast_array) {
        JSShape *sh;
        JSShapeProperty *prs;
        /* check that there are no enumerable normal fields */
        sh = p->shape;
        for(i = 0, prs = get_shape_prop(sh); i < sh->prop_count; i++, prs++) {
            if (prs->flags & JS_PROP_ENUMERABLE)
                goto normal_case;
        }
        /* the implicit GetOwnProperty raises an exception if the
           typed array is detached */
        if ((p->class_id >= JS_CLASS_UINT8C_ARRAY &&
             p->class_id <= JS_CLASS_FLOAT64_ARRAY) &&
            typed_array_is_detached(ctx, p) &&
            typed_array_get_length(ctx, p) != 0) {
            JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
            goto fail;
        }
        /* for fast arrays, we only store the number of elements */
        it->is_array = TRUE;
        it->array_length = p->u.array.count;
    } else {
        normal_case:
        if (JS_GetOwnPropertyNamesInternal(ctx, &tab_atom, &tab_atom_count, p,
                                           JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY))
            goto fail;
        for(i = 0; i < tab_atom_count; i++) {
            JS_SetPropertyInternal(ctx, enum_obj, tab_atom[i].atom, JS_NULL, 0);
        }
        js_free_prop_enum(ctx, tab_atom, tab_atom_count);
    }
    return enum_obj;

    slow_path:
    /* non enumerable properties hide the enumerables ones in the
       prototype chain */
    while (p != NULL) {
        if (JS_GetOwnPropertyNamesInternal(ctx, &tab_atom, &tab_atom_count, p,
                                           JS_GPN_STRING_MASK | JS_GPN_SET_ENUM))
            goto fail;
        for(i = 0; i < tab_atom_count; i++) {
            JS_DefinePropertyValue(ctx, enum_obj, tab_atom[i].atom, JS_NULL,
                                   (tab_atom[i].is_enumerable ?
                                    JS_PROP_ENUMERABLE : 0));
        }
        js_free_prop_enum(ctx, tab_atom, tab_atom_count);
        p = p->shape->proto;
    }
    return enum_obj;

    fail:
    JS_FreeValue(ctx, enum_obj);
    return JS_EXCEPTION;
}

/* obj -> enum_obj */
static __exception int js_for_in_start(JSContext *ctx, JSValue *sp)
{
    sp[-1] = build_for_in_iterator(ctx, sp[-1]);
    if (JS_IsException(sp[-1]))
        return -1;
    return 0;
}

/* enum_obj -> enum_obj value done */
static __exception int js_for_in_next(JSContext *ctx, JSValue *sp)
{
    JSValueConst enum_obj;
    JSObject *p;
    JSAtom prop;
    JSForInIterator *it;
    int ret;

    enum_obj = sp[-1];
    /* fail safe */
    if (JS_VALUE_GET_TAG(enum_obj) != JS_TAG_OBJECT)
        goto done;
    p = JS_VALUE_GET_OBJ(enum_obj);
    if (p->class_id != JS_CLASS_FOR_IN_ITERATOR)
        goto done;
    it = p->u.for_in_iterator;

    for(;;) {
        if (it->is_array) {
            if (it->idx >= it->array_length)
                goto done;
            prop = __JS_AtomFromUInt32(it->idx);
            it->idx++;
        } else {
            JSShape *sh = p->shape;
            JSShapeProperty *prs;
            if (it->idx >= sh->prop_count)
                goto done;
            prs = get_shape_prop(sh) + it->idx;
            prop = prs->atom;
            it->idx++;
            if (prop == JS_ATOM_NULL || !(prs->flags & JS_PROP_ENUMERABLE))
                continue;
        }
        /* check if the property was deleted */
        ret = JS_HasProperty(ctx, it->obj, prop);
        if (ret < 0)
            return ret;
        if (ret)
            break;
    }
    /* return the property */
    sp[0] = JS_AtomToValue(ctx, prop);
    sp[1] = JS_FALSE;
    return 0;
    done:
    /* return the end */
    sp[0] = JS_UNDEFINED;
    sp[1] = JS_TRUE;
    return 0;
}

static JSValue JS_GetIterator2(JSContext *ctx, JSValueConst obj,
                               JSValueConst method)
{
    JSValue enum_obj;

    enum_obj = JS_Call(ctx, method, obj, 0, NULL);
    if (JS_IsException(enum_obj))
        return enum_obj;
    if (!JS_IsObject(enum_obj)) {
        JS_FreeValue(ctx, enum_obj);
        return JS_ThrowTypeErrorNotAnObject(ctx);
    }
    return enum_obj;
}

static JSValue JS_GetIterator(JSContext *ctx, JSValueConst obj, BOOL is_async)
{
    JSValue method, ret, sync_iter;

    if (is_async) {
        method = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_asyncIterator);
        if (JS_IsException(method))
            return method;
        if (JS_IsUndefined(method) || JS_IsNull(method)) {
            method = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_iterator);
            if (JS_IsException(method))
                return method;
            sync_iter = JS_GetIterator2(ctx, obj, method);
            JS_FreeValue(ctx, method);
            if (JS_IsException(sync_iter))
                return sync_iter;
            ret = JS_CreateAsyncFromSyncIterator(ctx, sync_iter);
            JS_FreeValue(ctx, sync_iter);
            return ret;
        }
    } else {
        method = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_iterator);
        if (JS_IsException(method))
            return method;
    }
    if (!JS_IsFunction(ctx, method)) {
        JS_FreeValue(ctx, method);
        return JS_ThrowTypeError(ctx, "value is not iterable");
    }
    ret = JS_GetIterator2(ctx, obj, method);
    JS_FreeValue(ctx, method);
    return ret;
}

/* return *pdone = 2 if the iterator object is not parsed */
static JSValue JS_IteratorNext2(JSContext *ctx, JSValueConst enum_obj,
                                JSValueConst method,
                                int argc, JSValueConst *argv, int *pdone)
{
    JSValue obj;

    /* fast path for the built-in iterators (avoid creating the
       intermediate result object) */
    if (JS_IsObject(method)) {
        JSObject *p = JS_VALUE_GET_OBJ(method);
        if (p->class_id == JS_CLASS_C_FUNCTION &&
            p->u.cfunc.cproto == JS_CFUNC_iterator_next) {
            JSCFunctionType func;
            JSValueConst args[1];

            /* in case the function expects one argument */
            if (argc == 0) {
                args[0] = JS_UNDEFINED;
                argv = args;
            }
            func = p->u.cfunc.c_function;
            return func.iterator_next(ctx, enum_obj, argc, argv,
                                      pdone, p->u.cfunc.magic);
        }
    }
    obj = JS_Call(ctx, method, enum_obj, argc, argv);
    if (JS_IsException(obj))
        goto fail;
    if (!JS_IsObject(obj)) {
        JS_FreeValue(ctx, obj);
        JS_ThrowTypeError(ctx, "iterator must return an object");
        goto fail;
    }
    *pdone = 2;
    return obj;
    fail:
    *pdone = FALSE;
    return JS_EXCEPTION;
}

static JSValue JS_IteratorNext(JSContext *ctx, JSValueConst enum_obj,
                               JSValueConst method,
                               int argc, JSValueConst *argv, BOOL *pdone)
{
    JSValue obj, value, done_val;
    int done;

    obj = JS_IteratorNext2(ctx, enum_obj, method, argc, argv, &done);
    if (JS_IsException(obj))
        goto fail;
    if (done != 2) {
        *pdone = done;
        return obj;
    } else {
        done_val = JS_GetProperty(ctx, obj, JS_ATOM_done);
        if (JS_IsException(done_val))
            goto fail;
        *pdone = JS_ToBoolFree(ctx, done_val);
        value = JS_UNDEFINED;
        if (!*pdone) {
            value = JS_GetProperty(ctx, obj, JS_ATOM_value);
        }
        JS_FreeValue(ctx, obj);
        return value;
    }
    fail:
    JS_FreeValue(ctx, obj);
    *pdone = FALSE;
    return JS_EXCEPTION;
}

/* return < 0 in case of exception */
static int JS_IteratorClose(JSContext *ctx, JSValueConst enum_obj,
                            BOOL is_exception_pending)
{
    JSValue method, ret, ex_obj;
    int res;

    if (is_exception_pending) {
        ex_obj = ctx->rt->current_exception;
        ctx->rt->current_exception = JS_NULL;
        res = -1;
    } else {
        ex_obj = JS_UNDEFINED;
        res = 0;
    }
    method = JS_GetProperty(ctx, enum_obj, JS_ATOM_return);
    if (JS_IsException(method)) {
        res = -1;
        goto done;
    }
    if (JS_IsUndefined(method) || JS_IsNull(method)) {
        goto done;
    }
    ret = JS_CallFree(ctx, method, enum_obj, 0, NULL);
    if (!is_exception_pending) {
        if (JS_IsException(ret)) {
            res = -1;
        } else if (!JS_IsObject(ret)) {
            JS_ThrowTypeErrorNotAnObject(ctx);
            res = -1;
        }
    }
    JS_FreeValue(ctx, ret);
    done:
    if (is_exception_pending) {
        JS_Throw(ctx, ex_obj);
    }
    return res;
}

/* obj -> enum_rec (3 slots) */
static __exception int js_for_of_start(JSContext *ctx, JSValue *sp,
                                       BOOL is_async)
{
    JSValue op1, obj, method;
    op1 = sp[-1];
    obj = JS_GetIterator(ctx, op1, is_async);
    if (JS_IsException(obj))
        return -1;
    JS_FreeValue(ctx, op1);
    sp[-1] = obj;
    method = JS_GetProperty(ctx, obj, JS_ATOM_next);
    if (JS_IsException(method))
        return -1;
    sp[0] = method;
    return 0;
}

/* enum_rec -> enum_rec value done */
static __exception int js_for_of_next(JSContext *ctx, JSValue *sp, int offset)
{
    JSValue value = JS_UNDEFINED;
    int done = 1;

    if (likely(!JS_IsUndefined(sp[offset]))) {
        value = JS_IteratorNext(ctx, sp[offset], sp[offset + 1], 0, NULL, &done);
        if (JS_IsException(value))
            done = -1;
        if (done) {
            /* value is JS_UNDEFINED or JS_EXCEPTION */
            /* replace the iteration object with undefined */
            JS_FreeValue(ctx, sp[offset]);
            sp[offset] = JS_UNDEFINED;
            if (done < 0)
                return -1;
        }
    }
    sp[0] = value;
    sp[1] = JS_NewBool(ctx, done);
    return 0;
}

static __exception int js_for_await_of_next(JSContext *ctx, JSValue *sp)
{
    JSValue result;
    result = JS_Call(ctx, sp[-2], sp[-3], 0, NULL);
    if (JS_IsException(result))
        return -1;
    sp[0] = result;
    return 0;
}

static JSValue JS_IteratorGetCompleteValue(JSContext *ctx, JSValueConst obj,
                                           BOOL *pdone)
{
    JSValue done_val, value;
    BOOL done;
    done_val = JS_GetProperty(ctx, obj, JS_ATOM_done);
    if (JS_IsException(done_val))
        goto fail;
    done = JS_ToBoolFree(ctx, done_val);
    value = JS_GetProperty(ctx, obj, JS_ATOM_value);
    if (JS_IsException(value))
        goto fail;
    *pdone = done;
    return value;
    fail:
    *pdone = FALSE;
    return JS_EXCEPTION;
}

static __exception int js_iterator_get_value_done(JSContext *ctx, JSValue *sp)
{
    JSValue obj, value;
    BOOL done;
    obj = sp[-1];
    if (!JS_IsObject(obj)) {
        JS_ThrowTypeError(ctx, "iterator must return an object");
        return -1;
    }
    value = JS_IteratorGetCompleteValue(ctx, obj, &done);
    if (JS_IsException(value))
        return -1;
    JS_FreeValue(ctx, obj);
    sp[-1] = value;
    sp[0] = JS_NewBool(ctx, done);
    return 0;
}

static JSValue js_create_iterator_result(JSContext *ctx,
                                         JSValue val,
                                         BOOL done)
{
    JSValue obj;
    obj = JS_NewObject(ctx);
    if (JS_IsException(obj)) {
        JS_FreeValue(ctx, val);
        return obj;
    }
    if (JS_DefinePropertyValue(ctx, obj, JS_ATOM_value,
                               val, JS_PROP_C_W_E) < 0) {
        goto fail;
    }
    if (JS_DefinePropertyValue(ctx, obj, JS_ATOM_done,
                               JS_NewBool(ctx, done), JS_PROP_C_W_E) < 0) {
        fail:
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    return obj;
}

static JSValue js_array_iterator_next(JSContext *ctx, JSValueConst this_val,
                                      int argc, JSValueConst *argv,
                                      BOOL *pdone, int magic);

static JSValue js_create_array_iterator(JSContext *ctx, JSValueConst this_val,
                                        int argc, JSValueConst *argv, int magic);

static BOOL js_is_fast_array(JSContext *ctx, JSValueConst obj)
{
    /* Try and handle fast arrays explicitly */
    if (JS_VALUE_GET_TAG(obj) == JS_TAG_OBJECT) {
        JSObject *p = JS_VALUE_GET_OBJ(obj);
        if (p->class_id == JS_CLASS_ARRAY && p->fast_array) {
            return TRUE;
        }
    }
    return FALSE;
}

/* Access an Array's internal JSValue array if available */
static BOOL js_get_fast_array(JSContext *ctx, JSValueConst obj,
                              JSValue **arrpp, uint32_t *countp)
{
    /* Try and handle fast arrays explicitly */
    if (JS_VALUE_GET_TAG(obj) == JS_TAG_OBJECT) {
        JSObject *p = JS_VALUE_GET_OBJ(obj);
        if (p->class_id == JS_CLASS_ARRAY && p->fast_array) {
            *countp = p->u.array.count;
            *arrpp = p->u.array.u.values;
            return TRUE;
        }
    }
    return FALSE;
}

static __exception int js_append_enumerate(JSContext *ctx, JSValue *sp)
{
    JSValue iterator, enumobj, method, value;
    int pos, is_array_iterator;
    JSValue *arrp;
    uint32_t i, count32;

    if (JS_VALUE_GET_TAG(sp[-2]) != JS_TAG_INT) {
        JS_ThrowInternalError(ctx, "invalid index for append");
        return -1;
    }

    pos = JS_VALUE_GET_INT(sp[-2]);

    /* XXX: further optimisations:
       - use ctx->array_proto_values?
       - check if array_iterator_prototype next method is built-in and
         avoid constructing actual iterator object?
       - build this into js_for_of_start and use in all `for (x of o)` loops
     */
    iterator = JS_GetProperty(ctx, sp[-1], JS_ATOM_Symbol_iterator);
    if (JS_IsException(iterator))
        return -1;
    is_array_iterator = JS_IsCFunction(ctx, iterator,
                                       (JSCFunction *)js_create_array_iterator,
                                       JS_ITERATOR_KIND_VALUE);
    JS_FreeValue(ctx, iterator);

    enumobj = JS_GetIterator(ctx, sp[-1], FALSE);
    if (JS_IsException(enumobj))
        return -1;
    method = JS_GetProperty(ctx, enumobj, JS_ATOM_next);
    if (JS_IsException(method)) {
        JS_FreeValue(ctx, enumobj);
        return -1;
    }
    if (is_array_iterator
        &&  JS_IsCFunction(ctx, method, (JSCFunction *)js_array_iterator_next, 0)
        &&  js_get_fast_array(ctx, sp[-1], &arrp, &count32)) {
        int64_t len;
        /* Handle fast arrays explicitly */
        if (js_get_length64(ctx, &len, sp[-1]))
            goto exception;
        for (i = 0; i < count32; i++) {
            if (JS_DefinePropertyValueUint32(ctx, sp[-3], pos++,
                                             JS_DupValue(ctx, arrp[i]), JS_PROP_C_W_E) < 0)
                goto exception;
        }
        if (len > count32) {
            /* This is not strictly correct because the trailing elements are
               empty instead of undefined. Append undefined entries instead.
             */
            pos += len - count32;
            if (JS_SetProperty(ctx, sp[-3], JS_ATOM_length, JS_NewUint32(ctx, pos)) < 0)
                goto exception;
        }
    } else {
        for (;;) {
            BOOL done;
            value = JS_IteratorNext(ctx, enumobj, method, 0, NULL, &done);
            if (JS_IsException(value))
                goto exception;
            if (done) {
                /* value is JS_UNDEFINED */
                break;
            }
            if (JS_DefinePropertyValueUint32(ctx, sp[-3], pos++, value, JS_PROP_C_W_E) < 0)
                goto exception;
        }
    }
    sp[-2] = JS_NewInt32(ctx, pos);
    JS_FreeValue(ctx, enumobj);
    JS_FreeValue(ctx, method);
    return 0;

    exception:
    JS_IteratorClose(ctx, enumobj, TRUE);
    JS_FreeValue(ctx, enumobj);
    JS_FreeValue(ctx, method);
    return -1;
}

static __exception int JS_CopyDataProperties(JSContext *ctx,
                                             JSValueConst target,
                                             JSValueConst source,
                                             JSValueConst excluded,
                                             BOOL setprop)
{
    JSPropertyEnum *tab_atom;
    JSValue val;
    uint32_t i, tab_atom_count;
    JSObject *p;
    JSObject *pexcl = NULL;
    int ret = 0, flags;

    if (JS_VALUE_GET_TAG(source) != JS_TAG_OBJECT)
        return 0;

    if (JS_VALUE_GET_TAG(excluded) == JS_TAG_OBJECT)
        pexcl = JS_VALUE_GET_OBJ(excluded);

    p = JS_VALUE_GET_OBJ(source);
    if (JS_GetOwnPropertyNamesInternal(ctx, &tab_atom, &tab_atom_count, p,
                                       JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK |
                                       JS_GPN_ENUM_ONLY))
        return -1;

    flags = JS_PROP_C_W_E;

    for (i = 0; i < tab_atom_count; i++) {
        if (pexcl) {
            ret = JS_GetOwnPropertyInternal(ctx, NULL, pexcl, tab_atom[i].atom);
            if (ret) {
                if (ret < 0)
                    break;
                ret = 0;
                continue;
            }
        }
        ret = -1;
        val = JS_GetProperty(ctx, source, tab_atom[i].atom);
        if (JS_IsException(val))
            break;
        if (setprop)
            ret = JS_SetProperty(ctx, target, tab_atom[i].atom, val);
        else
            ret = JS_DefinePropertyValue(ctx, target, tab_atom[i].atom, val, flags);
        if (ret < 0)
            break;
        ret = 0;
    }
    js_free_prop_enum(ctx, tab_atom, tab_atom_count);
    return ret;
}

/* only valid inside C functions */
static JSValueConst JS_GetActiveFunction(JSContext *ctx)
{
    return ctx->rt->current_stack_frame->cur_func;
}

static JSVarRef *get_var_ref(JSContext *ctx, JSStackFrame *sf,
                             int var_idx, BOOL is_arg)
{
    JSVarRef *var_ref;
    struct list_head *el;

    list_for_each(el, &sf->var_ref_list) {
        var_ref = list_entry(el, JSVarRef, header.link);
        if (var_ref->var_idx == var_idx && var_ref->is_arg == is_arg) {
            var_ref->header.ref_count++;
            return var_ref;
        }
    }
    /* create a new one */
    var_ref = js_malloc(ctx, sizeof(JSVarRef));
    if (!var_ref)
        return NULL;
    var_ref->header.ref_count = 1;
    var_ref->is_detached = FALSE;
    var_ref->is_arg = is_arg;
    var_ref->var_idx = var_idx;
    list_add_tail(&var_ref->header.link, &sf->var_ref_list);
    if (is_arg)
        var_ref->pvalue = &sf->arg_buf[var_idx];
    else
        var_ref->pvalue = &sf->var_buf[var_idx];
    var_ref->value = JS_UNDEFINED;
    return var_ref;
}

static JSValue js_closure2(JSContext *ctx, JSValue func_obj,
                           JSFunctionBytecode *b,
                           JSVarRef **cur_var_refs,
                           JSStackFrame *sf)
{
    JSObject *p;
    JSVarRef **var_refs;
    int i;

    p = JS_VALUE_GET_OBJ(func_obj);
    p->u.func.function_bytecode = b;
    p->u.func.home_object = NULL;
    p->u.func.var_refs = NULL;
    if (b->closure_var_count) {
        var_refs = js_mallocz(ctx, sizeof(var_refs[0]) * b->closure_var_count);
        if (!var_refs)
            goto fail;
        p->u.func.var_refs = var_refs;
        for(i = 0; i < b->closure_var_count; i++) {
            JSClosureVar *cv = &b->closure_var[i];
            JSVarRef *var_ref;
            if (cv->is_local) {
                /* reuse the existing variable reference if it already exists */
                var_ref = get_var_ref(ctx, sf, cv->var_idx, cv->is_arg);
                if (!var_ref)
                    goto fail;
            } else {
                var_ref = cur_var_refs[cv->var_idx];
                var_ref->header.ref_count++;
            }
            var_refs[i] = var_ref;
        }
    }
    return func_obj;
    fail:
    /* bfunc is freed when func_obj is freed */
    JS_FreeValue(ctx, func_obj);
    return JS_EXCEPTION;
}

static int js_instantiate_prototype(JSContext *ctx, JSObject *p, JSAtom atom, void *opaque)
{
    JSValue obj, this_val;
    int ret;

    this_val = JS_MKPTR(JS_TAG_OBJECT, p);
    obj = JS_NewObject(ctx);
    if (JS_IsException(obj))
        return -1;
    set_cycle_flag(ctx, obj);
    set_cycle_flag(ctx, this_val);
    ret = JS_DefinePropertyValue(ctx, obj, JS_ATOM_constructor,
                                 JS_DupValue(ctx, this_val),
                                 JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    if (JS_DefinePropertyValue(ctx, this_val, atom, obj, JS_PROP_WRITABLE) < 0 || ret < 0)
        return -1;
    return 0;
}

static const uint16_t func_kind_to_class_id[] = {
        [JS_FUNC_NORMAL] = JS_CLASS_BYTECODE_FUNCTION,
        [JS_FUNC_GENERATOR] = JS_CLASS_GENERATOR_FUNCTION,
        [JS_FUNC_ASYNC] = JS_CLASS_ASYNC_FUNCTION,
        [JS_FUNC_ASYNC_GENERATOR] = JS_CLASS_ASYNC_GENERATOR_FUNCTION,
};

static JSValue js_closure(JSContext *ctx, JSValue bfunc,
                          JSVarRef **cur_var_refs,
                          JSStackFrame *sf)
{
    JSFunctionBytecode *b;
    JSValue func_obj;
    JSAtom name_atom;

    b = JS_VALUE_GET_PTR(bfunc);
    func_obj = JS_NewObjectClass(ctx, func_kind_to_class_id[b->func_kind]);
    if (JS_IsException(func_obj)) {
        JS_FreeValue(ctx, bfunc);
        return JS_EXCEPTION;
    }
    func_obj = js_closure2(ctx, func_obj, b, cur_var_refs, sf);
    if (JS_IsException(func_obj)) {
        /* bfunc has been freed */
        goto fail;
    }
    name_atom = b->func_name;
    if (name_atom == JS_ATOM_NULL)
        name_atom = JS_ATOM_empty_string;
    js_function_set_properties(ctx, func_obj, name_atom,
                               b->defined_arg_count);

    if (b->func_kind & JS_FUNC_GENERATOR) {
        JSValue proto;
        int proto_class_id;
        /* generators have a prototype field which is used as
           prototype for the generator object */
        if (b->func_kind == JS_FUNC_ASYNC_GENERATOR)
            proto_class_id = JS_CLASS_ASYNC_GENERATOR;
        else
            proto_class_id = JS_CLASS_GENERATOR;
        proto = JS_NewObjectProto(ctx, ctx->class_proto[proto_class_id]);
        if (JS_IsException(proto))
            goto fail;
        JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_prototype, proto,
                               JS_PROP_WRITABLE);
    } else if (b->has_prototype) {
        /* add the 'prototype' property: delay instantiation to avoid
           creating cycles for every javascript function. The prototype
           object is created on the fly when first accessed */
        JS_SetConstructorBit(ctx, func_obj, TRUE);
        JS_DefineAutoInitProperty(ctx, func_obj, JS_ATOM_prototype,
                                  JS_AUTOINIT_ID_PROTOTYPE, NULL,
                                  JS_PROP_WRITABLE);
    }
    return func_obj;
    fail:
    /* bfunc is freed when func_obj is freed */
    JS_FreeValue(ctx, func_obj);
    return JS_EXCEPTION;
}

#define JS_DEFINE_CLASS_HAS_HERITAGE     (1 << 0)

static int js_op_define_class(JSContext *ctx, JSValue *sp,
                              JSAtom class_name, int class_flags,
                              JSVarRef **cur_var_refs,
                              JSStackFrame *sf, BOOL is_computed_name)
{
    JSValue bfunc, parent_class, proto = JS_UNDEFINED;
    JSValue ctor = JS_UNDEFINED, parent_proto = JS_UNDEFINED;
    JSFunctionBytecode *b;

    parent_class = sp[-2];
    bfunc = sp[-1];

    if (class_flags & JS_DEFINE_CLASS_HAS_HERITAGE) {
        if (JS_IsNull(parent_class)) {
            parent_proto = JS_NULL;
            parent_class = JS_DupValue(ctx, ctx->function_proto);
        } else {
            if (!JS_IsConstructor(ctx, parent_class)) {
                JS_ThrowTypeError(ctx, "parent class must be constructor");
                goto fail;
            }
            parent_proto = JS_GetProperty(ctx, parent_class, JS_ATOM_prototype);
            if (JS_IsException(parent_proto))
                goto fail;
            if (!JS_IsNull(parent_proto) && !JS_IsObject(parent_proto)) {
                JS_ThrowTypeError(ctx, "parent prototype must be an object or null");
                goto fail;
            }
        }
    } else {
        /* parent_class is JS_UNDEFINED in this case */
        parent_proto = JS_DupValue(ctx, ctx->class_proto[JS_CLASS_OBJECT]);
        parent_class = JS_DupValue(ctx, ctx->function_proto);
    }
    proto = JS_NewObjectProto(ctx, parent_proto);
    if (JS_IsException(proto))
        goto fail;

    b = JS_VALUE_GET_PTR(bfunc);
    assert(b->func_kind == JS_FUNC_NORMAL);
    ctor = JS_NewObjectProtoClass(ctx, parent_class,
                                  JS_CLASS_BYTECODE_FUNCTION);
    if (JS_IsException(ctor))
        goto fail;
    ctor = js_closure2(ctx, ctor, b, cur_var_refs, sf);
    bfunc = JS_UNDEFINED;
    if (JS_IsException(ctor))
        goto fail;
    js_method_set_home_object(ctx, ctor, proto);
    JS_SetConstructorBit(ctx, ctor, TRUE);

    JS_DefinePropertyValue(ctx, ctor, JS_ATOM_length,
                           JS_NewInt32(ctx, b->defined_arg_count),
                           JS_PROP_CONFIGURABLE);

    if (is_computed_name) {
        if (JS_DefineObjectNameComputed(ctx, ctor, sp[-3],
                                        JS_PROP_CONFIGURABLE) < 0)
            goto fail;
    } else {
        if (JS_DefineObjectName(ctx, ctor, class_name, JS_PROP_CONFIGURABLE) < 0)
            goto fail;
    }

    /* the constructor property must be first. It can be overriden by
       computed property names */
    if (JS_DefinePropertyValue(ctx, proto, JS_ATOM_constructor,
                               JS_DupValue(ctx, ctor),
                               JS_PROP_CONFIGURABLE |
                               JS_PROP_WRITABLE | JS_PROP_THROW) < 0)
        goto fail;
    /* set the prototype property */
    if (JS_DefinePropertyValue(ctx, ctor, JS_ATOM_prototype,
                               JS_DupValue(ctx, proto), JS_PROP_THROW) < 0)
        goto fail;
    set_cycle_flag(ctx, ctor);
    set_cycle_flag(ctx, proto);

    JS_FreeValue(ctx, parent_proto);
    JS_FreeValue(ctx, parent_class);

    sp[-2] = ctor;
    sp[-1] = proto;
    return 0;
    fail:
    JS_FreeValue(ctx, parent_class);
    JS_FreeValue(ctx, parent_proto);
    JS_FreeValue(ctx, bfunc);
    JS_FreeValue(ctx, proto);
    JS_FreeValue(ctx, ctor);
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static void close_var_refs(JSRuntime *rt, JSStackFrame *sf)
{
    struct list_head *el, *el1;
    JSVarRef *var_ref;
    int var_idx;

    list_for_each_safe(el, el1, &sf->var_ref_list) {
        var_ref = list_entry(el, JSVarRef, header.link);
        var_idx = var_ref->var_idx;
        if (var_ref->is_arg)
            var_ref->value = JS_DupValueRT(rt, sf->arg_buf[var_idx]);
        else
            var_ref->value = JS_DupValueRT(rt, sf->var_buf[var_idx]);
        var_ref->pvalue = &var_ref->value;
        /* the reference is no longer to a local variable */
        var_ref->is_detached = TRUE;
        add_gc_object(rt, &var_ref->header, JS_GC_OBJ_TYPE_VAR_REF);
    }
}

static void close_lexical_var(JSContext *ctx, JSStackFrame *sf, int idx, int is_arg)
{
    struct list_head *el, *el1;
    JSVarRef *var_ref;
    int var_idx = idx;

    list_for_each_safe(el, el1, &sf->var_ref_list) {
        var_ref = list_entry(el, JSVarRef, header.link);
        if (var_idx == var_ref->var_idx && var_ref->is_arg == is_arg) {
            var_ref->value = JS_DupValue(ctx, sf->var_buf[var_idx]);
            var_ref->pvalue = &var_ref->value;
            list_del(&var_ref->header.link);
            /* the reference is no longer to a local variable */
            var_ref->is_detached = TRUE;
            add_gc_object(ctx->rt, &var_ref->header, JS_GC_OBJ_TYPE_VAR_REF);
        }
    }
}

#define JS_CALL_FLAG_COPY_ARGV   (1 << 1)
#define JS_CALL_FLAG_GENERATOR   (1 << 2)

static JSValue js_call_c_function(JSContext *ctx, JSValueConst func_obj,
                                  JSValueConst this_obj,
                                  int argc, JSValueConst *argv, int flags)
{
    JSRuntime *rt = ctx->rt;
    JSCFunctionType func;
    JSObject *p;
    JSStackFrame sf_s, *sf = &sf_s, *prev_sf;
    JSValue ret_val;
    JSValueConst *arg_buf;
    int arg_count, i;
    JSCFunctionEnum cproto;

    p = JS_VALUE_GET_OBJ(func_obj);
    cproto = p->u.cfunc.cproto;
    arg_count = p->u.cfunc.length;

    /* better to always check stack overflow */
    if (js_check_stack_overflow(rt, sizeof(arg_buf[0]) * arg_count))
        return JS_ThrowStackOverflow(ctx);

    prev_sf = rt->current_stack_frame;
    sf->prev_frame = prev_sf;
    rt->current_stack_frame = sf;
    ctx = p->u.cfunc.realm; /* change the current realm */

#ifdef CONFIG_BIGNUM
    /* we only propagate the bignum mode as some runtime functions
       test it */
    if (prev_sf)
        sf->js_mode = prev_sf->js_mode & JS_MODE_MATH;
    else
        sf->js_mode = 0;
#else
    sf->js_mode = 0;
#endif
    sf->cur_func = func_obj;
    sf->arg_count = argc;
    arg_buf = argv;

    if (unlikely(argc < arg_count)) {
        /* ensure that at least argc_count arguments are readable */
        arg_buf = alloca(sizeof(arg_buf[0]) * arg_count);
        for(i = 0; i < argc; i++)
            arg_buf[i] = argv[i];
        for(i = argc; i < arg_count; i++)
            arg_buf[i] = JS_UNDEFINED;
        sf->arg_count = arg_count;
    }
    sf->arg_buf = (JSValue*)arg_buf;

    func = p->u.cfunc.c_function;
    switch(cproto) {
        case JS_CFUNC_constructor:
        case JS_CFUNC_constructor_or_func:
            if (!(flags & JS_CALL_FLAG_CONSTRUCTOR)) {
                if (cproto == JS_CFUNC_constructor) {
                    not_a_constructor:
                    ret_val = JS_ThrowTypeError(ctx, "must be called with new");
                    break;
                } else {
                    this_obj = JS_UNDEFINED;
                }
            }
            /* here this_obj is new_target */
            /* fall thru */
        case JS_CFUNC_generic:
            ret_val = func.generic(ctx, this_obj, argc, arg_buf);
            break;
        case JS_CFUNC_constructor_magic:
        case JS_CFUNC_constructor_or_func_magic:
            if (!(flags & JS_CALL_FLAG_CONSTRUCTOR)) {
                if (cproto == JS_CFUNC_constructor_magic) {
                    goto not_a_constructor;
                } else {
                    this_obj = JS_UNDEFINED;
                }
            }
            /* fall thru */
        case JS_CFUNC_generic_magic:
            ret_val = func.generic_magic(ctx, this_obj, argc, arg_buf,
                                         p->u.cfunc.magic);
            break;
        case JS_CFUNC_getter:
            ret_val = func.getter(ctx, this_obj);
            break;
        case JS_CFUNC_setter:
            ret_val = func.setter(ctx, this_obj, arg_buf[0]);
            break;
        case JS_CFUNC_getter_magic:
            ret_val = func.getter_magic(ctx, this_obj, p->u.cfunc.magic);
            break;
        case JS_CFUNC_setter_magic:
            ret_val = func.setter_magic(ctx, this_obj, arg_buf[0], p->u.cfunc.magic);
            break;
        case JS_CFUNC_f_f:
        {
            double d1;

            if (unlikely(JS_ToFloat64(ctx, &d1, arg_buf[0]))) {
                ret_val = JS_EXCEPTION;
                break;
            }
            ret_val = JS_NewFloat64(ctx, func.f_f(d1));
        }
            break;
        case JS_CFUNC_f_f_f:
        {
            double d1, d2;

            if (unlikely(JS_ToFloat64(ctx, &d1, arg_buf[0]))) {
                ret_val = JS_EXCEPTION;
                break;
            }
            if (unlikely(JS_ToFloat64(ctx, &d2, arg_buf[1]))) {
                ret_val = JS_EXCEPTION;
                break;
            }
            ret_val = JS_NewFloat64(ctx, func.f_f_f(d1, d2));
        }
            break;
        case JS_CFUNC_iterator_next:
        {
            int done;
            ret_val = func.iterator_next(ctx, this_obj, argc, arg_buf,
                                         &done, p->u.cfunc.magic);
            if (!JS_IsException(ret_val) && done != 2) {
                ret_val = js_create_iterator_result(ctx, ret_val, done);
            }
        }
            break;
        default:
            abort();
    }

    rt->current_stack_frame = sf->prev_frame;
    return ret_val;
}

static JSValue js_call_bound_function(JSContext *ctx, JSValueConst func_obj,
                                      JSValueConst this_obj,
                                      int argc, JSValueConst *argv, int flags)
{
    JSObject *p;
    JSBoundFunction *bf;
    JSValueConst *arg_buf, new_target;
    int arg_count, i;

    p = JS_VALUE_GET_OBJ(func_obj);
    bf = p->u.bound_function;
    arg_count = bf->argc + argc;
    if (js_check_stack_overflow(ctx->rt, sizeof(JSValue) * arg_count))
        return JS_ThrowStackOverflow(ctx);
    arg_buf = alloca(sizeof(JSValue) * arg_count);
    for(i = 0; i < bf->argc; i++) {
        arg_buf[i] = bf->argv[i];
    }
    for(i = 0; i < argc; i++) {
        arg_buf[bf->argc + i] = argv[i];
    }
    if (flags & JS_CALL_FLAG_CONSTRUCTOR) {
        new_target = this_obj;
        if (js_same_value(ctx, func_obj, new_target))
            new_target = bf->func_obj;
        return JS_CallConstructor2(ctx, bf->func_obj, new_target,
                                   arg_count, arg_buf);
    } else {
        return JS_Call(ctx, bf->func_obj, bf->this_val,
                       arg_count, arg_buf);
    }
}

/* argument of OP_special_object */
typedef enum {
    OP_SPECIAL_OBJECT_ARGUMENTS,
    OP_SPECIAL_OBJECT_MAPPED_ARGUMENTS,
    OP_SPECIAL_OBJECT_THIS_FUNC,
    OP_SPECIAL_OBJECT_NEW_TARGET,
    OP_SPECIAL_OBJECT_HOME_OBJECT,
    OP_SPECIAL_OBJECT_VAR_OBJECT,
    OP_SPECIAL_OBJECT_IMPORT_META,
} OPSpecialObjectEnum;

#define FUNC_RET_AWAIT      0
#define FUNC_RET_YIELD      1
#define FUNC_RET_YIELD_STAR 2

/* argv[] is modified if (flags & JS_CALL_FLAG_COPY_ARGV) = 0. */
static JSValue JS_CallInternal(JSContext *caller_ctx, JSValueConst func_obj,
                               JSValueConst this_obj, JSValueConst new_target,
                               int argc, JSValue *argv, int flags)
{
    JSRuntime *rt = caller_ctx->rt;
    JSContext *ctx;
    JSObject *p;
    JSFunctionBytecode *b;
    JSStackFrame sf_s, *sf = &sf_s;
    const uint8_t *pc;
    int opcode, arg_allocated_size, i;
    JSValue *local_buf, *stack_buf, *var_buf, *arg_buf, *sp, ret_val, *pval;
    JSVarRef **var_refs;
    size_t alloca_size;

#if !DIRECT_DISPATCH
#define SWITCH(pc)      switch (opcode = *pc++)
#define CASE(op)        case op
#define DEFAULT         default
#define BREAK           break
#else
    static const void * const dispatch_table[256] = {
#define DEF(id, size, n_pop, n_push, f) && case_OP_ ## id,
#if SHORT_OPCODES
#define def(id, size, n_pop, n_push, f)
#else
#define def(id, size, n_pop, n_push, f) && case_default,
#endif
#include "quickjs-opcode.h"
        [ OP_COUNT ... 255 ] = &&case_default
    };
#define SWITCH(pc)      goto *dispatch_table[opcode = *pc++];
#define CASE(op)        case_ ## op
#define DEFAULT         case_default
#define BREAK           SWITCH(pc)
#endif

    if (js_poll_interrupts(caller_ctx))
        return JS_EXCEPTION;
    if (unlikely(JS_VALUE_GET_TAG(func_obj) != JS_TAG_OBJECT)) {
        if (flags & JS_CALL_FLAG_GENERATOR) {
            JSAsyncFunctionState *s = JS_VALUE_GET_PTR(func_obj);
            /* func_obj get contains a pointer to JSFuncAsyncState */
            /* the stack frame is already allocated */
            sf = &s->frame;
            p = JS_VALUE_GET_OBJ(sf->cur_func);
            b = p->u.func.function_bytecode;
            ctx = b->realm;
            var_refs = p->u.func.var_refs;
            local_buf = arg_buf = sf->arg_buf;
            var_buf = sf->var_buf;
            stack_buf = sf->var_buf + b->var_count;
            sp = sf->cur_sp;
            sf->cur_sp = NULL; /* cur_sp is NULL if the function is running */
            pc = sf->cur_pc;
            sf->prev_frame = rt->current_stack_frame;
            rt->current_stack_frame = sf;
            if (s->throw_flag)
                goto exception;
            else
                goto restart;
        } else {
            goto not_a_function;
        }
    }
    p = JS_VALUE_GET_OBJ(func_obj);
    if (unlikely(p->class_id != JS_CLASS_BYTECODE_FUNCTION)) {
        JSClassCall *call_func;
        call_func = rt->class_array[p->class_id].call;
        if (!call_func) {
            not_a_function:
            return JS_ThrowTypeError(caller_ctx, "not a function");
        }
        return call_func(caller_ctx, func_obj, this_obj, argc,
                         (JSValueConst *)argv, flags);
    }
    b = p->u.func.function_bytecode;

    if (unlikely(argc < b->arg_count || (flags & JS_CALL_FLAG_COPY_ARGV))) {
        arg_allocated_size = b->arg_count;
    } else {
        arg_allocated_size = 0;
    }

    alloca_size = sizeof(JSValue) * (arg_allocated_size + b->var_count +
                                     b->stack_size);
    if (js_check_stack_overflow(rt, alloca_size))
        return JS_ThrowStackOverflow(caller_ctx);

    sf->js_mode = b->js_mode;
    arg_buf = argv;
    sf->arg_count = argc;
    sf->cur_func = func_obj;
    init_list_head(&sf->var_ref_list);
    var_refs = p->u.func.var_refs;

    local_buf = alloca(alloca_size);
    if (unlikely(arg_allocated_size)) {
        int n = min_int(argc, b->arg_count);
        arg_buf = local_buf;
        for(i = 0; i < n; i++)
            arg_buf[i] = JS_DupValue(caller_ctx, argv[i]);
        for(; i < b->arg_count; i++)
            arg_buf[i] = JS_UNDEFINED;
        sf->arg_count = b->arg_count;
    }
    var_buf = local_buf + arg_allocated_size;
    sf->var_buf = var_buf;
    sf->arg_buf = arg_buf;

    for(i = 0; i < b->var_count; i++)
        var_buf[i] = JS_UNDEFINED;

    stack_buf = var_buf + b->var_count;
    sp = stack_buf;
    pc = b->byte_code_buf;
    sf->prev_frame = rt->current_stack_frame;
    rt->current_stack_frame = sf;
    ctx = b->realm; /* set the current realm */

    restart:
    for(;;) {
        int call_argc;
        JSValue *call_argv;

        SWITCH(pc) {
            CASE(OP_push_i32):
                *sp++ = JS_NewInt32(ctx, get_u32(pc));
                pc += 4;
                BREAK;
            CASE(OP_push_const):
                *sp++ = JS_DupValue(ctx, b->cpool[get_u32(pc)]);
                pc += 4;
                BREAK;
#if SHORT_OPCODES
                CASE(OP_push_minus1):
        CASE(OP_push_0):
        CASE(OP_push_1):
        CASE(OP_push_2):
        CASE(OP_push_3):
        CASE(OP_push_4):
        CASE(OP_push_5):
        CASE(OP_push_6):
        CASE(OP_push_7):
            *sp++ = JS_NewInt32(ctx, opcode - OP_push_0);
            BREAK;
        CASE(OP_push_i8):
            *sp++ = JS_NewInt32(ctx, get_i8(pc));
            pc += 1;
            BREAK;
        CASE(OP_push_i16):
            *sp++ = JS_NewInt32(ctx, get_i16(pc));
            pc += 2;
            BREAK;
        CASE(OP_push_const8):
            *sp++ = JS_DupValue(ctx, b->cpool[*pc++]);
            BREAK;
        CASE(OP_fclosure8):
            *sp++ = js_closure(ctx, JS_DupValue(ctx, b->cpool[*pc++]), var_refs, sf);
            if (unlikely(JS_IsException(sp[-1])))
                goto exception;
            BREAK;
        CASE(OP_push_empty_string):
            *sp++ = JS_AtomToString(ctx, JS_ATOM_empty_string);
            BREAK;
        CASE(OP_get_length):
            {
                JSValue val;

                val = JS_GetProperty(ctx, sp[-1], JS_ATOM_length);
                if (unlikely(JS_IsException(val)))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = val;
            }
            BREAK;
#endif
            CASE(OP_push_atom_value):
                *sp++ = JS_AtomToValue(ctx, get_u32(pc));
                pc += 4;
                BREAK;
            CASE(OP_undefined):
                *sp++ = JS_UNDEFINED;
                BREAK;
            CASE(OP_null):
                *sp++ = JS_NULL;
                BREAK;
            CASE(OP_push_this):
                /* OP_push_this is only called at the start of a function */
            {
                JSValue val;
                if (!(b->js_mode & JS_MODE_STRICT)) {
                    uint32_t tag = JS_VALUE_GET_TAG(this_obj);
                    if (likely(tag == JS_TAG_OBJECT))
                        goto normal_this;
                    if (tag == JS_TAG_NULL || tag == JS_TAG_UNDEFINED) {
                        val = JS_DupValue(ctx, ctx->global_obj);
                    } else {
                        val = JS_ToObject(ctx, this_obj);
                        if (JS_IsException(val))
                            goto exception;
                    }
                } else {
                    normal_this:
                    val = JS_DupValue(ctx, this_obj);
                }
                *sp++ = val;
            }
                BREAK;
            CASE(OP_push_false):
                *sp++ = JS_FALSE;
                BREAK;
            CASE(OP_push_true):
                *sp++ = JS_TRUE;
                BREAK;
            CASE(OP_object):
                *sp++ = JS_NewObject(ctx);
                if (unlikely(JS_IsException(sp[-1])))
                    goto exception;
                BREAK;
            CASE(OP_special_object):
            {
                int arg = *pc++;
                switch(arg) {
                    case OP_SPECIAL_OBJECT_ARGUMENTS:
                        *sp++ = js_build_arguments(ctx, argc, (JSValueConst *)argv);
                        if (unlikely(JS_IsException(sp[-1])))
                            goto exception;
                        break;
                    case OP_SPECIAL_OBJECT_MAPPED_ARGUMENTS:
                        *sp++ = js_build_mapped_arguments(ctx, argc, (JSValueConst *)argv,
                                                          sf, min_int(argc, b->arg_count));
                        if (unlikely(JS_IsException(sp[-1])))
                            goto exception;
                        break;
                    case OP_SPECIAL_OBJECT_THIS_FUNC:
                        *sp++ = JS_DupValue(ctx, sf->cur_func);
                        break;
                    case OP_SPECIAL_OBJECT_NEW_TARGET:
                        *sp++ = JS_DupValue(ctx, new_target);
                        break;
                    case OP_SPECIAL_OBJECT_HOME_OBJECT:
                    {
                        JSObject *p1;
                        p1 = p->u.func.home_object;
                        if (unlikely(!p1))
                            *sp++ = JS_UNDEFINED;
                        else
                            *sp++ = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p1));
                    }
                        break;
                    case OP_SPECIAL_OBJECT_VAR_OBJECT:
                        *sp++ = JS_NewObjectProto(ctx, JS_NULL);
                        if (unlikely(JS_IsException(sp[-1])))
                            goto exception;
                        break;
                    case OP_SPECIAL_OBJECT_IMPORT_META:
                        *sp++ = js_import_meta(ctx);
                        if (unlikely(JS_IsException(sp[-1])))
                            goto exception;
                        break;
                    default:
                        abort();
                }
            }
                BREAK;
            CASE(OP_rest):
            {
                int first = get_u16(pc);
                pc += 2;
                *sp++ = js_build_rest(ctx, first, argc, (JSValueConst *)argv);
                if (unlikely(JS_IsException(sp[-1])))
                    goto exception;
            }
                BREAK;

            CASE(OP_drop):
                JS_FreeValue(ctx, sp[-1]);
                sp--;
                BREAK;
            CASE(OP_nip):
                JS_FreeValue(ctx, sp[-2]);
                sp[-2] = sp[-1];
                sp--;
                BREAK;
            CASE(OP_nip1): /* a b c -> b c */
                JS_FreeValue(ctx, sp[-3]);
                sp[-3] = sp[-2];
                sp[-2] = sp[-1];
                sp--;
                BREAK;
            CASE(OP_dup):
                sp[0] = JS_DupValue(ctx, sp[-1]);
                sp++;
                BREAK;
            CASE(OP_dup2): /* a b -> a b a b */
                sp[0] = JS_DupValue(ctx, sp[-2]);
                sp[1] = JS_DupValue(ctx, sp[-1]);
                sp += 2;
                BREAK;
            CASE(OP_dup3): /* a b c -> a b c a b c */
                sp[0] = JS_DupValue(ctx, sp[-3]);
                sp[1] = JS_DupValue(ctx, sp[-2]);
                sp[2] = JS_DupValue(ctx, sp[-1]);
                sp += 3;
                BREAK;
            CASE(OP_dup1): /* a b -> a a b */
                sp[0] = sp[-1];
                sp[-1] = JS_DupValue(ctx, sp[-2]);
                sp++;
                BREAK;
            CASE(OP_insert2): /* obj a -> a obj a (dup_x1) */
                sp[0] = sp[-1];
                sp[-1] = sp[-2];
                sp[-2] = JS_DupValue(ctx, sp[0]);
                sp++;
                BREAK;
            CASE(OP_insert3): /* obj prop a -> a obj prop a (dup_x2) */
                sp[0] = sp[-1];
                sp[-1] = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = JS_DupValue(ctx, sp[0]);
                sp++;
                BREAK;
            CASE(OP_insert4): /* this obj prop a -> a this obj prop a */
                sp[0] = sp[-1];
                sp[-1] = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = sp[-4];
                sp[-4] = JS_DupValue(ctx, sp[0]);
                sp++;
                BREAK;
            CASE(OP_perm3): /* obj a b -> a obj b (213) */
            {
                JSValue tmp;
                tmp = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = tmp;
            }
                BREAK;
            CASE(OP_rot3l): /* x a b -> a b x (231) */
            {
                JSValue tmp;
                tmp = sp[-3];
                sp[-3] = sp[-2];
                sp[-2] = sp[-1];
                sp[-1] = tmp;
            }
                BREAK;
            CASE(OP_rot4l): /* x a b c -> a b c x */
            {
                JSValue tmp;
                tmp = sp[-4];
                sp[-4] = sp[-3];
                sp[-3] = sp[-2];
                sp[-2] = sp[-1];
                sp[-1] = tmp;
            }
                BREAK;
            CASE(OP_rot5l): /* x a b c d -> a b c d x */
            {
                JSValue tmp;
                tmp = sp[-5];
                sp[-5] = sp[-4];
                sp[-4] = sp[-3];
                sp[-3] = sp[-2];
                sp[-2] = sp[-1];
                sp[-1] = tmp;
            }
                BREAK;
            CASE(OP_rot3r): /* a b x -> x a b (312) */
            {
                JSValue tmp;
                tmp = sp[-1];
                sp[-1] = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = tmp;
            }
                BREAK;
            CASE(OP_perm4): /* obj prop a b -> a obj prop b */
            {
                JSValue tmp;
                tmp = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = sp[-4];
                sp[-4] = tmp;
            }
                BREAK;
            CASE(OP_perm5): /* this obj prop a b -> a this obj prop b */
            {
                JSValue tmp;
                tmp = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = sp[-4];
                sp[-4] = sp[-5];
                sp[-5] = tmp;
            }
                BREAK;
            CASE(OP_swap): /* a b -> b a */
            {
                JSValue tmp;
                tmp = sp[-2];
                sp[-2] = sp[-1];
                sp[-1] = tmp;
            }
                BREAK;
            CASE(OP_swap2): /* a b c d -> c d a b */
            {
                JSValue tmp1, tmp2;
                tmp1 = sp[-4];
                tmp2 = sp[-3];
                sp[-4] = sp[-2];
                sp[-3] = sp[-1];
                sp[-2] = tmp1;
                sp[-1] = tmp2;
            }
                BREAK;

            CASE(OP_fclosure):
            {
                JSValue bfunc = JS_DupValue(ctx, b->cpool[get_u32(pc)]);
                pc += 4;
                *sp++ = js_closure(ctx, bfunc, var_refs, sf);
                if (unlikely(JS_IsException(sp[-1])))
                    goto exception;
            }
                BREAK;
#if SHORT_OPCODES
                CASE(OP_call0):
        CASE(OP_call1):
        CASE(OP_call2):
        CASE(OP_call3):
            call_argc = opcode - OP_call0;
            goto has_call_argc;
#endif
            CASE(OP_call):
            CASE(OP_tail_call):
            {
                call_argc = get_u16(pc);
                pc += 2;
                goto has_call_argc;
                has_call_argc:
                call_argv = sp - call_argc;
                sf->cur_pc = pc;
                ret_val = JS_CallInternal(ctx, call_argv[-1], JS_UNDEFINED,
                                          JS_UNDEFINED, call_argc, call_argv, 0);
                if (unlikely(JS_IsException(ret_val)))
                    goto exception;
                if (opcode == OP_tail_call)
                    goto done;
                for(i = -1; i < call_argc; i++)
                    JS_FreeValue(ctx, call_argv[i]);
                sp -= call_argc + 1;
                *sp++ = ret_val;
            }
                BREAK;
            CASE(OP_call_constructor):
            {
                call_argc = get_u16(pc);
                pc += 2;
                call_argv = sp - call_argc;
                sf->cur_pc = pc;
                ret_val = JS_CallConstructorInternal(ctx, call_argv[-2],
                                                     call_argv[-1],
                                                     call_argc, call_argv, 0);
                if (unlikely(JS_IsException(ret_val)))
                    goto exception;
                for(i = -2; i < call_argc; i++)
                    JS_FreeValue(ctx, call_argv[i]);
                sp -= call_argc + 2;
                *sp++ = ret_val;
            }
                BREAK;
            CASE(OP_call_method):
            CASE(OP_tail_call_method):
            {
                call_argc = get_u16(pc);
                pc += 2;
                call_argv = sp - call_argc;
                sf->cur_pc = pc;
                ret_val = JS_CallInternal(ctx, call_argv[-1], call_argv[-2],
                                          JS_UNDEFINED, call_argc, call_argv, 0);
                if (unlikely(JS_IsException(ret_val)))
                    goto exception;
                if (opcode == OP_tail_call_method)
                    goto done;
                for(i = -2; i < call_argc; i++)
                    JS_FreeValue(ctx, call_argv[i]);
                sp -= call_argc + 2;
                *sp++ = ret_val;
            }
                BREAK;
            CASE(OP_array_from):
            {
                int i, ret;

                call_argc = get_u16(pc);
                pc += 2;
                ret_val = JS_NewArray(ctx);
                if (unlikely(JS_IsException(ret_val)))
                    goto exception;
                call_argv = sp - call_argc;
                for(i = 0; i < call_argc; i++) {
                    ret = JS_DefinePropertyValue(ctx, ret_val, __JS_AtomFromUInt32(i), call_argv[i],
                                                 JS_PROP_C_W_E | JS_PROP_THROW);
                    call_argv[i] = JS_UNDEFINED;
                    if (ret < 0) {
                        JS_FreeValue(ctx, ret_val);
                        goto exception;
                    }
                }
                sp -= call_argc;
                *sp++ = ret_val;
            }
                BREAK;

            CASE(OP_apply):
            {
                int magic;
                magic = get_u16(pc);
                pc += 2;

                ret_val = js_function_apply(ctx, sp[-3], 2, (JSValueConst *)&sp[-2], magic);
                if (unlikely(JS_IsException(ret_val)))
                    goto exception;
                JS_FreeValue(ctx, sp[-3]);
                JS_FreeValue(ctx, sp[-2]);
                JS_FreeValue(ctx, sp[-1]);
                sp -= 3;
                *sp++ = ret_val;
            }
                BREAK;
            CASE(OP_return):
                ret_val = *--sp;
                goto done;
            CASE(OP_return_undef):
                ret_val = JS_UNDEFINED;
                goto done;

            CASE(OP_check_ctor_return):
                /* return TRUE if 'this' should be returned */
                if (!JS_IsObject(sp[-1])) {
                    if (!JS_IsUndefined(sp[-1])) {
                        JS_ThrowTypeError(caller_ctx, "derived class constructor must return an object or undefined");
                        goto exception;
                    }
                    sp[0] = JS_TRUE;
                } else {
                    sp[0] = JS_FALSE;
                }
                sp++;
                BREAK;
            CASE(OP_check_ctor):
                if (JS_IsUndefined(new_target)) {
                    JS_ThrowTypeError(caller_ctx, "class constructors must be invoked with 'new'");
                    goto exception;
                }
                BREAK;
            CASE(OP_check_brand):
                if (JS_CheckBrand(ctx, sp[-2], sp[-1]) < 0)
                    goto exception;
                BREAK;
            CASE(OP_add_brand):
                if (JS_AddBrand(ctx, sp[-2], sp[-1]) < 0)
                    goto exception;
                JS_FreeValue(ctx, sp[-2]);
                JS_FreeValue(ctx, sp[-1]);
                sp -= 2;
                BREAK;

            CASE(OP_throw):
                JS_Throw(ctx, *--sp);
                goto exception;

            CASE(OP_throw_var):
#define JS_THROW_VAR_RO             0
#define JS_THROW_VAR_REDECL         1
#define JS_THROW_VAR_UNINITIALIZED  2
#define JS_THROW_VAR_DELETE_SUPER   3
            {
                JSAtom atom;
                int type;
                atom = get_u32(pc);
                type = pc[4];
                pc += 5;
                if (type == JS_THROW_VAR_RO)
                    JS_ThrowTypeErrorReadOnly(ctx, JS_PROP_THROW, atom);
                else
                if (type == JS_THROW_VAR_REDECL)
                    JS_ThrowSyntaxErrorVarRedeclaration(ctx, atom);
                else
                if (type == JS_THROW_VAR_UNINITIALIZED)
                    JS_ThrowReferenceErrorUninitialized(ctx, atom);
                else
                if (type == JS_THROW_VAR_DELETE_SUPER)
                    JS_ThrowReferenceError(ctx, "unsupported reference to 'super'");
                else
                    JS_ThrowInternalError(ctx, "invalid throw var type %d", type);
            }
                goto exception;

            CASE(OP_eval):
            {
                JSValueConst obj;
                int scope_idx;
                call_argc = get_u16(pc);
                scope_idx = get_u16(pc + 2) - 1;
                pc += 4;
                call_argv = sp - call_argc;
                sf->cur_pc = pc;
                if (js_same_value(ctx, call_argv[-1], ctx->eval_obj)) {
                    if (call_argc >= 1)
                        obj = call_argv[0];
                    else
                        obj = JS_UNDEFINED;
                    ret_val = JS_EvalObject(ctx, JS_UNDEFINED, obj,
                                            JS_EVAL_TYPE_DIRECT, scope_idx);
                } else {
                    ret_val = JS_CallInternal(ctx, call_argv[-1], JS_UNDEFINED,
                                              JS_UNDEFINED, call_argc, call_argv, 0);
                }
                if (unlikely(JS_IsException(ret_val)))
                    goto exception;
                for(i = -1; i < call_argc; i++)
                    JS_FreeValue(ctx, call_argv[i]);
                sp -= call_argc + 1;
                *sp++ = ret_val;
            }
                BREAK;
                /* could merge with OP_apply */
            CASE(OP_apply_eval):
            {
                int scope_idx;
                uint32_t len;
                JSValue *tab;
                JSValueConst obj;

                scope_idx = get_u16(pc) - 1;
                pc += 2;
                tab = build_arg_list(ctx, &len, sp[-1]);
                if (!tab)
                    goto exception;
                if (js_same_value(ctx, sp[-2], ctx->eval_obj)) {
                    if (len >= 1)
                        obj = tab[0];
                    else
                        obj = JS_UNDEFINED;
                    ret_val = JS_EvalObject(ctx, JS_UNDEFINED, obj,
                                            JS_EVAL_TYPE_DIRECT, scope_idx);
                } else {
                    ret_val = JS_Call(ctx, sp[-2], JS_UNDEFINED, len,
                                      (JSValueConst *)tab);
                }
                free_arg_list(ctx, tab, len);
                if (unlikely(JS_IsException(ret_val)))
                    goto exception;
                JS_FreeValue(ctx, sp[-2]);
                JS_FreeValue(ctx, sp[-1]);
                sp -= 2;
                *sp++ = ret_val;
            }
                BREAK;

            CASE(OP_regexp):
            {
                sp[-2] = js_regexp_constructor_internal(ctx, JS_UNDEFINED,
                                                        sp[-2], sp[-1]);
                sp--;
            }
                BREAK;

            CASE(OP_get_super):
            {
                JSValue proto;
                proto = JS_GetPrototype(ctx, sp[-1]);
                if (JS_IsException(proto))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = proto;
            }
                BREAK;

            CASE(OP_import):
            {
                JSValue val;
                val = js_dynamic_import(ctx, sp[-1]);
                if (JS_IsException(val))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = val;
            }
                BREAK;

            CASE(OP_check_var):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;

                ret = JS_CheckGlobalVar(ctx, atom);
                if (ret < 0)
                    goto exception;
                *sp++ = JS_NewBool(ctx, ret);
            }
                BREAK;

            CASE(OP_get_var_undef):
            CASE(OP_get_var):
            {
                JSValue val;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;

                val = JS_GetGlobalVar(ctx, atom, opcode - OP_get_var_undef);
                if (unlikely(JS_IsException(val)))
                    goto exception;
                *sp++ = val;
            }
                BREAK;

            CASE(OP_put_var):
            CASE(OP_put_var_init):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;

                ret = JS_SetGlobalVar(ctx, atom, sp[-1], opcode - OP_put_var);
                sp--;
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_put_var_strict):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;

                /* sp[-2] is JS_TRUE or JS_FALSE */
                if (unlikely(!JS_VALUE_GET_INT(sp[-2]))) {
                    JS_ThrowReferenceErrorNotDefined(ctx, atom);
                    goto exception;
                }
                ret = JS_SetGlobalVar(ctx, atom, sp[-1], 2);
                sp -= 2;
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_check_define_var):
            {
                JSAtom atom;
                int flags;
                atom = get_u32(pc);
                flags = pc[4];
                pc += 5;
                if (JS_CheckDefineGlobalVar(ctx, atom, flags))
                    goto exception;
            }
                BREAK;
            CASE(OP_define_var):
            {
                JSAtom atom;
                int flags;
                atom = get_u32(pc);
                flags = pc[4];
                pc += 5;
                if (JS_DefineGlobalVar(ctx, atom, flags))
                    goto exception;
            }
                BREAK;
            CASE(OP_define_func):
            {
                JSAtom atom;
                int flags;
                atom = get_u32(pc);
                flags = pc[4];
                pc += 5;
                if (JS_DefineGlobalFunction(ctx, atom, sp[-1], flags))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp--;
            }
                BREAK;

            CASE(OP_get_loc):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                sp[0] = JS_DupValue(ctx, var_buf[idx]);
                sp++;
            }
                BREAK;
            CASE(OP_put_loc):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, &var_buf[idx], sp[-1]);
                sp--;
            }
                BREAK;
            CASE(OP_set_loc):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, &var_buf[idx], JS_DupValue(ctx, sp[-1]));
            }
                BREAK;
            CASE(OP_get_arg):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                sp[0] = JS_DupValue(ctx, arg_buf[idx]);
                sp++;
            }
                BREAK;
            CASE(OP_put_arg):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, &arg_buf[idx], sp[-1]);
                sp--;
            }
                BREAK;
            CASE(OP_set_arg):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, &arg_buf[idx], JS_DupValue(ctx, sp[-1]));
            }
                BREAK;

#if SHORT_OPCODES
                CASE(OP_get_loc8): *sp++ = JS_DupValue(ctx, var_buf[*pc++]); BREAK;
        CASE(OP_put_loc8): set_value(ctx, &var_buf[*pc++], *--sp); BREAK;
        CASE(OP_set_loc8): set_value(ctx, &var_buf[*pc++], JS_DupValue(ctx, sp[-1])); BREAK;

        CASE(OP_get_loc0): *sp++ = JS_DupValue(ctx, var_buf[0]); BREAK;
        CASE(OP_get_loc1): *sp++ = JS_DupValue(ctx, var_buf[1]); BREAK;
        CASE(OP_get_loc2): *sp++ = JS_DupValue(ctx, var_buf[2]); BREAK;
        CASE(OP_get_loc3): *sp++ = JS_DupValue(ctx, var_buf[3]); BREAK;
        CASE(OP_put_loc0): set_value(ctx, &var_buf[0], *--sp); BREAK;
        CASE(OP_put_loc1): set_value(ctx, &var_buf[1], *--sp); BREAK;
        CASE(OP_put_loc2): set_value(ctx, &var_buf[2], *--sp); BREAK;
        CASE(OP_put_loc3): set_value(ctx, &var_buf[3], *--sp); BREAK;
        CASE(OP_set_loc0): set_value(ctx, &var_buf[0], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_loc1): set_value(ctx, &var_buf[1], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_loc2): set_value(ctx, &var_buf[2], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_loc3): set_value(ctx, &var_buf[3], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_get_arg0): *sp++ = JS_DupValue(ctx, arg_buf[0]); BREAK;
        CASE(OP_get_arg1): *sp++ = JS_DupValue(ctx, arg_buf[1]); BREAK;
        CASE(OP_get_arg2): *sp++ = JS_DupValue(ctx, arg_buf[2]); BREAK;
        CASE(OP_get_arg3): *sp++ = JS_DupValue(ctx, arg_buf[3]); BREAK;
        CASE(OP_put_arg0): set_value(ctx, &arg_buf[0], *--sp); BREAK;
        CASE(OP_put_arg1): set_value(ctx, &arg_buf[1], *--sp); BREAK;
        CASE(OP_put_arg2): set_value(ctx, &arg_buf[2], *--sp); BREAK;
        CASE(OP_put_arg3): set_value(ctx, &arg_buf[3], *--sp); BREAK;
        CASE(OP_set_arg0): set_value(ctx, &arg_buf[0], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_arg1): set_value(ctx, &arg_buf[1], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_arg2): set_value(ctx, &arg_buf[2], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_arg3): set_value(ctx, &arg_buf[3], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_get_var_ref0): *sp++ = JS_DupValue(ctx, *var_refs[0]->pvalue); BREAK;
        CASE(OP_get_var_ref1): *sp++ = JS_DupValue(ctx, *var_refs[1]->pvalue); BREAK;
        CASE(OP_get_var_ref2): *sp++ = JS_DupValue(ctx, *var_refs[2]->pvalue); BREAK;
        CASE(OP_get_var_ref3): *sp++ = JS_DupValue(ctx, *var_refs[3]->pvalue); BREAK;
        CASE(OP_put_var_ref0): set_value(ctx, var_refs[0]->pvalue, *--sp); BREAK;
        CASE(OP_put_var_ref1): set_value(ctx, var_refs[1]->pvalue, *--sp); BREAK;
        CASE(OP_put_var_ref2): set_value(ctx, var_refs[2]->pvalue, *--sp); BREAK;
        CASE(OP_put_var_ref3): set_value(ctx, var_refs[3]->pvalue, *--sp); BREAK;
        CASE(OP_set_var_ref0): set_value(ctx, var_refs[0]->pvalue, JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_var_ref1): set_value(ctx, var_refs[1]->pvalue, JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_var_ref2): set_value(ctx, var_refs[2]->pvalue, JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_var_ref3): set_value(ctx, var_refs[3]->pvalue, JS_DupValue(ctx, sp[-1])); BREAK;
#endif

            CASE(OP_get_var_ref):
            {
                int idx;
                JSValue val;
                idx = get_u16(pc);
                pc += 2;
                val = *var_refs[idx]->pvalue;
                sp[0] = JS_DupValue(ctx, val);
                sp++;
            }
                BREAK;
            CASE(OP_put_var_ref):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, var_refs[idx]->pvalue, sp[-1]);
                sp--;
            }
                BREAK;
            CASE(OP_set_var_ref):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, var_refs[idx]->pvalue, JS_DupValue(ctx, sp[-1]));
            }
                BREAK;
            CASE(OP_get_var_ref_check):
            {
                int idx;
                JSValue val;
                idx = get_u16(pc);
                pc += 2;
                val = *var_refs[idx]->pvalue;
                if (unlikely(JS_IsUninitialized(val))) {
                    JS_ThrowReferenceErrorUninitialized(ctx, JS_ATOM_NULL);
                    goto exception;
                }
                sp[0] = JS_DupValue(ctx, val);
                sp++;
            }
                BREAK;
            CASE(OP_put_var_ref_check):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                if (unlikely(JS_IsUninitialized(*var_refs[idx]->pvalue))) {
                    JS_ThrowReferenceErrorUninitialized(ctx, JS_ATOM_NULL);
                    goto exception;
                }
                set_value(ctx, var_refs[idx]->pvalue, sp[-1]);
                sp--;
            }
                BREAK;
            CASE(OP_put_var_ref_check_init):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                if (unlikely(!JS_IsUninitialized(*var_refs[idx]->pvalue))) {
                    JS_ThrowReferenceErrorUninitialized(ctx, JS_ATOM_NULL);
                    goto exception;
                }
                set_value(ctx, var_refs[idx]->pvalue, sp[-1]);
                sp--;
            }
                BREAK;
            CASE(OP_set_loc_uninitialized):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, &var_buf[idx], JS_UNINITIALIZED);
            }
                BREAK;
            CASE(OP_get_loc_check):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                if (unlikely(JS_IsUninitialized(var_buf[idx]))) {
                    JS_ThrowReferenceErrorUninitialized(ctx, JS_ATOM_NULL);
                    goto exception;
                }
                sp[0] = JS_DupValue(ctx, var_buf[idx]);
                sp++;
            }
                BREAK;
            CASE(OP_put_loc_check):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                if (unlikely(JS_IsUninitialized(var_buf[idx]))) {
                    JS_ThrowReferenceErrorUninitialized(ctx, JS_ATOM_NULL);
                    goto exception;
                }
                set_value(ctx, &var_buf[idx], sp[-1]);
                sp--;
            }
                BREAK;
            CASE(OP_put_loc_check_init):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                if (unlikely(!JS_IsUninitialized(var_buf[idx]))) {
                    JS_ThrowReferenceError(ctx, "'this' can be initialized only once");
                    goto exception;
                }
                set_value(ctx, &var_buf[idx], sp[-1]);
                sp--;
            }
                BREAK;
            CASE(OP_close_loc):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                close_lexical_var(ctx, sf, idx, FALSE);
            }
                BREAK;

            CASE(OP_make_loc_ref):
            CASE(OP_make_arg_ref):
            CASE(OP_make_var_ref_ref):
            {
                JSVarRef *var_ref;
                JSProperty *pr;
                JSAtom atom;
                int idx;
                atom = get_u32(pc);
                idx = get_u16(pc + 4);
                pc += 6;
                *sp++ = JS_NewObjectProto(ctx, JS_NULL);
                if (unlikely(JS_IsException(sp[-1])))
                    goto exception;
                if (opcode == OP_make_var_ref_ref) {
                    var_ref = var_refs[idx];
                    var_ref->header.ref_count++;
                } else {
                    var_ref = get_var_ref(ctx, sf, idx, opcode == OP_make_arg_ref);
                    if (!var_ref)
                        goto exception;
                }
                pr = add_property(ctx, JS_VALUE_GET_OBJ(sp[-1]), atom,
                                  JS_PROP_WRITABLE | JS_PROP_VARREF);
                if (!pr) {
                    free_var_ref(rt, var_ref);
                    goto exception;
                }
                pr->u.var_ref = var_ref;
                *sp++ = JS_AtomToValue(ctx, atom);
            }
                BREAK;
            CASE(OP_make_var_ref):
            {
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;

                if (JS_GetGlobalVarRef(ctx, atom, sp))
                    goto exception;
                sp += 2;
            }
                BREAK;

            CASE(OP_goto):
                pc += (int32_t)get_u32(pc);
                if (unlikely(js_poll_interrupts(ctx)))
                    goto exception;
                BREAK;
#if SHORT_OPCODES
                CASE(OP_goto16):
            pc += (int16_t)get_u16(pc);
            if (unlikely(js_poll_interrupts(ctx)))
                goto exception;
            BREAK;
        CASE(OP_goto8):
            pc += (int8_t)pc[0];
            if (unlikely(js_poll_interrupts(ctx)))
                goto exception;
            BREAK;
#endif
            CASE(OP_if_true):
            {
                int res;
                JSValue op1;

                op1 = sp[-1];
                pc += 4;
                if ((uint32_t)JS_VALUE_GET_TAG(op1) <= JS_TAG_UNDEFINED) {
                    res = JS_VALUE_GET_INT(op1);
                } else {
                    res = JS_ToBoolFree(ctx, op1);
                }
                sp--;
                if (res) {
                    pc += (int32_t)get_u32(pc - 4) - 4;
                }
                if (unlikely(js_poll_interrupts(ctx)))
                    goto exception;
            }
                BREAK;
            CASE(OP_if_false):
            {
                int res;
                JSValue op1;

                op1 = sp[-1];
                pc += 4;
                if ((uint32_t)JS_VALUE_GET_TAG(op1) <= JS_TAG_UNDEFINED) {
                    res = JS_VALUE_GET_INT(op1);
                } else {
                    res = JS_ToBoolFree(ctx, op1);
                }
                sp--;
                if (!res) {
                    pc += (int32_t)get_u32(pc - 4) - 4;
                }
                if (unlikely(js_poll_interrupts(ctx)))
                    goto exception;
            }
                BREAK;
#if SHORT_OPCODES
                CASE(OP_if_true8):
            {
                int res;
                JSValue op1;

                op1 = sp[-1];
                pc += 1;
                if ((uint32_t)JS_VALUE_GET_TAG(op1) <= JS_TAG_UNDEFINED) {
                    res = JS_VALUE_GET_INT(op1);
                } else {
                    res = JS_ToBoolFree(ctx, op1);
                }
                sp--;
                if (res) {
                    pc += (int8_t)pc[-1] - 1;
                }
                if (unlikely(js_poll_interrupts(ctx)))
                    goto exception;
            }
            BREAK;
        CASE(OP_if_false8):
            {
                int res;
                JSValue op1;

                op1 = sp[-1];
                pc += 1;
                if ((uint32_t)JS_VALUE_GET_TAG(op1) <= JS_TAG_UNDEFINED) {
                    res = JS_VALUE_GET_INT(op1);
                } else {
                    res = JS_ToBoolFree(ctx, op1);
                }
                sp--;
                if (!res) {
                    pc += (int8_t)pc[-1] - 1;
                }
                if (unlikely(js_poll_interrupts(ctx)))
                    goto exception;
            }
            BREAK;
#endif
            CASE(OP_catch):
            {
                int32_t diff;
                diff = get_u32(pc);
                sp[0] = JS_NewCatchOffset(ctx, pc + diff - b->byte_code_buf);
                sp++;
                pc += 4;
            }
                BREAK;
            CASE(OP_gosub):
            {
                int32_t diff;
                diff = get_u32(pc);
                /* XXX: should have a different tag to avoid security flaw */
                sp[0] = JS_NewInt32(ctx, pc + 4 - b->byte_code_buf);
                sp++;
                pc += diff;
            }
                BREAK;
            CASE(OP_ret):
            {
                JSValue op1;
                uint32_t pos;
                op1 = sp[-1];
                if (unlikely(JS_VALUE_GET_TAG(op1) != JS_TAG_INT))
                    goto ret_fail;
                pos = JS_VALUE_GET_INT(op1);
                if (unlikely(pos >= b->byte_code_len)) {
                    ret_fail:
                    JS_ThrowInternalError(ctx, "invalid ret value");
                    goto exception;
                }
                sp--;
                pc = b->byte_code_buf + pos;
            }
                BREAK;

            CASE(OP_for_in_start):
                if (js_for_in_start(ctx, sp))
                    goto exception;
                BREAK;
            CASE(OP_for_in_next):
                if (js_for_in_next(ctx, sp))
                    goto exception;
                sp += 2;
                BREAK;
            CASE(OP_for_of_start):
                if (js_for_of_start(ctx, sp, FALSE))
                    goto exception;
                sp += 1;
                *sp++ = JS_NewCatchOffset(ctx, 0);
                BREAK;
            CASE(OP_for_of_next):
            {
                int offset = -3 - pc[0];
                pc += 1;
                if (js_for_of_next(ctx, sp, offset))
                    goto exception;
                sp += 2;
            }
                BREAK;
            CASE(OP_for_await_of_start):
                if (js_for_of_start(ctx, sp, TRUE))
                    goto exception;
                sp += 1;
                *sp++ = JS_NewCatchOffset(ctx, 0);
                BREAK;
            CASE(OP_for_await_of_next):
                if (js_for_await_of_next(ctx, sp))
                    goto exception;
                sp += 1;
                BREAK;
            CASE(OP_iterator_get_value_done):
                if (js_iterator_get_value_done(ctx, sp))
                    goto exception;
                sp += 1;
                BREAK;

            CASE(OP_iterator_close):
                /* iter_obj next catch_offset -> */
                sp--; /* drop the catch offset to avoid getting caught by exception */
                JS_FreeValue(ctx, sp[-1]); /* drop the next method */
                sp--;
                if (!JS_IsUndefined(sp[-1])) {
                    if (JS_IteratorClose(ctx, sp[-1], FALSE))
                        goto exception;
                    JS_FreeValue(ctx, sp[-1]);
                }
                sp--;
                BREAK;
            CASE(OP_iterator_close_return):
            {
                JSValue ret_val;
                /* iter_obj next catch_offset ... ret_val ->
                   ret_eval iter_obj next catch_offset */
                ret_val = *--sp;
                while (sp > stack_buf &&
                       JS_VALUE_GET_TAG(sp[-1]) != JS_TAG_CATCH_OFFSET) {
                    JS_FreeValue(ctx, *--sp);
                }
                if (unlikely(sp < stack_buf + 3)) {
                    JS_ThrowInternalError(ctx, "iterator_close_return");
                    JS_FreeValue(ctx, ret_val);
                    goto exception;
                }
                sp[0] = sp[-1];
                sp[-1] = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = ret_val;
                sp++;
            }
                BREAK;

            CASE(OP_async_iterator_close):
                /* iter_obj next catch_offset -> value flag */
            {
                JSValue ret, method;
                int ret_flag;
                sp--; /* remove the catch offset */
                method = JS_GetProperty(ctx, sp[-2], JS_ATOM_return);
                if (JS_IsException(method))
                    goto exception;
                if (JS_IsUndefined(method) || JS_IsNull(method)) {
                    ret = JS_UNDEFINED;
                    ret_flag = TRUE;
                } else {
                    ret = JS_CallFree(ctx, method, sp[-2], 0, NULL);
                    if (JS_IsException(ret))
                        goto exception;
                    if (!JS_IsObject(ret)) {
                        JS_FreeValue(ctx, ret);
                        JS_ThrowTypeErrorNotAnObject(ctx);
                        goto exception;
                    }
                    ret_flag = FALSE;
                }
                JS_FreeValue(ctx, sp[-2]);
                JS_FreeValue(ctx, sp[-1]);
                sp[-2] = ret;
                sp[-1] = JS_NewBool(ctx, ret_flag);
            }
                BREAK;

            CASE(OP_async_iterator_next):
                /* stack: iter_obj next catch_offset val */
            {
                JSValue ret;
                ret = JS_Call(ctx, sp[-3], sp[-4],
                              1, (JSValueConst *)(sp - 1));
                if (JS_IsException(ret))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = ret;
            }
                BREAK;

            CASE(OP_async_iterator_get):
                /* stack: iter_obj next catch_offset val */
            {
                JSValue method, ret;
                BOOL ret_flag;
                int flags;
                flags = *pc++;
                /* XXX: use another opcode such as OP_throw_var */
                if (flags == 2) {
                    JS_ThrowTypeError(ctx, "iterator does not have a throw method");
                    goto exception;
                }
                method = JS_GetProperty(ctx, sp[-4], flags ? JS_ATOM_throw : JS_ATOM_return);
                if (JS_IsException(method))
                    goto exception;
                if (JS_IsUndefined(method) || JS_IsNull(method)) {
                    ret_flag = TRUE;
                } else {
                    ret = JS_CallFree(ctx, method, sp[-4],
                                      1, (JSValueConst *)(sp - 1));
                    if (JS_IsException(ret))
                        goto exception;
                    JS_FreeValue(ctx, sp[-1]);
                    sp[-1] = ret;
                    ret_flag = FALSE;
                }
                sp[0] = JS_NewBool(ctx, ret_flag);
                sp += 1;
            }
                BREAK;

            CASE(OP_lnot):
            {
                int res;
                JSValue op1;

                op1 = sp[-1];
                if ((uint32_t)JS_VALUE_GET_TAG(op1) <= JS_TAG_UNDEFINED) {
                    res = JS_VALUE_GET_INT(op1) != 0;
                } else {
                    res = JS_ToBoolFree(ctx, op1);
                }
                sp[-1] = JS_NewBool(ctx, !res);
            }
                BREAK;

            CASE(OP_get_field):
            {
                JSValue val;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;

                val = JS_GetProperty(ctx, sp[-1], atom);
                if (unlikely(JS_IsException(val)))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = val;
            }
                BREAK;

            CASE(OP_get_field2):
            {
                JSValue val;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;

                val = JS_GetProperty(ctx, sp[-1], atom);
                if (unlikely(JS_IsException(val)))
                    goto exception;
                *sp++ = val;
            }
                BREAK;

            CASE(OP_put_field):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;

                ret = JS_SetPropertyInternal(ctx, sp[-2], atom, sp[-1],
                                             JS_PROP_THROW_STRICT);
                JS_FreeValue(ctx, sp[-2]);
                sp -= 2;
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_private_symbol):
            {
                JSAtom atom;
                JSValue val;

                atom = get_u32(pc);
                pc += 4;
                val = JS_NewSymbolFromAtom(ctx, atom, JS_ATOM_TYPE_PRIVATE);
                if (JS_IsException(val))
                    goto exception;
                *sp++ = val;
            }
                BREAK;

            CASE(OP_get_private_field):
            {
                JSValue val;

                val = JS_GetPrivateField(ctx, sp[-2], sp[-1]);
                JS_FreeValue(ctx, sp[-1]);
                JS_FreeValue(ctx, sp[-2]);
                sp[-2] = val;
                sp--;
                if (unlikely(JS_IsException(val)))
                    goto exception;
            }
                BREAK;

            CASE(OP_put_private_field):
            {
                int ret;
                ret = JS_SetPrivateField(ctx, sp[-3], sp[-1], sp[-2]);
                JS_FreeValue(ctx, sp[-3]);
                JS_FreeValue(ctx, sp[-1]);
                sp -= 3;
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_define_private_field):
            {
                int ret;
                ret = JS_DefinePrivateField(ctx, sp[-3], sp[-2], sp[-1]);
                JS_FreeValue(ctx, sp[-2]);
                sp -= 2;
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_define_field):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;

                ret = JS_DefinePropertyValue(ctx, sp[-2], atom, sp[-1],
                                             JS_PROP_C_W_E | JS_PROP_THROW);
                sp--;
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_set_name):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;

                ret = JS_DefineObjectName(ctx, sp[-1], atom, JS_PROP_CONFIGURABLE);
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;
            CASE(OP_set_name_computed):
            {
                int ret;
                ret = JS_DefineObjectNameComputed(ctx, sp[-1], sp[-2], JS_PROP_CONFIGURABLE);
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;
            CASE(OP_set_proto):
            {
                JSValue proto;
                proto = sp[-1];
                if (JS_IsObject(proto) || JS_IsNull(proto)) {
                    if (JS_SetPrototypeInternal(ctx, sp[-2], proto, TRUE) < 0)
                        goto exception;
                }
                JS_FreeValue(ctx, proto);
                sp--;
            }
                BREAK;
            CASE(OP_set_home_object):
                js_method_set_home_object(ctx, sp[-1], sp[-2]);
                BREAK;
            CASE(OP_define_method):
            CASE(OP_define_method_computed):
            {
                JSValue getter, setter, value;
                JSValueConst obj;
                JSAtom atom;
                int flags, ret, op_flags;
                BOOL is_computed;
#define OP_DEFINE_METHOD_METHOD 0
#define OP_DEFINE_METHOD_GETTER 1
#define OP_DEFINE_METHOD_SETTER 2
#define OP_DEFINE_METHOD_ENUMERABLE 4

                is_computed = (opcode == OP_define_method_computed);
                if (is_computed) {
                    atom = JS_ValueToAtom(ctx, sp[-2]);
                    if (unlikely(atom == JS_ATOM_NULL))
                        goto exception;
                    opcode += OP_define_method - OP_define_method_computed;
                } else {
                    atom = get_u32(pc);
                    pc += 4;
                }
                op_flags = *pc++;

                obj = sp[-2 - is_computed];
                flags = JS_PROP_HAS_CONFIGURABLE | JS_PROP_CONFIGURABLE |
                        JS_PROP_HAS_ENUMERABLE | JS_PROP_THROW;
                if (op_flags & OP_DEFINE_METHOD_ENUMERABLE)
                    flags |= JS_PROP_ENUMERABLE;
                op_flags &= 3;
                value = JS_UNDEFINED;
                getter = JS_UNDEFINED;
                setter = JS_UNDEFINED;
                if (op_flags == OP_DEFINE_METHOD_METHOD) {
                    value = sp[-1];
                    flags |= JS_PROP_HAS_VALUE | JS_PROP_HAS_WRITABLE | JS_PROP_WRITABLE;
                } else if (op_flags == OP_DEFINE_METHOD_GETTER) {
                    getter = sp[-1];
                    flags |= JS_PROP_HAS_GET;
                } else {
                    setter = sp[-1];
                    flags |= JS_PROP_HAS_SET;
                }
                ret = js_method_set_properties(ctx, sp[-1], atom, flags, obj);
                if (ret >= 0) {
                    ret = JS_DefineProperty(ctx, obj, atom, value,
                                            getter, setter, flags);
                }
                JS_FreeValue(ctx, sp[-1]);
                if (is_computed) {
                    JS_FreeAtom(ctx, atom);
                    JS_FreeValue(ctx, sp[-2]);
                }
                sp -= 1 + is_computed;
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_define_class):
            CASE(OP_define_class_computed):
            {
                int class_flags;
                JSAtom atom;

                atom = get_u32(pc);
                class_flags = pc[4];
                pc += 5;
                if (js_op_define_class(ctx, sp, atom, class_flags,
                                       var_refs, sf,
                                       (opcode == OP_define_class_computed)) < 0)
                    goto exception;
            }
                BREAK;

            CASE(OP_get_array_el):
            {
                JSValue val;

                val = JS_GetPropertyValue(ctx, sp[-2], sp[-1]);
                JS_FreeValue(ctx, sp[-2]);
                sp[-2] = val;
                sp--;
                if (unlikely(JS_IsException(val)))
                    goto exception;
            }
                BREAK;

            CASE(OP_get_array_el2):
            {
                JSValue val;

                val = JS_GetPropertyValue(ctx, sp[-2], sp[-1]);
                sp[-1] = val;
                if (unlikely(JS_IsException(val)))
                    goto exception;
            }
                BREAK;

            CASE(OP_get_ref_value):
            {
                JSValue val;
                if (unlikely(JS_IsUndefined(sp[-2]))) {
                    JSAtom atom = JS_ValueToAtom(ctx, sp[-1]);
                    if (atom != JS_ATOM_NULL) {
                        JS_ThrowReferenceErrorNotDefined(ctx, atom);
                        JS_FreeAtom(ctx, atom);
                    }
                    goto exception;
                }
                val = JS_GetPropertyValue(ctx, sp[-2],
                                          JS_DupValue(ctx, sp[-1]));
                if (unlikely(JS_IsException(val)))
                    goto exception;
                sp[0] = val;
                sp++;
            }
                BREAK;

            CASE(OP_get_super_value):
            {
                JSValue val;
                JSAtom atom;
                atom = JS_ValueToAtom(ctx, sp[-1]);
                if (unlikely(atom == JS_ATOM_NULL))
                    goto exception;
                val = JS_GetPropertyInternal(ctx, sp[-2], atom, sp[-3], FALSE);
                JS_FreeAtom(ctx, atom);
                if (unlikely(JS_IsException(val)))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                JS_FreeValue(ctx, sp[-2]);
                JS_FreeValue(ctx, sp[-3]);
                sp[-3] = val;
                sp -= 2;
            }
                BREAK;

            CASE(OP_put_array_el):
            {
                int ret;

                ret = JS_SetPropertyValue(ctx, sp[-3], sp[-2], sp[-1], JS_PROP_THROW_STRICT);
                JS_FreeValue(ctx, sp[-3]);
                sp -= 3;
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_put_ref_value):
            {
                int ret;
                if (unlikely(JS_IsUndefined(sp[-3]))) {
                    if (is_strict_mode(ctx)) {
                        JSAtom atom = JS_ValueToAtom(ctx, sp[-2]);
                        if (atom != JS_ATOM_NULL) {
                            JS_ThrowReferenceErrorNotDefined(ctx, atom);
                            JS_FreeAtom(ctx, atom);
                        }
                        goto exception;
                    } else {
                        sp[-3] = JS_DupValue(ctx, ctx->global_obj);
                    }
                }
                ret = JS_SetPropertyValue(ctx, sp[-3], sp[-2], sp[-1], JS_PROP_THROW_STRICT);
                JS_FreeValue(ctx, sp[-3]);
                sp -= 3;
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_put_super_value):
            {
                int ret;
                JSAtom atom;
                if (JS_VALUE_GET_TAG(sp[-3]) != JS_TAG_OBJECT) {
                    JS_ThrowTypeErrorNotAnObject(ctx);
                    goto exception;
                }
                atom = JS_ValueToAtom(ctx, sp[-2]);
                if (unlikely(atom == JS_ATOM_NULL))
                    goto exception;
                ret = JS_SetPropertyGeneric(ctx, JS_VALUE_GET_OBJ(sp[-3]),
                                            atom, sp[-1], sp[-4],
                                            JS_PROP_THROW_STRICT);
                JS_FreeAtom(ctx, atom);
                JS_FreeValue(ctx, sp[-4]);
                JS_FreeValue(ctx, sp[-3]);
                JS_FreeValue(ctx, sp[-2]);
                sp -= 4;
                if (ret < 0)
                    goto exception;
            }
                BREAK;

            CASE(OP_define_array_el):
            {
                int ret;
                ret = JS_DefinePropertyValueValue(ctx, sp[-3], JS_DupValue(ctx, sp[-2]), sp[-1],
                                                  JS_PROP_C_W_E | JS_PROP_THROW);
                sp -= 1;
                if (unlikely(ret < 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_append):    /* array pos enumobj -- array pos */
            {
                if (js_append_enumerate(ctx, sp))
                    goto exception;
                JS_FreeValue(ctx, *--sp);
            }
                BREAK;

            CASE(OP_copy_data_properties):    /* target source excludeList */
            {
                /* stack offsets (-1 based):
                   2 bits for target,
                   3 bits for source,
                   2 bits for exclusionList */
                int mask;

                mask = *pc++;
                if (JS_CopyDataProperties(ctx, sp[-1 - (mask & 3)],
                                          sp[-1 - ((mask >> 2) & 7)],
                                          sp[-1 - ((mask >> 5) & 7)], 0))
                    goto exception;
            }
                BREAK;

            CASE(OP_add):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    int64_t r;
                    r = (int64_t)JS_VALUE_GET_INT(op1) + JS_VALUE_GET_INT(op2);
                    if (unlikely((int)r != r))
                        goto add_slow;
                    sp[-2] = JS_NewInt32(ctx, r);
                    sp--;
                } else if (JS_VALUE_IS_BOTH_FLOAT(op1, op2)) {
                    sp[-2] = __JS_NewFloat64(ctx, JS_VALUE_GET_FLOAT64(op1) +
                                                  JS_VALUE_GET_FLOAT64(op2));
                    sp--;
                } else {
                    add_slow:
                    if (js_add_slow(ctx, sp))
                        goto exception;
                    sp--;
                }
            }
                BREAK;
            CASE(OP_add_loc):
            {
                JSValue ops[2];
                int idx;
                idx = *pc;
                pc += 1;

                ops[0] = var_buf[idx];
                ops[1] = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(ops[0], ops[1]))) {
                    int64_t r;
                    r = (int64_t)JS_VALUE_GET_INT(ops[0]) + JS_VALUE_GET_INT(ops[1]);
                    if (unlikely((int)r != r))
                        goto add_loc_slow;
                    var_buf[idx] = JS_NewInt32(ctx, r);
                    sp--;
                } else if (JS_VALUE_GET_TAG(ops[0]) == JS_TAG_STRING) {
                    sp--;
                    ops[1] = JS_ToPrimitiveFree(ctx, ops[1], HINT_NONE);
                    if (JS_IsException(ops[1])) {
                        goto exception;
                    }
                    /* XXX: should not modify the variable in case of
                       exception */
                    ops[0] = JS_ConcatString(ctx, ops[0], ops[1]);
                    if (JS_IsException(ops[0])) {
                        var_buf[idx] = JS_UNDEFINED;
                        goto exception;
                    }
                    var_buf[idx] = ops[0];
                } else {
                    add_loc_slow:
                    /* XXX: should not modify the variable in case of
                       exception */
                    sp--;
                    /* In case of exception, js_add_slow frees ops[0]
                       and ops[1]. */
                    /* XXX: change API */
                    if (js_add_slow(ctx, ops + 2)) {
                        var_buf[idx] = JS_UNDEFINED;
                        goto exception;
                    }
                    var_buf[idx] = ops[0];
                }
            }
                BREAK;
            CASE(OP_sub):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    int64_t r;
                    r = (int64_t)JS_VALUE_GET_INT(op1) - JS_VALUE_GET_INT(op2);
                    if (unlikely((int)r != r))
                        goto binary_arith_slow;
                    sp[-2] = JS_NewInt32(ctx, r);
                    sp--;
                } else if (JS_VALUE_IS_BOTH_FLOAT(op1, op2)) {
                    sp[-2] = __JS_NewFloat64(ctx, JS_VALUE_GET_FLOAT64(op1) -
                                                  JS_VALUE_GET_FLOAT64(op2));
                    sp--;
                } else {
                    goto binary_arith_slow;
                }
            }
                BREAK;
            CASE(OP_mul):
            {
                JSValue op1, op2;
                double d;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    int32_t v1, v2;
                    int64_t r;
                    v1 = JS_VALUE_GET_INT(op1);
                    v2 = JS_VALUE_GET_INT(op2);
                    r = (int64_t)v1 * v2;
                    if (unlikely((int)r != r)) {
#ifdef CONFIG_BIGNUM
                        if (unlikely(sf->js_mode & JS_MODE_MATH) &&
                            (r < -MAX_SAFE_INTEGER || r > MAX_SAFE_INTEGER))
                            goto binary_arith_slow;
#endif
                        d = (double)r;
                        goto mul_fp_res;
                    }
                    /* need to test zero case for -0 result */
                    if (unlikely(r == 0 && (v1 | v2) < 0)) {
                        d = -0.0;
                        goto mul_fp_res;
                    }
                    sp[-2] = JS_NewInt32(ctx, r);
                    sp--;
                } else if (JS_VALUE_IS_BOTH_FLOAT(op1, op2)) {
#ifdef CONFIG_BIGNUM
                    if (unlikely(sf->js_mode & JS_MODE_MATH))
                        goto binary_arith_slow;
#endif
                    d = JS_VALUE_GET_FLOAT64(op1) * JS_VALUE_GET_FLOAT64(op2);
                    mul_fp_res:
                    sp[-2] = __JS_NewFloat64(ctx, d);
                    sp--;
                } else {
                    goto binary_arith_slow;
                }
            }
                BREAK;
            CASE(OP_div):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    int v1, v2;
                    if (unlikely(sf->js_mode & JS_MODE_MATH))
                        goto binary_arith_slow;
                    v1 = JS_VALUE_GET_INT(op1);
                    v2 = JS_VALUE_GET_INT(op2);
                    sp[-2] = JS_NewFloat64(ctx, (double)v1 / (double)v2);
                    sp--;
                } else {
                    goto binary_arith_slow;
                }
            }
                BREAK;
            CASE(OP_mod):
#ifdef CONFIG_BIGNUM
                CASE(OP_math_mod):
#endif
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    int v1, v2, r;
                    v1 = JS_VALUE_GET_INT(op1);
                    v2 = JS_VALUE_GET_INT(op2);
                    /* We must avoid v2 = 0, v1 = INT32_MIN and v2 =
                       -1 and the cases where the result is -0. */
                    if (unlikely(v1 < 0 || v2 <= 0))
                        goto binary_arith_slow;
                    r = v1 % v2;
                    sp[-2] = JS_NewInt32(ctx, r);
                    sp--;
                } else {
                    goto binary_arith_slow;
                }
            }
                BREAK;
            CASE(OP_pow):
            binary_arith_slow:
                if (js_binary_arith_slow(ctx, sp, opcode))
                    goto exception;
                sp--;
                BREAK;

            CASE(OP_plus):
            {
                JSValue op1;
                uint32_t tag;
                op1 = sp[-1];
                tag = JS_VALUE_GET_TAG(op1);
                if (tag == JS_TAG_INT || JS_TAG_IS_FLOAT64(tag)) {
                } else {
                    if (js_unary_arith_slow(ctx, sp, opcode))
                        goto exception;
                }
            }
                BREAK;
            CASE(OP_neg):
            {
                JSValue op1;
                uint32_t tag;
                int val;
                double d;
                op1 = sp[-1];
                tag = JS_VALUE_GET_TAG(op1);
                if (tag == JS_TAG_INT) {
                    val = JS_VALUE_GET_INT(op1);
                    /* Note: -0 cannot be expressed as integer */
                    if (unlikely(val == 0)) {
                        d = -0.0;
                        goto neg_fp_res;
                    }
                    if (unlikely(val == INT32_MIN)) {
                        d = -(double)val;
                        goto neg_fp_res;
                    }
                    sp[-1] = JS_NewInt32(ctx, -val);
                } else if (JS_TAG_IS_FLOAT64(tag)) {
                    d = -JS_VALUE_GET_FLOAT64(op1);
                    neg_fp_res:
                    sp[-1] = __JS_NewFloat64(ctx, d);
                } else {
                    if (js_unary_arith_slow(ctx, sp, opcode))
                        goto exception;
                }
            }
                BREAK;
            CASE(OP_inc):
            {
                JSValue op1;
                int val;
                op1 = sp[-1];
                if (JS_VALUE_GET_TAG(op1) == JS_TAG_INT) {
                    val = JS_VALUE_GET_INT(op1);
                    if (unlikely(val == INT32_MAX))
                        goto inc_slow;
                    sp[-1] = JS_NewInt32(ctx, val + 1);
                } else {
                    inc_slow:
                    if (js_unary_arith_slow(ctx, sp, opcode))
                        goto exception;
                }
            }
                BREAK;
            CASE(OP_dec):
            {
                JSValue op1;
                int val;
                op1 = sp[-1];
                if (JS_VALUE_GET_TAG(op1) == JS_TAG_INT) {
                    val = JS_VALUE_GET_INT(op1);
                    if (unlikely(val == INT32_MIN))
                        goto dec_slow;
                    sp[-1] = JS_NewInt32(ctx, val - 1);
                } else {
                    dec_slow:
                    if (js_unary_arith_slow(ctx, sp, opcode))
                        goto exception;
                }
            }
                BREAK;
            CASE(OP_post_inc):
            CASE(OP_post_dec):
                if (js_post_inc_slow(ctx, sp, opcode))
                    goto exception;
                sp++;
                BREAK;
            CASE(OP_inc_loc):
            {
                JSValue op1;
                int val;
                int idx;
                idx = *pc;
                pc += 1;

                op1 = var_buf[idx];
                if (JS_VALUE_GET_TAG(op1) == JS_TAG_INT) {
                    val = JS_VALUE_GET_INT(op1);
                    if (unlikely(val == INT32_MAX))
                        goto inc_loc_slow;
                    var_buf[idx] = JS_NewInt32(ctx, val + 1);
                } else {
                    inc_loc_slow:
                    if (js_unary_arith_slow(ctx, var_buf + idx + 1, OP_inc))
                        goto exception;
                }
            }
                BREAK;
            CASE(OP_dec_loc):
            {
                JSValue op1;
                int val;
                int idx;
                idx = *pc;
                pc += 1;

                op1 = var_buf[idx];
                if (JS_VALUE_GET_TAG(op1) == JS_TAG_INT) {
                    val = JS_VALUE_GET_INT(op1);
                    if (unlikely(val == INT32_MIN))
                        goto dec_loc_slow;
                    var_buf[idx] = JS_NewInt32(ctx, val - 1);
                } else {
                    dec_loc_slow:
                    if (js_unary_arith_slow(ctx, var_buf + idx + 1, OP_dec))
                        goto exception;
                }
            }
                BREAK;
            CASE(OP_not):
            {
                JSValue op1;
                op1 = sp[-1];
                if (JS_VALUE_GET_TAG(op1) == JS_TAG_INT) {
                    sp[-1] = JS_NewInt32(ctx, ~JS_VALUE_GET_INT(op1));
                } else {
                    if (js_not_slow(ctx, sp))
                        goto exception;
                }
            }
                BREAK;

            CASE(OP_shl):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    uint32_t v1, v2;
                    v1 = JS_VALUE_GET_INT(op1);
                    v2 = JS_VALUE_GET_INT(op2);
#ifdef CONFIG_BIGNUM
                    {
                        int64_t r;
                        if (unlikely(sf->js_mode & JS_MODE_MATH)) {
                            if (v2 > 0x1f)
                                goto shl_slow;
                            r = (int64_t)v1 << v2;
                            if ((int)r != r)
                                goto shl_slow;
                        } else {
                            v2 &= 0x1f;
                        }
                    }
#else
                    v2 &= 0x1f;
#endif
                    sp[-2] = JS_NewInt32(ctx, v1 << v2);
                    sp--;
                } else {
#ifdef CONFIG_BIGNUM
                    shl_slow:
#endif
                    if (js_binary_logic_slow(ctx, sp, opcode))
                        goto exception;
                    sp--;
                }
            }
                BREAK;
            CASE(OP_shr):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    uint32_t v2;
                    v2 = JS_VALUE_GET_INT(op2);
                    /* v1 >>> v2 retains its JS semantics if CONFIG_BIGNUM */
                    v2 &= 0x1f;
                    sp[-2] = JS_NewUint32(ctx,
                                          (uint32_t)JS_VALUE_GET_INT(op1) >>
                                                                          v2);
                    sp--;
                } else {
                    if (js_shr_slow(ctx, sp))
                        goto exception;
                    sp--;
                }
            }
                BREAK;
            CASE(OP_sar):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    uint32_t v2;
                    v2 = JS_VALUE_GET_INT(op2);
#ifdef CONFIG_BIGNUM
                    if (unlikely(v2 > 0x1f)) {
                        if (unlikely(sf->js_mode & JS_MODE_MATH))
                            goto sar_slow;
                        else
                            v2 &= 0x1f;
                    }
#else
                    v2 &= 0x1f;
#endif
                    sp[-2] = JS_NewInt32(ctx,
                                         (int)JS_VALUE_GET_INT(op1) >> v2);
                    sp--;
                } else {
#ifdef CONFIG_BIGNUM
                    sar_slow:
#endif
                    if (js_binary_logic_slow(ctx, sp, opcode))
                        goto exception;
                    sp--;
                }
            }
                BREAK;
            CASE(OP_and):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    sp[-2] = JS_NewInt32(ctx,
                                         JS_VALUE_GET_INT(op1) &
                                         JS_VALUE_GET_INT(op2));
                    sp--;
                } else {
                    if (js_binary_logic_slow(ctx, sp, opcode))
                        goto exception;
                    sp--;
                }
            }
                BREAK;
            CASE(OP_or):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    sp[-2] = JS_NewInt32(ctx,
                                         JS_VALUE_GET_INT(op1) |
                                         JS_VALUE_GET_INT(op2));
                    sp--;
                } else {
                    if (js_binary_logic_slow(ctx, sp, opcode))
                        goto exception;
                    sp--;
                }
            }
                BREAK;
            CASE(OP_xor):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    sp[-2] = JS_NewInt32(ctx,
                                         JS_VALUE_GET_INT(op1) ^
                                         JS_VALUE_GET_INT(op2));
                    sp--;
                } else {
                    if (js_binary_logic_slow(ctx, sp, opcode))
                        goto exception;
                    sp--;
                }
            }
                BREAK;


#define OP_CMP(opcode, binary_op, slow_call)              \
            CASE(opcode):                                 \
                {                                         \
                JSValue op1, op2;                         \
                op1 = sp[-2];                             \
                op2 = sp[-1];                                   \
                if (likely(JS_VALUE_IS_BOTH_INT(op1, op2))) {           \
                    sp[-2] = JS_NewBool(ctx, JS_VALUE_GET_INT(op1) binary_op JS_VALUE_GET_INT(op2)); \
                    sp--;                                               \
                } else {                                                \
                    if (slow_call)                                      \
                        goto exception;                                 \
                    sp--;                                               \
                }                                                       \
                }                                                       \
            BREAK

            OP_CMP(OP_lt, <, js_relational_slow(ctx, sp, opcode));
            OP_CMP(OP_lte, <=, js_relational_slow(ctx, sp, opcode));
            OP_CMP(OP_gt, >, js_relational_slow(ctx, sp, opcode));
            OP_CMP(OP_gte, >=, js_relational_slow(ctx, sp, opcode));
            OP_CMP(OP_eq, ==, js_eq_slow(ctx, sp, 0));
            OP_CMP(OP_neq, !=, js_eq_slow(ctx, sp, 1));
            OP_CMP(OP_strict_eq, ==, js_strict_eq_slow(ctx, sp, 0));
            OP_CMP(OP_strict_neq, !=, js_strict_eq_slow(ctx, sp, 1));

#ifdef CONFIG_BIGNUM
                CASE(OP_mul_pow10):
            if (rt->bigfloat_ops.mul_pow10(ctx, sp))
                goto exception;
            sp--;
            BREAK;
#endif
            CASE(OP_in):
                if (js_operator_in(ctx, sp))
                    goto exception;
                sp--;
                BREAK;
            CASE(OP_instanceof):
                if (js_operator_instanceof(ctx, sp))
                    goto exception;
                sp--;
                BREAK;
            CASE(OP_typeof):
            {
                JSValue op1;
                JSAtom atom;

                op1 = sp[-1];
                atom = js_operator_typeof(ctx, op1);
                JS_FreeValue(ctx, op1);
                sp[-1] = JS_AtomToString(ctx, atom);
            }
                BREAK;
            CASE(OP_delete):
                if (js_operator_delete(ctx, sp))
                    goto exception;
                sp--;
                BREAK;
            CASE(OP_delete_var):
            {
                JSAtom atom;
                int ret;

                atom = get_u32(pc);
                pc += 4;

                ret = JS_DeleteProperty(ctx, ctx->global_obj, atom, 0);
                if (unlikely(ret < 0))
                    goto exception;
                *sp++ = JS_NewBool(ctx, ret);
            }
                BREAK;

            CASE(OP_to_object):
                if (JS_VALUE_GET_TAG(sp[-1]) != JS_TAG_OBJECT) {
                    ret_val = JS_ToObject(ctx, sp[-1]);
                    if (JS_IsException(ret_val))
                        goto exception;
                    JS_FreeValue(ctx, sp[-1]);
                    sp[-1] = ret_val;
                }
                BREAK;

            CASE(OP_to_propkey):
                switch (JS_VALUE_GET_TAG(sp[-1])) {
                    case JS_TAG_INT:
                    case JS_TAG_STRING:
                    case JS_TAG_SYMBOL:
                        break;
                    default:
                        ret_val = JS_ToPropertyKey(ctx, sp[-1]);
                        if (JS_IsException(ret_val))
                            goto exception;
                        JS_FreeValue(ctx, sp[-1]);
                        sp[-1] = ret_val;
                        break;
                }
                BREAK;

            CASE(OP_to_propkey2):
                /* must be tested first */
                if (unlikely(JS_IsUndefined(sp[-2]) || JS_IsNull(sp[-2]))) {
                    JS_ThrowTypeError(ctx, "value has no property");
                    goto exception;
                }
                switch (JS_VALUE_GET_TAG(sp[-1])) {
                    case JS_TAG_INT:
                    case JS_TAG_STRING:
                    case JS_TAG_SYMBOL:
                        break;
                    default:
                        ret_val = JS_ToPropertyKey(ctx, sp[-1]);
                        if (JS_IsException(ret_val))
                            goto exception;
                        JS_FreeValue(ctx, sp[-1]);
                        sp[-1] = ret_val;
                        break;
                }
                BREAK;
#if 0
                CASE(OP_to_string):
            if (JS_VALUE_GET_TAG(sp[-1]) != JS_TAG_STRING) {
                ret_val = JS_ToString(ctx, sp[-1]);
                if (JS_IsException(ret_val))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = ret_val;
            }
            BREAK;
#endif
            CASE(OP_with_get_var):
            CASE(OP_with_put_var):
            CASE(OP_with_delete_var):
            CASE(OP_with_make_ref):
            CASE(OP_with_get_ref):
            CASE(OP_with_get_ref_undef):
            {
                JSAtom atom;
                int32_t diff;
                JSValue obj, val;
                int ret, is_with;
                atom = get_u32(pc);
                diff = get_u32(pc + 4);
                is_with = pc[8];
                pc += 9;

                obj = sp[-1];
                ret = JS_HasProperty(ctx, obj, atom);
                if (unlikely(ret < 0))
                    goto exception;
                if (ret) {
                    if (is_with) {
                        ret = js_has_unscopable(ctx, obj, atom);
                        if (unlikely(ret < 0))
                            goto exception;
                        if (ret)
                            goto no_with;
                    }
                    switch (opcode) {
                        case OP_with_get_var:
                            val = JS_GetProperty(ctx, obj, atom);
                            if (unlikely(JS_IsException(val)))
                                goto exception;
                            set_value(ctx, &sp[-1], val);
                            break;
                        case OP_with_put_var:
                            ret = JS_SetPropertyInternal(ctx, obj, atom, sp[-2],
                                                         JS_PROP_THROW_STRICT);
                            JS_FreeValue(ctx, sp[-1]);
                            sp -= 2;
                            if (unlikely(ret < 0))
                                goto exception;
                            break;
                        case OP_with_delete_var:
                            ret = JS_DeleteProperty(ctx, obj, atom, 0);
                            if (unlikely(ret < 0))
                                goto exception;
                            JS_FreeValue(ctx, sp[-1]);
                            sp[-1] = JS_NewBool(ctx, ret);
                            break;
                        case OP_with_make_ref:
                            /* produce a pair object/propname on the stack */
                            *sp++ = JS_AtomToValue(ctx, atom);
                            break;
                        case OP_with_get_ref:
                            /* produce a pair object/method on the stack */
                            val = JS_GetProperty(ctx, obj, atom);
                            if (unlikely(JS_IsException(val)))
                                goto exception;
                            *sp++ = val;
                            break;
                        case OP_with_get_ref_undef:
                            /* produce a pair undefined/function on the stack */
                            val = JS_GetProperty(ctx, obj, atom);
                            if (unlikely(JS_IsException(val)))
                                goto exception;
                            JS_FreeValue(ctx, sp[-1]);
                            sp[-1] = JS_UNDEFINED;
                            *sp++ = val;
                            break;
                    }
                    pc += diff - 5;
                } else {
                    no_with:
                    /* if not jumping, drop the object argument */
                    JS_FreeValue(ctx, sp[-1]);
                    sp--;
                }
            }
                BREAK;

            CASE(OP_await):
                ret_val = JS_NewInt32(ctx, FUNC_RET_AWAIT);
                goto done_generator;
            CASE(OP_yield):
                ret_val = JS_NewInt32(ctx, FUNC_RET_YIELD);
                goto done_generator;
            CASE(OP_yield_star):
            CASE(OP_async_yield_star):
                ret_val = JS_NewInt32(ctx, FUNC_RET_YIELD_STAR);
                goto done_generator;
            CASE(OP_return_async):
            CASE(OP_initial_yield):
                ret_val = JS_UNDEFINED;
                goto done_generator;

            CASE(OP_nop):
                BREAK;
            CASE(OP_is_undefined_or_null):
                if (JS_VALUE_GET_TAG(sp[-1]) == JS_TAG_UNDEFINED ||
                    JS_VALUE_GET_TAG(sp[-1]) == JS_TAG_NULL) {
                    goto set_true;
                } else {
                    goto free_and_set_false;
                }
#if SHORT_OPCODES
                CASE(OP_is_undefined):
            if (JS_VALUE_GET_TAG(sp[-1]) == JS_TAG_UNDEFINED) {
                goto set_true;
            } else {
                goto free_and_set_false;
            }
        CASE(OP_is_null):
            if (JS_VALUE_GET_TAG(sp[-1]) == JS_TAG_NULL) {
                goto set_true;
            } else {
                goto free_and_set_false;
            }
        CASE(OP_is_function):
            if (js_operator_typeof(ctx, sp[-1]) == JS_ATOM_function) {
                goto free_and_set_true;
            } else {
                goto free_and_set_false;
            }
        free_and_set_true:
            JS_FreeValue(ctx, sp[-1]);
#endif
            set_true:
                sp[-1] = JS_TRUE;
                BREAK;
            free_and_set_false:
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = JS_FALSE;
                BREAK;
            CASE(OP_invalid):
            DEFAULT:
                JS_ThrowInternalError(ctx, "invalid opcode: pc=%u opcode=0x%02x",
                                      (int)(pc - b->byte_code_buf - 1), opcode);
                goto exception;
        }
    }
    exception:
    if (is_backtrace_needed(ctx, rt->current_exception)) {
        /* add the backtrace information now (it is not done
           before if the exception happens in a bytecode
           operation */
        sf->cur_pc = pc;
        build_backtrace(ctx, rt->current_exception, NULL, 0, 0);
    }
    if (!JS_IsUncatchableError(ctx, rt->current_exception)) {
        while (sp > stack_buf) {
            JSValue val = *--sp;
            JS_FreeValue(ctx, val);
            if (JS_VALUE_GET_TAG(val) == JS_TAG_CATCH_OFFSET) {
                int pos = JS_VALUE_GET_INT(val);
                if (pos == 0) {
                    /* enumerator: close it with a throw */
                    JS_FreeValue(ctx, sp[-1]); /* drop the next method */
                    sp--;
                    JS_IteratorClose(ctx, sp[-1], TRUE);
                } else {
                    *sp++ = rt->current_exception;
                    rt->current_exception = JS_NULL;
                    pc = b->byte_code_buf + pos;
                    goto restart;
                }
            }
        }
    }
    ret_val = JS_EXCEPTION;
    /* the local variables are freed by the caller in the generator
       case. Hence the label 'done' should never be reached in a
       generator function. */
    if (b->func_kind != JS_FUNC_NORMAL) {
        done_generator:
        sf->cur_pc = pc;
        sf->cur_sp = sp;
    } else {
        done:
        if (unlikely(!list_empty(&sf->var_ref_list))) {
            /* variable references reference the stack: must close them */
            close_var_refs(rt, sf);
        }
        /* free the local variables and stack */
        for(pval = local_buf; pval < sp; pval++) {
            JS_FreeValue(ctx, *pval);
        }
    }
    rt->current_stack_frame = sf->prev_frame;
    return ret_val;
}

JSValue JS_Call(JSContext *ctx, JSValueConst func_obj, JSValueConst this_obj,
                int argc, JSValueConst *argv)
{
    return JS_CallInternal(ctx, func_obj, this_obj, JS_UNDEFINED,
                           argc, (JSValue *)argv, JS_CALL_FLAG_COPY_ARGV);
}

static JSValue JS_CallFree(JSContext *ctx, JSValue func_obj, JSValueConst this_obj,
                           int argc, JSValueConst *argv)
{
    JSValue res = JS_CallInternal(ctx, func_obj, this_obj, JS_UNDEFINED,
    argc, (JSValue *)argv, JS_CALL_FLAG_COPY_ARGV);
    JS_FreeValue(ctx, func_obj);
    return res;
}

/* warning: the refcount of the context is not incremented. Return
   NULL in case of exception (case of revoked proxy only) */
static JSContext *JS_GetFunctionRealm(JSContext *ctx, JSValueConst func_obj)
{
    JSObject *p;
    JSContext *realm;

    if (JS_VALUE_GET_TAG(func_obj) != JS_TAG_OBJECT)
        return ctx;
    p = JS_VALUE_GET_OBJ(func_obj);
    switch(p->class_id) {
        case JS_CLASS_C_FUNCTION:
            realm = p->u.cfunc.realm;
            break;
        case JS_CLASS_BYTECODE_FUNCTION:
        case JS_CLASS_GENERATOR_FUNCTION:
        case JS_CLASS_ASYNC_FUNCTION:
        case JS_CLASS_ASYNC_GENERATOR_FUNCTION:
        {
            JSFunctionBytecode *b;
            b = p->u.func.function_bytecode;
            realm = b->realm;
        }
            break;
        case JS_CLASS_PROXY:
        {
            JSProxyData *s = p->u.opaque;
            if (!s)
                return ctx;
            if (s->is_revoked) {
                JS_ThrowTypeErrorRevokedProxy(ctx);
                return NULL;
            } else {
                realm = JS_GetFunctionRealm(ctx, s->target);
            }
        }
            break;
        case JS_CLASS_BOUND_FUNCTION:
        {
            JSBoundFunction *bf = p->u.bound_function;
            realm = JS_GetFunctionRealm(ctx, bf->func_obj);
        }
            break;
        default:
            realm = ctx;
            break;
    }
    return realm;
}

static JSValue js_create_from_ctor(JSContext *ctx, JSValueConst ctor,
                                   int class_id)
{
    JSValue proto, obj;
    JSContext *realm;

    if (JS_IsUndefined(ctor)) {
        proto = JS_DupValue(ctx, ctx->class_proto[class_id]);
    } else {
        proto = JS_GetProperty(ctx, ctor, JS_ATOM_prototype);
        if (JS_IsException(proto))
            return proto;
        if (!JS_IsObject(proto)) {
            JS_FreeValue(ctx, proto);
            realm = JS_GetFunctionRealm(ctx, ctor);
            if (!realm)
                return JS_EXCEPTION;
            proto = JS_DupValue(ctx, realm->class_proto[class_id]);
        }
    }
    obj = JS_NewObjectProtoClass(ctx, proto, class_id);
    JS_FreeValue(ctx, proto);
    return obj;
}

/* argv[] is modified if (flags & JS_CALL_FLAG_COPY_ARGV) = 0. */
static JSValue JS_CallConstructorInternal(JSContext *ctx,
                                          JSValueConst func_obj,
                                          JSValueConst new_target,
                                          int argc, JSValue *argv, int flags)
{
    JSObject *p;
    JSFunctionBytecode *b;

    if (js_poll_interrupts(ctx))
        return JS_EXCEPTION;
    flags |= JS_CALL_FLAG_CONSTRUCTOR;
    if (unlikely(JS_VALUE_GET_TAG(func_obj) != JS_TAG_OBJECT))
        goto not_a_function;
    p = JS_VALUE_GET_OBJ(func_obj);
    if (unlikely(!p->is_constructor))
        return JS_ThrowTypeError(ctx, "not a constructor");
    if (unlikely(p->class_id != JS_CLASS_BYTECODE_FUNCTION)) {
        JSClassCall *call_func;
        call_func = ctx->rt->class_array[p->class_id].call;
        if (!call_func) {
            not_a_function:
            return JS_ThrowTypeError(ctx, "not a function");
        }
        return call_func(ctx, func_obj, new_target, argc,
                         (JSValueConst *)argv, flags);
    }

    b = p->u.func.function_bytecode;
    if (b->is_derived_class_constructor) {
        return JS_CallInternal(ctx, func_obj, JS_UNDEFINED, new_target, argc, argv, flags);
    } else {
        JSValue obj, ret;
        /* legacy constructor behavior */
        obj = js_create_from_ctor(ctx, new_target, JS_CLASS_OBJECT);
        if (JS_IsException(obj))
            return JS_EXCEPTION;
        ret = JS_CallInternal(ctx, func_obj, obj, new_target, argc, argv, flags);
        if (JS_VALUE_GET_TAG(ret) == JS_TAG_OBJECT ||
            JS_IsException(ret)) {
            JS_FreeValue(ctx, obj);
            return ret;
        } else {
            JS_FreeValue(ctx, ret);
            return obj;
        }
    }
}

JSValue JS_CallConstructor2(JSContext *ctx, JSValueConst func_obj,
                            JSValueConst new_target,
                            int argc, JSValueConst *argv)
{
    return JS_CallConstructorInternal(ctx, func_obj, new_target,
                                      argc, (JSValue *)argv,
            JS_CALL_FLAG_COPY_ARGV);
}

JSValue JS_CallConstructor(JSContext *ctx, JSValueConst func_obj,
                           int argc, JSValueConst *argv)
{
    return JS_CallConstructorInternal(ctx, func_obj, func_obj,
                                      argc, (JSValue *)argv,
            JS_CALL_FLAG_COPY_ARGV);
}

JSValue JS_Invoke(JSContext *ctx, JSValueConst this_val, JSAtom atom,
                  int argc, JSValueConst *argv)
{
    JSValue func_obj;
    func_obj = JS_GetProperty(ctx, this_val, atom);
    if (JS_IsException(func_obj))
        return func_obj;
    return JS_CallFree(ctx, func_obj, this_val, argc, argv);
}

static JSValue JS_InvokeFree(JSContext *ctx, JSValue this_val, JSAtom atom,
                             int argc, JSValueConst *argv)
{
    JSValue res = JS_Invoke(ctx, this_val, atom, argc, argv);
    JS_FreeValue(ctx, this_val);
    return res;
}

/* JSAsyncFunctionState (used by generator and async functions) */
static __exception int async_func_init(JSContext *ctx, JSAsyncFunctionState *s,
                                       JSValueConst func_obj, JSValueConst this_obj,
                                       int argc, JSValueConst *argv)
{
    JSObject *p;
    JSFunctionBytecode *b;
    JSStackFrame *sf;
    int local_count, i, arg_buf_len, n;

    sf = &s->frame;
    init_list_head(&sf->var_ref_list);
    p = JS_VALUE_GET_OBJ(func_obj);
    b = p->u.func.function_bytecode;
    sf->js_mode = b->js_mode;
    sf->cur_pc = b->byte_code_buf;
    arg_buf_len = max_int(b->arg_count, argc);
    local_count = arg_buf_len + b->var_count + b->stack_size;
    sf->arg_buf = js_malloc(ctx, sizeof(JSValue) * max_int(local_count, 1));
    if (!sf->arg_buf)
        return -1;
    sf->cur_func = JS_DupValue(ctx, func_obj);
    s->this_val = JS_DupValue(ctx, this_obj);
    s->argc = argc;
    sf->arg_count = arg_buf_len;
    sf->var_buf = sf->arg_buf + arg_buf_len;
    sf->cur_sp = sf->var_buf + b->var_count;
    for(i = 0; i < argc; i++)
        sf->arg_buf[i] = JS_DupValue(ctx, argv[i]);
    n = arg_buf_len + b->var_count;
    for(i = argc; i < n; i++)
        sf->arg_buf[i] = JS_UNDEFINED;
    return 0;
}

static void async_func_mark(JSRuntime *rt, JSAsyncFunctionState *s,
                            JS_MarkFunc *mark_func)
{
    JSStackFrame *sf;
    JSValue *sp;

    sf = &s->frame;
    JS_MarkValue(rt, sf->cur_func, mark_func);
    JS_MarkValue(rt, s->this_val, mark_func);
    if (sf->cur_sp) {
        /* if the function is running, cur_sp is not known so we
           cannot mark the stack. Marking the variables is not needed
           because a running function cannot be part of a removable
           cycle */
        for(sp = sf->arg_buf; sp < sf->cur_sp; sp++)
            JS_MarkValue(rt, *sp, mark_func);
    }
}

static void async_func_free(JSRuntime *rt, JSAsyncFunctionState *s)
{
    JSStackFrame *sf;
    JSValue *sp;

    sf = &s->frame;

    /* close the closure variables. */
    close_var_refs(rt, sf);

    if (sf->arg_buf) {
        /* cannot free the function if it is running */
        assert(sf->cur_sp != NULL);
        for(sp = sf->arg_buf; sp < sf->cur_sp; sp++) {
            JS_FreeValueRT(rt, *sp);
        }
        js_free_rt(rt, sf->arg_buf);
    }
    JS_FreeValueRT(rt, sf->cur_func);
    JS_FreeValueRT(rt, s->this_val);
}

static JSValue async_func_resume(JSContext *ctx, JSAsyncFunctionState *s)
{
    JSValue func_obj;

    if (js_check_stack_overflow(ctx->rt, 0))
        return JS_ThrowStackOverflow(ctx);

    /* the tag does not matter provided it is not an object */
    func_obj = JS_MKPTR(JS_TAG_INT, s);
    return JS_CallInternal(ctx, func_obj, s->this_val, JS_UNDEFINED,
                           s->argc, s->frame.arg_buf, JS_CALL_FLAG_GENERATOR);
}
#endif //QUICKJS_QUICKJS_GC_H
