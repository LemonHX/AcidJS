#ifndef QUICKJS_QUICKJS_SHAPE_H
#define QUICKJS_QUICKJS_SHAPE_H

static inline size_t get_shape_size(size_t hash_size, size_t prop_size)
{
    return hash_size * sizeof(uint32_t) + sizeof(JSShape) +
           prop_size * sizeof(JSShapeProperty);
}

static inline JSShape *get_shape_from_alloc(void *sh_alloc, size_t hash_size)
{
    return (JSShape *)(void *)((uint32_t *)sh_alloc + hash_size);
}

static inline void *get_alloc_from_shape(JSShape *sh)
{
    return JSSHAPE_HASH_OFFSET(sh, -((intptr_t)sh->prop_hash_mask + 1));
}

static inline JSShapeProperty *get_shape_prop(JSShape *sh)
{
    return sh->prop;
}

static int init_shape_hash(JSRuntime *rt)
{
    rt->shape_hash_bits = 4;   /* 16 shapes */
    rt->shape_hash_size = 1 << rt->shape_hash_bits;
    rt->shape_hash_count = 0;
    rt->shape_hash = js_mallocz_rt(rt, sizeof(rt->shape_hash[0]) *
                                       rt->shape_hash_size);
    if (!rt->shape_hash)
        return -1;
    return 0;
}

/* same magic hash multiplier as the Linux kernel */
static uint32_t shape_hash(uint32_t h, uint32_t val)
{
    return (h + val) * 0x9e370001;
}

/* truncate the shape hash to 'hash_bits' bits */
static uint32_t get_shape_hash(uint32_t h, int hash_bits)
{
    return h >> (32 - hash_bits);
}

static uint32_t shape_initial_hash(JSObject *proto)
{
    uint32_t h;
    h = shape_hash(1, (uintptr_t)proto);
    if (sizeof(proto) > 4)
        h = shape_hash(h, (uint64_t)(uintptr_t)proto >> 32);
    return h;
}

static int resize_shape_hash(JSRuntime *rt, int new_shape_hash_bits)
{
    int new_shape_hash_size, i;
    uint32_t h;
    JSShape **new_shape_hash, *sh, *sh_next;

    new_shape_hash_size = 1 << new_shape_hash_bits;
    new_shape_hash = js_mallocz_rt(rt, sizeof(rt->shape_hash[0]) *
                                       new_shape_hash_size);
    if (!new_shape_hash)
        return -1;
    for(i = 0; i < rt->shape_hash_size; i++) {
        for(sh = rt->shape_hash[i]; sh != NULL; sh = sh_next) {
            sh_next = sh->shape_hash_next;
            h = get_shape_hash(sh->hash, new_shape_hash_bits);
            sh->shape_hash_next = new_shape_hash[h];
            new_shape_hash[h] = sh;
        }
    }
    js_free_rt(rt, rt->shape_hash);
    rt->shape_hash_bits = new_shape_hash_bits;
    rt->shape_hash_size = new_shape_hash_size;
    rt->shape_hash = new_shape_hash;
    return 0;
}

static void js_shape_hash_link(JSRuntime *rt, JSShape *sh)
{
    uint32_t h;
    h = get_shape_hash(sh->hash, rt->shape_hash_bits);
    sh->shape_hash_next = rt->shape_hash[h];
    rt->shape_hash[h] = sh;
    rt->shape_hash_count++;
}

static void js_shape_hash_unlink(JSRuntime *rt, JSShape *sh)
{
    uint32_t h;
    JSShape **psh;

    h = get_shape_hash(sh->hash, rt->shape_hash_bits);
    psh = &rt->shape_hash[h];
    while (*psh != sh)
        psh = &(*psh)->shape_hash_next;
    *psh = sh->shape_hash_next;
    rt->shape_hash_count--;
}

/* create a new empty shape with prototype 'proto' */
static no_inline JSShape *js_new_shape2(JSContext *ctx, JSObject *proto,
                                        int hash_size, int prop_size)
{
    JSRuntime *rt = ctx->rt;
    void *sh_alloc;
    JSShape *sh;

/* resize the shape hash table if necessary */
    if (2 * (rt->shape_hash_count + 1) > rt->shape_hash_size) {
        resize_shape_hash(rt, rt->shape_hash_bits + 1);
    }

    sh_alloc = js_malloc(ctx, get_shape_size(hash_size, prop_size));
    if (!sh_alloc)
        return NULL;
    sh = get_shape_from_alloc(sh_alloc, hash_size);
    sh->header.ref_count = 1;
    add_gc_object(rt, &sh->header, JS_GC_OBJ_TYPE_SHAPE);
    if (proto)
        JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, proto));
    sh->proto = proto;
    memset(JSSHAPE_HASH_OFFSET(sh, -(intptr_t)hash_size), 0, sizeof(uint32_t) *
                                                             hash_size);
    sh->prop_hash_mask = hash_size - 1;
    sh->prop_size = prop_size;
    sh->prop_count = 0;
    sh->deleted_prop_count = 0;

/* insert in the hash table */
    sh->hash = shape_initial_hash(proto);
    sh->is_hashed = TRUE;
    sh->has_small_array_index = FALSE;
    js_shape_hash_link(ctx->rt, sh);
    return sh;
}

static JSShape *js_new_shape(JSContext *ctx, JSObject *proto)
{
    return js_new_shape2(ctx, proto, JS_PROP_INITIAL_HASH_SIZE,
                         JS_PROP_INITIAL_SIZE);
}

/* The shape is cloned. The new shape is not inserted in the shape
   hash table */
