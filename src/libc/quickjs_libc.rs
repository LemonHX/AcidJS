#![allow(dead_code, mutable_transmutes, non_camel_case_types, non_snake_case,
         non_upper_case_globals, unused_assignments, unused_mut)]
#![register_tool(c2rust)]
#![feature(const_raw_ptr_to_usize_cast, extern_types, label_break_value,
           ptr_wrapping_offset_from, register_tool)]
extern "C" {
    pub type __sFILEX;
    pub type _telldir;
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
    fn signal(_: libc::c_int,
              _: Option<unsafe extern "C" fn(_: libc::c_int) -> ()>)
     -> Option<unsafe extern "C" fn(_: libc::c_int) -> ()>;
    #[no_mangle]
    fn waitpid(_: pid_t, _: *mut libc::c_int, _: libc::c_int) -> pid_t;
    #[no_mangle]
    fn malloc(_: libc::c_ulong) -> *mut libc::c_void;
    #[no_mangle]
    fn free(_: *mut libc::c_void);
    #[no_mangle]
    fn atexit(_: Option<unsafe extern "C" fn() -> ()>) -> libc::c_int;
    #[no_mangle]
    fn atoi(_: *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn exit(_: libc::c_int) -> !;
    #[no_mangle]
    fn getenv(_: *const libc::c_char) -> *mut libc::c_char;
    #[no_mangle]
    fn realpath(_: *const libc::c_char, _: *mut libc::c_char)
     -> *mut libc::c_char;
    #[no_mangle]
    static mut __stdinp: *mut FILE;
    #[no_mangle]
    static mut __stdoutp: *mut FILE;
    #[no_mangle]
    static mut __stderrp: *mut FILE;
    #[no_mangle]
    fn clearerr(_: *mut FILE);
    #[no_mangle]
    fn fclose(_: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn feof(_: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn ferror(_: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn fflush(_: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn fgetc(_: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn fopen(_: *const libc::c_char, _: *const libc::c_char) -> *mut FILE;
    #[no_mangle]
    fn fprintf(_: *mut FILE, _: *const libc::c_char, _: ...) -> libc::c_int;
    #[no_mangle]
    fn fputc(_: libc::c_int, _: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn fread(_: *mut libc::c_void, _: libc::c_ulong, _: libc::c_ulong,
             _: *mut FILE) -> libc::c_ulong;
    #[no_mangle]
    fn fseek(_: *mut FILE, _: libc::c_long, _: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn ftell(_: *mut FILE) -> libc::c_long;
    #[no_mangle]
    fn fwrite(_: *const libc::c_void, _: libc::c_ulong, _: libc::c_ulong,
              _: *mut FILE) -> libc::c_ulong;
    #[no_mangle]
    fn putchar(_: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn remove(_: *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn rename(__old: *const libc::c_char, __new: *const libc::c_char)
     -> libc::c_int;
    #[no_mangle]
    fn tmpfile() -> *mut FILE;
    #[no_mangle]
    fn fdopen(_: libc::c_int, _: *const libc::c_char) -> *mut FILE;
    #[no_mangle]
    fn fileno(_: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn pclose(_: *mut FILE) -> libc::c_int;
    #[no_mangle]
    fn popen(_: *const libc::c_char, _: *const libc::c_char) -> *mut FILE;
    #[no_mangle]
    fn snprintf(_: *mut libc::c_char, _: libc::c_ulong,
                _: *const libc::c_char, _: ...) -> libc::c_int;
    #[no_mangle]
    fn memcpy(_: *mut libc::c_void, _: *const libc::c_void, _: libc::c_ulong)
     -> *mut libc::c_void;
    #[no_mangle]
    fn memset(_: *mut libc::c_void, _: libc::c_int, _: libc::c_ulong)
     -> *mut libc::c_void;
    #[no_mangle]
    fn strchr(_: *const libc::c_char, _: libc::c_int) -> *mut libc::c_char;
    #[no_mangle]
    fn strcmp(_: *const libc::c_char, _: *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn strcpy(_: *mut libc::c_char, _: *const libc::c_char)
     -> *mut libc::c_char;
    #[no_mangle]
    fn strerror(_: libc::c_int) -> *mut libc::c_char;
    #[no_mangle]
    fn strlen(_: *const libc::c_char) -> libc::c_ulong;
    #[no_mangle]
    fn strspn(_: *const libc::c_char, _: *const libc::c_char)
     -> libc::c_ulong;
    #[no_mangle]
    fn __assert_rtn(_: *const libc::c_char, _: *const libc::c_char,
                    _: libc::c_int, _: *const libc::c_char) -> !;
    #[no_mangle]
    fn __error() -> *mut libc::c_int;
    #[no_mangle]
    fn open(_: *const libc::c_char, _: libc::c_int, _: ...) -> libc::c_int;
    #[no_mangle]
    fn nanosleep(__rqtp: *const timespec, __rmtp: *mut timespec)
     -> libc::c_int;
    #[no_mangle]
    fn clock_gettime(__clock_id: clockid_t, __tp: *mut timespec)
     -> libc::c_int;
    #[no_mangle]
    fn kill(_: pid_t, _: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn lstat(_: *const libc::c_char, _: *mut stat) -> libc::c_int;
    #[no_mangle]
    fn mkdir(_: *const libc::c_char, _: mode_t) -> libc::c_int;
    #[no_mangle]
    fn stat(_: *const libc::c_char, _: *mut stat) -> libc::c_int;
    #[no_mangle]
    fn _exit(_: libc::c_int) -> !;
    #[no_mangle]
    fn chdir(_: *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn close(_: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn dup(_: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn dup2(_: libc::c_int, _: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn execve(__file: *const libc::c_char, __argv: *const *mut libc::c_char,
              __envp: *const *mut libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn fork() -> pid_t;
    #[no_mangle]
    fn getcwd(_: *mut libc::c_char, _: size_t) -> *mut libc::c_char;
    #[no_mangle]
    fn isatty(_: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn lseek(_: libc::c_int, _: off_t, _: libc::c_int) -> off_t;
    #[no_mangle]
    fn pipe(_: *mut libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn read(_: libc::c_int, _: *mut libc::c_void, _: size_t) -> ssize_t;
    #[no_mangle]
    fn setgid(_: gid_t) -> libc::c_int;
    #[no_mangle]
    fn setuid(_: uid_t) -> libc::c_int;
    #[no_mangle]
    fn sysconf(_: libc::c_int) -> libc::c_long;
    #[no_mangle]
    fn write(__fd: libc::c_int, __buf: *const libc::c_void, __nbyte: size_t)
     -> ssize_t;
    #[no_mangle]
    fn readlink(_: *const libc::c_char, _: *mut libc::c_char, _: size_t)
     -> ssize_t;
    #[no_mangle]
    fn symlink(_: *const libc::c_char, _: *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn select(_: libc::c_int, _: *mut fd_set, _: *mut fd_set, _: *mut fd_set,
              _: *mut timeval) -> libc::c_int;
    #[no_mangle]
    fn closedir(_: *mut DIR) -> libc::c_int;
    #[no_mangle]
    fn opendir(_: *const libc::c_char) -> *mut DIR;
    #[no_mangle]
    fn readdir(_: *mut DIR) -> *mut dirent;
    #[no_mangle]
    fn dlclose(__handle: *mut libc::c_void) -> libc::c_int;
    #[no_mangle]
    fn dlopen(__path: *const libc::c_char, __mode: libc::c_int)
     -> *mut libc::c_void;
    #[no_mangle]
    fn dlsym(__handle: *mut libc::c_void, __symbol: *const libc::c_char)
     -> *mut libc::c_void;
    #[no_mangle]
    fn tcgetattr(_: libc::c_int, _: *mut termios) -> libc::c_int;
    #[no_mangle]
    fn tcsetattr(_: libc::c_int, _: libc::c_int, _: *const termios)
     -> libc::c_int;
    #[no_mangle]
    fn ioctl(_: libc::c_int, _: libc::c_ulong, _: ...) -> libc::c_int;
    #[no_mangle]
    fn utimes(_: *const libc::c_char, _: *const timeval) -> libc::c_int;
    #[no_mangle]
    fn _NSGetEnviron() -> *mut *mut *mut libc::c_char;
    #[no_mangle]
    fn pstrcpy(buf: *mut libc::c_char, buf_size: libc::c_int,
               str: *const libc::c_char);
    #[no_mangle]
    fn pstrcat(buf: *mut libc::c_char, buf_size: libc::c_int,
               s: *const libc::c_char) -> *mut libc::c_char;
    #[no_mangle]
    fn has_suffix(str: *const libc::c_char, suffix: *const libc::c_char)
     -> libc::c_int;
    #[no_mangle]
    fn dbuf_init2(s: *mut DynBuf, opaque: *mut libc::c_void,
                  realloc_func: Option<DynBufReallocFunc>);
    #[no_mangle]
    fn dbuf_put(s: *mut DynBuf, data: *const uint8_t, len: size_t)
     -> libc::c_int;
    #[no_mangle]
    fn dbuf_putc(s: *mut DynBuf, c: uint8_t) -> libc::c_int;
    #[no_mangle]
    fn dbuf_putstr(s: *mut DynBuf, str: *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn dbuf_printf(s: *mut DynBuf, fmt: *const libc::c_char, _: ...)
     -> libc::c_int;
    #[no_mangle]
    fn dbuf_free(s: *mut DynBuf);
    #[no_mangle]
    fn unicode_to_utf8(buf: *mut uint8_t, c: libc::c_uint) -> libc::c_int;
    #[no_mangle]
    fn unicode_from_utf8(p: *const uint8_t, max_len: libc::c_int,
                         pp: *mut *const uint8_t) -> libc::c_int;
    #[no_mangle]
    fn JS_GetRuntimeOpaque(rt: *mut JSRuntime) -> *mut libc::c_void;
    #[no_mangle]
    fn JS_SetRuntimeOpaque(rt: *mut JSRuntime, opaque: *mut libc::c_void);
    #[no_mangle]
    fn JS_MarkValue(rt: *mut JSRuntime, val: JSValue,
                    mark_func: Option<JS_MarkFunc>);
    #[no_mangle]
    fn JS_RunGC(rt: *mut JSRuntime);
    #[no_mangle]
    fn JS_GetRuntime(ctx: *mut JSContext) -> *mut JSRuntime;
    #[no_mangle]
    fn JS_SetClassProto(ctx: *mut JSContext, class_id: JSClassID,
                        obj: JSValue);
    #[no_mangle]
    fn js_free_rt(rt: *mut JSRuntime, ptr: *mut libc::c_void);
    #[no_mangle]
    fn js_realloc_rt(rt: *mut JSRuntime, ptr: *mut libc::c_void, size: size_t)
     -> *mut libc::c_void;
    #[no_mangle]
    fn js_malloc(ctx: *mut JSContext, size: size_t) -> *mut libc::c_void;
    #[no_mangle]
    fn js_free(ctx: *mut JSContext, ptr: *mut libc::c_void);
    #[no_mangle]
    fn js_mallocz(ctx: *mut JSContext, size: size_t) -> *mut libc::c_void;
    #[no_mangle]
    fn JS_FreeAtom(ctx: *mut JSContext, v: JSAtom);
    #[no_mangle]
    fn JS_AtomToCString(ctx: *mut JSContext, atom: JSAtom)
     -> *const libc::c_char;
    #[no_mangle]
    fn JS_NewClassID(pclass_id: *mut JSClassID) -> JSClassID;
    #[no_mangle]
    fn JS_NewClass(rt: *mut JSRuntime, class_id: JSClassID,
                   class_def: *const JSClassDef) -> libc::c_int;
    #[no_mangle]
    fn JS_NewBigInt64(ctx: *mut JSContext, v: int64_t) -> JSValue;
    #[no_mangle]
    fn JS_GetException(ctx: *mut JSContext) -> JSValue;
    #[no_mangle]
    fn JS_IsError(ctx: *mut JSContext, val: JSValue) -> libc::c_int;
    #[no_mangle]
    fn JS_ResetUncatchableError(ctx: *mut JSContext);
    #[no_mangle]
    fn JS_ThrowTypeError(ctx: *mut JSContext, fmt: *const libc::c_char,
                         _: ...) -> JSValue;
    #[no_mangle]
    fn JS_ThrowReferenceError(ctx: *mut JSContext, fmt: *const libc::c_char,
                              _: ...) -> JSValue;
    #[no_mangle]
    fn JS_ThrowRangeError(ctx: *mut JSContext, fmt: *const libc::c_char,
                          _: ...) -> JSValue;
    #[no_mangle]
    fn JS_ThrowOutOfMemory(ctx: *mut JSContext) -> JSValue;
    #[no_mangle]
    fn __JS_FreeValue(ctx: *mut JSContext, v: JSValue);
    #[no_mangle]
    fn __JS_FreeValueRT(rt: *mut JSRuntime, v: JSValue);
    #[no_mangle]
    fn JS_ToBool(ctx: *mut JSContext, val: JSValue) -> libc::c_int;
    #[no_mangle]
    fn JS_ToInt32(ctx: *mut JSContext, pres: *mut int32_t, val: JSValue)
     -> libc::c_int;
    #[no_mangle]
    fn JS_ToInt64(ctx: *mut JSContext, pres: *mut int64_t, val: JSValue)
     -> libc::c_int;
    #[no_mangle]
    fn JS_ToIndex(ctx: *mut JSContext, plen: *mut uint64_t, val: JSValue)
     -> libc::c_int;
    #[no_mangle]
    fn JS_ToFloat64(ctx: *mut JSContext, pres: *mut libc::c_double,
                    val: JSValue) -> libc::c_int;
    #[no_mangle]
    fn JS_ToInt64Ext(ctx: *mut JSContext, pres: *mut int64_t, val: JSValue)
     -> libc::c_int;
    #[no_mangle]
    fn JS_NewStringLen(ctx: *mut JSContext, str1: *const libc::c_char,
                       len1: size_t) -> JSValue;
    #[no_mangle]
    fn JS_NewString(ctx: *mut JSContext, str: *const libc::c_char) -> JSValue;
    #[no_mangle]
    fn JS_ToCStringLen2(ctx: *mut JSContext, plen: *mut size_t, val1: JSValue,
                        cesu8: libc::c_int) -> *const libc::c_char;
    #[no_mangle]
    fn JS_FreeCString(ctx: *mut JSContext, ptr: *const libc::c_char);
    #[no_mangle]
    fn JS_NewObjectClass(ctx: *mut JSContext, class_id: libc::c_int)
     -> JSValue;
    #[no_mangle]
    fn JS_NewObject(ctx: *mut JSContext) -> JSValue;
    #[no_mangle]
    fn JS_IsFunction(ctx: *mut JSContext, val: JSValue) -> libc::c_int;
    #[no_mangle]
    fn JS_NewArray(ctx: *mut JSContext) -> JSValue;
    #[no_mangle]
    fn JS_GetPropertyInternal(ctx: *mut JSContext, obj: JSValue, prop: JSAtom,
                              receiver: JSValue, throw_ref_error: libc::c_int)
     -> JSValue;
    #[no_mangle]
    fn JS_GetPropertyStr(ctx: *mut JSContext, this_obj: JSValue,
                         prop: *const libc::c_char) -> JSValue;
    #[no_mangle]
    fn JS_GetPropertyUint32(ctx: *mut JSContext, this_obj: JSValue,
                            idx: uint32_t) -> JSValue;
    #[no_mangle]
    fn JS_SetPropertyUint32(ctx: *mut JSContext, this_obj: JSValue,
                            idx: uint32_t, val: JSValue) -> libc::c_int;
    #[no_mangle]
    fn JS_SetPropertyStr(ctx: *mut JSContext, this_obj: JSValue,
                         prop: *const libc::c_char, val: JSValue)
     -> libc::c_int;
    #[no_mangle]
    fn JS_GetOwnPropertyNames(ctx: *mut JSContext,
                              ptab: *mut *mut JSPropertyEnum,
                              plen: *mut uint32_t, obj: JSValue,
                              flags: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn JS_Call(ctx: *mut JSContext, func_obj: JSValue, this_obj: JSValue,
               argc: libc::c_int, argv: *mut JSValue) -> JSValue;
    #[no_mangle]
    fn JS_Eval(ctx: *mut JSContext, input: *const libc::c_char,
               input_len: size_t, filename: *const libc::c_char,
               eval_flags: libc::c_int) -> JSValue;
    #[no_mangle]
    fn JS_EvalFunction(ctx: *mut JSContext, fun_obj: JSValue) -> JSValue;
    #[no_mangle]
    fn JS_GetGlobalObject(ctx: *mut JSContext) -> JSValue;
    #[no_mangle]
    fn JS_DefinePropertyValueUint32(ctx: *mut JSContext, this_obj: JSValue,
                                    idx: uint32_t, val: JSValue,
                                    flags: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn JS_DefinePropertyValueStr(ctx: *mut JSContext, this_obj: JSValue,
                                 prop: *const libc::c_char, val: JSValue,
                                 flags: libc::c_int) -> libc::c_int;
    #[no_mangle]
    fn JS_SetOpaque(obj: JSValue, opaque: *mut libc::c_void);
    #[no_mangle]
    fn JS_GetOpaque(obj: JSValue, class_id: JSClassID) -> *mut libc::c_void;
    #[no_mangle]
    fn JS_GetOpaque2(ctx: *mut JSContext, obj: JSValue, class_id: JSClassID)
     -> *mut libc::c_void;
    #[no_mangle]
    fn JS_ParseJSON2(ctx: *mut JSContext, buf: *const libc::c_char,
                     buf_len: size_t, filename: *const libc::c_char,
                     flags: libc::c_int) -> JSValue;
    #[no_mangle]
    fn JS_NewArrayBufferCopy(ctx: *mut JSContext, buf: *const uint8_t,
                             len: size_t) -> JSValue;
    #[no_mangle]
    fn JS_GetArrayBuffer(ctx: *mut JSContext, psize: *mut size_t,
                         obj: JSValue) -> *mut uint8_t;
    #[no_mangle]
    fn JS_SetInterruptHandler(rt: *mut JSRuntime,
                              cb: Option<JSInterruptHandler>,
                              opaque: *mut libc::c_void);
    #[no_mangle]
    fn JS_GetImportMeta(ctx: *mut JSContext, m: *mut JSModuleDef) -> JSValue;
    #[no_mangle]
    fn JS_GetModuleName(ctx: *mut JSContext, m: *mut JSModuleDef) -> JSAtom;
    #[no_mangle]
    fn JS_ExecutePendingJob(rt: *mut JSRuntime, pctx: *mut *mut JSContext)
     -> libc::c_int;
    /* allow function/module */
    /* avoid duplicating 'buf' data */
    /* allow SharedArrayBuffer */
    /* allow object references */
    #[no_mangle]
    fn JS_ReadObject(ctx: *mut JSContext, buf: *const uint8_t,
                     buf_len: size_t, flags: libc::c_int) -> JSValue;
    /* load the dependencies of the module 'obj'. Useful when JS_ReadObject()
   returns a module. */
    #[no_mangle]
    fn JS_ResolveModule(ctx: *mut JSContext, obj: JSValue) -> libc::c_int;
    #[no_mangle]
    fn JS_NewCFunction2(ctx: *mut JSContext, func: Option<JSCFunction>,
                        name: *const libc::c_char, length: libc::c_int,
                        cproto: JSCFunctionEnum, magic: libc::c_int)
     -> JSValue;
    /* Note: c++ does not like nested designators */
    #[no_mangle]
    fn JS_SetPropertyFunctionList(ctx: *mut JSContext, obj: JSValue,
                                  tab: *const JSCFunctionListEntry,
                                  len: libc::c_int);
    /* C module definition */
    #[no_mangle]
    fn JS_NewCModule(ctx: *mut JSContext, name_str: *const libc::c_char,
                     func: Option<JSModuleInitFunc>) -> *mut JSModuleDef;
    /* can only be called before the module is instantiated */
    #[no_mangle]
    fn JS_AddModuleExport(ctx: *mut JSContext, m: *mut JSModuleDef,
                          name_str: *const libc::c_char) -> libc::c_int;
    #[no_mangle]
    fn JS_AddModuleExportList(ctx: *mut JSContext, m: *mut JSModuleDef,
                              tab: *const JSCFunctionListEntry,
                              len: libc::c_int) -> libc::c_int;
    /* can only be called after the module is instantiated */
    #[no_mangle]
    fn JS_SetModuleExport(ctx: *mut JSContext, m: *mut JSModuleDef,
                          export_name: *const libc::c_char, val: JSValue)
     -> libc::c_int;
    #[no_mangle]
    fn JS_SetModuleExportList(ctx: *mut JSContext, m: *mut JSModuleDef,
                              tab: *const JSCFunctionListEntry,
                              len: libc::c_int) -> libc::c_int;
}
pub type __uint8_t = libc::c_uchar;
pub type __uint16_t = libc::c_ushort;
pub type __int32_t = libc::c_int;
pub type __uint32_t = libc::c_uint;
pub type __int64_t = libc::c_longlong;
pub type __uint64_t = libc::c_ulonglong;
pub type __darwin_size_t = libc::c_ulong;
pub type __darwin_ssize_t = libc::c_long;
pub type __darwin_time_t = libc::c_long;
pub type __darwin_blkcnt_t = __int64_t;
pub type __darwin_blksize_t = __int32_t;
pub type __darwin_dev_t = __int32_t;
pub type __darwin_gid_t = __uint32_t;
pub type __darwin_ino64_t = __uint64_t;
pub type __darwin_mode_t = __uint16_t;
pub type __darwin_off_t = __int64_t;
pub type __darwin_pid_t = __int32_t;
pub type __darwin_suseconds_t = __int32_t;
pub type __darwin_uid_t = __uint32_t;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct _opaque_pthread_mutex_t {
    pub __sig: libc::c_long,
    pub __opaque: [libc::c_char; 56],
}
pub type __darwin_pthread_mutex_t = _opaque_pthread_mutex_t;
pub type pid_t = __darwin_pid_t;
pub type int8_t = libc::c_schar;
pub type int16_t = libc::c_short;
pub type int32_t = libc::c_int;
pub type int64_t = libc::c_longlong;
pub type uintptr_t = libc::c_ulong;
pub type size_t = __darwin_size_t;
pub type uid_t = __darwin_uid_t;
pub type sig_t = Option<unsafe extern "C" fn(_: libc::c_int) -> ()>;
pub type uint8_t = libc::c_uchar;
pub type uint16_t = libc::c_ushort;
pub type uint32_t = libc::c_uint;
pub type uint64_t = libc::c_ulonglong;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct timeval {
    pub tv_sec: __darwin_time_t,
    pub tv_usec: __darwin_suseconds_t,
}
pub type dev_t = __darwin_dev_t;
pub type mode_t = __darwin_mode_t;
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
pub type off_t = __darwin_off_t;
pub type ssize_t = __darwin_ssize_t;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct timespec {
    pub tv_sec: __darwin_time_t,
    pub tv_nsec: libc::c_long,
}
pub type clockid_t = libc::c_uint;
pub const _CLOCK_THREAD_CPUTIME_ID: clockid_t = 16;
pub const _CLOCK_PROCESS_CPUTIME_ID: clockid_t = 12;
pub const _CLOCK_UPTIME_RAW_APPROX: clockid_t = 9;
pub const _CLOCK_UPTIME_RAW: clockid_t = 8;
pub const _CLOCK_MONOTONIC_RAW_APPROX: clockid_t = 5;
pub const _CLOCK_MONOTONIC_RAW: clockid_t = 4;
pub const _CLOCK_MONOTONIC: clockid_t = 6;
pub const _CLOCK_REALTIME: clockid_t = 0;
pub type blkcnt_t = __darwin_blkcnt_t;
pub type blksize_t = __darwin_blksize_t;
pub type nlink_t = __uint16_t;
pub type gid_t = __darwin_gid_t;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct stat {
    pub st_dev: dev_t,
    pub st_mode: mode_t,
    pub st_nlink: nlink_t,
    pub st_ino: __darwin_ino64_t,
    pub st_uid: uid_t,
    pub st_gid: gid_t,
    pub st_rdev: dev_t,
    pub st_atimespec: timespec,
    pub st_mtimespec: timespec,
    pub st_ctimespec: timespec,
    pub st_birthtimespec: timespec,
    pub st_size: off_t,
    pub st_blocks: blkcnt_t,
    pub st_blksize: blksize_t,
    pub st_flags: __uint32_t,
    pub st_gen: __uint32_t,
    pub st_lspare: __int32_t,
    pub st_qspare: [__int64_t; 2],
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct fd_set {
    pub fds_bits: [__int32_t; 32],
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct dirent {
    pub d_ino: __uint64_t,
    pub d_seekoff: __uint64_t,
    pub d_reclen: __uint16_t,
    pub d_namlen: __uint16_t,
    pub d_type: __uint8_t,
    pub d_name: [libc::c_char; 1024],
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct DIR {
    pub __dd_fd: libc::c_int,
    pub __dd_loc: libc::c_long,
    pub __dd_size: libc::c_long,
    pub __dd_buf: *mut libc::c_char,
    pub __dd_len: libc::c_int,
    pub __dd_seek: libc::c_long,
    pub __padding: libc::c_long,
    pub __dd_flags: libc::c_int,
    pub __dd_lock: __darwin_pthread_mutex_t,
    pub __dd_td: *mut _telldir,
}
pub type tcflag_t = libc::c_ulong;
pub type cc_t = libc::c_uchar;
pub type speed_t = libc::c_ulong;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct termios {
    pub c_iflag: tcflag_t,
    pub c_oflag: tcflag_t,
    pub c_cflag: tcflag_t,
    pub c_lflag: tcflag_t,
    pub c_cc: [cc_t; 20],
    pub c_ispeed: speed_t,
    pub c_ospeed: speed_t,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct winsize {
    pub ws_row: libc::c_ushort,
    pub ws_col: libc::c_ushort,
    pub ws_xpixel: libc::c_ushort,
    pub ws_ypixel: libc::c_ushort,
}
/*
 * QuickJS C library
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
pub type sighandler_t = sig_t;
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
/* XXX: should take an extra argument to pass slack information to the caller */
pub type DynBufReallocFunc
    =
    unsafe extern "C" fn(_: *mut libc::c_void, _: *mut libc::c_void,
                         _: size_t) -> *mut libc::c_void;
#[derive(Copy, Clone)]
#[repr(C)]
pub struct DynBuf {
    pub buf: *mut uint8_t,
    pub size: size_t,
    pub allocated_size: size_t,
    pub error: libc::c_int,
    pub realloc_func: Option<DynBufReallocFunc>,
    pub opaque: *mut libc::c_void,
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
pub struct JSOSRWHandler {
    pub link: list_head,
    pub fd: libc::c_int,
    pub rw_func: [JSValue; 2],
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
pub type JSModuleInitFunc
    =
    unsafe extern "C" fn(_: *mut JSContext, _: *mut JSModuleDef)
        -> libc::c_int;
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
pub type JSModuleNormalizeFunc
    =
    unsafe extern "C" fn(_: *mut JSContext, _: *const libc::c_char,
                         _: *const libc::c_char, _: *mut libc::c_void)
        -> *mut libc::c_char;
pub type JSHostPromiseRejectionTracker
    =
    unsafe extern "C" fn(_: *mut JSContext, _: JSValue, _: JSValue,
                         _: libc::c_int, _: *mut libc::c_void) -> ();
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
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSOSSignalHandler {
    pub link: list_head,
    pub sig_num: libc::c_int,
    pub func: JSValue,
}
pub type JSClassID = uint32_t;
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
pub struct JSClassDef {
    pub class_name: *const libc::c_char,
    pub finalizer: Option<JSClassFinalizer>,
    pub gc_mark: Option<JSClassGCMark>,
    pub call: Option<JSClassCall>,
    pub exotic: *mut JSClassExoticMethods,
}
/* C function definition */
pub type JSCFunctionEnum = libc::c_uint;
pub const JS_CFUNC_iterator_next: JSCFunctionEnum = 12;
pub const JS_CFUNC_setter_magic: JSCFunctionEnum = 11;
pub const JS_CFUNC_getter_magic: JSCFunctionEnum = 10;
pub const JS_CFUNC_setter: JSCFunctionEnum = 9;
pub const JS_CFUNC_getter: JSCFunctionEnum = 8;
pub const JS_CFUNC_f_f_f: JSCFunctionEnum = 7;
pub const JS_CFUNC_f_f: JSCFunctionEnum = 6;
pub const JS_CFUNC_constructor_or_func_magic: JSCFunctionEnum = 5;
pub const JS_CFUNC_constructor_or_func: JSCFunctionEnum = 4;
pub const JS_CFUNC_constructor_magic: JSCFunctionEnum = 3;
pub const JS_CFUNC_constructor: JSCFunctionEnum = 2;
pub const JS_CFUNC_generic_magic: JSCFunctionEnum = 1;
/* XXX: should rename for namespace isolation */
pub const JS_CFUNC_generic: JSCFunctionEnum = 0;
/* C property definition */
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSCFunctionListEntry {
    pub name: *const libc::c_char,
    pub prop_flags: uint8_t,
    pub def_type: uint8_t,
    pub magic: int16_t,
    pub u: C2RustUnnamed_18,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub union C2RustUnnamed_18 {
    pub func: C2RustUnnamed_22,
    pub getset: C2RustUnnamed_21,
    pub alias: C2RustUnnamed_20,
    pub prop_list: C2RustUnnamed_19,
    pub str_0: *const libc::c_char,
    pub i32_0: int32_t,
    pub i64_0: int64_t,
    pub f64_0: libc::c_double,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_19 {
    pub tab: *const JSCFunctionListEntry,
    pub len: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_20 {
    pub name: *const libc::c_char,
    pub base: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_21 {
    pub get: JSCFunctionType,
    pub set: JSCFunctionType,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct C2RustUnnamed_22 {
    pub length: uint8_t,
    pub cproto: uint8_t,
    pub cfunc: JSCFunctionType,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSSTDFile {
    pub f: *mut FILE,
    pub close_in_finalizer: libc::c_int,
    pub is_popen: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSThreadState {
    pub os_rw_handlers: list_head,
    pub os_signal_handlers: list_head,
    pub os_timers: list_head,
    pub port_list: list_head,
    pub eval_script_recurse: libc::c_int,
    pub recv_pipe: *mut JSWorkerMessagePipe,
    pub send_pipe: *mut JSWorkerMessagePipe,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSWorkerMessagePipe {
    pub ref_count: libc::c_int,
    pub msg_queue: list_head,
    pub read_fd: libc::c_int,
    pub write_fd: libc::c_int,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSOSTimer {
    pub link: list_head,
    pub has_object: libc::c_int,
    pub timeout: int64_t,
    pub func: JSValue,
}
#[derive(Copy, Clone)]
#[repr(C)]
pub struct JSWorkerMessageHandler {
    pub link: list_head,
    pub recv_pipe: *mut JSWorkerMessagePipe,
    pub on_message_func: JSValue,
}
pub type JSInitModuleFunc
    =
    unsafe extern "C" fn(_: *mut JSContext, _: *const libc::c_char)
        -> *mut JSModuleDef;
#[inline]
unsafe extern "C" fn __darwin_fd_isset(mut _n: libc::c_int,
                                       mut _p: *const fd_set) -> libc::c_int {
    return (*_p).fds_bits[(_n as
                               libc::c_ulong).wrapping_div((::std::mem::size_of::<__int32_t>()
                                                                as
                                                                libc::c_ulong).wrapping_mul(8
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                libc::c_ulong))
                              as usize] &
               ((1 as libc::c_int as libc::c_ulong) <<
                    (_n as
                         libc::c_ulong).wrapping_rem((::std::mem::size_of::<__int32_t>()
                                                          as
                                                          libc::c_ulong).wrapping_mul(8
                                                                                          as
                                                                                          libc::c_int
                                                                                          as
                                                                                          libc::c_ulong)))
                   as __int32_t;
}
#[inline]
unsafe extern "C" fn max_int(mut a: libc::c_int, mut b: libc::c_int)
 -> libc::c_int {
    if a > b { return a } else { return b };
}
#[inline]
unsafe extern "C" fn dbuf_error(mut s: *mut DynBuf) -> libc::c_int {
    return (*s).error;
}
/* return the pointer of type 'type *' containing 'el' as field 'member' */
#[inline]
unsafe extern "C" fn init_list_head(mut head: *mut list_head) {
    (*head).prev = head;
    (*head).next = head;
}
/* insert 'el' between 'prev' and 'next' */
#[inline]
unsafe extern "C" fn __list_add(mut el: *mut list_head,
                                mut prev: *mut list_head,
                                mut next: *mut list_head) {
    (*prev).next = el;
    (*el).prev = prev;
    (*el).next = next;
    (*next).prev = el;
}
/* add 'el' at the end of the list 'head' (= before element head) */
#[inline]
unsafe extern "C" fn list_add_tail(mut el: *mut list_head,
                                   mut head: *mut list_head) {
    __list_add(el, (*head).prev, head); /* fail safe */
}
#[inline]
unsafe extern "C" fn list_del(mut el: *mut list_head) {
    let mut prev: *mut list_head = 0 as *mut list_head;
    let mut next: *mut list_head = 0 as *mut list_head;
    prev = (*el).prev;
    next = (*el).next;
    (*prev).next = next;
    (*next).prev = prev;
    (*el).prev = 0 as *mut list_head;
    (*el).next = 0 as *mut list_head;
    /* fail safe */
}
#[inline]
unsafe extern "C" fn list_empty(mut el: *mut list_head) -> libc::c_int {
    return ((*el).next == el) as libc::c_int;
}
/* same as JS_VALUE_GET_TAG, but return JS_TAG_FLOAT64 with NaN boxing */
#[inline]
unsafe extern "C" fn __JS_NewFloat64(mut ctx: *mut JSContext,
                                     mut d: libc::c_double) -> JSValue {
    let mut v: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    v.tag = JS_TAG_FLOAT64 as libc::c_int as int64_t;
    v.u.float64 = d;
    return v;
}
#[inline(always)]
unsafe extern "C" fn JS_NewBool(mut ctx: *mut JSContext, mut val: libc::c_int)
 -> JSValue {
    return {
               let mut init =
                   JSValue{u:
                               JSValueUnion{int32:
                                                (val != 0 as libc::c_int) as
                                                    libc::c_int,},
                           tag: JS_TAG_BOOL as libc::c_int as int64_t,};
               init
           };
}
#[inline(always)]
unsafe extern "C" fn JS_NewInt32(mut ctx: *mut JSContext, mut val: int32_t)
 -> JSValue {
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: val,},
                           tag: JS_TAG_INT as libc::c_int as int64_t,};
               init
           };
}
#[inline(always)]
unsafe extern "C" fn JS_NewInt64(mut ctx: *mut JSContext, mut val: int64_t)
 -> JSValue {
    let mut v: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    if val == val as int32_t as libc::c_longlong {
        v = JS_NewInt32(ctx, val as int32_t)
    } else { v = __JS_NewFloat64(ctx, val as libc::c_double) }
    return v;
}
#[inline]
unsafe extern "C" fn JS_IsBigInt(mut ctx: *mut JSContext, mut v: JSValue)
 -> libc::c_int {
    let mut tag: libc::c_int = v.tag as int32_t;
    return (tag == JS_TAG_BIG_INT as libc::c_int) as libc::c_int;
}
#[inline]
unsafe extern "C" fn JS_IsNull(mut v: JSValue) -> libc::c_int {
    return (v.tag as int32_t == JS_TAG_NULL as libc::c_int) as libc::c_int;
}
#[inline]
unsafe extern "C" fn JS_IsUndefined(mut v: JSValue) -> libc::c_int {
    return (v.tag as int32_t == JS_TAG_UNDEFINED as libc::c_int) as
               libc::c_int;
}
#[inline]
unsafe extern "C" fn JS_IsException(mut v: JSValue) -> libc::c_int {
    return (v.tag as int32_t == JS_TAG_EXCEPTION as libc::c_int) as
               libc::c_int as libc::c_long as libc::c_int;
}
#[inline]
unsafe extern "C" fn JS_IsString(mut v: JSValue) -> libc::c_int {
    return (v.tag as int32_t == JS_TAG_STRING as libc::c_int) as libc::c_int;
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
#[inline]
unsafe extern "C" fn JS_FreeValueRT(mut rt: *mut JSRuntime, mut v: JSValue) {
    if v.tag as int32_t as libc::c_uint >=
           JS_TAG_FIRST as libc::c_int as libc::c_uint {
        let mut p: *mut JSRefCountHeader = v.u.ptr as *mut JSRefCountHeader;
        (*p).ref_count -= 1;
        if (*p).ref_count <= 0 as libc::c_int { __JS_FreeValueRT(rt, v); }
    };
}
#[inline]
unsafe extern "C" fn JS_DupValue(mut ctx: *mut JSContext, mut v: JSValue)
 -> JSValue {
    if v.tag as int32_t as libc::c_uint >=
           JS_TAG_FIRST as libc::c_int as libc::c_uint {
        let mut p: *mut JSRefCountHeader = v.u.ptr as *mut JSRefCountHeader;
        (*p).ref_count += 1
    }
    return v;
}
#[inline]
unsafe extern "C" fn JS_ToUint32(mut ctx: *mut JSContext,
                                 mut pres: *mut uint32_t, mut val: JSValue)
 -> libc::c_int {
    return JS_ToInt32(ctx, pres as *mut int32_t, val);
}
#[inline]
unsafe extern "C" fn JS_ToCStringLen(mut ctx: *mut JSContext,
                                     mut plen: *mut size_t, mut val1: JSValue)
 -> *const libc::c_char {
    return JS_ToCStringLen2(ctx, plen, val1, 0 as libc::c_int);
}
#[inline]
unsafe extern "C" fn JS_ToCString(mut ctx: *mut JSContext, mut val1: JSValue)
 -> *const libc::c_char {
    return JS_ToCStringLen2(ctx, 0 as *mut size_t, val1, 0 as libc::c_int);
}
#[inline(always)]
unsafe extern "C" fn JS_GetProperty(mut ctx: *mut JSContext,
                                    mut this_obj: JSValue, mut prop: JSAtom)
 -> JSValue {
    return JS_GetPropertyInternal(ctx, this_obj, prop, this_obj,
                                  0 as libc::c_int);
}
#[inline]
unsafe extern "C" fn JS_NewCFunction(mut ctx: *mut JSContext,
                                     mut func: Option<JSCFunction>,
                                     mut name: *const libc::c_char,
                                     mut length: libc::c_int) -> JSValue {
    return JS_NewCFunction2(ctx, func, name, length, JS_CFUNC_generic,
                            0 as libc::c_int);
}
static mut os_pending_signals: uint64_t = 0;
static mut os_poll_func:
       Option<unsafe extern "C" fn(_: *mut JSContext) -> libc::c_int> =
    None;
unsafe extern "C" fn js_std_dbuf_init(mut ctx: *mut JSContext,
                                      mut s: *mut DynBuf) {
    dbuf_init2(s, JS_GetRuntime(ctx) as *mut libc::c_void,
               ::std::mem::transmute::<Option<unsafe extern "C" fn(_:
                                                                       *mut JSRuntime,
                                                                   _:
                                                                       *mut libc::c_void,
                                                                   _: size_t)
                                                  -> *mut libc::c_void>,
                                       Option<DynBufReallocFunc>>(Some(js_realloc_rt
                                                                           as
                                                                           unsafe extern "C" fn(_:
                                                                                                    *mut JSRuntime,
                                                                                                _:
                                                                                                    *mut libc::c_void,
                                                                                                _:
                                                                                                    size_t)
                                                                               ->
                                                                                   *mut libc::c_void)));
}
unsafe extern "C" fn my_isdigit(mut c: libc::c_int) -> libc::c_int {
    return (c >= '0' as i32 && c <= '9' as i32) as libc::c_int;
}
unsafe extern "C" fn js_printf_internal(mut ctx: *mut JSContext,
                                        mut argc: libc::c_int,
                                        mut argv: *mut JSValue,
                                        mut fp: *mut FILE) -> JSValue {
    let mut current_block: u64;
    let mut fmtbuf: [libc::c_char; 32] = [0; 32];
    let mut cbuf: [uint8_t; 7] = [0; 7];
    let mut res: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut dbuf: DynBuf =
        DynBuf{buf: 0 as *mut uint8_t,
               size: 0,
               allocated_size: 0,
               error: 0,
               realloc_func: None,
               opaque: 0 as *mut libc::c_void,};
    let mut fmt_str: *const libc::c_char = 0 as *const libc::c_char;
    let mut fmt: *const uint8_t = 0 as *const uint8_t;
    let mut fmt_end: *const uint8_t = 0 as *const uint8_t;
    let mut p: *const uint8_t = 0 as *const uint8_t;
    let mut q: *mut libc::c_char = 0 as *mut libc::c_char;
    let mut i: libc::c_int = 0;
    let mut c: libc::c_int = 0;
    let mut len: libc::c_int = 0;
    let mut mod_0: libc::c_int = 0;
    let mut fmt_len: size_t = 0;
    let mut int32_arg: int32_t = 0;
    let mut int64_arg: int64_t = 0;
    let mut double_arg: libc::c_double = 0.;
    let mut string_arg: *const libc::c_char = 0 as *const libc::c_char;
    /* Use indirect call to dbuf_printf to prevent gcc warning */
    let mut dbuf_printf_fun:
            Option<unsafe extern "C" fn(_: *mut DynBuf,
                                        _: *const libc::c_char, _: ...)
                       -> libc::c_int> =
        ::std::mem::transmute::<*mut libc::c_void,
                                Option<unsafe extern "C" fn(_: *mut DynBuf,
                                                            _:
                                                                *const libc::c_char,
                                                            _: ...)
                                           ->
                                               libc::c_int>>(::std::mem::transmute::<Option<unsafe extern "C" fn(_:
                                                                                                                     *mut DynBuf,
                                                                                                                 _:
                                                                                                                     *const libc::c_char,
                                                                                                                 _:
                                                                                                                     ...)
                                                                                                ->
                                                                                                    libc::c_int>,
                                                                                     *mut libc::c_void>(Some(dbuf_printf
                                                                                                                 as
                                                                                                                 unsafe extern "C" fn(_:
                                                                                                                                          *mut DynBuf,
                                                                                                                                      _:
                                                                                                                                          *const libc::c_char,
                                                                                                                                      _:
                                                                                                                                          ...)
                                                                                                                     ->
                                                                                                                         libc::c_int))); /* copy '%' */
    js_std_dbuf_init(ctx, &mut dbuf);
    if argc > 0 as libc::c_int {
        fmt_str =
            JS_ToCStringLen(ctx, &mut fmt_len,
                            *argv.offset(0 as libc::c_int as isize));
        if fmt_str.is_null() {
            current_block = 3554158773753863382;
        } else {
            i = 1 as libc::c_int;
            fmt = fmt_str as *const uint8_t;
            fmt_end = fmt.offset(fmt_len as isize);
            loop  {
                if !(fmt < fmt_end) {
                    current_block = 11865390570819897086;
                    break ;
                }
                p = fmt;
                while fmt < fmt_end && *fmt as libc::c_int != '%' as i32 {
                    fmt = fmt.offset(1)
                }
                dbuf_put(&mut dbuf, p,
                         fmt.wrapping_offset_from(p) as libc::c_long as
                             size_t);
                if fmt >= fmt_end {
                    current_block = 11865390570819897086;
                    break ;
                }
                q = fmtbuf.as_mut_ptr();
                let fresh0 = fmt;
                fmt = fmt.offset(1);
                let fresh1 = q;
                q = q.offset(1);
                *fresh1 = *fresh0 as libc::c_char;
                loop 
                     /* flags */
                     {
                    c = *fmt as libc::c_int;
                    if !(c == '0' as i32 || c == '#' as i32 || c == '+' as i32
                             || c == '-' as i32 || c == ' ' as i32 ||
                             c == '\'' as i32) {
                        current_block = 17788412896529399552;
                        break ;
                    }
                    if q >=
                           fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                          as libc::c_ulong as
                                                          isize).offset(-(1 as
                                                                              libc::c_int
                                                                              as
                                                                              isize))
                       {
                        current_block = 531654916993384523;
                        break ;
                    }
                    let fresh2 = q;
                    q = q.offset(1);
                    *fresh2 = c as libc::c_char;
                    fmt = fmt.offset(1)
                }
                match current_block {
                    17788412896529399552 =>
                    /* width */
                    {
                        if *fmt as libc::c_int == '*' as i32 {
                            if i >= argc {
                                current_block = 11462318409609933101;
                            } else {
                                let fresh3 = i;
                                i = i + 1;
                                if JS_ToInt32(ctx, &mut int32_arg,
                                              *argv.offset(fresh3 as isize))
                                       != 0 {
                                    current_block = 3554158773753863382;
                                    break ;
                                }
                                q =
                                    q.offset(snprintf(q,
                                                      fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                                     as
                                                                                     libc::c_ulong
                                                                                     as
                                                                                     isize).wrapping_offset_from(q)
                                                          as libc::c_long as
                                                          libc::c_ulong,
                                                      b"%d\x00" as *const u8
                                                          as
                                                          *const libc::c_char,
                                                      int32_arg) as isize);
                                fmt = fmt.offset(1);
                                current_block = 3934796541983872331;
                            }
                        } else {
                            loop  {
                                if !(my_isdigit(*fmt as libc::c_int) != 0) {
                                    current_block = 3934796541983872331;
                                    break ;
                                }
                                if q >=
                                       fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                      as
                                                                      libc::c_ulong
                                                                      as
                                                                      isize).offset(-(1
                                                                                          as
                                                                                          libc::c_int
                                                                                          as
                                                                                          isize))
                                   {
                                    current_block = 531654916993384523;
                                    break ;
                                }
                                let fresh4 = fmt;
                                fmt = fmt.offset(1);
                                let fresh5 = q;
                                q = q.offset(1);
                                *fresh5 = *fresh4 as libc::c_char
                            }
                        }
                        match current_block {
                            531654916993384523 => { }
                            _ => {
                                match current_block {
                                    3934796541983872331 => {
                                        if *fmt as libc::c_int == '.' as i32 {
                                            if q >=
                                                   fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                                  as
                                                                                  libc::c_ulong
                                                                                  as
                                                                                  isize).offset(-(1
                                                                                                      as
                                                                                                      libc::c_int
                                                                                                      as
                                                                                                      isize))
                                               {
                                                current_block =
                                                    531654916993384523;
                                            } else {
                                                let fresh6 = fmt;
                                                fmt = fmt.offset(1);
                                                let fresh7 = q;
                                                q = q.offset(1);
                                                *fresh7 =
                                                    *fresh6 as libc::c_char;
                                                if *fmt as libc::c_int ==
                                                       '*' as i32 {
                                                    if i >= argc {
                                                        current_block =
                                                            11462318409609933101;
                                                    } else {
                                                        let fresh8 = i;
                                                        i = i + 1;
                                                        if JS_ToInt32(ctx,
                                                                      &mut int32_arg,
                                                                      *argv.offset(fresh8
                                                                                       as
                                                                                       isize))
                                                               != 0 {
                                                            current_block =
                                                                3554158773753863382;
                                                            break ;
                                                        }
                                                        q =
                                                            q.offset(snprintf(q,
                                                                              fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                                                             as
                                                                                                             libc::c_ulong
                                                                                                             as
                                                                                                             isize).wrapping_offset_from(q)
                                                                                  as
                                                                                  libc::c_long
                                                                                  as
                                                                                  libc::c_ulong,
                                                                              b"%d\x00"
                                                                                  as
                                                                                  *const u8
                                                                                  as
                                                                                  *const libc::c_char,
                                                                              int32_arg)
                                                                         as
                                                                         isize);
                                                        fmt = fmt.offset(1);
                                                        current_block =
                                                            13763002826403452995;
                                                    }
                                                } else {
                                                    loop  {
                                                        if !(my_isdigit(*fmt
                                                                            as
                                                                            libc::c_int)
                                                                 != 0) {
                                                            current_block =
                                                                13763002826403452995;
                                                            break ;
                                                        }
                                                        if q >=
                                                               fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                                              as
                                                                                              libc::c_ulong
                                                                                              as
                                                                                              isize).offset(-(1
                                                                                                                  as
                                                                                                                  libc::c_int
                                                                                                                  as
                                                                                                                  isize))
                                                           {
                                                            current_block =
                                                                531654916993384523;
                                                            break ;
                                                        }
                                                        let fresh9 = fmt;
                                                        fmt = fmt.offset(1);
                                                        let fresh10 = q;
                                                        q = q.offset(1);
                                                        *fresh10 =
                                                            *fresh9 as
                                                                libc::c_char
                                                    }
                                                }
                                            }
                                        } else {
                                            current_block =
                                                13763002826403452995;
                                        }
                                        match current_block {
                                            531654916993384523 => { }
                                            11462318409609933101 => { }
                                            _ => {
                                                /* we only support the "l" modifier for 64 bit numbers */
                                                mod_0 = ' ' as i32;
                                                if *fmt as libc::c_int ==
                                                       'l' as i32 {
                                                    let fresh11 = fmt;
                                                    fmt = fmt.offset(1);
                                                    mod_0 =
                                                        *fresh11 as
                                                            libc::c_int
                                                }
                                                /* type */
                                                let fresh12 = fmt;
                                                fmt = fmt.offset(1);
                                                c = *fresh12 as libc::c_int;
                                                if q >=
                                                       fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                                      as
                                                                                      libc::c_ulong
                                                                                      as
                                                                                      isize).offset(-(1
                                                                                                          as
                                                                                                          libc::c_int
                                                                                                          as
                                                                                                          isize))
                                                   {
                                                    current_block =
                                                        531654916993384523;
                                                } else {
                                                    let fresh13 = q;
                                                    q = q.offset(1);
                                                    *fresh13 =
                                                        c as libc::c_char;
                                                    *q =
                                                        '\u{0}' as i32 as
                                                            libc::c_char;
                                                    match c {
                                                        99 => {
                                                            current_block =
                                                                8540457983839371850;
                                                            match current_block
                                                                {
                                                                14995964958133890621
                                                                => {
                                                                    dbuf_putc(&mut dbuf,
                                                                              '%'
                                                                                  as
                                                                                  i32
                                                                                  as
                                                                                  uint8_t);
                                                                    continue ;
                                                                }
                                                                1744868089722402712
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh16 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        if JS_ToInt64Ext(ctx,
                                                                                         &mut int64_arg,
                                                                                         *argv.offset(fresh16
                                                                                                          as
                                                                                                          isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        if mod_0
                                                                               ==
                                                                               'l'
                                                                                   as
                                                                                   i32
                                                                           {
                                                                            /* 64 bit number */
                                                                            if !(q
                                                                                     >=
                                                                                     fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                                                                    as
                                                                                                                    libc::c_ulong
                                                                                                                    as
                                                                                                                    isize).offset(-(2
                                                                                                                                        as
                                                                                                                                        libc::c_int
                                                                                                                                        as
                                                                                                                                        isize)))
                                                                               {
                                                                                *q.offset(1
                                                                                              as
                                                                                              libc::c_int
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    *q.offset(-(1
                                                                                                    as
                                                                                                    libc::c_int)
                                                                                                  as
                                                                                                  isize);
                                                                                let ref mut fresh17 =
                                                                                    *q.offset(0
                                                                                                  as
                                                                                                  libc::c_int
                                                                                                  as
                                                                                                  isize);
                                                                                *fresh17
                                                                                    =
                                                                                    'l'
                                                                                        as
                                                                                        i32
                                                                                        as
                                                                                        libc::c_char;
                                                                                *q.offset(-(1
                                                                                                as
                                                                                                libc::c_int)
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    *fresh17;
                                                                                *q.offset(2
                                                                                              as
                                                                                              libc::c_int
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    '\u{0}'
                                                                                        as
                                                                                        i32
                                                                                        as
                                                                                        libc::c_char;
                                                                                dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                        as
                                                                                                                                        *mut DynBuf,
                                                                                                                                    fmtbuf.as_mut_ptr(),
                                                                                                                                    int64_arg);
                                                                                continue
                                                                                    ;
                                                                            }
                                                                        } else {
                                                                            dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                    as
                                                                                                                                    *mut DynBuf,
                                                                                                                                fmtbuf.as_mut_ptr(),
                                                                                                                                int64_arg
                                                                                                                                    as
                                                                                                                                    libc::c_int);
                                                                            continue
                                                                                ;
                                                                        }
                                                                        current_block
                                                                            =
                                                                            531654916993384523;
                                                                    }
                                                                }
                                                                18336436617056464348
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        /* XXX: handle strings containing null characters */
                                                                        let fresh18 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        string_arg
                                                                            =
                                                                            JS_ToCString(ctx,
                                                                                         *argv.offset(fresh18
                                                                                                          as
                                                                                                          isize));
                                                                        if string_arg.is_null()
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                as
                                                                                                                                *mut DynBuf,
                                                                                                                            fmtbuf.as_mut_ptr(),
                                                                                                                            string_arg);
                                                                        JS_FreeCString(ctx,
                                                                                       string_arg);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                                13214951420223402289
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh19 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        if JS_ToFloat64(ctx,
                                                                                        &mut double_arg,
                                                                                        *argv.offset(fresh19
                                                                                                         as
                                                                                                         isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                as
                                                                                                                                *mut DynBuf,
                                                                                                                            fmtbuf.as_mut_ptr(),
                                                                                                                            double_arg);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                                _ => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        if JS_IsString(*argv.offset(i
                                                                                                        as
                                                                                                        isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            let fresh14 =
                                                                                i;
                                                                            i
                                                                                =
                                                                                i
                                                                                    +
                                                                                    1;
                                                                            string_arg
                                                                                =
                                                                                JS_ToCString(ctx,
                                                                                             *argv.offset(fresh14
                                                                                                              as
                                                                                                              isize));
                                                                            if string_arg.is_null()
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    3554158773753863382;
                                                                                break
                                                                                    ;
                                                                            }
                                                                            int32_arg
                                                                                =
                                                                                unicode_from_utf8(string_arg
                                                                                                      as
                                                                                                      *mut uint8_t,
                                                                                                  6
                                                                                                      as
                                                                                                      libc::c_int,
                                                                                                  &mut p);
                                                                            JS_FreeCString(ctx,
                                                                                           string_arg);
                                                                        } else {
                                                                            let fresh15 =
                                                                                i;
                                                                            i
                                                                                =
                                                                                i
                                                                                    +
                                                                                    1;
                                                                            if JS_ToInt32(ctx,
                                                                                          &mut int32_arg,
                                                                                          *argv.offset(fresh15
                                                                                                           as
                                                                                                           isize))
                                                                                   !=
                                                                                   0
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    3554158773753863382;
                                                                                break
                                                                                    ;
                                                                            }
                                                                        }
                                                                        /* handle utf-8 encoding explicitly */
                                                                        if int32_arg
                                                                               as
                                                                               libc::c_uint
                                                                               >
                                                                               0x10ffff
                                                                                   as
                                                                                   libc::c_int
                                                                                   as
                                                                                   libc::c_uint
                                                                           {
                                                                            int32_arg
                                                                                =
                                                                                0xfffd
                                                                                    as
                                                                                    libc::c_int
                                                                        }
                                                                        /* ignore conversion flags, width and precision */
                                                                        len =
                                                                            unicode_to_utf8(cbuf.as_mut_ptr(),
                                                                                            int32_arg
                                                                                                as
                                                                                                libc::c_uint);
                                                                        dbuf_put(&mut dbuf,
                                                                                 cbuf.as_mut_ptr(),
                                                                                 len
                                                                                     as
                                                                                     size_t);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        100 | 105 | 111 | 117
                                                        | 120 | 88 => {
                                                            current_block =
                                                                1744868089722402712;
                                                            match current_block
                                                                {
                                                                14995964958133890621
                                                                => {
                                                                    dbuf_putc(&mut dbuf,
                                                                              '%'
                                                                                  as
                                                                                  i32
                                                                                  as
                                                                                  uint8_t);
                                                                    continue ;
                                                                }
                                                                1744868089722402712
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh16 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        if JS_ToInt64Ext(ctx,
                                                                                         &mut int64_arg,
                                                                                         *argv.offset(fresh16
                                                                                                          as
                                                                                                          isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        if mod_0
                                                                               ==
                                                                               'l'
                                                                                   as
                                                                                   i32
                                                                           {
                                                                            if !(q
                                                                                     >=
                                                                                     fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                                                                    as
                                                                                                                    libc::c_ulong
                                                                                                                    as
                                                                                                                    isize).offset(-(2
                                                                                                                                        as
                                                                                                                                        libc::c_int
                                                                                                                                        as
                                                                                                                                        isize)))
                                                                               {
                                                                                *q.offset(1
                                                                                              as
                                                                                              libc::c_int
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    *q.offset(-(1
                                                                                                    as
                                                                                                    libc::c_int)
                                                                                                  as
                                                                                                  isize);
                                                                                let ref mut fresh17 =
                                                                                    *q.offset(0
                                                                                                  as
                                                                                                  libc::c_int
                                                                                                  as
                                                                                                  isize);
                                                                                *fresh17
                                                                                    =
                                                                                    'l'
                                                                                        as
                                                                                        i32
                                                                                        as
                                                                                        libc::c_char;
                                                                                *q.offset(-(1
                                                                                                as
                                                                                                libc::c_int)
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    *fresh17;
                                                                                *q.offset(2
                                                                                              as
                                                                                              libc::c_int
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    '\u{0}'
                                                                                        as
                                                                                        i32
                                                                                        as
                                                                                        libc::c_char;
                                                                                dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                        as
                                                                                                                                        *mut DynBuf,
                                                                                                                                    fmtbuf.as_mut_ptr(),
                                                                                                                                    int64_arg);
                                                                                continue
                                                                                    ;
                                                                            }
                                                                        } else {
                                                                            dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                    as
                                                                                                                                    *mut DynBuf,
                                                                                                                                fmtbuf.as_mut_ptr(),
                                                                                                                                int64_arg
                                                                                                                                    as
                                                                                                                                    libc::c_int);
                                                                            continue
                                                                                ;
                                                                        }
                                                                        current_block
                                                                            =
                                                                            531654916993384523;
                                                                    }
                                                                }
                                                                18336436617056464348
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh18 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        string_arg
                                                                            =
                                                                            JS_ToCString(ctx,
                                                                                         *argv.offset(fresh18
                                                                                                          as
                                                                                                          isize));
                                                                        if string_arg.is_null()
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                as
                                                                                                                                *mut DynBuf,
                                                                                                                            fmtbuf.as_mut_ptr(),
                                                                                                                            string_arg);
                                                                        JS_FreeCString(ctx,
                                                                                       string_arg);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                                13214951420223402289
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh19 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        if JS_ToFloat64(ctx,
                                                                                        &mut double_arg,
                                                                                        *argv.offset(fresh19
                                                                                                         as
                                                                                                         isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                as
                                                                                                                                *mut DynBuf,
                                                                                                                            fmtbuf.as_mut_ptr(),
                                                                                                                            double_arg);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                                _ => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        if JS_IsString(*argv.offset(i
                                                                                                        as
                                                                                                        isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            let fresh14 =
                                                                                i;
                                                                            i
                                                                                =
                                                                                i
                                                                                    +
                                                                                    1;
                                                                            string_arg
                                                                                =
                                                                                JS_ToCString(ctx,
                                                                                             *argv.offset(fresh14
                                                                                                              as
                                                                                                              isize));
                                                                            if string_arg.is_null()
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    3554158773753863382;
                                                                                break
                                                                                    ;
                                                                            }
                                                                            int32_arg
                                                                                =
                                                                                unicode_from_utf8(string_arg
                                                                                                      as
                                                                                                      *mut uint8_t,
                                                                                                  6
                                                                                                      as
                                                                                                      libc::c_int,
                                                                                                  &mut p);
                                                                            JS_FreeCString(ctx,
                                                                                           string_arg);
                                                                        } else {
                                                                            let fresh15 =
                                                                                i;
                                                                            i
                                                                                =
                                                                                i
                                                                                    +
                                                                                    1;
                                                                            if JS_ToInt32(ctx,
                                                                                          &mut int32_arg,
                                                                                          *argv.offset(fresh15
                                                                                                           as
                                                                                                           isize))
                                                                                   !=
                                                                                   0
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    3554158773753863382;
                                                                                break
                                                                                    ;
                                                                            }
                                                                        }
                                                                        if int32_arg
                                                                               as
                                                                               libc::c_uint
                                                                               >
                                                                               0x10ffff
                                                                                   as
                                                                                   libc::c_int
                                                                                   as
                                                                                   libc::c_uint
                                                                           {
                                                                            int32_arg
                                                                                =
                                                                                0xfffd
                                                                                    as
                                                                                    libc::c_int
                                                                        }
                                                                        len =
                                                                            unicode_to_utf8(cbuf.as_mut_ptr(),
                                                                                            int32_arg
                                                                                                as
                                                                                                libc::c_uint);
                                                                        dbuf_put(&mut dbuf,
                                                                                 cbuf.as_mut_ptr(),
                                                                                 len
                                                                                     as
                                                                                     size_t);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        115 => {
                                                            current_block =
                                                                18336436617056464348;
                                                            match current_block
                                                                {
                                                                14995964958133890621
                                                                => {
                                                                    dbuf_putc(&mut dbuf,
                                                                              '%'
                                                                                  as
                                                                                  i32
                                                                                  as
                                                                                  uint8_t);
                                                                    continue ;
                                                                }
                                                                1744868089722402712
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh16 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        if JS_ToInt64Ext(ctx,
                                                                                         &mut int64_arg,
                                                                                         *argv.offset(fresh16
                                                                                                          as
                                                                                                          isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        if mod_0
                                                                               ==
                                                                               'l'
                                                                                   as
                                                                                   i32
                                                                           {
                                                                            if !(q
                                                                                     >=
                                                                                     fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                                                                    as
                                                                                                                    libc::c_ulong
                                                                                                                    as
                                                                                                                    isize).offset(-(2
                                                                                                                                        as
                                                                                                                                        libc::c_int
                                                                                                                                        as
                                                                                                                                        isize)))
                                                                               {
                                                                                *q.offset(1
                                                                                              as
                                                                                              libc::c_int
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    *q.offset(-(1
                                                                                                    as
                                                                                                    libc::c_int)
                                                                                                  as
                                                                                                  isize);
                                                                                let ref mut fresh17 =
                                                                                    *q.offset(0
                                                                                                  as
                                                                                                  libc::c_int
                                                                                                  as
                                                                                                  isize);
                                                                                *fresh17
                                                                                    =
                                                                                    'l'
                                                                                        as
                                                                                        i32
                                                                                        as
                                                                                        libc::c_char;
                                                                                *q.offset(-(1
                                                                                                as
                                                                                                libc::c_int)
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    *fresh17;
                                                                                *q.offset(2
                                                                                              as
                                                                                              libc::c_int
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    '\u{0}'
                                                                                        as
                                                                                        i32
                                                                                        as
                                                                                        libc::c_char;
                                                                                dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                        as
                                                                                                                                        *mut DynBuf,
                                                                                                                                    fmtbuf.as_mut_ptr(),
                                                                                                                                    int64_arg);
                                                                                continue
                                                                                    ;
                                                                            }
                                                                        } else {
                                                                            dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                    as
                                                                                                                                    *mut DynBuf,
                                                                                                                                fmtbuf.as_mut_ptr(),
                                                                                                                                int64_arg
                                                                                                                                    as
                                                                                                                                    libc::c_int);
                                                                            continue
                                                                                ;
                                                                        }
                                                                        current_block
                                                                            =
                                                                            531654916993384523;
                                                                    }
                                                                }
                                                                18336436617056464348
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh18 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        string_arg
                                                                            =
                                                                            JS_ToCString(ctx,
                                                                                         *argv.offset(fresh18
                                                                                                          as
                                                                                                          isize));
                                                                        if string_arg.is_null()
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                as
                                                                                                                                *mut DynBuf,
                                                                                                                            fmtbuf.as_mut_ptr(),
                                                                                                                            string_arg);
                                                                        JS_FreeCString(ctx,
                                                                                       string_arg);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                                13214951420223402289
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh19 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        if JS_ToFloat64(ctx,
                                                                                        &mut double_arg,
                                                                                        *argv.offset(fresh19
                                                                                                         as
                                                                                                         isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                as
                                                                                                                                *mut DynBuf,
                                                                                                                            fmtbuf.as_mut_ptr(),
                                                                                                                            double_arg);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                                _ => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        if JS_IsString(*argv.offset(i
                                                                                                        as
                                                                                                        isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            let fresh14 =
                                                                                i;
                                                                            i
                                                                                =
                                                                                i
                                                                                    +
                                                                                    1;
                                                                            string_arg
                                                                                =
                                                                                JS_ToCString(ctx,
                                                                                             *argv.offset(fresh14
                                                                                                              as
                                                                                                              isize));
                                                                            if string_arg.is_null()
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    3554158773753863382;
                                                                                break
                                                                                    ;
                                                                            }
                                                                            int32_arg
                                                                                =
                                                                                unicode_from_utf8(string_arg
                                                                                                      as
                                                                                                      *mut uint8_t,
                                                                                                  6
                                                                                                      as
                                                                                                      libc::c_int,
                                                                                                  &mut p);
                                                                            JS_FreeCString(ctx,
                                                                                           string_arg);
                                                                        } else {
                                                                            let fresh15 =
                                                                                i;
                                                                            i
                                                                                =
                                                                                i
                                                                                    +
                                                                                    1;
                                                                            if JS_ToInt32(ctx,
                                                                                          &mut int32_arg,
                                                                                          *argv.offset(fresh15
                                                                                                           as
                                                                                                           isize))
                                                                                   !=
                                                                                   0
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    3554158773753863382;
                                                                                break
                                                                                    ;
                                                                            }
                                                                        }
                                                                        if int32_arg
                                                                               as
                                                                               libc::c_uint
                                                                               >
                                                                               0x10ffff
                                                                                   as
                                                                                   libc::c_int
                                                                                   as
                                                                                   libc::c_uint
                                                                           {
                                                                            int32_arg
                                                                                =
                                                                                0xfffd
                                                                                    as
                                                                                    libc::c_int
                                                                        }
                                                                        len =
                                                                            unicode_to_utf8(cbuf.as_mut_ptr(),
                                                                                            int32_arg
                                                                                                as
                                                                                                libc::c_uint);
                                                                        dbuf_put(&mut dbuf,
                                                                                 cbuf.as_mut_ptr(),
                                                                                 len
                                                                                     as
                                                                                     size_t);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        101 | 102 | 103 | 97 |
                                                        69 | 70 | 71 | 65 => {
                                                            current_block =
                                                                13214951420223402289;
                                                            match current_block
                                                                {
                                                                14995964958133890621
                                                                => {
                                                                    dbuf_putc(&mut dbuf,
                                                                              '%'
                                                                                  as
                                                                                  i32
                                                                                  as
                                                                                  uint8_t);
                                                                    continue ;
                                                                }
                                                                1744868089722402712
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh16 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        if JS_ToInt64Ext(ctx,
                                                                                         &mut int64_arg,
                                                                                         *argv.offset(fresh16
                                                                                                          as
                                                                                                          isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        if mod_0
                                                                               ==
                                                                               'l'
                                                                                   as
                                                                                   i32
                                                                           {
                                                                            if !(q
                                                                                     >=
                                                                                     fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                                                                    as
                                                                                                                    libc::c_ulong
                                                                                                                    as
                                                                                                                    isize).offset(-(2
                                                                                                                                        as
                                                                                                                                        libc::c_int
                                                                                                                                        as
                                                                                                                                        isize)))
                                                                               {
                                                                                *q.offset(1
                                                                                              as
                                                                                              libc::c_int
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    *q.offset(-(1
                                                                                                    as
                                                                                                    libc::c_int)
                                                                                                  as
                                                                                                  isize);
                                                                                let ref mut fresh17 =
                                                                                    *q.offset(0
                                                                                                  as
                                                                                                  libc::c_int
                                                                                                  as
                                                                                                  isize);
                                                                                *fresh17
                                                                                    =
                                                                                    'l'
                                                                                        as
                                                                                        i32
                                                                                        as
                                                                                        libc::c_char;
                                                                                *q.offset(-(1
                                                                                                as
                                                                                                libc::c_int)
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    *fresh17;
                                                                                *q.offset(2
                                                                                              as
                                                                                              libc::c_int
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    '\u{0}'
                                                                                        as
                                                                                        i32
                                                                                        as
                                                                                        libc::c_char;
                                                                                dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                        as
                                                                                                                                        *mut DynBuf,
                                                                                                                                    fmtbuf.as_mut_ptr(),
                                                                                                                                    int64_arg);
                                                                                continue
                                                                                    ;
                                                                            }
                                                                        } else {
                                                                            dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                    as
                                                                                                                                    *mut DynBuf,
                                                                                                                                fmtbuf.as_mut_ptr(),
                                                                                                                                int64_arg
                                                                                                                                    as
                                                                                                                                    libc::c_int);
                                                                            continue
                                                                                ;
                                                                        }
                                                                        current_block
                                                                            =
                                                                            531654916993384523;
                                                                    }
                                                                }
                                                                18336436617056464348
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh18 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        string_arg
                                                                            =
                                                                            JS_ToCString(ctx,
                                                                                         *argv.offset(fresh18
                                                                                                          as
                                                                                                          isize));
                                                                        if string_arg.is_null()
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                as
                                                                                                                                *mut DynBuf,
                                                                                                                            fmtbuf.as_mut_ptr(),
                                                                                                                            string_arg);
                                                                        JS_FreeCString(ctx,
                                                                                       string_arg);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                                13214951420223402289
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh19 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        if JS_ToFloat64(ctx,
                                                                                        &mut double_arg,
                                                                                        *argv.offset(fresh19
                                                                                                         as
                                                                                                         isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                as
                                                                                                                                *mut DynBuf,
                                                                                                                            fmtbuf.as_mut_ptr(),
                                                                                                                            double_arg);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                                _ => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        if JS_IsString(*argv.offset(i
                                                                                                        as
                                                                                                        isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            let fresh14 =
                                                                                i;
                                                                            i
                                                                                =
                                                                                i
                                                                                    +
                                                                                    1;
                                                                            string_arg
                                                                                =
                                                                                JS_ToCString(ctx,
                                                                                             *argv.offset(fresh14
                                                                                                              as
                                                                                                              isize));
                                                                            if string_arg.is_null()
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    3554158773753863382;
                                                                                break
                                                                                    ;
                                                                            }
                                                                            int32_arg
                                                                                =
                                                                                unicode_from_utf8(string_arg
                                                                                                      as
                                                                                                      *mut uint8_t,
                                                                                                  6
                                                                                                      as
                                                                                                      libc::c_int,
                                                                                                  &mut p);
                                                                            JS_FreeCString(ctx,
                                                                                           string_arg);
                                                                        } else {
                                                                            let fresh15 =
                                                                                i;
                                                                            i
                                                                                =
                                                                                i
                                                                                    +
                                                                                    1;
                                                                            if JS_ToInt32(ctx,
                                                                                          &mut int32_arg,
                                                                                          *argv.offset(fresh15
                                                                                                           as
                                                                                                           isize))
                                                                                   !=
                                                                                   0
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    3554158773753863382;
                                                                                break
                                                                                    ;
                                                                            }
                                                                        }
                                                                        if int32_arg
                                                                               as
                                                                               libc::c_uint
                                                                               >
                                                                               0x10ffff
                                                                                   as
                                                                                   libc::c_int
                                                                                   as
                                                                                   libc::c_uint
                                                                           {
                                                                            int32_arg
                                                                                =
                                                                                0xfffd
                                                                                    as
                                                                                    libc::c_int
                                                                        }
                                                                        len =
                                                                            unicode_to_utf8(cbuf.as_mut_ptr(),
                                                                                            int32_arg
                                                                                                as
                                                                                                libc::c_uint);
                                                                        dbuf_put(&mut dbuf,
                                                                                 cbuf.as_mut_ptr(),
                                                                                 len
                                                                                     as
                                                                                     size_t);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        37 => {
                                                            current_block =
                                                                14995964958133890621;
                                                            match current_block
                                                                {
                                                                14995964958133890621
                                                                => {
                                                                    dbuf_putc(&mut dbuf,
                                                                              '%'
                                                                                  as
                                                                                  i32
                                                                                  as
                                                                                  uint8_t);
                                                                    continue ;
                                                                }
                                                                1744868089722402712
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh16 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        if JS_ToInt64Ext(ctx,
                                                                                         &mut int64_arg,
                                                                                         *argv.offset(fresh16
                                                                                                          as
                                                                                                          isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        if mod_0
                                                                               ==
                                                                               'l'
                                                                                   as
                                                                                   i32
                                                                           {
                                                                            if !(q
                                                                                     >=
                                                                                     fmtbuf.as_mut_ptr().offset(::std::mem::size_of::<[libc::c_char; 32]>()
                                                                                                                    as
                                                                                                                    libc::c_ulong
                                                                                                                    as
                                                                                                                    isize).offset(-(2
                                                                                                                                        as
                                                                                                                                        libc::c_int
                                                                                                                                        as
                                                                                                                                        isize)))
                                                                               {
                                                                                *q.offset(1
                                                                                              as
                                                                                              libc::c_int
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    *q.offset(-(1
                                                                                                    as
                                                                                                    libc::c_int)
                                                                                                  as
                                                                                                  isize);
                                                                                let ref mut fresh17 =
                                                                                    *q.offset(0
                                                                                                  as
                                                                                                  libc::c_int
                                                                                                  as
                                                                                                  isize);
                                                                                *fresh17
                                                                                    =
                                                                                    'l'
                                                                                        as
                                                                                        i32
                                                                                        as
                                                                                        libc::c_char;
                                                                                *q.offset(-(1
                                                                                                as
                                                                                                libc::c_int)
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    *fresh17;
                                                                                *q.offset(2
                                                                                              as
                                                                                              libc::c_int
                                                                                              as
                                                                                              isize)
                                                                                    =
                                                                                    '\u{0}'
                                                                                        as
                                                                                        i32
                                                                                        as
                                                                                        libc::c_char;
                                                                                dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                        as
                                                                                                                                        *mut DynBuf,
                                                                                                                                    fmtbuf.as_mut_ptr(),
                                                                                                                                    int64_arg);
                                                                                continue
                                                                                    ;
                                                                            }
                                                                        } else {
                                                                            dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                    as
                                                                                                                                    *mut DynBuf,
                                                                                                                                fmtbuf.as_mut_ptr(),
                                                                                                                                int64_arg
                                                                                                                                    as
                                                                                                                                    libc::c_int);
                                                                            continue
                                                                                ;
                                                                        }
                                                                        current_block
                                                                            =
                                                                            531654916993384523;
                                                                    }
                                                                }
                                                                18336436617056464348
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh18 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        string_arg
                                                                            =
                                                                            JS_ToCString(ctx,
                                                                                         *argv.offset(fresh18
                                                                                                          as
                                                                                                          isize));
                                                                        if string_arg.is_null()
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                as
                                                                                                                                *mut DynBuf,
                                                                                                                            fmtbuf.as_mut_ptr(),
                                                                                                                            string_arg);
                                                                        JS_FreeCString(ctx,
                                                                                       string_arg);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                                13214951420223402289
                                                                => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        let fresh19 =
                                                                            i;
                                                                        i =
                                                                            i
                                                                                +
                                                                                1;
                                                                        if JS_ToFloat64(ctx,
                                                                                        &mut double_arg,
                                                                                        *argv.offset(fresh19
                                                                                                         as
                                                                                                         isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                3554158773753863382;
                                                                            break
                                                                                ;
                                                                        }
                                                                        dbuf_printf_fun.expect("non-null function pointer")(&mut dbuf
                                                                                                                                as
                                                                                                                                *mut DynBuf,
                                                                                                                            fmtbuf.as_mut_ptr(),
                                                                                                                            double_arg);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                                _ => {
                                                                    if i >=
                                                                           argc
                                                                       {
                                                                        current_block
                                                                            =
                                                                            11462318409609933101;
                                                                    } else {
                                                                        if JS_IsString(*argv.offset(i
                                                                                                        as
                                                                                                        isize))
                                                                               !=
                                                                               0
                                                                           {
                                                                            let fresh14 =
                                                                                i;
                                                                            i
                                                                                =
                                                                                i
                                                                                    +
                                                                                    1;
                                                                            string_arg
                                                                                =
                                                                                JS_ToCString(ctx,
                                                                                             *argv.offset(fresh14
                                                                                                              as
                                                                                                              isize));
                                                                            if string_arg.is_null()
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    3554158773753863382;
                                                                                break
                                                                                    ;
                                                                            }
                                                                            int32_arg
                                                                                =
                                                                                unicode_from_utf8(string_arg
                                                                                                      as
                                                                                                      *mut uint8_t,
                                                                                                  6
                                                                                                      as
                                                                                                      libc::c_int,
                                                                                                  &mut p);
                                                                            JS_FreeCString(ctx,
                                                                                           string_arg);
                                                                        } else {
                                                                            let fresh15 =
                                                                                i;
                                                                            i
                                                                                =
                                                                                i
                                                                                    +
                                                                                    1;
                                                                            if JS_ToInt32(ctx,
                                                                                          &mut int32_arg,
                                                                                          *argv.offset(fresh15
                                                                                                           as
                                                                                                           isize))
                                                                                   !=
                                                                                   0
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    3554158773753863382;
                                                                                break
                                                                                    ;
                                                                            }
                                                                        }
                                                                        if int32_arg
                                                                               as
                                                                               libc::c_uint
                                                                               >
                                                                               0x10ffff
                                                                                   as
                                                                                   libc::c_int
                                                                                   as
                                                                                   libc::c_uint
                                                                           {
                                                                            int32_arg
                                                                                =
                                                                                0xfffd
                                                                                    as
                                                                                    libc::c_int
                                                                        }
                                                                        len =
                                                                            unicode_to_utf8(cbuf.as_mut_ptr(),
                                                                                            int32_arg
                                                                                                as
                                                                                                libc::c_uint);
                                                                        dbuf_put(&mut dbuf,
                                                                                 cbuf.as_mut_ptr(),
                                                                                 len
                                                                                     as
                                                                                     size_t);
                                                                        continue
                                                                            ;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        _ => {
                                                            current_block =
                                                                531654916993384523;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    _ => { }
                                }
                                match current_block {
                                    531654916993384523 => { }
                                    _ => {
                                        JS_ThrowReferenceError(ctx,
                                                               b"missing argument for conversion specifier\x00"
                                                                   as
                                                                   *const u8
                                                                   as
                                                                   *const libc::c_char);
                                        current_block = 3554158773753863382;
                                        break ;
                                    }
                                }
                            }
                        }
                    }
                    _ => { }
                }
                /* XXX: should support an extension mechanism */
                JS_ThrowTypeError(ctx,
                                  b"invalid conversion specifier in format string\x00"
                                      as *const u8 as *const libc::c_char);
                current_block = 3554158773753863382;
                break ;
            }
            match current_block {
                3554158773753863382 => { }
                _ => {
                    JS_FreeCString(ctx, fmt_str);
                    current_block = 16593409533420678784;
                }
            }
        }
        match current_block {
            16593409533420678784 => { }
            _ => {
                dbuf_free(&mut dbuf);
                return {
                           let mut init =
                               JSValue{u:
                                           JSValueUnion{int32:
                                                            0 as
                                                                libc::c_int,},
                                       tag:
                                           JS_TAG_EXCEPTION as libc::c_int as
                                               int64_t,};
                           init
                       }
            }
        }
    }
    if dbuf.error != 0 {
        res = JS_ThrowOutOfMemory(ctx)
    } else if !fp.is_null() {
        len =
            fwrite(dbuf.buf as *const libc::c_void,
                   1 as libc::c_int as libc::c_ulong, dbuf.size, fp) as
                libc::c_int;
        res = JS_NewInt32(ctx, len)
    } else {
        res = JS_NewStringLen(ctx, dbuf.buf as *mut libc::c_char, dbuf.size)
    }
    dbuf_free(&mut dbuf);
    return res;
}
#[no_mangle]
pub unsafe extern "C" fn js_load_file(mut ctx: *mut JSContext,
                                      mut pbuf_len: *mut size_t,
                                      mut filename: *const libc::c_char)
 -> *mut uint8_t {
    let mut f: *mut FILE = 0 as *mut FILE;
    let mut buf: *mut uint8_t = 0 as *mut uint8_t;
    let mut buf_len: size_t = 0;
    let mut lret: libc::c_long = 0;
    f = fopen(filename, b"rb\x00" as *const u8 as *const libc::c_char);
    if f.is_null() { return 0 as *mut uint8_t }
    if !(fseek(f, 0 as libc::c_int as libc::c_long, 2 as libc::c_int) <
             0 as libc::c_int) {
        lret = ftell(f);
        if !(lret < 0 as libc::c_int as libc::c_long) {
            /* XXX: on Linux, ftell() return LONG_MAX for directories */
            if lret == 9223372036854775807 as libc::c_long {
                *__error() = 21 as libc::c_int
            } else {
                buf_len = lret as size_t;
                if !(fseek(f, 0 as libc::c_int as libc::c_long,
                           0 as libc::c_int) < 0 as libc::c_int) {
                    if !ctx.is_null() {
                        buf =
                            js_malloc(ctx,
                                      buf_len.wrapping_add(1 as libc::c_int as
                                                               libc::c_ulong))
                                as *mut uint8_t
                    } else {
                        buf =
                            malloc(buf_len.wrapping_add(1 as libc::c_int as
                                                            libc::c_ulong)) as
                                *mut uint8_t
                    }
                    if !buf.is_null() {
                        if fread(buf as *mut libc::c_void,
                                 1 as libc::c_int as libc::c_ulong, buf_len,
                                 f) != buf_len {
                            *__error() = 5 as libc::c_int;
                            if !ctx.is_null() {
                                js_free(ctx, buf as *mut libc::c_void);
                            } else { free(buf as *mut libc::c_void); }
                        } else {
                            *buf.offset(buf_len as isize) =
                                '\u{0}' as i32 as uint8_t;
                            fclose(f);
                            *pbuf_len = buf_len;
                            return buf
                        }
                    }
                }
            }
        }
    }
    fclose(f);
    return 0 as *mut uint8_t;
}
/* load and evaluate a file */
unsafe extern "C" fn js_loadScript(mut ctx: *mut JSContext,
                                   mut this_val: JSValue,
                                   mut argc: libc::c_int,
                                   mut argv: *mut JSValue) -> JSValue {
    let mut buf: *mut uint8_t = 0 as *mut uint8_t;
    let mut filename: *const libc::c_char = 0 as *const libc::c_char;
    let mut ret: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut buf_len: size_t = 0;
    filename = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if filename.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    buf = js_load_file(ctx, &mut buf_len, filename);
    if buf.is_null() {
        JS_ThrowReferenceError(ctx,
                               b"could not load \'%s\'\x00" as *const u8 as
                                   *const libc::c_char, filename);
        JS_FreeCString(ctx, filename);
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret =
        JS_Eval(ctx, buf as *mut libc::c_char, buf_len, filename,
                (0 as libc::c_int) << 0 as libc::c_int);
    js_free(ctx, buf as *mut libc::c_void);
    JS_FreeCString(ctx, filename);
    return ret;
}
/* load a file as a UTF-8 encoded string */
unsafe extern "C" fn js_std_loadFile(mut ctx: *mut JSContext,
                                     mut this_val: JSValue,
                                     mut argc: libc::c_int,
                                     mut argv: *mut JSValue) -> JSValue {
    let mut buf: *mut uint8_t = 0 as *mut uint8_t;
    let mut filename: *const libc::c_char = 0 as *const libc::c_char;
    let mut ret: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut buf_len: size_t = 0;
    filename = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if filename.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    buf = js_load_file(ctx, &mut buf_len, filename);
    JS_FreeCString(ctx, filename);
    if buf.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag: JS_TAG_NULL as libc::c_int as int64_t,};
                   init
               }
    }
    ret = JS_NewStringLen(ctx, buf as *mut libc::c_char, buf_len);
    js_free(ctx, buf as *mut libc::c_void);
    return ret;
}
unsafe extern "C" fn js_module_loader_so(mut ctx: *mut JSContext,
                                         mut module_name: *const libc::c_char)
 -> *mut JSModuleDef {
    let mut m: *mut JSModuleDef = 0 as *mut JSModuleDef;
    let mut hd: *mut libc::c_void = 0 as *mut libc::c_void;
    let mut init: Option<JSInitModuleFunc> = None;
    let mut filename: *mut libc::c_char = 0 as *mut libc::c_char;
    if strchr(module_name, '/' as i32).is_null() {
        /* must add a '/' so that the DLL is not searched in the
           system library paths */
        filename =
            js_malloc(ctx,
                      strlen(module_name).wrapping_add(2 as libc::c_int as
                                                           libc::c_ulong).wrapping_add(1
                                                                                           as
                                                                                           libc::c_int
                                                                                           as
                                                                                           libc::c_ulong))
                as *mut libc::c_char;
        if filename.is_null() { return 0 as *mut JSModuleDef }
        strcpy(filename, b"./\x00" as *const u8 as *const libc::c_char);
        strcpy(filename.offset(2 as libc::c_int as isize), module_name);
    } else { filename = module_name as *mut libc::c_char }
    /* C module */
    hd = dlopen(filename, 0x2 as libc::c_int | 0x4 as libc::c_int);
    if filename != module_name as *mut libc::c_char {
        js_free(ctx, filename as *mut libc::c_void);
    }
    if hd.is_null() {
        JS_ThrowReferenceError(ctx,
                               b"could not load module filename \'%s\' as shared library\x00"
                                   as *const u8 as *const libc::c_char,
                               module_name);
    } else {
        init =
            ::std::mem::transmute::<*mut libc::c_void,
                                    Option<JSInitModuleFunc>>(dlsym(hd,
                                                                    b"js_init_module\x00"
                                                                        as
                                                                        *const u8
                                                                        as
                                                                        *const libc::c_char));
        if init.is_none() {
            JS_ThrowReferenceError(ctx,
                                   b"could not load module filename \'%s\': js_init_module not found\x00"
                                       as *const u8 as *const libc::c_char,
                                   module_name);
        } else {
            m = init.expect("non-null function pointer")(ctx, module_name);
            if m.is_null() {
                JS_ThrowReferenceError(ctx,
                                       b"could not load module filename \'%s\': initialization error\x00"
                                           as *const u8 as
                                           *const libc::c_char, module_name);
            } else { return m }
        }
    }
    if !hd.is_null() { dlclose(hd); }
    return 0 as *mut JSModuleDef;
}
/* !_WIN32 */
#[no_mangle]
pub unsafe extern "C" fn js_module_set_import_meta(mut ctx: *mut JSContext,
                                                   mut func_val: JSValue,
                                                   mut use_realpath:
                                                       libc::c_int,
                                                   mut is_main: libc::c_int)
 -> libc::c_int {
    let mut m: *mut JSModuleDef = 0 as *mut JSModuleDef;
    let mut buf: [libc::c_char; 1040] = [0; 1040];
    let mut meta_obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut module_name_atom: JSAtom = 0;
    let mut module_name: *const libc::c_char = 0 as *const libc::c_char;
    if !(func_val.tag as int32_t == JS_TAG_MODULE as libc::c_int) as
           libc::c_int as libc::c_long != 0 {
        __assert_rtn((*::std::mem::transmute::<&[u8; 26],
                                               &[libc::c_char; 26]>(b"js_module_set_import_meta\x00")).as_ptr(),
                     b"/Users/lemonhx/Desktop/Cpp/AcidJS/src/libc/quickjs-libc.c\x00"
                         as *const u8 as *const libc::c_char,
                     571 as libc::c_int,
                     b"JS_VALUE_GET_TAG(func_val) == JS_TAG_MODULE\x00" as
                         *const u8 as *const libc::c_char);
    } else { };
    m = func_val.u.ptr as *mut JSModuleDef;
    module_name_atom = JS_GetModuleName(ctx, m);
    module_name = JS_AtomToCString(ctx, module_name_atom);
    JS_FreeAtom(ctx, module_name_atom);
    if module_name.is_null() { return -(1 as libc::c_int) }
    if strchr(module_name, ':' as i32).is_null() {
        strcpy(buf.as_mut_ptr(),
               b"file://\x00" as *const u8 as *const libc::c_char);
        /* realpath() cannot be used with modules compiled with qjsc
           because the corresponding module source code is not
           necessarily present */
        if use_realpath != 0 {
            let mut res: *mut libc::c_char =
                realpath(module_name,
                         buf.as_mut_ptr().offset(strlen(buf.as_mut_ptr()) as
                                                     isize));
            if res.is_null() {
                JS_ThrowTypeError(ctx,
                                  b"realpath failure\x00" as *const u8 as
                                      *const libc::c_char);
                JS_FreeCString(ctx, module_name);
                return -(1 as libc::c_int)
            }
        } else {
            pstrcat(buf.as_mut_ptr(),
                    ::std::mem::size_of::<[libc::c_char; 1040]>() as
                        libc::c_ulong as libc::c_int, module_name);
        }
    } else {
        pstrcpy(buf.as_mut_ptr(),
                ::std::mem::size_of::<[libc::c_char; 1040]>() as libc::c_ulong
                    as libc::c_int, module_name);
    }
    JS_FreeCString(ctx, module_name);
    meta_obj = JS_GetImportMeta(ctx, m);
    if JS_IsException(meta_obj) != 0 { return -(1 as libc::c_int) }
    JS_DefinePropertyValueStr(ctx, meta_obj,
                              b"url\x00" as *const u8 as *const libc::c_char,
                              JS_NewString(ctx, buf.as_mut_ptr()),
                              (1 as libc::c_int) << 0 as libc::c_int |
                                  (1 as libc::c_int) << 1 as libc::c_int |
                                  (1 as libc::c_int) << 2 as libc::c_int);
    JS_DefinePropertyValueStr(ctx, meta_obj,
                              b"main\x00" as *const u8 as *const libc::c_char,
                              JS_NewBool(ctx, is_main),
                              (1 as libc::c_int) << 0 as libc::c_int |
                                  (1 as libc::c_int) << 1 as libc::c_int |
                                  (1 as libc::c_int) << 2 as libc::c_int);
    JS_FreeValue(ctx, meta_obj);
    return 0 as libc::c_int;
}
#[no_mangle]
pub unsafe extern "C" fn js_module_loader(mut ctx: *mut JSContext,
                                          mut module_name:
                                              *const libc::c_char,
                                          mut opaque: *mut libc::c_void)
 -> *mut JSModuleDef {
    let mut m: *mut JSModuleDef = 0 as *mut JSModuleDef;
    if has_suffix(module_name, b".so\x00" as *const u8 as *const libc::c_char)
           != 0 {
        m = js_module_loader_so(ctx, module_name)
    } else {
        let mut buf_len: size_t = 0;
        let mut buf: *mut uint8_t = 0 as *mut uint8_t;
        let mut func_val: JSValue =
            JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
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
        /* XXX: could propagate the exception */
        js_module_set_import_meta(ctx, func_val, TRUE as libc::c_int,
                                  FALSE as libc::c_int);
        /* the module is already referenced, so we must free it */
        m = func_val.u.ptr as *mut JSModuleDef;
        JS_FreeValue(ctx, func_val);
    }
    return m;
}
unsafe extern "C" fn js_std_exit(mut ctx: *mut JSContext,
                                 mut this_val: JSValue, mut argc: libc::c_int,
                                 mut argv: *mut JSValue) -> JSValue {
    let mut status: libc::c_int = 0;
    if JS_ToInt32(ctx, &mut status, *argv.offset(0 as libc::c_int as isize))
           != 0 {
        status = -(1 as libc::c_int)
    }
    exit(status);
}
unsafe extern "C" fn js_std_getenv(mut ctx: *mut JSContext,
                                   mut this_val: JSValue,
                                   mut argc: libc::c_int,
                                   mut argv: *mut JSValue) -> JSValue {
    let mut name: *const libc::c_char = 0 as *const libc::c_char;
    let mut str: *const libc::c_char = 0 as *const libc::c_char;
    name = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if name.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    str = getenv(name);
    JS_FreeCString(ctx, name);
    if str.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_UNDEFINED as libc::c_int as
                                       int64_t,};
                   init
               }
    } else { return JS_NewString(ctx, str) };
}
unsafe extern "C" fn js_std_gc(mut ctx: *mut JSContext, mut this_val: JSValue,
                               mut argc: libc::c_int, mut argv: *mut JSValue)
 -> JSValue {
    JS_RunGC(JS_GetRuntime(ctx));
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
               init
           };
}
unsafe extern "C" fn interrupt_handler(mut rt: *mut JSRuntime,
                                       mut opaque: *mut libc::c_void)
 -> libc::c_int {
    return (os_pending_signals >> 2 as libc::c_int &
                1 as libc::c_int as libc::c_ulonglong) as libc::c_int;
}
unsafe extern "C" fn get_bool_option(mut ctx: *mut JSContext,
                                     mut pbool: *mut libc::c_int,
                                     mut obj: JSValue,
                                     mut option: *const libc::c_char)
 -> libc::c_int {
    let mut val: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    val = JS_GetPropertyStr(ctx, obj, option);
    if JS_IsException(val) != 0 { return -(1 as libc::c_int) }
    if JS_IsUndefined(val) == 0 { *pbool = JS_ToBool(ctx, val) }
    JS_FreeValue(ctx, val);
    return 0 as libc::c_int;
}
unsafe extern "C" fn js_evalScript(mut ctx: *mut JSContext,
                                   mut this_val: JSValue,
                                   mut argc: libc::c_int,
                                   mut argv: *mut JSValue) -> JSValue {
    let mut rt: *mut JSRuntime = JS_GetRuntime(ctx);
    let mut ts: *mut JSThreadState =
        JS_GetRuntimeOpaque(rt) as *mut JSThreadState;
    let mut str: *const libc::c_char = 0 as *const libc::c_char;
    let mut len: size_t = 0;
    let mut ret: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut options_obj: JSValue =
        JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut backtrace_barrier: libc::c_int = FALSE as libc::c_int;
    let mut flags: libc::c_int = 0;
    if argc >= 2 as libc::c_int {
        options_obj = *argv.offset(1 as libc::c_int as isize);
        if get_bool_option(ctx, &mut backtrace_barrier, options_obj,
                           b"backtrace_barrier\x00" as *const u8 as
                               *const libc::c_char) != 0 {
            return {
                       let mut init =
                           JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                   tag:
                                       JS_TAG_EXCEPTION as libc::c_int as
                                           int64_t,};
                       init
                   }
        }
    }
    str =
        JS_ToCStringLen(ctx, &mut len,
                        *argv.offset(0 as libc::c_int as isize));
    if str.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if (*ts).recv_pipe.is_null() &&
           {
               (*ts).eval_script_recurse += 1;
               ((*ts).eval_script_recurse) == 1 as libc::c_int
           } {
        /* install the interrupt handler */
        JS_SetInterruptHandler(JS_GetRuntime(ctx),
                               Some(interrupt_handler as
                                        unsafe extern "C" fn(_:
                                                                 *mut JSRuntime,
                                                             _:
                                                                 *mut libc::c_void)
                                            -> libc::c_int),
                               0 as *mut libc::c_void);
    }
    flags = (0 as libc::c_int) << 0 as libc::c_int;
    if backtrace_barrier != 0 {
        flags |= (1 as libc::c_int) << 6 as libc::c_int
    }
    ret =
        JS_Eval(ctx, str, len,
                b"<evalScript>\x00" as *const u8 as *const libc::c_char,
                flags);
    JS_FreeCString(ctx, str);
    if (*ts).recv_pipe.is_null() &&
           {
               (*ts).eval_script_recurse -= 1;
               ((*ts).eval_script_recurse) == 0 as libc::c_int
           } {
        /* remove the interrupt handler */
        JS_SetInterruptHandler(JS_GetRuntime(ctx), None,
                               0 as *mut libc::c_void);
        os_pending_signals &=
            !((1 as libc::c_int as uint64_t) << 2 as libc::c_int);
        /* convert the uncatchable "interrupted" error into a normal error
           so that it can be caught by the REPL */
        if JS_IsException(ret) != 0 { JS_ResetUncatchableError(ctx); }
    }
    return ret;
}
static mut js_std_file_class_id: JSClassID = 0;
unsafe extern "C" fn js_std_file_finalizer(mut rt: *mut JSRuntime,
                                           mut val: JSValue) {
    let mut s: *mut JSSTDFile =
        JS_GetOpaque(val, js_std_file_class_id) as *mut JSSTDFile;
    if !s.is_null() {
        if !(*s).f.is_null() && (*s).close_in_finalizer != 0 {
            if (*s).is_popen != 0 { pclose((*s).f); } else { fclose((*s).f); }
        }
        js_free_rt(rt, s as *mut libc::c_void);
    };
}
unsafe extern "C" fn js_get_errno(mut ret: ssize_t) -> ssize_t {
    if ret == -(1 as libc::c_int) as libc::c_long {
        ret = -*__error() as ssize_t
    }
    return ret;
}
unsafe extern "C" fn js_std_strerror(mut ctx: *mut JSContext,
                                     mut this_val: JSValue,
                                     mut argc: libc::c_int,
                                     mut argv: *mut JSValue) -> JSValue {
    let mut err: libc::c_int = 0;
    if JS_ToInt32(ctx, &mut err, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    return JS_NewString(ctx, strerror(err));
}
unsafe extern "C" fn js_std_parseExtJSON(mut ctx: *mut JSContext,
                                         mut this_val: JSValue,
                                         mut argc: libc::c_int,
                                         mut argv: *mut JSValue) -> JSValue {
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut str: *const libc::c_char = 0 as *const libc::c_char;
    let mut len: size_t = 0;
    str =
        JS_ToCStringLen(ctx, &mut len,
                        *argv.offset(0 as libc::c_int as isize));
    if str.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    obj =
        JS_ParseJSON2(ctx, str, len,
                      b"<input>\x00" as *const u8 as *const libc::c_char,
                      (1 as libc::c_int) << 0 as libc::c_int);
    JS_FreeCString(ctx, str);
    return obj;
}
unsafe extern "C" fn js_new_std_file(mut ctx: *mut JSContext,
                                     mut f: *mut FILE,
                                     mut close_in_finalizer: libc::c_int,
                                     mut is_popen: libc::c_int) -> JSValue {
    let mut s: *mut JSSTDFile = 0 as *mut JSSTDFile;
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    obj = JS_NewObjectClass(ctx, js_std_file_class_id as libc::c_int);
    if JS_IsException(obj) != 0 { return obj }
    s =
        js_mallocz(ctx, ::std::mem::size_of::<JSSTDFile>() as libc::c_ulong)
            as *mut JSSTDFile;
    if s.is_null() {
        JS_FreeValue(ctx, obj);
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    (*s).close_in_finalizer = close_in_finalizer;
    (*s).is_popen = is_popen;
    (*s).f = f;
    JS_SetOpaque(obj, s as *mut libc::c_void);
    return obj;
}
unsafe extern "C" fn js_set_error_object(mut ctx: *mut JSContext,
                                         mut obj: JSValue,
                                         mut err: libc::c_int) {
    if JS_IsUndefined(obj) == 0 {
        JS_SetPropertyStr(ctx, obj,
                          b"errno\x00" as *const u8 as *const libc::c_char,
                          JS_NewInt32(ctx, err));
    };
}
unsafe extern "C" fn js_std_open(mut ctx: *mut JSContext,
                                 mut this_val: JSValue, mut argc: libc::c_int,
                                 mut argv: *mut JSValue) -> JSValue {
    let mut filename: *const libc::c_char = 0 as *const libc::c_char;
    let mut mode: *const libc::c_char = 0 as *const libc::c_char;
    let mut f: *mut FILE = 0 as *mut FILE;
    let mut err: libc::c_int = 0;
    filename = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if !filename.is_null() {
        mode = JS_ToCString(ctx, *argv.offset(1 as libc::c_int as isize));
        if !mode.is_null() {
            if *mode.offset(strspn(mode,
                                   b"rwa+b\x00" as *const u8 as
                                       *const libc::c_char) as isize) as
                   libc::c_int != '\u{0}' as i32 {
                JS_ThrowTypeError(ctx,
                                  b"invalid file mode\x00" as *const u8 as
                                      *const libc::c_char);
            } else {
                f = fopen(filename, mode);
                if f.is_null() {
                    err = *__error()
                } else { err = 0 as libc::c_int }
                if argc >= 3 as libc::c_int {
                    js_set_error_object(ctx,
                                        *argv.offset(2 as libc::c_int as
                                                         isize), err);
                }
                JS_FreeCString(ctx, filename);
                JS_FreeCString(ctx, mode);
                if f.is_null() {
                    return {
                               let mut init =
                                   JSValue{u:
                                               JSValueUnion{int32:
                                                                0 as
                                                                    libc::c_int,},
                                           tag:
                                               JS_TAG_NULL as libc::c_int as
                                                   int64_t,};
                               init
                           }
                }
                return js_new_std_file(ctx, f, TRUE as libc::c_int,
                                       FALSE as libc::c_int)
            }
        }
    }
    JS_FreeCString(ctx, filename);
    JS_FreeCString(ctx, mode);
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_EXCEPTION as libc::c_int as int64_t,};
               init
           };
}
unsafe extern "C" fn js_std_popen(mut ctx: *mut JSContext,
                                  mut this_val: JSValue,
                                  mut argc: libc::c_int,
                                  mut argv: *mut JSValue) -> JSValue {
    let mut filename: *const libc::c_char = 0 as *const libc::c_char;
    let mut mode: *const libc::c_char = 0 as *const libc::c_char;
    let mut f: *mut FILE = 0 as *mut FILE;
    let mut err: libc::c_int = 0;
    filename = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if !filename.is_null() {
        mode = JS_ToCString(ctx, *argv.offset(1 as libc::c_int as isize));
        if !mode.is_null() {
            if *mode.offset(strspn(mode,
                                   b"rw\x00" as *const u8 as
                                       *const libc::c_char) as isize) as
                   libc::c_int != '\u{0}' as i32 {
                JS_ThrowTypeError(ctx,
                                  b"invalid file mode\x00" as *const u8 as
                                      *const libc::c_char);
            } else {
                f = popen(filename, mode);
                if f.is_null() {
                    err = *__error()
                } else { err = 0 as libc::c_int }
                if argc >= 3 as libc::c_int {
                    js_set_error_object(ctx,
                                        *argv.offset(2 as libc::c_int as
                                                         isize), err);
                }
                JS_FreeCString(ctx, filename);
                JS_FreeCString(ctx, mode);
                if f.is_null() {
                    return {
                               let mut init =
                                   JSValue{u:
                                               JSValueUnion{int32:
                                                                0 as
                                                                    libc::c_int,},
                                           tag:
                                               JS_TAG_NULL as libc::c_int as
                                                   int64_t,};
                               init
                           }
                }
                return js_new_std_file(ctx, f, TRUE as libc::c_int,
                                       TRUE as libc::c_int)
            }
        }
    }
    JS_FreeCString(ctx, filename);
    JS_FreeCString(ctx, mode);
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_EXCEPTION as libc::c_int as int64_t,};
               init
           };
}
unsafe extern "C" fn js_std_fdopen(mut ctx: *mut JSContext,
                                   mut this_val: JSValue,
                                   mut argc: libc::c_int,
                                   mut argv: *mut JSValue) -> JSValue {
    let mut mode: *const libc::c_char = 0 as *const libc::c_char;
    let mut f: *mut FILE = 0 as *mut FILE;
    let mut fd: libc::c_int = 0;
    let mut err: libc::c_int = 0;
    if JS_ToInt32(ctx, &mut fd, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    mode = JS_ToCString(ctx, *argv.offset(1 as libc::c_int as isize));
    if !mode.is_null() {
        if *mode.offset(strspn(mode,
                               b"rwa+\x00" as *const u8 as
                                   *const libc::c_char) as isize) as
               libc::c_int != '\u{0}' as i32 {
            JS_ThrowTypeError(ctx,
                              b"invalid file mode\x00" as *const u8 as
                                  *const libc::c_char);
        } else {
            f = fdopen(fd, mode);
            if f.is_null() {
                err = *__error()
            } else { err = 0 as libc::c_int }
            if argc >= 3 as libc::c_int {
                js_set_error_object(ctx,
                                    *argv.offset(2 as libc::c_int as isize),
                                    err);
            }
            JS_FreeCString(ctx, mode);
            if f.is_null() {
                return {
                           let mut init =
                               JSValue{u:
                                           JSValueUnion{int32:
                                                            0 as
                                                                libc::c_int,},
                                       tag:
                                           JS_TAG_NULL as libc::c_int as
                                               int64_t,};
                           init
                       }
            }
            return js_new_std_file(ctx, f, TRUE as libc::c_int,
                                   FALSE as libc::c_int)
        }
    }
    JS_FreeCString(ctx, mode);
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_EXCEPTION as libc::c_int as int64_t,};
               init
           };
}
unsafe extern "C" fn js_std_tmpfile(mut ctx: *mut JSContext,
                                    mut this_val: JSValue,
                                    mut argc: libc::c_int,
                                    mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = 0 as *mut FILE;
    f = tmpfile();
    if argc >= 1 as libc::c_int {
        js_set_error_object(ctx, *argv.offset(0 as libc::c_int as isize),
                            if !f.is_null() {
                                0 as libc::c_int
                            } else { *__error() });
    }
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag: JS_TAG_NULL as libc::c_int as int64_t,};
                   init
               }
    }
    return js_new_std_file(ctx, f, TRUE as libc::c_int, FALSE as libc::c_int);
}
unsafe extern "C" fn js_std_sprintf(mut ctx: *mut JSContext,
                                    mut this_val: JSValue,
                                    mut argc: libc::c_int,
                                    mut argv: *mut JSValue) -> JSValue {
    return js_printf_internal(ctx, argc, argv, 0 as *mut FILE);
}
unsafe extern "C" fn js_std_printf(mut ctx: *mut JSContext,
                                   mut this_val: JSValue,
                                   mut argc: libc::c_int,
                                   mut argv: *mut JSValue) -> JSValue {
    return js_printf_internal(ctx, argc, argv, __stdoutp);
}
unsafe extern "C" fn js_std_file_get(mut ctx: *mut JSContext,
                                     mut obj: JSValue) -> *mut FILE {
    let mut s: *mut JSSTDFile =
        JS_GetOpaque2(ctx, obj, js_std_file_class_id) as *mut JSSTDFile;
    if s.is_null() { return 0 as *mut FILE }
    if (*s).f.is_null() {
        JS_ThrowTypeError(ctx,
                          b"invalid file handle\x00" as *const u8 as
                              *const libc::c_char);
        return 0 as *mut FILE
    }
    return (*s).f;
}
unsafe extern "C" fn js_std_file_puts(mut ctx: *mut JSContext,
                                      mut this_val: JSValue,
                                      mut argc: libc::c_int,
                                      mut argv: *mut JSValue,
                                      mut magic: libc::c_int) -> JSValue {
    let mut f: *mut FILE = 0 as *mut FILE;
    let mut i: libc::c_int = 0;
    let mut str: *const libc::c_char = 0 as *const libc::c_char;
    let mut len: size_t = 0;
    if magic == 0 as libc::c_int {
        f = __stdoutp
    } else {
        f = js_std_file_get(ctx, this_val);
        if f.is_null() {
            return {
                       let mut init =
                           JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                   tag:
                                       JS_TAG_EXCEPTION as libc::c_int as
                                           int64_t,};
                       init
                   }
        }
    }
    i = 0 as libc::c_int;
    while i < argc {
        str = JS_ToCStringLen(ctx, &mut len, *argv.offset(i as isize));
        if str.is_null() {
            return {
                       let mut init =
                           JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                   tag:
                                       JS_TAG_EXCEPTION as libc::c_int as
                                           int64_t,};
                       init
                   }
        }
        fwrite(str as *const libc::c_void, 1 as libc::c_int as libc::c_ulong,
               len, f);
        JS_FreeCString(ctx, str);
        i += 1
    }
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
               init
           };
}
unsafe extern "C" fn js_std_file_close(mut ctx: *mut JSContext,
                                       mut this_val: JSValue,
                                       mut argc: libc::c_int,
                                       mut argv: *mut JSValue) -> JSValue {
    let mut s: *mut JSSTDFile =
        JS_GetOpaque2(ctx, this_val, js_std_file_class_id) as *mut JSSTDFile;
    let mut err: libc::c_int = 0;
    if s.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if (*s).f.is_null() {
        return JS_ThrowTypeError(ctx,
                                 b"invalid file handle\x00" as *const u8 as
                                     *const libc::c_char)
    }
    if (*s).is_popen != 0 {
        err = js_get_errno(pclose((*s).f) as ssize_t) as libc::c_int
    } else { err = js_get_errno(fclose((*s).f) as ssize_t) as libc::c_int }
    (*s).f = 0 as *mut FILE;
    return JS_NewInt32(ctx, err);
}
unsafe extern "C" fn js_std_file_printf(mut ctx: *mut JSContext,
                                        mut this_val: JSValue,
                                        mut argc: libc::c_int,
                                        mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    return js_printf_internal(ctx, argc, argv, f);
}
unsafe extern "C" fn js_std_file_flush(mut ctx: *mut JSContext,
                                       mut this_val: JSValue,
                                       mut argc: libc::c_int,
                                       mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    fflush(f);
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
               init
           };
}
unsafe extern "C" fn js_std_file_tell(mut ctx: *mut JSContext,
                                      mut this_val: JSValue,
                                      mut argc: libc::c_int,
                                      mut argv: *mut JSValue,
                                      mut is_bigint: libc::c_int) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    let mut pos: int64_t = 0;
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    pos = ftell(f) as int64_t;
    if is_bigint != 0 {
        return JS_NewBigInt64(ctx, pos)
    } else { return JS_NewInt64(ctx, pos) };
}
unsafe extern "C" fn js_std_file_seek(mut ctx: *mut JSContext,
                                      mut this_val: JSValue,
                                      mut argc: libc::c_int,
                                      mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    let mut pos: int64_t = 0;
    let mut whence: libc::c_int = 0;
    let mut ret: libc::c_int = 0;
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToInt64Ext(ctx, &mut pos, *argv.offset(0 as libc::c_int as isize))
           != 0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToInt32(ctx, &mut whence, *argv.offset(1 as libc::c_int as isize))
           != 0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = fseek(f, pos as libc::c_long, whence);
    if ret < 0 as libc::c_int { ret = -*__error() }
    return JS_NewInt32(ctx, ret);
}
unsafe extern "C" fn js_std_file_eof(mut ctx: *mut JSContext,
                                     mut this_val: JSValue,
                                     mut argc: libc::c_int,
                                     mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    return JS_NewBool(ctx, feof(f));
}
unsafe extern "C" fn js_std_file_error(mut ctx: *mut JSContext,
                                       mut this_val: JSValue,
                                       mut argc: libc::c_int,
                                       mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    return JS_NewBool(ctx, ferror(f));
}
unsafe extern "C" fn js_std_file_clearerr(mut ctx: *mut JSContext,
                                          mut this_val: JSValue,
                                          mut argc: libc::c_int,
                                          mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    clearerr(f);
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
               init
           };
}
unsafe extern "C" fn js_std_file_fileno(mut ctx: *mut JSContext,
                                        mut this_val: JSValue,
                                        mut argc: libc::c_int,
                                        mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    return JS_NewInt32(ctx, fileno(f));
}
unsafe extern "C" fn js_std_file_read_write(mut ctx: *mut JSContext,
                                            mut this_val: JSValue,
                                            mut argc: libc::c_int,
                                            mut argv: *mut JSValue,
                                            mut magic: libc::c_int)
 -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    let mut pos: uint64_t = 0;
    let mut len: uint64_t = 0;
    let mut size: size_t = 0;
    let mut ret: size_t = 0;
    let mut buf: *mut uint8_t = 0 as *mut uint8_t;
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToIndex(ctx, &mut pos, *argv.offset(1 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToIndex(ctx, &mut len, *argv.offset(2 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    buf =
        JS_GetArrayBuffer(ctx, &mut size,
                          *argv.offset(0 as libc::c_int as isize));
    if buf.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if pos.wrapping_add(len) > size as libc::c_ulonglong {
        return JS_ThrowRangeError(ctx,
                                  b"read/write array buffer overflow\x00" as
                                      *const u8 as *const libc::c_char)
    }
    if magic != 0 {
        ret =
            fwrite(buf.offset(pos as isize) as *const libc::c_void,
                   1 as libc::c_int as libc::c_ulong, len as libc::c_ulong, f)
    } else {
        ret =
            fread(buf.offset(pos as isize) as *mut libc::c_void,
                  1 as libc::c_int as libc::c_ulong, len as libc::c_ulong, f)
    }
    return JS_NewInt64(ctx, ret as int64_t);
}
/* XXX: could use less memory and go faster */
unsafe extern "C" fn js_std_file_getline(mut ctx: *mut JSContext,
                                         mut this_val: JSValue,
                                         mut argc: libc::c_int,
                                         mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    let mut c: libc::c_int = 0;
    let mut dbuf: DynBuf =
        DynBuf{buf: 0 as *mut uint8_t,
               size: 0,
               allocated_size: 0,
               error: 0,
               realloc_func: None,
               opaque: 0 as *mut libc::c_void,};
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    js_std_dbuf_init(ctx, &mut dbuf);
    loop  {
        c = fgetc(f);
        if c == -(1 as libc::c_int) {
            if !(dbuf.size == 0 as libc::c_int as libc::c_ulong) { break ; }
            /* EOF */
            dbuf_free(&mut dbuf);
            return {
                       let mut init =
                           JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                   tag:
                                       JS_TAG_NULL as libc::c_int as
                                           int64_t,};
                       init
                   }
        } else {
            if c == '\n' as i32 { break ; }
            if dbuf_putc(&mut dbuf, c as uint8_t) != 0 {
                dbuf_free(&mut dbuf);
                return JS_ThrowOutOfMemory(ctx)
            }
        }
    }
    obj = JS_NewStringLen(ctx, dbuf.buf as *const libc::c_char, dbuf.size);
    dbuf_free(&mut dbuf);
    return obj;
}
/* XXX: could use less memory and go faster */
unsafe extern "C" fn js_std_file_readAsString(mut ctx: *mut JSContext,
                                              mut this_val: JSValue,
                                              mut argc: libc::c_int,
                                              mut argv: *mut JSValue)
 -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    let mut c: libc::c_int = 0;
    let mut dbuf: DynBuf =
        DynBuf{buf: 0 as *mut uint8_t,
               size: 0,
               allocated_size: 0,
               error: 0,
               realloc_func: None,
               opaque: 0 as *mut libc::c_void,};
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut max_size64: uint64_t = 0;
    let mut max_size: size_t = 0;
    let mut max_size_val: JSValue =
        JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if argc >= 1 as libc::c_int {
        max_size_val = *argv.offset(0 as libc::c_int as isize)
    } else {
        max_size_val =
            {
                let mut init =
                    JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                            tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
                init
            }
    }
    max_size = -(1 as libc::c_int) as size_t;
    if JS_IsUndefined(max_size_val) == 0 {
        if JS_ToIndex(ctx, &mut max_size64, max_size_val) != 0 {
            return {
                       let mut init =
                           JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                   tag:
                                       JS_TAG_EXCEPTION as libc::c_int as
                                           int64_t,};
                       init
                   }
        }
        if max_size64 < max_size as libc::c_ulonglong {
            max_size = max_size64 as size_t
        }
    }
    js_std_dbuf_init(ctx, &mut dbuf);
    while max_size != 0 as libc::c_int as libc::c_ulong {
        c = fgetc(f);
        if c == -(1 as libc::c_int) { break ; }
        if dbuf_putc(&mut dbuf, c as uint8_t) != 0 {
            dbuf_free(&mut dbuf);
            return {
                       let mut init =
                           JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                   tag:
                                       JS_TAG_EXCEPTION as libc::c_int as
                                           int64_t,};
                       init
                   }
        }
        max_size = max_size.wrapping_sub(1)
    }
    obj = JS_NewStringLen(ctx, dbuf.buf as *const libc::c_char, dbuf.size);
    dbuf_free(&mut dbuf);
    return obj;
}
unsafe extern "C" fn js_std_file_getByte(mut ctx: *mut JSContext,
                                         mut this_val: JSValue,
                                         mut argc: libc::c_int,
                                         mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    return JS_NewInt32(ctx, fgetc(f));
}
unsafe extern "C" fn js_std_file_putByte(mut ctx: *mut JSContext,
                                         mut this_val: JSValue,
                                         mut argc: libc::c_int,
                                         mut argv: *mut JSValue) -> JSValue {
    let mut f: *mut FILE = js_std_file_get(ctx, this_val);
    let mut c: libc::c_int = 0;
    if f.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToInt32(ctx, &mut c, *argv.offset(0 as libc::c_int as isize)) != 0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    c = fputc(c, f);
    return JS_NewInt32(ctx, c);
}
unsafe extern "C" fn http_get_header_line(mut f: *mut FILE,
                                          mut buf: *mut libc::c_char,
                                          mut buf_size: size_t,
                                          mut dbuf: *mut DynBuf)
 -> libc::c_int {
    let mut c: libc::c_int = 0;
    let mut p: *mut libc::c_char = 0 as *mut libc::c_char;
    p = buf;
    loop  {
        c = fgetc(f);
        if c < 0 as libc::c_int { return -(1 as libc::c_int) }
        if (p.wrapping_offset_from(buf) as libc::c_long as libc::c_ulong) <
               buf_size.wrapping_sub(1 as libc::c_int as libc::c_ulong) {
            let fresh20 = p;
            p = p.offset(1);
            *fresh20 = c as libc::c_char
        }
        if !dbuf.is_null() { dbuf_putc(dbuf, c as uint8_t); }
        if c == '\n' as i32 { break ; }
    }
    *p = '\u{0}' as i32 as libc::c_char;
    return 0 as libc::c_int;
}
unsafe extern "C" fn http_get_status(mut buf: *const libc::c_char)
 -> libc::c_int {
    let mut p: *const libc::c_char = buf;
    while *p as libc::c_int != ' ' as i32 &&
              *p as libc::c_int != '\u{0}' as i32 {
        p = p.offset(1)
    }
    if *p as libc::c_int != ' ' as i32 { return 0 as libc::c_int }
    while *p as libc::c_int == ' ' as i32 { p = p.offset(1) }
    return atoi(p);
}
unsafe extern "C" fn js_std_urlGet(mut ctx: *mut JSContext,
                                   mut this_val: JSValue,
                                   mut argc: libc::c_int,
                                   mut argv: *mut JSValue) -> JSValue {
    let mut current_block: u64;
    let mut url: *const libc::c_char = 0 as *const libc::c_char;
    let mut cmd_buf: DynBuf =
        DynBuf{buf: 0 as *mut uint8_t,
               size: 0,
               allocated_size: 0,
               error: 0,
               realloc_func: None,
               opaque: 0 as *mut libc::c_void,};
    let mut data_buf_s: DynBuf =
        DynBuf{buf: 0 as *mut uint8_t,
               size: 0,
               allocated_size: 0,
               error: 0,
               realloc_func: None,
               opaque: 0 as *mut libc::c_void,};
    let mut data_buf: *mut DynBuf = &mut data_buf_s;
    let mut header_buf_s: DynBuf =
        DynBuf{buf: 0 as *mut uint8_t,
               size: 0,
               allocated_size: 0,
               error: 0,
               realloc_func: None,
               opaque: 0 as *mut libc::c_void,};
    let mut header_buf: *mut DynBuf = &mut header_buf_s;
    let mut buf: *mut libc::c_char = 0 as *mut libc::c_char;
    let mut i: size_t = 0;
    let mut len: size_t = 0;
    let mut c: libc::c_int = 0;
    let mut status: libc::c_int = 0;
    let mut response: JSValue =
        {
            let mut init =
                JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                        tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
            init
        };
    let mut ret_obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut options_obj: JSValue =
        JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut f: *mut FILE = 0 as *mut FILE;
    let mut binary_flag: libc::c_int = 0;
    let mut full_flag: libc::c_int = 0;
    url = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if url.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    binary_flag = FALSE as libc::c_int;
    full_flag = FALSE as libc::c_int;
    if argc >= 2 as libc::c_int {
        's_85:
            {
                options_obj = *argv.offset(1 as libc::c_int as isize);
                if !(get_bool_option(ctx, &mut binary_flag, options_obj,
                                     b"binary\x00" as *const u8 as
                                         *const libc::c_char) != 0) {
                    if !(get_bool_option(ctx, &mut full_flag, options_obj,
                                         b"full\x00" as *const u8 as
                                             *const libc::c_char) != 0) {
                        break 's_85 ;
                    }
                }
                JS_FreeCString(ctx, url);
                return {
                           let mut init =
                               JSValue{u:
                                           JSValueUnion{int32:
                                                            0 as
                                                                libc::c_int,},
                                       tag:
                                           JS_TAG_EXCEPTION as libc::c_int as
                                               int64_t,};
                           init
                       }
            }
    }
    js_std_dbuf_init(ctx, &mut cmd_buf);
    dbuf_printf(&mut cmd_buf as *mut DynBuf,
                b"%s \'\'\x00" as *const u8 as *const libc::c_char,
                b"curl -s -i\x00" as *const u8 as *const libc::c_char);
    len = strlen(url);
    i = 0 as libc::c_int as size_t;
    while i < len {
        c = *url.offset(i as isize) as libc::c_int;
        if c == '\'' as i32 || c == '\\' as i32 {
            dbuf_putc(&mut cmd_buf, '\\' as i32 as uint8_t);
        }
        dbuf_putc(&mut cmd_buf, c as uint8_t);
        i = i.wrapping_add(1)
    }
    JS_FreeCString(ctx, url);
    dbuf_putstr(&mut cmd_buf,
                b"\'\'\x00" as *const u8 as *const libc::c_char);
    dbuf_putc(&mut cmd_buf, '\u{0}' as i32 as uint8_t);
    if dbuf_error(&mut cmd_buf) != 0 {
        dbuf_free(&mut cmd_buf);
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    //    printf("%s\n", (char *)cmd_buf.buf);
    f =
        popen(cmd_buf.buf as *mut libc::c_char,
              b"r\x00" as *const u8 as *const libc::c_char);
    dbuf_free(&mut cmd_buf);
    if f.is_null() {
        return JS_ThrowTypeError(ctx,
                                 b"could not start curl\x00" as *const u8 as
                                     *const libc::c_char)
    }
    js_std_dbuf_init(ctx, data_buf);
    js_std_dbuf_init(ctx, header_buf);
    buf = js_malloc(ctx, 4096 as libc::c_int as size_t) as *mut libc::c_char;
    if !buf.is_null() {
        /* get the HTTP status */
        if http_get_header_line(f, buf, 4096 as libc::c_int as size_t,
                                0 as *mut DynBuf) < 0 as libc::c_int {
            status = 0 as libc::c_int;
            current_block = 258861209040827084;
        } else {
            status = http_get_status(buf);
            if full_flag == 0 &&
                   !(status >= 200 as libc::c_int &&
                         status <= 299 as libc::c_int) {
                current_block = 258861209040827084;
            } else {
                loop 
                     /* wait until there is an empty line */
                     {
                    if http_get_header_line(f, buf,
                                            4096 as libc::c_int as size_t,
                                            header_buf) < 0 as libc::c_int {
                        current_block =
                            258861209040827084; /* remove the trailing CRLF */
                        break ;
                    }
                    if !(strcmp(buf,
                                b"\r\n\x00" as *const u8 as
                                    *const libc::c_char) == 0) {
                        continue ;
                    }
                    if dbuf_error(header_buf) != 0 {
                        current_block = 4254343112372971250;
                        break ;
                    } else { current_block = 5892776923941496671; break ; }
                }
                match current_block {
                    258861209040827084 => { }
                    4254343112372971250 => { }
                    _ => {
                        (*header_buf).size =
                            ((*header_buf).size as
                                 libc::c_ulong).wrapping_sub(2 as libc::c_int
                                                                 as
                                                                 libc::c_ulong)
                                as size_t as size_t;
                        loop 
                             /* download the data */
                             {
                            len =
                                fread(buf as *mut libc::c_void,
                                      1 as libc::c_int as libc::c_ulong,
                                      4096 as libc::c_int as libc::c_ulong,
                                      f);
                            if len == 0 as libc::c_int as libc::c_ulong {
                                break ;
                            }
                            dbuf_put(data_buf, buf as *mut uint8_t, len);
                        }
                        if dbuf_error(data_buf) != 0 {
                            current_block = 4254343112372971250;
                        } else {
                            if binary_flag != 0 {
                                response =
                                    JS_NewArrayBufferCopy(ctx,
                                                          (*data_buf).buf,
                                                          (*data_buf).size)
                            } else {
                                response =
                                    JS_NewStringLen(ctx,
                                                    (*data_buf).buf as
                                                        *mut libc::c_char,
                                                    (*data_buf).size)
                            }
                            if JS_IsException(response) != 0 {
                                current_block = 4254343112372971250;
                            } else { current_block = 17141116216678922059; }
                        }
                    }
                }
            }
        }
        match current_block {
            4254343112372971250 => { }
            _ => {
                match current_block {
                    258861209040827084 => {
                        response =
                            {
                                let mut init =
                                    JSValue{u:
                                                JSValueUnion{int32:
                                                                 0 as
                                                                     libc::c_int,},
                                            tag:
                                                JS_TAG_NULL as libc::c_int as
                                                    int64_t,};
                                init
                            }
                    }
                    _ => { }
                }
                js_free(ctx, buf as *mut libc::c_void);
                buf = 0 as *mut libc::c_char;
                pclose(f);
                f = 0 as *mut FILE;
                dbuf_free(data_buf);
                data_buf = 0 as *mut DynBuf;
                if full_flag != 0 {
                    ret_obj = JS_NewObject(ctx);
                    if JS_IsException(ret_obj) != 0 {
                        current_block = 4254343112372971250;
                    } else {
                        JS_DefinePropertyValueStr(ctx, ret_obj,
                                                  b"response\x00" as *const u8
                                                      as *const libc::c_char,
                                                  response,
                                                  (1 as libc::c_int) <<
                                                      0 as libc::c_int |
                                                      (1 as libc::c_int) <<
                                                          1 as libc::c_int |
                                                      (1 as libc::c_int) <<
                                                          2 as libc::c_int);
                        if JS_IsNull(response) == 0 {
                            JS_DefinePropertyValueStr(ctx, ret_obj,
                                                      b"responseHeaders\x00"
                                                          as *const u8 as
                                                          *const libc::c_char,
                                                      JS_NewStringLen(ctx,
                                                                      (*header_buf).buf
                                                                          as
                                                                          *mut libc::c_char,
                                                                      (*header_buf).size),
                                                      (1 as libc::c_int) <<
                                                          0 as libc::c_int |
                                                          (1 as libc::c_int)
                                                              <<
                                                              1 as libc::c_int
                                                          |
                                                          (1 as libc::c_int)
                                                              <<
                                                              2 as
                                                                  libc::c_int);
                            JS_DefinePropertyValueStr(ctx, ret_obj,
                                                      b"status\x00" as
                                                          *const u8 as
                                                          *const libc::c_char,
                                                      JS_NewInt32(ctx,
                                                                  status),
                                                      (1 as libc::c_int) <<
                                                          0 as libc::c_int |
                                                          (1 as libc::c_int)
                                                              <<
                                                              1 as libc::c_int
                                                          |
                                                          (1 as libc::c_int)
                                                              <<
                                                              2 as
                                                                  libc::c_int);
                        }
                        current_block = 993425571616822999;
                    }
                } else {
                    ret_obj = response;
                    current_block = 993425571616822999;
                }
                match current_block {
                    4254343112372971250 => { }
                    _ => { dbuf_free(header_buf); return ret_obj }
                }
            }
        }
    }
    if !f.is_null() { pclose(f); }
    js_free(ctx, buf as *mut libc::c_void);
    if !data_buf.is_null() { dbuf_free(data_buf); }
    if !header_buf.is_null() { dbuf_free(header_buf); }
    JS_FreeValue(ctx, response);
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_EXCEPTION as libc::c_int as int64_t,};
               init
           };
}
static mut js_std_file_class: JSClassDef =
    unsafe {
        {
            let mut init =
                JSClassDef{class_name:
                               b"FILE\x00" as *const u8 as
                                   *const libc::c_char,
                           finalizer:
                               Some(js_std_file_finalizer as
                                        unsafe extern "C" fn(_:
                                                                 *mut JSRuntime,
                                                             _: JSValue)
                                            -> ()),
                           gc_mark: None,
                           call: None,
                           exotic:
                               0 as *const JSClassExoticMethods as
                                   *mut JSClassExoticMethods,};
            init
        }
    };
static mut js_std_error_props: [JSCFunctionListEntry; 11] =
    [{
         let mut init =
             JSCFunctionListEntry{name:
                                      b"EINVAL\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           22 as
                                                               libc::c_int,},};
         init
     },
     {
         let mut init =
             JSCFunctionListEntry{name:
                                      b"EIO\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           5 as
                                                               libc::c_int,},};
         init
     },
     {
         let mut init =
             JSCFunctionListEntry{name:
                                      b"EACCES\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           13 as
                                                               libc::c_int,},};
         init
     },
     {
         let mut init =
             JSCFunctionListEntry{name:
                                      b"EEXIST\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           17 as
                                                               libc::c_int,},};
         init
     },
     {
         let mut init =
             JSCFunctionListEntry{name:
                                      b"ENOSPC\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           28 as
                                                               libc::c_int,},};
         init
     },
     {
         let mut init =
             JSCFunctionListEntry{name:
                                      b"ENOSYS\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           78 as
                                                               libc::c_int,},};
         init
     },
     {
         let mut init =
             JSCFunctionListEntry{name:
                                      b"EBUSY\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           16 as
                                                               libc::c_int,},};
         init
     },
     {
         let mut init =
             JSCFunctionListEntry{name:
                                      b"ENOENT\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           2 as
                                                               libc::c_int,},};
         init
     },
     {
         let mut init =
             JSCFunctionListEntry{name:
                                      b"EPERM\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           1 as
                                                               libc::c_int,},};
         init
     },
     {
         let mut init =
             JSCFunctionListEntry{name:
                                      b"EPIPE\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           32 as
                                                               libc::c_int,},};
         init
     },
     {
         let mut init =
             JSCFunctionListEntry{name:
                                      b"EBADF\x00" as *const u8 as
                                          *const libc::c_char,
                                  prop_flags:
                                      ((1 as libc::c_int) << 0 as libc::c_int)
                                          as uint8_t,
                                  def_type: 4 as libc::c_int as uint8_t,
                                  magic: 0 as libc::c_int as int16_t,
                                  u:
                                      C2RustUnnamed_18{i32_0:
                                                           9 as
                                                               libc::c_int,},};
         init
     }];
// Initialized in run_static_initializers
static mut js_std_funcs: [JSCFunctionListEntry; 20] =
    [JSCFunctionListEntry{name: 0 as *const libc::c_char,
                          prop_flags: 0,
                          def_type: 0,
                          magic: 0,
                          u:
                              C2RustUnnamed_18{func:
                                                   C2RustUnnamed_22{length: 0,
                                                                    cproto: 0,
                                                                    cfunc:
                                                                        JSCFunctionType{generic:
                                                                                            None,},},},};
        20];
static mut js_std_file_proto_funcs: [JSCFunctionListEntry; 17] =
    unsafe {
        [{
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"close\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_close
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"puts\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 1 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_std_file_puts
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"printf\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_printf
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"flush\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_flush
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"tell\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_std_file_tell
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"tello\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 1 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_std_file_tell
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"seek\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_seek
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"eof\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_eof
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"fileno\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_fileno
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"error\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_error
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"clearerr\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_clearerr
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"read\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            3
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_std_file_read_write
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"write\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 1 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            3
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_std_file_read_write
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"getline\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_getline
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"readAsString\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_readAsString
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"getByte\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_getByte
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"putByte\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_file_putByte
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         }]
    };
unsafe extern "C" fn js_std_init(mut ctx: *mut JSContext,
                                 mut m: *mut JSModuleDef) -> libc::c_int {
    let mut proto: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    /* FILE class */
    /* the class ID is created once */
    JS_NewClassID(&mut js_std_file_class_id);
    /* the class is created once per runtime */
    JS_NewClass(JS_GetRuntime(ctx), js_std_file_class_id,
                &mut js_std_file_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_std_file_proto_funcs.as_ptr(),
                               (::std::mem::size_of::<[JSCFunctionListEntry; 17]>()
                                    as
                                    libc::c_ulong).wrapping_div(::std::mem::size_of::<JSCFunctionListEntry>()
                                                                    as
                                                                    libc::c_ulong)
                                   as libc::c_int);
    JS_SetClassProto(ctx, js_std_file_class_id, proto);
    JS_SetModuleExportList(ctx, m, js_std_funcs.as_ptr(),
                           (::std::mem::size_of::<[JSCFunctionListEntry; 20]>()
                                as
                                libc::c_ulong).wrapping_div(::std::mem::size_of::<JSCFunctionListEntry>()
                                                                as
                                                                libc::c_ulong)
                               as libc::c_int);
    JS_SetModuleExport(ctx, m, b"in\x00" as *const u8 as *const libc::c_char,
                       js_new_std_file(ctx, __stdinp, FALSE as libc::c_int,
                                       FALSE as libc::c_int));
    JS_SetModuleExport(ctx, m, b"out\x00" as *const u8 as *const libc::c_char,
                       js_new_std_file(ctx, __stdoutp, FALSE as libc::c_int,
                                       FALSE as libc::c_int));
    JS_SetModuleExport(ctx, m, b"err\x00" as *const u8 as *const libc::c_char,
                       js_new_std_file(ctx, __stderrp, FALSE as libc::c_int,
                                       FALSE as libc::c_int));
    return 0 as libc::c_int;
}
#[no_mangle]
pub unsafe extern "C" fn js_init_module_std(mut ctx: *mut JSContext,
                                            mut module_name:
                                                *const libc::c_char)
 -> *mut JSModuleDef {
    let mut m: *mut JSModuleDef = 0 as *mut JSModuleDef;
    m =
        JS_NewCModule(ctx, module_name,
                      Some(js_std_init as
                               unsafe extern "C" fn(_: *mut JSContext,
                                                    _: *mut JSModuleDef)
                                   -> libc::c_int));
    if m.is_null() { return 0 as *mut JSModuleDef }
    JS_AddModuleExportList(ctx, m, js_std_funcs.as_ptr(),
                           (::std::mem::size_of::<[JSCFunctionListEntry; 20]>()
                                as
                                libc::c_ulong).wrapping_div(::std::mem::size_of::<JSCFunctionListEntry>()
                                                                as
                                                                libc::c_ulong)
                               as libc::c_int);
    JS_AddModuleExport(ctx, m, b"in\x00" as *const u8 as *const libc::c_char);
    JS_AddModuleExport(ctx, m,
                       b"out\x00" as *const u8 as *const libc::c_char);
    JS_AddModuleExport(ctx, m,
                       b"err\x00" as *const u8 as *const libc::c_char);
    return m;
}
/* *********************************************************/
/* 'os' object */
unsafe extern "C" fn js_os_open(mut ctx: *mut JSContext,
                                mut this_val: JSValue, mut argc: libc::c_int,
                                mut argv: *mut JSValue) -> JSValue {
    let mut current_block: u64;
    let mut filename: *const libc::c_char = 0 as *const libc::c_char;
    let mut flags: libc::c_int = 0;
    let mut mode: libc::c_int = 0;
    let mut ret: libc::c_int = 0;
    filename = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if filename.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if !(JS_ToInt32(ctx, &mut flags, *argv.offset(1 as libc::c_int as isize))
             != 0) {
        if argc >= 3 as libc::c_int &&
               JS_IsUndefined(*argv.offset(2 as libc::c_int as isize)) == 0 {
            if JS_ToInt32(ctx, &mut mode,
                          *argv.offset(2 as libc::c_int as isize)) != 0 {
                current_block = 7798592442671369844;
            } else { current_block = 17965632435239708295; }
        } else {
            mode = 0o666 as libc::c_int;
            current_block = 17965632435239708295;
        }
        match current_block {
            7798592442671369844 => { }
            _ => {
                ret =
                    js_get_errno(open(filename, flags, mode) as ssize_t) as
                        libc::c_int;
                JS_FreeCString(ctx, filename);
                return JS_NewInt32(ctx, ret)
            }
        }
    }
    JS_FreeCString(ctx, filename);
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_EXCEPTION as libc::c_int as int64_t,};
               init
           };
}
unsafe extern "C" fn js_os_close(mut ctx: *mut JSContext,
                                 mut this_val: JSValue, mut argc: libc::c_int,
                                 mut argv: *mut JSValue) -> JSValue {
    let mut fd: libc::c_int = 0;
    let mut ret: libc::c_int = 0;
    if JS_ToInt32(ctx, &mut fd, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = js_get_errno(close(fd) as ssize_t) as libc::c_int;
    return JS_NewInt32(ctx, ret);
}
unsafe extern "C" fn js_os_seek(mut ctx: *mut JSContext,
                                mut this_val: JSValue, mut argc: libc::c_int,
                                mut argv: *mut JSValue) -> JSValue {
    let mut fd: libc::c_int = 0;
    let mut whence: libc::c_int = 0;
    let mut pos: int64_t = 0;
    let mut ret: int64_t = 0;
    let mut is_bigint: libc::c_int = 0;
    if JS_ToInt32(ctx, &mut fd, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    is_bigint = JS_IsBigInt(ctx, *argv.offset(1 as libc::c_int as isize));
    if JS_ToInt64Ext(ctx, &mut pos, *argv.offset(1 as libc::c_int as isize))
           != 0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToInt32(ctx, &mut whence, *argv.offset(2 as libc::c_int as isize))
           != 0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = lseek(fd, pos, whence);
    if ret == -(1 as libc::c_int) as libc::c_longlong {
        ret = -*__error() as int64_t
    }
    if is_bigint != 0 {
        return JS_NewBigInt64(ctx, ret)
    } else { return JS_NewInt64(ctx, ret) };
}
unsafe extern "C" fn js_os_read_write(mut ctx: *mut JSContext,
                                      mut this_val: JSValue,
                                      mut argc: libc::c_int,
                                      mut argv: *mut JSValue,
                                      mut magic: libc::c_int) -> JSValue {
    let mut fd: libc::c_int = 0;
    let mut pos: uint64_t = 0;
    let mut len: uint64_t = 0;
    let mut size: size_t = 0;
    let mut ret: ssize_t = 0;
    let mut buf: *mut uint8_t = 0 as *mut uint8_t;
    if JS_ToInt32(ctx, &mut fd, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToIndex(ctx, &mut pos, *argv.offset(2 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToIndex(ctx, &mut len, *argv.offset(3 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    buf =
        JS_GetArrayBuffer(ctx, &mut size,
                          *argv.offset(1 as libc::c_int as isize));
    if buf.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if pos.wrapping_add(len) > size as libc::c_ulonglong {
        return JS_ThrowRangeError(ctx,
                                  b"read/write array buffer overflow\x00" as
                                      *const u8 as *const libc::c_char)
    }
    if magic != 0 {
        ret =
            js_get_errno(write(fd,
                               buf.offset(pos as isize) as
                                   *const libc::c_void, len as size_t))
    } else {
        ret =
            js_get_errno(read(fd,
                              buf.offset(pos as isize) as *mut libc::c_void,
                              len as size_t))
    }
    return JS_NewInt64(ctx, ret as int64_t);
}
unsafe extern "C" fn js_os_isatty(mut ctx: *mut JSContext,
                                  mut this_val: JSValue,
                                  mut argc: libc::c_int,
                                  mut argv: *mut JSValue) -> JSValue {
    let mut fd: libc::c_int = 0;
    if JS_ToInt32(ctx, &mut fd, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    return JS_NewBool(ctx, (isatty(fd) == 1 as libc::c_int) as libc::c_int);
}
unsafe extern "C" fn js_os_ttyGetWinSize(mut ctx: *mut JSContext,
                                         mut this_val: JSValue,
                                         mut argc: libc::c_int,
                                         mut argv: *mut JSValue) -> JSValue {
    let mut fd: libc::c_int = 0;
    let mut ws: winsize =
        winsize{ws_row: 0, ws_col: 0, ws_xpixel: 0, ws_ypixel: 0,};
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    if JS_ToInt32(ctx, &mut fd, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if ioctl(fd,
             0x40000000 as libc::c_int as __uint32_t as libc::c_ulong |
                 (::std::mem::size_of::<winsize>() as libc::c_ulong &
                      0x1fff as libc::c_int as libc::c_ulong) <<
                     16 as libc::c_int |
                 (('t' as i32) << 8 as libc::c_int) as libc::c_ulong |
                 104 as libc::c_int as libc::c_ulong, &mut ws as *mut winsize)
           == 0 as libc::c_int && ws.ws_col as libc::c_int >= 4 as libc::c_int
           && ws.ws_row as libc::c_int >= 4 as libc::c_int {
        obj = JS_NewArray(ctx);
        if JS_IsException(obj) != 0 { return obj }
        JS_DefinePropertyValueUint32(ctx, obj, 0 as libc::c_int as uint32_t,
                                     JS_NewInt32(ctx, ws.ws_col as int32_t),
                                     (1 as libc::c_int) << 0 as libc::c_int |
                                         (1 as libc::c_int) <<
                                             1 as libc::c_int |
                                         (1 as libc::c_int) <<
                                             2 as libc::c_int);
        JS_DefinePropertyValueUint32(ctx, obj, 1 as libc::c_int as uint32_t,
                                     JS_NewInt32(ctx, ws.ws_row as int32_t),
                                     (1 as libc::c_int) << 0 as libc::c_int |
                                         (1 as libc::c_int) <<
                                             1 as libc::c_int |
                                         (1 as libc::c_int) <<
                                             2 as libc::c_int);
        return obj
    } else {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag: JS_TAG_NULL as libc::c_int as int64_t,};
                   init
               }
    };
}
static mut oldtty: termios =
    termios{c_iflag: 0,
            c_oflag: 0,
            c_cflag: 0,
            c_lflag: 0,
            c_cc: [0; 20],
            c_ispeed: 0,
            c_ospeed: 0,};
unsafe extern "C" fn term_exit() {
    tcsetattr(0 as libc::c_int, 0 as libc::c_int, &mut oldtty);
}
/* XXX: should add a way to go back to normal mode */
unsafe extern "C" fn js_os_ttySetRaw(mut ctx: *mut JSContext,
                                     mut this_val: JSValue,
                                     mut argc: libc::c_int,
                                     mut argv: *mut JSValue) -> JSValue {
    let mut tty: termios =
        termios{c_iflag: 0,
                c_oflag: 0,
                c_cflag: 0,
                c_lflag: 0,
                c_cc: [0; 20],
                c_ispeed: 0,
                c_ospeed: 0,};
    let mut fd: libc::c_int = 0;
    if JS_ToInt32(ctx, &mut fd, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    memset(&mut tty as *mut termios as *mut libc::c_void, 0 as libc::c_int,
           ::std::mem::size_of::<termios>() as libc::c_ulong);
    tcgetattr(fd, &mut tty);
    oldtty = tty;
    tty.c_iflag &=
        !(0x1 as libc::c_int | 0x2 as libc::c_int | 0x8 as libc::c_int |
              0x20 as libc::c_int | 0x40 as libc::c_int | 0x80 as libc::c_int
              | 0x100 as libc::c_int | 0x200 as libc::c_int) as libc::c_ulong;
    tty.c_oflag |= 0x1 as libc::c_int as libc::c_ulong;
    tty.c_lflag &=
        !(0x8 as libc::c_int | 0x10 as libc::c_int | 0x100 as libc::c_int |
              0x400 as libc::c_int) as libc::c_ulong;
    tty.c_cflag &=
        !(0x300 as libc::c_int | 0x1000 as libc::c_int) as libc::c_ulong;
    tty.c_cflag |= 0x300 as libc::c_int as libc::c_ulong;
    tty.c_cc[16 as libc::c_int as usize] = 1 as libc::c_int as cc_t;
    tty.c_cc[17 as libc::c_int as usize] = 0 as libc::c_int as cc_t;
    tcsetattr(fd, 0 as libc::c_int, &mut tty);
    atexit(Some(term_exit as unsafe extern "C" fn() -> ()));
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
               init
           };
}
/* !_WIN32 */
unsafe extern "C" fn js_os_remove(mut ctx: *mut JSContext,
                                  mut this_val: JSValue,
                                  mut argc: libc::c_int,
                                  mut argv: *mut JSValue) -> JSValue {
    let mut filename: *const libc::c_char = 0 as *const libc::c_char;
    let mut ret: libc::c_int = 0;
    filename = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if filename.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = js_get_errno(remove(filename) as ssize_t) as libc::c_int;
    JS_FreeCString(ctx, filename);
    return JS_NewInt32(ctx, ret);
}
unsafe extern "C" fn js_os_rename(mut ctx: *mut JSContext,
                                  mut this_val: JSValue,
                                  mut argc: libc::c_int,
                                  mut argv: *mut JSValue) -> JSValue {
    let mut oldpath: *const libc::c_char = 0 as *const libc::c_char;
    let mut newpath: *const libc::c_char = 0 as *const libc::c_char;
    let mut ret: libc::c_int = 0;
    oldpath = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if oldpath.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    newpath = JS_ToCString(ctx, *argv.offset(1 as libc::c_int as isize));
    if newpath.is_null() {
        JS_FreeCString(ctx, oldpath);
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = js_get_errno(rename(oldpath, newpath) as ssize_t) as libc::c_int;
    JS_FreeCString(ctx, oldpath);
    JS_FreeCString(ctx, newpath);
    return JS_NewInt32(ctx, ret);
}
unsafe extern "C" fn is_main_thread(mut rt: *mut JSRuntime) -> libc::c_int {
    let mut ts: *mut JSThreadState =
        JS_GetRuntimeOpaque(rt) as *mut JSThreadState;
    return (*ts).recv_pipe.is_null() as libc::c_int;
}
unsafe extern "C" fn find_rh(mut ts: *mut JSThreadState, mut fd: libc::c_int)
 -> *mut JSOSRWHandler {
    let mut rh: *mut JSOSRWHandler = 0 as *mut JSOSRWHandler;
    let mut el: *mut list_head = 0 as *mut list_head;
    el = (*ts).os_rw_handlers.next;
    while el != &mut (*ts).os_rw_handlers as *mut list_head {
        rh =
            (el as
                 *mut uint8_t).offset(-(&mut (*(0 as *mut JSOSRWHandler)).link
                                            as *mut list_head as size_t as
                                            isize)) as *mut JSOSRWHandler;
        if (*rh).fd == fd { return rh }
        el = (*el).next
    }
    return 0 as *mut JSOSRWHandler;
}
unsafe extern "C" fn free_rw_handler(mut rt: *mut JSRuntime,
                                     mut rh: *mut JSOSRWHandler) {
    let mut i: libc::c_int = 0;
    list_del(&mut (*rh).link);
    i = 0 as libc::c_int;
    while i < 2 as libc::c_int {
        JS_FreeValueRT(rt, (*rh).rw_func[i as usize]);
        i += 1
    }
    js_free_rt(rt, rh as *mut libc::c_void);
}
unsafe extern "C" fn js_os_setReadHandler(mut ctx: *mut JSContext,
                                          mut this_val: JSValue,
                                          mut argc: libc::c_int,
                                          mut argv: *mut JSValue,
                                          mut magic: libc::c_int) -> JSValue {
    let mut rt: *mut JSRuntime = JS_GetRuntime(ctx);
    let mut ts: *mut JSThreadState =
        JS_GetRuntimeOpaque(rt) as *mut JSThreadState;
    let mut rh: *mut JSOSRWHandler = 0 as *mut JSOSRWHandler;
    let mut fd: libc::c_int = 0;
    let mut func: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    if JS_ToInt32(ctx, &mut fd, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    func = *argv.offset(1 as libc::c_int as isize);
    if JS_IsNull(func) != 0 {
        rh = find_rh(ts, fd);
        if !rh.is_null() {
            JS_FreeValue(ctx, (*rh).rw_func[magic as usize]);
            (*rh).rw_func[magic as usize] =
                {
                    let mut init =
                        JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                tag: JS_TAG_NULL as libc::c_int as int64_t,};
                    init
                };
            if JS_IsNull((*rh).rw_func[0 as libc::c_int as usize]) != 0 &&
                   JS_IsNull((*rh).rw_func[1 as libc::c_int as usize]) != 0 {
                /* remove the entry */
                free_rw_handler(JS_GetRuntime(ctx), rh);
            }
        }
    } else {
        if JS_IsFunction(ctx, func) == 0 {
            return JS_ThrowTypeError(ctx,
                                     b"not a function\x00" as *const u8 as
                                         *const libc::c_char)
        }
        rh = find_rh(ts, fd);
        if rh.is_null() {
            rh =
                js_mallocz(ctx,
                           ::std::mem::size_of::<JSOSRWHandler>() as
                               libc::c_ulong) as *mut JSOSRWHandler;
            if rh.is_null() {
                return {
                           let mut init =
                               JSValue{u:
                                           JSValueUnion{int32:
                                                            0 as
                                                                libc::c_int,},
                                       tag:
                                           JS_TAG_EXCEPTION as libc::c_int as
                                               int64_t,};
                           init
                       }
            }
            (*rh).fd = fd;
            (*rh).rw_func[0 as libc::c_int as usize] =
                {
                    let mut init =
                        JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                tag: JS_TAG_NULL as libc::c_int as int64_t,};
                    init
                };
            (*rh).rw_func[1 as libc::c_int as usize] =
                {
                    let mut init =
                        JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                tag: JS_TAG_NULL as libc::c_int as int64_t,};
                    init
                };
            list_add_tail(&mut (*rh).link, &mut (*ts).os_rw_handlers);
        }
        JS_FreeValue(ctx, (*rh).rw_func[magic as usize]);
        (*rh).rw_func[magic as usize] = JS_DupValue(ctx, func)
    }
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
               init
           };
}
unsafe extern "C" fn find_sh(mut ts: *mut JSThreadState,
                             mut sig_num: libc::c_int)
 -> *mut JSOSSignalHandler {
    let mut sh: *mut JSOSSignalHandler = 0 as *mut JSOSSignalHandler;
    let mut el: *mut list_head = 0 as *mut list_head;
    el = (*ts).os_signal_handlers.next;
    while el != &mut (*ts).os_signal_handlers as *mut list_head {
        sh =
            (el as
                 *mut uint8_t).offset(-(&mut (*(0 as
                                                    *mut JSOSSignalHandler)).link
                                            as *mut list_head as size_t as
                                            isize)) as *mut JSOSSignalHandler;
        if (*sh).sig_num == sig_num { return sh }
        el = (*el).next
    }
    return 0 as *mut JSOSSignalHandler;
}
unsafe extern "C" fn free_sh(mut rt: *mut JSRuntime,
                             mut sh: *mut JSOSSignalHandler) {
    list_del(&mut (*sh).link);
    JS_FreeValueRT(rt, (*sh).func);
    js_free_rt(rt, sh as *mut libc::c_void);
}
unsafe extern "C" fn os_signal_handler(mut sig_num: libc::c_int) {
    os_pending_signals |= (1 as libc::c_int as uint64_t) << sig_num;
}
unsafe extern "C" fn js_os_signal(mut ctx: *mut JSContext,
                                  mut this_val: JSValue,
                                  mut argc: libc::c_int,
                                  mut argv: *mut JSValue) -> JSValue {
    let mut rt: *mut JSRuntime = JS_GetRuntime(ctx);
    let mut ts: *mut JSThreadState =
        JS_GetRuntimeOpaque(rt) as *mut JSThreadState;
    let mut sh: *mut JSOSSignalHandler = 0 as *mut JSOSSignalHandler;
    let mut sig_num: uint32_t = 0;
    let mut func: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut handler: sighandler_t = None;
    if is_main_thread(rt) == 0 {
        return JS_ThrowTypeError(ctx,
                                 b"signal handler can only be set in the main thread\x00"
                                     as *const u8 as *const libc::c_char)
    }
    if JS_ToUint32(ctx, &mut sig_num, *argv.offset(0 as libc::c_int as isize))
           != 0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if sig_num >= 64 as libc::c_int as libc::c_uint {
        return JS_ThrowRangeError(ctx,
                                  b"invalid signal number\x00" as *const u8 as
                                      *const libc::c_char)
    }
    func = *argv.offset(1 as libc::c_int as isize);
    /* func = null: SIG_DFL, func = undefined, SIG_IGN */
    if JS_IsNull(func) != 0 || JS_IsUndefined(func) != 0 {
        sh = find_sh(ts, sig_num as libc::c_int);
        if !sh.is_null() { free_sh(JS_GetRuntime(ctx), sh); }
        if JS_IsNull(func) != 0 {
            handler = None
        } else {
            handler =
                ::std::mem::transmute::<libc::intptr_t,
                                        Option<unsafe extern "C" fn(_:
                                                                        libc::c_int)
                                                   ->
                                                       ()>>(1 as libc::c_int
                                                                as
                                                                libc::intptr_t)
        }
        signal(sig_num as libc::c_int, handler);
    } else {
        if JS_IsFunction(ctx, func) == 0 {
            return JS_ThrowTypeError(ctx,
                                     b"not a function\x00" as *const u8 as
                                         *const libc::c_char)
        }
        sh = find_sh(ts, sig_num as libc::c_int);
        if sh.is_null() {
            sh =
                js_mallocz(ctx,
                           ::std::mem::size_of::<JSOSSignalHandler>() as
                               libc::c_ulong) as *mut JSOSSignalHandler;
            if sh.is_null() {
                return {
                           let mut init =
                               JSValue{u:
                                           JSValueUnion{int32:
                                                            0 as
                                                                libc::c_int,},
                                       tag:
                                           JS_TAG_EXCEPTION as libc::c_int as
                                               int64_t,};
                           init
                       }
            }
            (*sh).sig_num = sig_num as libc::c_int;
            list_add_tail(&mut (*sh).link, &mut (*ts).os_signal_handlers);
        }
        JS_FreeValue(ctx, (*sh).func);
        (*sh).func = JS_DupValue(ctx, func);
        signal(sig_num as libc::c_int,
               Some(os_signal_handler as
                        unsafe extern "C" fn(_: libc::c_int) -> ()));
    }
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
               init
           };
}
unsafe extern "C" fn get_time_ms() -> int64_t {
    let mut ts: timespec = timespec{tv_sec: 0, tv_nsec: 0,};
    clock_gettime(_CLOCK_MONOTONIC, &mut ts);
    return (ts.tv_sec as
                uint64_t).wrapping_mul(1000 as libc::c_int as
                                           libc::c_ulonglong).wrapping_add((ts.tv_nsec
                                                                                /
                                                                                1000000
                                                                                    as
                                                                                    libc::c_int
                                                                                    as
                                                                                    libc::c_long)
                                                                               as
                                                                               libc::c_ulonglong)
               as int64_t;
}
unsafe extern "C" fn unlink_timer(mut rt: *mut JSRuntime,
                                  mut th: *mut JSOSTimer) {
    if !(*th).link.prev.is_null() {
        list_del(&mut (*th).link);
        (*th).link.next = 0 as *mut list_head;
        (*th).link.prev = (*th).link.next
    };
}
unsafe extern "C" fn free_timer(mut rt: *mut JSRuntime,
                                mut th: *mut JSOSTimer) {
    JS_FreeValueRT(rt, (*th).func);
    js_free_rt(rt, th as *mut libc::c_void);
}
static mut js_os_timer_class_id: JSClassID = 0;
unsafe extern "C" fn js_os_timer_finalizer(mut rt: *mut JSRuntime,
                                           mut val: JSValue) {
    let mut th: *mut JSOSTimer =
        JS_GetOpaque(val, js_os_timer_class_id) as *mut JSOSTimer;
    if !th.is_null() {
        (*th).has_object = FALSE as libc::c_int;
        if (*th).link.prev.is_null() { free_timer(rt, th); }
    };
}
unsafe extern "C" fn js_os_timer_mark(mut rt: *mut JSRuntime,
                                      mut val: JSValue,
                                      mut mark_func: Option<JS_MarkFunc>) {
    let mut th: *mut JSOSTimer =
        JS_GetOpaque(val, js_os_timer_class_id) as *mut JSOSTimer;
    if !th.is_null() { JS_MarkValue(rt, (*th).func, mark_func); };
}
unsafe extern "C" fn js_os_setTimeout(mut ctx: *mut JSContext,
                                      mut this_val: JSValue,
                                      mut argc: libc::c_int,
                                      mut argv: *mut JSValue) -> JSValue {
    let mut rt: *mut JSRuntime = JS_GetRuntime(ctx);
    let mut ts: *mut JSThreadState =
        JS_GetRuntimeOpaque(rt) as *mut JSThreadState;
    let mut delay: int64_t = 0;
    let mut func: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut th: *mut JSOSTimer = 0 as *mut JSOSTimer;
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    func = *argv.offset(0 as libc::c_int as isize);
    if JS_IsFunction(ctx, func) == 0 {
        return JS_ThrowTypeError(ctx,
                                 b"not a function\x00" as *const u8 as
                                     *const libc::c_char)
    }
    if JS_ToInt64(ctx, &mut delay, *argv.offset(1 as libc::c_int as isize)) !=
           0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    obj = JS_NewObjectClass(ctx, js_os_timer_class_id as libc::c_int);
    if JS_IsException(obj) != 0 { return obj }
    th =
        js_mallocz(ctx, ::std::mem::size_of::<JSOSTimer>() as libc::c_ulong)
            as *mut JSOSTimer;
    if th.is_null() {
        JS_FreeValue(ctx, obj);
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    (*th).has_object = TRUE as libc::c_int;
    (*th).timeout = get_time_ms() + delay;
    (*th).func = JS_DupValue(ctx, func);
    list_add_tail(&mut (*th).link, &mut (*ts).os_timers);
    JS_SetOpaque(obj, th as *mut libc::c_void);
    return obj;
}
unsafe extern "C" fn js_os_clearTimeout(mut ctx: *mut JSContext,
                                        mut this_val: JSValue,
                                        mut argc: libc::c_int,
                                        mut argv: *mut JSValue) -> JSValue {
    let mut th: *mut JSOSTimer =
        JS_GetOpaque2(ctx, *argv.offset(0 as libc::c_int as isize),
                      js_os_timer_class_id) as *mut JSOSTimer;
    if th.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    unlink_timer(JS_GetRuntime(ctx), th);
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
               init
           };
}
static mut js_os_timer_class: JSClassDef =
    unsafe {
        {
            let mut init =
                JSClassDef{class_name:
                               b"OSTimer\x00" as *const u8 as
                                   *const libc::c_char,
                           finalizer:
                               Some(js_os_timer_finalizer as
                                        unsafe extern "C" fn(_:
                                                                 *mut JSRuntime,
                                                             _: JSValue)
                                            -> ()),
                           gc_mark:
                               Some(js_os_timer_mark as
                                        unsafe extern "C" fn(_:
                                                                 *mut JSRuntime,
                                                             _: JSValue,
                                                             _:
                                                                 Option<JS_MarkFunc>)
                                            -> ()),
                           call: None,
                           exotic:
                               0 as *const JSClassExoticMethods as
                                   *mut JSClassExoticMethods,};
            init
        }
    };
unsafe extern "C" fn call_handler(mut ctx: *mut JSContext,
                                  mut func: JSValue) {
    let mut ret: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut func1: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    /* 'func' might be destroyed when calling itself (if it frees the
       handler), so must take extra care */
    func1 = JS_DupValue(ctx, func);
    ret =
        JS_Call(ctx, func1,
                {
                    let mut init =
                        JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                tag:
                                    JS_TAG_UNDEFINED as libc::c_int as
                                        int64_t,};
                    init
                }, 0 as libc::c_int, 0 as *mut JSValue);
    JS_FreeValue(ctx, func1);
    if JS_IsException(ret) != 0 { js_std_dump_error(ctx); }
    JS_FreeValue(ctx, ret);
}
unsafe extern "C" fn handle_posted_message(mut rt: *mut JSRuntime,
                                           mut ctx: *mut JSContext,
                                           mut port:
                                               *mut JSWorkerMessageHandler)
 -> libc::c_int {
    return 0 as libc::c_int;
}
unsafe extern "C" fn js_os_poll(mut ctx: *mut JSContext) -> libc::c_int {
    let mut current_block: u64;
    let mut rt: *mut JSRuntime = JS_GetRuntime(ctx);
    let mut ts: *mut JSThreadState =
        JS_GetRuntimeOpaque(rt) as *mut JSThreadState;
    let mut ret: libc::c_int = 0;
    let mut fd_max: libc::c_int = 0;
    let mut min_delay: libc::c_int = 0;
    let mut cur_time: int64_t = 0;
    let mut delay: int64_t = 0;
    let mut rfds: fd_set = fd_set{fds_bits: [0; 32],};
    let mut wfds: fd_set = fd_set{fds_bits: [0; 32],};
    let mut rh: *mut JSOSRWHandler = 0 as *mut JSOSRWHandler;
    let mut el: *mut list_head = 0 as *mut list_head;
    let mut tv: timeval = timeval{tv_sec: 0, tv_usec: 0,};
    let mut tvp: *mut timeval = 0 as *mut timeval;
    /* only check signals in the main thread */
    if (*ts).recv_pipe.is_null() &&
           (os_pending_signals != 0 as libc::c_int as libc::c_ulonglong) as
               libc::c_int as libc::c_long != 0 {
        let mut sh: *mut JSOSSignalHandler =
            0 as *mut JSOSSignalHandler; /* no more events */
        let mut mask: uint64_t = 0;
        el = (*ts).os_signal_handlers.next;
        while el != &mut (*ts).os_signal_handlers as *mut list_head {
            sh =
                (el as
                     *mut uint8_t).offset(-(&mut (*(0 as
                                                        *mut JSOSSignalHandler)).link
                                                as *mut list_head as size_t as
                                                isize)) as
                    *mut JSOSSignalHandler;
            mask = (1 as libc::c_int as uint64_t) << (*sh).sig_num;
            if os_pending_signals & mask != 0 {
                os_pending_signals &= !mask;
                call_handler(ctx, (*sh).func);
                return 0 as libc::c_int
            }
            el = (*el).next
        }
    }
    if list_empty(&mut (*ts).os_rw_handlers) != 0 &&
           list_empty(&mut (*ts).os_timers) != 0 &&
           list_empty(&mut (*ts).port_list) != 0 {
        return -(1 as libc::c_int)
    }
    if list_empty(&mut (*ts).os_timers) == 0 {
        cur_time = get_time_ms();
        min_delay = 10000 as libc::c_int;
        el = (*ts).os_timers.next;
        while el != &mut (*ts).os_timers as *mut list_head {
            let mut th: *mut JSOSTimer =
                (el as
                     *mut uint8_t).offset(-(&mut (*(0 as *mut JSOSTimer)).link
                                                as *mut list_head as size_t as
                                                isize)) as *mut JSOSTimer;
            delay = (*th).timeout - cur_time;
            if delay <= 0 as libc::c_int as libc::c_longlong {
                let mut func: JSValue =
                    JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
                /* the timer expired */
                func = (*th).func;
                (*th).func =
                    {
                        let mut init =
                            JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                    tag:
                                        JS_TAG_UNDEFINED as libc::c_int as
                                            int64_t,};
                        init
                    };
                unlink_timer(rt, th);
                if (*th).has_object == 0 { free_timer(rt, th); }
                call_handler(ctx, func);
                JS_FreeValue(ctx, func);
                return 0 as libc::c_int
            } else {
                if delay < min_delay as libc::c_longlong {
                    min_delay = delay as libc::c_int
                }
            }
            el = (*el).next
        }
        tv.tv_sec = (min_delay / 1000 as libc::c_int) as __darwin_time_t;
        tv.tv_usec = min_delay % 1000 as libc::c_int * 1000 as libc::c_int;
        tvp = &mut tv
    } else { tvp = 0 as *mut timeval }
    fd_max = -(1 as libc::c_int);
    el = (*ts).os_rw_handlers.next;
    while el != &mut (*ts).os_rw_handlers as *mut list_head {
        rh =
            (el as
                 *mut uint8_t).offset(-(&mut (*(0 as *mut JSOSRWHandler)).link
                                            as *mut list_head as size_t as
                                            isize)) as *mut JSOSRWHandler;
        fd_max = max_int(fd_max, (*rh).fd);
        if JS_IsNull((*rh).rw_func[0 as libc::c_int as usize]) == 0 {
            let mut __fd: libc::c_int = (*rh).fd;
            rfds.fds_bits[(__fd as
                               libc::c_ulong).wrapping_div((::std::mem::size_of::<__int32_t>()
                                                                as
                                                                libc::c_ulong).wrapping_mul(8
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                libc::c_ulong))
                              as usize] |=
                ((1 as libc::c_int as libc::c_ulong) <<
                     (__fd as
                          libc::c_ulong).wrapping_rem((::std::mem::size_of::<__int32_t>()
                                                           as
                                                           libc::c_ulong).wrapping_mul(8
                                                                                           as
                                                                                           libc::c_int
                                                                                           as
                                                                                           libc::c_ulong)))
                    as __int32_t
        }
        if JS_IsNull((*rh).rw_func[1 as libc::c_int as usize]) == 0 {
            let mut __fd_0: libc::c_int = (*rh).fd;
            wfds.fds_bits[(__fd_0 as
                               libc::c_ulong).wrapping_div((::std::mem::size_of::<__int32_t>()
                                                                as
                                                                libc::c_ulong).wrapping_mul(8
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                libc::c_ulong))
                              as usize] |=
                ((1 as libc::c_int as libc::c_ulong) <<
                     (__fd_0 as
                          libc::c_ulong).wrapping_rem((::std::mem::size_of::<__int32_t>()
                                                           as
                                                           libc::c_ulong).wrapping_mul(8
                                                                                           as
                                                                                           libc::c_int
                                                                                           as
                                                                                           libc::c_ulong)))
                    as __int32_t
        }
        el = (*el).next
    }
    el = (*ts).port_list.next;
    while el != &mut (*ts).port_list as *mut list_head {
        let mut port: *mut JSWorkerMessageHandler =
            (el as
                 *mut uint8_t).offset(-(&mut (*(0 as
                                                    *mut JSWorkerMessageHandler)).link
                                            as *mut list_head as size_t as
                                            isize)) as
                *mut JSWorkerMessageHandler;
        if JS_IsNull((*port).on_message_func) == 0 {
            let mut ps: *mut JSWorkerMessagePipe = (*port).recv_pipe;
            fd_max = max_int(fd_max, (*ps).read_fd);
            let mut __fd_1: libc::c_int = (*ps).read_fd;
            rfds.fds_bits[(__fd_1 as
                               libc::c_ulong).wrapping_div((::std::mem::size_of::<__int32_t>()
                                                                as
                                                                libc::c_ulong).wrapping_mul(8
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                libc::c_ulong))
                              as usize] |=
                ((1 as libc::c_int as libc::c_ulong) <<
                     (__fd_1 as
                          libc::c_ulong).wrapping_rem((::std::mem::size_of::<__int32_t>()
                                                           as
                                                           libc::c_ulong).wrapping_mul(8
                                                                                           as
                                                                                           libc::c_int
                                                                                           as
                                                                                           libc::c_ulong)))
                    as __int32_t
        }
        el = (*el).next
    }
    ret =
        select(fd_max + 1 as libc::c_int, &mut rfds, &mut wfds,
               0 as *mut fd_set, tvp);
    if ret > 0 as libc::c_int {
        el = (*ts).os_rw_handlers.next;
        loop  {
            if !(el != &mut (*ts).os_rw_handlers as *mut list_head) {
                current_block = 17485376261910781866;
                break ;
            }
            rh =
                (el as
                     *mut uint8_t).offset(-(&mut (*(0 as
                                                        *mut JSOSRWHandler)).link
                                                as *mut list_head as size_t as
                                                isize)) as *mut JSOSRWHandler;
            if JS_IsNull((*rh).rw_func[0 as libc::c_int as usize]) == 0 &&
                   __darwin_fd_isset((*rh).fd, &mut rfds) != 0 {
                call_handler(ctx, (*rh).rw_func[0 as libc::c_int as usize]);
                /* must stop because the list may have been modified */
                current_block = 9495212956593647312;
                break ;
            } else if JS_IsNull((*rh).rw_func[1 as libc::c_int as usize]) == 0
                          && __darwin_fd_isset((*rh).fd, &mut wfds) != 0 {
                call_handler(ctx, (*rh).rw_func[1 as libc::c_int as usize]);
                current_block = 9495212956593647312;
                break ;
            } else { el = (*el).next }
        }
        match current_block {
            9495212956593647312 => { }
            _ => {
                el = (*ts).port_list.next;
                while el != &mut (*ts).port_list as *mut list_head {
                    let mut port_0: *mut JSWorkerMessageHandler =
                        (el as
                             *mut uint8_t).offset(-(&mut (*(0 as
                                                                *mut JSWorkerMessageHandler)).link
                                                        as *mut list_head as
                                                        size_t as isize)) as
                            *mut JSWorkerMessageHandler;
                    if JS_IsNull((*port_0).on_message_func) == 0 {
                        let mut ps_0: *mut JSWorkerMessagePipe =
                            (*port_0).recv_pipe;
                        if __darwin_fd_isset((*ps_0).read_fd, &mut rfds) != 0
                           {
                            if handle_posted_message(rt, ctx, port_0) != 0 {
                                break ;
                            }
                        }
                    }
                    el = (*el).next
                }
            }
        }
    }
    /* must stop because the list may have been modified */
    return 0 as libc::c_int;
}
/* !_WIN32 */
unsafe extern "C" fn make_obj_error(mut ctx: *mut JSContext, mut obj: JSValue,
                                    mut err: libc::c_int) -> JSValue {
    let mut arr: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    if JS_IsException(obj) != 0 { return obj }
    arr = JS_NewArray(ctx);
    if JS_IsException(arr) != 0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    JS_DefinePropertyValueUint32(ctx, arr, 0 as libc::c_int as uint32_t, obj,
                                 (1 as libc::c_int) << 0 as libc::c_int |
                                     (1 as libc::c_int) << 1 as libc::c_int |
                                     (1 as libc::c_int) << 2 as libc::c_int);
    JS_DefinePropertyValueUint32(ctx, arr, 1 as libc::c_int as uint32_t,
                                 JS_NewInt32(ctx, err),
                                 (1 as libc::c_int) << 0 as libc::c_int |
                                     (1 as libc::c_int) << 1 as libc::c_int |
                                     (1 as libc::c_int) << 2 as libc::c_int);
    return arr;
}
unsafe extern "C" fn make_string_error(mut ctx: *mut JSContext,
                                       mut buf: *const libc::c_char,
                                       mut err: libc::c_int) -> JSValue {
    return make_obj_error(ctx, JS_NewString(ctx, buf), err);
}
/* return [cwd, errorcode] */
unsafe extern "C" fn js_os_getcwd(mut ctx: *mut JSContext,
                                  mut this_val: JSValue,
                                  mut argc: libc::c_int,
                                  mut argv: *mut JSValue) -> JSValue {
    let mut buf: [libc::c_char; 1024] = [0; 1024];
    let mut err: libc::c_int = 0;
    if getcwd(buf.as_mut_ptr(),
              ::std::mem::size_of::<[libc::c_char; 1024]>() as
                  libc::c_ulong).is_null() {
        buf[0 as libc::c_int as usize] = '\u{0}' as i32 as libc::c_char;
        err = *__error()
    } else { err = 0 as libc::c_int }
    return make_string_error(ctx, buf.as_mut_ptr(), err);
}
unsafe extern "C" fn js_os_chdir(mut ctx: *mut JSContext,
                                 mut this_val: JSValue, mut argc: libc::c_int,
                                 mut argv: *mut JSValue) -> JSValue {
    let mut target: *const libc::c_char = 0 as *const libc::c_char;
    let mut err: libc::c_int = 0;
    target = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if target.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    err = js_get_errno(chdir(target) as ssize_t) as libc::c_int;
    JS_FreeCString(ctx, target);
    return JS_NewInt32(ctx, err);
}
unsafe extern "C" fn js_os_mkdir(mut ctx: *mut JSContext,
                                 mut this_val: JSValue, mut argc: libc::c_int,
                                 mut argv: *mut JSValue) -> JSValue {
    let mut mode: libc::c_int = 0;
    let mut ret: libc::c_int = 0;
    let mut path: *const libc::c_char = 0 as *const libc::c_char;
    if argc >= 2 as libc::c_int {
        if JS_ToInt32(ctx, &mut mode, *argv.offset(1 as libc::c_int as isize))
               != 0 {
            return {
                       let mut init =
                           JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                   tag:
                                       JS_TAG_EXCEPTION as libc::c_int as
                                           int64_t,};
                       init
                   }
        }
    } else { mode = 0o777 as libc::c_int }
    path = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if path.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = js_get_errno(mkdir(path, mode as mode_t) as ssize_t) as libc::c_int;
    JS_FreeCString(ctx, path);
    return JS_NewInt32(ctx, ret);
}
/* return [array, errorcode] */
unsafe extern "C" fn js_os_readdir(mut ctx: *mut JSContext,
                                   mut this_val: JSValue,
                                   mut argc: libc::c_int,
                                   mut argv: *mut JSValue) -> JSValue {
    let mut path: *const libc::c_char = 0 as *const libc::c_char;
    let mut f: *mut DIR = 0 as *mut DIR;
    let mut d: *mut dirent = 0 as *mut dirent;
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut err: libc::c_int = 0;
    let mut len: uint32_t = 0;
    path = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if path.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    obj = JS_NewArray(ctx);
    if JS_IsException(obj) != 0 {
        JS_FreeCString(ctx, path);
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    f = opendir(path);
    if f.is_null() { err = *__error() } else { err = 0 as libc::c_int }
    JS_FreeCString(ctx, path);
    if !f.is_null() {
        len = 0 as libc::c_int as uint32_t;
        loop  {
            *__error() = 0 as libc::c_int;
            d = readdir(f);
            if d.is_null() {
                err = *__error();
                break ;
            } else {
                let fresh21 = len;
                len = len.wrapping_add(1);
                JS_DefinePropertyValueUint32(ctx, obj, fresh21,
                                             JS_NewString(ctx,
                                                          (*d).d_name.as_mut_ptr()),
                                             (1 as libc::c_int) <<
                                                 0 as libc::c_int |
                                                 (1 as libc::c_int) <<
                                                     1 as libc::c_int |
                                                 (1 as libc::c_int) <<
                                                     2 as libc::c_int);
            }
        }
        closedir(f);
    }
    return make_obj_error(ctx, obj, err);
}
unsafe extern "C" fn timespec_to_ms(mut tv: *const timespec) -> int64_t {
    return (*tv).tv_sec as int64_t * 1000 as libc::c_int as libc::c_longlong +
               ((*tv).tv_nsec / 1000000 as libc::c_int as libc::c_long) as
                   libc::c_longlong;
}
/* return [obj, errcode] */
unsafe extern "C" fn js_os_stat(mut ctx: *mut JSContext,
                                mut this_val: JSValue, mut argc: libc::c_int,
                                mut argv: *mut JSValue,
                                mut is_lstat: libc::c_int) -> JSValue {
    let mut path: *const libc::c_char = 0 as *const libc::c_char;
    let mut err: libc::c_int = 0;
    let mut res: libc::c_int = 0;
    let mut st: stat =
        stat{st_dev: 0,
             st_mode: 0,
             st_nlink: 0,
             st_ino: 0,
             st_uid: 0,
             st_gid: 0,
             st_rdev: 0,
             st_atimespec: timespec{tv_sec: 0, tv_nsec: 0,},
             st_mtimespec: timespec{tv_sec: 0, tv_nsec: 0,},
             st_ctimespec: timespec{tv_sec: 0, tv_nsec: 0,},
             st_birthtimespec: timespec{tv_sec: 0, tv_nsec: 0,},
             st_size: 0,
             st_blocks: 0,
             st_blksize: 0,
             st_flags: 0,
             st_gen: 0,
             st_lspare: 0,
             st_qspare: [0; 2],};
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    path = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if path.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if is_lstat != 0 {
        res = lstat(path, &mut st)
    } else { res = stat(path, &mut st) }
    JS_FreeCString(ctx, path);
    if res < 0 as libc::c_int {
        err = *__error();
        obj =
            {
                let mut init =
                    JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                            tag: JS_TAG_NULL as libc::c_int as int64_t,};
                init
            }
    } else {
        err = 0 as libc::c_int;
        obj = JS_NewObject(ctx);
        if JS_IsException(obj) != 0 {
            return {
                       let mut init =
                           JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                   tag:
                                       JS_TAG_EXCEPTION as libc::c_int as
                                           int64_t,};
                       init
                   }
        }
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"dev\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx, st.st_dev as int64_t),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"ino\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx, st.st_ino as int64_t),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"mode\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt32(ctx, st.st_mode as int32_t),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"nlink\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx, st.st_nlink as int64_t),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"uid\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx, st.st_uid as int64_t),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"gid\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx, st.st_gid as int64_t),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"rdev\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx, st.st_rdev as int64_t),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"size\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx, st.st_size),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"blocks\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx, st.st_blocks),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"atime\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx,
                                              timespec_to_ms(&mut st.st_atimespec)),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"mtime\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx,
                                              timespec_to_ms(&mut st.st_mtimespec)),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
        JS_DefinePropertyValueStr(ctx, obj,
                                  b"ctime\x00" as *const u8 as
                                      *const libc::c_char,
                                  JS_NewInt64(ctx,
                                              timespec_to_ms(&mut st.st_ctimespec)),
                                  (1 as libc::c_int) << 0 as libc::c_int |
                                      (1 as libc::c_int) << 1 as libc::c_int |
                                      (1 as libc::c_int) << 2 as libc::c_int);
    }
    return make_obj_error(ctx, obj, err);
}
unsafe extern "C" fn ms_to_timeval(mut tv: *mut timeval, mut v: uint64_t) {
    (*tv).tv_sec =
        v.wrapping_div(1000 as libc::c_int as libc::c_ulonglong) as
            __darwin_time_t;
    (*tv).tv_usec =
        v.wrapping_rem(1000 as libc::c_int as
                           libc::c_ulonglong).wrapping_mul(1000 as libc::c_int
                                                               as
                                                               libc::c_ulonglong)
            as __darwin_suseconds_t;
}
unsafe extern "C" fn js_os_utimes(mut ctx: *mut JSContext,
                                  mut this_val: JSValue,
                                  mut argc: libc::c_int,
                                  mut argv: *mut JSValue) -> JSValue {
    let mut path: *const libc::c_char = 0 as *const libc::c_char;
    let mut atime: int64_t = 0;
    let mut mtime: int64_t = 0;
    let mut ret: libc::c_int = 0;
    if JS_ToInt64(ctx, &mut atime, *argv.offset(1 as libc::c_int as isize)) !=
           0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToInt64(ctx, &mut mtime, *argv.offset(2 as libc::c_int as isize)) !=
           0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    path = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if path.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    let mut times: [timeval; 2] = [timeval{tv_sec: 0, tv_usec: 0,}; 2];
    ms_to_timeval(&mut *times.as_mut_ptr().offset(0 as libc::c_int as isize),
                  atime as uint64_t);
    ms_to_timeval(&mut *times.as_mut_ptr().offset(1 as libc::c_int as isize),
                  mtime as uint64_t);
    ret =
        js_get_errno(utimes(path, times.as_mut_ptr()) as ssize_t) as
            libc::c_int;
    JS_FreeCString(ctx, path);
    return JS_NewInt32(ctx, ret);
}
/* return [path, errorcode] */
unsafe extern "C" fn js_os_realpath(mut ctx: *mut JSContext,
                                    mut this_val: JSValue,
                                    mut argc: libc::c_int,
                                    mut argv: *mut JSValue) -> JSValue {
    let mut path: *const libc::c_char = 0 as *const libc::c_char;
    let mut buf: [libc::c_char; 1024] = [0; 1024];
    let mut res: *mut libc::c_char = 0 as *mut libc::c_char;
    let mut err: libc::c_int = 0;
    path = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if path.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    res = realpath(path, buf.as_mut_ptr());
    JS_FreeCString(ctx, path);
    if res.is_null() {
        buf[0 as libc::c_int as usize] = '\u{0}' as i32 as libc::c_char;
        err = *__error()
    } else { err = 0 as libc::c_int }
    return make_string_error(ctx, buf.as_mut_ptr(), err);
}
unsafe extern "C" fn js_os_symlink(mut ctx: *mut JSContext,
                                   mut this_val: JSValue,
                                   mut argc: libc::c_int,
                                   mut argv: *mut JSValue) -> JSValue {
    let mut target: *const libc::c_char = 0 as *const libc::c_char;
    let mut linkpath: *const libc::c_char = 0 as *const libc::c_char;
    let mut err: libc::c_int = 0;
    target = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if target.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    linkpath = JS_ToCString(ctx, *argv.offset(1 as libc::c_int as isize));
    if linkpath.is_null() {
        JS_FreeCString(ctx, target);
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    err = js_get_errno(symlink(target, linkpath) as ssize_t) as libc::c_int;
    JS_FreeCString(ctx, target);
    JS_FreeCString(ctx, linkpath);
    return JS_NewInt32(ctx, err);
}
/* return [path, errorcode] */
unsafe extern "C" fn js_os_readlink(mut ctx: *mut JSContext,
                                    mut this_val: JSValue,
                                    mut argc: libc::c_int,
                                    mut argv: *mut JSValue) -> JSValue {
    let mut path: *const libc::c_char = 0 as *const libc::c_char;
    let mut buf: [libc::c_char; 1024] = [0; 1024];
    let mut err: libc::c_int = 0;
    let mut res: ssize_t = 0;
    path = JS_ToCString(ctx, *argv.offset(0 as libc::c_int as isize));
    if path.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    res =
        readlink(path, buf.as_mut_ptr(),
                 (::std::mem::size_of::<[libc::c_char; 1024]>() as
                      libc::c_ulong).wrapping_sub(1 as libc::c_int as
                                                      libc::c_ulong));
    if res < 0 as libc::c_int as libc::c_long {
        buf[0 as libc::c_int as usize] = '\u{0}' as i32 as libc::c_char;
        err = *__error()
    } else {
        buf[res as usize] = '\u{0}' as i32 as libc::c_char;
        err = 0 as libc::c_int
    }
    JS_FreeCString(ctx, path);
    return make_string_error(ctx, buf.as_mut_ptr(), err);
}
unsafe extern "C" fn build_envp(mut ctx: *mut JSContext, mut obj: JSValue)
 -> *mut *mut libc::c_char {
    let mut current_block: u64;
    let mut len: uint32_t = 0;
    let mut i: uint32_t = 0;
    let mut tab: *mut JSPropertyEnum = 0 as *mut JSPropertyEnum;
    let mut envp: *mut *mut libc::c_char = 0 as *mut *mut libc::c_char;
    let mut pair: *mut libc::c_char = 0 as *mut libc::c_char;
    let mut key: *const libc::c_char = 0 as *const libc::c_char;
    let mut str: *const libc::c_char = 0 as *const libc::c_char;
    let mut val: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut key_len: size_t = 0;
    let mut str_len: size_t = 0;
    if JS_GetOwnPropertyNames(ctx, &mut tab, &mut len, obj,
                              (1 as libc::c_int) << 0 as libc::c_int |
                                  (1 as libc::c_int) << 4 as libc::c_int) <
           0 as libc::c_int {
        return 0 as *mut *mut libc::c_char
    }
    envp =
        js_mallocz(ctx,
                   (::std::mem::size_of::<*mut libc::c_char>() as
                        libc::c_ulong).wrapping_mul((len as
                                                         size_t).wrapping_add(1
                                                                                  as
                                                                                  libc::c_int
                                                                                  as
                                                                                  libc::c_ulong)))
            as *mut *mut libc::c_char;
    if envp.is_null() {
        current_block = 11768454257421168423;
    } else {
        i = 0 as libc::c_int as uint32_t;
        loop  {
            if !(i < len) { current_block = 6356883586791534486; break ; }
            val = JS_GetProperty(ctx, obj, (*tab.offset(i as isize)).atom);
            if JS_IsException(val) != 0 {
                current_block = 11768454257421168423;
                break ;
            }
            str = JS_ToCString(ctx, val);
            JS_FreeValue(ctx, val);
            if str.is_null() { current_block = 11768454257421168423; break ; }
            key = JS_AtomToCString(ctx, (*tab.offset(i as isize)).atom);
            if key.is_null() {
                JS_FreeCString(ctx, str);
                current_block = 11768454257421168423;
                break ;
            } else {
                key_len = strlen(key);
                str_len = strlen(str);
                pair =
                    js_malloc(ctx,
                              key_len.wrapping_add(str_len).wrapping_add(2 as
                                                                             libc::c_int
                                                                             as
                                                                             libc::c_ulong))
                        as *mut libc::c_char;
                if pair.is_null() {
                    JS_FreeCString(ctx, key);
                    JS_FreeCString(ctx, str);
                    current_block = 11768454257421168423;
                    break ;
                } else {
                    memcpy(pair as *mut libc::c_void,
                           key as *const libc::c_void, key_len);
                    *pair.offset(key_len as isize) =
                        '=' as i32 as libc::c_char;
                    memcpy(pair.offset(key_len as
                                           isize).offset(1 as libc::c_int as
                                                             isize) as
                               *mut libc::c_void, str as *const libc::c_void,
                           str_len);
                    *pair.offset(key_len.wrapping_add(1 as libc::c_int as
                                                          libc::c_ulong).wrapping_add(str_len)
                                     as isize) =
                        '\u{0}' as i32 as libc::c_char;
                    let ref mut fresh22 = *envp.offset(i as isize);
                    *fresh22 = pair;
                    JS_FreeCString(ctx, key);
                    JS_FreeCString(ctx, str);
                    i = i.wrapping_add(1)
                }
            }
        }
    }
    match current_block {
        11768454257421168423 => {
            if !envp.is_null() {
                i = 0 as libc::c_int as uint32_t;
                while i < len {
                    js_free(ctx,
                            *envp.offset(i as isize) as *mut libc::c_void);
                    i = i.wrapping_add(1)
                }
                js_free(ctx, envp as *mut libc::c_void);
                envp = 0 as *mut *mut libc::c_char
            }
        }
        _ => { }
    }
    i = 0 as libc::c_int as uint32_t;
    while i < len {
        JS_FreeAtom(ctx, (*tab.offset(i as isize)).atom);
        i = i.wrapping_add(1)
    }
    js_free(ctx, tab as *mut libc::c_void);
    return envp;
}
/* execvpe is not available on non GNU systems */
unsafe extern "C" fn my_execvpe(mut filename: *const libc::c_char,
                                mut argv: *mut *mut libc::c_char,
                                mut envp: *mut *mut libc::c_char)
 -> libc::c_int {
    let mut path: *mut libc::c_char = 0 as *mut libc::c_char;
    let mut p: *mut libc::c_char = 0 as *mut libc::c_char;
    let mut p_next: *mut libc::c_char = 0 as *mut libc::c_char;
    let mut p1: *mut libc::c_char = 0 as *mut libc::c_char;
    let mut buf: [libc::c_char; 1024] = [0; 1024];
    let mut filename_len: size_t = 0;
    let mut path_len: size_t = 0;
    let mut eacces_error: libc::c_int = 0;
    filename_len = strlen(filename);
    if filename_len == 0 as libc::c_int as libc::c_ulong {
        *__error() = 2 as libc::c_int;
        return -(1 as libc::c_int)
    }
    if !strchr(filename, '/' as i32).is_null() {
        return execve(filename, argv, envp)
    }
    path = getenv(b"PATH\x00" as *const u8 as *const libc::c_char);
    if path.is_null() {
        path =
            b"/bin:/usr/bin\x00" as *const u8 as *const libc::c_char as
                *mut libc::c_char
    }
    eacces_error = FALSE as libc::c_int;
    p = path;
    p = path;
    while !p.is_null() {
        p1 = strchr(p, ':' as i32);
        if p1.is_null() {
            p_next = 0 as *mut libc::c_char;
            path_len = strlen(p)
        } else {
            p_next = p1.offset(1 as libc::c_int as isize);
            path_len = p1.wrapping_offset_from(p) as libc::c_long as size_t
        }
        /* path too long */
        if !(path_len.wrapping_add(1 as libc::c_int as
                                       libc::c_ulong).wrapping_add(filename_len).wrapping_add(1
                                                                                                  as
                                                                                                  libc::c_int
                                                                                                  as
                                                                                                  libc::c_ulong)
                 > 1024 as libc::c_int as libc::c_ulong) {
            memcpy(buf.as_mut_ptr() as *mut libc::c_void,
                   p as *const libc::c_void, path_len);
            buf[path_len as usize] = '/' as i32 as libc::c_char;
            memcpy(buf.as_mut_ptr().offset(path_len as
                                               isize).offset(1 as libc::c_int
                                                                 as isize) as
                       *mut libc::c_void, filename as *const libc::c_void,
                   filename_len);
            buf[path_len.wrapping_add(1 as libc::c_int as
                                          libc::c_ulong).wrapping_add(filename_len)
                    as usize] = '\u{0}' as i32 as libc::c_char;
            execve(buf.as_mut_ptr(), argv, envp);
            match *__error() {
                13 => { eacces_error = TRUE as libc::c_int }
                2 | 20 => { }
                _ => { return -(1 as libc::c_int) }
            }
        }
        p = p_next
    }
    if eacces_error != 0 { *__error() = 13 as libc::c_int }
    return -(1 as libc::c_int);
}
/* exec(args[, options]) -> exitcode */
unsafe extern "C" fn js_os_exec(mut ctx: *mut JSContext,
                                mut this_val: JSValue, mut argc: libc::c_int,
                                mut argv: *mut JSValue) -> JSValue {
    let mut current_block: u64;
    let mut options: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut args: JSValue = *argv.offset(0 as libc::c_int as isize);
    let mut val: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut ret_val: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut exec_argv: *mut *const libc::c_char =
        0 as *mut *const libc::c_char;
    let mut file: *const libc::c_char = 0 as *const libc::c_char;
    let mut str: *const libc::c_char = 0 as *const libc::c_char;
    let mut cwd: *const libc::c_char = 0 as *const libc::c_char;
    let mut envp: *mut *mut libc::c_char = *_NSGetEnviron();
    let mut exec_argc: uint32_t = 0;
    let mut i: uint32_t = 0;
    let mut ret: libc::c_int = 0;
    let mut pid: libc::c_int = 0;
    let mut status: libc::c_int = 0;
    let mut block_flag: libc::c_int = TRUE as libc::c_int;
    let mut use_path: libc::c_int = TRUE as libc::c_int;
    static mut std_name: [*const libc::c_char; 3] =
        [b"stdin\x00" as *const u8 as *const libc::c_char,
         b"stdout\x00" as *const u8 as *const libc::c_char,
         b"stderr\x00" as *const u8 as *const libc::c_char];
    let mut std_fds: [libc::c_int; 3] = [0; 3];
    let mut uid: uint32_t = -(1 as libc::c_int) as uint32_t;
    let mut gid: uint32_t = -(1 as libc::c_int) as uint32_t;
    val =
        JS_GetPropertyStr(ctx, args,
                          b"length\x00" as *const u8 as *const libc::c_char);
    if JS_IsException(val) != 0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = JS_ToUint32(ctx, &mut exec_argc, val);
    JS_FreeValue(ctx, val);
    if ret != 0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    /* arbitrary limit to avoid overflow */
    if exec_argc < 1 as libc::c_int as libc::c_uint ||
           exec_argc > 65535 as libc::c_int as libc::c_uint {
        return JS_ThrowTypeError(ctx,
                                 b"invalid number of arguments\x00" as
                                     *const u8 as *const libc::c_char)
    }
    exec_argv =
        js_mallocz(ctx,
                   (::std::mem::size_of::<*const libc::c_char>() as
                        libc::c_ulong).wrapping_mul(exec_argc.wrapping_add(1
                                                                               as
                                                                               libc::c_int
                                                                               as
                                                                               libc::c_uint)
                                                        as libc::c_ulong)) as
            *mut *const libc::c_char;
    if exec_argv.is_null() {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    i = 0 as libc::c_int as uint32_t;
    loop  {
        if !(i < exec_argc) { current_block = 17478428563724192186; break ; }
        val = JS_GetPropertyUint32(ctx, args, i);
        if JS_IsException(val) != 0 {
            current_block = 11555166796505691691;
            break ;
        }
        str = JS_ToCString(ctx, val);
        JS_FreeValue(ctx, val);
        if str.is_null() { current_block = 11555166796505691691; break ; }
        let ref mut fresh23 = *exec_argv.offset(i as isize);
        *fresh23 = str;
        i = i.wrapping_add(1)
    }
    match current_block {
        17478428563724192186 => {
            let ref mut fresh24 = *exec_argv.offset(exec_argc as isize);
            *fresh24 = 0 as *const libc::c_char;
            i = 0 as libc::c_int as uint32_t;
            while i < 3 as libc::c_int as libc::c_uint {
                std_fds[i as usize] = i as libc::c_int;
                i = i.wrapping_add(1)
            }
            /* get the options, if any */
            if argc >= 2 as libc::c_int {
                options = *argv.offset(1 as libc::c_int as isize);
                if get_bool_option(ctx, &mut block_flag, options,
                                   b"block\x00" as *const u8 as
                                       *const libc::c_char) != 0 {
                    current_block = 11555166796505691691;
                } else if get_bool_option(ctx, &mut use_path, options,
                                          b"usePath\x00" as *const u8 as
                                              *const libc::c_char) != 0 {
                    current_block = 11555166796505691691;
                } else {
                    val =
                        JS_GetPropertyStr(ctx, options,
                                          b"file\x00" as *const u8 as
                                              *const libc::c_char);
                    if JS_IsException(val) != 0 {
                        current_block = 11555166796505691691;
                    } else {
                        if JS_IsUndefined(val) == 0 {
                            file = JS_ToCString(ctx, val);
                            JS_FreeValue(ctx, val);
                            if file.is_null() {
                                current_block = 11555166796505691691;
                            } else { current_block = 7746103178988627676; }
                        } else { current_block = 7746103178988627676; }
                        match current_block {
                            11555166796505691691 => { }
                            _ => {
                                val =
                                    JS_GetPropertyStr(ctx, options,
                                                      b"cwd\x00" as *const u8
                                                          as
                                                          *const libc::c_char);
                                if JS_IsException(val) != 0 {
                                    current_block = 11555166796505691691;
                                } else {
                                    if JS_IsUndefined(val) == 0 {
                                        cwd = JS_ToCString(ctx, val);
                                        JS_FreeValue(ctx, val);
                                        if cwd.is_null() {
                                            current_block =
                                                11555166796505691691;
                                        } else {
                                            current_block =
                                                6717214610478484138;
                                        }
                                    } else {
                                        current_block = 6717214610478484138;
                                    }
                                    match current_block {
                                        11555166796505691691 => { }
                                        _ =>
                                        /* stdin/stdout/stderr handles */
                                        {
                                            i = 0 as libc::c_int as uint32_t;
                                            loop  {
                                                if !(i <
                                                         3 as libc::c_int as
                                                             libc::c_uint) {
                                                    current_block =
                                                        10753070352654377903;
                                                    break ;
                                                }
                                                val =
                                                    JS_GetPropertyStr(ctx,
                                                                      options,
                                                                      std_name[i
                                                                                   as
                                                                                   usize]);
                                                if JS_IsException(val) != 0 {
                                                    current_block =
                                                        11555166796505691691;
                                                    break ;
                                                }
                                                if JS_IsUndefined(val) == 0 {
                                                    let mut fd: libc::c_int =
                                                        0;
                                                    ret =
                                                        JS_ToInt32(ctx,
                                                                   &mut fd,
                                                                   val);
                                                    JS_FreeValue(ctx, val);
                                                    if ret != 0 {
                                                        current_block =
                                                            11555166796505691691;
                                                        break ;
                                                    }
                                                    std_fds[i as usize] = fd
                                                }
                                                i = i.wrapping_add(1)
                                            }
                                            match current_block {
                                                11555166796505691691 => { }
                                                _ => {
                                                    val =
                                                        JS_GetPropertyStr(ctx,
                                                                          options,
                                                                          b"env\x00"
                                                                              as
                                                                              *const u8
                                                                              as
                                                                              *const libc::c_char);
                                                    if JS_IsException(val) !=
                                                           0 {
                                                        current_block =
                                                            11555166796505691691;
                                                    } else {
                                                        if JS_IsUndefined(val)
                                                               == 0 {
                                                            envp =
                                                                build_envp(ctx,
                                                                           val);
                                                            JS_FreeValue(ctx,
                                                                         val);
                                                            if envp.is_null()
                                                               {
                                                                current_block
                                                                    =
                                                                    11555166796505691691;
                                                            } else {
                                                                current_block
                                                                    =
                                                                    3392087639489470149;
                                                            }
                                                        } else {
                                                            current_block =
                                                                3392087639489470149;
                                                        }
                                                        match current_block {
                                                            11555166796505691691
                                                            => {
                                                            }
                                                            _ => {
                                                                val =
                                                                    JS_GetPropertyStr(ctx,
                                                                                      options,
                                                                                      b"uid\x00"
                                                                                          as
                                                                                          *const u8
                                                                                          as
                                                                                          *const libc::c_char);
                                                                if JS_IsException(val)
                                                                       != 0 {
                                                                    current_block
                                                                        =
                                                                        11555166796505691691;
                                                                } else {
                                                                    if JS_IsUndefined(val)
                                                                           ==
                                                                           0 {
                                                                        ret =
                                                                            JS_ToUint32(ctx,
                                                                                        &mut uid,
                                                                                        val);
                                                                        JS_FreeValue(ctx,
                                                                                     val);
                                                                        if ret
                                                                               !=
                                                                               0
                                                                           {
                                                                            current_block
                                                                                =
                                                                                11555166796505691691;
                                                                        } else {
                                                                            current_block
                                                                                =
                                                                                6281126495347172768;
                                                                        }
                                                                    } else {
                                                                        current_block
                                                                            =
                                                                            6281126495347172768;
                                                                    }
                                                                    match current_block
                                                                        {
                                                                        11555166796505691691
                                                                        => {
                                                                        }
                                                                        _ => {
                                                                            val
                                                                                =
                                                                                JS_GetPropertyStr(ctx,
                                                                                                  options,
                                                                                                  b"gid\x00"
                                                                                                      as
                                                                                                      *const u8
                                                                                                      as
                                                                                                      *const libc::c_char);
                                                                            if JS_IsException(val)
                                                                                   !=
                                                                                   0
                                                                               {
                                                                                current_block
                                                                                    =
                                                                                    11555166796505691691;
                                                                            } else if JS_IsUndefined(val)
                                                                                          ==
                                                                                          0
                                                                             {
                                                                                ret
                                                                                    =
                                                                                    JS_ToUint32(ctx,
                                                                                                &mut gid,
                                                                                                val);
                                                                                JS_FreeValue(ctx,
                                                                                             val);
                                                                                if ret
                                                                                       !=
                                                                                       0
                                                                                   {
                                                                                    current_block
                                                                                        =
                                                                                        11555166796505691691;
                                                                                } else {
                                                                                    current_block
                                                                                        =
                                                                                        919954187481050311;
                                                                                }
                                                                            } else {
                                                                                current_block
                                                                                    =
                                                                                    919954187481050311;
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } else { current_block = 919954187481050311; }
            match current_block {
                11555166796505691691 => { }
                _ => {
                    pid = fork();
                    if pid < 0 as libc::c_int {
                        JS_ThrowTypeError(ctx,
                                          b"fork error\x00" as *const u8 as
                                              *const libc::c_char);
                        current_block = 11555166796505691691;
                    } else {
                        if pid == 0 as libc::c_int {
                            /* child */
                            let mut fd_max: libc::c_int =
                                sysconf(5 as libc::c_int) as libc::c_int;
                            /* remap the stdin/stdout/stderr handles if necessary */
                            i = 0 as libc::c_int as uint32_t;
                            while i < 3 as libc::c_int as libc::c_uint {
                                if std_fds[i as usize] as libc::c_uint != i {
                                    if dup2(std_fds[i as usize],
                                            i as libc::c_int) <
                                           0 as libc::c_int {
                                        _exit(127 as libc::c_int);
                                    }
                                }
                                i = i.wrapping_add(1)
                            }
                            i = 3 as libc::c_int as uint32_t;
                            while i < fd_max as libc::c_uint {
                                close(i as libc::c_int);
                                i = i.wrapping_add(1)
                            }
                            if !cwd.is_null() {
                                if chdir(cwd) < 0 as libc::c_int {
                                    _exit(127 as libc::c_int);
                                }
                            }
                            if uid != -(1 as libc::c_int) as libc::c_uint {
                                if setuid(uid) < 0 as libc::c_int {
                                    _exit(127 as libc::c_int);
                                }
                            }
                            if gid != -(1 as libc::c_int) as libc::c_uint {
                                if setgid(gid) < 0 as libc::c_int {
                                    _exit(127 as libc::c_int);
                                }
                            }
                            if file.is_null() {
                                file =
                                    *exec_argv.offset(0 as libc::c_int as
                                                          isize)
                            }
                            if use_path != 0 {
                                ret =
                                    my_execvpe(file,
                                               exec_argv as
                                                   *mut *mut libc::c_char,
                                               envp)
                            } else {
                                ret =
                                    execve(file,
                                           exec_argv as
                                               *mut *mut libc::c_char, envp)
                            }
                            _exit(127 as libc::c_int);
                        }
                        /* parent */
                        if block_flag != 0 {
                            loop  {
                                ret =
                                    waitpid(pid, &mut status,
                                            0 as libc::c_int);
                                if !(ret == pid) { continue ; }
                                if *(&mut status as *mut libc::c_int) &
                                       0o177 as libc::c_int ==
                                       0 as libc::c_int {
                                    ret =
                                        *(&mut status as *mut libc::c_int) >>
                                            8 as libc::c_int &
                                            0xff as libc::c_int;
                                    break ;
                                } else {
                                    if !(*(&mut status as *mut libc::c_int) &
                                             0o177 as libc::c_int !=
                                             0o177 as libc::c_int &&
                                             *(&mut status as
                                                   *mut libc::c_int) &
                                                 0o177 as libc::c_int !=
                                                 0 as libc::c_int) {
                                        continue ;
                                    }
                                    ret =
                                        -(*(&mut status as *mut libc::c_int) &
                                              0o177 as libc::c_int);
                                    break ;
                                }
                            }
                        } else { ret = pid }
                        ret_val = JS_NewInt32(ctx, ret);
                        current_block = 9063217057728162195;
                    }
                }
            }
        }
        _ => { }
    }
    match current_block {
        11555166796505691691 => {
            ret_val =
                {
                    let mut init =
                        JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                tag:
                                    JS_TAG_EXCEPTION as libc::c_int as
                                        int64_t,};
                    init
                }
        }
        _ => { }
    }
    JS_FreeCString(ctx, file);
    JS_FreeCString(ctx, cwd);
    i = 0 as libc::c_int as uint32_t;
    while i < exec_argc {
        JS_FreeCString(ctx, *exec_argv.offset(i as isize));
        i = i.wrapping_add(1)
    }
    js_free(ctx, exec_argv as *mut libc::c_void);
    if envp != *_NSGetEnviron() {
        let mut p: *mut *mut libc::c_char = 0 as *mut *mut libc::c_char;
        p = envp;
        while !(*p).is_null() {
            js_free(ctx, *p as *mut libc::c_void);
            p = p.offset(1)
        }
        js_free(ctx, envp as *mut libc::c_void);
    }
    return ret_val;
}
/* waitpid(pid, block) -> [pid, status] */
unsafe extern "C" fn js_os_waitpid(mut ctx: *mut JSContext,
                                   mut this_val: JSValue,
                                   mut argc: libc::c_int,
                                   mut argv: *mut JSValue) -> JSValue {
    let mut pid: libc::c_int = 0;
    let mut status: libc::c_int = 0;
    let mut options: libc::c_int = 0;
    let mut ret: libc::c_int = 0;
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    if JS_ToInt32(ctx, &mut pid, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToInt32(ctx, &mut options, *argv.offset(1 as libc::c_int as isize))
           != 0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = waitpid(pid, &mut status, options);
    if ret < 0 as libc::c_int { ret = -*__error(); status = 0 as libc::c_int }
    obj = JS_NewArray(ctx);
    if JS_IsException(obj) != 0 { return obj }
    JS_DefinePropertyValueUint32(ctx, obj, 0 as libc::c_int as uint32_t,
                                 JS_NewInt32(ctx, ret),
                                 (1 as libc::c_int) << 0 as libc::c_int |
                                     (1 as libc::c_int) << 1 as libc::c_int |
                                     (1 as libc::c_int) << 2 as libc::c_int);
    JS_DefinePropertyValueUint32(ctx, obj, 1 as libc::c_int as uint32_t,
                                 JS_NewInt32(ctx, status),
                                 (1 as libc::c_int) << 0 as libc::c_int |
                                     (1 as libc::c_int) << 1 as libc::c_int |
                                     (1 as libc::c_int) << 2 as libc::c_int);
    return obj;
}
/* pipe() -> [read_fd, write_fd] or null if error */
unsafe extern "C" fn js_os_pipe(mut ctx: *mut JSContext,
                                mut this_val: JSValue, mut argc: libc::c_int,
                                mut argv: *mut JSValue) -> JSValue {
    let mut pipe_fds: [libc::c_int; 2] = [0; 2];
    let mut ret: libc::c_int = 0;
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    ret = pipe(pipe_fds.as_mut_ptr());
    if ret < 0 as libc::c_int {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag: JS_TAG_NULL as libc::c_int as int64_t,};
                   init
               }
    }
    obj = JS_NewArray(ctx);
    if JS_IsException(obj) != 0 { return obj }
    JS_DefinePropertyValueUint32(ctx, obj, 0 as libc::c_int as uint32_t,
                                 JS_NewInt32(ctx,
                                             pipe_fds[0 as libc::c_int as
                                                          usize]),
                                 (1 as libc::c_int) << 0 as libc::c_int |
                                     (1 as libc::c_int) << 1 as libc::c_int |
                                     (1 as libc::c_int) << 2 as libc::c_int);
    JS_DefinePropertyValueUint32(ctx, obj, 1 as libc::c_int as uint32_t,
                                 JS_NewInt32(ctx,
                                             pipe_fds[1 as libc::c_int as
                                                          usize]),
                                 (1 as libc::c_int) << 0 as libc::c_int |
                                     (1 as libc::c_int) << 1 as libc::c_int |
                                     (1 as libc::c_int) << 2 as libc::c_int);
    return obj;
}
/* kill(pid, sig) */
unsafe extern "C" fn js_os_kill(mut ctx: *mut JSContext,
                                mut this_val: JSValue, mut argc: libc::c_int,
                                mut argv: *mut JSValue) -> JSValue {
    let mut pid: libc::c_int = 0;
    let mut sig: libc::c_int = 0;
    let mut ret: libc::c_int = 0;
    if JS_ToInt32(ctx, &mut pid, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToInt32(ctx, &mut sig, *argv.offset(1 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = js_get_errno(kill(pid, sig) as ssize_t) as libc::c_int;
    return JS_NewInt32(ctx, ret);
}
/* sleep(delay_ms) */
unsafe extern "C" fn js_os_sleep(mut ctx: *mut JSContext,
                                 mut this_val: JSValue, mut argc: libc::c_int,
                                 mut argv: *mut JSValue) -> JSValue {
    let mut delay: int64_t = 0;
    let mut ts: timespec = timespec{tv_sec: 0, tv_nsec: 0,};
    let mut ret: libc::c_int = 0;
    if JS_ToInt64(ctx, &mut delay, *argv.offset(0 as libc::c_int as isize)) !=
           0 {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ts.tv_sec =
        (delay / 1000 as libc::c_int as libc::c_longlong) as __darwin_time_t;
    ts.tv_nsec =
        (delay % 1000 as libc::c_int as libc::c_longlong *
             1000000 as libc::c_int as libc::c_longlong) as libc::c_long;
    ret =
        js_get_errno(nanosleep(&mut ts, 0 as *mut timespec) as ssize_t) as
            libc::c_int;
    return JS_NewInt32(ctx, ret);
}
/* dup(fd) */
unsafe extern "C" fn js_os_dup(mut ctx: *mut JSContext, mut this_val: JSValue,
                               mut argc: libc::c_int, mut argv: *mut JSValue)
 -> JSValue {
    let mut fd: libc::c_int = 0;
    let mut ret: libc::c_int = 0;
    if JS_ToInt32(ctx, &mut fd, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = js_get_errno(dup(fd) as ssize_t) as libc::c_int;
    return JS_NewInt32(ctx, ret);
}
/* dup2(fd) */
unsafe extern "C" fn js_os_dup2(mut ctx: *mut JSContext,
                                mut this_val: JSValue, mut argc: libc::c_int,
                                mut argv: *mut JSValue) -> JSValue {
    let mut fd: libc::c_int = 0;
    let mut fd2: libc::c_int = 0;
    let mut ret: libc::c_int = 0;
    if JS_ToInt32(ctx, &mut fd, *argv.offset(0 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    if JS_ToInt32(ctx, &mut fd2, *argv.offset(1 as libc::c_int as isize)) != 0
       {
        return {
                   let mut init =
                       JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                               tag:
                                   JS_TAG_EXCEPTION as libc::c_int as
                                       int64_t,};
                   init
               }
    }
    ret = js_get_errno(dup2(fd, fd2) as ssize_t) as libc::c_int;
    return JS_NewInt32(ctx, ret);
}
/* !_WIN32 */
/* USE_WORKER */
static mut js_os_funcs: [JSCFunctionListEntry; 68] =
    unsafe {
        [{
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"open\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_open
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"O_RDONLY\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"O_WRONLY\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0x1 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"O_RDWR\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0x2 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"O_APPEND\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0x8 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"O_CREAT\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0x200 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"O_EXCL\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0x800 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"O_TRUNC\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0x400 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"close\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_close
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"seek\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            3
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_seek
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"read\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            4
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_os_read_write
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"write\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 1 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            4
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_os_read_write
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"isatty\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_isatty
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"ttyGetWinSize\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_ttyGetWinSize
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"ttySetRaw\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_ttySetRaw
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"remove\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_remove
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"rename\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_rename
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"setReadHandler\x00" as *const u8
                                              as *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_os_setReadHandler
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"setWriteHandler\x00" as *const u8
                                              as *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 1 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_os_setReadHandler
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"signal\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_signal
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGINT\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               2 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGABRT\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               6 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGFPE\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               8 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGILL\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               4 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGSEGV\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               11 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGTERM\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               15 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGQUIT\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               3 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGPIPE\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               13 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGALRM\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               14 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGUSR1\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               30 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGUSR2\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               31 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGCHLD\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               20 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGCONT\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               19 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGSTOP\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               17 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGTSTP\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               18 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGTTIN\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               21 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SIGTTOU\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               22 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"setTimeout\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_setTimeout
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"clearTimeout\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_clearTimeout
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"platform\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags: 0 as libc::c_int as uint8_t,
                                      def_type: 3 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{str_0:
                                                               b"darwin\x00"
                                                                   as
                                                                   *const u8
                                                                   as
                                                                   *const libc::c_char,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"getcwd\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_getcwd
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"chdir\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_chdir
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"mkdir\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_mkdir
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"readdir\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_readdir
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"S_IFMT\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0o170000 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"S_IFIFO\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0o10000 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"S_IFCHR\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0o20000 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"S_IFDIR\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0o40000 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"S_IFBLK\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0o60000 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"S_IFREG\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0o100000 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"S_IFSOCK\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0o140000 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"S_IFLNK\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0o120000 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"S_ISGID\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0o2000 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"S_ISUID\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0o4000 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"stat\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_os_stat
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"utimes\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            3
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_utimes
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"lstat\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 1 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_os_stat
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"realpath\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_realpath
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"symlink\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_symlink
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"readlink\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_readlink
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"exec\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_exec
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"waitpid\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_waitpid
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"WNOHANG\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0x1 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"pipe\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_pipe
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"kill\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_kill
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"sleep\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_sleep
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"dup\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_dup
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"dup2\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_os_dup2
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         }]
    };
unsafe extern "C" fn js_os_init(mut ctx: *mut JSContext,
                                mut m: *mut JSModuleDef) -> libc::c_int {
    os_poll_func =
        Some(js_os_poll as
                 unsafe extern "C" fn(_: *mut JSContext) -> libc::c_int);
    /* OSTimer class */
    JS_NewClassID(&mut js_os_timer_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_os_timer_class_id,
                &mut js_os_timer_class);
    /* USE_WORKER */
    return JS_SetModuleExportList(ctx, m, js_os_funcs.as_ptr(),
                                  (::std::mem::size_of::<[JSCFunctionListEntry; 68]>()
                                       as
                                       libc::c_ulong).wrapping_div(::std::mem::size_of::<JSCFunctionListEntry>()
                                                                       as
                                                                       libc::c_ulong)
                                      as libc::c_int);
}
#[no_mangle]
pub unsafe extern "C" fn js_init_module_os(mut ctx: *mut JSContext,
                                           mut module_name:
                                               *const libc::c_char)
 -> *mut JSModuleDef {
    let mut m: *mut JSModuleDef = 0 as *mut JSModuleDef;
    m =
        JS_NewCModule(ctx, module_name,
                      Some(js_os_init as
                               unsafe extern "C" fn(_: *mut JSContext,
                                                    _: *mut JSModuleDef)
                                   -> libc::c_int));
    if m.is_null() { return 0 as *mut JSModuleDef }
    JS_AddModuleExportList(ctx, m, js_os_funcs.as_ptr(),
                           (::std::mem::size_of::<[JSCFunctionListEntry; 68]>()
                                as
                                libc::c_ulong).wrapping_div(::std::mem::size_of::<JSCFunctionListEntry>()
                                                                as
                                                                libc::c_ulong)
                               as libc::c_int);
    return m;
}
/* *********************************************************/
unsafe extern "C" fn js_print(mut ctx: *mut JSContext, mut this_val: JSValue,
                              mut argc: libc::c_int, mut argv: *mut JSValue)
 -> JSValue {
    let mut i: libc::c_int = 0;
    let mut str: *const libc::c_char = 0 as *const libc::c_char;
    let mut len: size_t = 0;
    i = 0 as libc::c_int;
    while i < argc {
        if i != 0 as libc::c_int { putchar(' ' as i32); }
        str = JS_ToCStringLen(ctx, &mut len, *argv.offset(i as isize));
        if str.is_null() {
            return {
                       let mut init =
                           JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                                   tag:
                                       JS_TAG_EXCEPTION as libc::c_int as
                                           int64_t,};
                       init
                   }
        }
        fwrite(str as *const libc::c_void, 1 as libc::c_int as libc::c_ulong,
               len, __stdoutp);
        JS_FreeCString(ctx, str);
        i += 1
    }
    putchar('\n' as i32);
    return {
               let mut init =
                   JSValue{u: JSValueUnion{int32: 0 as libc::c_int,},
                           tag: JS_TAG_UNDEFINED as libc::c_int as int64_t,};
               init
           };
}
#[no_mangle]
pub unsafe extern "C" fn js_std_add_helpers(mut ctx: *mut JSContext,
                                            mut argc: libc::c_int,
                                            mut argv:
                                                *mut *mut libc::c_char) {
    let mut global_obj: JSValue =
        JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut console: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut args: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut i: libc::c_int = 0;
    /* XXX: should these global definitions be enumerable? */
    global_obj = JS_GetGlobalObject(ctx);
    console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console,
                      b"log\x00" as *const u8 as *const libc::c_char,
                      JS_NewCFunction(ctx,
                                      Some(js_print as
                                               unsafe extern "C" fn(_:
                                                                        *mut JSContext,
                                                                    _:
                                                                        JSValue,
                                                                    _:
                                                                        libc::c_int,
                                                                    _:
                                                                        *mut JSValue)
                                                   -> JSValue),
                                      b"log\x00" as *const u8 as
                                          *const libc::c_char,
                                      1 as libc::c_int));
    JS_SetPropertyStr(ctx, global_obj,
                      b"console\x00" as *const u8 as *const libc::c_char,
                      console);
    /* same methods as the mozilla JS shell */
    if argc >= 0 as libc::c_int {
        args = JS_NewArray(ctx);
        i = 0 as libc::c_int;
        while i < argc {
            JS_SetPropertyUint32(ctx, args, i as uint32_t,
                                 JS_NewString(ctx, *argv.offset(i as isize)));
            i += 1
        }
        JS_SetPropertyStr(ctx, global_obj,
                          b"scriptArgs\x00" as *const u8 as
                              *const libc::c_char, args);
    }
    JS_SetPropertyStr(ctx, global_obj,
                      b"print\x00" as *const u8 as *const libc::c_char,
                      JS_NewCFunction(ctx,
                                      Some(js_print as
                                               unsafe extern "C" fn(_:
                                                                        *mut JSContext,
                                                                    _:
                                                                        JSValue,
                                                                    _:
                                                                        libc::c_int,
                                                                    _:
                                                                        *mut JSValue)
                                                   -> JSValue),
                                      b"print\x00" as *const u8 as
                                          *const libc::c_char,
                                      1 as libc::c_int));
    JS_SetPropertyStr(ctx, global_obj,
                      b"__loadScript\x00" as *const u8 as *const libc::c_char,
                      JS_NewCFunction(ctx,
                                      Some(js_loadScript as
                                               unsafe extern "C" fn(_:
                                                                        *mut JSContext,
                                                                    _:
                                                                        JSValue,
                                                                    _:
                                                                        libc::c_int,
                                                                    _:
                                                                        *mut JSValue)
                                                   -> JSValue),
                                      b"__loadScript\x00" as *const u8 as
                                          *const libc::c_char,
                                      1 as libc::c_int));
    JS_FreeValue(ctx, global_obj);
}
#[no_mangle]
pub unsafe extern "C" fn js_std_init_handlers(mut rt: *mut JSRuntime) {
    let mut ts: *mut JSThreadState = 0 as *mut JSThreadState;
    ts =
        malloc(::std::mem::size_of::<JSThreadState>() as libc::c_ulong) as
            *mut JSThreadState;
    if ts.is_null() {
        fprintf(__stderrp,
                b"Could not allocate memory for the worker\x00" as *const u8
                    as *const libc::c_char);
        exit(1 as libc::c_int);
    }
    memset(ts as *mut libc::c_void, 0 as libc::c_int,
           ::std::mem::size_of::<JSThreadState>() as libc::c_ulong);
    init_list_head(&mut (*ts).os_rw_handlers);
    init_list_head(&mut (*ts).os_signal_handlers);
    init_list_head(&mut (*ts).os_timers);
    init_list_head(&mut (*ts).port_list);
    JS_SetRuntimeOpaque(rt, ts as *mut libc::c_void);
}
#[no_mangle]
pub unsafe extern "C" fn js_std_free_handlers(mut rt: *mut JSRuntime) {
    let mut ts: *mut JSThreadState =
        JS_GetRuntimeOpaque(rt) as *mut JSThreadState;
    let mut el: *mut list_head = 0 as *mut list_head;
    let mut el1: *mut list_head = 0 as *mut list_head;
    el = (*ts).os_rw_handlers.next;
    el1 = (*el).next;
    while el != &mut (*ts).os_rw_handlers as *mut list_head {
        let mut rh: *mut JSOSRWHandler =
            (el as
                 *mut uint8_t).offset(-(&mut (*(0 as *mut JSOSRWHandler)).link
                                            as *mut list_head as size_t as
                                            isize)) as *mut JSOSRWHandler;
        free_rw_handler(rt, rh);
        el = el1;
        el1 = (*el).next
    }
    el = (*ts).os_signal_handlers.next;
    el1 = (*el).next;
    while el != &mut (*ts).os_signal_handlers as *mut list_head {
        let mut sh: *mut JSOSSignalHandler =
            (el as
                 *mut uint8_t).offset(-(&mut (*(0 as
                                                    *mut JSOSSignalHandler)).link
                                            as *mut list_head as size_t as
                                            isize)) as *mut JSOSSignalHandler;
        free_sh(rt, sh);
        el = el1;
        el1 = (*el).next
    }
    el = (*ts).os_timers.next;
    el1 = (*el).next;
    while el != &mut (*ts).os_timers as *mut list_head {
        let mut th: *mut JSOSTimer =
            (el as
                 *mut uint8_t).offset(-(&mut (*(0 as *mut JSOSTimer)).link as
                                            *mut list_head as size_t as
                                            isize)) as *mut JSOSTimer;
        unlink_timer(rt, th);
        if (*th).has_object == 0 { free_timer(rt, th); }
        el = el1;
        el1 = (*el).next
    }
    free(ts as *mut libc::c_void);
    JS_SetRuntimeOpaque(rt, 0 as *mut libc::c_void);
    /* fail safe */
}
unsafe extern "C" fn js_dump_obj(mut ctx: *mut JSContext, mut f: *mut FILE,
                                 mut val: JSValue) {
    let mut str: *const libc::c_char = 0 as *const libc::c_char;
    str = JS_ToCString(ctx, val);
    if !str.is_null() {
        fprintf(f, b"%s\n\x00" as *const u8 as *const libc::c_char, str);
        JS_FreeCString(ctx, str);
    } else {
        fprintf(f, b"[exception]\n\x00" as *const u8 as *const libc::c_char);
    };
}
unsafe extern "C" fn js_std_dump_error1(mut ctx: *mut JSContext,
                                        mut exception_val: JSValue) {
    let mut val: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut is_error: libc::c_int = 0;
    is_error = JS_IsError(ctx, exception_val);
    js_dump_obj(ctx, __stderrp, exception_val);
    if is_error != 0 {
        val =
            JS_GetPropertyStr(ctx, exception_val,
                              b"stack\x00" as *const u8 as
                                  *const libc::c_char);
        if JS_IsUndefined(val) == 0 { js_dump_obj(ctx, __stderrp, val); }
        JS_FreeValue(ctx, val);
    };
}
#[no_mangle]
pub unsafe extern "C" fn js_std_dump_error(mut ctx: *mut JSContext) {
    let mut exception_val: JSValue =
        JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    exception_val = JS_GetException(ctx);
    js_std_dump_error1(ctx, exception_val);
    JS_FreeValue(ctx, exception_val);
}
#[no_mangle]
pub unsafe extern "C" fn js_std_promise_rejection_tracker(mut ctx:
                                                              *mut JSContext,
                                                          mut promise:
                                                              JSValue,
                                                          mut reason: JSValue,
                                                          mut is_handled:
                                                              libc::c_int,
                                                          mut opaque:
                                                              *mut libc::c_void) {
    if is_handled == 0 {
        fprintf(__stderrp,
                b"Possibly unhandled promise rejection: \x00" as *const u8 as
                    *const libc::c_char);
        js_std_dump_error1(ctx, reason);
    };
}
/* main loop which calls the user JS callbacks */
#[no_mangle]
pub unsafe extern "C" fn js_std_loop(mut ctx: *mut JSContext) {
    let mut ctx1: *mut JSContext = 0 as *mut JSContext;
    let mut err: libc::c_int = 0;
    loop  {
        loop 
             /* execute the pending jobs */
             {
            err = JS_ExecutePendingJob(JS_GetRuntime(ctx), &mut ctx1);
            if !(err <= 0 as libc::c_int) { continue ; }
            if err < 0 as libc::c_int { js_std_dump_error(ctx1); }
            break ;
        }
        if os_poll_func.is_none() ||
               os_poll_func.expect("non-null function pointer")(ctx) != 0 {
            break ;
        }
    };
}
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
pub unsafe extern "C" fn js_std_eval_binary(mut ctx: *mut JSContext,
                                            mut buf: *const uint8_t,
                                            mut buf_len: size_t,
                                            mut load_only: libc::c_int) {
    let mut current_block: u64;
    let mut obj: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    let mut val: JSValue = JSValue{u: JSValueUnion{int32: 0,}, tag: 0,};
    obj =
        JS_ReadObject(ctx, buf, buf_len,
                      (1 as libc::c_int) << 0 as libc::c_int);
    if !(JS_IsException(obj) != 0) {
        if load_only != 0 {
            if obj.tag as int32_t == JS_TAG_MODULE as libc::c_int {
                js_module_set_import_meta(ctx, obj, FALSE as libc::c_int,
                                          FALSE as libc::c_int);
            }
            current_block = 17833034027772472439;
        } else {
            if obj.tag as int32_t == JS_TAG_MODULE as libc::c_int {
                if JS_ResolveModule(ctx, obj) < 0 as libc::c_int {
                    JS_FreeValue(ctx, obj);
                    current_block = 5274085479655304947;
                } else {
                    js_module_set_import_meta(ctx, obj, FALSE as libc::c_int,
                                              TRUE as libc::c_int);
                    current_block = 7651349459974463963;
                }
            } else { current_block = 7651349459974463963; }
            match current_block {
                5274085479655304947 => { }
                _ => {
                    val = JS_EvalFunction(ctx, obj);
                    if JS_IsException(val) != 0 {
                        current_block = 5274085479655304947;
                    } else {
                        JS_FreeValue(ctx, val);
                        current_block = 17833034027772472439;
                    }
                }
            }
        }
        match current_block { 5274085479655304947 => { } _ => { return; } }
    }
    js_std_dump_error(ctx);
    exit(1 as libc::c_int);
}
unsafe extern "C" fn run_static_initializers() {
    js_std_funcs =
        [{
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"exit\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_exit
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"gc\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_gc
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"evalScript\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_evalScript
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"loadScript\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_loadScript
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"getenv\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_getenv
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"urlGet\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_urlGet
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"loadFile\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_loadFile
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"strerror\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_strerror
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"parseExtJSON\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_parseExtJSON
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"open\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_open
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"popen\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_popen
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"fdopen\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            2
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_fdopen
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"tmpfile\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            0
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_tmpfile
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"puts\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic_magic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic_magic:
                                                                                                                Some(js_std_file_puts
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"printf\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_printf
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"sprintf\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               1 as libc::c_int |
                                               (1 as libc::c_int) <<
                                                   0 as libc::c_int) as
                                              uint8_t,
                                      def_type: 0 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{func:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_22{length:
                                                                                            1
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cproto:
                                                                                            JS_CFUNC_generic
                                                                                                as
                                                                                                libc::c_int
                                                                                                as
                                                                                                uint8_t,
                                                                                        cfunc:
                                                                                            JSCFunctionType{generic:
                                                                                                                Some(js_std_sprintf
                                                                                                                         as
                                                                                                                         unsafe extern "C" fn(_:
                                                                                                                                                  *mut JSContext,
                                                                                                                                              _:
                                                                                                                                                  JSValue,
                                                                                                                                              _:
                                                                                                                                                  libc::c_int,
                                                                                                                                              _:
                                                                                                                                                  *mut JSValue)
                                                                                                                             ->
                                                                                                                                 JSValue),},};
                                                                   init
                                                               },},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SEEK_SET\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               0 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SEEK_CUR\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               1 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"SEEK_END\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 4 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{i32_0:
                                                               2 as
                                                                   libc::c_int,},};
             init
         },
         {
             let mut init =
                 JSCFunctionListEntry{name:
                                          b"Error\x00" as *const u8 as
                                              *const libc::c_char,
                                      prop_flags:
                                          ((1 as libc::c_int) <<
                                               0 as libc::c_int) as uint8_t,
                                      def_type: 8 as libc::c_int as uint8_t,
                                      magic: 0 as libc::c_int as int16_t,
                                      u:
                                          C2RustUnnamed_18{prop_list:
                                                               {
                                                                   let mut init =
                                                                       C2RustUnnamed_19{tab:
                                                                                            js_std_error_props.as_ptr(),
                                                                                        len:
                                                                                            (::std::mem::size_of::<[JSCFunctionListEntry; 11]>()
                                                                                                 as
                                                                                                 libc::c_ulong).wrapping_div(::std::mem::size_of::<JSCFunctionListEntry>()
                                                                                                                                 as
                                                                                                                                 libc::c_ulong)
                                                                                                as
                                                                                                libc::c_int,};
                                                                   init
                                                               },},};
             init
         }]
}
#[used]
#[cfg_attr(target_os = "linux", link_section = ".init_array")]
#[cfg_attr(target_os = "windows", link_section = ".CRT$XIB")]
#[cfg_attr(target_os = "macos", link_section = "__DATA,__mod_init_func")]
static INIT_ARRAY: [unsafe extern "C" fn(); 1] = [run_static_initializers];
