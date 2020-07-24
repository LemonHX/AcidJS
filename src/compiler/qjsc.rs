#![allow(dead_code, mutable_transmutes, non_camel_case_types, non_snake_case,
         non_upper_case_globals, unused_assignments, unused_mut)]
#![register_tool(c2rust)]
#![feature(const_raw_ptr_to_usize_cast, extern_types, label_break_value, main,
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
    fn free(_: *mut libc::c_void);
    #[no_mangle]
    fn realloc(_: *mut libc::c_void, _: libc::c_ulong) -> *mut libc::c_void;
    #[no_mangle]
    fn abort() -> !;
    #[no_mangle]
    fn exit(_: libc::c_int) -> !;
    #[no_mangle]
    fn strtod(_: *const libc::c_char, _: *mut *mut libc::c_char)
     -> libc::c_double;
    #[no_mangle]
    static mut __stderrp: *mut FILE;
    #[no_mangle]
    fn fclose(_: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn fopen(_: *const libc::c_char, _: *const libc::c_char) -> *mut FILE;
    #[no_mangle]
    fn fprintf(_: *mut FILE, _: *const libc::c_char, _: ...) -> libc::c_int;
    #[no_mangle]
    fn fputs(_: *const libc::c_char, _: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn perror(_: *const libc::c_char);
    #[no_mangle]
    fn printf(_: *const libc::c_char, _: ...) -> libc::c_int;
    #[no_mangle]
    fn snprintf(_: *mut libc::c_char, _: libc::c_ulong,
                _: *const libc::c_char, _: ...) -> libc::c_int;
    #[no_mangle]
    fn strchr(_: *const libc::c_char, _: libc::c_int) -> *mut libc::c_char;
    #[no_mangle]
    fn strcmp(_: *const libc::c_char, _: *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn strlen(_: *const libc::c_char) -> libc::c_ulong;
    #[no_mangle]
    fn strrchr(_: *const libc::c_char, _: libc::c_int) -> *mut libc::c_char;
    #[no_mangle]
    fn strdup(_: *const libc::c_char) -> *mut libc::c_char;
    #[no_mangle]
    fn __assert_rtn(_: *const libc::c_char, _: *const libc::c_char,
                    _: libc::c_int, _: *const libc::c_char) -> !;
    #[no_mangle]
    fn getpid() -> pid_t;
    #[no_mangle]
    fn getopt(_: libc::c_int, _: *const *mut libc::c_char,
              _: *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    static mut optarg: *mut libc::c_char;
    #[no_mangle]
    static mut optind: libc::c_int;
    #[no_mangle]
    fn pstrcpy(buf: *mut libc::c_char, buf_size: libc::c_int,
               str: *const libc::c_char);
    #[no_mangle]
    fn strstart(str: *const libc::c_char, val: *const libc::c_char,
                ptr: *mut *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn has_suffix(str: *const libc::c_char, suffix: *const libc::c_char)
     -> libc::c_int;
    #[no_mangle]
    fn JS_NewRuntime() -> *mut JSRuntime;
    #[no_mangle]
    fn JS_FreeRuntime(rt: *mut JSRuntime);
    #[no_mangle]
    fn JS_NewContext(rt: *mut JSRuntime) -> *mut JSContext;
    #[no_mangle]
    fn JS_FreeContext(s: *mut JSContext);
    #[no_mangle]
    fn js_free(ctx: *mut JSContext, ptr: *mut libc::c_void);
    #[no_mangle]
    fn JS_ThrowReferenceError(ctx: *mut JSContext, fmt: *const libc::c_char,
                              _: ...) -> JSValue;
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
    /* module_normalize = NULL is allowed and invokes the default module
   filename normalizer */
    #[no_mangle]
    fn JS_SetModuleLoaderFunc(rt: *mut JSRuntime,
                              module_normalize: Option<JSModuleNormalizeFunc>,
                              module_loader: Option<JSModuleLoaderFunc>,
                              opaque: *mut libc::c_void);
    /* byte swapped output */
    /* allow SharedArrayBuffer */
    /* allow object references to
                                           encode arbitrary object
                                           graph */
    #[no_mangle]
    fn JS_WriteObject(ctx: *mut JSContext, psize: *mut size_t, obj: JSValue,
                      flags: libc::c_int) -> *mut uint8_t;
    #[no_mangle]
    fn js_load_file(ctx: *mut JSContext, pbuf_len: *mut size_t,
                    filename: *const libc::c_char) -> *mut uint8_t;
    #[no_mangle]
    fn js_std_dump_error(ctx: *mut JSContext);
    #[no_mangle]
    fn JS_NewCModule(ctx: *mut JSContext, name_str: *const libc::c_char,
                     func: Option<JSModuleInitFunc>) -> *mut JSModuleDef;
}
pub type __int32_t = libc::c_int;
pub type __int64_t = libc::c_longlong;
pub type __darwin_size_t = libc::c_ulong;
pub type __darwin_off_t = __int64_t;
pub type __darwin_pid_t = __int32_t;
pub type pid_t = __darwin_pid_t;
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
/*
 * C utilities
 *
 * Copyright (c) 2017 Fabrice Bellard
 * Copyright (c) 2018 Charlie Gordon
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
/* set if CPU is big endian */
pub type C2RustUnnamed = libc::c_uint;
pub const TRUE: C2RustUnnamed = 1;
pub const FALSE: C2RustUnnamed = 0;
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
    pub c2rust_unnamed: C2RustUnnamed_13,
    pub shape: *mut JSShape,
    pub prop: *mut JSProperty,
    pub first_weak_ref: *mut JSMapRecord,
    pub u: C2RustUnnamed_0,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_0 {
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
    pub func: C2RustUnnamed_6,
    pub cfunc: C2RustUnnamed_5,
    pub array: C2RustUnnamed_2,
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
    pub u: C2RustUnnamed_1,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_1 {
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
pub struct C2RustUnnamed_2 {
    pub u1: C2RustUnnamed_4,
    pub u: C2RustUnnamed_3,
    pub count: uint32_t,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_3 {
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
pub union C2RustUnnamed_4 {
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
/*
 * Linux klist like system
 * 
 * Copyright (c) 2016-2017 Fabrice Bellard
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
#[derive(Copy, Clone)]
#[repr(C)]
pub struct list_head {
    pub prev: *mut list_head,
    pub next: *mut list_head,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_5 {
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
pub struct C2RustUnnamed_6 {
    pub function_bytecode: *mut JSFunctionBytecode,
    pub var_refs: *mut *mut JSVarRef,
    pub home_object: *mut JSObject,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSVarRef {
    pub c2rust_unnamed: C2RustUnnamed_7,
    pub pvalue: *mut JSValue,
    pub value: JSValue,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_7 {
    pub header: JSGCObjectHeader,
    pub c2rust_unnamed: C2RustUnnamed_8,
}
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct C2RustUnnamed_8 {
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
    pub debug: C2RustUnnamed_9,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_9 {
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
    pub u: C2RustUnnamed_10,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_10 {
    pub value: JSValue,
    pub getset: C2RustUnnamed_12,
    pub var_ref: *mut JSVarRef,
    pub init: C2RustUnnamed_11,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_11 {
    pub realm_and_id: uintptr_t,
    pub opaque: *mut libc::c_void,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_12 {
    pub getter: *mut JSObject,
    pub setter: *mut JSObject,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_13 {
    pub header: JSGCObjectHeader,
    pub c2rust_unnamed: C2RustUnnamed_14,
}
#[derive(Copy, Clone, BitfieldStruct)]
#[repr(C)]
pub struct C2RustUnnamed_14 {
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
/* C function definition */
/* XXX: should rename for namespace isolation */
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
    pub u: C2RustUnnamed_15,
    pub export_type: JSExportTypeEnum,
    pub local_name: JSAtom,
    pub export_name: JSAtom,
}
pub type JSExportTypeEnum = libc::c_uint;
pub const JS_EXPORT_TYPE_INDIRECT: JSExportTypeEnum = 1;
pub const JS_EXPORT_TYPE_LOCAL: JSExportTypeEnum = 0;
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_15 {
    pub local: C2RustUnnamed_16,
    pub req_module_idx: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_16 {
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
pub type C2RustUnnamed_17 = libc::c_int;
pub const JS_TAG_FLOAT64: C2RustUnnamed_17 = 7;
pub const JS_TAG_EXCEPTION: C2RustUnnamed_17 = 6;
pub const JS_TAG_CATCH_OFFSET: C2RustUnnamed_17 = 5;
pub const JS_TAG_UNINITIALIZED: C2RustUnnamed_17 = 4;
pub const JS_TAG_UNDEFINED: C2RustUnnamed_17 = 3;
pub const JS_TAG_NULL: C2RustUnnamed_17 = 2;
pub const JS_TAG_BOOL: C2RustUnnamed_17 = 1;
pub const JS_TAG_INT: C2RustUnnamed_17 = 0;
/* any larger tag is FLOAT64 if JS_NAN_BOXING */
/* used internally */
pub const JS_TAG_OBJECT: C2RustUnnamed_17 = -1;
/* used internally */
pub const JS_TAG_FUNCTION_BYTECODE: C2RustUnnamed_17 = -2;
pub const JS_TAG_MODULE: C2RustUnnamed_17 = -3;
pub const JS_TAG_STRING: C2RustUnnamed_17 = -7;
pub const JS_TAG_SYMBOL: C2RustUnnamed_17 = -8;
pub const JS_TAG_BIG_FLOAT: C2RustUnnamed_17 = -9;
pub const JS_TAG_BIG_INT: C2RustUnnamed_17 = -10;
/* first negative tag */
pub const JS_TAG_BIG_DECIMAL: C2RustUnnamed_17 = -11;
/* all tags with a reference count are negative */
pub const JS_TAG_FIRST: C2RustUnnamed_17 = -11;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct namelist_entry_t {
    pub name: *mut libc::c_char,
    pub short_name: *mut libc::c_char,
    pub flags: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct namelist_t {
    pub array: *mut namelist_entry_t,
    pub count: libc::c_int,
    pub size: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct FeatureEntry {
    pub option_name: *const libc::c_char,
    pub init_name: *const libc::c_char,
}
pub type OutputTypeEnum = libc::c_uint;
pub const OUTPUT_EXECUTABLE: OutputTypeEnum = 2;
pub const OUTPUT_C_MAIN: OutputTypeEnum = 1;
pub const OUTPUT_C: OutputTypeEnum = 0;
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
static mut cname_list: namelist_t =
    namelist_t{array: 0 as *const namelist_entry_t as *mut namelist_entry_t,
               count: 0,
               size: 0,};
static mut cmodule_list: namelist_t =
    namelist_t{array: 0 as *const namelist_entry_t as *mut namelist_entry_t,
               count: 0,
               size: 0,};
static mut init_module_list: namelist_t =
    namelist_t{array: 0 as *const namelist_entry_t as *mut namelist_entry_t,
               count: 0,
               size: 0,};
static mut feature_bitmap: uint64_t = 0;
static mut outfile: *mut FILE = 0 as *const FILE as *mut FILE;
static mut byte_swap: libc::c_int = 0;
static mut dynamic_export: libc::c_int = 0;
static mut c_ident_prefix: *const libc::c_char =
    b"qjsc_\x00" as *const u8 as *const libc::c_char;
static mut feature_list: [FeatureEntry; 10] =
    [{
         let mut init =
             FeatureEntry{option_name:
                              b"date\x00" as *const u8 as *const libc::c_char,
                          init_name:
                              b"Date\x00" as *const u8 as
                                  *const libc::c_char,};
         init
     },
     {
         let mut init =
             FeatureEntry{option_name:
                              b"eval\x00" as *const u8 as *const libc::c_char,
                          init_name:
                              b"Eval\x00" as *const u8 as
                                  *const libc::c_char,};
         init
     },
     {
         let mut init =
             FeatureEntry{option_name:
                              b"string-normalize\x00" as *const u8 as
                                  *const libc::c_char,
                          init_name:
                              b"StringNormalize\x00" as *const u8 as
                                  *const libc::c_char,};
         init
     },
     {
         let mut init =
             FeatureEntry{option_name:
                              b"regexp\x00" as *const u8 as
                                  *const libc::c_char,
                          init_name:
                              b"RegExp\x00" as *const u8 as
                                  *const libc::c_char,};
         init
     },
     {
         let mut init =
             FeatureEntry{option_name:
                              b"json\x00" as *const u8 as *const libc::c_char,
                          init_name:
                              b"JSON\x00" as *const u8 as
                                  *const libc::c_char,};
         init
     },
     {
         let mut init =
             FeatureEntry{option_name:
                              b"proxy\x00" as *const u8 as
                                  *const libc::c_char,
                          init_name:
                              b"Proxy\x00" as *const u8 as
                                  *const libc::c_char,};
         init
     },
     {
         let mut init =
             FeatureEntry{option_name:
                              b"map\x00" as *const u8 as *const libc::c_char,
                          init_name:
                              b"MapSet\x00" as *const u8 as
                                  *const libc::c_char,};
         init
     },
     {
         let mut init =
             FeatureEntry{option_name:
                              b"typedarray\x00" as *const u8 as
                                  *const libc::c_char,
                          init_name:
                              b"TypedArrays\x00" as *const u8 as
                                  *const libc::c_char,};
         init
     },
     {
         let mut init =
             FeatureEntry{option_name:
                              b"promise\x00" as *const u8 as
                                  *const libc::c_char,
                          init_name:
                              b"Promise\x00" as *const u8 as
                                  *const libc::c_char,};
         init
     },
     {
         let mut init =
             FeatureEntry{option_name:
                              b"module-loader\x00" as *const u8 as
                                  *const libc::c_char,
                          init_name: 0 as *const libc::c_char,};
         init
     }];
#[no_mangle]
pub unsafe extern "C" fn namelist_add(mut lp: *mut namelist_t,
                                      mut name: *const libc::c_char,
                                      mut short_name: *const libc::c_char,
                                      mut flags: libc::c_int) {
    let mut e: *mut namelist_entry_t = 0 as *mut namelist_entry_t;
    if (*lp).count == (*lp).size {
        let mut newsize: size_t =
            ((*lp).size + ((*lp).size >> 1 as libc::c_int) + 4 as libc::c_int)
                as size_t;
        let mut a: *mut namelist_entry_t =
            realloc((*lp).array as *mut libc::c_void,
                    (::std::mem::size_of::<namelist_entry_t>() as
                         libc::c_ulong).wrapping_mul(newsize)) as
                *mut namelist_entry_t;
        /* XXX: check for realloc failure */
        (*lp).array = a;
        (*lp).size = newsize as libc::c_int
    }
    let fresh0 = (*lp).count;
    (*lp).count = (*lp).count + 1;
    e = &mut *(*lp).array.offset(fresh0 as isize) as *mut namelist_entry_t;
    (*e).name = strdup(name);
    if !short_name.is_null() {
        (*e).short_name = strdup(short_name)
    } else { (*e).short_name = 0 as *mut libc::c_char }
    (*e).flags = flags;
}
#[no_mangle]
pub unsafe extern "C" fn namelist_free(mut lp: *mut namelist_t) {
    while (*lp).count > 0 as libc::c_int {
        (*lp).count -= 1;
        let mut e: *mut namelist_entry_t =
            &mut *(*lp).array.offset((*lp).count as isize) as
                *mut namelist_entry_t;
        free((*e).name as *mut libc::c_void);
        free((*e).short_name as *mut libc::c_void);
    }
    free((*lp).array as *mut libc::c_void);
    (*lp).array = 0 as *mut namelist_entry_t;
    (*lp).size = 0 as libc::c_int;
}
#[no_mangle]
pub unsafe extern "C" fn namelist_find(mut lp: *mut namelist_t,
                                       mut name: *const libc::c_char)
 -> *mut namelist_entry_t {
    let mut i: libc::c_int = 0;
    i = 0 as libc::c_int;
    while i < (*lp).count {
        let mut e: *mut namelist_entry_t =
            &mut *(*lp).array.offset(i as isize) as *mut namelist_entry_t;
        if strcmp((*e).name, name) == 0 { return e }
        i += 1
    }
    return 0 as *mut namelist_entry_t;
}
unsafe extern "C" fn get_c_name(mut buf: *mut libc::c_char,
                                mut buf_size: size_t,
                                mut file: *const libc::c_char) {
    let mut p: *const libc::c_char = 0 as *const libc::c_char;
    let mut r: *const libc::c_char = 0 as *const libc::c_char;
    let mut len: size_t = 0;
    let mut i: size_t = 0;
    let mut c: libc::c_int = 0;
    let mut q: *mut libc::c_char = 0 as *mut libc::c_char;
    p = strrchr(file, '/' as i32);
    if p.is_null() { p = file } else { p = p.offset(1) }
    r = strrchr(p, '.' as i32);
    if r.is_null() {
        len = strlen(p)
    } else { len = r.wrapping_offset_from(p) as libc::c_long as size_t }
    pstrcpy(buf, buf_size as libc::c_int, c_ident_prefix);
    q = buf.offset(strlen(buf) as isize);
    i = 0 as libc::c_int as size_t;
    while i < len {
        c = *p.offset(i as isize) as libc::c_int;
        if !(c >= '0' as i32 && c <= '9' as i32 ||
                 c >= 'A' as i32 && c <= 'Z' as i32 ||
                 c >= 'a' as i32 && c <= 'z' as i32) {
            c = '_' as i32
        }
        if (q.wrapping_offset_from(buf) as libc::c_long as libc::c_ulong) <
               buf_size.wrapping_sub(1 as libc::c_int as libc::c_ulong) {
            let fresh1 = q;
            q = q.offset(1);
            *fresh1 = c as libc::c_char
        }
        i = i.wrapping_add(1)
    }
    *q = '\u{0}' as i32 as libc::c_char;
}
unsafe extern "C" fn dump_hex(mut f: *mut FILE, mut buf: *const uint8_t,
                              mut len: size_t) {
    let mut i: size_t = 0;
    let mut col: size_t = 0;
    col = 0 as libc::c_int as size_t;
    i = 0 as libc::c_int as size_t;
    while i < len {
        fprintf(f, b" 0x%02x,\x00" as *const u8 as *const libc::c_char,
                *buf.offset(i as isize) as libc::c_int);
        col = col.wrapping_add(1);
        if col == 8 as libc::c_int as libc::c_ulong {
            fprintf(f, b"\n\x00" as *const u8 as *const libc::c_char);
            col = 0 as libc::c_int as size_t
        }
        i = i.wrapping_add(1)
    }
    if col != 0 as libc::c_int as libc::c_ulong {
        fprintf(f, b"\n\x00" as *const u8 as *const libc::c_char);
    };
}
unsafe extern "C" fn output_object_code(mut ctx: *mut JSContext,
                                        mut fo: *mut FILE, mut obj: JSValue,
                                        mut c_name: *const libc::c_char,
                                        mut load_only: libc::c_int) {
    let mut out_buf: *mut uint8_t = 0 as *mut uint8_t;
    let mut out_buf_len: size_t = 0;
    let mut flags: libc::c_int = 0;
    flags = (1 as libc::c_int) << 0 as libc::c_int;
    if byte_swap != 0 { flags |= (1 as libc::c_int) << 1 as libc::c_int }
    out_buf = JS_WriteObject(ctx, &mut out_buf_len, obj, flags);
    if out_buf.is_null() { js_std_dump_error(ctx); exit(1 as libc::c_int); }
    namelist_add(&mut cname_list, c_name, 0 as *const libc::c_char,
                 load_only);
    fprintf(fo,
            b"const uint32_t %s_size = %u;\n\n\x00" as *const u8 as
                *const libc::c_char, c_name, out_buf_len as libc::c_uint);
    fprintf(fo,
            b"const uint8_t %s[%u] = {\n\x00" as *const u8 as
                *const libc::c_char, c_name, out_buf_len as libc::c_uint);
    dump_hex(fo, out_buf, out_buf_len);
    fprintf(fo, b"};\n\n\x00" as *const u8 as *const libc::c_char);
    js_free(ctx, out_buf as *mut libc::c_void);
}
unsafe extern "C" fn js_module_dummy_init(mut ctx: *mut JSContext,
                                          mut m: *mut JSModuleDef)
 -> libc::c_int {
    /* should never be called when compiling JS code */
    abort();
}
unsafe extern "C" fn find_unique_cname(mut cname: *mut libc::c_char,
                                       mut cname_size: size_t) {
    let mut cname1: [libc::c_char; 1024] = [0; 1024];
    let mut suffix_num: libc::c_int = 0;
    let mut len: size_t = 0;
    let mut max_len: size_t = 0;
    if !(cname_size >= 32 as libc::c_int as libc::c_ulong) as libc::c_int as
           libc::c_long != 0 {
        __assert_rtn((*::std::mem::transmute::<&[u8; 18],
                                               &[libc::c_char; 18]>(b"find_unique_cname\x00")).as_ptr(),
                     b"/Users/lemonhx/Desktop/Cpp/AcidJS/src/compiler/qjsc.c\x00"
                         as *const u8 as *const libc::c_char,
                     222 as libc::c_int,
                     b"cname_size >= 32\x00" as *const u8 as
                         *const libc::c_char);
    } else { };
    /* find a C name not matching an existing module C name by
       adding a numeric suffix */
    len = strlen(cname);
    max_len = cname_size.wrapping_sub(16 as libc::c_int as libc::c_ulong);
    if len > max_len {
        *cname.offset(max_len as isize) = '\u{0}' as i32 as libc::c_char
    }
    suffix_num = 1 as libc::c_int;
    loop  {
        snprintf(cname1.as_mut_ptr(),
                 ::std::mem::size_of::<[libc::c_char; 1024]>() as
                     libc::c_ulong,
                 b"%s_%d\x00" as *const u8 as *const libc::c_char, cname,
                 suffix_num);
        if namelist_find(&mut cname_list, cname1.as_mut_ptr()).is_null() {
            break ;
        }
        suffix_num += 1
    }
    pstrcpy(cname, cname_size as libc::c_int, cname1.as_mut_ptr());
}
#[no_mangle]
pub unsafe extern "C" fn jsc_module_loader(mut ctx: *mut JSContext,
                                           mut module_name:
                                               *const libc::c_char,
                                           mut opaque: *mut libc::c_void)
 -> *mut JSModuleDef {
    let mut m: *mut JSModuleDef = 0 as *mut JSModuleDef;
    let mut e: *mut namelist_entry_t = 0 as *mut namelist_entry_t;
    /* check if it is a declared C or system module */
    e = namelist_find(&mut cmodule_list, module_name);
    if !e.is_null() {
        /* add in the static init module list */
        namelist_add(&mut init_module_list, (*e).name, (*e).short_name,
                     0 as libc::c_int);
        /* create a dummy module */
        m =
            JS_NewCModule(ctx, module_name,
                          Some(js_module_dummy_init as
                                   unsafe extern "C" fn(_: *mut JSContext,
                                                        _: *mut JSModuleDef)
                                       -> libc::c_int))
    } else if has_suffix(module_name,
                         b".so\x00" as *const u8 as *const libc::c_char) != 0
     {
        fprintf(__stderrp,
                b"Warning: binary module \'%s\' will be dynamically loaded\n\x00"
                    as *const u8 as *const libc::c_char, module_name);
        /* create a dummy module */
        m =
            JS_NewCModule(ctx, module_name,
                          Some(js_module_dummy_init as
                                   unsafe extern "C" fn(_: *mut JSContext,
                                                        _: *mut JSModuleDef)
                                       -> libc::c_int));
        /* the resulting executable will export its symbols for the
           dynamic library */
        dynamic_export = TRUE as libc::c_int
    } else {
        let mut buf_len: size_t = 0;
        let mut buf: *mut uint8_t = 0 as *mut uint8_t;
        let mut func_val: JSValue =
            JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
        let mut cname: [libc::c_char; 1024] = [0; 1024];
        buf = js_load_file(ctx, &mut buf_len, module_name);
        if buf.is_null() {
            JS_ThrowReferenceError(ctx,
                                   b"could not load module filename \'%s\'\x00"
                                       as *const u8 as *const libc::c_char,
                                   module_name);
            return 0 as *mut JSModuleDef
        }
        /* compile the module */
        func_val =
            JS_Eval(ctx, buf as *mut libc::c_char, buf_len, module_name,
                    (1 as libc::c_int) << 0 as libc::c_int |
                        (1 as libc::c_int) << 5 as libc::c_int);
        js_free(ctx, buf as *mut libc::c_void);
        if JS_IsException(func_val) != 0 { return 0 as *mut JSModuleDef }
        get_c_name(cname.as_mut_ptr(),
                   ::std::mem::size_of::<[libc::c_char; 1024]>() as
                       libc::c_ulong, module_name);
        if !namelist_find(&mut cname_list, cname.as_mut_ptr()).is_null() {
            find_unique_cname(cname.as_mut_ptr(),
                              ::std::mem::size_of::<[libc::c_char; 1024]>() as
                                  libc::c_ulong);
        }
        output_object_code(ctx, outfile, func_val, cname.as_mut_ptr(),
                           TRUE as libc::c_int);
        /* the module is already referenced, so we must free it */
        m = func_val.u.ptr as *mut JSModuleDef;
        JS_FreeValue(ctx, func_val);
    }
    return m;
}
unsafe extern "C" fn compile_file(mut ctx: *mut JSContext, mut fo: *mut FILE,
                                  mut filename: *const libc::c_char,
                                  mut c_name1: *const libc::c_char,
                                  mut module: libc::c_int) {
    let mut buf: *mut uint8_t = 0 as *mut uint8_t;
    let mut c_name: [libc::c_char; 1024] = [0; 1024];
    let mut eval_flags: libc::c_int = 0;
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut buf_len: size_t = 0;
    buf = js_load_file(ctx, &mut buf_len, filename);
    if buf.is_null() {
        fprintf(__stderrp,
                b"Could not load \'%s\'\n\x00" as *const u8 as
                    *const libc::c_char, filename);
        exit(1 as libc::c_int);
    }
    eval_flags = (1 as libc::c_int) << 5 as libc::c_int;
    if module < 0 as libc::c_int {
        module =
            (has_suffix(filename,
                        b".mjs\x00" as *const u8 as *const libc::c_char) != 0
                 || JS_DetectModule(buf as *const libc::c_char, buf_len) != 0)
                as libc::c_int
    }
    if module != 0 {
        eval_flags |= (1 as libc::c_int) << 0 as libc::c_int
    } else { eval_flags |= (0 as libc::c_int) << 0 as libc::c_int }
    obj =
        JS_Eval(ctx, buf as *const libc::c_char, buf_len, filename,
                eval_flags);
    if JS_IsException(obj) != 0 {
        js_std_dump_error(ctx);
        exit(1 as libc::c_int);
    }
    js_free(ctx, buf as *mut libc::c_void);
    if !c_name1.is_null() {
        pstrcpy(c_name.as_mut_ptr(),
                ::std::mem::size_of::<[libc::c_char; 1024]>() as libc::c_ulong
                    as libc::c_int, c_name1);
    } else {
        get_c_name(c_name.as_mut_ptr(),
                   ::std::mem::size_of::<[libc::c_char; 1024]>() as
                       libc::c_ulong, filename);
    }
    output_object_code(ctx, fo, obj, c_name.as_mut_ptr(),
                       FALSE as libc::c_int);
    JS_FreeValue(ctx, obj);
}
static mut main_c_template1: [libc::c_char; 122] =
    [105, 110, 116, 32, 109, 97, 105, 110, 40, 105, 110, 116, 32, 97, 114,
     103, 99, 44, 32, 99, 104, 97, 114, 32, 42, 42, 97, 114, 103, 118, 41, 10,
     123, 10, 32, 32, 74, 83, 82, 117, 110, 116, 105, 109, 101, 32, 42, 114,
     116, 59, 10, 32, 32, 74, 83, 67, 111, 110, 116, 101, 120, 116, 32, 42,
     99, 116, 120, 59, 10, 32, 32, 114, 116, 32, 61, 32, 74, 83, 95, 78, 101,
     119, 82, 117, 110, 116, 105, 109, 101, 40, 41, 59, 10, 32, 32, 106, 115,
     95, 115, 116, 100, 95, 105, 110, 105, 116, 95, 104, 97, 110, 100, 108,
     101, 114, 115, 40, 114, 116, 41, 59, 10, 0];
static mut main_c_template2: [libc::c_char; 80] =
    [32, 32, 106, 115, 95, 115, 116, 100, 95, 108, 111, 111, 112, 40, 99, 116,
     120, 41, 59, 10, 32, 32, 74, 83, 95, 70, 114, 101, 101, 67, 111, 110,
     116, 101, 120, 116, 40, 99, 116, 120, 41, 59, 10, 32, 32, 74, 83, 95, 70,
     114, 101, 101, 82, 117, 110, 116, 105, 109, 101, 40, 114, 116, 41, 59,
     10, 32, 32, 114, 101, 116, 117, 114, 110, 32, 48, 59, 10, 125, 10, 0];
#[no_mangle]
pub unsafe extern "C" fn help() {
    printf(b"QuickJS Compiler version 2020-07-05\nusage: qjsc [options] [files]\n\noptions are:\n-c          only output bytecode in a C file\n-e          output main() and bytecode in a C file (default = executable output)\n-o output   set the output filename\n-N cname    set the C name of the generated data\n-m          compile as Javascript module (default=autodetect)\n-M module_name[,cname] add initialization code for an external C module\n-x          byte swapped output\n-p prefix   set the prefix of the generated C names\n-S n        set the maximum stack size to \'n\' bytes (default=%d)\n\x00"
               as *const u8 as *const libc::c_char,
           256 as libc::c_int * 1024 as libc::c_int);
    exit(1 as libc::c_int);
}
unsafe extern "C" fn output_executable(mut out_filename: *const libc::c_char,
                                       mut cfilename: *const libc::c_char,
                                       mut use_lto: libc::c_int,
                                       mut verbose: libc::c_int,
                                       mut exename: *const libc::c_char)
 -> libc::c_int {
    fprintf(__stderrp,
            b"Executable output is not supported for this target\n\x00" as
                *const u8 as *const libc::c_char);
    exit(1 as libc::c_int);
}
unsafe fn main_0(mut argc: libc::c_int, mut argv: *mut *mut libc::c_char)
 -> libc::c_int {
    let mut c: libc::c_int = 0;
    let mut i: libc::c_int = 0;
    let mut verbose: libc::c_int = 0;
    let mut out_filename: *const libc::c_char = 0 as *const libc::c_char;
    let mut cname: *const libc::c_char = 0 as *const libc::c_char;
    let mut cfilename: [libc::c_char; 1024] = [0; 1024];
    let mut fo: *mut FILE = 0 as *mut FILE;
    let mut rt: *mut JSRuntime = 0 as *mut JSRuntime;
    let mut ctx: *mut JSContext = 0 as *mut JSContext;
    let mut use_lto: libc::c_int = 0;
    let mut module: libc::c_int = 0;
    let mut output_type: OutputTypeEnum = OUTPUT_C;
    let mut stack_size: size_t = 0;
    out_filename = 0 as *const libc::c_char;
    output_type = OUTPUT_EXECUTABLE;
    cname = 0 as *const libc::c_char;
    feature_bitmap = -(1 as libc::c_int) as uint64_t;
    module = -(1 as libc::c_int);
    byte_swap = FALSE as libc::c_int;
    verbose = 0 as libc::c_int;
    use_lto = FALSE as libc::c_int;
    stack_size = 0 as libc::c_int as size_t;
    /* add system modules */
    namelist_add(&mut cmodule_list,
                 b"std\x00" as *const u8 as *const libc::c_char,
                 b"std\x00" as *const u8 as *const libc::c_char,
                 0 as libc::c_int);
    namelist_add(&mut cmodule_list,
                 b"os\x00" as *const u8 as *const libc::c_char,
                 b"os\x00" as *const u8 as *const libc::c_char,
                 0 as libc::c_int);
    loop  {
        c =
            getopt(argc, argv as *const *mut libc::c_char,
                   b"ho:cN:f:mxevM:p:S:\x00" as *const u8 as
                       *const libc::c_char);
        if c == -(1 as libc::c_int) { break ; }
        let mut current_block_43: u64;
        match c {
            104 => { help(); current_block_43 = 11162031913793250012; }
            111 => { current_block_43 = 11162031913793250012; }
            99 => {
                output_type = OUTPUT_C;
                current_block_43 = 15594839951440953787;
            }
            101 => {
                output_type = OUTPUT_C_MAIN;
                current_block_43 = 15594839951440953787;
            }
            78 => { cname = optarg; current_block_43 = 15594839951440953787; }
            102 => {
                let mut p: *const libc::c_char = 0 as *const libc::c_char;
                p = optarg;
                if strcmp(optarg,
                          b"lto\x00" as *const u8 as *const libc::c_char) == 0
                   {
                    use_lto = TRUE as libc::c_int
                } else {
                    's_211:
                        {
                            if strstart(p,
                                        b"no-\x00" as *const u8 as
                                            *const libc::c_char, &mut p) != 0
                               {
                                use_lto = TRUE as libc::c_int;
                                i = 0 as libc::c_int;
                                while (i as libc::c_ulong) <
                                          (::std::mem::size_of::<[FeatureEntry; 10]>()
                                               as
                                               libc::c_ulong).wrapping_div(::std::mem::size_of::<FeatureEntry>()
                                                                               as
                                                                               libc::c_ulong)
                                      {
                                    if strcmp(p,
                                              feature_list[i as
                                                               usize].option_name)
                                           == 0 {
                                        feature_bitmap &=
                                            !((1 as libc::c_int as uint64_t)
                                                  << i);
                                        break ;
                                    } else { i += 1 }
                                }
                                if !(i as libc::c_ulong ==
                                         (::std::mem::size_of::<[FeatureEntry; 10]>()
                                              as
                                              libc::c_ulong).wrapping_div(::std::mem::size_of::<FeatureEntry>()
                                                                              as
                                                                              libc::c_ulong))
                                   {
                                    break 's_211 ;
                                }
                            }
                            fprintf(__stderrp,
                                    b"unsupported feature: %s\n\x00" as
                                        *const u8 as *const libc::c_char,
                                    optarg);
                            exit(1 as libc::c_int);
                        }
                }
                current_block_43 = 15594839951440953787;
            }
            109 => {
                module = 1 as libc::c_int;
                current_block_43 = 15594839951440953787;
            }
            77 => {
                let mut p_0: *mut libc::c_char = 0 as *mut libc::c_char;
                let mut path: [libc::c_char; 1024] = [0; 1024];
                let mut cname_0: [libc::c_char; 1024] = [0; 1024];
                pstrcpy(path.as_mut_ptr(),
                        ::std::mem::size_of::<[libc::c_char; 1024]>() as
                            libc::c_ulong as libc::c_int, optarg);
                p_0 = strchr(path.as_mut_ptr(), ',' as i32);
                if !p_0.is_null() {
                    *p_0 = '\u{0}' as i32 as libc::c_char;
                    pstrcpy(cname_0.as_mut_ptr(),
                            ::std::mem::size_of::<[libc::c_char; 1024]>() as
                                libc::c_ulong as libc::c_int,
                            p_0.offset(1 as libc::c_int as isize));
                } else {
                    get_c_name(cname_0.as_mut_ptr(),
                               ::std::mem::size_of::<[libc::c_char; 1024]>()
                                   as libc::c_ulong, path.as_mut_ptr());
                }
                namelist_add(&mut cmodule_list, path.as_mut_ptr(),
                             cname_0.as_mut_ptr(), 0 as libc::c_int);
                current_block_43 = 15594839951440953787;
            }
            120 => {
                byte_swap = TRUE as libc::c_int;
                current_block_43 = 15594839951440953787;
            }
            118 => { verbose += 1; current_block_43 = 15594839951440953787; }
            112 => {
                c_ident_prefix = optarg;
                current_block_43 = 15594839951440953787;
            }
            83 => {
                stack_size =
                    strtod(optarg, 0 as *mut *mut libc::c_char) as size_t;
                current_block_43 = 15594839951440953787;
            }
            _ => { current_block_43 = 15594839951440953787; }
        }
        match current_block_43 {
            11162031913793250012 => { out_filename = optarg }
            _ => { }
        }
    }
    if optind >= argc { help(); }
    if out_filename.is_null() {
        if output_type as libc::c_uint ==
               OUTPUT_EXECUTABLE as libc::c_int as libc::c_uint {
            out_filename = b"a.out\x00" as *const u8 as *const libc::c_char
        } else {
            out_filename = b"out.c\x00" as *const u8 as *const libc::c_char
        }
    }
    if output_type as libc::c_uint ==
           OUTPUT_EXECUTABLE as libc::c_int as libc::c_uint {
        snprintf(cfilename.as_mut_ptr(),
                 ::std::mem::size_of::<[libc::c_char; 1024]>() as
                     libc::c_ulong,
                 b"/tmp/out%d.c\x00" as *const u8 as *const libc::c_char,
                 getpid());
    } else {
        pstrcpy(cfilename.as_mut_ptr(),
                ::std::mem::size_of::<[libc::c_char; 1024]>() as libc::c_ulong
                    as libc::c_int, out_filename);
    }
    fo =
        fopen(cfilename.as_mut_ptr(),
              b"w\x00" as *const u8 as *const libc::c_char);
    if fo.is_null() {
        perror(cfilename.as_mut_ptr());
        exit(1 as libc::c_int);
    }
    outfile = fo;
    rt = JS_NewRuntime();
    ctx = JS_NewContext(rt);
    /* loader for ES6 modules */
    JS_SetModuleLoaderFunc(rt, None,
                           Some(jsc_module_loader as
                                    unsafe extern "C" fn(_: *mut JSContext,
                                                         _:
                                                             *const libc::c_char,
                                                         _: *mut libc::c_void)
                                        -> *mut JSModuleDef),
                           0 as *mut libc::c_void);
    fprintf(fo,
            b"/* File generated automatically by the QuickJS compiler. */\n\n\x00"
                as *const u8 as *const libc::c_char);
    if output_type as libc::c_uint != OUTPUT_C as libc::c_int as libc::c_uint
       {
        fprintf(fo,
                b"#include \"quickjs-libc.h\"\n\n\x00" as *const u8 as
                    *const libc::c_char);
    } else {
        fprintf(fo,
                b"#include <inttypes.h>\n\n\x00" as *const u8 as
                    *const libc::c_char);
    }
    i = optind;
    while i < argc {
        let mut filename: *const libc::c_char = *argv.offset(i as isize);
        compile_file(ctx, fo, filename, cname, module);
        cname = 0 as *const libc::c_char;
        i += 1
    }
    if output_type as libc::c_uint != OUTPUT_C as libc::c_int as libc::c_uint
       {
        fputs(main_c_template1.as_ptr(), fo);
        fprintf(fo,
                b"  ctx = JS_NewContextRaw(rt);\n\x00" as *const u8 as
                    *const libc::c_char);
        if stack_size != 0 as libc::c_int as libc::c_ulong {
            fprintf(fo,
                    b"  JS_SetMaxStackSize(rt, %u);\n\x00" as *const u8 as
                        *const libc::c_char, stack_size as libc::c_uint);
        }
        /* add the module loader if necessary */
        if feature_bitmap &
               ((1 as libc::c_int) << 9 as libc::c_int) as libc::c_ulonglong
               != 0 {
            fprintf(fo,
                    b"  JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);\n\x00"
                        as *const u8 as *const libc::c_char);
        }
        /* add the basic objects */
        fprintf(fo,
                b"  JS_AddIntrinsicBaseObjects(ctx);\n\x00" as *const u8 as
                    *const libc::c_char);
        i = 0 as libc::c_int;
        while (i as libc::c_ulong) <
                  (::std::mem::size_of::<[FeatureEntry; 10]>() as
                       libc::c_ulong).wrapping_div(::std::mem::size_of::<FeatureEntry>()
                                                       as libc::c_ulong) {
            if feature_bitmap & (1 as libc::c_int as uint64_t) << i != 0 &&
                   !feature_list[i as usize].init_name.is_null() {
                fprintf(fo,
                        b"  JS_AddIntrinsic%s(ctx);\n\x00" as *const u8 as
                            *const libc::c_char,
                        feature_list[i as usize].init_name);
            }
            i += 1
        }
        fprintf(fo,
                b"  js_std_add_helpers(ctx, argc, argv);\n\x00" as *const u8
                    as *const libc::c_char);
        i = 0 as libc::c_int;
        while i < init_module_list.count {
            let mut e: *mut namelist_entry_t =
                &mut *init_module_list.array.offset(i as isize) as
                    *mut namelist_entry_t;
            /* initialize the static C modules */
            fprintf(fo,
                    b"  {\n    extern JSModuleDef *js_init_module_%s(JSContext *ctx, const char *name);\n    js_init_module_%s(ctx, \"%s\");\n  }\n\x00"
                        as *const u8 as *const libc::c_char, (*e).short_name,
                    (*e).short_name, (*e).name);
            i += 1
        }
        i = 0 as libc::c_int;
        while i < cname_list.count {
            let mut e_0: *mut namelist_entry_t =
                &mut *cname_list.array.offset(i as isize) as
                    *mut namelist_entry_t;
            fprintf(fo,
                    b"  js_std_eval_binary(ctx, %s, %s_size, %s);\n\x00" as
                        *const u8 as *const libc::c_char, (*e_0).name,
                    (*e_0).name,
                    if (*e_0).flags != 0 {
                        b"1\x00" as *const u8 as *const libc::c_char
                    } else { b"0\x00" as *const u8 as *const libc::c_char });
            i += 1
        }
        fputs(main_c_template2.as_ptr(), fo);
    }
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
    fclose(fo);
    if output_type as libc::c_uint ==
           OUTPUT_EXECUTABLE as libc::c_int as libc::c_uint {
        return output_executable(out_filename, cfilename.as_mut_ptr(),
                                 use_lto, verbose,
                                 *argv.offset(0 as libc::c_int as isize))
    }
    namelist_free(&mut cname_list);
    namelist_free(&mut cmodule_list);
    namelist_free(&mut init_module_list);
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