static JSShape *js_clone_shape(JSContext *ctx, JSShape *sh1)
{
    JSShape *sh;
    void *sh_alloc, *sh_alloc1;
    size_t size;
    JSShapeProperty *pr;
    uint32_t i, hash_size;

    hash_size = sh1->prop_hash_mask + 1;
    size = get_shape_size(hash_size, sh1->prop_size);
    sh_alloc = js_malloc(ctx, size);
    if (!sh_alloc)
        return NULL;
    sh_alloc1 = get_alloc_from_shape(sh1);
    memcpy(sh_alloc, sh_alloc1, size);
    sh = get_shape_from_alloc(sh_alloc, hash_size);
    sh->header.ref_count = 1;
    add_gc_object(ctx->rt, &sh->header, JS_GC_OBJ_TYPE_SHAPE);
    sh->is_hashed = FALSE;
    if (sh->proto) {
        JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, sh->proto));
    }
    for(i = 0, pr = get_shape_prop(sh); i < sh->prop_count; i++, pr++) {
        JS_DupAtom(ctx, pr->atom);
    }
    return sh;
}

static JSShape *js_dup_shape(JSShape *sh)
{
    sh->header.ref_count++;
    return sh;
}

static void js_free_shape0(JSRuntime *rt, JSShape *sh)
{
    uint32_t i;
    JSShapeProperty *pr;

    assert(sh->header.ref_count == 0);
    if (sh->is_hashed)
        js_shape_hash_unlink(rt, sh);
    if (sh->proto != NULL) {
        JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_OBJECT, sh->proto));
    }
    pr = get_shape_prop(sh);
    for(i = 0; i < sh->prop_count; i++) {
        JS_FreeAtomRT(rt, pr->atom);
        pr++;
    }
    remove_gc_object(&sh->header);
    js_free_rt(rt, get_alloc_from_shape(sh));
}

static void js_free_shape(JSRuntime *rt, JSShape *sh)
{
    if (unlikely(--sh->header.ref_count <= 0)) {
        js_free_shape0(rt, sh);
    }
}

static void js_free_shape_null(JSRuntime *rt, JSShape *sh)
{
    if (sh)
        js_free_shape(rt, sh);
}

/* make space to hold at least 'count' properties */
static no_inline int resize_properties(JSContext *ctx, JSShape **psh,
                                       JSObject *p, uint32_t count)
{
    JSShape *sh;
    uint32_t new_size, new_hash_size, new_hash_mask, i;
    JSShapeProperty *pr;
    void *sh_alloc;
    intptr_t h;

    sh = *psh;
    new_size = max_int(count, sh->prop_size * 3 / 2);
    /* Reallocate prop array first to avoid crash or size inconsistency
       in case of memory allocation failure */
    if (p) {
        JSProperty *new_prop;
        new_prop = js_realloc(ctx, p->prop, sizeof(new_prop[0]) * new_size);
        if (unlikely(!new_prop))
            return -1;
        p->prop = new_prop;
    }
    new_hash_size = sh->prop_hash_mask + 1;
    while (new_hash_size < new_size)
        new_hash_size = 2 * new_hash_size;
    if (new_hash_size != (sh->prop_hash_mask + 1)) {
        JSShape *old_sh;
        /* resize the hash table and the properties */
        old_sh = sh;
        sh_alloc = js_malloc(ctx, get_shape_size(new_hash_size, new_size));
        if (!sh_alloc)
            return -1;
        sh = get_shape_from_alloc(sh_alloc, new_hash_size);
        list_del(&old_sh->header.link);
        /* copy all the fields and the properties */
        memcpy(sh, old_sh,
               sizeof(JSShape) + sizeof(sh->prop[0]) * old_sh->prop_count);
        list_add_tail(&sh->header.link, &ctx->rt->gc_obj_list);
        new_hash_mask = new_hash_size - 1;
        sh->prop_hash_mask = new_hash_mask;
        memset(JSSHAPE_HASH_OFFSET(sh, -(intptr_t)new_hash_size), 0,
               sizeof(uint32_t) * new_hash_size);
        for(i = 0, pr = sh->prop; i < sh->prop_count; i++, pr++) {
            if (pr->atom != JS_ATOM_NULL) {
                h = ((uintptr_t)pr->atom & new_hash_mask);
                pr->hash_next = *JSSHAPE_HASH_OFFSET(sh, -h - 1);
                *JSSHAPE_HASH_OFFSET(sh, -h - 1) = i + 1;
            }
        }
        js_free(ctx, get_alloc_from_shape(old_sh));
    } else {
        /* only resize the properties */
        list_del(&sh->header.link);
        sh_alloc = js_realloc(ctx, get_alloc_from_shape(sh),
                              get_shape_size(new_hash_size, new_size));
        if (unlikely(!sh_alloc)) {
            /* insert again in the GC list */
            list_add_tail(&sh->header.link, &ctx->rt->gc_obj_list);
            return -1;
        }
        sh = get_shape_from_alloc(sh_alloc, new_hash_size);
        list_add_tail(&sh->header.link, &ctx->rt->gc_obj_list);
    }
    *psh = sh;
    sh->prop_size = new_size;
    return 0;
}

