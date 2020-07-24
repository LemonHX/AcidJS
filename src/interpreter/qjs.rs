#![allow(dead_code, mutable_transmutes, non_camel_case_types, non_snake_case,
         non_upper_case_globals, unused_assignments, unused_mut)]
#![register_tool(c2rust)]
#![feature(c_variadic, const_raw_ptr_to_usize_cast, extern_types, main,
           ptr_wrapping_offset_from, register_tool)]
extern "C" {
    pub type __sFILEX;
    pub type JSAsyncGeneratorData;
    pub type JSAsyncFromSyncIteratorData;
    pub type JSPromiseFunctionData;
    pub type JSPromiseData;
    pub type JSGeneratorData;
    pub type JSRegExpStringIteratorData;
    pub type JSArrayIteratorData;
    pub type JSMapIteratorData;
    pub type JSMapState;
    pub type JSCFunctionDataRecord;
    pub type JSMapRecord;
    #[no_mangle]
    fn malloc(_: libc::c_ulong) -> *mut libc::c_void;
    #[no_mangle]
    fn free(_: *mut libc::c_void);
    #[no_mangle]
    fn realloc(_: *mut libc::c_void, _: libc::c_ulong) -> *mut libc::c_void;
    #[no_mangle]
    fn exit(_: libc::c_int) -> !;
    #[no_mangle]
    fn strtod(_: *const libc::c_char, _: *mut *mut libc::c_char)
     -> libc::c_double;
    #[no_mangle]
    static mut __stdoutp: *mut FILE;
    #[no_mangle]
    static mut __stderrp: *mut FILE;
    #[no_mangle]
    fn fprintf(_: *mut FILE, _: *const libc::c_char, _: ...) -> libc::c_int;
    #[no_mangle]
    fn perror(_: *const libc::c_char);
    #[no_mangle]
    fn printf(_: *const libc::c_char, _: ...) -> libc::c_int;
    #[no_mangle]
    fn putc(_: libc::c_int, _: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn strcmp(_: *const libc::c_char, _: *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn strlen(_: *const libc::c_char) -> libc::c_ulong;
    #[no_mangle]
    fn __assert_rtn(_: *const libc::c_char, _: *const libc::c_char,
                    _: libc::c_int, _: *const libc::c_char) -> !;
    #[no_mangle]
    fn clock() -> clock_t;
    #[no_mangle]
    fn malloc_size(ptr: *const libc::c_void) -> size_t;
    #[no_mangle]
    fn has_suffix(str: *const libc::c_char, suffix: *const libc::c_char)
     -> libc::c_int;
    #[no_mangle]
    fn JS_NewRuntime() -> *mut JSRuntime;
    #[no_mangle]
    fn JS_SetMemoryLimit(rt: *mut JSRuntime, limit: size_t);
    #[no_mangle]
    fn JS_SetMaxStackSize(rt: *mut JSRuntime, stack_size: size_t);
    #[no_mangle]
    fn JS_NewRuntime2(mf: *const JSMallocFunctions, opaque: *mut libc::c_void)
     -> *mut JSRuntime;
    #[no_mangle]
    fn JS_FreeRuntime(rt: *mut JSRuntime);
    #[no_mangle]
    fn JS_NewContext(rt: *mut JSRuntime) -> *mut JSContext;
    #[no_mangle]
    fn JS_FreeContext(s: *mut JSContext);
    #[no_mangle]
    fn js_free(ctx: *mut JSContext, ptr: *mut libc::c_void);
    #[no_mangle]
    fn JS_ComputeMemoryUsage(rt: *mut JSRuntime, s: *mut JSMemoryUsage);
    #[no_mangle]
    fn JS_DumpMemoryUsage(fp: *mut FILE, s: *const JSMemoryUsage,
                          rt: *mut JSRuntime);
    #[no_mangle]
    fn __JS_FreeValue(ctx: *mut JSContext, v: JSValue);
    #[no_mangle]
    fn JS_DetectModule(input: *const libc::c_char, input_len: size_t)
     -> libc::c_int;
    /* 'input' must be zero terminated i.e. input[input_len] = '\0'. */
    #[no_mangle]
    fn JS_Eval(ctx: *mut JSContext, input: *const libc::c_char,
               input_len: size_t, filename: *const libc::c_char,
               eval_flags: libc::c_int) -> JSValue;
    #[no_mangle]
    fn JS_EvalFunction(ctx: *mut JSContext, fun_obj: JSValue) -> JSValue;
    #[no_mangle]
    fn JS_SetHostPromiseRejectionTracker(rt: *mut JSRuntime,
                                         cb:
                                             Option<JSHostPromiseRejectionTracker>,
                                         opaque: *mut libc::c_void);
    /* module_normalize = NULL is allowed and invokes the default module
   filename normalizer */
    #[no_mangle]
    fn JS_SetModuleLoaderFunc(rt: *mut JSRuntime,
                              module_normalize: Option<JSModuleNormalizeFunc>,
                              module_loader: Option<JSModuleLoaderFunc>,
                              opaque: *mut libc::c_void);
    /*
 * QuickJS C library
 * 
 * Copyright (c) 2017-2018 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
    #[no_mangle]
    fn js_std_promise_rejection_tracker(ctx: *mut JSContext, promise: JSValue,
                                        reason: JSValue,
                                        is_handled: libc::c_int,
                                        opaque: *mut libc::c_void);
    #[no_mangle]
    fn js_module_loader(ctx: *mut JSContext, module_name: *const libc::c_char,
                        opaque: *mut libc::c_void) -> *mut JSModuleDef;
    #[no_mangle]
    fn js_module_set_import_meta(ctx: *mut JSContext, func_val: JSValue,
                                 use_realpath: libc::c_int,
                                 is_main: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn js_load_file(ctx: *mut JSContext, pbuf_len: *mut size_t,
                    filename: *const libc::c_char) -> *mut uint8_t;
    #[no_mangle]
    fn js_std_dump_error(ctx: *mut JSContext);
    #[no_mangle]
    fn js_std_free_handlers(rt: *mut JSRuntime);
    #[no_mangle]
    fn js_std_init_handlers(rt: *mut JSRuntime);
    #[no_mangle]
    fn js_std_loop(ctx: *mut JSContext);
    #[no_mangle]
    fn js_std_add_helpers(ctx: *mut JSContext, argc: libc::c_int,
                          argv: *mut *mut libc::c_char);
    #[no_mangle]
    fn js_init_module_os(ctx: *mut JSContext,
                         module_name: *const libc::c_char)
     -> *mut JSModuleDef;
    #[no_mangle]
    fn js_init_module_std(ctx: *mut JSContext,
                          module_name: *const libc::c_char)
     -> *mut JSModuleDef;
}
pub type __builtin_va_list = [__va_list_tag; 1];
#[derive(Copy, Clone)]
#[repr(C)]
pub struct __va_list_tag {
    pub gp_offset: libc::c_uint,
    pub fp_offset: libc::c_uint,
    pub overflow_arg_area: *mut libc::c_void,
    pub reg_save_area: *mut libc::c_void,
}
pub type __int64_t = libc::c_longlong;
pub type __darwin_size_t = libc::c_ulong;
pub type __darwin_va_list = __builtin_va_list;
pub type __darwin_clock_t = libc::c_ulong;
pub type __darwin_off_t = __int64_t;
pub type int8_t = libc::c_schar;
pub type int16_t = libc::c_short;
pub type int32_t = libc::c_int;
pub type int64_t = libc::c_longlong;
pub type uintptr_t = libc::c_ulong;
pub type size_t = __darwin_size_t;
pub type uint8_t = libc::c_uchar;
pub type uint16_t = libc::c_ushort;
pub type uint32_t = libc::c_uint;
pub type uint64_t = libc::c_ulonglong;
pub type va_list = __darwin_va_list;
pub type fpos_t = __darwin_off_t;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct __sbuf {
    pub _base: *mut libc::c_uchar,
    pub _size: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct __sFILE {
    pub _p: *mut libc::c_uchar,
    pub _r: libc::c_int,
    pub _w: libc::c_int,
    pub _flags: libc::c_short,
    pub _file: libc::c_short,
    pub _bf: __sbuf,
    pub _lbfsize: libc::c_int,
    pub _cookie: *mut libc::c_void,
    pub _close: Option<unsafe extern "C" fn(_: *mut libc::c_void)
                           -> libc::c_int>,
    pub _read: Option<unsafe extern "C" fn(_: *mut libc::c_void,
                                           _: *mut libc::c_char,
                                           _: libc::c_int) -> libc::c_int>,
    pub _seek: Option<unsafe extern "C" fn(_: *mut libc::c_void, _: fpos_t,
                                           _: libc::c_int) -> fpos_t>,
    pub _write: Option<unsafe extern "C" fn(_: *mut libc::c_void,
                                            _: *const libc::c_char,
                                            _: libc::c_int) -> libc::c_int>,
    pub _ub: __sbuf,
    pub _extra: *mut __sFILEX,
    pub _ur: libc::c_int,
    pub _ubuf: [libc::c_uchar; 3],
    pub _nbuf: [libc::c_uchar; 1],
    pub _lb: __sbuf,
    pub _blksize: libc::c_int,
    pub _offset: fpos_t,
}
pub type FILE = __sFILE;
pub type clock_t = __darwin_clock_t;
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct JSRuntime {
    pub mf: JSMallocFunctions,
    pub malloc_state: JSMallocState,
    pub rt_info: *const libc::c_char,
    pub atom_hash_size: libc::c_int,
    pub atom_count: libc::c_int,
    pub atom_size: libc::c_int,
    pub atom_count_resize: libc::c_int,
    pub atom_hash: *mut uint32_t,
    pub atom_array: *mut *mut JSAtomStruct,
    pub atom_free_index: libc::c_int,
    pub class_count: libc::c_int,
    pub class_array: *mut JSClass,
    pub context_list: list_head,
    pub gc_obj_list: list_head,
    pub gc_zero_ref_count_list: list_head,
    pub tmp_obj_list: list_head,
    #[bitfield(name = "gc_phase", ty = "JSGCPhaseEnum", bits = "0..=7")]
    pub gc_phase: [u8; 1],
    #[bitfield(padding)]
    pub c2rust_padding: [u8; 7],
    pub malloc_gc_threshold: size_t,
    pub stack_top: *const uint8_t,
    pub stack_size: size_t,
    pub current_exception: JSValue,
    #[bitfield(name = "in_out_of_memory", ty = "libc::c_int", bits = "0..=7")]
    pub in_out_of_memory: [u8; 1],
    #[bitfield(padding)]
    pub c2rust_padding_0: [u8; 7],
    pub current_stack_frame: *mut JSStackFrame,
    pub interrupt_handler: Option<JSInterruptHandler>,
    pub interrupt_opaque: *mut libc::c_void,
    pub host_promise_rejection_tracker: Option<JSHostPromiseRejectionTracker>,
    pub host_promise_rejection_tracker_opaque: *mut libc::c_void,
    pub job_list: list_head,
    pub module_normalize_func: Option<JSModuleNormalizeFunc>,
    pub module_loader_func: Option<JSModuleLoaderFunc>,
    pub module_loader_opaque: *mut libc::c_void,
    #[bitfield(name = "can_block", ty = "libc::c_int", bits = "0..=7")]
    pub can_block: [u8; 1],
    #[bitfield(padding)]
    pub c2rust_padding_1: [u8; 7],
    pub sab_funcs: JSSharedArrayBufferFunctions,
    pub shape_hash_bits: libc::c_int,
    pub shape_hash_size: libc::c_int,
    pub shape_hash_count: libc::c_int,
    pub shape_hash: *mut *mut JSShape,
    pub user_opaque: *mut libc::c_void,
}
/* XXX: could use an object instead to avoid the
   JS_TAG_ASYNC_FUNCTION tag for the GC */
/* must come first */
/* true if the async function state is valid */
/* binary operators */
/* unary operators */
/* self operators */
/* OperatorSet for a primitive type */
/* NULL if no operator is defined */
/* self operators */
/* used using resolution */
/* closure variable index */
/* if != NULL, reference to the variable */
/* for local export */
/* module for indirect export */
/* '*' if export ns from. not used for local
                          export after compilation */
/* exported variable name */
/* in req_module_entries */
/* closure variable index */
/* in req_module_entries */
/* must come first, 32-bit */
/* only used for JS modules */
/* only used for C modules */
/* temporary use during js_evaluate_module() */
/* true if evaluation yielded an exception. It is saved in
       eval_exception */
/* for import.meta */
/* JS_PROP_NORMAL */
/* JS_PROP_GETSET */
/* NULL if undefined */
/* NULL if undefined */
/* JS_PROP_VARREF */
/* JS_PROP_AUTOINIT */
/* in order to use only 2 pointers, we compress the realm
               and the init function pointer */
/* realm and init_id (JS_AUTOINIT_ID_x)
                                       in the 2 low bits */
/* must be a power of two */
/* 0 if last in list */
/* JS_PROP_XXX */
/* JS_ATOM_NULL = free property entry */
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSShape {
    pub prop_hash_end: [uint32_t; 0],
    pub header: JSGCObjectHeader,
    pub is_hashed: uint8_t,
    pub has_small_array_index: uint8_t,
    pub hash: uint32_t,
    pub prop_hash_mask: uint32_t,
    pub prop_size: libc::c_int,
    pub prop_count: libc::c_int,
    pub deleted_prop_count: libc::c_int,
    pub shape_hash_next: *mut JSShape,
    pub proto: *mut JSObject,
    pub prop: [JSShapeProperty; 0],
}
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct JSShapeProperty {
    #[bitfield(name = "hash_next", ty = "uint32_t", bits = "0..=25")]
    #[bitfield(name = "flags", ty = "uint32_t", bits = "26..=31")]
    pub hash_next_flags: [u8; 4],
    pub atom: JSAtom,
}
pub type JSAtom = uint32_t;
/* prop_size elements */
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSObject {
    pub c2rust_unnamed: C2RustUnnamed_12,
    pub shape: *mut JSShape,
    pub prop: *mut JSProperty,
    pub first_weak_ref: *mut JSMapRecord,
    pub u: C2RustUnnamed,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed {
    pub opaque: *mut libc::c_void,
    pub bound_function: *mut JSBoundFunction,
    pub c_function_data_record: *mut JSCFunctionDataRecord,
    pub for_in_iterator: *mut JSForInIterator,
    pub array_buffer: *mut JSArrayBuffer,
    pub typed_array: *mut JSTypedArray,
    pub map_state: *mut JSMapState,
    pub map_iterator_data: *mut JSMapIteratorData,
    pub array_iterator_data: *mut JSArrayIteratorData,
    pub regexp_string_iterator_data: *mut JSRegExpStringIteratorData,
    pub generator_data: *mut JSGeneratorData,
    pub proxy_data: *mut JSProxyData,
    pub promise_data: *mut JSPromiseData,
    pub promise_function_data: *mut JSPromiseFunctionData,
    pub async_function_data: *mut JSAsyncFunctionData,
    pub async_from_sync_iterator_data: *mut JSAsyncFromSyncIteratorData,
    pub async_generator_data: *mut JSAsyncGeneratorData,
    pub func: C2RustUnnamed_5,
    pub cfunc: C2RustUnnamed_4,
    pub array: C2RustUnnamed_1,
    pub regexp: JSRegExp,
    pub object_data: JSValue,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSValue {
    pub u: JSValueUnion,
    pub tag: int64_t,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union JSValueUnion {
    pub int32: int32_t,
    pub float64: libc::c_double,
    pub ptr: *mut libc::c_void,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSRegExp {
    pub pattern: *mut JSString,
    pub bytecode: *mut JSString,
}
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct JSString {
    pub header: JSRefCountHeader,
    #[bitfield(name = "len", ty = "uint32_t", bits = "0..=30")]
    #[bitfield(name = "is_wide_char", ty = "uint8_t", bits = "31..=31")]
    #[bitfield(name = "hash", ty = "uint32_t", bits = "32..=61")]
    #[bitfield(name = "atom_type", ty = "uint8_t", bits = "62..=63")]
    pub len_is_wide_char_hash_atom_type: [u8; 8],
    pub hash_next: uint32_t,
    pub u: C2RustUnnamed_0,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_0 {
    pub str8: [uint8_t; 0],
    pub str16: [uint16_t; 0],
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSRefCountHeader {
    pub ref_count: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_1 {
    pub u1: C2RustUnnamed_3,
    pub u: C2RustUnnamed_2,
    pub count: uint32_t,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_2 {
    pub values: *mut JSValue,
    pub ptr: *mut libc::c_void,
    pub int8_ptr: *mut int8_t,
    pub uint8_ptr: *mut uint8_t,
    pub int16_ptr: *mut int16_t,
    pub uint16_ptr: *mut uint16_t,
    pub int32_ptr: *mut int32_t,
    pub uint32_ptr: *mut uint32_t,
    pub int64_ptr: *mut int64_t,
    pub uint64_ptr: *mut uint64_t,
    pub float_ptr: *mut libc::c_float,
    pub double_ptr: *mut libc::c_double,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_3 {
    pub size: uint32_t,
    pub typed_array: *mut JSTypedArray,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSTypedArray {
    pub link: list_head,
    pub obj: *mut JSObject,
    pub buffer: *mut JSObject,
    pub offset: uint32_t,
    pub length: uint32_t,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct list_head {
    pub prev: *mut list_head,
    pub next: *mut list_head,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_4 {
    pub realm: *mut JSContext,
    pub c_function: JSCFunctionType,
    pub length: uint8_t,
    pub cproto: uint8_t,
    pub magic: int16_t,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union JSCFunctionType {
    pub generic: Option<JSCFunction>,
    pub generic_magic: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                   _: JSValue, _: libc::c_int,
                                                   _: *mut JSValue,
                                                   _: libc::c_int)
                                  -> JSValue>,
    pub constructor: Option<JSCFunction>,
    pub constructor_magic: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                       _: JSValue,
                                                       _: libc::c_int,
                                                       _: *mut JSValue,
                                                       _: libc::c_int)
                                      -> JSValue>,
    pub constructor_or_func: Option<JSCFunction>,
    pub f_f: Option<unsafe extern "C" fn(_: libc::c_double)
                        -> libc::c_double>,
    pub f_f_f: Option<unsafe extern "C" fn(_: libc::c_double,
                                           _: libc::c_double)
                          -> libc::c_double>,
    pub getter: Option<unsafe extern "C" fn(_: *mut JSContext, _: JSValue)
                           -> JSValue>,
    pub setter: Option<unsafe extern "C" fn(_: *mut JSContext, _: JSValue,
                                            _: JSValue) -> JSValue>,
    pub getter_magic: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                  _: JSValue, _: libc::c_int)
                                 -> JSValue>,
    pub setter_magic: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                  _: JSValue, _: JSValue,
                                                  _: libc::c_int) -> JSValue>,
    pub iterator_next: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                   _: JSValue, _: libc::c_int,
                                                   _: *mut JSValue,
                                                   _: *mut libc::c_int,
                                                   _: libc::c_int)
                                  -> JSValue>,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSContext {
    pub header: JSGCObjectHeader,
    pub rt: *mut JSRuntime,
    pub link: list_head,
    pub binary_object_count: uint16_t,
    pub binary_object_size: libc::c_int,
    pub array_shape: *mut JSShape,
    pub class_proto: *mut JSValue,
    pub function_proto: JSValue,
    pub function_ctor: JSValue,
    pub array_ctor: JSValue,
    pub regexp_ctor: JSValue,
    pub promise_ctor: JSValue,
    pub native_error_proto: [JSValue; 8],
    pub iterator_proto: JSValue,
    pub async_iterator_proto: JSValue,
    pub array_proto_values: JSValue,
    pub throw_type_error: JSValue,
    pub eval_obj: JSValue,
    pub global_obj: JSValue,
    pub global_var_obj: JSValue,
    pub random_state: uint64_t,
    pub interrupt_counter: libc::c_int,
    pub is_error_property_enabled: libc::c_int,
    pub loaded_modules: list_head,
    pub compile_regexp: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                    _: JSValue, _: JSValue)
                                   -> JSValue>,
    pub eval_internal: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                   _: JSValue,
                                                   _: *const libc::c_char,
                                                   _: size_t,
                                                   _: *const libc::c_char,
                                                   _: libc::c_int,
                                                   _: libc::c_int)
                                  -> JSValue>,
    pub user_opaque: *mut libc::c_void,
}
//
// Created by Admin on 2020/7/21.
//
/* number of different NativeError objects */
/* power of two */
/* resize hash table at this count */
/* 0 = none */
/* size of class_array */
/* list of JSContext.link */
/* list of JSGCObjectHeader.link. List of allocated GC objects (used
       by the garbage collector) */
/* list of JSGCObjectHeader.link. Used during JS_FreeValueRT() */
/* used during GC */
/* stack limitation */
/* in bytes */
/* true if inside an out of memory error, to avoid recursing */
/* list of JSJobEntry.link */
/* TRUE if Atomics.wait can block */
/* used to allocate, free and clone SharedArrayBuffers */
/* Shape hash table */
/* number of hashed shapes */
/* 0 means free entry */
/* pointers for exotic behavior, can be NULL if none are present */
/* NULL if first stack frame */
/* current function, JS_UNDEFINED if the frame is detached */
/* arguments */
/* variables */
/* list of JSVarRef.link */
/* only used in bytecode functions : PC of the
                        instruction after the call */
/* 0 or JS_MODE_MATH for C functions */
/* only used in generators. Current stack pointer value. NULL if
       the function is running. */
/* header for GC objects. GC objects are C data structures with a
   reference count that can reference other GC objects. JS Objects are
   a particular type of GC object. */
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct JSGCObjectHeader {
    pub ref_count: libc::c_int,
    #[bitfield(name = "gc_obj_type", ty = "JSGCObjectTypeEnum", bits =
               "0..=3")]
    #[bitfield(name = "mark", ty = "uint8_t", bits = "4..=7")]
    pub gc_obj_type_mark: [u8; 1],
    pub dummy1: uint8_t,
    pub dummy2: uint16_t,
    pub link: list_head,
}
pub type JSGCObjectTypeEnum = libc::c_uint;
pub const JS_GC_OBJ_TYPE_JS_CONTEXT: JSGCObjectTypeEnum = 5;
pub const JS_GC_OBJ_TYPE_ASYNC_FUNCTION: JSGCObjectTypeEnum = 4;
pub const JS_GC_OBJ_TYPE_VAR_REF: JSGCObjectTypeEnum = 3;
pub const JS_GC_OBJ_TYPE_SHAPE: JSGCObjectTypeEnum = 2;
pub const JS_GC_OBJ_TYPE_FUNCTION_BYTECODE: JSGCObjectTypeEnum = 1;
pub const JS_GC_OBJ_TYPE_JS_OBJECT: JSGCObjectTypeEnum = 0;
/* don't include the stack frames before this eval in the Error() backtraces */
pub type JSCFunction
    =
    unsafe extern "C" fn(_: *mut JSContext, _: JSValue, _: libc::c_int,
                         _: *mut JSValue) -> JSValue;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_5 {
    pub function_bytecode: *mut JSFunctionBytecode,
    pub var_refs: *mut *mut JSVarRef,
    pub home_object: *mut JSObject,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSVarRef {
    pub c2rust_unnamed: C2RustUnnamed_6,
    pub pvalue: *mut JSValue,
    pub value: JSValue,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_6 {
    pub header: JSGCObjectHeader,
    pub c2rust_unnamed: C2RustUnnamed_7,
}
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct C2RustUnnamed_7 {
    pub __gc_ref_count: libc::c_int,
    pub __gc_mark: uint8_t,
    #[bitfield(name = "is_detached", ty = "uint8_t", bits = "0..=0")]
    #[bitfield(name = "is_arg", ty = "uint8_t", bits = "1..=1")]
    pub is_detached_is_arg: [u8; 1],
    pub var_idx: uint16_t,
}
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct JSFunctionBytecode {
    pub header: JSGCObjectHeader,
    pub js_mode: uint8_t,
    #[bitfield(name = "has_prototype", ty = "uint8_t", bits = "0..=0")]
    #[bitfield(name = "has_simple_parameter_list", ty = "uint8_t", bits =
               "1..=1")]
    #[bitfield(name = "is_derived_class_constructor", ty = "uint8_t", bits =
               "2..=2")]
    #[bitfield(name = "need_home_object", ty = "uint8_t", bits = "3..=3")]
    #[bitfield(name = "func_kind", ty = "uint8_t", bits = "4..=5")]
    #[bitfield(name = "new_target_allowed", ty = "uint8_t", bits = "6..=6")]
    #[bitfield(name = "super_call_allowed", ty = "uint8_t", bits = "7..=7")]
    #[bitfield(name = "super_allowed", ty = "uint8_t", bits = "8..=8")]
    #[bitfield(name = "arguments_allowed", ty = "uint8_t", bits = "9..=9")]
    #[bitfield(name = "has_debug", ty = "uint8_t", bits = "10..=10")]
    #[bitfield(name = "backtrace_barrier", ty = "uint8_t", bits = "11..=11")]
    #[bitfield(name = "read_only_bytecode", ty = "uint8_t", bits = "12..=12")]
    pub has_prototype_has_simple_parameter_list_is_derived_class_constructor_need_home_object_func_kind_new_target_allowed_super_call_allowed_super_allowed_arguments_allowed_has_debug_backtrace_barrier_read_only_bytecode: [u8; 2],
    #[bitfield(padding)]
    pub c2rust_padding: [u8; 5],
    pub byte_code_buf: *mut uint8_t,
    pub byte_code_len: libc::c_int,
    pub func_name: JSAtom,
    pub vardefs: *mut JSVarDef,
    pub closure_var: *mut JSClosureVar,
    pub arg_count: uint16_t,
    pub var_count: uint16_t,
    pub defined_arg_count: uint16_t,
    pub stack_size: uint16_t,
    pub realm: *mut JSContext,
    pub cpool: *mut JSValue,
    pub cpool_count: libc::c_int,
    pub closure_var_count: libc::c_int,
    pub debug: C2RustUnnamed_8,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_8 {
    pub filename: JSAtom,
    pub line_num: libc::c_int,
    pub source_len: libc::c_int,
    pub pc2line_len: libc::c_int,
    pub pc2line_buf: *mut uint8_t,
    pub source: *mut libc::c_char,
}
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct JSClosureVar {
    #[bitfield(name = "is_local", ty = "uint8_t", bits = "0..=0")]
    #[bitfield(name = "is_arg", ty = "uint8_t", bits = "1..=1")]
    #[bitfield(name = "is_const", ty = "uint8_t", bits = "2..=2")]
    #[bitfield(name = "is_lexical", ty = "uint8_t", bits = "3..=3")]
    #[bitfield(name = "var_kind", ty = "uint8_t", bits = "4..=6")]
    pub is_local_is_arg_is_const_is_lexical_var_kind: [u8; 1],
    #[bitfield(padding)]
    pub c2rust_padding: [u8; 1],
    pub var_idx: uint16_t,
    pub var_name: JSAtom,
}
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct JSVarDef {
    pub var_name: JSAtom,
    pub scope_level: libc::c_int,
    pub scope_next: libc::c_int,
    #[bitfield(name = "is_func_var", ty = "uint8_t", bits = "0..=0")]
    #[bitfield(name = "is_const", ty = "uint8_t", bits = "1..=1")]
    #[bitfield(name = "is_lexical", ty = "uint8_t", bits = "2..=2")]
    #[bitfield(name = "is_captured", ty = "uint8_t", bits = "3..=3")]
    #[bitfield(name = "var_kind", ty = "uint8_t", bits = "4..=7")]
    #[bitfield(name = "func_pool_or_scope_idx", ty = "libc::c_int", bits =
               "8..=31")]
    pub is_func_var_is_const_is_lexical_is_captured_var_kind_func_pool_or_scope_idx: [u8; 4],
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSAsyncFunctionData {
    pub header: JSGCObjectHeader,
    pub resolving_funcs: [JSValue; 2],
    pub is_active: libc::c_int,
    pub func_state: JSAsyncFunctionState,
}
/* must come first */
/* corresponds to header.ref_count */
/* corresponds to header.mark/gc_obj_type */
/* 0 : the JSVarRef is on the stack. header.link is an element
               of JSStackFrame.var_ref_list.
               1 : the JSVarRef is detached. header.link has the normal meanning
            */
/* index of the corresponding function variable on
                                 the stack */
/* pointer to the value, either on the stack or
                        to 'value' */
/* used when the variable is no longer on the stack */
/* must be large enough to have a negligible runtime cost and small
   enough to call the interrupt callback often. */
/* must come first */
/* initial shape for Array objects */
/* global object */
/* contains the global let/const definitions */
/* when the counter reaches zero, JSRutime.interrupt_handler is called */
/* list of JSModuleDef.link */
/* if NULL, RegExp compilation is not supported */
/* if NULL, eval is not supported */
/* must come first, 32-bit */
/* 0 = 8 bits, 1 = 16 bits characters */
/* for JS_ATOM_TYPE_SYMBOL: hash = 0, atom_type = 3,
       for JS_ATOM_TYPE_PRIVATE: hash = 1, atom_type = 3
       XXX: could change encoding to have one more bit in hash */
/* != 0 if atom, JS_ATOM_TYPE_x */
/* atom_index for JS_ATOM_TYPE_SYMBOL */
/* 8 bit strings will get an extra null terminator */
/* see JSVarKindEnum */
/* 9 bits available */
/* is_local = TRUE: index to a normal variable of the
                    parent function. otherwise: index to a closure
                    variable of the parent function */
/* index into fd->scopes of the enclosing scope */
/* index into fd->vars of the last variable in this scope */
/* XXX: add more variable kinds here instead of using bit fields */
/* lexical var with function declaration */
/* lexical var with async/generator
                                 function declaration */
/* must come after JS_VAR_PRIVATE_GETTER */
/* must come after JS_VAR_PRIVATE_SETTER */
/* index into fd->scopes of this variable lexical scope */
/* index into fd->vars of the next variable in the
                        * same or enclosing lexical scope */
/* used for the function self reference */
/* see JSVarKindEnum */
/* only used during compilation: function pool index for lexical
       variables with var_kind =
       JS_VAR_FUNCTION_DECL/JS_VAR_NEW_FUNCTION_DECL or scope level of
       the definition of the 'var' variables (they have scope_level =
       0) */
/* only used during compilation */
/* for the encoding of the pc2line table */
/* must come first */
/* true if a prototype field is necessary */
/* true if home_object needs to be initialized */
/* stop backtrace on this function */
/* XXX: 4 bits available */
/* (self pointer) */
/* arguments + local variables (arg_count + var_count) (self pointer) */
/* list of variables in the closure (self pointer) */
/* for length function property */
/* maximum stack size */
/* function realm */
/* constant pool (self pointer) */
/* debug info, move to separate structure to save memory? */
/* also contains the flags */
/* 0 if detached */
/* if shared, the array buffer cannot be detached */
/* NULL if detached */
/* link to arraybuffer */
/* back pointer to the TypedArray/DataView object */
/* based array buffer */
/* offset in the array buffer */
/* length in the array buffer */
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSAsyncFunctionState {
    pub this_val: JSValue,
    pub argc: libc::c_int,
    pub throw_flag: libc::c_int,
    pub frame: JSStackFrame,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSStackFrame {
    pub prev_frame: *mut JSStackFrame,
    pub cur_func: JSValue,
    pub arg_buf: *mut JSValue,
    pub var_buf: *mut JSValue,
    pub var_ref_list: list_head,
    pub cur_pc: *const uint8_t,
    pub arg_count: libc::c_int,
    pub js_mode: libc::c_int,
    pub cur_sp: *mut JSValue,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSProxyData {
    pub target: JSValue,
    pub handler: JSValue,
    pub is_func: uint8_t,
    pub is_revoked: uint8_t,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSArrayBuffer {
    pub byte_length: libc::c_int,
    pub detached: uint8_t,
    pub shared: uint8_t,
    pub data: *mut uint8_t,
    pub array_list: list_head,
    pub opaque: *mut libc::c_void,
    pub free_func: Option<JSFreeArrayBufferDataFunc>,
}
pub type JSFreeArrayBufferDataFunc
    =
    unsafe extern "C" fn(_: *mut JSRuntime, _: *mut libc::c_void,
                         _: *mut libc::c_void) -> ();
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSForInIterator {
    pub obj: JSValue,
    pub is_array: libc::c_int,
    pub array_length: uint32_t,
    pub idx: uint32_t,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSBoundFunction {
    pub func_obj: JSValue,
    pub this_val: JSValue,
    pub argc: libc::c_int,
    pub argv: [JSValue; 0],
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSProperty {
    pub u: C2RustUnnamed_9,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_9 {
    pub value: JSValue,
    pub getset: C2RustUnnamed_11,
    pub var_ref: *mut JSVarRef,
    pub init: C2RustUnnamed_10,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_10 {
    pub realm_and_id: uintptr_t,
    pub opaque: *mut libc::c_void,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_11 {
    pub getter: *mut JSObject,
    pub setter: *mut JSObject,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_12 {
    pub header: JSGCObjectHeader,
    pub c2rust_unnamed: C2RustUnnamed_13,
}
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct C2RustUnnamed_13 {
    pub __gc_ref_count: libc::c_int,
    pub __gc_mark: uint8_t,
    #[bitfield(name = "extensible", ty = "uint8_t", bits = "0..=0")]
    #[bitfield(name = "free_mark", ty = "uint8_t", bits = "1..=1")]
    #[bitfield(name = "is_exotic", ty = "uint8_t", bits = "2..=2")]
    #[bitfield(name = "fast_array", ty = "uint8_t", bits = "3..=3")]
    #[bitfield(name = "is_constructor", ty = "uint8_t", bits = "4..=4")]
    #[bitfield(name = "is_uncatchable_error", ty = "uint8_t", bits = "5..=5")]
    #[bitfield(name = "tmp_mark", ty = "uint8_t", bits = "6..=6")]
    pub extensible_free_mark_is_exotic_fast_array_is_constructor_is_uncatchable_error_tmp_mark: [u8; 1],
    pub class_id: uint16_t,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSSharedArrayBufferFunctions {
    pub sab_alloc: Option<unsafe extern "C" fn(_: *mut libc::c_void,
                                               _: size_t)
                              -> *mut libc::c_void>,
    pub sab_free: Option<unsafe extern "C" fn(_: *mut libc::c_void,
                                              _: *mut libc::c_void) -> ()>,
    pub sab_dup: Option<unsafe extern "C" fn(_: *mut libc::c_void,
                                             _: *mut libc::c_void) -> ()>,
    pub sab_opaque: *mut libc::c_void,
}
pub type JSModuleLoaderFunc
    =
    unsafe extern "C" fn(_: *mut JSContext, _: *const libc::c_char,
                         _: *mut libc::c_void) -> *mut JSModuleDef;
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct JSModuleDef {
    pub header: JSRefCountHeader,
    pub module_name: JSAtom,
    pub link: list_head,
    pub req_module_entries: *mut JSReqModuleEntry,
    pub req_module_entries_count: libc::c_int,
    pub req_module_entries_size: libc::c_int,
    pub export_entries: *mut JSExportEntry,
    pub export_entries_count: libc::c_int,
    pub export_entries_size: libc::c_int,
    pub star_export_entries: *mut JSStarExportEntry,
    pub star_export_entries_count: libc::c_int,
    pub star_export_entries_size: libc::c_int,
    pub import_entries: *mut JSImportEntry,
    pub import_entries_count: libc::c_int,
    pub import_entries_size: libc::c_int,
    pub module_ns: JSValue,
    pub func_obj: JSValue,
    pub init_func: Option<JSModuleInitFunc>,
    #[bitfield(name = "resolved", ty = "libc::c_int", bits = "0..=7")]
    #[bitfield(name = "func_created", ty = "libc::c_int", bits = "8..=15")]
    #[bitfield(name = "instantiated", ty = "libc::c_int", bits = "16..=23")]
    #[bitfield(name = "evaluated", ty = "libc::c_int", bits = "24..=31")]
    #[bitfield(name = "eval_mark", ty = "libc::c_int", bits = "32..=39")]
    #[bitfield(name = "eval_has_exception", ty = "libc::c_int", bits =
               "40..=47")]
    pub resolved_func_created_instantiated_evaluated_eval_mark_eval_has_exception: [u8; 6],
    #[bitfield(padding)]
    pub c2rust_padding: [u8; 2],
    pub eval_exception: JSValue,
    pub meta_obj: JSValue,
}
/* C property definition */
/* XXX: should move outside union */
/* XXX: should move outside union */
/* Note: c++ does not like nested designators */
/* C module definition */
pub type JSModuleInitFunc
    =
    unsafe extern "C" fn(_: *mut JSContext, _: *mut JSModuleDef)
        -> libc::c_int;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSImportEntry {
    pub var_idx: libc::c_int,
    pub import_name: JSAtom,
    pub req_module_idx: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSStarExportEntry {
    pub req_module_idx: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSExportEntry {
    pub u: C2RustUnnamed_14,
    pub export_type: JSExportTypeEnum,
    pub local_name: JSAtom,
    pub export_name: JSAtom,
}
pub type JSExportTypeEnum = libc::c_uint;
pub const JS_EXPORT_TYPE_INDIRECT: JSExportTypeEnum = 1;
pub const JS_EXPORT_TYPE_LOCAL: JSExportTypeEnum = 0;
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_14 {
    pub local: C2RustUnnamed_15,
    pub req_module_idx: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_15 {
    pub var_idx: libc::c_int,
    pub var_ref: *mut JSVarRef,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSReqModuleEntry {
    pub module_name: JSAtom,
    pub module: *mut JSModuleDef,
}
/* return the module specifier (allocated with js_malloc()) or NULL if
   exception */
pub type JSModuleNormalizeFunc
    =
    unsafe extern "C" fn(_: *mut JSContext, _: *const libc::c_char,
                         _: *const libc::c_char, _: *mut libc::c_void)
        -> *mut libc::c_char;
/* is_handled = TRUE means that the rejection is handled */
pub type JSHostPromiseRejectionTracker
    =
    unsafe extern "C" fn(_: *mut JSContext, _: JSValue, _: JSValue,
                         _: libc::c_int, _: *mut libc::c_void) -> ();
/* return != 0 if the JS code needs to be interrupted */
pub type JSInterruptHandler
    =
    unsafe extern "C" fn(_: *mut JSRuntime, _: *mut libc::c_void)
        -> libc::c_int;
pub type JSGCPhaseEnum = libc::c_uint;
pub const JS_GC_PHASE_REMOVE_CYCLES: JSGCPhaseEnum = 2;
pub const JS_GC_PHASE_DECREF: JSGCPhaseEnum = 1;
pub const JS_GC_PHASE_NONE: JSGCPhaseEnum = 0;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSClass {
    pub class_id: uint32_t,
    pub class_name: JSAtom,
    pub finalizer: Option<JSClassFinalizer>,
    pub gc_mark: Option<JSClassGCMark>,
    pub call: Option<JSClassCall>,
    pub exotic: *const JSClassExoticMethods,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSClassExoticMethods {
    pub get_own_property: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                      _:
                                                          *mut JSPropertyDescriptor,
                                                      _: JSValue, _: JSAtom)
                                     -> libc::c_int>,
    pub get_own_property_names: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                            _:
                                                                *mut *mut JSPropertyEnum,
                                                            _: *mut uint32_t,
                                                            _: JSValue)
                                           -> libc::c_int>,
    pub delete_property: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                     _: JSValue, _: JSAtom)
                                    -> libc::c_int>,
    pub define_own_property: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                         _: JSValue,
                                                         _: JSAtom,
                                                         _: JSValue,
                                                         _: JSValue,
                                                         _: JSValue,
                                                         _: libc::c_int)
                                        -> libc::c_int>,
    pub has_property: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                  _: JSValue, _: JSAtom)
                                 -> libc::c_int>,
    pub get_property: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                  _: JSValue, _: JSAtom,
                                                  _: JSValue) -> JSValue>,
    pub set_property: Option<unsafe extern "C" fn(_: *mut JSContext,
                                                  _: JSValue, _: JSAtom,
                                                  _: JSValue, _: JSValue,
                                                  _: libc::c_int)
                                 -> libc::c_int>,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSPropertyEnum {
    pub is_enumerable: libc::c_int,
    pub atom: JSAtom,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSPropertyDescriptor {
    pub flags: libc::c_int,
    pub value: JSValue,
    pub getter: JSValue,
    pub setter: JSValue,
}
pub type JSClassCall
    =
    unsafe extern "C" fn(_: *mut JSContext, _: JSValue, _: JSValue,
                         _: libc::c_int, _: *mut JSValue, _: libc::c_int)
        -> JSValue;
pub type JSClassGCMark
    =
    unsafe extern "C" fn(_: *mut JSRuntime, _: JSValue,
                         _: Option<JS_MarkFunc>) -> ();
pub type JS_MarkFunc
    =
    unsafe extern "C" fn(_: *mut JSRuntime, _: *mut JSGCObjectHeader) -> ();
pub type JSClassFinalizer
    =
    unsafe extern "C" fn(_: *mut JSRuntime, _: JSValue) -> ();
pub type JSAtomStruct = JSString;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSMallocState {
    pub malloc_count: size_t,
    pub malloc_size: size_t,
    pub malloc_limit: size_t,
    pub opaque: *mut libc::c_void,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSMallocFunctions {
    pub js_malloc: Option<unsafe extern "C" fn(_: *mut JSMallocState,
                                               _: size_t)
                              -> *mut libc::c_void>,
    pub js_free: Option<unsafe extern "C" fn(_: *mut JSMallocState,
                                             _: *mut libc::c_void) -> ()>,
    pub js_realloc: Option<unsafe extern "C" fn(_: *mut JSMallocState,
                                                _: *mut libc::c_void,
                                                _: size_t)
                               -> *mut libc::c_void>,
    pub js_malloc_usable_size: Option<unsafe extern "C" fn(_:
                                                               *const libc::c_void)
                                          -> size_t>,
}
pub type C2RustUnnamed_16 = libc::c_int;
pub const JS_TAG_FLOAT64: C2RustUnnamed_16 = 7;
pub const JS_TAG_EXCEPTION: C2RustUnnamed_16 = 6;
pub const JS_TAG_CATCH_OFFSET: C2RustUnnamed_16 = 5;
pub const JS_TAG_UNINITIALIZED: C2RustUnnamed_16 = 4;
pub const JS_TAG_UNDEFINED: C2RustUnnamed_16 = 3;
pub const JS_TAG_NULL: C2RustUnnamed_16 = 2;
pub const JS_TAG_BOOL: C2RustUnnamed_16 = 1;
pub const JS_TAG_INT: C2RustUnnamed_16 = 0;
/* any larger tag is FLOAT64 if JS_NAN_BOXING */
/* used internally */
pub const JS_TAG_OBJECT: C2RustUnnamed_16 = -1;
/* used internally */
pub const JS_TAG_FUNCTION_BYTECODE: C2RustUnnamed_16 = -2;
pub const JS_TAG_MODULE: C2RustUnnamed_16 = -3;
pub const JS_TAG_STRING: C2RustUnnamed_16 = -7;
pub const JS_TAG_SYMBOL: C2RustUnnamed_16 = -8;
pub const JS_TAG_BIG_FLOAT: C2RustUnnamed_16 = -9;
pub const JS_TAG_BIG_INT: C2RustUnnamed_16 = -10;
/* first negative tag */
pub const JS_TAG_BIG_DECIMAL: C2RustUnnamed_16 = -11;
/* all tags with a reference count are negative */
pub const JS_TAG_FIRST: C2RustUnnamed_16 = -11;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSMemoryUsage {
    pub malloc_size: int64_t,
    pub malloc_limit: int64_t,
    pub memory_used_size: int64_t,
    pub malloc_count: int64_t,
    pub memory_used_count: int64_t,
    pub atom_count: int64_t,
    pub atom_size: int64_t,
    pub str_count: int64_t,
    pub str_size: int64_t,
    pub obj_count: int64_t,
    pub obj_size: int64_t,
    pub prop_count: int64_t,
    pub prop_size: int64_t,
    pub shape_count: int64_t,
    pub shape_size: int64_t,
    pub js_func_count: int64_t,
    pub js_func_size: int64_t,
    pub js_func_code_size: int64_t,
    pub js_func_pc2line_count: int64_t,
    pub js_func_pc2line_size: int64_t,
    pub c_func_count: int64_t,
    pub array_count: int64_t,
    pub fast_array_count: int64_t,
    pub fast_array_elements: int64_t,
    pub binary_object_count: int64_t,
    pub binary_object_size: int64_t,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct trace_malloc_data {
    pub base: *mut uint8_t,
}
#[inline]
unsafe extern "C" fn JS_IsException(mut v: JSValue) -> libc::c_int {
    return (v.tag as int32_t == JS_TAG_EXCEPTION as libc::c_int) as
               libc::c_int as libc::c_long as libc::c_int;
}
#[inline]
unsafe extern "C" fn JS_FreeValue(mut ctx: *mut JSContext, mut v: JSValue) {
    if v.tag as int32_t as libc::c_uint >=
           JS_TAG_FIRST as libc::c_int as libc::c_uint {
        let mut p: *mut JSRefCountHeader = v.u.ptr as *mut JSRefCountHeader;
        (*p).ref_count -= 1;
        if (*p).ref_count <= 0 as libc::c_int { __JS_FreeValue(ctx, v); }
    };
}
/*
 * QuickJS stand alone interpreter
 * 
 * Copyright (c) 2017-2020 Fabrice Bellard
 * Copyright (c) 2017-2020 Charlie Gordon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
//extern const uint8_t qjsc_repl[];
//extern const uint32_t qjsc_repl_size;
unsafe extern "C" fn eval_buf(mut ctx: *mut JSContext,
                              mut buf: *const libc::c_void,
                              mut buf_len: libc::c_int,
                              mut filename: *const libc::c_char,
                              mut eval_flags: libc::c_int) -> libc::c_int {
    let mut val: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut ret: libc::c_int = 0;
    if eval_flags & (3 as libc::c_int) << 0 as libc::c_int ==
           (1 as libc::c_int) << 0 as libc::c_int {
        /* for the modules, we compile then run to be able to set
           import.meta */
        val =
            JS_Eval(ctx, buf as *const libc::c_char, buf_len as size_t,
                    filename,
                    eval_flags | (1 as libc::c_int) << 5 as libc::c_int);
        if JS_IsException(val) == 0 {
            js_module_set_import_meta(ctx, val, 1 as libc::c_int,
                                      1 as libc::c_int);
            val = JS_EvalFunction(ctx, val)
        }
    } else {
        val =
            JS_Eval(ctx, buf as *const libc::c_char, buf_len as size_t,
                    filename, eval_flags)
    }
    if JS_IsException(val) != 0 {
        js_std_dump_error(ctx);
        ret = -(1 as libc::c_int)
    } else { ret = 0 as libc::c_int }
    JS_FreeValue(ctx, val);
    return ret;
}
unsafe extern "C" fn eval_file(mut ctx: *mut JSContext,
                               mut filename: *const libc::c_char,
                               mut module: libc::c_int) -> libc::c_int {
    let mut buf: *mut uint8_t = 0 as *mut uint8_t;
    let mut ret: libc::c_int = 0;
    let mut eval_flags: libc::c_int = 0;
    let mut buf_len: size_t = 0;
    buf = js_load_file(ctx, &mut buf_len, filename);
    if buf.is_null() { perror(filename); exit(1 as libc::c_int); }
    if module < 0 as libc::c_int {
        module =
            (has_suffix(filename,
                        b".mjs\x00" as *const u8 as *const libc::c_char) != 0
                 || JS_DetectModule(buf as *const libc::c_char, buf_len) != 0)
                as libc::c_int
    }
    if module != 0 {
        eval_flags = (1 as libc::c_int) << 0 as libc::c_int
    } else { eval_flags = (0 as libc::c_int) << 0 as libc::c_int }
    ret =
        eval_buf(ctx, buf as *const libc::c_void, buf_len as libc::c_int,
                 filename, eval_flags);
    js_free(ctx, buf as *mut libc::c_void);
    return ret;
}
#[inline]
unsafe extern "C" fn js_trace_malloc_ptr_offset(mut ptr: *mut uint8_t,
                                                mut dp:
                                                    *mut trace_malloc_data)
 -> libc::c_ulonglong {
    return ptr.wrapping_offset_from((*dp).base) as libc::c_long as
               libc::c_ulonglong;
}
/* default memory allocation functions with memory limitation */
#[inline]
unsafe extern "C" fn js_trace_malloc_usable_size(mut ptr: *mut libc::c_void)
 -> size_t {
    return malloc_size(ptr);
}
unsafe extern "C" fn js_trace_malloc_printf(mut s: *mut JSMallocState,
                                            mut fmt: *const libc::c_char,
                                            mut args: ...) {
    let mut ap: ::std::ffi::VaListImpl;
    let mut c: libc::c_int = 0;
    ap = args.clone();
    loop  {
        let fresh0 = fmt;
        fmt = fmt.offset(1);
        c = *fresh0 as libc::c_int;
        if !(c != '\u{0}' as i32) { break ; }
        if c == '%' as i32 {
            /* only handle %p and %zd */
            if *fmt as libc::c_int == 'p' as i32 {
                let mut ptr: *mut uint8_t =
                    ap.as_va_list().arg::<*mut libc::c_void>() as
                        *mut uint8_t;
                if ptr.is_null() {
                    printf(b"NULL\x00" as *const u8 as *const libc::c_char);
                } else {
                    printf(b"H%+06lld.%zd\x00" as *const u8 as
                               *const libc::c_char,
                           js_trace_malloc_ptr_offset(ptr,
                                                      (*s).opaque as
                                                          *mut trace_malloc_data),
                           js_trace_malloc_usable_size(ptr as
                                                           *mut libc::c_void));
                }
                fmt = fmt.offset(1);
                continue ;
            } else if *fmt.offset(0 as libc::c_int as isize) as libc::c_int ==
                          'z' as i32 &&
                          *fmt.offset(1 as libc::c_int as isize) as
                              libc::c_int == 'd' as i32 {
                let mut sz: size_t = ap.as_va_list().arg::<size_t>();
                printf(b"%zd\x00" as *const u8 as *const libc::c_char, sz);
                fmt = fmt.offset(2 as libc::c_int as isize);
                continue ;
            }
        }
        putc(c, __stdoutp);
    };
}
unsafe extern "C" fn js_trace_malloc_init(mut s: *mut trace_malloc_data) {
    (*s).base = malloc(8 as libc::c_int as libc::c_ulong) as *mut uint8_t;
    free((*s).base as *mut libc::c_void);
}
unsafe extern "C" fn js_trace_malloc(mut s: *mut JSMallocState,
                                     mut size: size_t) -> *mut libc::c_void {
    let mut ptr: *mut libc::c_void = 0 as *mut libc::c_void;
    /* Do not allocate zero bytes: behavior is platform dependent */
    if !(size != 0 as libc::c_int as libc::c_ulong) as libc::c_int as
           libc::c_long != 0 {
        __assert_rtn((*::std::mem::transmute::<&[u8; 16],
                                               &[libc::c_char; 16]>(b"js_trace_malloc\x00")).as_ptr(),
                     b"/Users/lemonhx/Desktop/Cpp/AcidJS/src/interpreter/qjs.c\x00"
                         as *const u8 as *const libc::c_char,
                     185 as libc::c_int,
                     b"size != 0\x00" as *const u8 as *const libc::c_char);
    } else { };
    if ((*s).malloc_size.wrapping_add(size) > (*s).malloc_limit) as
           libc::c_int as libc::c_long != 0 {
        return 0 as *mut libc::c_void
    }
    ptr = malloc(size);
    js_trace_malloc_printf(s,
                           b"A %zd -> %p\n\x00" as *const u8 as
                               *const libc::c_char, size, ptr);
    if !ptr.is_null() {
        (*s).malloc_count = (*s).malloc_count.wrapping_add(1);
        (*s).malloc_size =
            ((*s).malloc_size as
                 libc::c_ulong).wrapping_add(js_trace_malloc_usable_size(ptr).wrapping_add(0
                                                                                               as
                                                                                               libc::c_int
                                                                                               as
                                                                                               libc::c_ulong))
                as size_t as size_t
    }
    return ptr;
}
unsafe extern "C" fn js_trace_free(mut s: *mut JSMallocState,
                                   mut ptr: *mut libc::c_void) {
    if ptr.is_null() { return }
    js_trace_malloc_printf(s,
                           b"F %p\n\x00" as *const u8 as *const libc::c_char,
                           ptr);
    (*s).malloc_count = (*s).malloc_count.wrapping_sub(1);
    (*s).malloc_size =
        ((*s).malloc_size as
             libc::c_ulong).wrapping_sub(js_trace_malloc_usable_size(ptr).wrapping_add(0
                                                                                           as
                                                                                           libc::c_int
                                                                                           as
                                                                                           libc::c_ulong))
            as size_t as size_t;
    free(ptr);
}
unsafe extern "C" fn js_trace_realloc(mut s: *mut JSMallocState,
                                      mut ptr: *mut libc::c_void,
                                      mut size: size_t) -> *mut libc::c_void {
    let mut old_size: size_t = 0;
    if ptr.is_null() {
        if size == 0 as libc::c_int as libc::c_ulong {
            return 0 as *mut libc::c_void
        }
        return js_trace_malloc(s, size)
    }
    old_size = js_trace_malloc_usable_size(ptr);
    if size == 0 as libc::c_int as libc::c_ulong {
        js_trace_malloc_printf(s,
                               b"R %zd %p\n\x00" as *const u8 as
                                   *const libc::c_char, size, ptr);
        (*s).malloc_count = (*s).malloc_count.wrapping_sub(1);
        (*s).malloc_size =
            ((*s).malloc_size as
                 libc::c_ulong).wrapping_sub(old_size.wrapping_add(0 as
                                                                       libc::c_int
                                                                       as
                                                                       libc::c_ulong))
                as size_t as size_t;
        free(ptr);
        return 0 as *mut libc::c_void
    }
    if (*s).malloc_size.wrapping_add(size).wrapping_sub(old_size) >
           (*s).malloc_limit {
        return 0 as *mut libc::c_void
    }
    js_trace_malloc_printf(s,
                           b"R %zd %p\x00" as *const u8 as
                               *const libc::c_char, size, ptr);
    ptr = realloc(ptr, size);
    js_trace_malloc_printf(s,
                           b" -> %p\n\x00" as *const u8 as
                               *const libc::c_char, ptr);
    if !ptr.is_null() {
        (*s).malloc_size =
            ((*s).malloc_size as
                 libc::c_ulong).wrapping_add(js_trace_malloc_usable_size(ptr).wrapping_sub(old_size))
                as size_t as size_t
    }
    return ptr;
}
static mut trace_mf: JSMallocFunctions =
    unsafe {
        {
            let mut init =
                JSMallocFunctions{js_malloc:
                                      Some(js_trace_malloc as
                                               unsafe extern "C" fn(_:
                                                                        *mut JSMallocState,
                                                                    _: size_t)
                                                   -> *mut libc::c_void),
                                  js_free:
                                      Some(js_trace_free as
                                               unsafe extern "C" fn(_:
                                                                        *mut JSMallocState,
                                                                    _:
                                                                        *mut libc::c_void)
                                                   -> ()),
                                  js_realloc:
                                      Some(js_trace_realloc as
                                               unsafe extern "C" fn(_:
                                                                        *mut JSMallocState,
                                                                    _:
                                                                        *mut libc::c_void,
                                                                    _: size_t)
                                                   -> *mut libc::c_void),
                                  js_malloc_usable_size:
                                      Some(malloc_size as
                                               unsafe extern "C" fn(_:
                                                                        *const libc::c_void)
                                                   -> size_t),};
            init
        }
    };
#[no_mangle]
pub unsafe extern "C" fn help() {
    printf(b"QuickJS version 2020-07-05\nusage: qjs [options] [file [args]]\n-h  --help         list options\n-e  --eval EXPR    evaluate EXPR\n-i  --interactive  go to interactive mode\n-m  --module       load as ES6 module (default=autodetect)\n    --script       load as ES6 script (default=autodetect)\n-I  --include file include an additional file\n    --std          make \'std\' and \'os\' available to the loaded script\n-T  --trace        trace memory allocation\n-d  --dump         dump the memory usage stats\n    --memory-limit n       limit the memory usage to \'n\' bytes\n    --stack-size n         limit the stack size to \'n\' bytes\n    --unhandled-rejection  dump unhandled promise rejections\n-q  --quit         just instantiate the interpreter and quit\n\x00"
               as *const u8 as *const libc::c_char);
    exit(1 as libc::c_int);
}
unsafe fn main_0(mut argc: libc::c_int, mut argv: *mut *mut libc::c_char)
 -> libc::c_int {
    let mut current_block: u64;
    let mut rt: *mut JSRuntime = 0 as *mut JSRuntime;
    let mut ctx: *mut JSContext = 0 as *mut JSContext;
    let mut trace_data: trace_malloc_data =
        { let mut init = trace_malloc_data{base: 0 as *mut uint8_t,}; init };
    let mut optind: libc::c_int = 0;
    let mut expr: *mut libc::c_char = 0 as *mut libc::c_char;
    let mut interactive: libc::c_int = 0 as libc::c_int;
    let mut dump_memory: libc::c_int = 0 as libc::c_int;
    let mut trace_memory: libc::c_int = 0 as libc::c_int;
    let mut empty_run: libc::c_int = 0 as libc::c_int;
    let mut module: libc::c_int = -(1 as libc::c_int);
    let mut load_std: libc::c_int = 0 as libc::c_int;
    let mut dump_unhandled_promise_rejection: libc::c_int = 0 as libc::c_int;
    let mut memory_limit: size_t = 0 as libc::c_int as size_t;
    let mut include_list: [*mut libc::c_char; 32] =
        [0 as *mut libc::c_char; 32];
    let mut i: libc::c_int = 0;
    let mut include_count: libc::c_int = 0 as libc::c_int;
    let mut stack_size: size_t = 0 as libc::c_int as size_t;
    /* cannot use getopt because we want to pass the command line to
       the script */
    optind = 1 as libc::c_int;
    while optind < argc &&
              **argv.offset(optind as isize) as libc::c_int == '-' as i32 {
        let mut arg: *mut libc::c_char =
            (*argv.offset(optind as isize)).offset(1 as libc::c_int as isize);
        let mut longopt: *const libc::c_char =
            b"\x00" as *const u8 as *const libc::c_char;
        /* a single - is not an option, it also stops argument scanning */
        if *arg == 0 { break ; }
        optind += 1;
        if *arg as libc::c_int == '-' as i32 {
            longopt = arg.offset(1 as libc::c_int as isize);
            arg = arg.offset(strlen(arg) as isize);
            /* -- stops argument scanning */
            if *longopt == 0 { break ; }
        }
        while *arg as libc::c_int != 0 || *longopt as libc::c_int != 0 {
            let mut opt: libc::c_char = *arg;
            if opt != 0 { arg = arg.offset(1) }
            if opt as libc::c_int == 'h' as i32 ||
                   opt as libc::c_int == '?' as i32 ||
                   strcmp(longopt,
                          b"help\x00" as *const u8 as *const libc::c_char) ==
                       0 {
                help();
            } else if opt as libc::c_int == 'e' as i32 ||
                          strcmp(longopt,
                                 b"eval\x00" as *const u8 as
                                     *const libc::c_char) == 0 {
                if *arg != 0 {
                    expr = arg;
                    break ;
                } else if optind < argc {
                    let fresh1 = optind;
                    optind = optind + 1;
                    expr = *argv.offset(fresh1 as isize);
                    break ;
                } else {
                    fprintf(__stderrp,
                            b"qjs: missing expression for -e\n\x00" as
                                *const u8 as *const libc::c_char);
                    exit(2 as libc::c_int);
                }
            } else if opt as libc::c_int == 'I' as i32 ||
                          strcmp(longopt,
                                 b"include\x00" as *const u8 as
                                     *const libc::c_char) == 0 {
                if optind >= argc {
                    fprintf(__stderrp,
                            b"expecting filename\x00" as *const u8 as
                                *const libc::c_char);
                    exit(1 as libc::c_int);
                }
                if include_count as libc::c_ulong >=
                       (::std::mem::size_of::<[*mut libc::c_char; 32]>() as
                            libc::c_ulong).wrapping_div(::std::mem::size_of::<*mut libc::c_char>()
                                                            as libc::c_ulong)
                   {
                    fprintf(__stderrp,
                            b"too many included files\x00" as *const u8 as
                                *const libc::c_char);
                    exit(1 as libc::c_int);
                }
                let fresh2 = optind;
                optind = optind + 1;
                let fresh3 = include_count;
                include_count = include_count + 1;
                include_list[fresh3 as usize] = *argv.offset(fresh2 as isize)
            } else if opt as libc::c_int == 'i' as i32 ||
                          strcmp(longopt,
                                 b"interactive\x00" as *const u8 as
                                     *const libc::c_char) == 0 {
                interactive += 1
            } else if opt as libc::c_int == 'm' as i32 ||
                          strcmp(longopt,
                                 b"module\x00" as *const u8 as
                                     *const libc::c_char) == 0 {
                module = 1 as libc::c_int
            } else if strcmp(longopt,
                             b"script\x00" as *const u8 as
                                 *const libc::c_char) == 0 {
                module = 0 as libc::c_int
            } else if opt as libc::c_int == 'd' as i32 ||
                          strcmp(longopt,
                                 b"dump\x00" as *const u8 as
                                     *const libc::c_char) == 0 {
                dump_memory += 1
            } else if opt as libc::c_int == 'T' as i32 ||
                          strcmp(longopt,
                                 b"trace\x00" as *const u8 as
                                     *const libc::c_char) == 0 {
                trace_memory += 1
            } else if strcmp(longopt,
                             b"std\x00" as *const u8 as *const libc::c_char)
                          == 0 {
                load_std = 1 as libc::c_int
            } else if strcmp(longopt,
                             b"unhandled-rejection\x00" as *const u8 as
                                 *const libc::c_char) == 0 {
                dump_unhandled_promise_rejection = 1 as libc::c_int
            } else if opt as libc::c_int == 'q' as i32 ||
                          strcmp(longopt,
                                 b"quit\x00" as *const u8 as
                                     *const libc::c_char) == 0 {
                empty_run += 1
            } else if strcmp(longopt,
                             b"memory-limit\x00" as *const u8 as
                                 *const libc::c_char) == 0 {
                if optind >= argc {
                    fprintf(__stderrp,
                            b"expecting memory limit\x00" as *const u8 as
                                *const libc::c_char);
                    exit(1 as libc::c_int);
                }
                let fresh4 = optind;
                optind = optind + 1;
                memory_limit =
                    strtod(*argv.offset(fresh4 as isize),
                           0 as *mut *mut libc::c_char) as size_t
            } else if strcmp(longopt,
                             b"stack-size\x00" as *const u8 as
                                 *const libc::c_char) == 0 {
                if optind >= argc {
                    fprintf(__stderrp,
                            b"expecting stack size\x00" as *const u8 as
                                *const libc::c_char);
                    exit(1 as libc::c_int);
                }
                let fresh5 = optind;
                optind = optind + 1;
                stack_size =
                    strtod(*argv.offset(fresh5 as isize),
                           0 as *mut *mut libc::c_char) as size_t
            } else {
                if opt != 0 {
                    fprintf(__stderrp,
                            b"qjs: unknown option \'-%c\'\n\x00" as *const u8
                                as *const libc::c_char, opt as libc::c_int);
                } else {
                    fprintf(__stderrp,
                            b"qjs: unknown option \'--%s\'\n\x00" as *const u8
                                as *const libc::c_char, longopt);
                }
                help();
            }
            longopt = b"\x00" as *const u8 as *const libc::c_char
        }
    }
    if trace_memory != 0 {
        js_trace_malloc_init(&mut trace_data);
        rt =
            JS_NewRuntime2(&trace_mf,
                           &mut trace_data as *mut trace_malloc_data as
                               *mut libc::c_void)
    } else { rt = JS_NewRuntime() }
    if rt.is_null() {
        fprintf(__stderrp,
                b"qjs: cannot allocate JS runtime\n\x00" as *const u8 as
                    *const libc::c_char);
        exit(2 as libc::c_int);
    }
    if memory_limit != 0 as libc::c_int as libc::c_ulong {
        JS_SetMemoryLimit(rt, memory_limit);
    }
    if stack_size != 0 as libc::c_int as libc::c_ulong {
        JS_SetMaxStackSize(rt, stack_size);
    }
    js_std_init_handlers(rt);
    ctx = JS_NewContext(rt);
    if ctx.is_null() {
        fprintf(__stderrp,
                b"qjs: cannot allocate JS context\n\x00" as *const u8 as
                    *const libc::c_char);
        exit(2 as libc::c_int);
    }
    /* loader for ES6 modules */
    JS_SetModuleLoaderFunc(rt, None,
                           Some(js_module_loader as
                                    unsafe extern "C" fn(_: *mut JSContext,
                                                         _:
                                                             *const libc::c_char,
                                                         _: *mut libc::c_void)
                                        -> *mut JSModuleDef),
                           0 as *mut libc::c_void);
    if dump_unhandled_promise_rejection != 0 {
        JS_SetHostPromiseRejectionTracker(rt,
                                          Some(js_std_promise_rejection_tracker
                                                   as
                                                   unsafe extern "C" fn(_:
                                                                            *mut JSContext,
                                                                        _:
                                                                            JSValue,
                                                                        _:
                                                                            JSValue,
                                                                        _:
                                                                            libc::c_int,
                                                                        _:
                                                                            *mut libc::c_void)
                                                       -> ()),
                                          0 as *mut libc::c_void);
    }
    if empty_run == 0 {
        js_std_add_helpers(ctx, argc - optind, argv.offset(optind as isize));
        /* system modules */
        js_init_module_std(ctx,
                           b"std\x00" as *const u8 as *const libc::c_char);
        js_init_module_os(ctx, b"os\x00" as *const u8 as *const libc::c_char);
        /* make 'std' and 'os' visible to non module code */
        if load_std != 0 {
            let mut str: *const libc::c_char =
                b"import * as std from \'std\';\nimport * as os from \'os\';\nglobalThis.std = std;\nglobalThis.os = os;\n\x00"
                    as *const u8 as *const libc::c_char;
            eval_buf(ctx, str as *const libc::c_void,
                     strlen(str) as libc::c_int,
                     b"<input>\x00" as *const u8 as *const libc::c_char,
                     (1 as libc::c_int) << 0 as libc::c_int);
        }
        i = 0 as libc::c_int;
        loop  {
            if !(i < include_count) {
                current_block = 2522825242109451841;
                break ;
            }
            if eval_file(ctx, include_list[i as usize], module) != 0 {
                current_block = 11086394427076829997;
                break ;
            }
            i += 1
        }
        match current_block {
            2522825242109451841 => {
                if !expr.is_null() {
                    if eval_buf(ctx, expr as *const libc::c_void,
                                strlen(expr) as libc::c_int,
                                b"<cmdline>\x00" as *const u8 as
                                    *const libc::c_char, 0 as libc::c_int) !=
                           0 {
                        current_block = 11086394427076829997;
                    } else { current_block = 16314074004867283505; }
                } else if optind >= argc {
                    /* interactive mode */
                    interactive = 1 as libc::c_int;
                    current_block = 16314074004867283505;
                } else {
                    let mut filename: *const libc::c_char =
                        0 as *const libc::c_char;
                    filename = *argv.offset(optind as isize);
                    if eval_file(ctx, filename, module) != 0 {
                        current_block = 11086394427076829997;
                    } else { current_block = 16314074004867283505; }
                }
                match current_block {
                    11086394427076829997 => { }
                    _ => {
                        //        if (interactive) {
//            js_std_eval_binary(ctx, qjsc_repl, qjsc_repl_size, 0);
//        }
                        js_std_loop(ctx);
                        current_block = 12696043255897098083;
                    }
                }
            }
            _ => { }
        }
        match current_block {
            12696043255897098083 => { }
            _ => {
                js_std_free_handlers(rt);
                JS_FreeContext(ctx);
                JS_FreeRuntime(rt);
                return 1 as libc::c_int
            }
        }
    }
    if dump_memory != 0 {
        let mut stats: JSMemoryUsage =
            JSMemoryUsage{malloc_size: 0,
                          malloc_limit: 0,
                          memory_used_size: 0,
                          malloc_count: 0,
                          memory_used_count: 0,
                          atom_count: 0,
                          atom_size: 0,
                          str_count: 0,
                          str_size: 0,
                          obj_count: 0,
                          obj_size: 0,
                          prop_count: 0,
                          prop_size: 0,
                          shape_count: 0,
                          shape_size: 0,
                          js_func_count: 0,
                          js_func_size: 0,
                          js_func_code_size: 0,
                          js_func_pc2line_count: 0,
                          js_func_pc2line_size: 0,
                          c_func_count: 0,
                          array_count: 0,
                          fast_array_count: 0,
                          fast_array_elements: 0,
                          binary_object_count: 0,
                          binary_object_size: 0,};
        JS_ComputeMemoryUsage(rt, &mut stats);
        JS_DumpMemoryUsage(__stdoutp, &mut stats, rt);
    }
    js_std_free_handlers(rt);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
    if empty_run != 0 && dump_memory != 0 {
        let mut t: [clock_t; 5] = [0; 5];
        let mut best: [libc::c_double; 5] = [0.; 5];
        let mut i_0: libc::c_int = 0;
        let mut j: libc::c_int = 0;
        i_0 = 0 as libc::c_int;
        while i_0 < 100 as libc::c_int {
            t[0 as libc::c_int as usize] = clock();
            rt = JS_NewRuntime();
            t[1 as libc::c_int as usize] = clock();
            ctx = JS_NewContext(rt);
            t[2 as libc::c_int as usize] = clock();
            JS_FreeContext(ctx);
            t[3 as libc::c_int as usize] = clock();
            JS_FreeRuntime(rt);
            t[4 as libc::c_int as usize] = clock();
            j = 4 as libc::c_int;
            while j > 0 as libc::c_int {
                let mut ms: libc::c_double =
                    1000.0f64 *
                        t[j as
                              usize].wrapping_sub(t[(j - 1 as libc::c_int) as
                                                        usize]) as
                            libc::c_double /
                        1000000 as libc::c_int as libc::c_double;
                if i_0 == 0 as libc::c_int || best[j as usize] > ms {
                    best[j as usize] = ms
                }
                j -= 1
            }
            i_0 += 1
        }
        printf(b"\nInstantiation times (ms): %.3f = %.3f+%.3f+%.3f+%.3f\n\x00"
                   as *const u8 as *const libc::c_char,
               best[1 as libc::c_int as usize] +
                   best[2 as libc::c_int as usize] +
                   best[3 as libc::c_int as usize] +
                   best[4 as libc::c_int as usize],
               best[1 as libc::c_int as usize],
               best[2 as libc::c_int as usize],
               best[3 as libc::c_int as usize],
               best[4 as libc::c_int as usize]);
    }
    return 0 as libc::c_int;
}
#[main]
pub fn main() {
    let mut args: Vec<*mut libc::c_char> = Vec::new();
    for arg in ::std::env::args() {
        args.push(::std::ffi::CString::new(arg).expect("Failed to convert argument into CString.").into_raw());
    };
    args.push(::std::ptr::null_mut());
    unsafe {
        ::std::process::exit(main_0((args.len() - 1) as libc::c_int,
                                    args.as_mut_ptr() as
                                        *mut *mut libc::c_char) as i32)
    }
}