/* remove the deleted properties. */
static int compact_properties(JSContext *ctx, JSObject *p)
{
    JSShape *sh, *old_sh;
    void *sh_alloc;
    intptr_t h;
    uint32_t new_hash_size, i, j, new_hash_mask, new_size;
    JSShapeProperty *old_pr, *pr;
    JSProperty *prop, *new_prop;

    sh = p->shape;
    assert(!sh->is_hashed);

    new_size = max_int(JS_PROP_INITIAL_SIZE,
                       sh->prop_count - sh->deleted_prop_count);
    assert(new_size <= sh->prop_size);

    new_hash_size = sh->prop_hash_mask + 1;
    while ((new_hash_size / 2) >= new_size)
        new_hash_size = new_hash_size / 2;
    new_hash_mask = new_hash_size - 1;

    /* resize the hash table and the properties */
    old_sh = sh;
    sh_alloc = js_malloc(ctx, get_shape_size(new_hash_size, new_size));
    if (!sh_alloc)
        return -1;
    sh = get_shape_from_alloc(sh_alloc, new_hash_size);
    list_del(&old_sh->header.link);
    memcpy(sh, old_sh, sizeof(JSShape));
    list_add_tail(&sh->header.link, &ctx->rt->gc_obj_list);

    memset(JSSHAPE_HASH_OFFSET(sh, -(intptr_t)new_hash_size), 0,
           sizeof(uint32_t) * new_hash_size);

    j = 0;
    old_pr = old_sh->prop;
    pr = sh->prop;
    prop = p->prop;
    for(i = 0; i < sh->prop_count; i++) {
        if (old_pr->atom != JS_ATOM_NULL) {
            pr->atom = old_pr->atom;
            pr->flags = old_pr->flags;
            h = ((uintptr_t)old_pr->atom & new_hash_mask);
            pr->hash_next = *JSSHAPE_HASH_OFFSET(sh, -h - 1);
            *JSSHAPE_HASH_OFFSET(sh, -h - 1) = j + 1;
            prop[j] = prop[i];
            j++;
            pr++;
        }
        old_pr++;
    }
    assert(j == (sh->prop_count - sh->deleted_prop_count));
    sh->prop_hash_mask = new_hash_mask;
    sh->prop_size = new_size;
    sh->deleted_prop_count = 0;
    sh->prop_count = j;

    p->shape = sh;
    js_free(ctx, get_alloc_from_shape(old_sh));

    /* reduce the size of the object properties */
    new_prop = js_realloc(ctx, p->prop, sizeof(new_prop[0]) * new_size);
    if (new_prop)
        p->prop = new_prop;
    return 0;
}

static int add_shape_property(JSContext *ctx, JSShape **psh,
                              JSObject *p, JSAtom atom, int prop_flags)
{
    JSRuntime *rt = ctx->rt;
    JSShape *sh = *psh;
    JSShapeProperty *pr, *prop;
    uint32_t hash_mask, new_shape_hash = 0;
    intptr_t h;

    /* update the shape hash */
    if (sh->is_hashed) {
        js_shape_hash_unlink(rt, sh);
        new_shape_hash = shape_hash(shape_hash(sh->hash, atom), prop_flags);
    }

    if (unlikely(sh->prop_count >= sh->prop_size)) {
        if (resize_properties(ctx, psh, p, sh->prop_count + 1)) {
            /* in case of error, reinsert in the hash table.
               sh is still valid if resize_properties() failed */
            if (sh->is_hashed)
                js_shape_hash_link(rt, sh);
            return -1;
        }
        sh = *psh;
    }
    if (sh->is_hashed) {
        sh->hash = new_shape_hash;
        js_shape_hash_link(rt, sh);
    }
    /* Initialize the new shape property.
       The object property at p->prop[sh->prop_count] is uninitialized */
    prop = get_shape_prop(sh);
    pr = &prop[sh->prop_count++];
    pr->atom = JS_DupAtom(ctx, atom);
    pr->flags = prop_flags;
    sh->has_small_array_index |= __JS_AtomIsTaggedInt(atom);
    /* add in hash table */
    hash_mask = sh->prop_hash_mask;
    h = atom & hash_mask;
    pr->hash_next = *JSSHAPE_HASH_OFFSET(sh, -h - 1);
    *JSSHAPE_HASH_OFFSET(sh, -h - 1) = sh->prop_count;
    return 0;
}

/* find a hashed empty shape matching the prototype. Return NULL if
   not found */
static JSShape *find_hashed_shape_proto(JSRuntime *rt, JSObject *proto)
{
    JSShape *sh1;
    uint32_t h, h1;

    h = shape_initial_hash(proto);
    h1 = get_shape_hash(h, rt->shape_hash_bits);
    for(sh1 = rt->shape_hash[h1]; sh1 != NULL; sh1 = sh1->shape_hash_next) {
        if (sh1->hash == h &&
            sh1->proto == proto &&
            sh1->prop_count == 0) {
            return sh1;
        }
    }
    return NULL;
}

/* find a hashed shape matching sh + (prop, prop_flags). Return NULL if
   not found */
static JSShape *find_hashed_shape_prop(JSRuntime *rt, JSShape *sh,
                                       JSAtom atom, int prop_flags)
{
    JSShape *sh1;
    uint32_t h, h1, i, n;

    h = sh->hash;
    h = shape_hash(h, atom);
    h = shape_hash(h, prop_flags);
    h1 = get_shape_hash(h, rt->shape_hash_bits);
    for(sh1 = rt->shape_hash[h1]; sh1 != NULL; sh1 = sh1->shape_hash_next) {
        /* we test the hash first so that the rest is done only if the
           shapes really match */
        if (sh1->hash == h &&
            sh1->proto == sh->proto &&
            sh1->prop_count == ((n = sh->prop_count) + 1)) {
            for(i = 0; i < n; i++) {
                if (unlikely(sh1->prop[i].atom != sh->prop[i].atom) ||
                    unlikely(sh1->prop[i].flags != sh->prop[i].flags))
                    goto next;
            }
            if (unlikely(sh1->prop[n].atom != atom) ||
                unlikely(sh1->prop[n].flags != prop_flags))
                goto next;
            return sh1;
        }
        next: ;
    }
    return NULL;
}

static __maybe_unused void JS_DumpShape(JSRuntime *rt, int i, JSShape *sh)
{
    char atom_buf[ATOM_GET_STR_BUF_SIZE];
    int j;

    /* XXX: should output readable class prototype */
    printf("%5d %3d%c %14p %5d %5d", i,
           sh->header.ref_count, " *"[sh->is_hashed],
           (void *)sh->proto, sh->prop_size, sh->prop_count);
    for(j = 0; j < sh->prop_count; j++) {
        printf(" %s", JS_AtomGetStrRT(rt, atom_buf, sizeof(atom_buf),
                                      sh->prop[j].atom));
    }
    printf("\n");
}

static __maybe_unused void JS_DumpShapes(JSRuntime *rt)
{
    int i;
    JSShape *sh;
    struct list_head *el;
    JSObject *p;
    JSGCObjectHeader *gp;

    printf("JSShapes: {\n");
    printf("%5s %4s %14s %5s %5s %s\n", "SLOT", "REFS", "PROTO", "SIZE", "COUNT", "PROPS");
    for(i = 0; i < rt->shape_hash_size; i++) {
        for(sh = rt->shape_hash[i]; sh != NULL; sh = sh->shape_hash_next) {
            JS_DumpShape(rt, i, sh);
            assert(sh->is_hashed);
        }
    }
    /* dump non-hashed shapes */
    list_for_each(el, &rt->gc_obj_list) {
        gp = list_entry(el, JSGCObjectHeader, link);
        if (gp->gc_obj_type == JS_GC_OBJ_TYPE_JS_OBJECT) {
            p = (JSObject *)gp;
            if (!p->shape->is_hashed) {
                JS_DumpShape(rt, -1, p->shape);
            }
        }
    }
    printf("}\n");
}

static JSValue JS_NewObjectFromShape(JSContext *ctx, JSShape *sh, JSClassID class_id)
{
    JSObject *p;

    js_trigger_gc(ctx->rt, sizeof(JSObject));
    p = js_malloc(ctx, sizeof(JSObject));
    if (unlikely(!p))
        goto fail;
    p->class_id = class_id;
    p->extensible = TRUE;
    p->free_mark = 0;
    p->is_exotic = 0;
    p->fast_array = 0;
    p->is_constructor = 0;
    p->is_uncatchable_error = 0;
    p->tmp_mark = 0;
    p->first_weak_ref = NULL;
    p->u.opaque = NULL;
    p->shape = sh;
    p->prop = js_malloc(ctx, sizeof(JSProperty) * sh->prop_size);
    if (unlikely(!p->prop)) {
        js_free(ctx, p);
        fail:
        js_free_shape(ctx->rt, sh);
        return JS_EXCEPTION;
    }

    switch(class_id) {
        case JS_CLASS_OBJECT:
            break;
        case JS_CLASS_ARRAY:
        {
            JSProperty *pr;
            p->is_exotic = 1;
            p->fast_array = 1;
            p->u.array.u.values = NULL;
            p->u.array.count = 0;
            p->u.array.u1.size = 0;
            /* the length property is always the first one */
            if (likely(sh == ctx->array_shape)) {
                pr = &p->prop[0];
            } else {
                /* only used for the first array */
                /* cannot fail */
                pr = add_property(ctx, p, JS_ATOM_length,
                                  JS_PROP_WRITABLE | JS_PROP_LENGTH);
            }
            pr->u.value = JS_NewInt32(ctx, 0);
        }
            break;
        case JS_CLASS_C_FUNCTION:
            p->prop[0].u.value = JS_UNDEFINED;
            break;
        case JS_CLASS_ARGUMENTS:
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
            p->is_exotic = 1;
            p->fast_array = 1;
            p->u.array.u.ptr = NULL;
            p->u.array.count = 0;
            break;
        case JS_CLASS_DATAVIEW:
            p->u.array.u.ptr = NULL;
            p->u.array.count = 0;
            break;
        case JS_CLASS_NUMBER:
        case JS_CLASS_STRING:
        case JS_CLASS_BOOLEAN:
        case JS_CLASS_SYMBOL:
        case JS_CLASS_DATE:
#ifdef CONFIG_BIGNUM
            case JS_CLASS_BIG_INT:
    case JS_CLASS_BIG_FLOAT:
    case JS_CLASS_BIG_DECIMAL:
#endif
            p->u.object_data = JS_UNDEFINED;
            goto set_exotic;
        case JS_CLASS_REGEXP:
            p->u.regexp.pattern = NULL;
            p->u.regexp.bytecode = NULL;
            goto set_exotic;
        default:
        set_exotic:
            if (ctx->rt->class_array[class_id].exotic) {
                p->is_exotic = 1;
            }
            break;
    }
    p->header.ref_count = 1;
    add_gc_object(ctx->rt, &p->header, JS_GC_OBJ_TYPE_JS_OBJECT);
    return JS_MKPTR(JS_TAG_OBJECT, p);
}

static JSObject *get_proto_obj(JSValueConst proto_val)
{
    if (JS_VALUE_GET_TAG(proto_val) != JS_TAG_OBJECT)
        return NULL;
    else
        return JS_VALUE_GET_OBJ(proto_val);
}

/* WARNING: proto must be an object or JS_NULL */
JSValue JS_NewObjectProtoClass(JSContext *ctx, JSValueConst proto_val,
                               JSClassID class_id)
{
    JSShape *sh;
    JSObject *proto;

    proto = get_proto_obj(proto_val);
    sh = find_hashed_shape_proto(ctx->rt, proto);
    if (likely(sh)) {
        sh = js_dup_shape(sh);
    } else {
        sh = js_new_shape(ctx, proto);
        if (!sh)
            return JS_EXCEPTION;
    }
    return JS_NewObjectFromShape(ctx, sh, class_id);
}

#if 0
static JSValue JS_GetObjectData(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;

    if (JS_VALUE_GET_TAG(obj) == JS_TAG_OBJECT) {
        p = JS_VALUE_GET_OBJ(obj);
        switch(p->class_id) {
        case JS_CLASS_NUMBER:
        case JS_CLASS_STRING:
        case JS_CLASS_BOOLEAN:
        case JS_CLASS_SYMBOL:
        case JS_CLASS_DATE:
#ifdef CONFIG_BIGNUM
        case JS_CLASS_BIG_INT:
        case JS_CLASS_BIG_FLOAT:
        case JS_CLASS_BIG_DECIMAL:
#endif
            return JS_DupValue(ctx, p->u.object_data);
        }
    }
    return JS_UNDEFINED;
}
#endif

static int JS_SetObjectData(JSContext *ctx, JSValueConst obj, JSValue val)
{
    JSObject *p;

    if (JS_VALUE_GET_TAG(obj) == JS_TAG_OBJECT) {
        p = JS_VALUE_GET_OBJ(obj);
        switch(p->class_id) {
            case JS_CLASS_NUMBER:
            case JS_CLASS_STRING:
            case JS_CLASS_BOOLEAN:
            case JS_CLASS_SYMBOL:
            case JS_CLASS_DATE:
#ifdef CONFIG_BIGNUM
                case JS_CLASS_BIG_INT:
        case JS_CLASS_BIG_FLOAT:
        case JS_CLASS_BIG_DECIMAL:
#endif
                JS_FreeValue(ctx, p->u.object_data);
                p->u.object_data = val;
                return 0;
        }
    }
    JS_FreeValue(ctx, val);
    if (!JS_IsException(obj))
        JS_ThrowTypeError(ctx, "invalid object type");
    return -1;
}

JSValue JS_NewObjectClass(JSContext *ctx, int class_id)
{
    return JS_NewObjectProtoClass(ctx, ctx->class_proto[class_id], class_id);
}

JSValue JS_NewObjectProto(JSContext *ctx, JSValueConst proto)
{
    return JS_NewObjectProtoClass(ctx, proto, JS_CLASS_OBJECT);
}

JSValue JS_NewArray(JSContext *ctx)
{
    return JS_NewObjectFromShape(ctx, js_dup_shape(ctx->array_shape),
                                 JS_CLASS_ARRAY);
}

JSValue JS_NewObject(JSContext *ctx)
{
    /* inline JS_NewObjectClass(ctx, JS_CLASS_OBJECT); */
    return JS_NewObjectProtoClass(ctx, ctx->class_proto[JS_CLASS_OBJECT], JS_CLASS_OBJECT);
}

static void js_function_set_properties(JSContext *ctx, JSValueConst func_obj,
                                       JSAtom name, int len)
{
    /* ES6 feature non compatible with ES5.1: length is configurable */
    JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_length, JS_NewInt32(ctx, len),
                           JS_PROP_CONFIGURABLE);
    JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_name,
                           JS_AtomToString(ctx, name), JS_PROP_CONFIGURABLE);
}

static BOOL js_class_has_bytecode(JSClassID class_id)
{
    return (class_id == JS_CLASS_BYTECODE_FUNCTION ||
            class_id == JS_CLASS_GENERATOR_FUNCTION ||
            class_id == JS_CLASS_ASYNC_FUNCTION ||
            class_id == JS_CLASS_ASYNC_GENERATOR_FUNCTION);
}

/* return NULL without exception if not a function or no bytecode */
static JSFunctionBytecode *JS_GetFunctionBytecode(JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return NULL;
    p = JS_VALUE_GET_OBJ(val);
    if (!js_class_has_bytecode(p->class_id))
        return NULL;
    return p->u.func.function_bytecode;
}

static void js_method_set_home_object(JSContext *ctx, JSValueConst func_obj,
                                      JSValueConst home_obj)
{
    JSObject *p, *p1;
    JSFunctionBytecode *b;

    if (JS_VALUE_GET_TAG(func_obj) != JS_TAG_OBJECT)
        return;
    p = JS_VALUE_GET_OBJ(func_obj);
    if (!js_class_has_bytecode(p->class_id))
        return;
    b = p->u.func.function_bytecode;
    if (b->need_home_object) {
        p1 = p->u.func.home_object;
        if (p1) {
            JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p1));
        }
        if (JS_VALUE_GET_TAG(home_obj) == JS_TAG_OBJECT)
            p1 = JS_VALUE_GET_OBJ(JS_DupValue(ctx, home_obj));
        else
            p1 = NULL;
        p->u.func.home_object = p1;
    }
}

static JSValue js_get_function_name(JSContext *ctx, JSAtom name)
{
    JSValue name_str;

    name_str = JS_AtomToString(ctx, name);
    if (JS_AtomSymbolHasDescription(ctx, name)) {
        name_str = JS_ConcatString3(ctx, "[", name_str, "]");
    }
    return name_str;
}

/* Modify the name of a method according to the atom and
   'flags'. 'flags' is a bitmask of JS_PROP_HAS_GET and
   JS_PROP_HAS_SET. Also set the home object of the method.
   Return < 0 if exception. */
static int js_method_set_properties(JSContext *ctx, JSValueConst func_obj,
                                    JSAtom name, int flags, JSValueConst home_obj)
{
    JSValue name_str;

    name_str = js_get_function_name(ctx, name);
    if (flags & JS_PROP_HAS_GET) {
        name_str = JS_ConcatString3(ctx, "get ", name_str, "");
    } else if (flags & JS_PROP_HAS_SET) {
        name_str = JS_ConcatString3(ctx, "set ", name_str, "");
    }
    if (JS_IsException(name_str))
        return -1;
    if (JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_name, name_str,
                               JS_PROP_CONFIGURABLE) < 0)
        return -1;
    js_method_set_home_object(ctx, func_obj, home_obj);
    return 0;
}

/* Note: at least 'length' arguments will be readable in 'argv' */
static JSValue JS_NewCFunction3(JSContext *ctx, JSCFunction *func,
                                const char *name,
                                int length, JSCFunctionEnum cproto, int magic,
                                JSValueConst proto_val)
{
    JSValue func_obj;
    JSObject *p;
    JSAtom name_atom;

    func_obj = JS_NewObjectProtoClass(ctx, proto_val, JS_CLASS_C_FUNCTION);
    if (JS_IsException(func_obj))
        return func_obj;
    p = JS_VALUE_GET_OBJ(func_obj);
    p->u.cfunc.realm = JS_DupContext(ctx);
    p->u.cfunc.c_function.generic = func;
    p->u.cfunc.length = length;
    p->u.cfunc.cproto = cproto;
    p->u.cfunc.magic = magic;
    p->is_constructor = (cproto == JS_CFUNC_constructor ||
                         cproto == JS_CFUNC_constructor_magic ||
                         cproto == JS_CFUNC_constructor_or_func ||
                         cproto == JS_CFUNC_constructor_or_func_magic);
    if (!name)
        name = "";
    name_atom = JS_NewAtom(ctx, name);
    js_function_set_properties(ctx, func_obj, name_atom, length);
    JS_FreeAtom(ctx, name_atom);
    return func_obj;
}

/* Note: at least 'length' arguments will be readable in 'argv' */
JSValue JS_NewCFunction2(JSContext *ctx, JSCFunction *func,
                         const char *name,
                         int length, JSCFunctionEnum cproto, int magic)
{
    return JS_NewCFunction3(ctx, func, name, length, cproto, magic,
                            ctx->function_proto);
}

typedef struct JSCFunctionDataRecord {
    JSCFunctionData *func;
    uint8_t length;
    uint8_t data_len;
    uint16_t magic;
    JSValue data[0];
} JSCFunctionDataRecord;

static void js_c_function_data_finalizer(JSRuntime *rt, JSValue val)
{
    JSCFunctionDataRecord *s = JS_GetOpaque(val, JS_CLASS_C_FUNCTION_DATA);
    int i;

    if (s) {
        for(i = 0; i < s->data_len; i++) {
            JS_FreeValueRT(rt, s->data[i]);
        }
        js_free_rt(rt, s);
    }
}

static void js_c_function_data_mark(JSRuntime *rt, JSValueConst val,
                                    JS_MarkFunc *mark_func)
{
    JSCFunctionDataRecord *s = JS_GetOpaque(val, JS_CLASS_C_FUNCTION_DATA);
    int i;

    if (s) {
        for(i = 0; i < s->data_len; i++) {
            JS_MarkValue(rt, s->data[i], mark_func);
        }
    }
}

static JSValue js_c_function_data_call(JSContext *ctx, JSValueConst func_obj,
                                       JSValueConst this_val,
                                       int argc, JSValueConst *argv, int flags)
{
    JSCFunctionDataRecord *s = JS_GetOpaque(func_obj, JS_CLASS_C_FUNCTION_DATA);
    JSValueConst *arg_buf;
    int i;

    /* XXX: could add the function on the stack for debug */
    if (unlikely(argc < s->length)) {
        arg_buf = alloca(sizeof(arg_buf[0]) * s->length);
        for(i = 0; i < argc; i++)
            arg_buf[i] = argv[i];
        for(i = argc; i < s->length; i++)
            arg_buf[i] = JS_UNDEFINED;
    } else {
        arg_buf = argv;
    }

    return s->func(ctx, this_val, argc, arg_buf, s->magic, s->data);
}

JSValue JS_NewCFunctionData(JSContext *ctx, JSCFunctionData *func,
                            int length, int magic, int data_len,
                            JSValueConst *data)
{
    JSCFunctionDataRecord *s;
    JSValue func_obj;
    int i;

    func_obj = JS_NewObjectProtoClass(ctx, ctx->function_proto,
                                      JS_CLASS_C_FUNCTION_DATA);
    if (JS_IsException(func_obj))
        return func_obj;
    s = js_malloc(ctx, sizeof(*s) + data_len * sizeof(JSValue));
    if (!s) {
        JS_FreeValue(ctx, func_obj);
        return JS_EXCEPTION;
    }
    s->func = func;
    s->length = length;
    s->data_len = data_len;
    s->magic = magic;
    for(i = 0; i < data_len; i++)
        s->data[i] = JS_DupValue(ctx, data[i]);
    JS_SetOpaque(func_obj, s);
    js_function_set_properties(ctx, func_obj,
                               JS_ATOM_empty_string, length);
    return func_obj;
}

static JSContext *js_autoinit_get_realm(JSProperty *pr)
{
    return (JSContext *)(pr->u.init.realm_and_id & ~3);
}

static JSAutoInitIDEnum js_autoinit_get_id(JSProperty *pr)
{
    return pr->u.init.realm_and_id & 3;
}

static void js_autoinit_free(JSRuntime *rt, JSProperty *pr)
{
    JS_FreeContext(js_autoinit_get_realm(pr));
}

static void js_autoinit_mark(JSRuntime *rt, JSProperty *pr,
                             JS_MarkFunc *mark_func)
{
    mark_func(rt, &js_autoinit_get_realm(pr)->header);
}

static void free_property(JSRuntime *rt, JSProperty *pr, int prop_flags)
{
    if (unlikely(prop_flags & JS_PROP_TMASK)) {
        if ((prop_flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
            if (pr->u.getset.getter)
                JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.getter));
            if (pr->u.getset.setter)
                JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.setter));
        } else if ((prop_flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
            free_var_ref(rt, pr->u.var_ref);
        } else if ((prop_flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
            js_autoinit_free(rt, pr);
        }
    } else {
        JS_FreeValueRT(rt, pr->u.value);
    }
}

static force_inline JSShapeProperty *find_own_property1(JSObject *p,
                                                        JSAtom atom)
{
    JSShape *sh;
    JSShapeProperty *pr, *prop;
    intptr_t h;
    sh = p->shape;
    h = (uintptr_t)atom & sh->prop_hash_mask;
    h = *JSSHAPE_HASH_OFFSET(sh, -h - 1);
    prop = get_shape_prop(sh);
    while (h) {
        pr = &prop[h - 1];
        if (likely(pr->atom == atom)) {
            return pr;
        }
        h = pr->hash_next;
    }
    return NULL;
}

static force_inline JSShapeProperty *find_own_property(JSProperty **ppr,
                                                       JSObject *p,
                                                       JSAtom atom)
{
    JSShape *sh;
    JSShapeProperty *pr, *prop;
    intptr_t h;
    sh = p->shape;
    h = (uintptr_t)atom & sh->prop_hash_mask;
    h = *JSSHAPE_HASH_OFFSET(sh, -h - 1);
    prop = get_shape_prop(sh);
    while (h) {
        pr = &prop[h - 1];
        if (likely(pr->atom == atom)) {
            *ppr = &p->prop[h - 1];
/* the compiler should be able to assume that pr != NULL here */
            return pr;
        }
        h = pr->hash_next;
    }
    *ppr = NULL;
    return NULL;
}

/* indicate that the object may be part of a function prototype cycle */
static void set_cycle_flag(JSContext *ctx, JSValueConst obj)
{
}

static void free_var_ref(JSRuntime *rt, JSVarRef *var_ref)
{
    if (var_ref) {
        assert(var_ref->header.ref_count > 0);
        if (--var_ref->header.ref_count == 0) {
            if (var_ref->is_detached) {
                JS_FreeValueRT(rt, var_ref->value);
                remove_gc_object(&var_ref->header);
            } else {
                list_del(&var_ref->header.link); /* still on the stack */
            }
            js_free_rt(rt, var_ref);
        }
    }
}

static void js_array_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    int i;

    for(i = 0; i < p->u.array.count; i++) {
        JS_FreeValueRT(rt, p->u.array.u.values[i]);
    }
    js_free_rt(rt, p->u.array.u.values);
}

static void js_array_mark(JSRuntime *rt, JSValueConst val,
                          JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    int i;

    for(i = 0; i < p->u.array.count; i++) {
        JS_MarkValue(rt, p->u.array.u.values[i], mark_func);
    }
}

static void js_object_data_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JS_FreeValueRT(rt, p->u.object_data);
    p->u.object_data = JS_UNDEFINED;
}

static void js_object_data_mark(JSRuntime *rt, JSValueConst val,
                                JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JS_MarkValue(rt, p->u.object_data, mark_func);
}

static void js_c_function_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);

    if (p->u.cfunc.realm)
        JS_FreeContext(p->u.cfunc.realm);
}

static void js_c_function_mark(JSRuntime *rt, JSValueConst val,
                               JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);

    if (p->u.cfunc.realm)
        mark_func(rt, &p->u.cfunc.realm->header);
}

static void js_bytecode_function_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p1, *p = JS_VALUE_GET_OBJ(val);
    JSFunctionBytecode *b;
    JSVarRef **var_refs;
    int i;

    p1 = p->u.func.home_object;
    if (p1) {
        JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_OBJECT, p1));
    }
    b = p->u.func.function_bytecode;
    if (b) {
        var_refs = p->u.func.var_refs;
        if (var_refs) {
            for(i = 0; i < b->closure_var_count; i++)
                free_var_ref(rt, var_refs[i]);
            js_free_rt(rt, var_refs);
        }
        JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_FUNCTION_BYTECODE, b));
    }
}

static void js_bytecode_function_mark(JSRuntime *rt, JSValueConst val,
                                      JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSVarRef **var_refs = p->u.func.var_refs;
    JSFunctionBytecode *b = p->u.func.function_bytecode;
    int i;

    if (p->u.func.home_object) {
        JS_MarkValue(rt, JS_MKPTR(JS_TAG_OBJECT, p->u.func.home_object),
                     mark_func);
    }
    if (b) {
        if (var_refs) {
            for(i = 0; i < b->closure_var_count; i++) {
                JSVarRef *var_ref = var_refs[i];
                if (var_ref && var_ref->is_detached) {
                    mark_func(rt, &var_ref->header);
                }
            }
        }
        /* must mark the function bytecode because template objects may be
           part of a cycle */
        JS_MarkValue(rt, JS_MKPTR(JS_TAG_FUNCTION_BYTECODE, b), mark_func);
    }
}

static void js_bound_function_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSBoundFunction *bf = p->u.bound_function;
    int i;

    JS_FreeValueRT(rt, bf->func_obj);
    JS_FreeValueRT(rt, bf->this_val);
    for(i = 0; i < bf->argc; i++) {
        JS_FreeValueRT(rt, bf->argv[i]);
    }
    js_free_rt(rt, bf);
}

static void js_bound_function_mark(JSRuntime *rt, JSValueConst val,
                                   JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSBoundFunction *bf = p->u.bound_function;
    int i;

    JS_MarkValue(rt, bf->func_obj, mark_func);
    JS_MarkValue(rt, bf->this_val, mark_func);
    for(i = 0; i < bf->argc; i++)
        JS_MarkValue(rt, bf->argv[i], mark_func);
}

static void js_for_in_iterator_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSForInIterator *it = p->u.for_in_iterator;
    JS_FreeValueRT(rt, it->obj);
    js_free_rt(rt, it);
}

static void js_for_in_iterator_mark(JSRuntime *rt, JSValueConst val,
                                    JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSForInIterator *it = p->u.for_in_iterator;
    JS_MarkValue(rt, it->obj, mark_func);
}

static void free_object(JSRuntime *rt, JSObject *p)
{
    int i;
    JSClassFinalizer *finalizer;
    JSShape *sh;
    JSShapeProperty *pr;

    p->free_mark = 1; /* used to tell the object is invalid when
                         freeing cycles */
    /* free all the fields */
    sh = p->shape;
    pr = get_shape_prop(sh);
    for(i = 0; i < sh->prop_count; i++) {
        free_property(rt, &p->prop[i], pr->flags);
        pr++;
    }
    js_free_rt(rt, p->prop);
    /* as an optimization we destroy the shape immediately without
       putting it in gc_zero_ref_count_list */
    js_free_shape(rt, sh);

    /* fail safe */
    p->shape = NULL;
    p->prop = NULL;

    if (unlikely(p->first_weak_ref)) {
        reset_weak_ref(rt, p);
    }

    finalizer = rt->class_array[p->class_id].finalizer;
    if (finalizer)
        (*finalizer)(rt, JS_MKPTR(JS_TAG_OBJECT, p));

    /* fail safe */
    p->class_id = 0;
    p->u.opaque = NULL;
    p->u.func.var_refs = NULL;
    p->u.func.home_object = NULL;

    remove_gc_object(&p->header);
    if (rt->gc_phase == JS_GC_PHASE_REMOVE_CYCLES && p->header.ref_count != 0) {
        list_add_tail(&p->header.link, &rt->gc_zero_ref_count_list);
    } else {
        js_free_rt(rt, p);
    }
}

static void free_gc_object(JSRuntime *rt, JSGCObjectHeader *gp)
{
    switch(gp->gc_obj_type) {
        case JS_GC_OBJ_TYPE_JS_OBJECT:
            free_object(rt, (JSObject *)gp);
            break;
        case JS_GC_OBJ_TYPE_FUNCTION_BYTECODE:
            free_function_bytecode(rt, (JSFunctionBytecode *)gp);
            break;
        default:
            abort();
    }
}

static void free_zero_refcount(JSRuntime *rt)
{
    struct list_head *el;
    JSGCObjectHeader *p;

    rt->gc_phase = JS_GC_PHASE_DECREF;
    for(;;) {
        el = rt->gc_zero_ref_count_list.next;
        if (el == &rt->gc_zero_ref_count_list)
            break;
        p = list_entry(el, JSGCObjectHeader, link);
        assert(p->ref_count == 0);
        free_gc_object(rt, p);
    }
    rt->gc_phase = JS_GC_PHASE_NONE;
}

/* called with the ref_count of 'v' reaches zero. */
void __JS_FreeValueRT(JSRuntime *rt, JSValue v)
{
    uint32_t tag = JS_VALUE_GET_TAG(v);

#ifdef DUMP_FREE
    {
        printf("Freeing ");
        if (tag == JS_TAG_OBJECT) {
            JS_DumpObject(rt, JS_VALUE_GET_OBJ(v));
        } else {
            JS_DumpValueShort(rt, v);
            printf("\n");
        }
    }
#endif

    switch(tag) {
        case JS_TAG_STRING:
        {
            JSString *p = JS_VALUE_GET_STRING(v);
            if (p->atom_type) {
                JS_FreeAtomStruct(rt, p);
            } else {
#ifdef DUMP_LEAKS
                list_del(&p->link);
#endif
                js_free_rt(rt, p);
            }
        }
            break;
        case JS_TAG_OBJECT:
        case JS_TAG_FUNCTION_BYTECODE:
        {
            JSGCObjectHeader *p = JS_VALUE_GET_PTR(v);
            if (rt->gc_phase != JS_GC_PHASE_REMOVE_CYCLES) {
                list_del(&p->link);
                list_add(&p->link, &rt->gc_zero_ref_count_list);
                if (rt->gc_phase == JS_GC_PHASE_NONE) {
                    free_zero_refcount(rt);
                }
            }
        }
            break;
        case JS_TAG_MODULE:
            abort(); /* never freed here */
            break;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_INT:
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *bf = JS_VALUE_GET_PTR(v);
            bf_delete(&bf->num);
            js_free_rt(rt, bf);
        }
        break;
    case JS_TAG_BIG_DECIMAL:
        {
            JSBigDecimal *bf = JS_VALUE_GET_PTR(v);
            bfdec_delete(&bf->num);
            js_free_rt(rt, bf);
        }
        break;
#endif
        case JS_TAG_SYMBOL:
        {
            JSAtomStruct *p = JS_VALUE_GET_PTR(v);
            JS_FreeAtomStruct(rt, p);
        }
            break;
        default:
            printf("__JS_FreeValue: unknown tag=%d\n", tag);
            abort();
    }
}

void __JS_FreeValue(JSContext *ctx, JSValue v)
{
    __JS_FreeValueRT(ctx->rt, v);
}

#endif //QUICKJS_QUICKJS_SHAPE_H
