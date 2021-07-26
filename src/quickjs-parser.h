#ifndef QUICKJS_QUICKJS_PARSER_H
#define QUICKJS_QUICKJS_PARSER_H
enum {
    TOK_NUMBER = -128,
    TOK_STRING,
    TOK_TEMPLATE,
    TOK_IDENT,
    TOK_REGEXP,
    /* warning: order matters (see js_parse_assign_expr) */
    TOK_MUL_ASSIGN,
    TOK_DIV_ASSIGN,
    TOK_MOD_ASSIGN,
    TOK_PLUS_ASSIGN,
    TOK_MINUS_ASSIGN,
    TOK_SHL_ASSIGN,
    TOK_SAR_ASSIGN,
    TOK_SHR_ASSIGN,
    TOK_AND_ASSIGN,
    TOK_XOR_ASSIGN,
    TOK_OR_ASSIGN,
#ifdef CONFIG_BIGNUM
    TOK_MATH_POW_ASSIGN,
#endif
    TOK_POW_ASSIGN,
    TOK_DEC,
    TOK_INC,
    TOK_SHL,
    TOK_SAR,
    TOK_SHR,
    TOK_LT,
    TOK_LTE,
    TOK_GT,
    TOK_GTE,
    TOK_EQ,
    TOK_STRICT_EQ,
    TOK_NEQ,
    TOK_STRICT_NEQ,
    TOK_LAND,
    TOK_LOR,
#ifdef CONFIG_BIGNUM
    TOK_MATH_POW,
#endif
    TOK_POW,
    TOK_ARROW,
    TOK_ELLIPSIS,
    TOK_DOUBLE_QUESTION_MARK,
    TOK_QUESTION_MARK_DOT,
    TOK_ERROR,
    TOK_PRIVATE_NAME,
    TOK_EOF,
    /* keywords: WARNING: same order as atoms */
    TOK_NULL, /* must be first */
    TOK_FALSE,
    TOK_TRUE,
    TOK_IF,
    TOK_ELSE,
    TOK_RETURN,
    TOK_VAR,
    TOK_THIS,
    TOK_DELETE,
    TOK_VOID,
    TOK_TYPEOF,
    TOK_NEW,
    TOK_IN,
    TOK_INSTANCEOF,
    TOK_DO,
    TOK_WHILE,
    TOK_FOR,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_SWITCH,
    TOK_CASE,
    TOK_DEFAULT,
    TOK_THROW,
    TOK_TRY,
    TOK_CATCH,
    TOK_FINALLY,
    TOK_FUNCTION,
    TOK_DEBUGGER,
    TOK_WITH,
    /* FutureReservedWord */
    TOK_CLASS,
    TOK_CONST,
    TOK_ENUM,
    TOK_EXPORT,
    TOK_EXTENDS,
    TOK_IMPORT,
    TOK_SUPER,
    /* FutureReservedWords when parsing strict mode code */
    TOK_IMPLEMENTS,
    TOK_INTERFACE,
    TOK_LET,
    TOK_PACKAGE,
    TOK_PRIVATE,
    TOK_PROTECTED,
    TOK_PUBLIC,
    TOK_STATIC,
    TOK_YIELD,
    TOK_AWAIT, /* must be last */
    TOK_OF,     /* only used for js_parse_skip_parens_token() */
};

#define TOK_FIRST_KEYWORD   TOK_NULL
#define TOK_LAST_KEYWORD    TOK_AWAIT

/* unicode code points */
#define CP_NBSP 0x00a0
#define CP_BOM  0xfeff

#define CP_LS   0x2028
#define CP_PS   0x2029

typedef struct BlockEnv {
    struct BlockEnv *prev;
    JSAtom label_name; /* JS_ATOM_NULL if none */
    int label_break; /* -1 if none */
    int label_cont; /* -1 if none */
    int drop_count; /* number of stack elements to drop */
    int label_finally; /* -1 if none */
    int scope_level;
    int has_iterator;
} BlockEnv;

typedef struct JSHoistedDef {
    int cpool_idx; /* -1 means variable global definition */
    uint8_t force_init : 1; /* initialize to undefined */
    uint8_t is_lexical : 1; /* global let/const definition */
    uint8_t is_const   : 1; /* const definition */
    int var_idx;   /* function object index if cpool_idx >= 0 */
    int scope_level;    /* scope of definition */
    JSAtom var_name;  /* variable name if cpool_idx < 0 */
} JSHoistedDef;

typedef struct RelocEntry {
    struct RelocEntry *next;
    uint32_t addr; /* address to patch */
    int size;   /* address size: 1, 2 or 4 bytes */
} RelocEntry;

typedef struct JumpSlot {
    int op;
    int size;
    int pos;
    int label;
} JumpSlot;

typedef struct LabelSlot {
    int ref_count;
    int pos;    /* phase 1 address, -1 means not resolved yet */
    int pos2;   /* phase 2 address, -1 means not resolved yet */
    int addr;   /* phase 3 address, -1 means not resolved yet */
    RelocEntry *first_reloc;
} LabelSlot;

typedef struct LineNumberSlot {
    uint32_t pc;
    int line_num;
} LineNumberSlot;

typedef enum JSParseFunctionEnum {
    JS_PARSE_FUNC_STATEMENT,
    JS_PARSE_FUNC_VAR,
    JS_PARSE_FUNC_EXPR,
    JS_PARSE_FUNC_ARROW,
    JS_PARSE_FUNC_GETTER,
    JS_PARSE_FUNC_SETTER,
    JS_PARSE_FUNC_METHOD,
    JS_PARSE_FUNC_CLASS_CONSTRUCTOR,
    JS_PARSE_FUNC_DERIVED_CLASS_CONSTRUCTOR,
} JSParseFunctionEnum;

typedef enum JSParseExportEnum {
    JS_PARSE_EXPORT_NONE,
    JS_PARSE_EXPORT_NAMED,
    JS_PARSE_EXPORT_DEFAULT,
} JSParseExportEnum;

typedef struct JSFunctionDef {
    JSContext *ctx;
    struct JSFunctionDef *parent;
    int parent_cpool_idx; /* index in the constant pool of the parent
                             or -1 if none */
    int parent_scope_level; /* scope level in parent at point of definition */
    struct list_head child_list; /* list of JSFunctionDef.link */
    struct list_head link;

    BOOL is_eval; /* TRUE if eval code */
    int eval_type; /* only valid if is_eval = TRUE */
    BOOL is_global_var; /* TRUE if variables are not defined locally:
                           eval global, eval module or non strict eval */
    BOOL is_func_expr; /* TRUE if function expression */
    BOOL has_home_object; /* TRUE if the home object is available */
    BOOL has_prototype; /* true if a prototype field is necessary */
    BOOL has_simple_parameter_list;
    BOOL has_use_strict; /* to reject directive in special cases */
    BOOL has_eval_call; /* true if the function contains a call to eval() */
    BOOL has_arguments_binding; /* true if the 'arguments' binding is
                                   available in the function */
    BOOL has_this_binding; /* true if the 'this' and new.target binding are
                              available in the function */
    BOOL new_target_allowed; /* true if the 'new.target' does not
                                throw a syntax error */
    BOOL super_call_allowed; /* true if super() is allowed */
    BOOL super_allowed; /* true if super. or super[] is allowed */
    BOOL arguments_allowed; /* true if the 'arguments' identifier is allowed */
    BOOL is_derived_class_constructor;
    BOOL in_function_body;
    BOOL backtrace_barrier;
    JSFunctionKindEnum func_kind : 8;
    JSParseFunctionEnum func_type : 8;
    uint8_t js_mode; /* bitmap of JS_MODE_x */
    JSAtom func_name; /* JS_ATOM_NULL if no name */

    JSVarDef *vars;
    int var_size; /* allocated size for vars[] */
    int var_count;
    JSVarDef *args;
    int arg_size; /* allocated size for args[] */
    int arg_count; /* number of arguments */
    int defined_arg_count;
    int var_object_idx; /* -1 if none */
    int arguments_var_idx; /* -1 if none */
    int func_var_idx; /* variable containing the current function (-1
                         if none, only used if is_func_expr is true) */
    int eval_ret_idx; /* variable containing the return value of the eval, -1 if none */
    int this_var_idx; /* variable containg the 'this' value, -1 if none */
    int new_target_var_idx; /* variable containg the 'new.target' value, -1 if none */
    int this_active_func_var_idx; /* variable containg the 'this.active_func' value, -1 if none */
    int home_object_var_idx;
    BOOL need_home_object;

    int scope_level;    /* index into fd->scopes if the current lexical scope */
    int scope_first;    /* index into vd->vars of first lexically scoped variable */
    int scope_size;     /* allocated size of fd->scopes array */
    int scope_count;    /* number of entries used in the fd->scopes array */
    JSVarScope *scopes;
    JSVarScope def_scope_array[4];

    int hoisted_def_count;
    int hoisted_def_size;
    JSHoistedDef *hoisted_def;

    DynBuf byte_code;
    int last_opcode_pos; /* -1 if no last opcode */
    int last_opcode_line_num;
    BOOL use_short_opcodes; /* true if short opcodes are used in byte_code */

    LabelSlot *label_slots;
    int label_size; /* allocated size for label_slots[] */
    int label_count;
    BlockEnv *top_break; /* break/continue label stack */

    /* constant pool (strings, functions, numbers) */
    JSValue *cpool;
    int cpool_count;
    int cpool_size;

    /* list of variables in the closure */
    int closure_var_count;
    int closure_var_size;
    JSClosureVar *closure_var;

    JumpSlot *jump_slots;
    int jump_size;
    int jump_count;

    LineNumberSlot *line_number_slots;
    int line_number_size;
    int line_number_count;
    int line_number_last;
    int line_number_last_pc;

    /* pc2line table */
    JSAtom filename;
    int line_num;
    DynBuf pc2line;

    char *source;  /* raw source, utf-8 encoded */
    int source_len;

    JSModuleDef *module; /* != NULL when parsing a module */
} JSFunctionDef;

typedef struct JSToken {
    int val;
    int line_num;   /* line number of token start */
    const uint8_t *ptr;
    union {
        struct {
            JSValue str;
            int sep;
        } str;
        struct {
            JSValue val;
#ifdef CONFIG_BIGNUM
            slimb_t exponent; /* may be != 0 only if val is a float */
#endif
        } num;
        struct {
            JSAtom atom;
            BOOL has_escape;
            BOOL is_reserved;
        } ident;
        struct {
            JSValue body;
            JSValue flags;
        } regexp;
    } u;
} JSToken;

typedef struct JSParseState {
    JSContext *ctx;
    int last_line_num;  /* line number of last token */
    int line_num;       /* line number of current offset */
    const char *filename;
    JSToken token;
    BOOL got_lf; /* true if got line feed before the current token */
    const uint8_t *last_ptr;
    const uint8_t *buf_ptr;
    const uint8_t *buf_end;

    /* current function code */
    JSFunctionDef *cur_func;
    BOOL is_module; /* parsing a module */
    BOOL allow_html_comments;
    BOOL ext_json; /* true if accepting JSON superset */
} JSParseState;

typedef struct JSOpCode {
#ifdef DUMP_BYTECODE
    const char *name;
#endif
    uint8_t size; /* in bytes */
    /* the opcodes remove n_pop items from the top of the stack, then
       pushes n_push items */
    uint8_t n_pop;
    uint8_t n_push;
    uint8_t fmt;
} JSOpCode;

static const JSOpCode opcode_info[OP_COUNT + (OP_TEMP_END - OP_TEMP_START)] = {
#define FMT(f)
#ifdef DUMP_BYTECODE
#define DEF(id, size, n_pop, n_push, f) { #id, size, n_pop, n_push, OP_FMT_ ## f },
#else
#define DEF(id, size, n_pop, n_push, f) { size, n_pop, n_push, OP_FMT_ ## f },
#endif
#include "quickjs-opcode.h"
#undef DEF
#undef FMT
};

#if SHORT_OPCODES
/* After the final compilation pass, short opcodes are used. Their
   opcodes overlap with the temporary opcodes which cannot appear in
   the final bytecode. Their description is after the temporary
   opcodes in opcode_info[]. */
#define short_opcode_info(op)           \
    opcode_info[(op) >= OP_TEMP_START ? \
                (op) + (OP_TEMP_END - OP_TEMP_START) : (op)]
#else
#define short_opcode_info(op) opcode_info[op]
#endif

static __exception int next_token(JSParseState *s);

static void free_token(JSParseState *s, JSToken *token)
{
    switch(token->val) {
#ifdef CONFIG_BIGNUM
        case TOK_NUMBER:
        JS_FreeValue(s->ctx, token->u.num.val);
        break;
#endif
        case TOK_STRING:
        case TOK_TEMPLATE:
            JS_FreeValue(s->ctx, token->u.str.str);
            break;
        case TOK_REGEXP:
            JS_FreeValue(s->ctx, token->u.regexp.body);
            JS_FreeValue(s->ctx, token->u.regexp.flags);
            break;
        case TOK_IDENT:
        case TOK_PRIVATE_NAME:
            JS_FreeAtom(s->ctx, token->u.ident.atom);
            break;
        default:
            if (token->val >= TOK_FIRST_KEYWORD && token->val <= TOK_LAST_KEYWORD)
                JS_FreeAtom(s->ctx, token->u.ident.atom);
            break;
    }
}

static void __maybe_unused dump_token(JSParseState *s,
const JSToken *token)
{
switch(token->val) {
case TOK_NUMBER:
{
double d;
JS_ToFloat64(s->ctx, &d, token->u.num.val);  /* no exception possible */
printf("number: %.14g\n", d);
}
break;
case TOK_IDENT:
dump_atom:
{
char buf[ATOM_GET_STR_BUF_SIZE];
printf("ident: '%s'\n",
JS_AtomGetStr(s->ctx, buf, sizeof(buf), token->u.ident.atom));
}
break;
case TOK_STRING:
{
const char *str;
/* XXX: quote the string */
str = JS_ToCString(s->ctx, token->u.str.str);
printf("string: '%s'\n", str);
JS_FreeCString(s->ctx, str);
}
break;
case TOK_TEMPLATE:
{
const char *str;
str = JS_ToCString(s->ctx, token->u.str.str);
printf("template: `%s`\n", str);
JS_FreeCString(s->ctx, str);
}
break;
case TOK_REGEXP:
{
const char *str, *str2;
str = JS_ToCString(s->ctx, token->u.regexp.body);
str2 = JS_ToCString(s->ctx, token->u.regexp.flags);
printf("regexp: '%s' '%s'\n", str, str2);
JS_FreeCString(s->ctx, str);
JS_FreeCString(s->ctx, str2);
}
break;
case TOK_EOF:
printf("eof\n");
break;
default:
if (s->token.val >= TOK_NULL && s->token.val <= TOK_LAST_KEYWORD) {
goto dump_atom;
} else if (s->token.val >= 256) {
printf("token: %d\n", token->val);
} else {
printf("token: '%c'\n", token->val);
}
break;
}
}

int __js_printf_like(2, 3) js_parse_error(JSParseState *s, const char *fmt, ...)
{
JSContext *ctx = s->ctx;
va_list ap;
int backtrace_flags;

va_start(ap, fmt);
JS_ThrowError2(ctx, JS_SYNTAX_ERROR, fmt, ap, FALSE);
va_end(ap);
backtrace_flags = 0;
if (s->cur_func && s->cur_func->backtrace_barrier)
backtrace_flags = JS_BACKTRACE_FLAG_SINGLE_LEVEL;
build_backtrace(ctx, ctx->rt->current_exception, s->filename, s->line_num,
backtrace_flags);
return -1;
}

static int js_parse_expect(JSParseState *s, int tok)
{
    if (s->token.val != tok) {
        /* XXX: dump token correctly in all cases */
        return js_parse_error(s, "expecting '%c'", tok);
    }
    return next_token(s);
}

static int js_parse_expect_semi(JSParseState *s)
{
    if (s->token.val != ';') {
        /* automatic insertion of ';' */
        if (s->token.val == TOK_EOF || s->token.val == '}' || s->got_lf) {
            return 0;
        }
        return js_parse_error(s, "expecting '%c'", ';');
    }
    return next_token(s);
}

static int js_parse_error_reserved_identifier(JSParseState *s)
{
    char buf1[ATOM_GET_STR_BUF_SIZE];
    return js_parse_error(s, "'%s' is a reserved identifier",
                          JS_AtomGetStr(s->ctx, buf1, sizeof(buf1),
                                        s->token.u.ident.atom));
}

static __exception int js_parse_template_part(JSParseState *s, const uint8_t *p)
{
    uint32_t c;
    StringBuffer b_s, *b = &b_s;

    /* p points to the first byte of the template part */
    if (string_buffer_init(s->ctx, b, 32))
        goto fail;
    for(;;) {
        if (p >= s->buf_end)
            goto unexpected_eof;
        c = *p++;
        if (c == '`') {
            /* template end part */
            break;
        }
        if (c == '$' && *p == '{') {
            /* template start or middle part */
            p++;
            break;
        }
        if (c == '\\') {
            if (string_buffer_putc8(b, c))
                goto fail;
            if (p >= s->buf_end)
                goto unexpected_eof;
            c = *p++;
        }
        /* newline sequences are normalized as single '\n' bytes */
        if (c == '\r') {
            if (*p == '\n')
                p++;
            c = '\n';
        }
        if (c == '\n') {
            s->line_num++;
        } else if (c >= 0x80) {
            const uint8_t *p_next;
            c = unicode_from_utf8(p - 1, UTF8_CHAR_LEN_MAX, &p_next);
            if (c > 0x10FFFF) {
                js_parse_error(s, "invalid UTF-8 sequence");
                goto fail;
            }
            p = p_next;
        }
        if (string_buffer_putc(b, c))
            goto fail;
    }
    s->token.val = TOK_TEMPLATE;
    s->token.u.str.sep = c;
    s->token.u.str.str = string_buffer_end(b);
    s->buf_ptr = p;
    return 0;

    unexpected_eof:
    js_parse_error(s, "unexpected end of string");
    fail:
    string_buffer_free(b);
    return -1;
}

static __exception int js_parse_string(JSParseState *s, int sep,
                                       BOOL do_throw, const uint8_t *p,
                                       JSToken *token, const uint8_t **pp)
{
    int ret;
    uint32_t c;
    StringBuffer b_s, *b = &b_s;

    /* string */
    if (string_buffer_init(s->ctx, b, 32))
        goto fail;
    for(;;) {
        if (p >= s->buf_end)
            goto invalid_char;
        c = *p;
        if (c < 0x20) {
            if (!s->cur_func) {
                if (do_throw)
                    js_parse_error(s, "invalid character in a JSON string");
                goto fail;
            }
            if (sep == '`') {
                if (c == '\r') {
                    if (p[1] == '\n')
                        p++;
                    c = '\n';
                }
                /* do not update s->line_num */
            } else if (c == '\n' || c == '\r')
                goto invalid_char;
        }
        p++;
        if (c == sep)
            break;
        if (c == '$' && *p == '{' && sep == '`') {
            /* template start or middle part */
            p++;
            break;
        }
        if (c == '\\') {
            c = *p;
            switch(c) {
                case '\0':
                    if (p >= s->buf_end)
                        goto invalid_char;
                    p++;
                    break;
                case '\'':
                case '\"':
                case '\\':
                    p++;
                    break;
                case '\r':  /* accept DOS and MAC newline sequences */
                    if (p[1] == '\n') {
                        p++;
                    }
                    /* fall thru */
                case '\n':
                    /* ignore escaped newline sequence */
                    p++;
                    if (sep != '`')
                        s->line_num++;
                    continue;
                default:
                    if (c >= '0' && c <= '7') {
                        if (!s->cur_func)
                            goto invalid_octal; /* JSON case */
                        if (!(s->cur_func->js_mode & JS_MODE_STRICT) && sep != '`')
                            goto parse_escape;
                        if (c == '0' && !(p[1] >= '0' && p[1] <= '9')) {
                            p++;
                            c = '\0';
                        } else {
                            invalid_octal:
                            if (do_throw)
                                js_parse_error(s, "invalid octal syntax in strict mode");
                            goto fail;
                        }
                    } else if (c >= 0x80) {
                        const uint8_t *p_next;
                        c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p_next);
                        if (c > 0x10FFFF) {
                            goto invalid_utf8;
                        }
                        p = p_next;
                        /* LS or PS are skipped */
                        if (c == CP_LS || c == CP_PS)
                            continue;
                    } else {
                        parse_escape:
                        ret = lre_parse_escape(&p, TRUE);
                        if (ret == -1) {
                            if (do_throw)
                                js_parse_error(s, "malformed escape sequence in string literal");
                            goto fail;
                        } else if (ret < 0) {
                            /* ignore the '\' (could output a warning) */
                            p++;
                        } else {
                            c = ret;
                        }
                    }
                    break;
            }
        } else if (c >= 0x80) {
            const uint8_t *p_next;
            c = unicode_from_utf8(p - 1, UTF8_CHAR_LEN_MAX, &p_next);
            if (c > 0x10FFFF)
                goto invalid_utf8;
            p = p_next;
        }
        if (string_buffer_putc(b, c))
            goto fail;
    }
    token->val = TOK_STRING;
    token->u.str.sep = c;
    token->u.str.str = string_buffer_end(b);
    *pp = p;
    return 0;

    invalid_utf8:
    if (do_throw)
        js_parse_error(s, "invalid UTF-8 sequence");
    goto fail;
    invalid_char:
    if (do_throw)
        js_parse_error(s, "unexpected end of string");
    fail:
    string_buffer_free(b);
    return -1;
}

static inline BOOL token_is_pseudo_keyword(JSParseState *s, JSAtom atom) {
    return s->token.val == TOK_IDENT && s->token.u.ident.atom == atom &&
           !s->token.u.ident.has_escape;
}

static __exception int js_parse_regexp(JSParseState *s)
{
    const uint8_t *p;
    BOOL in_class;
    StringBuffer b_s, *b = &b_s;
    StringBuffer b2_s, *b2 = &b2_s;
    uint32_t c;

    p = s->buf_ptr;
    p++;
    in_class = FALSE;
    if (string_buffer_init(s->ctx, b, 32))
        return -1;
    if (string_buffer_init(s->ctx, b2, 1))
        goto fail;
    for(;;) {
        if (p >= s->buf_end) {
            eof_error:
            js_parse_error(s, "unexpected end of regexp");
            goto fail;
        }
        c = *p++;
        if (c == '\n' || c == '\r') {
            goto eol_error;
        } else if (c == '/') {
            if (!in_class)
                break;
        } else if (c == '[') {
            in_class = TRUE;
        } else if (c == ']') {
            /* XXX: incorrect as the first character in a class */
            in_class = FALSE;
        } else if (c == '\\') {
            if (string_buffer_putc8(b, c))
                goto fail;
            c = *p++;
            if (c == '\n' || c == '\r')
                goto eol_error;
            else if (c == '\0' && p >= s->buf_end)
                goto eof_error;
            else if (c >= 0x80) {
                const uint8_t *p_next;
                c = unicode_from_utf8(p - 1, UTF8_CHAR_LEN_MAX, &p_next);
                if (c > 0x10FFFF) {
                    goto invalid_utf8;
                }
                p = p_next;
                if (c == CP_LS || c == CP_PS)
                    goto eol_error;
            }
        } else if (c >= 0x80) {
            const uint8_t *p_next;
            c = unicode_from_utf8(p - 1, UTF8_CHAR_LEN_MAX, &p_next);
            if (c > 0x10FFFF) {
                invalid_utf8:
                js_parse_error(s, "invalid UTF-8 sequence");
                goto fail;
            }
            p = p_next;
            /* LS or PS are considered as line terminator */
            if (c == CP_LS || c == CP_PS) {
                eol_error:
                js_parse_error(s, "unexpected line terminator in regexp");
                goto fail;
            }
        }
        if (string_buffer_putc(b, c))
            goto fail;
    }

    /* flags */
    for(;;) {
        const uint8_t *p_next = p;
        c = *p_next++;
        if (c >= 0x80) {
            c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p_next);
            if (c > 0x10FFFF) {
                goto invalid_utf8;
            }
        }
        if (!lre_js_is_ident_next(c))
            break;
        if (string_buffer_putc(b2, c))
            goto fail;
        p = p_next;
    }

    s->token.val = TOK_REGEXP;
    s->token.u.regexp.body = string_buffer_end(b);
    s->token.u.regexp.flags = string_buffer_end(b2);
    s->buf_ptr = p;
    return 0;
    fail:
    string_buffer_free(b);
    string_buffer_free(b2);
    return -1;
}

static __exception int ident_realloc(JSContext *ctx, char **pbuf, size_t *psize,
                                     char *static_buf)
{
    char *buf, *new_buf;
    size_t size, new_size;

    buf = *pbuf;
    size = *psize;
    if (size >= (SIZE_MAX / 3) * 2)
        new_size = SIZE_MAX;
    else
        new_size = size + (size >> 1);
    if (buf == static_buf) {
        new_buf = js_malloc(ctx, new_size);
        if (!new_buf)
            return -1;
        memcpy(new_buf, buf, size);
    } else {
        new_buf = js_realloc(ctx, buf, new_size);
        if (!new_buf)
            return -1;
    }
    *pbuf = new_buf;
    *psize = new_size;
    return 0;
}

/* 'c' is the first character. Return JS_ATOM_NULL in case of error */
static JSAtom parse_ident(JSParseState *s, const uint8_t **pp,
                          BOOL *pident_has_escape, int c, BOOL is_private)
{
    const uint8_t *p, *p1;
    char ident_buf[128], *buf;
    size_t ident_size, ident_pos;
    JSAtom atom;

    p = *pp;
    buf = ident_buf;
    ident_size = sizeof(ident_buf);
    ident_pos = 0;
    if (is_private)
        buf[ident_pos++] = '#';
    for(;;) {
        p1 = p;

        if (c < 128) {
            buf[ident_pos++] = c;
        } else {
            ident_pos += unicode_to_utf8((uint8_t*)buf + ident_pos, c);
        }
        c = *p1++;
        if (c == '\\' && *p1 == 'u') {
            c = lre_parse_escape(&p1, TRUE);
            *pident_has_escape = TRUE;
        } else if (c >= 128) {
            c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p1);
        }
        if (!lre_js_is_ident_next(c))
            break;
        p = p1;
        if (unlikely(ident_pos >= ident_size - UTF8_CHAR_LEN_MAX)) {
            if (ident_realloc(s->ctx, &buf, &ident_size, ident_buf)) {
                atom = JS_ATOM_NULL;
                goto done;
            }
        }
    }
    atom = JS_NewAtomLen(s->ctx, buf, ident_pos);
    done:
    if (unlikely(buf != ident_buf))
        js_free(s->ctx, buf);
    *pp = p;
    return atom;
}


static __exception int next_token(JSParseState *s)
{
    const uint8_t *p;
    int c;
    BOOL ident_has_escape;
    JSAtom atom;

    if (js_check_stack_overflow(s->ctx->rt, 0)) {
        return js_parse_error(s, "stack overflow");
    }

    free_token(s, &s->token);

    p = s->last_ptr = s->buf_ptr;
    s->got_lf = FALSE;
    s->last_line_num = s->token.line_num;
    redo:
    s->token.line_num = s->line_num;
    s->token.ptr = p;
    c = *p;
    switch(c) {
        case 0:
            if (p >= s->buf_end) {
                s->token.val = TOK_EOF;
            } else {
                goto def_token;
            }
            break;
        case '`':
            if (js_parse_template_part(s, p + 1))
                goto fail;
            p = s->buf_ptr;
            break;
        case '\'':
        case '\"':
            if (js_parse_string(s, c, TRUE, p + 1, &s->token, &p))
                goto fail;
            break;
        case '\r':  /* accept DOS and MAC newline sequences */
            if (p[1] == '\n') {
                p++;
            }
            /* fall thru */
        case '\n':
            p++;
        line_terminator:
            s->got_lf = TRUE;
            s->line_num++;
            goto redo;
        case '\f':
        case '\v':
        case ' ':
        case '\t':
            p++;
            goto redo;
        case '/':
            if (p[1] == '*') {
                /* comment */
                p += 2;
                for(;;) {
                    if (*p == '\0' && p >= s->buf_end) {
                        js_parse_error(s, "unexpected end of comment");
                        goto fail;
                    }
                    if (p[0] == '*' && p[1] == '/') {
                        p += 2;
                        break;
                    }
                    if (*p == '\n') {
                        s->line_num++;
                        s->got_lf = TRUE; /* considered as LF for ASI */
                        p++;
                    } else if (*p == '\r') {
                        s->got_lf = TRUE; /* considered as LF for ASI */
                        p++;
                    } else if (*p >= 0x80) {
                        c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p);
                        if (c == CP_LS || c == CP_PS) {
                            s->got_lf = TRUE; /* considered as LF for ASI */
                        } else if (c == -1) {
                            p++; /* skip invalid UTF-8 */
                        }
                    } else {
                        p++;
                    }
                }
                goto redo;
            } else if (p[1] == '/') {
                /* line comment */
                p += 2;
                skip_line_comment:
                for(;;) {
                    if (*p == '\0' && p >= s->buf_end)
                        break;
                    if (*p == '\r' || *p == '\n')
                        break;
                    if (*p >= 0x80) {
                        c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p);
                        /* LS or PS are considered as line terminator */
                        if (c == CP_LS || c == CP_PS) {
                            break;
                        } else if (c == -1) {
                            p++; /* skip invalid UTF-8 */
                        }
                    } else {
                        p++;
                    }
                }
                goto redo;
            } else if (p[1] == '=') {
                p += 2;
                s->token.val = TOK_DIV_ASSIGN;
            } else {
                p++;
                s->token.val = c;
            }
            break;
        case '\\':
            if (p[1] == 'u') {
                const uint8_t *p1 = p + 1;
                int c1 = lre_parse_escape(&p1, TRUE);
                if (c1 >= 0 && lre_js_is_ident_first(c1)) {
                    c = c1;
                    p = p1;
                    ident_has_escape = TRUE;
                    goto has_ident;
                } else {
                    /* XXX: syntax error? */
                }
            }
            goto def_token;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
        case '$':
            /* identifier */
            p++;
            ident_has_escape = FALSE;
        has_ident:
            atom = parse_ident(s, &p, &ident_has_escape, c, FALSE);
            if (atom == JS_ATOM_NULL)
                goto fail;
            s->token.u.ident.atom = atom;
            s->token.u.ident.has_escape = ident_has_escape;
            s->token.u.ident.is_reserved = FALSE;
            if (s->token.u.ident.atom <= JS_ATOM_LAST_KEYWORD ||
                (s->token.u.ident.atom <= JS_ATOM_LAST_STRICT_KEYWORD &&
                 (s->cur_func->js_mode & JS_MODE_STRICT)) ||
                (s->token.u.ident.atom == JS_ATOM_yield &&
                 ((s->cur_func->func_kind & JS_FUNC_GENERATOR) ||
                  (s->cur_func->func_type == JS_PARSE_FUNC_ARROW &&
                   !s->cur_func->in_function_body && s->cur_func->parent &&
                   (s->cur_func->parent->func_kind & JS_FUNC_GENERATOR)))) ||
                (s->token.u.ident.atom == JS_ATOM_await &&
                 (s->is_module ||
                  (((s->cur_func->func_kind & JS_FUNC_ASYNC) ||
                    (s->cur_func->func_type == JS_PARSE_FUNC_ARROW &&
                     !s->cur_func->in_function_body && s->cur_func->parent &&
                     (s->cur_func->parent->func_kind & JS_FUNC_ASYNC))))))) {
                if (ident_has_escape) {
                    s->token.u.ident.is_reserved = TRUE;
                    s->token.val = TOK_IDENT;
                } else {
                    /* The keywords atoms are pre allocated */
                    s->token.val = s->token.u.ident.atom - 1 + TOK_FIRST_KEYWORD;
                }
            } else {
                s->token.val = TOK_IDENT;
            }
            break;
        case '#':
            /* private name */
        {
            const uint8_t *p1;
            p++;
            p1 = p;
            c = *p1++;
            if (c == '\\' && *p1 == 'u') {
                c = lre_parse_escape(&p1, TRUE);
            } else if (c >= 128) {
                c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p1);
            }
            if (!lre_js_is_ident_first(c)) {
                js_parse_error(s, "invalid first character of private name");
                goto fail;
            }
            p = p1;
            ident_has_escape = FALSE; /* not used */
            atom = parse_ident(s, &p, &ident_has_escape, c, TRUE);
            if (atom == JS_ATOM_NULL)
                goto fail;
            s->token.u.ident.atom = atom;
            s->token.val = TOK_PRIVATE_NAME;
        }
            break;
        case '.':
            if (p[1] == '.' && p[2] == '.') {
                p += 3;
                s->token.val = TOK_ELLIPSIS;
                break;
            }
            if (p[1] >= '0' && p[1] <= '9') {
                goto parse_number;
            } else {
                goto def_token;
            }
            break;
        case '0':
            /* in strict mode, octal literals are not accepted */
            if (is_digit(p[1]) && (s->cur_func->js_mode & JS_MODE_STRICT)) {
                js_parse_error(s, "octal literals are deprecated in strict mode");
                goto fail;
            }
            goto parse_number;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            /* number */
        parse_number:
        {
            JSValue ret;
            const uint8_t *p1;
            int flags, radix;
            flags = ATOD_ACCEPT_BIN_OCT | ATOD_ACCEPT_LEGACY_OCTAL |
                    ATOD_ACCEPT_UNDERSCORES;
#ifdef CONFIG_BIGNUM
            flags |= ATOD_ACCEPT_SUFFIX;
            if (s->cur_func->js_mode & JS_MODE_MATH) {
                flags |= ATOD_MODE_BIGINT;
                if (s->cur_func->js_mode & JS_MODE_MATH)
                    flags |= ATOD_TYPE_BIG_FLOAT;
            }
#endif
            radix = 0;
#ifdef CONFIG_BIGNUM
            s->token.u.num.exponent = 0;
            ret = js_atof2(s->ctx, (const char *)p, (const char **)&p, radix,
                           flags, &s->token.u.num.exponent);
#else
            ret = js_atof(s->ctx, (const char *)p, (const char **)&p, radix,
                          flags);
#endif
            if (JS_IsException(ret))
                goto fail;
            /* reject `10instanceof Number` */
            if (JS_VALUE_IS_NAN(ret) ||
                lre_js_is_ident_next(unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p1))) {
                JS_FreeValue(s->ctx, ret);
                js_parse_error(s, "invalid number literal");
                goto fail;
            }
            s->token.val = TOK_NUMBER;
            s->token.u.num.val = ret;
        }
            break;
        case '*':
            if (p[1] == '=') {
                p += 2;
                s->token.val = TOK_MUL_ASSIGN;
            } else if (p[1] == '*') {
                if (p[2] == '=') {
                    p += 3;
                    s->token.val = TOK_POW_ASSIGN;
                } else {
                    p += 2;
                    s->token.val = TOK_POW;
                }
            } else {
                goto def_token;
            }
            break;
        case '%':
            if (p[1] == '=') {
                p += 2;
                s->token.val = TOK_MOD_ASSIGN;
            } else {
                goto def_token;
            }
            break;
        case '+':
            if (p[1] == '=') {
                p += 2;
                s->token.val = TOK_PLUS_ASSIGN;
            } else if (p[1] == '+') {
                p += 2;
                s->token.val = TOK_INC;
            } else {
                goto def_token;
            }
            break;
        case '-':
            if (p[1] == '=') {
                p += 2;
                s->token.val = TOK_MINUS_ASSIGN;
            } else if (p[1] == '-') {
                if (s->allow_html_comments &&
                    p[2] == '>' && s->last_line_num != s->line_num) {
                    /* Annex B: `-->` at beginning of line is an html comment end.
                   It extends to the end of the line.
                 */
                    goto skip_line_comment;
                }
                p += 2;
                s->token.val = TOK_DEC;
            } else {
                goto def_token;
            }
            break;
        case '<':
            if (p[1] == '=') {
                p += 2;
                s->token.val = TOK_LTE;
            } else if (p[1] == '<') {
                if (p[2] == '=') {
                    p += 3;
                    s->token.val = TOK_SHL_ASSIGN;
                } else {
                    p += 2;
                    s->token.val = TOK_SHL;
                }
            } else if (s->allow_html_comments &&
                       p[1] == '!' && p[2] == '-' && p[3] == '-') {
                /* Annex B: handle `<!--` single line html comments */
                goto skip_line_comment;
            } else {
                goto def_token;
            }
            break;
        case '>':
            if (p[1] == '=') {
                p += 2;
                s->token.val = TOK_GTE;
            } else if (p[1] == '>') {
                if (p[2] == '>') {
                    if (p[3] == '=') {
                        p += 4;
                        s->token.val = TOK_SHR_ASSIGN;
                    } else {
                        p += 3;
                        s->token.val = TOK_SHR;
                    }
                } else if (p[2] == '=') {
                    p += 3;
                    s->token.val = TOK_SAR_ASSIGN;
                } else {
                    p += 2;
                    s->token.val = TOK_SAR;
                }
            } else {
                goto def_token;
            }
            break;
        case '=':
            if (p[1] == '=') {
                if (p[2] == '=') {
                    p += 3;
                    s->token.val = TOK_STRICT_EQ;
                } else {
                    p += 2;
                    s->token.val = TOK_EQ;
                }
            } else if (p[1] == '>') {
                p += 2;
                s->token.val = TOK_ARROW;
            } else {
                goto def_token;
            }
            break;
        case '!':
            if (p[1] == '=') {
                if (p[2] == '=') {
                    p += 3;
                    s->token.val = TOK_STRICT_NEQ;
                } else {
                    p += 2;
                    s->token.val = TOK_NEQ;
                }
            } else {
                goto def_token;
            }
            break;
        case '&':
            if (p[1] == '=') {
                p += 2;
                s->token.val = TOK_AND_ASSIGN;
            } else if (p[1] == '&') {
                p += 2;
                s->token.val = TOK_LAND;
            } else {
                goto def_token;
            }
            break;
#ifdef CONFIG_BIGNUM
            /* in math mode, '^' is the power operator. '^^' is always the
           xor operator and '**' is always the power operator */
    case '^':
        if (p[1] == '=') {
            p += 2;
            if (s->cur_func->js_mode & JS_MODE_MATH)
                s->token.val = TOK_MATH_POW_ASSIGN;
            else
                s->token.val = TOK_XOR_ASSIGN;
        } else if (p[1] == '^') {
            if (p[2] == '=') {
                p += 3;
                s->token.val = TOK_XOR_ASSIGN;
            } else {
                p += 2;
                s->token.val = '^';
            }
        } else {
            p++;
            if (s->cur_func->js_mode & JS_MODE_MATH)
                s->token.val = TOK_MATH_POW;
            else
                s->token.val = '^';
        }
        break;
#else
        case '^':
            if (p[1] == '=') {
                p += 2;
                s->token.val = TOK_XOR_ASSIGN;
            } else {
                goto def_token;
            }
            break;
#endif
        case '|':
            if (p[1] == '=') {
                p += 2;
                s->token.val = TOK_OR_ASSIGN;
            } else if (p[1] == '|') {
                p += 2;
                s->token.val = TOK_LOR;
            } else {
                goto def_token;
            }
            break;
        case '?':
            if (p[1] == '?') {
                p += 2;
                s->token.val = TOK_DOUBLE_QUESTION_MARK;
            } else if (p[1] == '.' && !(p[2] >= '0' && p[2] <= '9')) {
                p += 2;
                s->token.val = TOK_QUESTION_MARK_DOT;
            } else {
                goto def_token;
            }
            break;
        default:
            if (c >= 128) {
                /* unicode value */
                c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p);
                switch(c) {
                    case CP_PS:
                    case CP_LS:
                        /* XXX: should avoid incrementing line_number, but
                   needed to handle HTML comments */
                        goto line_terminator;
                    default:
                        if (lre_is_space(c)) {
                            goto redo;
                        } else if (lre_js_is_ident_first(c)) {
                            ident_has_escape = FALSE;
                            goto has_ident;
                        } else {
                            js_parse_error(s, "unexpected character");
                            goto fail;
                        }
                }
            }
        def_token:
            s->token.val = c;
            p++;
            break;
    }
    s->buf_ptr = p;

    //    dump_token(s, &s->token);
    return 0;

    fail:
    s->token.val = TOK_ERROR;
    return -1;
}

/* 'c' is the first character. Return JS_ATOM_NULL in case of error */
static JSAtom json_parse_ident(JSParseState *s, const uint8_t **pp, int c)
{
    const uint8_t *p;
    char ident_buf[128], *buf;
    size_t ident_size, ident_pos;
    JSAtom atom;

    p = *pp;
    buf = ident_buf;
    ident_size = sizeof(ident_buf);
    ident_pos = 0;
    for(;;) {
        buf[ident_pos++] = c;
        c = *p;
        if (c >= 128 ||
            !((lre_id_continue_table_ascii[c >> 5] >> (c & 31)) & 1))
            break;
        p++;
        if (unlikely(ident_pos >= ident_size - UTF8_CHAR_LEN_MAX)) {
            if (ident_realloc(s->ctx, &buf, &ident_size, ident_buf)) {
                atom = JS_ATOM_NULL;
                goto done;
            }
        }
    }
    atom = JS_NewAtomLen(s->ctx, buf, ident_pos);
    done:
    if (unlikely(buf != ident_buf))
        js_free(s->ctx, buf);
    *pp = p;
    return atom;
}

static __exception int json_next_token(JSParseState *s)
{
    const uint8_t *p;
    int c;
    JSAtom atom;

    if (js_check_stack_overflow(s->ctx->rt, 0)) {
        return js_parse_error(s, "stack overflow");
    }

    free_token(s, &s->token);

    p = s->last_ptr = s->buf_ptr;
    s->last_line_num = s->token.line_num;
    redo:
    s->token.line_num = s->line_num;
    s->token.ptr = p;
    c = *p;
    switch(c) {
        case 0:
            if (p >= s->buf_end) {
                s->token.val = TOK_EOF;
            } else {
                goto def_token;
            }
            break;
        case '\'':
            if (!s->ext_json) {
                /* JSON does not accept single quoted strings */
                goto def_token;
            }
            /* fall through */
        case '\"':
            if (js_parse_string(s, c, TRUE, p + 1, &s->token, &p))
                goto fail;
            break;
        case '\r':  /* accept DOS and MAC newline sequences */
            if (p[1] == '\n') {
                p++;
            }
            /* fall thru */
        case '\n':
            p++;
            s->line_num++;
            goto redo;
        case '\f':
        case '\v':
            if (!s->ext_json) {
                /* JSONWhitespace does not match <VT>, nor <FF> */
                goto def_token;
            }
            /* fall through */
        case ' ':
        case '\t':
            p++;
            goto redo;
        case '/':
            if (!s->ext_json) {
                /* JSON does not accept comments */
                goto def_token;
            }
            if (p[1] == '*') {
                /* comment */
                p += 2;
                for(;;) {
                    if (*p == '\0' && p >= s->buf_end) {
                        js_parse_error(s, "unexpected end of comment");
                        goto fail;
                    }
                    if (p[0] == '*' && p[1] == '/') {
                        p += 2;
                        break;
                    }
                    if (*p == '\n') {
                        s->line_num++;
                        p++;
                    } else if (*p == '\r') {
                        p++;
                    } else if (*p >= 0x80) {
                        c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p);
                        if (c == -1) {
                            p++; /* skip invalid UTF-8 */
                        }
                    } else {
                        p++;
                    }
                }
                goto redo;
            } else if (p[1] == '/') {
                /* line comment */
                p += 2;
                for(;;) {
                    if (*p == '\0' && p >= s->buf_end)
                        break;
                    if (*p == '\r' || *p == '\n')
                        break;
                    if (*p >= 0x80) {
                        c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p);
                        /* LS or PS are considered as line terminator */
                        if (c == CP_LS || c == CP_PS) {
                            break;
                        } else if (c == -1) {
                            p++; /* skip invalid UTF-8 */
                        }
                    } else {
                        p++;
                    }
                }
                goto redo;
            } else {
                goto def_token;
            }
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
        case '$':
            /* identifier : only pure ascii characters are accepted */
            p++;
            atom = json_parse_ident(s, &p, c);
            if (atom == JS_ATOM_NULL)
                goto fail;
            s->token.u.ident.atom = atom;
            s->token.u.ident.has_escape = FALSE;
            s->token.u.ident.is_reserved = FALSE;
            s->token.val = TOK_IDENT;
            break;
        case '+':
            if (!s->ext_json || !is_digit(p[1]))
                goto def_token;
            goto parse_number;
        case '0':
            if (is_digit(p[1]))
                goto def_token;
            goto parse_number;
        case '-':
            if (!is_digit(p[1]))
                goto def_token;
            goto parse_number;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            /* number */
        parse_number:
        {
            JSValue ret;
            int flags, radix;
            if (!s->ext_json) {
                flags = 0;
                radix = 10;
            } else {
                flags = ATOD_ACCEPT_BIN_OCT;
                radix = 0;
            }
            ret = js_atof(s->ctx, (const char *)p, (const char **)&p, radix,
                          flags);
            if (JS_IsException(ret))
                goto fail;
            s->token.val = TOK_NUMBER;
            s->token.u.num.val = ret;
        }
            break;
        default:
            if (c >= 128) {
                js_parse_error(s, "unexpected character");
                goto fail;
            }
        def_token:
            s->token.val = c;
            p++;
            break;
    }
    s->buf_ptr = p;

    //    dump_token(s, &s->token);
    return 0;

    fail:
    s->token.val = TOK_ERROR;
    return -1;
}

/* only used for ':' and '=>', 'let' or 'function' look-ahead. *pp is
   only set if TOK_IMPORT is returned */
/* XXX: handle all unicode cases */
static int simple_next_token(const uint8_t **pp, BOOL no_line_terminator)
{
    const uint8_t *p;
    uint32_t c;

    /* skip spaces and comments */
    p = *pp;
    for (;;) {
        switch(c = *p++) {
            case '\r':
            case '\n':
                if (no_line_terminator)
                    return '\n';
                continue;
            case ' ':
            case '\t':
            case '\v':
            case '\f':
                continue;
            case '/':
                if (*p == '/') {
                    if (no_line_terminator)
                        return '\n';
                    while (*p && *p != '\r' && *p != '\n')
                        p++;
                    continue;
                }
                if (*p == '*') {
                    while (*++p) {
                        if ((*p == '\r' || *p == '\n') && no_line_terminator)
                            return '\n';
                        if (*p == '*' && p[1] == '/') {
                            p += 2;
                            break;
                        }
                    }
                    continue;
                }
                break;
            case '=':
                if (*p == '>')
                    return TOK_ARROW;
                break;
            default:
                if (lre_js_is_ident_first(c)) {
                    if (c == 'i') {
                        if (p[0] == 'n' && !lre_js_is_ident_next(p[1])) {
                            return TOK_IN;
                        }
                        if (p[0] == 'm' && p[1] == 'p' && p[2] == 'o' &&
                            p[3] == 'r' && p[4] == 't' &&
                            !lre_js_is_ident_next(p[5])) {
                            *pp = p + 5;
                            return TOK_IMPORT;
                        }
                    } else if (c == 'o' && *p == 'f' && !lre_js_is_ident_next(p[1])) {
                        return TOK_OF;
                    } else if (c == 'e' &&
                               p[0] == 'x' && p[1] == 'p' && p[2] == 'o' &&
                               p[3] == 'r' && p[4] == 't' &&
                               !lre_js_is_ident_next(p[5])) {
                        *pp = p + 5;
                        return TOK_EXPORT;
                    } else if (c == 'f' && p[0] == 'u' && p[1] == 'n' &&
                               p[2] == 'c' && p[3] == 't' && p[4] == 'i' &&
                               p[5] == 'o' && p[6] == 'n' && !lre_js_is_ident_next(p[7])) {
                        return TOK_FUNCTION;
                    }
                    return TOK_IDENT;
                }
                break;
        }
        return c;
    }
}

static int peek_token(JSParseState *s, BOOL no_line_terminator)
{
    const uint8_t *p = s->buf_ptr;
    return simple_next_token(&p, no_line_terminator);
}

/* return true if 'input' contains the source of a module
   (heuristic). 'input' must be a zero terminated.

   Heuristic: skip comments and expect 'import' keyword not followed
   by '(' or '.' or export keyword.
*/
BOOL JS_DetectModule(const char *input, size_t input_len)
{
    const uint8_t *p = (const uint8_t *)input;
    int tok;
    switch(simple_next_token(&p, FALSE)) {
        case TOK_IMPORT:
            tok = simple_next_token(&p, FALSE);
            return (tok != '.' && tok != '(');
        case TOK_EXPORT:
            return TRUE;
        default:
            return FALSE;
    }
}

static inline int get_prev_opcode(JSFunctionDef *fd) {
    if (fd->last_opcode_pos < 0)
        return OP_invalid;
    else
        return fd->byte_code.buf[fd->last_opcode_pos];
}

static BOOL js_is_live_code(JSParseState *s) {
    switch (get_prev_opcode(s->cur_func)) {
        case OP_tail_call:
        case OP_tail_call_method:
        case OP_return:
        case OP_return_undef:
        case OP_return_async:
        case OP_throw:
        case OP_throw_var:
        case OP_goto:
#if SHORT_OPCODES
            case OP_goto8:
    case OP_goto16:
#endif
        case OP_ret:
            return FALSE;
        default:
            return TRUE;
    }
}

static void emit_u8(JSParseState *s, uint8_t val)
{
    dbuf_putc(&s->cur_func->byte_code, val);
}

static void emit_u16(JSParseState *s, uint16_t val)
{
    dbuf_put_u16(&s->cur_func->byte_code, val);
}

static void emit_u32(JSParseState *s, uint32_t val)
{
    dbuf_put_u32(&s->cur_func->byte_code, val);
}

static void emit_op(JSParseState *s, uint8_t val)
{
    JSFunctionDef *fd = s->cur_func;
    DynBuf *bc = &fd->byte_code;

    /* Use the line number of the last token used, not the next token,
       nor the current offset in the source file.
     */
    if (unlikely(fd->last_opcode_line_num != s->last_line_num)) {
        dbuf_putc(bc, OP_line_num);
        dbuf_put_u32(bc, s->last_line_num);
        fd->last_opcode_line_num = s->last_line_num;
    }
    fd->last_opcode_pos = bc->size;
    dbuf_putc(bc, val);
}

static void emit_atom(JSParseState *s, JSAtom name)
{
    emit_u32(s, JS_DupAtom(s->ctx, name));
}

static int update_label(JSFunctionDef *s, int label, int delta)
{
    LabelSlot *ls;

    assert(label >= 0 && label < s->label_count);
    ls = &s->label_slots[label];
    ls->ref_count += delta;
    assert(ls->ref_count >= 0);
    return ls->ref_count;
}

static int new_label_fd(JSFunctionDef *fd, int label)
{
    LabelSlot *ls;

    if (label < 0) {
        if (js_resize_array(fd->ctx, (void *)&fd->label_slots,
                            sizeof(fd->label_slots[0]),
                            &fd->label_size, fd->label_count + 1))
            return -1;
        label = fd->label_count++;
        ls = &fd->label_slots[label];
        ls->ref_count = 0;
        ls->pos = -1;
        ls->pos2 = -1;
        ls->addr = -1;
        ls->first_reloc = NULL;
    }
    return label;
}

static int new_label(JSParseState *s)
{
    return new_label_fd(s->cur_func, -1);
}

/* return the label ID offset */
static int emit_label(JSParseState *s, int label)
{
    if (label >= 0) {
        emit_op(s, OP_label);
        emit_u32(s, label);
        s->cur_func->label_slots[label].pos = s->cur_func->byte_code.size;
        return s->cur_func->byte_code.size - 4;
    } else {
        return -1;
    }
}

/* return label or -1 if dead code */
static int emit_goto(JSParseState *s, int opcode, int label)
{
    if (js_is_live_code(s)) {
        if (label < 0)
            label = new_label(s);
        emit_op(s, opcode);
        emit_u32(s, label);
        s->cur_func->label_slots[label].ref_count++;
        return label;
    }
    return -1;
}

/* return the constant pool index. 'val' is not duplicated. */
static int cpool_add(JSParseState *s, JSValue val)
{
    JSFunctionDef *fd = s->cur_func;

    if (js_resize_array(s->ctx, (void *)&fd->cpool, sizeof(fd->cpool[0]),
                        &fd->cpool_size, fd->cpool_count + 1))
        return -1;
    fd->cpool[fd->cpool_count++] = val;
    return fd->cpool_count - 1;
}

static __exception int emit_push_const(JSParseState *s, JSValueConst val,
                                       BOOL as_atom)
{
    int idx;

    if (JS_VALUE_GET_TAG(val) == JS_TAG_STRING && as_atom) {
        JSAtom atom;
        /* warning: JS_NewAtomStr frees the string value */
        JS_DupValue(s->ctx, val);
        atom = JS_NewAtomStr(s->ctx, JS_VALUE_GET_STRING(val));
        if (atom != JS_ATOM_NULL && !__JS_AtomIsTaggedInt(atom)) {
            emit_op(s, OP_push_atom_value);
            emit_u32(s, atom);
            return 0;
        }
    }

    idx = cpool_add(s, JS_DupValue(s->ctx, val));
    if (idx < 0)
        return -1;
    emit_op(s, OP_push_const);
    emit_u32(s, idx);
    return 0;
}

/* return the variable index or -1 if not found,
   add ARGUMENT_VAR_OFFSET for argument variables */
static int find_arg(JSContext *ctx, JSFunctionDef *fd, JSAtom name)
{
    int i;
    for(i = fd->arg_count; i-- > 0;) {
        if (fd->args[i].var_name == name)
            return i | ARGUMENT_VAR_OFFSET;
    }
    return -1;
}

static int find_var(JSContext *ctx, JSFunctionDef *fd, JSAtom name)
{
    int i;
    for(i = fd->var_count; i-- > 0;) {
        if (fd->vars[i].var_name == name && fd->vars[i].scope_level == 0)
            return i;
    }
    return find_arg(ctx, fd, name);
}

/* return true if scope == parent_scope or if scope is a child of
   parent_scope */
static BOOL is_child_scope(JSContext *ctx, JSFunctionDef *fd,
                           int scope, int parent_scope)
{
    while (scope >= 0) {
        if (scope == parent_scope)
            return TRUE;
        scope = fd->scopes[scope].parent;
    }
    return FALSE;
}

/* find a 'var' declaration in the same scope or a child scope */
static int find_var_in_child_scope(JSContext *ctx, JSFunctionDef *fd,
                                   JSAtom name, int scope_level)
{
    int i;
    for(i = 0; i < fd->var_count; i++) {
        JSVarDef *vd = &fd->vars[i];
        if (vd->var_name == name && vd->scope_level == 0) {
            if (is_child_scope(ctx, fd, vd->func_pool_or_scope_idx,
                               scope_level))
                return i;
        }
    }
    return -1;
}


static JSHoistedDef *find_hoisted_def(JSFunctionDef *fd, JSAtom name)
{
    int i;
    for(i = 0; i < fd->hoisted_def_count; i++) {
        JSHoistedDef *hf = &fd->hoisted_def[i];
        if (hf->var_name == name)
            return hf;
    }
    return NULL;

}

static JSHoistedDef *find_lexical_hoisted_def(JSFunctionDef *fd, JSAtom name)
{
    JSHoistedDef *hf = find_hoisted_def(fd, name);
    if (hf && hf->is_lexical)
        return hf;
    else
        return NULL;
}

static int find_lexical_decl(JSContext *ctx, JSFunctionDef *fd, JSAtom name,
                             int scope_idx, BOOL check_catch_var)
{
    while (scope_idx >= 0) {
        JSVarDef *vd = &fd->vars[scope_idx];
        if (vd->var_name == name &&
            (vd->is_lexical || (vd->var_kind == JS_VAR_CATCH &&
                                check_catch_var)))
            return scope_idx;
        scope_idx = vd->scope_next;
    }

    if (fd->is_eval && fd->eval_type == JS_EVAL_TYPE_GLOBAL) {
        if (find_lexical_hoisted_def(fd, name))
            return GLOBAL_VAR_OFFSET;
    }
    return -1;
}

static int push_scope(JSParseState *s) {
    if (s->cur_func) {
        JSFunctionDef *fd = s->cur_func;
        int scope = fd->scope_count;
        /* XXX: should check for scope overflow */
        if ((fd->scope_count + 1) > fd->scope_size) {
            int new_size;
            size_t slack;
            JSVarScope *new_buf;
            /* XXX: potential arithmetic overflow */
            new_size = max_int(fd->scope_count + 1, fd->scope_size * 3 / 2);
            if (fd->scopes == fd->def_scope_array) {
                new_buf = js_realloc2(s->ctx, NULL, new_size * sizeof(*fd->scopes), &slack);
                if (!new_buf)
                    return -1;
                memcpy(new_buf, fd->scopes, fd->scope_count * sizeof(*fd->scopes));
            } else {
                new_buf = js_realloc2(s->ctx, fd->scopes, new_size * sizeof(*fd->scopes), &slack);
                if (!new_buf)
                    return -1;
            }
            new_size += slack / sizeof(*new_buf);
            fd->scopes = new_buf;
            fd->scope_size = new_size;
        }
        fd->scope_count++;
        fd->scopes[scope].parent = fd->scope_level;
        fd->scopes[scope].first = fd->scope_first;
        emit_op(s, OP_enter_scope);
        emit_u16(s, scope);
        return fd->scope_level = scope;
    }
    return 0;
}

static int get_first_lexical_var(JSFunctionDef *fd, int scope)
{
    while (scope >= 0) {
        int scope_idx = fd->scopes[scope].first;
        if (scope_idx >= 0)
            return scope_idx;
        scope = fd->scopes[scope].parent;
    }
    return -1;
}

static void pop_scope(JSParseState *s) {
    if (s->cur_func) {
        /* disable scoped variables */
        JSFunctionDef *fd = s->cur_func;
        int scope = fd->scope_level;
        emit_op(s, OP_leave_scope);
        emit_u16(s, scope);
        fd->scope_level = fd->scopes[scope].parent;
        fd->scope_first = get_first_lexical_var(fd, fd->scope_level);
    }
}

static void close_scopes(JSParseState *s, int scope, int scope_stop)
{
    while (scope > scope_stop) {
        emit_op(s, OP_leave_scope);
        emit_u16(s, scope);
        scope = s->cur_func->scopes[scope].parent;
    }
}

/* return the variable index or -1 if error */
static int add_var(JSContext *ctx, JSFunctionDef *fd, JSAtom name)
{
    JSVarDef *vd;

    /* the local variable indexes are currently stored on 16 bits */
    if (fd->var_count >= JS_MAX_LOCAL_VARS) {
        JS_ThrowInternalError(ctx, "too many local variables");
        return -1;
    }
    if (js_resize_array(ctx, (void **)&fd->vars, sizeof(fd->vars[0]),
                        &fd->var_size, fd->var_count + 1))
        return -1;
    vd = &fd->vars[fd->var_count++];
    memset(vd, 0, sizeof(*vd));
    vd->var_name = JS_DupAtom(ctx, name);
    return fd->var_count - 1;
}

static int add_scope_var(JSContext *ctx, JSFunctionDef *fd, JSAtom name,
                         JSVarKindEnum var_kind)
{
    int idx = add_var(ctx, fd, name);
    if (idx >= 0) {
        JSVarDef *vd = &fd->vars[idx];
        vd->var_kind = var_kind;
        vd->scope_level = fd->scope_level;
        vd->scope_next = fd->scope_first;
        fd->scopes[fd->scope_level].first = idx;
        fd->scope_first = idx;
    }
    return idx;
}

static int add_func_var(JSContext *ctx, JSFunctionDef *fd, JSAtom name)
{
    int idx = fd->func_var_idx;
    if (idx < 0 && (idx = add_var(ctx, fd, name)) >= 0) {
        fd->func_var_idx = idx;
        fd->vars[idx].is_func_var = TRUE;
        if (fd->js_mode & JS_MODE_STRICT)
            fd->vars[idx].is_const = TRUE;
    }
    return idx;
}

static int add_arguments_var(JSContext *ctx, JSFunctionDef *fd, JSAtom name)
{
    int idx = fd->arguments_var_idx;
    if (idx < 0 && (idx = add_var(ctx, fd, name)) >= 0) {
        fd->arguments_var_idx = idx;
    }
    return idx;
}

static int add_arg(JSContext *ctx, JSFunctionDef *fd, JSAtom name)
{
    JSVarDef *vd;

    /* the local variable indexes are currently stored on 16 bits */
    if (fd->arg_count >= JS_MAX_LOCAL_VARS) {
        JS_ThrowInternalError(ctx, "too many arguments");
        return -1;
    }
    if (js_resize_array(ctx, (void **)&fd->args, sizeof(fd->args[0]),
                        &fd->arg_size, fd->arg_count + 1))
        return -1;
    vd = &fd->args[fd->arg_count++];
    memset(vd, 0, sizeof(*vd));
    vd->var_name = JS_DupAtom(ctx, name);
    return fd->arg_count - 1;
}

/* add a Hoisted definition for a function (cpool_idx >= 0) or a
   global variable (cpool_idx = -1) */
static JSHoistedDef *add_hoisted_def(JSContext *ctx,
                                     JSFunctionDef *s, int cpool_idx,
                                     JSAtom name, int var_idx, BOOL is_lexical)
{
    JSHoistedDef *hf;

    if (js_resize_array(ctx, (void **)&s->hoisted_def,
                        sizeof(s->hoisted_def[0]),
                        &s->hoisted_def_size, s->hoisted_def_count + 1))
        return NULL;
    hf = &s->hoisted_def[s->hoisted_def_count++];
    hf->cpool_idx = cpool_idx;
    hf->force_init = 0;
    hf->is_lexical = is_lexical;
    hf->is_const = FALSE;
    hf->var_idx = var_idx;
    hf->scope_level = s->scope_level;
    hf->var_name = JS_ATOM_NULL;
    if (name != JS_ATOM_NULL) {
        hf->var_name = JS_DupAtom(ctx, name);
    }
    return hf;
}

typedef enum {
    JS_VAR_DEF_WITH,
    JS_VAR_DEF_LET,
    JS_VAR_DEF_CONST,
    JS_VAR_DEF_FUNCTION_DECL, /* function declaration */
    JS_VAR_DEF_NEW_FUNCTION_DECL, /* async/generator function declaration */
    JS_VAR_DEF_CATCH,
    JS_VAR_DEF_VAR,
} JSVarDefEnum;

static int define_var(JSParseState *s, JSFunctionDef *fd, JSAtom name,
                      JSVarDefEnum var_def_type)
{
    JSContext *ctx = s->ctx;
    JSVarDef *vd;
    int idx;

    switch (var_def_type) {
        case JS_VAR_DEF_WITH:
            idx = add_scope_var(ctx, fd, name, JS_VAR_NORMAL);
            break;

        case JS_VAR_DEF_LET:
        case JS_VAR_DEF_CONST:
        case JS_VAR_DEF_FUNCTION_DECL:
        case JS_VAR_DEF_NEW_FUNCTION_DECL:
            idx = find_lexical_decl(ctx, fd, name, fd->scope_first, TRUE);
            if (idx >= 0) {
                if (idx < GLOBAL_VAR_OFFSET) {
                    if (fd->vars[idx].scope_level == fd->scope_level) {
                        /* same scope: in non strict mode, functions
                       can be redefined (annex B.3.3.4). */
                        if (!(!(fd->js_mode & JS_MODE_STRICT) &&
                              var_def_type == JS_VAR_DEF_FUNCTION_DECL &&
                              fd->vars[idx].var_kind == JS_VAR_FUNCTION_DECL)) {
                            goto redef_lex_error;
                        }
                    } else if (fd->vars[idx].var_kind == JS_VAR_CATCH && (fd->vars[idx].scope_level + 2) == fd->scope_level) {
                        goto redef_lex_error;
                    }
                } else {
                    if (fd->scope_level == 1) {
                        redef_lex_error:
                        /* redefining a scoped var in the same scope: error */
                        return js_parse_error(s, "invalid redefinition of lexical identifier");
                    }
                }
            }
            if (var_def_type != JS_VAR_DEF_FUNCTION_DECL &&
                var_def_type != JS_VAR_DEF_NEW_FUNCTION_DECL &&
                fd->scope_level == 1 &&
                find_arg(ctx, fd, name) >= 0) {
                /* lexical variable redefines a parameter name */
                return js_parse_error(s, "invalid redefinition of parameter name");
            }

            if (find_var_in_child_scope(ctx, fd, name, fd->scope_level) >= 0) {
                return js_parse_error(s, "invalid redefinition of a variable");
            }

            if (fd->is_global_var) {
                JSHoistedDef *hf;
                hf = find_hoisted_def(fd, name);
                if (hf && is_child_scope(ctx, fd, hf->scope_level,
                                         fd->scope_level)) {
                    return js_parse_error(s, "invalid redefinition of global identifier");
                }
            }

            if (fd->is_eval &&
                (fd->eval_type == JS_EVAL_TYPE_GLOBAL ||
                 fd->eval_type == JS_EVAL_TYPE_MODULE) &&
                fd->scope_level == 1) {
                JSHoistedDef *hf;
                hf = add_hoisted_def(s->ctx, fd, -1, name, -1, TRUE);
                if (!hf)
                    return -1;
                hf->is_const = (var_def_type == JS_VAR_DEF_CONST);
                idx = GLOBAL_VAR_OFFSET;
            } else {
                JSVarKindEnum var_kind;
                if (var_def_type == JS_VAR_DEF_FUNCTION_DECL)
                    var_kind = JS_VAR_FUNCTION_DECL;
                else if (var_def_type == JS_VAR_DEF_NEW_FUNCTION_DECL)
                    var_kind = JS_VAR_NEW_FUNCTION_DECL;
                else
                    var_kind = JS_VAR_NORMAL;
                idx = add_scope_var(ctx, fd, name, var_kind);
                if (idx >= 0) {
                    vd = &fd->vars[idx];
                    vd->is_lexical = 1;
                    vd->is_const = (var_def_type == JS_VAR_DEF_CONST);
                }
            }
            break;

        case JS_VAR_DEF_CATCH:
            idx = add_scope_var(ctx, fd, name, JS_VAR_CATCH);
            break;

        case JS_VAR_DEF_VAR:
            if (find_lexical_decl(ctx, fd, name, fd->scope_first,
                                  FALSE) >= 0) {
                invalid_lexical_redefinition:
                /* error to redefine a var that inside a lexical scope */
                return js_parse_error(s, "invalid redefinition of lexical identifier");
            }
            if (fd->is_global_var) {
                JSHoistedDef *hf;
                hf = find_hoisted_def(fd, name);
                if (hf && hf->is_lexical && hf->scope_level == fd->scope_level &&
                    fd->eval_type == JS_EVAL_TYPE_MODULE) {
                    goto invalid_lexical_redefinition;
                }
                hf = add_hoisted_def(s->ctx, fd, -1, name, -1, FALSE);
                if (!hf)
                    return -1;
                idx = GLOBAL_VAR_OFFSET;
            } else {
                /* if the variable already exists, don't add it again  */
                idx = find_var(ctx, fd, name);
                if (idx >= 0)
                    break;
                idx = add_var(ctx, fd, name);
                if (idx >= 0) {
                    if (name == JS_ATOM_arguments && fd->has_arguments_binding)
                        fd->arguments_var_idx = idx;
                    fd->vars[idx].func_pool_or_scope_idx = fd->scope_level;
                }
            }
            break;
        default:
            abort();
    }
    return idx;
}

/* add a private field variable in the current scope */
static int add_private_class_field(JSParseState *s, JSFunctionDef *fd,
                                   JSAtom name, JSVarKindEnum var_kind)
{
    JSContext *ctx = s->ctx;
    JSVarDef *vd;
    int idx;

    idx = add_scope_var(ctx, fd, name, var_kind);
    if (idx < 0)
        return idx;
    vd = &fd->vars[idx];
    vd->is_lexical = 1;
    vd->is_const = 1;
    return idx;
}

static __exception int js_parse_expr(JSParseState *s);
static __exception int js_parse_function_decl(JSParseState *s,
                                              JSParseFunctionEnum func_type,
                                              JSFunctionKindEnum func_kind,
                                              JSAtom func_name, const uint8_t *ptr,
                                              int start_line);
static JSFunctionDef *js_parse_function_class_fields_init(JSParseState *s);
static __exception int js_parse_function_decl2(JSParseState *s,
                                               JSParseFunctionEnum func_type,
                                               JSFunctionKindEnum func_kind,
                                               JSAtom func_name,
                                               const uint8_t *ptr,
                                               int function_line_num,
                                               JSParseExportEnum export_flag,
                                               JSFunctionDef **pfd);
static __exception int js_parse_assign_expr(JSParseState *s, int in_accepted);
static __exception int js_parse_unary(JSParseState *s, int exponentiation_flag);
static void push_break_entry(JSFunctionDef *fd, BlockEnv *be,
                             JSAtom label_name,
                             int label_break, int label_cont,
                             int drop_count);
static void pop_break_entry(JSFunctionDef *fd);
static JSExportEntry *add_export_entry(JSParseState *s, JSModuleDef *m,
                                       JSAtom local_name, JSAtom export_name,
                                       JSExportTypeEnum export_type);

/* Note: all the fields are already sealed except length */
static int seal_template_obj(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    JSShapeProperty *prs;

    p = JS_VALUE_GET_OBJ(obj);
    prs = find_own_property1(p, JS_ATOM_length);
    if (prs) {
        if (js_update_property_flags(ctx, p, &prs,
                                     prs->flags & ~(JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE)))
            return -1;
    }
    p->extensible = FALSE;
    return 0;
}

static __exception int js_parse_template(JSParseState *s, int call, int *argc)
{
    JSContext *ctx = s->ctx;
    JSValue raw_array, template_object;
    JSToken cooked;
    int depth, ret;

    raw_array = JS_UNDEFINED; /* avoid warning */
    template_object = JS_UNDEFINED; /* avoid warning */
    if (call) {
        /* Create a template object: an array of cooked strings */
        /* Create an array of raw strings and store it to the raw property */
        template_object = JS_NewArray(ctx);
        if (JS_IsException(template_object))
            return -1;
        //        pool_idx = s->cur_func->cpool_count;
        ret = emit_push_const(s, template_object, 0);
        JS_FreeValue(ctx, template_object);
        if (ret)
            return -1;
        raw_array = JS_NewArray(ctx);
        if (JS_IsException(raw_array))
            return -1;
        if (JS_DefinePropertyValue(ctx, template_object, JS_ATOM_raw,
                                   raw_array, JS_PROP_THROW) < 0) {
            return -1;
        }
    }

    depth = 0;
    while (s->token.val == TOK_TEMPLATE) {
        const uint8_t *p = s->token.ptr + 1;
        cooked = s->token;
        if (call) {
            if (JS_DefinePropertyValueUint32(ctx, raw_array, depth,
                                             JS_DupValue(ctx, s->token.u.str.str),
                                             JS_PROP_ENUMERABLE | JS_PROP_THROW) < 0) {
                return -1;
            }
            /* re-parse the string with escape sequences but do not throw a
               syntax error if it contains invalid sequences
             */
            if (js_parse_string(s, '`', FALSE, p, &cooked, &p)) {
                cooked.u.str.str = JS_UNDEFINED;
            }
            if (JS_DefinePropertyValueUint32(ctx, template_object, depth,
                                             cooked.u.str.str,
                                             JS_PROP_ENUMERABLE | JS_PROP_THROW) < 0) {
                return -1;
            }
        } else {
            JSString *str;
            /* re-parse the string with escape sequences and throw a
               syntax error if it contains invalid sequences
             */
            JS_FreeValue(ctx, s->token.u.str.str);
            s->token.u.str.str = JS_UNDEFINED;
            if (js_parse_string(s, '`', TRUE, p, &cooked, &p))
                return -1;
            str = JS_VALUE_GET_STRING(cooked.u.str.str);
            if (str->len != 0 || depth == 0) {
                ret = emit_push_const(s, cooked.u.str.str, 1);
                JS_FreeValue(s->ctx, cooked.u.str.str);
                if (ret)
                    return -1;
                if (depth == 0) {
                    if (s->token.u.str.sep == '`')
                        goto done1;
                    emit_op(s, OP_get_field2);
                    emit_atom(s, JS_ATOM_concat);
                }
                depth++;
            } else {
                JS_FreeValue(s->ctx, cooked.u.str.str);
            }
        }
        if (s->token.u.str.sep == '`')
            goto done;
        if (next_token(s))
            return -1;
        if (js_parse_expr(s))
            return -1;
        depth++;
        if (s->token.val != '}') {
            return js_parse_error(s, "expected '}' after template expression");
        }
        /* XXX: should convert to string at this stage? */
        free_token(s, &s->token);
        /* Resume TOK_TEMPLATE parsing (s->token.line_num and
         * s->token.ptr are OK) */
        s->got_lf = FALSE;
        s->last_line_num = s->token.line_num;
        if (js_parse_template_part(s, s->buf_ptr))
            return -1;
    }
    return js_parse_expect(s, TOK_TEMPLATE);

    done:
    if (call) {
        /* Seal the objects */
        seal_template_obj(ctx, raw_array);
        seal_template_obj(ctx, template_object);
        *argc = depth + 1;
    } else {
        emit_op(s, OP_call_method);
        emit_u16(s, depth - 1);
    }
    done1:
    return next_token(s);
}


#define PROP_TYPE_IDENT 0
#define PROP_TYPE_VAR   1
#define PROP_TYPE_GET   2
#define PROP_TYPE_SET   3
#define PROP_TYPE_STAR  4
#define PROP_TYPE_ASYNC 5
#define PROP_TYPE_ASYNC_STAR 6

#define PROP_TYPE_PRIVATE (1 << 4)

static BOOL token_is_ident(int tok)
{
    /* Accept keywords and reserved words as property names */
    return (tok == TOK_IDENT ||
            (tok >= TOK_FIRST_KEYWORD &&
             tok <= TOK_LAST_KEYWORD));
}

/* if the property is an expression, name = JS_ATOM_NULL */
static int __exception js_parse_property_name(JSParseState *s,
JSAtom *pname,
        BOOL allow_method, BOOL allow_var,
        BOOL allow_private)
{
int is_private = 0;
BOOL is_non_reserved_ident;
JSAtom name;
int prop_type;

prop_type = PROP_TYPE_IDENT;
if (allow_method) {
if (token_is_pseudo_keyword(s, JS_ATOM_get)
||  token_is_pseudo_keyword(s, JS_ATOM_set)) {
/* get x(), set x() */
name = JS_DupAtom(s->ctx, s->token.u.ident.atom);
if (next_token(s))
goto fail1;
if (s->token.val == ':' || s->token.val == ',' ||
s->token.val == '}' || s->token.val == '(') {
is_non_reserved_ident = TRUE;
goto ident_found;
}
prop_type = PROP_TYPE_GET + (name == JS_ATOM_set);
JS_FreeAtom(s->ctx, name);
} else if (s->token.val == '*') {
if (next_token(s))
goto fail;
prop_type = PROP_TYPE_STAR;
} else if (token_is_pseudo_keyword(s, JS_ATOM_async) &&
peek_token(s, TRUE) != '\n') {
name = JS_DupAtom(s->ctx, s->token.u.ident.atom);
if (next_token(s))
goto fail1;
if (s->token.val == ':' || s->token.val == ',' ||
s->token.val == '}' || s->token.val == '(') {
is_non_reserved_ident = TRUE;
goto ident_found;
}
JS_FreeAtom(s->ctx, name);
if (s->token.val == '*') {
if (next_token(s))
goto fail;
prop_type = PROP_TYPE_ASYNC_STAR;
} else {
prop_type = PROP_TYPE_ASYNC;
}
}
}

if (token_is_ident(s->token.val)) {
/* variable can only be a non-reserved identifier */
is_non_reserved_ident =
(s->token.val == TOK_IDENT && !s->token.u.ident.is_reserved);
/* keywords and reserved words have a valid atom */
name = JS_DupAtom(s->ctx, s->token.u.ident.atom);
if (next_token(s))
goto fail1;
ident_found:
if (is_non_reserved_ident &&
prop_type == PROP_TYPE_IDENT && allow_var) {
if (!(s->token.val == ':' ||
(s->token.val == '(' && allow_method))) {
prop_type = PROP_TYPE_VAR;
}
}
} else if (s->token.val == TOK_STRING) {
name = JS_ValueToAtom(s->ctx, s->token.u.str.str);
if (name == JS_ATOM_NULL)
goto fail;
if (next_token(s))
goto fail1;
} else if (s->token.val == TOK_NUMBER) {
JSValue val;
val = s->token.u.num.val;
#ifdef CONFIG_BIGNUM
if (JS_VALUE_GET_TAG(val) == JS_TAG_BIG_FLOAT) {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            val = s->ctx->rt->bigfloat_ops.
                mul_pow10_to_float64(s->ctx, &p->num,
                                     s->token.u.num.exponent);
            if (JS_IsException(val))
                goto fail;
            name = JS_ValueToAtom(s->ctx, val);
            JS_FreeValue(s->ctx, val);
        } else
#endif
{
name = JS_ValueToAtom(s->ctx, val);
}
if (name == JS_ATOM_NULL)
goto fail;
if (next_token(s))
goto fail1;
} else if (s->token.val == '[') {
if (next_token(s))
goto fail;
if (js_parse_expr(s))
goto fail;
if (js_parse_expect(s, ']'))
goto fail;
name = JS_ATOM_NULL;
} else if (s->token.val == TOK_PRIVATE_NAME && allow_private) {
name = JS_DupAtom(s->ctx, s->token.u.ident.atom);
if (next_token(s))
goto fail1;
is_private = PROP_TYPE_PRIVATE;
} else {
goto invalid_prop;
}
if (prop_type != PROP_TYPE_IDENT && prop_type != PROP_TYPE_VAR &&
s->token.val != '(') {
JS_FreeAtom(s->ctx, name);
invalid_prop:
js_parse_error(s, "invalid property name");
goto fail;
}
*pname = name;
return prop_type | is_private;
fail1:
JS_FreeAtom(s->ctx, name);
fail:
*pname = JS_ATOM_NULL;
return -1;
}

typedef struct JSParsePos {
    int last_line_num;
    int line_num;
    BOOL got_lf;
    const uint8_t *ptr;
} JSParsePos;

static int js_parse_get_pos(JSParseState *s, JSParsePos *sp)
{
    sp->last_line_num = s->last_line_num;
    sp->line_num = s->token.line_num;
    sp->ptr = s->token.ptr;
    sp->got_lf = s->got_lf;
    return 0;
}

static __exception int js_parse_seek_token(JSParseState *s, const JSParsePos *sp)
{
    s->token.line_num = sp->last_line_num;
    s->line_num = sp->line_num;
    s->buf_ptr = sp->ptr;
    s->got_lf = sp->got_lf;
    return next_token(s);
}

/* return TRUE if a regexp literal is allowed after this token */
static BOOL is_regexp_allowed(int tok)
{
    switch (tok) {
        case TOK_NUMBER:
        case TOK_STRING:
        case TOK_REGEXP:
        case TOK_DEC:
        case TOK_INC:
        case TOK_NULL:
        case TOK_FALSE:
        case TOK_TRUE:
        case TOK_THIS:
        case ')':
        case ']':
        case '}': /* XXX: regexp may occur after */
        case TOK_IDENT:
            return FALSE;
        default:
            return TRUE;
    }
}

#define SKIP_HAS_SEMI      (1 << 0)
#define SKIP_HAS_ELLIPSIS  (1 << 1)

/* XXX: improve speed with early bailout */
/* XXX: no longer works if regexps are present. Could use previous
   regexp parsing heuristics to handle most cases */
static int js_parse_skip_parens_token(JSParseState *s, int *pbits, BOOL no_line_terminator)
{
    char state[256];
    size_t level = 0;
    JSParsePos pos;
    int last_tok, tok = TOK_EOF;
    int c, tok_len, bits = 0;

    /* protect from underflow */
    state[level++] = 0;

    js_parse_get_pos(s, &pos);
    last_tok = 0;
    for (;;) {
        switch(s->token.val) {
            case '(':
            case '[':
            case '{':
                if (level >= sizeof(state))
                    goto done;
                state[level++] = s->token.val;
                break;
            case ')':
                if (state[--level] != '(')
                    goto done;
                break;
            case ']':
                if (state[--level] != '[')
                    goto done;
                break;
            case '}':
                c = state[--level];
                if (c == '`') {
                    /* continue the parsing of the template */
                    free_token(s, &s->token);
                    /* Resume TOK_TEMPLATE parsing (s->token.line_num and
                 * s->token.ptr are OK) */
                    s->got_lf = FALSE;
                    s->last_line_num = s->token.line_num;
                    if (js_parse_template_part(s, s->buf_ptr))
                        goto done;
                    goto handle_template;
                } else if (c != '{') {
                    goto done;
                }
                break;
            case TOK_TEMPLATE:
            handle_template:
                if (s->token.u.str.sep != '`') {
                    /* '${' inside the template : closing '}' and continue
                   parsing the template */
                    if (level >= sizeof(state))
                        goto done;
                    state[level++] = '`';
                }
                break;
            case TOK_EOF:
                goto done;
            case ';':
                if (level == 2) {
                    bits |= SKIP_HAS_SEMI;
                }
                break;
            case TOK_ELLIPSIS:
                if (level == 2) {
                    bits |= SKIP_HAS_ELLIPSIS;
                }
                break;

            case TOK_DIV_ASSIGN:
                tok_len = 2;
                goto parse_regexp;
            case '/':
                tok_len = 1;
            parse_regexp:
                if (is_regexp_allowed(last_tok)) {
                    s->buf_ptr -= tok_len;
                    if (js_parse_regexp(s)) {
                        /* XXX: should clear the exception */
                        goto done;
                    }
                }
                break;
        }
        /* last_tok is only used to recognize regexps */
        if (s->token.val == TOK_IDENT &&
            (token_is_pseudo_keyword(s, JS_ATOM_of) ||
             token_is_pseudo_keyword(s, JS_ATOM_yield))) {
            last_tok = TOK_OF;
        } else {
            last_tok = s->token.val;
        }
        if (next_token(s)) {
            /* XXX: should clear the exception generated by next_token() */
            break;
        }
        if (level <= 1) {
            tok = s->token.val;
            if (token_is_pseudo_keyword(s, JS_ATOM_of))
                tok = TOK_OF;
            if (no_line_terminator && s->last_line_num != s->token.line_num)
                tok = '\n';
            break;
        }
    }
    done:
    if (pbits) {
        *pbits = bits;
    }
    if (js_parse_seek_token(s, &pos))
        return -1;
    return tok;
}

static void set_object_name(JSParseState *s, JSAtom name)
{
    JSFunctionDef *fd = s->cur_func;
    int opcode;

    opcode = get_prev_opcode(fd);
    if (opcode == OP_set_name) {
        /* XXX: should free atom after OP_set_name? */
        fd->byte_code.size = fd->last_opcode_pos;
        fd->last_opcode_pos = -1;
        emit_op(s, OP_set_name);
        emit_atom(s, name);
    } else if (opcode == OP_set_class_name) {
        int define_class_pos;
        JSAtom atom;
        define_class_pos = fd->last_opcode_pos + 1 -
                           get_u32(fd->byte_code.buf + fd->last_opcode_pos + 1);
        assert(fd->byte_code.buf[define_class_pos] == OP_define_class);
        /* for consistency we free the previous atom which is
           JS_ATOM_empty_string */
        atom = get_u32(fd->byte_code.buf + define_class_pos + 1);
        JS_FreeAtom(s->ctx, atom);
        put_u32(fd->byte_code.buf + define_class_pos + 1,
                JS_DupAtom(s->ctx, name));
        fd->last_opcode_pos = -1;
    }
}

static void set_object_name_computed(JSParseState *s)
{
    JSFunctionDef *fd = s->cur_func;
    int opcode;

    opcode = get_prev_opcode(fd);
    if (opcode == OP_set_name) {
        /* XXX: should free atom after OP_set_name? */
        fd->byte_code.size = fd->last_opcode_pos;
        fd->last_opcode_pos = -1;
        emit_op(s, OP_set_name_computed);
    } else if (opcode == OP_set_class_name) {
        int define_class_pos;
        define_class_pos = fd->last_opcode_pos + 1 -
                           get_u32(fd->byte_code.buf + fd->last_opcode_pos + 1);
        assert(fd->byte_code.buf[define_class_pos] == OP_define_class);
        fd->byte_code.buf[define_class_pos] = OP_define_class_computed;
        fd->last_opcode_pos = -1;
    }
}

static __exception int js_parse_object_literal(JSParseState *s)
{
    JSAtom name = JS_ATOM_NULL;
    const uint8_t *start_ptr;
    int start_line, prop_type;
    BOOL has_proto;

    if (next_token(s))
        goto fail;
    /* XXX: add an initial length that will be patched back */
    emit_op(s, OP_object);
    has_proto = FALSE;
    while (s->token.val != '}') {
        /* specific case for getter/setter */
        start_ptr = s->token.ptr;
        start_line = s->token.line_num;

        if (s->token.val == TOK_ELLIPSIS) {
            if (next_token(s))
                return -1;
            if (js_parse_assign_expr(s, TRUE))
                return -1;
            emit_op(s, OP_null);  /* dummy excludeList */
            emit_op(s, OP_copy_data_properties);
            emit_u8(s, 2 | (1 << 2) | (0 << 5));
            emit_op(s, OP_drop); /* pop excludeList */
            emit_op(s, OP_drop); /* pop src object */
            goto next;
        }

        prop_type = js_parse_property_name(s, &name, TRUE, TRUE, FALSE);
        if (prop_type < 0)
            goto fail;

        if (prop_type == PROP_TYPE_VAR) {
            /* shortcut for x: x */
            emit_op(s, OP_scope_get_var);
            emit_atom(s, name);
            emit_u16(s, s->cur_func->scope_level);
            emit_op(s, OP_define_field);
            emit_atom(s, name);
        } else if (s->token.val == '(') {
            BOOL is_getset = (prop_type == PROP_TYPE_GET ||
                              prop_type == PROP_TYPE_SET);
            JSParseFunctionEnum func_type;
            JSFunctionKindEnum func_kind;
            int op_flags;

            func_kind = JS_FUNC_NORMAL;
            if (is_getset) {
                func_type = JS_PARSE_FUNC_GETTER + prop_type - PROP_TYPE_GET;
            } else {
                func_type = JS_PARSE_FUNC_METHOD;
                if (prop_type == PROP_TYPE_STAR)
                    func_kind = JS_FUNC_GENERATOR;
                else if (prop_type == PROP_TYPE_ASYNC)
                    func_kind = JS_FUNC_ASYNC;
                else if (prop_type == PROP_TYPE_ASYNC_STAR)
                    func_kind = JS_FUNC_ASYNC_GENERATOR;
            }
            if (js_parse_function_decl(s, func_type, func_kind, JS_ATOM_NULL,
                                       start_ptr, start_line))
                goto fail;
            if (name == JS_ATOM_NULL) {
                emit_op(s, OP_define_method_computed);
            } else {
                emit_op(s, OP_define_method);
                emit_atom(s, name);
            }
            if (is_getset) {
                op_flags = OP_DEFINE_METHOD_GETTER +
                           prop_type - PROP_TYPE_GET;
            } else {
                op_flags = OP_DEFINE_METHOD_METHOD;
            }
            emit_u8(s, op_flags | OP_DEFINE_METHOD_ENUMERABLE);
        } else {
            if (js_parse_expect(s, ':'))
                goto fail;
            if (js_parse_assign_expr(s, TRUE))
                goto fail;
            if (name == JS_ATOM_NULL) {
                set_object_name_computed(s);
                emit_op(s, OP_define_array_el);
                emit_op(s, OP_drop);
            } else if (name == JS_ATOM___proto__) {
                if (has_proto) {
                    js_parse_error(s, "duplicate __proto__ property name");
                    goto fail;
                }
                emit_op(s, OP_set_proto);
                has_proto = TRUE;
            } else {
                set_object_name(s, name);
                emit_op(s, OP_define_field);
                emit_atom(s, name);
            }
        }
        JS_FreeAtom(s->ctx, name);
        next:
        name = JS_ATOM_NULL;
        if (s->token.val != ',')
            break;
        if (next_token(s))
            goto fail;
    }
    if (js_parse_expect(s, '}'))
        goto fail;
    return 0;
    fail:
    JS_FreeAtom(s->ctx, name);
    return -1;
}

static __exception int js_parse_postfix_expr(JSParseState *s,
                                             BOOL accept_lparen);

/* XXX: is there is nicer solution ? */
static __exception int js_parse_class_default_ctor(JSParseState *s,
                                                   BOOL has_super,
                                                   JSFunctionDef **pfd)
{
    JSParsePos pos;
    const char *str;
    int ret, line_num;
    JSParseFunctionEnum func_type;
    const uint8_t *saved_buf_end;

    js_parse_get_pos(s, &pos);
    if (has_super) {
        str = "(...a){super(...a);}";
        func_type = JS_PARSE_FUNC_DERIVED_CLASS_CONSTRUCTOR;
    } else {
        str = "(){}";
        func_type = JS_PARSE_FUNC_CLASS_CONSTRUCTOR;
    }
    line_num = s->token.line_num;
    saved_buf_end = s->buf_end;
    s->buf_ptr = (uint8_t *)str;
    s->buf_end = (uint8_t *)(str + strlen(str));
    ret = next_token(s);
    if (!ret) {
        ret = js_parse_function_decl2(s, func_type, JS_FUNC_NORMAL,
                                      JS_ATOM_NULL, (uint8_t *)str,
                                      line_num, JS_PARSE_EXPORT_NONE, pfd);
    }
    s->buf_end = saved_buf_end;
    ret |= js_parse_seek_token(s, &pos);
    return ret;
}

/* find field in the current scope */
static int find_private_class_field(JSContext *ctx, JSFunctionDef *fd,
                                    JSAtom name, int scope_level)
{
    int idx;
    idx = fd->scopes[scope_level].first;
    while (idx != -1) {
        if (fd->vars[idx].scope_level != scope_level)
            break;
        if (fd->vars[idx].var_name == name)
            return idx;
        idx = fd->vars[idx].scope_next;
    }
    return -1;
}

/* initialize the class fields, called by the constructor. Note:
   super() can be called in an arrow function, so <this> and
   <class_fields_init> can be variable references */
static void emit_class_field_init(JSParseState *s)
{
    int label_next;

    emit_op(s, OP_scope_get_var);
    emit_atom(s, JS_ATOM_class_fields_init);
    emit_u16(s, s->cur_func->scope_level);

    /* no need to call the class field initializer if not defined */
    emit_op(s, OP_dup);
    label_next = emit_goto(s, OP_if_false, -1);

    emit_op(s, OP_scope_get_var);
    emit_atom(s, JS_ATOM_this);
    emit_u16(s, 0);

    emit_op(s, OP_swap);

    emit_op(s, OP_call_method);
    emit_u16(s, 0);

    emit_label(s, label_next);
    emit_op(s, OP_drop);
}

/* build a private setter function name from the private getter name */
static JSAtom get_private_setter_name(JSContext *ctx, JSAtom name)
{
    return js_atom_concat_str(ctx, name, "<set>");
}

typedef struct {
    JSFunctionDef *fields_init_fd;
    int computed_fields_count;
    BOOL has_brand;
    int brand_push_pos;
} ClassFieldsDef;

static __exception int emit_class_init_start(JSParseState *s,
                                             ClassFieldsDef *cf)
{
    int label_add_brand;

    cf->fields_init_fd = js_parse_function_class_fields_init(s);
    if (!cf->fields_init_fd)
        return -1;

    s->cur_func = cf->fields_init_fd;

    /* XXX: would be better to add the code only if needed, maybe in a
       later pass */
    emit_op(s, OP_push_false); /* will be patched later */
    cf->brand_push_pos = cf->fields_init_fd->last_opcode_pos;
    label_add_brand = emit_goto(s, OP_if_false, -1);

    emit_op(s, OP_scope_get_var);
    emit_atom(s, JS_ATOM_this);
    emit_u16(s, 0);

    emit_op(s, OP_scope_get_var);
    emit_atom(s, JS_ATOM_home_object);
    emit_u16(s, 0);

    emit_op(s, OP_add_brand);

    emit_label(s, label_add_brand);

    s->cur_func = s->cur_func->parent;
    return 0;
}

static __exception int add_brand(JSParseState *s, ClassFieldsDef *cf)
{
    if (!cf->has_brand) {
        /* define the brand field in 'this' of the initializer */
        if (!cf->fields_init_fd) {
            if (emit_class_init_start(s, cf))
                return -1;
        }
        /* patch the start of the function to enable the OP_add_brand code */
        cf->fields_init_fd->byte_code.buf[cf->brand_push_pos] = OP_push_true;

        cf->has_brand = TRUE;
    }
    return 0;
}

static void emit_class_init_end(JSParseState *s, ClassFieldsDef *cf)
{
    int cpool_idx;

    s->cur_func = cf->fields_init_fd;
    emit_op(s, OP_return_undef);
    s->cur_func = s->cur_func->parent;

    cpool_idx = cpool_add(s, JS_NULL);
    cf->fields_init_fd->parent_cpool_idx = cpool_idx;
    emit_op(s, OP_fclosure);
    emit_u32(s, cpool_idx);
    emit_op(s, OP_set_home_object);
}


static __exception int js_parse_class(JSParseState *s, BOOL is_class_expr,
                                      JSParseExportEnum export_flag)
{
    JSContext *ctx = s->ctx;
    JSFunctionDef *fd = s->cur_func;
    JSAtom name = JS_ATOM_NULL, class_name = JS_ATOM_NULL, class_name1;
    JSAtom class_var_name = JS_ATOM_NULL;
    JSFunctionDef *method_fd, *ctor_fd;
    int saved_js_mode, class_name_var_idx, prop_type, ctor_cpool_offset;
    int class_flags = 0, i, define_class_offset;
    BOOL is_static, is_private;
    const uint8_t *class_start_ptr = s->token.ptr;
    const uint8_t *start_ptr;
    ClassFieldsDef class_fields[2];

    /* classes are parsed and executed in strict mode */
    saved_js_mode = fd->js_mode;
    fd->js_mode |= JS_MODE_STRICT;
    if (next_token(s))
        goto fail;
    if (s->token.val == TOK_IDENT) {
        if (s->token.u.ident.is_reserved) {
            js_parse_error_reserved_identifier(s);
            goto fail;
        }
        class_name = JS_DupAtom(ctx, s->token.u.ident.atom);
        if (next_token(s))
            goto fail;
    } else if (!is_class_expr && export_flag != JS_PARSE_EXPORT_DEFAULT) {
        js_parse_error(s, "class statement requires a name");
        goto fail;
    }
    if (!is_class_expr) {
        if (class_name == JS_ATOM_NULL)
            class_var_name = JS_ATOM__default_; /* export default */
        else
            class_var_name = class_name;
        class_var_name = JS_DupAtom(ctx, class_var_name);
    }

    push_scope(s);

    if (s->token.val == TOK_EXTENDS) {
        class_flags = JS_DEFINE_CLASS_HAS_HERITAGE;
        if (next_token(s))
            goto fail;
        /* XXX: the grammar only allows LeftHandSideExpression */
        if (js_parse_postfix_expr(s, TRUE))
            goto fail;
    } else {
        emit_op(s, OP_undefined);
    }

    /* add a 'const' definition for the class name */
    if (class_name != JS_ATOM_NULL) {
        class_name_var_idx = define_var(s, fd, class_name, JS_VAR_DEF_CONST);
        if (class_name_var_idx < 0)
            goto fail;
    }

    if (js_parse_expect(s, '{'))
        goto fail;

    /* this scope contains the private fields */
    push_scope(s);

    emit_op(s, OP_push_const);
    ctor_cpool_offset = fd->byte_code.size;
    emit_u32(s, 0); /* will be patched at the end of the class parsing */

    if (class_name == JS_ATOM_NULL) {
        if (class_var_name != JS_ATOM_NULL)
            class_name1 = JS_ATOM_default;
        else
            class_name1 = JS_ATOM_empty_string;
    } else {
        class_name1 = class_name;
    }

    emit_op(s, OP_define_class);
    emit_atom(s, class_name1);
    emit_u8(s, class_flags);
    define_class_offset = fd->last_opcode_pos;

    for(i = 0; i < 2; i++) {
        ClassFieldsDef *cf = &class_fields[i];
        cf->fields_init_fd = NULL;
        cf->computed_fields_count = 0;
        cf->has_brand = FALSE;
    }

    ctor_fd = NULL;
    while (s->token.val != '}') {
        if (s->token.val == ';') {
            if (next_token(s))
                goto fail;
            continue;
        }
        is_static = (s->token.val == TOK_STATIC);
        prop_type = -1;
        if (is_static) {
            if (next_token(s))
                goto fail;
            /* allow "static" field name */
            if (s->token.val == ';' || s->token.val == '=') {
                is_static = FALSE;
                name = JS_DupAtom(ctx, JS_ATOM_static);
                prop_type = PROP_TYPE_IDENT;
            }
        }
        if (is_static)
            emit_op(s, OP_swap);
        start_ptr = s->token.ptr;
        if (prop_type < 0) {
            prop_type = js_parse_property_name(s, &name, TRUE, FALSE, TRUE);
            if (prop_type < 0)
                goto fail;
        }
        is_private = prop_type & PROP_TYPE_PRIVATE;
        prop_type &= ~PROP_TYPE_PRIVATE;

        if ((name == JS_ATOM_constructor && !is_static &&
             prop_type != PROP_TYPE_IDENT) ||
            (name == JS_ATOM_prototype && is_static) ||
            name == JS_ATOM_hash_constructor) {
            js_parse_error(s, "invalid method name");
            goto fail;
        }
        if (prop_type == PROP_TYPE_GET || prop_type == PROP_TYPE_SET) {
            BOOL is_set = prop_type - PROP_TYPE_GET;
            JSFunctionDef *method_fd;

            if (is_private) {
                int idx, var_kind;
                idx = find_private_class_field(ctx, fd, name, fd->scope_level);
                if (idx >= 0) {
                    var_kind = fd->vars[idx].var_kind;
                    if (var_kind == JS_VAR_PRIVATE_FIELD ||
                        var_kind == JS_VAR_PRIVATE_METHOD ||
                        var_kind == JS_VAR_PRIVATE_GETTER_SETTER ||
                        var_kind == (JS_VAR_PRIVATE_GETTER + is_set)) {
                        goto private_field_already_defined;
                    }
                    fd->vars[idx].var_kind = JS_VAR_PRIVATE_GETTER_SETTER;
                } else {
                    if (add_private_class_field(s, fd, name,
                                                JS_VAR_PRIVATE_GETTER + is_set) < 0)
                        goto fail;
                }
                if (add_brand(s, &class_fields[is_static]) < 0)
                    goto fail;
            }

            if (js_parse_function_decl2(s, JS_PARSE_FUNC_GETTER + is_set,
                                        JS_FUNC_NORMAL, JS_ATOM_NULL,
                                        start_ptr, s->token.line_num,
                                        JS_PARSE_EXPORT_NONE, &method_fd))
                goto fail;
            if (is_private) {
                method_fd->need_home_object = TRUE; /* needed for brand check */
                emit_op(s, OP_set_home_object);
                /* XXX: missing function name */
                emit_op(s, OP_scope_put_var_init);
                if (is_set) {
                    JSAtom setter_name;
                    int ret;

                    setter_name = get_private_setter_name(ctx, name);
                    if (setter_name == JS_ATOM_NULL)
                        goto fail;
                    emit_atom(s, setter_name);
                    ret = add_private_class_field(s, fd, setter_name,
                                                  JS_VAR_PRIVATE_SETTER);
                    JS_FreeAtom(ctx, setter_name);
                    if (ret < 0)
                        goto fail;
                } else {
                    emit_atom(s, name);
                }
                emit_u16(s, s->cur_func->scope_level);
            } else {
                if (name == JS_ATOM_NULL) {
                    emit_op(s, OP_define_method_computed);
                } else {
                    emit_op(s, OP_define_method);
                    emit_atom(s, name);
                }
                emit_u8(s, OP_DEFINE_METHOD_GETTER + is_set);
            }
        } else if (prop_type == PROP_TYPE_IDENT && s->token.val != '(') {
            ClassFieldsDef *cf = &class_fields[is_static];
            JSAtom field_var_name = JS_ATOM_NULL;

            /* class field */

            /* XXX: spec: not consistent with method name checks */
            if (name == JS_ATOM_constructor || name == JS_ATOM_prototype) {
                js_parse_error(s, "invalid field name");
                goto fail;
            }

            if (is_private) {
                if (find_private_class_field(ctx, fd, name,
                                             fd->scope_level) >= 0) {
                    goto private_field_already_defined;
                }
                if (add_private_class_field(s, fd, name,
                                            JS_VAR_PRIVATE_FIELD) < 0)
                    goto fail;
                emit_op(s, OP_private_symbol);
                emit_atom(s, name);
                emit_op(s, OP_scope_put_var_init);
                emit_atom(s, name);
                emit_u16(s, s->cur_func->scope_level);
            }

            if (!cf->fields_init_fd) {
                if (emit_class_init_start(s, cf))
                    goto fail;
            }
            if (name == JS_ATOM_NULL ) {
                /* save the computed field name into a variable */
                field_var_name = js_atom_concat_num(ctx, JS_ATOM_computed_field + is_static, cf->computed_fields_count);
                if (field_var_name == JS_ATOM_NULL)
                    goto fail;
                if (define_var(s, fd, field_var_name, JS_VAR_DEF_CONST) < 0) {
                    JS_FreeAtom(ctx, field_var_name);
                    goto fail;
                }
                emit_op(s, OP_to_propkey);
                emit_op(s, OP_scope_put_var_init);
                emit_atom(s, field_var_name);
                emit_u16(s, s->cur_func->scope_level);
            }
            s->cur_func = cf->fields_init_fd;
            emit_op(s, OP_scope_get_var);
            emit_atom(s, JS_ATOM_this);
            emit_u16(s, 0);

            if (name == JS_ATOM_NULL) {
                emit_op(s, OP_scope_get_var);
                emit_atom(s, field_var_name);
                emit_u16(s, s->cur_func->scope_level);
                cf->computed_fields_count++;
                JS_FreeAtom(ctx, field_var_name);
            } else if (is_private) {
                emit_op(s, OP_scope_get_var);
                emit_atom(s, name);
                emit_u16(s, s->cur_func->scope_level);
            }

            if (s->token.val == '=') {
                if (next_token(s))
                    goto fail;
                if (js_parse_assign_expr(s, TRUE))
                    goto fail;
            } else {
                emit_op(s, OP_undefined);
            }
            if (is_private) {
                set_object_name_computed(s);
                emit_op(s, OP_define_private_field);
            } else if (name == JS_ATOM_NULL) {
                set_object_name_computed(s);
                emit_op(s, OP_define_array_el);
                emit_op(s, OP_drop);
            } else {
                set_object_name(s, name);
                emit_op(s, OP_define_field);
                emit_atom(s, name);
            }
            s->cur_func = s->cur_func->parent;
            if (js_parse_expect_semi(s))
                goto fail;
        } else {
            JSParseFunctionEnum func_type;
            JSFunctionKindEnum func_kind;

            func_type = JS_PARSE_FUNC_METHOD;
            func_kind = JS_FUNC_NORMAL;
            if (prop_type == PROP_TYPE_STAR) {
                func_kind = JS_FUNC_GENERATOR;
            } else if (prop_type == PROP_TYPE_ASYNC) {
                func_kind = JS_FUNC_ASYNC;
            } else if (prop_type == PROP_TYPE_ASYNC_STAR) {
                func_kind = JS_FUNC_ASYNC_GENERATOR;
            } else if (name == JS_ATOM_constructor && !is_static) {
                if (ctor_fd) {
                    js_parse_error(s, "property constructor appears more than once");
                    goto fail;
                }
                if (class_flags & JS_DEFINE_CLASS_HAS_HERITAGE)
                    func_type = JS_PARSE_FUNC_DERIVED_CLASS_CONSTRUCTOR;
                else
                    func_type = JS_PARSE_FUNC_CLASS_CONSTRUCTOR;
            }
            if (is_private) {
                if (add_brand(s, &class_fields[is_static]) < 0)
                    goto fail;
            }
            if (js_parse_function_decl2(s, func_type, func_kind, JS_ATOM_NULL, start_ptr, s->token.line_num, JS_PARSE_EXPORT_NONE, &method_fd))
                goto fail;
            if (func_type == JS_PARSE_FUNC_DERIVED_CLASS_CONSTRUCTOR ||
                func_type == JS_PARSE_FUNC_CLASS_CONSTRUCTOR) {
                ctor_fd = method_fd;
            } else if (is_private) {
                method_fd->need_home_object = TRUE; /* needed for brand check */
                if (find_private_class_field(ctx, fd, name,
                                             fd->scope_level) >= 0) {
                    private_field_already_defined:
                    js_parse_error(s, "private class field is already defined");
                    goto fail;
                }
                if (add_private_class_field(s, fd, name,
                                            JS_VAR_PRIVATE_METHOD) < 0)
                    goto fail;
                emit_op(s, OP_set_home_object);
                emit_op(s, OP_set_name);
                emit_atom(s, name);
                emit_op(s, OP_scope_put_var_init);
                emit_atom(s, name);
                emit_u16(s, s->cur_func->scope_level);
            } else {
                if (name == JS_ATOM_NULL) {
                    emit_op(s, OP_define_method_computed);
                } else {
                    emit_op(s, OP_define_method);
                    emit_atom(s, name);
                }
                emit_u8(s, OP_DEFINE_METHOD_METHOD);
            }
        }
        if (is_static)
            emit_op(s, OP_swap);
        JS_FreeAtom(ctx, name);
        name = JS_ATOM_NULL;
    }

    if (s->token.val != '}') {
        js_parse_error(s, "expecting '%c'", '}');
        goto fail;
    }

    if (!ctor_fd) {
        if (js_parse_class_default_ctor(s, class_flags & JS_DEFINE_CLASS_HAS_HERITAGE, &ctor_fd))
            goto fail;
    }
    /* patch the constant pool index for the constructor */
    put_u32(fd->byte_code.buf + ctor_cpool_offset, ctor_fd->parent_cpool_idx);

    /* store the class source code in the constructor. */
    if (!(fd->js_mode & JS_MODE_STRIP)) {
        js_free(ctx, ctor_fd->source);
        ctor_fd->source_len = s->buf_ptr - class_start_ptr;
        ctor_fd->source = js_strndup(ctx, (const char *)class_start_ptr,
                                     ctor_fd->source_len);
        if (!ctor_fd->source)
            goto fail;
    }

    /* consume the '}' */
    if (next_token(s))
        goto fail;

    /* store the function to initialize the fields to that it can be
       referenced by the constructor */
    {
        ClassFieldsDef *cf = &class_fields[0];
        int var_idx;

        var_idx = define_var(s, fd, JS_ATOM_class_fields_init,
                             JS_VAR_DEF_CONST);
        if (var_idx < 0)
            goto fail;
        if (cf->fields_init_fd) {
            emit_class_init_end(s, cf);
        } else {
            emit_op(s, OP_undefined);
        }
        emit_op(s, OP_scope_put_var_init);
        emit_atom(s, JS_ATOM_class_fields_init);
        emit_u16(s, s->cur_func->scope_level);
    }

    /* drop the prototype */
    emit_op(s, OP_drop);

    /* initialize the static fields */
    if (class_fields[1].fields_init_fd != NULL) {
        ClassFieldsDef *cf = &class_fields[1];
        emit_op(s, OP_dup);
        emit_class_init_end(s, cf);
        emit_op(s, OP_call_method);
        emit_u16(s, 0);
        emit_op(s, OP_drop);
    }

    if (class_name != JS_ATOM_NULL) {
        /* store the class name in the scoped class name variable (it
           is independent from the class statement variable
           definition) */
        emit_op(s, OP_dup);
        emit_op(s, OP_scope_put_var_init);
        emit_atom(s, class_name);
        emit_u16(s, fd->scope_level);
    }
    pop_scope(s);
    pop_scope(s);

    /* the class statements have a block level scope */
    if (class_var_name != JS_ATOM_NULL) {
        if (define_var(s, fd, class_var_name, JS_VAR_DEF_LET) < 0)
            goto fail;
        emit_op(s, OP_scope_put_var_init);
        emit_atom(s, class_var_name);
        emit_u16(s, fd->scope_level);
    } else {
        if (class_name == JS_ATOM_NULL) {
            /* cannot use OP_set_name because the name of the class
               must be defined before the static initializers are
               executed */
            emit_op(s, OP_set_class_name);
            emit_u32(s, fd->last_opcode_pos + 1 - define_class_offset);
        }
    }

    if (export_flag != JS_PARSE_EXPORT_NONE) {
        if (!add_export_entry(s, fd->module,
                              class_var_name,
                              export_flag == JS_PARSE_EXPORT_NAMED ? class_var_name : JS_ATOM_default,
                              JS_EXPORT_TYPE_LOCAL))
            goto fail;
    }

    JS_FreeAtom(ctx, class_name);
    JS_FreeAtom(ctx, class_var_name);
    fd->js_mode = saved_js_mode;
    return 0;
    fail:
    JS_FreeAtom(ctx, name);
    JS_FreeAtom(ctx, class_name);
    JS_FreeAtom(ctx, class_var_name);
    fd->js_mode = saved_js_mode;
    return -1;
}

static __exception int js_parse_array_literal(JSParseState *s)
{
    uint32_t idx;
    BOOL need_length;

    if (next_token(s))
        return -1;
    /* small regular arrays are created on the stack */
    idx = 0;
    while (s->token.val != ']' && idx < 32) {
        if (s->token.val == ',' || s->token.val == TOK_ELLIPSIS)
            break;
        if (js_parse_assign_expr(s, TRUE))
            return -1;
        idx++;
        /* accept trailing comma */
        if (s->token.val == ',') {
            if (next_token(s))
                return -1;
        } else
        if (s->token.val != ']')
            goto done;
    }
    emit_op(s, OP_array_from);
    emit_u16(s, idx);

    /* larger arrays and holes are handled with explicit indices */
    need_length = FALSE;
    while (s->token.val != ']' && idx < 0x7fffffff) {
        if (s->token.val == TOK_ELLIPSIS)
            break;
        need_length = TRUE;
        if (s->token.val != ',') {
            if (js_parse_assign_expr(s, TRUE))
                return -1;
            emit_op(s, OP_define_field);
            emit_u32(s, __JS_AtomFromUInt32(idx));
            need_length = FALSE;
        }
        idx++;
        /* accept trailing comma */
        if (s->token.val == ',') {
            if (next_token(s))
                return -1;
        }
    }
    if (s->token.val == ']') {
        if (need_length) {
            /* Set the length: Cannot use OP_define_field because
               length is not configurable */
            emit_op(s, OP_dup);
            emit_op(s, OP_push_i32);
            emit_u32(s, idx);
            emit_op(s, OP_put_field);
            emit_atom(s, JS_ATOM_length);
        }
        goto done;
    }

    /* huge arrays and spread elements require a dynamic index on the stack */
    emit_op(s, OP_push_i32);
    emit_u32(s, idx);

    /* stack has array, index */
    while (s->token.val != ']') {
        if (s->token.val == TOK_ELLIPSIS) {
            if (next_token(s))
                return -1;
            if (js_parse_assign_expr(s, TRUE))
                return -1;
#if 1
            emit_op(s, OP_append);
#else
            int label_next, label_done;
            label_next = new_label(s);
            label_done = new_label(s);
            /* enumerate object */
            emit_op(s, OP_for_of_start);
            emit_op(s, OP_rot5l);
            emit_op(s, OP_rot5l);
            emit_label(s, label_next);
            /* on stack: enum_rec array idx */
            emit_op(s, OP_for_of_next);
            emit_u8(s, 2);
            emit_goto(s, OP_if_true, label_done);
            /* append element */
            /* enum_rec array idx val -> enum_rec array new_idx */
            emit_op(s, OP_define_array_el);
            emit_op(s, OP_inc);
            emit_goto(s, OP_goto, label_next);
            emit_label(s, label_done);
            /* close enumeration */
            emit_op(s, OP_drop); /* drop undef val */
            emit_op(s, OP_nip1); /* drop enum_rec */
            emit_op(s, OP_nip1);
            emit_op(s, OP_nip1);
#endif
        } else {
            need_length = TRUE;
            if (s->token.val != ',') {
                if (js_parse_assign_expr(s, TRUE))
                    return -1;
                /* a idx val */
                emit_op(s, OP_define_array_el);
                need_length = FALSE;
            }
            emit_op(s, OP_inc);
        }
        if (s->token.val != ',')
            break;
        if (next_token(s))
            return -1;
    }
    if (need_length) {
        /* Set the length: cannot use OP_define_field because
           length is not configurable */
        emit_op(s, OP_dup1);    /* array length - array array length */
        emit_op(s, OP_put_field);
        emit_atom(s, JS_ATOM_length);
    } else {
        emit_op(s, OP_drop);    /* array length - array */
    }
    done:
    return js_parse_expect(s, ']');
}

/* XXX: remove */
static BOOL has_with_scope(JSFunctionDef *s, int scope_level)
{
    /* check if scope chain contains a with statement */
    while (s) {
        int scope_idx = s->scopes[scope_level].first;
        while (scope_idx >= 0) {
            JSVarDef *vd = &s->vars[scope_idx];

            if (vd->var_name == JS_ATOM__with_)
                return TRUE;
            scope_idx = vd->scope_next;
        }
        /* check parent scopes */
        scope_level = s->parent_scope_level;
        s = s->parent;
    }
    return FALSE;
}

typedef struct JSLValue {
    int opcode;
    int scope;
    int label;
    int depth;
    int tok;
    JSAtom name;
} JSLValue;

static __exception int get_lvalue(JSParseState *s, int *popcode, int *pscope,
                                  JSAtom *pname, int *plabel, int *pdepth, BOOL keep,
                                  int tok)
{
    JSFunctionDef *fd;
    int opcode, scope, label, depth;
    JSAtom name;

    /* we check the last opcode to get the lvalue type */
    fd = s->cur_func;
    scope = 0;
    name = JS_ATOM_NULL;
    label = -1;
    depth = 0;
    switch(opcode = get_prev_opcode(fd)) {
        case OP_scope_get_var:
            name = get_u32(fd->byte_code.buf + fd->last_opcode_pos + 1);
            scope = get_u16(fd->byte_code.buf + fd->last_opcode_pos + 5);
            if ((name == JS_ATOM_arguments || name == JS_ATOM_eval) &&
                (fd->js_mode & JS_MODE_STRICT)) {
                return js_parse_error(s, "invalid lvalue in strict mode");
            }
            if (name == JS_ATOM_this || name == JS_ATOM_new_target)
                goto invalid_lvalue;
            depth = 2;  /* will generate OP_get_ref_value */
            break;
        case OP_get_field:
            name = get_u32(fd->byte_code.buf + fd->last_opcode_pos + 1);
            depth = 1;
            break;
        case OP_scope_get_private_field:
            name = get_u32(fd->byte_code.buf + fd->last_opcode_pos + 1);
            scope = get_u16(fd->byte_code.buf + fd->last_opcode_pos + 5);
            depth = 1;
            break;
        case OP_get_array_el:
            depth = 2;
            break;
        case OP_get_super_value:
            depth = 3;
            break;
        default:
        invalid_lvalue:
            if (tok == TOK_FOR) {
                return js_parse_error(s, "invalid for in/of left hand-side");
            } else if (tok == TOK_INC || tok == TOK_DEC) {
                return js_parse_error(s, "invalid increment/decrement operand");
            } else if (tok == '[' || tok == '{') {
                return js_parse_error(s, "invalid destructuring target");
            } else {
                return js_parse_error(s, "invalid assignment left-hand side");
            }
    }
    /* remove the last opcode */
    fd->byte_code.size = fd->last_opcode_pos;
    fd->last_opcode_pos = -1;

    if (keep) {
        /* get the value but keep the object/fields on the stack */
        switch(opcode) {
            case OP_scope_get_var:
                label = new_label(s);
                emit_op(s, OP_scope_make_ref);
                emit_atom(s, name);
                emit_u32(s, label);
                emit_u16(s, scope);
                update_label(fd, label, 1);
                emit_op(s, OP_get_ref_value);
                opcode = OP_get_ref_value;
                break;
            case OP_get_field:
                emit_op(s, OP_get_field2);
                emit_atom(s, name);
                break;
            case OP_scope_get_private_field:
                emit_op(s, OP_scope_get_private_field2);
                emit_atom(s, name);
                emit_u16(s, scope);
                break;
            case OP_get_array_el:
                /* XXX: replace by a single opcode ? */
                emit_op(s, OP_to_propkey2);
                emit_op(s, OP_dup2);
                emit_op(s, OP_get_array_el);
                break;
            case OP_get_super_value:
                emit_op(s, OP_to_propkey);
                emit_op(s, OP_dup3);
                emit_op(s, OP_get_super_value);
                break;
            default:
                abort();
        }
    } else {
        switch(opcode) {
            case OP_scope_get_var:
                label = new_label(s);
                emit_op(s, OP_scope_make_ref);
                emit_atom(s, name);
                emit_u32(s, label);
                emit_u16(s, scope);
                update_label(fd, label, 1);
                opcode = OP_get_ref_value;
                break;
            case OP_get_array_el:
                emit_op(s, OP_to_propkey2);
                break;
            case OP_get_super_value:
                emit_op(s, OP_to_propkey);
                break;
        }
    }

    *popcode = opcode;
    *pscope = scope;
    /* name has refcount for OP_get_field and OP_get_ref_value,
       and JS_ATOM_NULL for other opcodes */
    *pname = name;
    *plabel = label;
    if (pdepth)
        *pdepth = depth;
    return 0;
}

/* if special = TRUE: specific post inc/dec case */
/* name has a live reference */
static void put_lvalue(JSParseState *s, int opcode, int scope,
                       JSAtom name, int label, BOOL special)
{
    switch(opcode) {
        case OP_get_field:
            if (!special)
                emit_op(s, OP_insert2); /* obj v -> v obj v */
            else
                emit_op(s, OP_perm3); /* obj v0 v -> v0 obj v */
            emit_op(s, OP_put_field);
            emit_u32(s, name);  /* name has refcount */
            break;
        case OP_scope_get_private_field:
            if (!special)
                emit_op(s, OP_insert2); /* obj v -> v obj v */
            else
                emit_op(s, OP_perm3); /* obj v0 v -> v0 obj v */
            emit_op(s, OP_scope_put_private_field);
            emit_u32(s, name);  /* name has refcount */
            emit_u16(s, scope);
            break;
        case OP_get_array_el:
            if (!special)
                emit_op(s, OP_insert3); /* obj prop v -> v obj prop v */
            else
                emit_op(s, OP_perm4); /* obj prop v0 v -> v0 obj prop v */
            emit_op(s, OP_put_array_el);
            break;
        case OP_get_ref_value:
            JS_FreeAtom(s->ctx, name);
            emit_label(s, label);
            if (!special)
                emit_op(s, OP_insert3); /* obj prop v -> v obj prop v */
            else
                emit_op(s, OP_perm4); /* obj prop v0 v -> v0 obj prop v */
            emit_op(s, OP_put_ref_value);
            break;
        case OP_get_super_value:
            if (!special)
                emit_op(s, OP_insert4); /* this obj prop v -> v this obj prop v */
            else
                emit_op(s, OP_perm5); /* this obj prop v0 v -> v0 this obj prop v */
            emit_op(s, OP_put_super_value);
            break;
        default:
            abort();
    }
}

static void put_lvalue_nokeep(JSParseState *s, int opcode, int scope,
                              JSAtom name, int label, int var_tok)
{
    switch(opcode) {
        case OP_scope_get_var:  /* val -- */
            emit_op(s, (var_tok == TOK_CONST || var_tok == TOK_LET) ?
                       OP_scope_put_var_init : OP_scope_put_var);
            emit_u32(s, name);  /* has refcount */
            emit_u16(s, scope);
            break;
        case OP_get_field:      /* obj val -- */
            emit_op(s, OP_put_field);
            emit_u32(s, name);  /* has refcount */
            break;
        case OP_scope_get_private_field:
            emit_op(s, OP_scope_put_private_field);
            emit_u32(s, name);  /* has refcount */
            emit_u16(s, scope);
            break;
        case OP_get_array_el:   /* obj prop val -- */
            emit_op(s, OP_put_array_el);
            break;
        case OP_get_ref_value:   /* obj prop val -- */
            /* XXX: currently this reference is never optimized */
            JS_FreeAtom(s->ctx, name);
            emit_label(s, label);
            //emit_op(s, OP_nop);   /* emit 2 bytes for optimizer */
            emit_op(s, OP_put_ref_value);
            break;
        case OP_get_super_value:
            emit_op(s, OP_put_super_value);
            break;
        default:
            abort();
    }
}

static __exception int js_parse_expr_paren(JSParseState *s)
{
    if (js_parse_expect(s, '('))
        return -1;
    if (js_parse_expr(s))
        return -1;
    if (js_parse_expect(s, ')'))
        return -1;
    return 0;
}

static int js_unsupported_keyword(JSParseState *s, JSAtom atom)
{
    char buf[ATOM_GET_STR_BUF_SIZE];
    return js_parse_error(s, "unsupported keyword: %s",
                          JS_AtomGetStr(s->ctx, buf, sizeof(buf), atom));
}

static __exception int js_define_var(JSParseState *s, JSAtom name, int tok)
{
    JSFunctionDef *fd = s->cur_func;
    JSVarDefEnum var_def_type;

    if (name == JS_ATOM_yield && fd->func_kind == JS_FUNC_GENERATOR) {
        return js_parse_error(s, "yield is a reserved identifier");
    }
    if ((name == JS_ATOM_arguments || name == JS_ATOM_eval)
        &&  (fd->js_mode & JS_MODE_STRICT)) {
        return js_parse_error(s, "invalid variable name in strict mode");
    }
    if ((name == JS_ATOM_let || name == JS_ATOM_undefined)
        &&  (tok == TOK_LET || tok == TOK_CONST)) {
        return js_parse_error(s, "invalid lexical variable name");
    }
    switch(tok) {
        case TOK_LET:
            var_def_type = JS_VAR_DEF_LET;
            break;
        case TOK_CONST:
            var_def_type = JS_VAR_DEF_CONST;
            break;
        case TOK_VAR:
            var_def_type = JS_VAR_DEF_VAR;
            break;
        case TOK_CATCH:
            var_def_type = JS_VAR_DEF_CATCH;
            break;
        default:
            abort();
    }
    if (define_var(s, fd, name, var_def_type) < 0)
        return -1;
    return 0;
}

static void js_emit_spread_code(JSParseState *s, int depth)
{
    int label_rest_next, label_rest_done;

    /* XXX: could check if enum object is an actual array and optimize
       slice extraction. enumeration record and target array are in a
       different order from OP_append case. */
    /* enum_rec xxx -- enum_rec xxx array 0 */
    emit_op(s, OP_array_from);
    emit_u16(s, 0);
    emit_op(s, OP_push_i32);
    emit_u32(s, 0);
    emit_label(s, label_rest_next = new_label(s));
    emit_op(s, OP_for_of_next);
    emit_u8(s, 2 + depth);
    label_rest_done = emit_goto(s, OP_if_true, -1);
    /* array idx val -- array idx */
    emit_op(s, OP_define_array_el);
    emit_op(s, OP_inc);
    emit_goto(s, OP_goto, label_rest_next);
    emit_label(s, label_rest_done);
    /* enum_rec xxx array idx undef -- enum_rec xxx array */
    emit_op(s, OP_drop);
    emit_op(s, OP_drop);
}

static int js_parse_check_duplicate_parameter(JSParseState *s, JSAtom name)
{
    /* Check for duplicate parameter names */
    JSFunctionDef *fd = s->cur_func;
    int i;
    for (i = 0; i < fd->arg_count; i++) {
        if (fd->args[i].var_name == name)
            goto duplicate;
    }
    for (i = 0; i < fd->var_count; i++) {
        if (fd->vars[i].var_name == name)
            goto duplicate;
    }
    return 0;

    duplicate:
    return js_parse_error(s, "duplicate parameter names not allowed in this context");
}

static JSAtom js_parse_destructuring_var(JSParseState *s, int tok, int is_arg)
{
    JSAtom name;

    if (!(s->token.val == TOK_IDENT && !s->token.u.ident.is_reserved)
        ||  ((s->cur_func->js_mode & JS_MODE_STRICT) &&
             (s->token.u.ident.atom == JS_ATOM_eval || s->token.u.ident.atom == JS_ATOM_arguments))) {
        js_parse_error(s, "invalid destructuring target");
        return JS_ATOM_NULL;
    }
    name = JS_DupAtom(s->ctx, s->token.u.ident.atom);
    if (is_arg && js_parse_check_duplicate_parameter(s, name))
        goto fail;
    if (next_token(s))
        goto fail;

    return name;
    fail:
    JS_FreeAtom(s->ctx, name);
    return JS_ATOM_NULL;
}

static int js_parse_destructuring_element(JSParseState *s, int tok, int is_arg,
                                          int hasval, int has_ellipsis,
                                          BOOL allow_initializer)
{
    int label_parse, label_assign, label_done, label_lvalue, depth_lvalue;
    int start_addr, assign_addr;
    JSAtom prop_name, var_name;
    int opcode, scope, tok1, skip_bits;

    if (has_ellipsis < 0) {
        /* pre-parse destructuration target for spread detection */
        js_parse_skip_parens_token(s, &skip_bits, FALSE);
        has_ellipsis = skip_bits & SKIP_HAS_ELLIPSIS;
    }

    label_parse = new_label(s);
    label_assign = new_label(s);

    start_addr = s->cur_func->byte_code.size;
    if (hasval) {
        /* consume value from the stack */
        emit_op(s, OP_dup);
        emit_op(s, OP_undefined);
        emit_op(s, OP_strict_eq);
        emit_goto(s, OP_if_true, label_parse);
        emit_label(s, label_assign);
    } else {
        emit_goto(s, OP_goto, label_parse);
        emit_label(s, label_assign);
        /* leave value on the stack */
        emit_op(s, OP_dup);
    }
    assign_addr = s->cur_func->byte_code.size;
    if (s->token.val == '{') {
        if (next_token(s))
            return -1;
        /* throw an exception if the value cannot be converted to an object */
        emit_op(s, OP_to_object);
        if (has_ellipsis) {
            /* add excludeList on stack just below src object */
            emit_op(s, OP_object);
            emit_op(s, OP_swap);
        }
        while (s->token.val != '}') {
            int prop_type;
            if (s->token.val == TOK_ELLIPSIS) {
                if (!has_ellipsis) {
                    JS_ThrowInternalError(s->ctx, "unexpected ellipsis token");
                    return -1;
                }
                if (next_token(s))
                    return -1;
                if (tok) {
                    var_name = js_parse_destructuring_var(s, tok, is_arg);
                    if (var_name == JS_ATOM_NULL)
                        return -1;
                    opcode = OP_scope_get_var;
                    scope = s->cur_func->scope_level;
                    label_lvalue = -1;
                    depth_lvalue = 0;
                } else {
                    if (js_parse_postfix_expr(s, TRUE))
                        return -1;

                    if (get_lvalue(s, &opcode, &scope, &var_name,
                                   &label_lvalue, &depth_lvalue, FALSE, '{'))
                        return -1;
                }
                if (s->token.val != '}') {
                    js_parse_error(s, "assignment rest property must be last");
                    goto var_error;
                }
                emit_op(s, OP_object);  /* target */
                emit_op(s, OP_copy_data_properties);
                emit_u8(s, 0 | ((depth_lvalue + 1) << 2) | ((depth_lvalue + 2) << 5));
                goto set_val;
            }
            prop_type = js_parse_property_name(s, &prop_name, FALSE, TRUE, FALSE);
            if (prop_type < 0)
                return -1;
            var_name = JS_ATOM_NULL;
            opcode = OP_scope_get_var;
            scope = s->cur_func->scope_level;
            label_lvalue = -1;
            depth_lvalue = 0;
            if (prop_type == PROP_TYPE_IDENT) {
                if (next_token(s))
                    goto prop_error;
                if ((s->token.val == '[' || s->token.val == '{')
                    &&  ((tok1 = js_parse_skip_parens_token(s, &skip_bits, FALSE)) == ',' ||
                         tok1 == '=' || tok1 == '}')) {
                    if (prop_name == JS_ATOM_NULL) {
                        /* computed property name on stack */
                        if (has_ellipsis) {
                            /* define the property in excludeList */
                            emit_op(s, OP_to_propkey); /* avoid calling ToString twice */
                            emit_op(s, OP_perm3); /* TOS: src excludeList prop */
                            emit_op(s, OP_null); /* TOS: src excludeList prop null */
                            emit_op(s, OP_define_array_el); /* TOS: src excludeList prop */
                            emit_op(s, OP_perm3); /* TOS: excludeList src prop */
                        }
                        /* get the computed property from the source object */
                        emit_op(s, OP_get_array_el2);
                    } else {
                        /* named property */
                        if (has_ellipsis) {
                            /* define the property in excludeList */
                            emit_op(s, OP_swap); /* TOS: src excludeList */
                            emit_op(s, OP_null); /* TOS: src excludeList null */
                            emit_op(s, OP_define_field); /* TOS: src excludeList */
                            emit_atom(s, prop_name);
                            emit_op(s, OP_swap); /* TOS: excludeList src */
                        }
                        /* get the named property from the source object */
                        emit_op(s, OP_get_field2);
                        emit_u32(s, prop_name);
                    }
                    if (js_parse_destructuring_element(s, tok, is_arg, TRUE, -1, TRUE))
                        return -1;
                    if (s->token.val == '}')
                        break;
                    /* accept a trailing comma before the '}' */
                    if (js_parse_expect(s, ','))
                        return -1;
                    continue;
                }
                if (prop_name == JS_ATOM_NULL) {
                    emit_op(s, OP_to_propkey2);
                    if (has_ellipsis) {
                        /* define the property in excludeList */
                        emit_op(s, OP_perm3);
                        emit_op(s, OP_null);
                        emit_op(s, OP_define_array_el);
                        emit_op(s, OP_perm3);
                    }
                    /* source prop -- source source prop */
                    emit_op(s, OP_dup1);
                } else {
                    if (has_ellipsis) {
                        /* define the property in excludeList */
                        emit_op(s, OP_swap);
                        emit_op(s, OP_null);
                        emit_op(s, OP_define_field);
                        emit_atom(s, prop_name);
                        emit_op(s, OP_swap);
                    }
                    /* source -- source source */
                    emit_op(s, OP_dup);
                }
                if (tok) {
                    var_name = js_parse_destructuring_var(s, tok, is_arg);
                    if (var_name == JS_ATOM_NULL)
                        goto prop_error;
                } else {
                    if (js_parse_postfix_expr(s, TRUE))
                        goto prop_error;
                    lvalue:
                    if (get_lvalue(s, &opcode, &scope, &var_name,
                                   &label_lvalue, &depth_lvalue, FALSE, '{'))
                        goto prop_error;
                    /* swap ref and lvalue object if any */
                    if (prop_name == JS_ATOM_NULL) {
                        switch(depth_lvalue) {
                            case 1:
                                /* source prop x -> x source prop */
                                emit_op(s, OP_rot3r);
                                break;
                            case 2:
                                /* source prop x y -> x y source prop */
                                emit_op(s, OP_swap2);   /* t p2 s p1 */
                                break;
                            case 3:
                                /* source prop x y z -> x y z source prop */
                                emit_op(s, OP_rot5l);
                                emit_op(s, OP_rot5l);
                                break;
                        }
                    } else {
                        switch(depth_lvalue) {
                            case 1:
                                /* source x -> x source */
                                emit_op(s, OP_swap);
                                break;
                            case 2:
                                /* source x y -> x y source */
                                emit_op(s, OP_rot3l);
                                break;
                            case 3:
                                /* source x y z -> x y z source */
                                emit_op(s, OP_rot4l);
                                break;
                        }
                    }
                }
                if (prop_name == JS_ATOM_NULL) {
                    /* computed property name on stack */
                    /* XXX: should have OP_get_array_el2x with depth */
                    /* source prop -- val */
                    emit_op(s, OP_get_array_el);
                } else {
                    /* named property */
                    /* XXX: should have OP_get_field2x with depth */
                    /* source -- val */
                    emit_op(s, OP_get_field);
                    emit_u32(s, prop_name);
                }
            } else {
                /* prop_type = PROP_TYPE_VAR, cannot be a computed property */
                if (is_arg && js_parse_check_duplicate_parameter(s, prop_name))
                    goto prop_error;
                if ((s->cur_func->js_mode & JS_MODE_STRICT) &&
                    (prop_name == JS_ATOM_eval || prop_name == JS_ATOM_arguments)) {
                    js_parse_error(s, "invalid destructuring target");
                    goto prop_error;
                }
                if (has_ellipsis) {
                    /* define the property in excludeList */
                    emit_op(s, OP_swap);
                    emit_op(s, OP_null);
                    emit_op(s, OP_define_field);
                    emit_atom(s, prop_name);
                    emit_op(s, OP_swap);
                }
                if (!tok || tok == TOK_VAR) {
                    /* generate reference */
                    /* source -- source source */
                    emit_op(s, OP_dup);
                    emit_op(s, OP_scope_get_var);
                    emit_atom(s, prop_name);
                    emit_u16(s, s->cur_func->scope_level);
                    goto lvalue;
                }
                var_name = JS_DupAtom(s->ctx, prop_name);
                /* source -- source val */
                emit_op(s, OP_get_field2);
                emit_u32(s, prop_name);
            }
            set_val:
            if (tok) {
                if (js_define_var(s, var_name, tok))
                    goto var_error;
                scope = s->cur_func->scope_level;
            }
            if (s->token.val == '=') {  /* handle optional default value */
                int label_hasval;
                emit_op(s, OP_dup);
                emit_op(s, OP_undefined);
                emit_op(s, OP_strict_eq);
                label_hasval = emit_goto(s, OP_if_false, -1);
                if (next_token(s))
                    goto var_error;
                emit_op(s, OP_drop);
                if (js_parse_assign_expr(s, TRUE))
                    goto var_error;
                if (opcode == OP_scope_get_var || opcode == OP_get_ref_value)
                    set_object_name(s, var_name);
                emit_label(s, label_hasval);
            }
            /* store value into lvalue object */
            put_lvalue_nokeep(s, opcode, scope, var_name, label_lvalue, tok);
            if (s->token.val == '}')
                break;
            /* accept a trailing comma before the '}' */
            if (js_parse_expect(s, ','))
                return -1;
        }
        /* drop the source object */
        emit_op(s, OP_drop);
        if (has_ellipsis) {
            emit_op(s, OP_drop); /* pop excludeList */
        }
        if (next_token(s))
            return -1;
    } else if (s->token.val == '[') {
        BOOL has_spread;
        int enum_depth;
        BlockEnv block_env;

        if (next_token(s))
            return -1;
        /* the block environment is only needed in generators in case
           'yield' triggers a 'return' */
        push_break_entry(s->cur_func, &block_env,
                         JS_ATOM_NULL, -1, -1, 2);
        block_env.has_iterator = TRUE;
        emit_op(s, OP_for_of_start);
        has_spread = FALSE;
        while (s->token.val != ']') {
            /* get the next value */
            if (s->token.val == TOK_ELLIPSIS) {
                if (next_token(s))
                    return -1;
                if (s->token.val == ',' || s->token.val == ']')
                    return js_parse_error(s, "missing binding pattern...");
                has_spread = TRUE;
            }
            if (s->token.val == ',') {
                /* do nothing, skip the value, has_spread is false */
                emit_op(s, OP_for_of_next);
                emit_u8(s, 0);
                emit_op(s, OP_drop);
                emit_op(s, OP_drop);
            } else if ((s->token.val == '[' || s->token.val == '{')
                       &&  ((tok1 = js_parse_skip_parens_token(s, &skip_bits, FALSE)) == ',' ||
                            tok1 == '=' || tok1 == ']')) {
                if (has_spread) {
                    if (tok1 == '=')
                        return js_parse_error(s, "rest element cannot have a default value");
                    js_emit_spread_code(s, 0);
                } else {
                    emit_op(s, OP_for_of_next);
                    emit_u8(s, 0);
                    emit_op(s, OP_drop);
                }
                if (js_parse_destructuring_element(s, tok, is_arg, TRUE, skip_bits & SKIP_HAS_ELLIPSIS, TRUE))
                    return -1;
            } else {
                var_name = JS_ATOM_NULL;
                enum_depth = 0;
                if (tok) {
                    var_name = js_parse_destructuring_var(s, tok, is_arg);
                    if (var_name == JS_ATOM_NULL)
                        goto var_error;
                    if (js_define_var(s, var_name, tok))
                        goto var_error;
                    opcode = OP_scope_get_var;
                    scope = s->cur_func->scope_level;
                } else {
                    if (js_parse_postfix_expr(s, TRUE))
                        return -1;
                    if (get_lvalue(s, &opcode, &scope, &var_name,
                                   &label_lvalue, &enum_depth, FALSE, '[')) {
                        return -1;
                    }
                }
                if (has_spread) {
                    js_emit_spread_code(s, enum_depth);
                } else {
                    emit_op(s, OP_for_of_next);
                    emit_u8(s, enum_depth);
                    emit_op(s, OP_drop);
                }
                if (s->token.val == '=' && !has_spread) {
                    /* handle optional default value */
                    int label_hasval;
                    emit_op(s, OP_dup);
                    emit_op(s, OP_undefined);
                    emit_op(s, OP_strict_eq);
                    label_hasval = emit_goto(s, OP_if_false, -1);
                    if (next_token(s))
                        goto var_error;
                    emit_op(s, OP_drop);
                    if (js_parse_assign_expr(s, TRUE))
                        goto var_error;
                    if (opcode == OP_scope_get_var || opcode == OP_get_ref_value)
                        set_object_name(s, var_name);
                    emit_label(s, label_hasval);
                }
                /* store value into lvalue object */
                put_lvalue_nokeep(s, opcode, scope, var_name,
                                  label_lvalue, tok);
            }
            if (s->token.val == ']')
                break;
            if (has_spread)
                return js_parse_error(s, "rest element must be the last one");
            /* accept a trailing comma before the ']' */
            if (js_parse_expect(s, ','))
                return -1;
        }
        /* close iterator object:
           if completed, enum_obj has been replaced by undefined */
        emit_op(s, OP_iterator_close);
        pop_break_entry(s->cur_func);
        if (next_token(s))
            return -1;
    } else {
        return js_parse_error(s, "invalid assignment syntax");
    }
    if (s->token.val == '=' && allow_initializer) {
        label_done = emit_goto(s, OP_goto, -1);
        if (next_token(s))
            return -1;
        emit_label(s, label_parse);
        if (hasval)
            emit_op(s, OP_drop);
        if (js_parse_assign_expr(s, TRUE))
            return -1;
        emit_goto(s, OP_goto, label_assign);
        emit_label(s, label_done);
    } else {
        /* normally hasval is true except if
           js_parse_skip_parens_token() was wrong in the parsing */
        //        assert(hasval);
        if (!hasval) {
            js_parse_error(s, "too complicated destructuring expression");
            return -1;
        }
        /* remove test and decrement label ref count */
        memset(s->cur_func->byte_code.buf + start_addr, OP_nop,
               assign_addr - start_addr);
        s->cur_func->label_slots[label_parse].ref_count--;
    }
    return 0;

    prop_error:
    JS_FreeAtom(s->ctx, prop_name);
    var_error:
    JS_FreeAtom(s->ctx, var_name);
    return -1;
}

typedef enum FuncCallType {
    FUNC_CALL_NORMAL,
    FUNC_CALL_NEW,
    FUNC_CALL_SUPER_CTOR,
    FUNC_CALL_TEMPLATE,
} FuncCallType;

static void optional_chain_test(JSParseState *s, int *poptional_chaining_label,
                                int drop_count)
{
    int label_next, i;
    if (*poptional_chaining_label < 0)
        *poptional_chaining_label = new_label(s);
    /* XXX: could be more efficient with a specific opcode */
    emit_op(s, OP_dup);
    emit_op(s, OP_is_undefined_or_null);
    label_next = emit_goto(s, OP_if_false, -1);
    for(i = 0; i < drop_count; i++)
        emit_op(s, OP_drop);
    emit_op(s, OP_undefined);
    emit_goto(s, OP_goto, *poptional_chaining_label);
    emit_label(s, label_next);
}

static __exception int js_parse_postfix_expr(JSParseState *s, BOOL accept_lparen)
{
    FuncCallType call_type;
    int optional_chaining_label;

    call_type = FUNC_CALL_NORMAL;
    switch(s->token.val) {
        case TOK_NUMBER:
        {
            JSValue val;
            val = s->token.u.num.val;

            if (JS_VALUE_GET_TAG(val) == JS_TAG_INT) {
                emit_op(s, OP_push_i32);
                emit_u32(s, JS_VALUE_GET_INT(val));
            } else
#ifdef CONFIG_BIGNUM
                if (JS_VALUE_GET_TAG(val) == JS_TAG_BIG_FLOAT) {
                slimb_t e;
                int ret;

                /* need a runtime conversion */
                /* XXX: could add a cache and/or do it once at
                   the start of the function */
                if (emit_push_const(s, val, 0) < 0)
                    return -1;
                e = s->token.u.num.exponent;
                if (e == (int32_t)e) {
                    emit_op(s, OP_push_i32);
                    emit_u32(s, e);
                } else {
                    val = JS_NewBigInt64_1(s->ctx, e);
                    if (JS_IsException(val))
                        return -1;
                    ret = emit_push_const(s, val, 0);
                    JS_FreeValue(s->ctx, val);
                    if (ret < 0)
                        return -1;
                }
                emit_op(s, OP_mul_pow10);
            } else
#endif
            {
                if (emit_push_const(s, val, 0) < 0)
                    return -1;
            }
        }
            if (next_token(s))
                return -1;
            break;
        case TOK_TEMPLATE:
            if (js_parse_template(s, 0, NULL))
                return -1;
            break;
        case TOK_STRING:
            if (emit_push_const(s, s->token.u.str.str, 1))
                return -1;
            if (next_token(s))
                return -1;
            break;

        case TOK_DIV_ASSIGN:
            s->buf_ptr -= 2;
            goto parse_regexp;
        case '/':
            s->buf_ptr--;
        parse_regexp:
        {
            JSValue str;
            int ret, backtrace_flags;
            if (!s->ctx->compile_regexp)
                return js_parse_error(s, "RegExp are not supported");
            /* the previous token is '/' or '/=', so no need to free */
            if (js_parse_regexp(s))
                return -1;
            ret = emit_push_const(s, s->token.u.regexp.body, 0);
            str = s->ctx->compile_regexp(s->ctx, s->token.u.regexp.body,
                                         s->token.u.regexp.flags);
            if (JS_IsException(str)) {
                /* add the line number info */
                backtrace_flags = 0;
                if (s->cur_func && s->cur_func->backtrace_barrier)
                    backtrace_flags = JS_BACKTRACE_FLAG_SINGLE_LEVEL;
                build_backtrace(s->ctx, s->ctx->rt->current_exception,
                                s->filename, s->token.line_num,
                                backtrace_flags);
                return -1;
            }
            ret = emit_push_const(s, str, 0);
            JS_FreeValue(s->ctx, str);
            if (ret)
                return -1;
            /* we use a specific opcode to be sure the correct
               function is called (otherwise the bytecode would have
               to be verified by the RegExp constructor) */
            emit_op(s, OP_regexp);
            if (next_token(s))
                return -1;
        }
            break;
        case '(':
            if (js_parse_skip_parens_token(s, NULL, TRUE) == TOK_ARROW) {
                if (js_parse_function_decl(s, JS_PARSE_FUNC_ARROW,
                                           JS_FUNC_NORMAL, JS_ATOM_NULL,
                                           s->token.ptr, s->token.line_num))
                    return -1;
            } else {
                if (js_parse_expr_paren(s))
                    return -1;
            }
            break;
        case TOK_FUNCTION:
            if (js_parse_function_decl(s, JS_PARSE_FUNC_EXPR,
                                       JS_FUNC_NORMAL, JS_ATOM_NULL,
                                       s->token.ptr, s->token.line_num))
                return -1;
            break;
        case TOK_CLASS:
            if (js_parse_class(s, TRUE, JS_PARSE_EXPORT_NONE))
                return -1;
            break;
        case TOK_NULL:
            if (next_token(s))
                return -1;
            emit_op(s, OP_null);
            break;
        case TOK_THIS:
            if (next_token(s))
                return -1;
            emit_op(s, OP_scope_get_var);
            emit_atom(s, JS_ATOM_this);
            emit_u16(s, 0);
            break;
        case TOK_FALSE:
            if (next_token(s))
                return -1;
            emit_op(s, OP_push_false);
            break;
        case TOK_TRUE:
            if (next_token(s))
                return -1;
            emit_op(s, OP_push_true);
            break;
        case TOK_IDENT:
        {
            JSAtom name;
            if (s->token.u.ident.is_reserved) {
                return js_parse_error_reserved_identifier(s);
            }
            if (peek_token(s, TRUE) == TOK_ARROW) {
                if (js_parse_function_decl(s, JS_PARSE_FUNC_ARROW,
                                           JS_FUNC_NORMAL, JS_ATOM_NULL,
                                           s->token.ptr, s->token.line_num))
                    return -1;
            } else if (token_is_pseudo_keyword(s, JS_ATOM_async) &&
                       peek_token(s, TRUE) != '\n') {
                const uint8_t *source_ptr;
                int source_line_num;

                source_ptr = s->token.ptr;
                source_line_num = s->token.line_num;
                if (next_token(s))
                    return -1;
                if (s->token.val == TOK_FUNCTION) {
                    if (js_parse_function_decl(s, JS_PARSE_FUNC_EXPR,
                                               JS_FUNC_ASYNC, JS_ATOM_NULL,
                                               source_ptr, source_line_num))
                        return -1;
                } else if ((s->token.val == '(' &&
                            js_parse_skip_parens_token(s, NULL, TRUE) == TOK_ARROW) ||
                           (s->token.val == TOK_IDENT && !s->token.u.ident.is_reserved &&
                            peek_token(s, TRUE) == TOK_ARROW)) {
                    if (js_parse_function_decl(s, JS_PARSE_FUNC_ARROW,
                                               JS_FUNC_ASYNC, JS_ATOM_NULL,
                                               source_ptr, source_line_num))
                        return -1;
                } else {
                    name = JS_DupAtom(s->ctx, JS_ATOM_async);
                    goto do_get_var;
                }
            } else {
                if (s->token.u.ident.atom == JS_ATOM_arguments &&
                    !s->cur_func->arguments_allowed) {
                    js_parse_error(s, "'arguments' identifier is not allowed in class field initializer");
                    return -1;
                }
                name = JS_DupAtom(s->ctx, s->token.u.ident.atom);
                if (next_token(s))  /* update line number before emitting code */
                    return -1;
                do_get_var:
                emit_op(s, OP_scope_get_var);
                emit_u32(s, name);
                emit_u16(s, s->cur_func->scope_level);
            }
        }
            break;
        case '{':
        case '[':
        {
            int skip_bits;
            if (js_parse_skip_parens_token(s, &skip_bits, FALSE) == '=') {
                if (js_parse_destructuring_element(s, 0, 0, FALSE, skip_bits & SKIP_HAS_ELLIPSIS, TRUE))
                    return -1;
            } else {
                if (s->token.val == '{') {
                    if (js_parse_object_literal(s))
                        return -1;
                } else {
                    if (js_parse_array_literal(s))
                        return -1;
                }
            }
        }
            break;
        case TOK_NEW:
            if (next_token(s))
                return -1;
            if (s->token.val == '.') {
                if (next_token(s))
                    return -1;
                if (!token_is_pseudo_keyword(s, JS_ATOM_target))
                    return js_parse_error(s, "expecting target");
                if (!s->cur_func->new_target_allowed)
                    return js_parse_error(s, "new.target only allowed within functions");
                if (next_token(s))
                    return -1;
                emit_op(s, OP_scope_get_var);
                emit_atom(s, JS_ATOM_new_target);
                emit_u16(s, 0);
            } else {
                if (js_parse_postfix_expr(s, FALSE))
                    return -1;
                accept_lparen = TRUE;
                if (s->token.val != '(') {
                    /* new operator on an object */
                    emit_op(s, OP_dup);
                    emit_op(s, OP_call_constructor);
                    emit_u16(s, 0);
                } else {
                    call_type = FUNC_CALL_NEW;
                }
            }
            break;
        case TOK_SUPER:
            if (next_token(s))
                return -1;
            if (s->token.val == '(') {
                if (!s->cur_func->super_call_allowed)
                    return js_parse_error(s, "super() is only valid in a derived class constructor");
                call_type = FUNC_CALL_SUPER_CTOR;
            } else if (s->token.val == '.' || s->token.val == '[') {
                if (!s->cur_func->super_allowed)
                    return js_parse_error(s, "'super' is only valid in a method");
                emit_op(s, OP_scope_get_var);
                emit_atom(s, JS_ATOM_this);
                emit_u16(s, 0);
                emit_op(s, OP_scope_get_var);
                emit_atom(s, JS_ATOM_home_object);
                emit_u16(s, 0);
                emit_op(s, OP_get_super);
            } else {
                return js_parse_error(s, "invalid use of 'super'");
            }
            break;
        case TOK_IMPORT:
            if (next_token(s))
                return -1;
            if (s->token.val == '.') {
                if (next_token(s))
                    return -1;
                if (!token_is_pseudo_keyword(s, JS_ATOM_meta))
                    return js_parse_error(s, "meta expected");
                if (!s->is_module)
                    return js_parse_error(s, "import.meta only valid in module code");
                if (next_token(s))
                    return -1;
                emit_op(s, OP_special_object);
                emit_u8(s, OP_SPECIAL_OBJECT_IMPORT_META);
            } else {
                if (js_parse_expect(s, '('))
                    return -1;
                if (!accept_lparen)
                    return js_parse_error(s, "invalid use of 'import()'");
                if (js_parse_assign_expr(s, TRUE))
                    return -1;
                if (js_parse_expect(s, ')'))
                    return -1;
                emit_op(s, OP_import);
            }
            break;
        default:
            return js_parse_error(s, "unexpected token in expression: '%.*s'",
                                  (int)(s->buf_ptr - s->token.ptr), s->token.ptr);
    }

    optional_chaining_label = -1;
    for(;;) {
        JSFunctionDef *fd = s->cur_func;
        BOOL has_optional_chain = FALSE;

        if (s->token.val == TOK_QUESTION_MARK_DOT) {
            /* optional chaining */
            if (next_token(s))
                return -1;
            has_optional_chain = TRUE;
            if (s->token.val == '(' && accept_lparen) {
                goto parse_func_call;
            } else if (s->token.val == '[') {
                goto parse_array_access;
            } else {
                goto parse_property;
            }
        } else if (s->token.val == TOK_TEMPLATE &&
                   call_type == FUNC_CALL_NORMAL) {
            if (optional_chaining_label >= 0) {
                return js_parse_error(s, "template literal cannot appear in an optional chain");
            }
            call_type = FUNC_CALL_TEMPLATE;
            goto parse_func_call2;
        } else if (s->token.val == '(' && accept_lparen) {
            int opcode, arg_count, drop_count;

            /* function call */
            parse_func_call:
            if (next_token(s))
                return -1;

            if (call_type == FUNC_CALL_NORMAL) {
                parse_func_call2:
                switch(opcode = get_prev_opcode(fd)) {
                    case OP_get_field:
                        /* keep the object on the stack */
                        fd->byte_code.buf[fd->last_opcode_pos] = OP_get_field2;
                        drop_count = 2;
                        break;
                    case OP_scope_get_private_field:
                        /* keep the object on the stack */
                        fd->byte_code.buf[fd->last_opcode_pos] = OP_scope_get_private_field2;
                        drop_count = 2;
                        break;
                    case OP_get_array_el:
                        /* keep the object on the stack */
                        fd->byte_code.buf[fd->last_opcode_pos] = OP_get_array_el2;
                        drop_count = 2;
                        break;
                    case OP_scope_get_var:
                    {
                        JSAtom name;
                        int scope;
                        name = get_u32(fd->byte_code.buf + fd->last_opcode_pos + 1);
                        scope = get_u16(fd->byte_code.buf + fd->last_opcode_pos + 5);
                        if (name == JS_ATOM_eval && call_type == FUNC_CALL_NORMAL && !has_optional_chain) {
                            /* direct 'eval' */
                            opcode = OP_eval;
                        } else {
                            /* verify if function name resolves to a simple
                               get_loc/get_arg: a function call inside a `with`
                               statement can resolve to a method call of the
                               `with` context object
                             */
                            /* XXX: always generate the OP_scope_get_ref
                               and remove it in variable resolution
                               pass ? */
                            if (has_with_scope(fd, scope)) {
                                opcode = OP_scope_get_ref;
                                fd->byte_code.buf[fd->last_opcode_pos] = opcode;
                            }
                        }
                        drop_count = 1;
                    }
                        break;
                    case OP_get_super_value:
                        fd->byte_code.buf[fd->last_opcode_pos] = OP_get_array_el;
                        /* on stack: this func_obj */
                        opcode = OP_get_array_el;
                        drop_count = 2;
                        break;
                    default:
                        opcode = OP_invalid;
                        drop_count = 1;
                        break;
                }
                if (has_optional_chain) {
                    optional_chain_test(s, &optional_chaining_label,
                                        drop_count);
                }
            } else {
                opcode = OP_invalid;
            }

            if (call_type == FUNC_CALL_TEMPLATE) {
                if (js_parse_template(s, 1, &arg_count))
                    return -1;
                goto emit_func_call;
            } else if (call_type == FUNC_CALL_SUPER_CTOR) {
                emit_op(s, OP_scope_get_var);
                emit_atom(s, JS_ATOM_this_active_func);
                emit_u16(s, 0);

                emit_op(s, OP_get_super);

                emit_op(s, OP_scope_get_var);
                emit_atom(s, JS_ATOM_new_target);
                emit_u16(s, 0);
            } else if (call_type == FUNC_CALL_NEW) {
                emit_op(s, OP_dup); /* new.target = function */
            }

            /* parse arguments */
            arg_count = 0;
            while (s->token.val != ')') {
                if (arg_count >= 65535) {
                    return js_parse_error(s, "Too many call arguments");
                }
                if (s->token.val == TOK_ELLIPSIS)
                    break;
                if (js_parse_assign_expr(s, TRUE))
                    return -1;
                arg_count++;
                if (s->token.val == ')')
                    break;
                /* accept a trailing comma before the ')' */
                if (js_parse_expect(s, ','))
                    return -1;
            }
            if (s->token.val == TOK_ELLIPSIS) {
                emit_op(s, OP_array_from);
                emit_u16(s, arg_count);
                emit_op(s, OP_push_i32);
                emit_u32(s, arg_count);

                /* on stack: array idx */
                while (s->token.val != ')') {
                    if (s->token.val == TOK_ELLIPSIS) {
                        if (next_token(s))
                            return -1;
                        if (js_parse_assign_expr(s, TRUE))
                            return -1;
#if 1
                        /* XXX: could pass is_last indicator? */
                        emit_op(s, OP_append);
#else
                        int label_next, label_done;
                        label_next = new_label(s);
                        label_done = new_label(s);
                        /* push enumerate object below array/idx pair */
                        emit_op(s, OP_for_of_start);
                        emit_op(s, OP_rot5l);
                        emit_op(s, OP_rot5l);
                        emit_label(s, label_next);
                        /* on stack: enum_rec array idx */
                        emit_op(s, OP_for_of_next);
                        emit_u8(s, 2);
                        emit_goto(s, OP_if_true, label_done);
                        /* append element */
                        /* enum_rec array idx val -> enum_rec array new_idx */
                        emit_op(s, OP_define_array_el);
                        emit_op(s, OP_inc);
                        emit_goto(s, OP_goto, label_next);
                        emit_label(s, label_done);
                        /* close enumeration, drop enum_rec and idx */
                        emit_op(s, OP_drop); /* drop undef */
                        emit_op(s, OP_nip1); /* drop enum_rec */
                        emit_op(s, OP_nip1);
                        emit_op(s, OP_nip1);
#endif
                    } else {
                        if (js_parse_assign_expr(s, TRUE))
                            return -1;
                        /* array idx val */
                        emit_op(s, OP_define_array_el);
                        emit_op(s, OP_inc);
                    }
                    if (s->token.val == ')')
                        break;
                    /* accept a trailing comma before the ')' */
                    if (js_parse_expect(s, ','))
                        return -1;
                }
                if (next_token(s))
                    return -1;
                /* drop the index */
                emit_op(s, OP_drop);

                /* apply function call */
                switch(opcode) {
                    case OP_get_field:
                    case OP_scope_get_private_field:
                    case OP_get_array_el:
                    case OP_scope_get_ref:
                        /* obj func array -> func obj array */
                        emit_op(s, OP_perm3);
                        emit_op(s, OP_apply);
                        emit_u16(s, call_type == FUNC_CALL_NEW);
                        break;
                    case OP_eval:
                        emit_op(s, OP_apply_eval);
                        emit_u16(s, fd->scope_level);
                        fd->has_eval_call = TRUE;
                        break;
                    default:
                        if (call_type == FUNC_CALL_SUPER_CTOR) {
                            emit_op(s, OP_apply);
                            emit_u16(s, 1);
                            /* set the 'this' value */
                            emit_op(s, OP_dup);
                            emit_op(s, OP_scope_put_var_init);
                            emit_atom(s, JS_ATOM_this);
                            emit_u16(s, 0);

                            emit_class_field_init(s);
                        } else if (call_type == FUNC_CALL_NEW) {
                            /* obj func array -> func obj array */
                            emit_op(s, OP_perm3);
                            emit_op(s, OP_apply);
                            emit_u16(s, 1);
                        } else {
                            /* func array -> func undef array */
                            emit_op(s, OP_undefined);
                            emit_op(s, OP_swap);
                            emit_op(s, OP_apply);
                            emit_u16(s, 0);
                        }
                        break;
                }
            } else {
                if (next_token(s))
                    return -1;
                emit_func_call:
                switch(opcode) {
                    case OP_get_field:
                    case OP_scope_get_private_field:
                    case OP_get_array_el:
                    case OP_scope_get_ref:
                        emit_op(s, OP_call_method);
                        emit_u16(s, arg_count);
                        break;
                    case OP_eval:
                        emit_op(s, OP_eval);
                        emit_u16(s, arg_count);
                        emit_u16(s, fd->scope_level);
                        fd->has_eval_call = TRUE;
                        break;
                    default:
                        if (call_type == FUNC_CALL_SUPER_CTOR) {
                            emit_op(s, OP_call_constructor);
                            emit_u16(s, arg_count);

                            /* set the 'this' value */
                            emit_op(s, OP_dup);
                            emit_op(s, OP_scope_put_var_init);
                            emit_atom(s, JS_ATOM_this);
                            emit_u16(s, 0);

                            emit_class_field_init(s);
                        } else if (call_type == FUNC_CALL_NEW) {
                            emit_op(s, OP_call_constructor);
                            emit_u16(s, arg_count);
                        } else {
                            emit_op(s, OP_call);
                            emit_u16(s, arg_count);
                        }
                        break;
                }
            }
            call_type = FUNC_CALL_NORMAL;
        } else if (s->token.val == '.') {
            if (next_token(s))
                return -1;
            if (s->token.val == TOK_PRIVATE_NAME) {
                /* private class field */
                if (get_prev_opcode(fd) == OP_get_super) {
                    return js_parse_error(s, "private class field forbidden after super");
                }
                emit_op(s, OP_scope_get_private_field);
                emit_atom(s, s->token.u.ident.atom);
                emit_u16(s, s->cur_func->scope_level);
            } else {
                parse_property:
                if (!token_is_ident(s->token.val)) {
                    return js_parse_error(s, "expecting field name");
                }
                if (get_prev_opcode(fd) == OP_get_super) {
                    JSValue val;
                    int ret;
                    val = JS_AtomToValue(s->ctx, s->token.u.ident.atom);
                    ret = emit_push_const(s, val, 1);
                    JS_FreeValue(s->ctx, val);
                    if (ret)
                        return -1;
                    emit_op(s, OP_get_super_value);
                } else {
                    if (has_optional_chain) {
                        optional_chain_test(s, &optional_chaining_label, 1);
                    }
                    emit_op(s, OP_get_field);
                    emit_atom(s, s->token.u.ident.atom);
                }
            }
            if (next_token(s))
                return -1;
        } else if (s->token.val == '[') {
            int prev_op;

            parse_array_access:
            prev_op = get_prev_opcode(fd);
            if (has_optional_chain) {
                optional_chain_test(s, &optional_chaining_label, 1);
            }
            if (next_token(s))
                return -1;
            if (js_parse_expr(s))
                return -1;
            if (js_parse_expect(s, ']'))
                return -1;
            if (prev_op == OP_get_super) {
                emit_op(s, OP_get_super_value);
            } else {
                emit_op(s, OP_get_array_el);
            }
        } else {
            break;
        }
    }
    if (optional_chaining_label >= 0)
        emit_label(s, optional_chaining_label);
    return 0;
}

static __exception int js_parse_delete(JSParseState *s)
{
    JSFunctionDef *fd = s->cur_func;
    JSAtom name;
    int opcode;

    if (next_token(s))
        return -1;
    if (js_parse_unary(s, -1))
        return -1;
    switch(opcode = get_prev_opcode(fd)) {
        case OP_get_field:
        {
            JSValue val;
            int ret;

            name = get_u32(fd->byte_code.buf + fd->last_opcode_pos + 1);
            fd->byte_code.size = fd->last_opcode_pos;
            fd->last_opcode_pos = -1;
            val = JS_AtomToValue(s->ctx, name);
            ret = emit_push_const(s, val, 1);
            JS_FreeValue(s->ctx, val);
            JS_FreeAtom(s->ctx, name);
            if (ret)
                return ret;
        }
            goto do_delete;
        case OP_get_array_el:
            fd->byte_code.size = fd->last_opcode_pos;
            fd->last_opcode_pos = -1;
        do_delete:
            emit_op(s, OP_delete);
            break;
        case OP_scope_get_var:
            /* 'delete this': this is not a reference */
            name = get_u32(fd->byte_code.buf + fd->last_opcode_pos + 1);
            if (name == JS_ATOM_this || name == JS_ATOM_new_target)
                goto ret_true;
            if (fd->js_mode & JS_MODE_STRICT) {
                return js_parse_error(s, "cannot delete a direct reference in strict mode");
            } else {
                fd->byte_code.buf[fd->last_opcode_pos] = OP_scope_delete_var;
            }
            break;
        case OP_scope_get_private_field:
            return js_parse_error(s, "cannot delete a private class field");
        case OP_get_super_value:
            emit_op(s, OP_throw_var);
            emit_atom(s, JS_ATOM_NULL);
            emit_u8(s, JS_THROW_VAR_DELETE_SUPER);
            break;
        default:
        ret_true:
            emit_op(s, OP_drop);
            emit_op(s, OP_push_true);
            break;
    }
    return 0;
}

static __exception int js_parse_unary(JSParseState *s, int exponentiation_flag)
{
    int op;

    switch(s->token.val) {
        case '+':
        case '-':
        case '!':
        case '~':
        case TOK_VOID:
            op = s->token.val;
            if (next_token(s))
                return -1;
            if (js_parse_unary(s, -1))
                return -1;
            switch(op) {
                case '-':
                    emit_op(s, OP_neg);
                    break;
                case '+':
                    emit_op(s, OP_plus);
                    break;
                case '!':
                    emit_op(s, OP_lnot);
                    break;
                case '~':
                    emit_op(s, OP_not);
                    break;
                case TOK_VOID:
                    emit_op(s, OP_drop);
                    emit_op(s, OP_undefined);
                    break;
                default:
                    abort();
            }
            exponentiation_flag = 0;
            break;
        case TOK_DEC:
        case TOK_INC:
        {
            int opcode, op, scope, label;
            JSAtom name;
            op = s->token.val;
            if (next_token(s))
                return -1;
            /* XXX: should parse LeftHandSideExpression */
            if (js_parse_unary(s, 0))
                return -1;
            if (get_lvalue(s, &opcode, &scope, &name, &label, NULL, TRUE, op))
                return -1;
            emit_op(s, OP_dec + op - TOK_DEC);
            put_lvalue(s, opcode, scope, name, label, FALSE);
        }
            break;
        case TOK_TYPEOF:
        {
            JSFunctionDef *fd;
            if (next_token(s))
                return -1;
            if (js_parse_unary(s, -1))
                return -1;
            /* reference access should not return an exception, so we
               patch the get_var */
            fd = s->cur_func;
            if (get_prev_opcode(fd) == OP_scope_get_var) {
                fd->byte_code.buf[fd->last_opcode_pos] = OP_scope_get_var_undef;
            }
            emit_op(s, OP_typeof);
            exponentiation_flag = 0;
        }
            break;
        case TOK_DELETE:
            if (js_parse_delete(s))
                return -1;
            exponentiation_flag = 0;
            break;
        case TOK_AWAIT:
            if (!(s->cur_func->func_kind & JS_FUNC_ASYNC))
                return js_parse_error(s, "unexpected 'await' keyword");
            if (!s->cur_func->in_function_body)
                return js_parse_error(s, "await in default expression");
            if (next_token(s))
                return -1;
            if (js_parse_unary(s, -1))
                return -1;
            emit_op(s, OP_await);
            exponentiation_flag = 0;
            break;
        default:
            if (js_parse_postfix_expr(s, TRUE))
                return -1;
            if (!s->got_lf &&
                (s->token.val == TOK_DEC || s->token.val == TOK_INC)) {
                int opcode, op, scope, label;
                JSAtom name;
                op = s->token.val;
                if (get_lvalue(s, &opcode, &scope, &name, &label, NULL, TRUE, op))
                    return -1;
                emit_op(s, OP_post_dec + op - TOK_DEC);
                put_lvalue(s, opcode, scope, name, label, TRUE);
                if (next_token(s))
                    return -1;
            }
            break;
    }
    if (exponentiation_flag) {
#ifdef CONFIG_BIGNUM
        if (s->token.val == TOK_POW || s->token.val == TOK_MATH_POW) {
            /* Extended exponentiation syntax rules: we extend the ES7
               grammar in order to have more intuitive semantics:
               -2**2 evaluates to -4. */
            if (!(s->cur_func->js_mode & JS_MODE_MATH)) {
                if (exponentiation_flag < 0) {
                    JS_ThrowSyntaxError(s->ctx, "unparenthesized unary expression can't appear on the left-hand side of '**'");
                    return -1;
                }
            }
            if (next_token(s))
                return -1;
            if (js_parse_unary(s, 1))
                return -1;
            emit_op(s, OP_pow);
        }
#else
        if (s->token.val == TOK_POW) {
            /* Strict ES7 exponentiation syntax rules: To solve
               conficting semantics between different implementations
               regarding the precedence of prefix operators and the
               postifx exponential, ES7 specifies that -2**2 is a
               syntax error. */
            if (exponentiation_flag < 0) {
                JS_ThrowSyntaxError(s->ctx, "unparenthesized unary expression can't appear on the left-hand side of '**'");
                return -1;
            }
            if (next_token(s))
                return -1;
            if (js_parse_unary(s, 1))
                return -1;
            emit_op(s, OP_pow);
        }
#endif
    }
    return 0;
}

static __exception int js_parse_expr_binary(JSParseState *s, int level,
                                            BOOL in_accepted)
{
    int op, opcode;

    if (level == 0) {
        return js_parse_unary(s, 1);
    }
    if (js_parse_expr_binary(s, level - 1, in_accepted))
        return -1;
    for(;;) {
        op = s->token.val;
        switch(level) {
            case 1:
                switch(op) {
                    case '*':
                        opcode = OP_mul;
                        break;
                    case '/':
                        opcode = OP_div;
                        break;
                    case '%':
#ifdef CONFIG_BIGNUM
                        if (s->cur_func->js_mode & JS_MODE_MATH)
                    opcode = OP_math_mod;
                else
#endif
                        opcode = OP_mod;
                        break;
                    default:
                        return 0;
                }
                break;
            case 2:
                switch(op) {
                    case '+':
                        opcode = OP_add;
                        break;
                    case '-':
                        opcode = OP_sub;
                        break;
                    default:
                        return 0;
                }
                break;
            case 3:
                switch(op) {
                    case TOK_SHL:
                        opcode = OP_shl;
                        break;
                    case TOK_SAR:
                        opcode = OP_sar;
                        break;
                    case TOK_SHR:
                        opcode = OP_shr;
                        break;
                    default:
                        return 0;
                }
                break;
            case 4:
                switch(op) {
                    case '<':
                        opcode = OP_lt;
                        break;
                    case '>':
                        opcode = OP_gt;
                        break;
                    case TOK_LTE:
                        opcode = OP_lte;
                        break;
                    case TOK_GTE:
                        opcode = OP_gte;
                        break;
                    case TOK_INSTANCEOF:
                        opcode = OP_instanceof;
                        break;
                    case TOK_IN:
                        if (in_accepted) {
                            opcode = OP_in;
                        } else {
                            return 0;
                        }
                        break;
                    default:
                        return 0;
                }
                break;
            case 5:
                switch(op) {
                    case TOK_EQ:
                        opcode = OP_eq;
                        break;
                    case TOK_NEQ:
                        opcode = OP_neq;
                        break;
                    case TOK_STRICT_EQ:
                        opcode = OP_strict_eq;
                        break;
                    case TOK_STRICT_NEQ:
                        opcode = OP_strict_neq;
                        break;
                    default:
                        return 0;
                }
                break;
            case 6:
                switch(op) {
                    case '&':
                        opcode = OP_and;
                        break;
                    default:
                        return 0;
                }
                break;
            case 7:
                switch(op) {
                    case '^':
                        opcode = OP_xor;
                        break;
                    default:
                        return 0;
                }
                break;
            case 8:
                switch(op) {
                    case '|':
                        opcode = OP_or;
                        break;
                    default:
                        return 0;
                }
                break;
            default:
                abort();
        }
        if (next_token(s))
            return -1;
        if (js_parse_expr_binary(s, level - 1, in_accepted))
            return -1;
        emit_op(s, opcode);
    }
    return 0;
}

static __exception int js_parse_logical_and_or(JSParseState *s, int op,
                                               BOOL in_accepted)
{
    int label1;

    if (op == TOK_LAND) {
        if (js_parse_expr_binary(s, 8, in_accepted))
            return -1;
    } else {
        if (js_parse_logical_and_or(s, TOK_LAND, in_accepted))
            return -1;
    }
    if (s->token.val == op) {
        label1 = new_label(s);

        for(;;) {
            if (next_token(s))
                return -1;
            emit_op(s, OP_dup);
            emit_goto(s, op == TOK_LAND ? OP_if_false : OP_if_true, label1);
            emit_op(s, OP_drop);

            if (op == TOK_LAND) {
                if (js_parse_expr_binary(s, 8, in_accepted))
                    return -1;
            } else {
                if (js_parse_logical_and_or(s, TOK_LAND, in_accepted))
                    return -1;
            }
            if (s->token.val != op) {
                if (s->token.val == TOK_DOUBLE_QUESTION_MARK)
                    return js_parse_error(s, "cannot mix ?? with && or ||");
                break;
            }
        }

        emit_label(s, label1);
    }
    return 0;
}

static __exception int js_parse_coalesce_expr(JSParseState *s, BOOL in_accepted)
{
    int label1;

    if (js_parse_logical_and_or(s, TOK_LOR, in_accepted))
        return -1;
    if (s->token.val == TOK_DOUBLE_QUESTION_MARK) {
        label1 = new_label(s);
        for(;;) {
            if (next_token(s))
                return -1;

            emit_op(s, OP_dup);
            emit_op(s, OP_is_undefined_or_null);
            emit_goto(s, OP_if_false, label1);
            emit_op(s, OP_drop);

            if (js_parse_expr_binary(s, 8, in_accepted))
                return -1;
            if (s->token.val != TOK_DOUBLE_QUESTION_MARK)
                break;
        }
        emit_label(s, label1);
    }
    return 0;
}

static __exception int js_parse_cond_expr(JSParseState *s, BOOL in_accepted)
{
    int label1, label2;

    if (js_parse_coalesce_expr(s, in_accepted))
        return -1;
    if (s->token.val == '?') {
        if (next_token(s))
            return -1;
        label1 = emit_goto(s, OP_if_false, -1);

        if (js_parse_assign_expr(s, TRUE))
            return -1;
        if (js_parse_expect(s, ':'))
            return -1;

        label2 = emit_goto(s, OP_goto, -1);

        emit_label(s, label1);

        if (js_parse_assign_expr(s, in_accepted))
            return -1;

        emit_label(s, label2);
    }
    return 0;
}

static void emit_return(JSParseState *s, BOOL hasval);

static __exception int js_parse_assign_expr(JSParseState *s, BOOL in_accepted)
{
    int opcode, op, scope;
    JSAtom name0 = JS_ATOM_NULL;
    JSAtom name;

    if (s->token.val == TOK_YIELD) {
        BOOL is_star = FALSE;
        if (!(s->cur_func->func_kind & JS_FUNC_GENERATOR))
            return js_parse_error(s, "unexpected 'yield' keyword");
        if (!s->cur_func->in_function_body)
            return js_parse_error(s, "yield in default expression");
        if (next_token(s))
            return -1;
        /* XXX: is there a better method to detect 'yield' without
           parameters ? */
        if (s->token.val != ';' && s->token.val != ')' &&
            s->token.val != ']' && s->token.val != '}' &&
            s->token.val != ',' && s->token.val != ':' && !s->got_lf) {
            if (s->token.val == '*') {
                is_star = TRUE;
                if (next_token(s))
                    return -1;
            }
            if (js_parse_assign_expr(s, in_accepted))
                return -1;
        } else {
            emit_op(s, OP_undefined);
        }
        if (s->cur_func->func_kind == JS_FUNC_ASYNC_GENERATOR) {
            int label_loop, label_return, label_next;
            int label_return1, label_yield, label_throw, label_throw1;
            int label_throw2;

            if (is_star) {
                label_loop = new_label(s);
                label_yield = new_label(s);

                emit_op(s, OP_for_await_of_start);

                /* remove the catch offset (XXX: could avoid pushing back
                   undefined) */
                emit_op(s, OP_drop);
                emit_op(s, OP_undefined);

                emit_op(s, OP_undefined); /* initial value */

                emit_label(s, label_loop);
                emit_op(s, OP_async_iterator_next);
                emit_op(s, OP_await);
                emit_op(s, OP_iterator_get_value_done);
                label_next = emit_goto(s, OP_if_true, -1); /* end of loop */
                emit_op(s, OP_await);
                emit_label(s, label_yield);
                emit_op(s, OP_async_yield_star);
                emit_op(s, OP_dup);
                label_return = emit_goto(s, OP_if_true, -1);
                emit_op(s, OP_drop);
                emit_goto(s, OP_goto, label_loop);

                emit_label(s, label_return);
                emit_op(s, OP_push_i32);
                emit_u32(s, 2);
                emit_op(s, OP_strict_eq);
                label_throw = emit_goto(s, OP_if_true, -1);

                /* return handling */
                emit_op(s, OP_await);
                emit_op(s, OP_async_iterator_get);
                emit_u8(s, 0);
                label_return1 = emit_goto(s, OP_if_true, -1);
                emit_op(s, OP_await);
                emit_op(s, OP_iterator_get_value_done);
                /* XXX: the spec does not indicate that an await should be
                   performed in case done = true, but the tests assume it */
                emit_goto(s, OP_if_false, label_yield);

                emit_label(s, label_return1);
                emit_op(s, OP_nip);
                emit_op(s, OP_nip);
                emit_op(s, OP_nip);
                emit_return(s, TRUE);

                /* throw handling */
                emit_label(s, label_throw);
                emit_op(s, OP_async_iterator_get);
                emit_u8(s, 1);
                label_throw1 = emit_goto(s, OP_if_true, -1);
                emit_op(s, OP_await);
                emit_op(s, OP_iterator_get_value_done);
                emit_goto(s, OP_if_false, label_yield);
                /* XXX: the spec does not indicate that an await should be
                   performed in case done = true, but the tests assume it */
                emit_op(s, OP_await);
                emit_goto(s, OP_goto, label_next);
                /* close the iterator and throw a type error exception */
                emit_label(s, label_throw1);
                emit_op(s, OP_async_iterator_get);
                emit_u8(s, 0);
                label_throw2 = emit_goto(s, OP_if_true, -1);
                emit_op(s, OP_await);
                emit_label(s, label_throw2);
                emit_op(s, OP_async_iterator_get);
                emit_u8(s, 2); /* throw the type error exception */
                emit_op(s, OP_drop); /* never reached */

                emit_label(s, label_next);
                emit_op(s, OP_nip); /* keep the value associated with
                                       done = true */
                emit_op(s, OP_nip);
                emit_op(s, OP_nip);
            } else {
                emit_op(s, OP_await);
                emit_op(s, OP_yield);
                label_next = emit_goto(s, OP_if_false, -1);
                emit_return(s, TRUE);
                emit_label(s, label_next);
            }
        } else {
            int label_next;
            if (is_star) {
                emit_op(s, OP_for_of_start);
                emit_op(s, OP_drop);    /* drop the catch offset */
                emit_op(s, OP_yield_star);
            } else {
                emit_op(s, OP_yield);
            }
            label_next = emit_goto(s, OP_if_false, -1);
            emit_return(s, TRUE);
            emit_label(s, label_next);
        }
        return 0;
    }
    if (s->token.val == TOK_IDENT) {
        /* name0 is used to check for OP_set_name pattern, not duplicated */
        name0 = s->token.u.ident.atom;
    }
    if (js_parse_cond_expr(s, in_accepted))
        return -1;

    op = s->token.val;
    if (op == '=' || (op >= TOK_MUL_ASSIGN && op <= TOK_POW_ASSIGN)) {
        int label;
        if (next_token(s))
            return -1;
        if (get_lvalue(s, &opcode, &scope, &name, &label, NULL, (op != '='), op) < 0)
            return -1;

        if (js_parse_assign_expr(s, in_accepted)) {
            JS_FreeAtom(s->ctx, name);
            return -1;
        }

        if (op == '=') {
            if (opcode == OP_get_ref_value && name == name0) {
                set_object_name(s, name);
            }
        } else {
            static const uint8_t assign_opcodes[] = {
                    OP_mul, OP_div, OP_mod, OP_add, OP_sub,
                    OP_shl, OP_sar, OP_shr, OP_and, OP_xor, OP_or,
#ifdef CONFIG_BIGNUM
                    OP_pow,
#endif
                    OP_pow,
            };
            op = assign_opcodes[op - TOK_MUL_ASSIGN];
#ifdef CONFIG_BIGNUM
            if (s->cur_func->js_mode & JS_MODE_MATH) {
                if (op == OP_mod)
                    op = OP_math_mod;
            }
#endif
            emit_op(s, op);
        }
        put_lvalue(s, opcode, scope, name, label, FALSE);
    }
    return 0;
}

static __exception int js_parse_expr2(JSParseState *s, BOOL in_accepted)
{
    BOOL comma = FALSE;
    for(;;) {
        if (js_parse_assign_expr(s, in_accepted))
            return -1;
        if (comma) {
            /* prevent get_lvalue from using the last expression
               as an lvalue. This also prevents the conversion of
               of get_var to get_ref for method lookup in function
               call inside `with` statement.
             */
            s->cur_func->last_opcode_pos = -1;
        }
        if (s->token.val != ',')
            break;
        comma = TRUE;
        if (next_token(s))
            return -1;
        emit_op(s, OP_drop);
    }
    return 0;
}

static __exception int js_parse_expr(JSParseState *s)
{
    return js_parse_expr2(s, TRUE);
}

static void push_break_entry(JSFunctionDef *fd, BlockEnv *be,
                             JSAtom label_name,
                             int label_break, int label_cont,
                             int drop_count)
{
    be->prev = fd->top_break;
    fd->top_break = be;
    be->label_name = label_name;
    be->label_break = label_break;
    be->label_cont = label_cont;
    be->drop_count = drop_count;
    be->label_finally = -1;
    be->scope_level = fd->scope_level;
    be->has_iterator = FALSE;
}

static void pop_break_entry(JSFunctionDef *fd)
{
    BlockEnv *be;
    be = fd->top_break;
    fd->top_break = be->prev;
}

static __exception int emit_break(JSParseState *s, JSAtom name, int is_cont)
{
    BlockEnv *top;
    int i, scope_level;

    scope_level = s->cur_func->scope_level;
    top = s->cur_func->top_break;
    while (top != NULL) {
        close_scopes(s, scope_level, top->scope_level);
        scope_level = top->scope_level;
        if (is_cont &&
            top->label_cont != -1 &&
            (name == JS_ATOM_NULL || top->label_name == name)) {
            /* continue stays inside the same block */
            emit_goto(s, OP_goto, top->label_cont);
            return 0;
        }
        if (!is_cont &&
            top->label_break != -1 &&
            (name == JS_ATOM_NULL || top->label_name == name)) {
            emit_goto(s, OP_goto, top->label_break);
            return 0;
        }
        i = 0;
        if (top->has_iterator) {
            emit_op(s, OP_iterator_close);
            i += 3;
        }
        for(; i < top->drop_count; i++)
            emit_op(s, OP_drop);
        if (top->label_finally != -1) {
            /* must push dummy value to keep same stack depth */
            emit_op(s, OP_undefined);
            emit_goto(s, OP_gosub, top->label_finally);
            emit_op(s, OP_drop);
        }
        top = top->prev;
    }
    if (name == JS_ATOM_NULL) {
        if (is_cont)
            return js_parse_error(s, "continue must be inside loop");
        else
            return js_parse_error(s, "break must be inside loop or switch");
    } else {
        return js_parse_error(s, "break/continue label not found");
    }
}

/* execute the finally blocks before return */
static void emit_return(JSParseState *s, BOOL hasval)
{
    BlockEnv *top;
    int drop_count;

    drop_count = 0;
    top = s->cur_func->top_break;
    while (top != NULL) {
        /* XXX: emit the appropriate OP_leave_scope opcodes? Probably not
           required as all local variables will be closed upon returning
           from JS_CallInternal, but not in the same order. */
        if (top->has_iterator) {
            /* with 'yield', the exact number of OP_drop to emit is
               unknown, so we use a specific operation to look for
               the catch offset */
            if (!hasval) {
                emit_op(s, OP_undefined);
                hasval = TRUE;
            }
            emit_op(s, OP_iterator_close_return);
            if (s->cur_func->func_kind == JS_FUNC_ASYNC_GENERATOR) {
                int label_next;
                emit_op(s, OP_async_iterator_close);
                label_next = emit_goto(s, OP_if_true, -1);
                emit_op(s, OP_await);
                emit_label(s, label_next);
                emit_op(s, OP_drop);
            } else {
                emit_op(s, OP_iterator_close);
            }
            drop_count = -3;
        }
        drop_count += top->drop_count;
        if (top->label_finally != -1) {
            while(drop_count) {
                /* must keep the stack top if hasval */
                emit_op(s, hasval ? OP_nip : OP_drop);
                drop_count--;
            }
            if (!hasval) {
                /* must push return value to keep same stack size */
                emit_op(s, OP_undefined);
                hasval = TRUE;
            }
            emit_goto(s, OP_gosub, top->label_finally);
        }
        top = top->prev;
    }
    if (s->cur_func->is_derived_class_constructor) {
        int label_return;

        /* 'this' can be uninitialized, so it may be accessed only if
           the derived class constructor does not return an object */
        if (hasval) {
            emit_op(s, OP_check_ctor_return);
            label_return = emit_goto(s, OP_if_false, -1);
            emit_op(s, OP_drop);
        } else {
            label_return = -1;
        }

        /* XXX: if this is not initialized, should throw the
           ReferenceError in the caller realm */
        emit_op(s, OP_scope_get_var);
        emit_atom(s, JS_ATOM_this);
        emit_u16(s, 0);

        emit_label(s, label_return);
        emit_op(s, OP_return);
    } else if (s->cur_func->func_kind != JS_FUNC_NORMAL) {
        if (!hasval) {
            emit_op(s, OP_undefined);
        } else if (s->cur_func->func_kind == JS_FUNC_ASYNC_GENERATOR) {
            emit_op(s, OP_await);
        }
        emit_op(s, OP_return_async);
    } else {
        emit_op(s, hasval ? OP_return : OP_return_undef);
    }
}

#define DECL_MASK_FUNC  (1 << 0) /* allow normal function declaration */
/* ored with DECL_MASK_FUNC if function declarations are allowed with a label */
#define DECL_MASK_FUNC_WITH_LABEL (1 << 1)
#define DECL_MASK_OTHER (1 << 2) /* all other declarations */
#define DECL_MASK_ALL   (DECL_MASK_FUNC | DECL_MASK_FUNC_WITH_LABEL | DECL_MASK_OTHER)

static __exception int js_parse_statement_or_decl(JSParseState *s,
                                                  int decl_mask);

static __exception int js_parse_statement(JSParseState *s)
{
    return js_parse_statement_or_decl(s, 0);
}

static __exception int js_parse_block(JSParseState *s)
{
    if (js_parse_expect(s, '{'))
        return -1;
    if (s->token.val != '}') {
        push_scope(s);
        for(;;) {
            if (js_parse_statement_or_decl(s, DECL_MASK_ALL))
                return -1;
            if (s->token.val == '}')
                break;
        }
        pop_scope(s);
    }
    if (next_token(s))
        return -1;
    return 0;
}

static __exception int js_parse_var(JSParseState *s, BOOL in_accepted, int tok,
                                    BOOL export_flag)
{
    JSContext *ctx = s->ctx;
    JSFunctionDef *fd = s->cur_func;
    JSAtom name = JS_ATOM_NULL;

    for (;;) {
        if (s->token.val == TOK_IDENT) {
            if (s->token.u.ident.is_reserved) {
                return js_parse_error_reserved_identifier(s);
            }
            name = JS_DupAtom(ctx, s->token.u.ident.atom);
            if (name == JS_ATOM_let && (tok == TOK_LET || tok == TOK_CONST)) {
                js_parse_error(s, "'let' is not a valid lexical identifier");
                goto var_error;
            }
            if (next_token(s))
                goto var_error;
            if (js_define_var(s, name, tok))
                goto var_error;
            if (export_flag) {
                if (!add_export_entry(s, s->cur_func->module, name, name,
                                      JS_EXPORT_TYPE_LOCAL))
                    goto var_error;
            }

            if (s->token.val == '=') {
                if (next_token(s))
                    goto var_error;
                if (tok == TOK_VAR) {
                    /* Must make a reference for proper `with` semantics */
                    int opcode, scope, label;
                    JSAtom name1;

                    emit_op(s, OP_scope_get_var);
                    emit_atom(s, name);
                    emit_u16(s, fd->scope_level);
                    if (get_lvalue(s, &opcode, &scope, &name1, &label, NULL, FALSE, '=') < 0)
                        goto var_error;
                    if (js_parse_assign_expr(s, in_accepted)) {
                        JS_FreeAtom(ctx, name1);
                        goto var_error;
                    }
                    set_object_name(s, name);
                    put_lvalue(s, opcode, scope, name1, label, FALSE);
                    emit_op(s, OP_drop);
                } else {
                    if (js_parse_assign_expr(s, in_accepted))
                        goto var_error;
                    set_object_name(s, name);
                    emit_op(s, (tok == TOK_CONST || tok == TOK_LET) ?
                               OP_scope_put_var_init : OP_scope_put_var);
                    emit_atom(s, name);
                    emit_u16(s, fd->scope_level);
                }
            } else {
                if (tok == TOK_CONST) {
                    js_parse_error(s, "missing initializer for const variable");
                    goto var_error;
                }
                if (tok == TOK_LET) {
                    /* initialize lexical variable upon entering its scope */
                    emit_op(s, OP_undefined);
                    emit_op(s, OP_scope_put_var_init);
                    emit_atom(s, name);
                    emit_u16(s, fd->scope_level);
                }
            }
            JS_FreeAtom(ctx, name);
        } else {
            int skip_bits;
            if ((s->token.val == '[' || s->token.val == '{')
                &&  js_parse_skip_parens_token(s, &skip_bits, FALSE) == '=') {
                emit_op(s, OP_undefined);
                if (js_parse_destructuring_element(s, tok, 0, TRUE, skip_bits & SKIP_HAS_ELLIPSIS, TRUE))
                    return -1;
            } else {
                return js_parse_error(s, "variable name expected");
            }
        }
        if (s->token.val != ',')
            break;
        if (next_token(s))
            return -1;
    }
    return 0;

    var_error:
    JS_FreeAtom(ctx, name);
    return -1;
}

/* test if the current token is a label. Use simplistic look-ahead scanner */
static BOOL is_label(JSParseState *s)
{
    return (s->token.val == TOK_IDENT && !s->token.u.ident.is_reserved &&
            peek_token(s, FALSE) == ':');
}

/* test if the current token is a let keyword. Use simplistic look-ahead scanner */
static int is_let(JSParseState *s, int decl_mask)
{
    int res = FALSE;

    if (token_is_pseudo_keyword(s, JS_ATOM_let)) {
#if 1
        JSParsePos pos;
        js_parse_get_pos(s, &pos);
        for (;;) {
            if (next_token(s)) {
                res = -1;
                break;
            }
            if (s->token.val == '[') {
                /* let [ is a syntax restriction:
                   it never introduces an ExpressionStatement */
                res = TRUE;
                break;
            }
            if (s->token.val == '{' ||
                (s->token.val == TOK_IDENT && !s->token.u.ident.is_reserved) ||
                s->token.val == TOK_LET ||
                s->token.val == TOK_YIELD ||
                s->token.val == TOK_AWAIT) {
                /* Check for possible ASI if not scanning for Declaration */
                /* XXX: should also check that `{` introduces a BindingPattern,
                   but Firefox does not and rejects eval("let=1;let\n{if(1)2;}") */
                if (s->last_line_num == s->token.line_num || (decl_mask & DECL_MASK_OTHER)) {
                    res = TRUE;
                    break;
                }
                break;
            }
            break;
        }
        if (js_parse_seek_token(s, &pos)) {
            res = -1;
        }
#else
        int tok = peek_token(s, TRUE);
        if (tok == '{' || tok == TOK_IDENT || peek_token(s, FALSE) == '[') {
            res = TRUE;
        }
#endif
    }
    return res;
}

/* XXX: handle IteratorClose when exiting the loop before the
   enumeration is done */
static __exception int js_parse_for_in_of(JSParseState *s, int label_name,
                                          BOOL is_async)
{
    JSContext *ctx = s->ctx;
    JSFunctionDef *fd = s->cur_func;
    JSAtom var_name;
    BOOL has_initializer, is_for_of, has_destructuring;
    int tok, tok1, opcode, scope, block_scope_level;
    int label_next, label_expr, label_cont, label_body, label_break;
    int pos_next, pos_expr;
    BlockEnv break_entry;

    has_initializer = FALSE;
    has_destructuring = FALSE;
    is_for_of = FALSE;
    block_scope_level = fd->scope_level;
    label_cont = new_label(s);
    label_body = new_label(s);
    label_break = new_label(s);
    label_next = new_label(s);

    /* create scope for the lexical variables declared in the enumeration
       expressions. XXX: Not completely correct because of weird capturing
       semantics in `for (i of o) a.push(function(){return i})` */
    push_scope(s);

    /* local for_in scope starts here so individual elements
       can be closed in statement. */
    push_break_entry(s->cur_func, &break_entry,
                     label_name, label_break, label_cont, 1);
    break_entry.scope_level = block_scope_level;

    label_expr = emit_goto(s, OP_goto, -1);

    pos_next = s->cur_func->byte_code.size;
    emit_label(s, label_next);

    tok = s->token.val;
    switch (is_let(s, DECL_MASK_OTHER)) {
        case TRUE:
            tok = TOK_LET;
            break;
        case FALSE:
            break;
        default:
            return -1;
    }
    if (tok == TOK_VAR || tok == TOK_LET || tok == TOK_CONST) {
        if (next_token(s))
            return -1;

        if (!(s->token.val == TOK_IDENT && !s->token.u.ident.is_reserved)) {
            if (s->token.val == '[' || s->token.val == '{') {
                if (js_parse_destructuring_element(s, tok, 0, TRUE, -1, FALSE))
                    return -1;
                has_destructuring = TRUE;
            } else {
                return js_parse_error(s, "variable name expected");
            }
            var_name = JS_ATOM_NULL;
        } else {
            var_name = JS_DupAtom(ctx, s->token.u.ident.atom);
            if (next_token(s)) {
                JS_FreeAtom(s->ctx, var_name);
                return -1;
            }
            if (js_define_var(s, var_name, tok)) {
                JS_FreeAtom(s->ctx, var_name);
                return -1;
            }
            emit_op(s, (tok == TOK_CONST || tok == TOK_LET) ?
                       OP_scope_put_var_init : OP_scope_put_var);
            emit_atom(s, var_name);
            emit_u16(s, fd->scope_level);
        }
    } else {
        int skip_bits;
        if ((s->token.val == '[' || s->token.val == '{')
            &&  ((tok1 = js_parse_skip_parens_token(s, &skip_bits, FALSE)) == TOK_IN || tok1 == TOK_OF)) {
            if (js_parse_destructuring_element(s, 0, 0, TRUE, skip_bits & SKIP_HAS_ELLIPSIS, TRUE))
                return -1;
        } else {
            int lvalue_label, depth;
            if (js_parse_postfix_expr(s, TRUE))
                return -1;
            if (get_lvalue(s, &opcode, &scope, &var_name, &lvalue_label,
                           &depth, FALSE, TOK_FOR))
                return -1;
            /* swap value and lvalue object and store it into lvalue object */
            /* enum_rec val [depth] -- enum_rec */
            switch(depth) {
                case 1:
                    emit_op(s, OP_swap);
                    break;
                case 2:
                    emit_op(s, OP_rot3l);
                    break;
                case 3:
                    emit_op(s, OP_rot4l);
                    break;
                default:
                    abort();
            }
            put_lvalue_nokeep(s, opcode, scope, var_name, lvalue_label,
                              TOK_FOR /* not used */);
        }
        var_name = JS_ATOM_NULL;
    }
    emit_goto(s, OP_goto, label_body);

    pos_expr = s->cur_func->byte_code.size;
    emit_label(s, label_expr);
    if (s->token.val == '=') {
        /* XXX: potential scoping issue if inside `with` statement */
        has_initializer = TRUE;
        /* parse and evaluate initializer prior to evaluating the
           object (only used with "for in" with a non lexical variable
           in non strict mode */
        if (next_token(s) || js_parse_assign_expr(s, FALSE)) {
            JS_FreeAtom(ctx, var_name);
            return -1;
        }
        if (var_name != JS_ATOM_NULL) {
            emit_op(s, OP_scope_put_var);
            emit_atom(s, var_name);
            emit_u16(s, fd->scope_level);
        }
    }
    JS_FreeAtom(ctx, var_name);

    if (token_is_pseudo_keyword(s, JS_ATOM_of)) {
        break_entry.has_iterator = is_for_of = TRUE;
        break_entry.drop_count += 2;
        if (has_initializer)
            goto initializer_error;
    } else if (s->token.val == TOK_IN) {
        if (is_async)
            return js_parse_error(s, "'for await' loop should be used with 'of'");
        if (has_initializer &&
            (tok != TOK_VAR || (fd->js_mode & JS_MODE_STRICT) ||
             has_destructuring)) {
            initializer_error:
            return js_parse_error(s, "a declaration in the head of a for-%s loop can't have an initializer",
                                  is_for_of ? "of" : "in");
        }
    } else {
        return js_parse_error(s, "expected 'of' or 'in' in for control expression");
    }
    if (next_token(s))
        return -1;
    if (is_for_of) {
        if (js_parse_assign_expr(s, TRUE))
            return -1;
    } else {
        if (js_parse_expr(s))
            return -1;
    }
    /* close the scope after having evaluated the expression so that
       the TDZ values are in the closures */
    close_scopes(s, s->cur_func->scope_level, block_scope_level);
    if (is_for_of) {
        if (is_async)
            emit_op(s, OP_for_await_of_start);
        else
            emit_op(s, OP_for_of_start);
        /* on stack: enum_rec */
    } else {
        emit_op(s, OP_for_in_start);
        /* on stack: enum_obj */
    }
    emit_goto(s, OP_goto, label_cont);

    if (js_parse_expect(s, ')'))
        return -1;

    if (OPTIMIZE) {
        /* move the `next` code here */
        DynBuf *bc = &s->cur_func->byte_code;
        int chunk_size = pos_expr - pos_next;
        int offset = bc->size - pos_next;
        int i;
        dbuf_realloc(bc, bc->size + chunk_size);
        dbuf_put(bc, bc->buf + pos_next, chunk_size);
        memset(bc->buf + pos_next, OP_nop, chunk_size);
        /* `next` part ends with a goto */
        s->cur_func->last_opcode_pos = bc->size - 5;
        /* relocate labels */
        for (i = label_cont; i < s->cur_func->label_count; i++) {
            LabelSlot *ls = &s->cur_func->label_slots[i];
            if (ls->pos >= pos_next && ls->pos < pos_expr)
                ls->pos += offset;
        }
    }

    emit_label(s, label_body);
    if (js_parse_statement(s))
        return -1;

    close_scopes(s, s->cur_func->scope_level, block_scope_level);

    emit_label(s, label_cont);
    if (is_for_of) {
        if (is_async) {
            /* call the next method */
            emit_op(s, OP_for_await_of_next);
            /* get the result of the promise */
            emit_op(s, OP_await);
            /* unwrap the value and done values */
            emit_op(s, OP_iterator_get_value_done);
        } else {
            emit_op(s, OP_for_of_next);
            emit_u8(s, 0);
        }
    } else {
        emit_op(s, OP_for_in_next);
    }
    /* on stack: enum_rec / enum_obj value bool */
    emit_goto(s, OP_if_false, label_next);
    /* drop the undefined value from for_xx_next */
    emit_op(s, OP_drop);

    emit_label(s, label_break);
    if (is_for_of) {
        /* close and drop enum_rec */
        emit_op(s, OP_iterator_close);
    } else {
        emit_op(s, OP_drop);
    }
    pop_break_entry(s->cur_func);
    pop_scope(s);
    return 0;
}

static void set_eval_ret_undefined(JSParseState *s)
{
    if (s->cur_func->eval_ret_idx >= 0) {
        emit_op(s, OP_undefined);
        emit_op(s, OP_put_loc);
        emit_u16(s, s->cur_func->eval_ret_idx);
    }
}

static __exception int js_parse_statement_or_decl(JSParseState *s,
                                                  int decl_mask)
{
    JSContext *ctx = s->ctx;
    JSAtom label_name;
    int tok;

    /* specific label handling */
    /* XXX: support multiple labels on loop statements */
    label_name = JS_ATOM_NULL;
    if (is_label(s)) {
        BlockEnv *be;

        label_name = JS_DupAtom(ctx, s->token.u.ident.atom);

        for (be = s->cur_func->top_break; be; be = be->prev) {
            if (be->label_name == label_name) {
                js_parse_error(s, "duplicate label name");
                goto fail;
            }
        }

        if (next_token(s))
            goto fail;
        if (js_parse_expect(s, ':'))
            goto fail;
        if (s->token.val != TOK_FOR
            &&  s->token.val != TOK_DO
            &&  s->token.val != TOK_WHILE) {
            /* labelled regular statement */
            int label_break, mask;
            BlockEnv break_entry;

            label_break = new_label(s);
            push_break_entry(s->cur_func, &break_entry,
                             label_name, label_break, -1, 0);
            if (!(s->cur_func->js_mode & JS_MODE_STRICT) &&
                (decl_mask & DECL_MASK_FUNC_WITH_LABEL)) {
                mask = DECL_MASK_FUNC | DECL_MASK_FUNC_WITH_LABEL;
            } else {
                mask = 0;
            }
            if (js_parse_statement_or_decl(s, mask))
                goto fail;
            emit_label(s, label_break);
            pop_break_entry(s->cur_func);
            goto done;
        }
    }

    switch(tok = s->token.val) {
        case '{':
            if (js_parse_block(s))
                goto fail;
            break;
        case TOK_RETURN:
            if (s->cur_func->is_eval) {
                js_parse_error(s, "return not in a function");
                goto fail;
            }
            if (next_token(s))
                goto fail;
            if (s->token.val != ';' && s->token.val != '}' && !s->got_lf) {
                if (js_parse_expr(s))
                    goto fail;
                emit_return(s, TRUE);
            } else {
                emit_return(s, FALSE);
            }
            if (js_parse_expect_semi(s))
                goto fail;
            break;
        case TOK_THROW:
            if (next_token(s))
                goto fail;
            if (s->got_lf) {
                js_parse_error(s, "line terminator not allowed after throw");
                goto fail;
            }
            if (js_parse_expr(s))
                goto fail;
            emit_op(s, OP_throw);
            if (js_parse_expect_semi(s))
                goto fail;
            break;
        case TOK_LET:
        case TOK_CONST:
        haslet:
            if (!(decl_mask & DECL_MASK_OTHER)) {
                js_parse_error(s, "lexical declarations can't appear in single-statement context");
                goto fail;
            }
            /* fall thru */
        case TOK_VAR:
            if (next_token(s))
                goto fail;
            if (js_parse_var(s, TRUE, tok, FALSE))
                goto fail;
            if (js_parse_expect_semi(s))
                goto fail;
            break;
        case TOK_IF:
        {
            int label1, label2, mask;
            if (next_token(s))
                goto fail;
            /* create a new scope for `let f;if(1) function f(){}` */
            push_scope(s);
            set_eval_ret_undefined(s);
            if (js_parse_expr_paren(s))
                goto fail;
            label1 = emit_goto(s, OP_if_false, -1);
            if (s->cur_func->js_mode & JS_MODE_STRICT)
                mask = 0;
            else
                mask = DECL_MASK_FUNC; /* Annex B.3.4 */

            if (js_parse_statement_or_decl(s, mask))
                goto fail;

            if (s->token.val == TOK_ELSE) {
                label2 = emit_goto(s, OP_goto, -1);
                if (next_token(s))
                    goto fail;

                emit_label(s, label1);
                if (js_parse_statement_or_decl(s, mask))
                    goto fail;

                label1 = label2;
            }
            emit_label(s, label1);
            pop_scope(s);
        }
            break;
        case TOK_WHILE:
        {
            int label_cont, label_break;
            BlockEnv break_entry;

            label_cont = new_label(s);
            label_break = new_label(s);

            push_break_entry(s->cur_func, &break_entry,
                             label_name, label_break, label_cont, 0);

            if (next_token(s))
                goto fail;

            set_eval_ret_undefined(s);

            emit_label(s, label_cont);
            if (js_parse_expr_paren(s))
                goto fail;
            emit_goto(s, OP_if_false, label_break);

            if (js_parse_statement(s))
                goto fail;
            emit_goto(s, OP_goto, label_cont);

            emit_label(s, label_break);

            pop_break_entry(s->cur_func);
        }
            break;
        case TOK_DO:
        {
            int label_cont, label_break, label1;
            BlockEnv break_entry;

            label_cont = new_label(s);
            label_break = new_label(s);
            label1 = new_label(s);

            push_break_entry(s->cur_func, &break_entry,
                             label_name, label_break, label_cont, 0);

            if (next_token(s))
                goto fail;

            emit_label(s, label1);

            set_eval_ret_undefined(s);

            if (js_parse_statement(s))
                goto fail;

            emit_label(s, label_cont);
            if (js_parse_expect(s, TOK_WHILE))
                goto fail;
            if (js_parse_expr_paren(s))
                goto fail;
            /* Insert semicolon if missing */
            if (s->token.val == ';') {
                if (next_token(s))
                    goto fail;
            }
            emit_goto(s, OP_if_true, label1);

            emit_label(s, label_break);

            pop_break_entry(s->cur_func);
        }
            break;
        case TOK_FOR:
        {
            int label_cont, label_break, label_body, label_test;
            int pos_cont, pos_body, block_scope_level;
            BlockEnv break_entry;
            int tok, bits;
            BOOL is_async;

            if (next_token(s))
                goto fail;

            set_eval_ret_undefined(s);
            bits = 0;
            is_async = FALSE;
            if (s->token.val == '(') {
                js_parse_skip_parens_token(s, &bits, FALSE);
            } else if (s->token.val == TOK_AWAIT) {
                if (!(s->cur_func->func_kind & JS_FUNC_ASYNC)) {
                    js_parse_error(s, "for await is only valid in asynchronous functions");
                    goto fail;
                }
                is_async = TRUE;
                if (next_token(s))
                    goto fail;
            }
            if (js_parse_expect(s, '('))
                goto fail;

            if (!(bits & SKIP_HAS_SEMI)) {
                /* parse for/in or for/of */
                if (js_parse_for_in_of(s, label_name, is_async))
                    goto fail;
                break;
            }
            block_scope_level = s->cur_func->scope_level;

            /* create scope for the lexical variables declared in the initial,
               test and increment expressions */
            push_scope(s);
            /* initial expression */
            tok = s->token.val;
            if (tok != ';') {
                switch (is_let(s, DECL_MASK_OTHER)) {
                    case TRUE:
                        tok = TOK_LET;
                        break;
                    case FALSE:
                        break;
                    default:
                        goto fail;
                }
                if (tok == TOK_VAR || tok == TOK_LET || tok == TOK_CONST) {
                    if (next_token(s))
                        goto fail;
                    if (js_parse_var(s, FALSE, tok, FALSE))
                        goto fail;
                } else {
                    if (js_parse_expr2(s, FALSE))
                        goto fail;
                    emit_op(s, OP_drop);
                }

                /* close the closures before the first iteration */
                close_scopes(s, s->cur_func->scope_level, block_scope_level);
            }
            if (js_parse_expect(s, ';'))
                goto fail;

            label_test = new_label(s);
            label_cont = new_label(s);
            label_body = new_label(s);
            label_break = new_label(s);

            push_break_entry(s->cur_func, &break_entry,
                             label_name, label_break, label_cont, 0);

            /* test expression */
            if (s->token.val == ';') {
                /* no test expression */
                label_test = label_body;
            } else {
                emit_label(s, label_test);
                if (js_parse_expr(s))
                    goto fail;
                emit_goto(s, OP_if_false, label_break);
            }
            if (js_parse_expect(s, ';'))
                goto fail;

            if (s->token.val == ')') {
                /* no end expression */
                break_entry.label_cont = label_cont = label_test;
                pos_cont = 0; /* avoid warning */
            } else {
                /* skip the end expression */
                emit_goto(s, OP_goto, label_body);

                pos_cont = s->cur_func->byte_code.size;
                emit_label(s, label_cont);
                if (js_parse_expr(s))
                    goto fail;
                emit_op(s, OP_drop);
                if (label_test != label_body)
                    emit_goto(s, OP_goto, label_test);
            }
            if (js_parse_expect(s, ')'))
                goto fail;

            pos_body = s->cur_func->byte_code.size;
            emit_label(s, label_body);
            if (js_parse_statement(s))
                goto fail;

            /* close the closures before the next iteration */
            /* XXX: check continue case */
            close_scopes(s, s->cur_func->scope_level, block_scope_level);

            if (OPTIMIZE && label_test != label_body && label_cont != label_test) {
                /* move the increment code here */
                DynBuf *bc = &s->cur_func->byte_code;
                int chunk_size = pos_body - pos_cont;
                int offset = bc->size - pos_cont;
                int i;
                dbuf_realloc(bc, bc->size + chunk_size);
                dbuf_put(bc, bc->buf + pos_cont, chunk_size);
                memset(bc->buf + pos_cont, OP_nop, chunk_size);
                /* increment part ends with a goto */
                s->cur_func->last_opcode_pos = bc->size - 5;
                /* relocate labels */
                for (i = label_cont; i < s->cur_func->label_count; i++) {
                    LabelSlot *ls = &s->cur_func->label_slots[i];
                    if (ls->pos >= pos_cont && ls->pos < pos_body)
                        ls->pos += offset;
                }
            } else {
                emit_goto(s, OP_goto, label_cont);
            }

            emit_label(s, label_break);

            pop_break_entry(s->cur_func);
            pop_scope(s);
        }
            break;
        case TOK_BREAK:
        case TOK_CONTINUE:
        {
            int is_cont = s->token.val - TOK_BREAK;
            int label;

            if (next_token(s))
                goto fail;
            if (!s->got_lf && s->token.val == TOK_IDENT && !s->token.u.ident.is_reserved)
                label = s->token.u.ident.atom;
            else
                label = JS_ATOM_NULL;
            if (emit_break(s, label, is_cont))
                goto fail;
            if (label != JS_ATOM_NULL) {
                if (next_token(s))
                    goto fail;
            }
            if (js_parse_expect_semi(s))
                goto fail;
        }
            break;
        case TOK_SWITCH:
        {
            int label_case, label_break, label1;
            int default_label_pos;
            BlockEnv break_entry;

            if (next_token(s))
                goto fail;

            set_eval_ret_undefined(s);
            if (js_parse_expr_paren(s))
                goto fail;

            push_scope(s);
            label_break = new_label(s);
            push_break_entry(s->cur_func, &break_entry,
                             label_name, label_break, -1, 1);

            if (js_parse_expect(s, '{'))
                goto fail;

            default_label_pos = -1;
            label_case = -1;
            while (s->token.val != '}') {
                if (s->token.val == TOK_CASE) {
                    label1 = -1;
                    if (label_case >= 0) {
                        /* skip the case if needed */
                        label1 = emit_goto(s, OP_goto, -1);
                    }
                    emit_label(s, label_case);
                    label_case = -1;
                    for (;;) {
                        /* parse a sequence of case clauses */
                        if (next_token(s))
                            goto fail;
                        emit_op(s, OP_dup);
                        if (js_parse_expr(s))
                            goto fail;
                        if (js_parse_expect(s, ':'))
                            goto fail;
                        emit_op(s, OP_strict_eq);
                        if (s->token.val == TOK_CASE) {
                            label1 = emit_goto(s, OP_if_true, label1);
                        } else {
                            label_case = emit_goto(s, OP_if_false, -1);
                            emit_label(s, label1);
                            break;
                        }
                    }
                } else if (s->token.val == TOK_DEFAULT) {
                    if (next_token(s))
                        goto fail;
                    if (js_parse_expect(s, ':'))
                        goto fail;
                    if (default_label_pos >= 0) {
                        js_parse_error(s, "duplicate default");
                        goto fail;
                    }
                    if (label_case < 0) {
                        /* falling thru direct from switch expression */
                        label_case = emit_goto(s, OP_goto, -1);
                    }
                    /* Emit a dummy label opcode. Label will be patched after
                       the end of the switch body. Do not use emit_label(s, 0)
                       because it would clobber label 0 address, preventing
                       proper optimizer operation.
                     */
                    emit_op(s, OP_label);
                    emit_u32(s, 0);
                    default_label_pos = s->cur_func->byte_code.size - 4;
                } else {
                    if (label_case < 0) {
                        /* falling thru direct from switch expression */
                        js_parse_error(s, "invalid switch statement");
                        goto fail;
                    }
                    if (js_parse_statement_or_decl(s, DECL_MASK_ALL))
                        goto fail;
                }
            }
            if (js_parse_expect(s, '}'))
                goto fail;
            if (default_label_pos >= 0) {
                /* Ugly patch for the the `default` label, shameful and risky */
                put_u32(s->cur_func->byte_code.buf + default_label_pos,
                        label_case);
                s->cur_func->label_slots[label_case].pos = default_label_pos + 4;
            } else {
                emit_label(s, label_case);
            }
            emit_label(s, label_break);
            emit_op(s, OP_drop); /* drop the switch expression */

            pop_break_entry(s->cur_func);
            pop_scope(s);
        }
            break;
        case TOK_TRY:
        {
            int label_catch, label_catch2, label_finally, label_end;
            JSAtom name;
            BlockEnv block_env;

            set_eval_ret_undefined(s);
            if (next_token(s))
                goto fail;
            label_catch = new_label(s);
            label_catch2 = new_label(s);
            label_finally = new_label(s);
            label_end = new_label(s);

            emit_goto(s, OP_catch, label_catch);

            push_break_entry(s->cur_func, &block_env,
                             JS_ATOM_NULL, -1, -1, 1);
            block_env.label_finally = label_finally;

            if (js_parse_block(s))
                goto fail;

            pop_break_entry(s->cur_func);

            if (js_is_live_code(s)) {
                /* drop the catch offset */
                emit_op(s, OP_drop);
                /* must push dummy value to keep same stack size */
                emit_op(s, OP_undefined);
                emit_goto(s, OP_gosub, label_finally);
                emit_op(s, OP_drop);

                emit_goto(s, OP_goto, label_end);
            }

            if (s->token.val == TOK_CATCH) {
                if (next_token(s))
                    goto fail;

                push_scope(s);  /* catch variable */
                emit_label(s, label_catch);

                if (s->token.val == '{') {
                    /* support optional-catch-binding feature */
                    emit_op(s, OP_drop);    /* pop the exception object */
                } else {
                    if (js_parse_expect(s, '('))
                        goto fail;
                    if (!(s->token.val == TOK_IDENT && !s->token.u.ident.is_reserved)) {
                        if (s->token.val == '[' || s->token.val == '{') {
                            /* XXX: TOK_LET is not completely correct */
                            if (js_parse_destructuring_element(s, TOK_LET, 0, TRUE, -1, TRUE))
                                goto fail;
                        } else {
                            js_parse_error(s, "identifier expected");
                            goto fail;
                        }
                    } else {
                        name = JS_DupAtom(ctx, s->token.u.ident.atom);
                        if (next_token(s)
                            ||  js_define_var(s, name, TOK_CATCH) < 0) {
                            JS_FreeAtom(ctx, name);
                            goto fail;
                        }
                        /* store the exception value in the catch variable */
                        emit_op(s, OP_scope_put_var);
                        emit_u32(s, name);
                        emit_u16(s, s->cur_func->scope_level);
                    }
                    if (js_parse_expect(s, ')'))
                        goto fail;
                }
                /* XXX: should keep the address to nop it out if there is no finally block */
                emit_goto(s, OP_catch, label_catch2);

                push_scope(s);  /* catch block */
                push_break_entry(s->cur_func, &block_env, JS_ATOM_NULL,
                                 -1, -1, 1);
                block_env.label_finally = label_finally;

                if (js_parse_block(s))
                    goto fail;

                pop_break_entry(s->cur_func);
                pop_scope(s);  /* catch block */
                pop_scope(s);  /* catch variable */

                if (js_is_live_code(s)) {
                    /* drop the catch2 offset */
                    emit_op(s, OP_drop);
                    /* XXX: should keep the address to nop it out if there is no finally block */
                    /* must push dummy value to keep same stack size */
                    emit_op(s, OP_undefined);
                    emit_goto(s, OP_gosub, label_finally);
                    emit_op(s, OP_drop);
                    emit_goto(s, OP_goto, label_end);
                }
                /* catch exceptions thrown in the catch block to execute the
                 * finally clause and rethrow the exception */
                emit_label(s, label_catch2);
                /* catch value is at TOS, no need to push undefined */
                emit_goto(s, OP_gosub, label_finally);
                emit_op(s, OP_throw);

            } else if (s->token.val == TOK_FINALLY) {
                /* finally without catch : execute the finally clause
                 * and rethrow the exception */
                emit_label(s, label_catch);
                /* catch value is at TOS, no need to push undefined */
                emit_goto(s, OP_gosub, label_finally);
                emit_op(s, OP_throw);
            } else {
                js_parse_error(s, "expecting catch or finally");
                goto fail;
            }
            emit_label(s, label_finally);
            if (s->token.val == TOK_FINALLY) {
                int saved_eval_ret_idx;
                if (next_token(s))
                    goto fail;
                /* on the stack: ret_value gosub_ret_value */
                push_break_entry(s->cur_func, &block_env, JS_ATOM_NULL,
                                 -1, -1, 2);
                saved_eval_ret_idx = s->cur_func->eval_ret_idx;
                s->cur_func->eval_ret_idx = -1;
                /* 'finally' does not update eval_ret */
                if (js_parse_block(s))
                    goto fail;
                s->cur_func->eval_ret_idx = saved_eval_ret_idx;
                pop_break_entry(s->cur_func);
            }
            emit_op(s, OP_ret);
            emit_label(s, label_end);
        }
            break;
        case ';':
            /* empty statement */
            if (next_token(s))
                goto fail;
            break;
        case TOK_WITH:
            if (s->cur_func->js_mode & JS_MODE_STRICT) {
                js_parse_error(s, "invalid keyword: with");
                goto fail;
            } else {
                int with_idx;

                if (next_token(s))
                    goto fail;

                if (js_parse_expr_paren(s))
                    goto fail;

                push_scope(s);
                with_idx = define_var(s, s->cur_func, JS_ATOM__with_,
                                      JS_VAR_DEF_WITH);
                if (with_idx < 0)
                    goto fail;
                emit_op(s, OP_to_object);
                emit_op(s, OP_put_loc);
                emit_u16(s, with_idx);

                set_eval_ret_undefined(s);
                if (js_parse_statement(s))
                    goto fail;

                /* Popping scope drops lexical context for the with object variable */
                pop_scope(s);
            }
            break;
        case TOK_FUNCTION:
            /* ES6 Annex B.3.2 and B.3.3 semantics */
            if (!(decl_mask & DECL_MASK_FUNC))
                goto func_decl_error;
            if (!(decl_mask & DECL_MASK_OTHER) && peek_token(s, FALSE) == '*')
                goto func_decl_error;
            goto parse_func_var;
        case TOK_IDENT:
            if (s->token.u.ident.is_reserved) {
                js_parse_error_reserved_identifier(s);
                goto fail;
            }
            /* Determine if `let` introduces a Declaration or an ExpressionStatement */
            switch (is_let(s, decl_mask)) {
                case TRUE:
                    tok = TOK_LET;
                    goto haslet;
                case FALSE:
                    break;
                default:
                    goto fail;
            }
            if (token_is_pseudo_keyword(s, JS_ATOM_async) &&
                peek_token(s, TRUE) == TOK_FUNCTION) {
                if (!(decl_mask & DECL_MASK_OTHER)) {
                    func_decl_error:
                    js_parse_error(s, "function declarations can't appear in single-statement context");
                    goto fail;
                }
                parse_func_var:
                if (js_parse_function_decl(s, JS_PARSE_FUNC_VAR,
                                           JS_FUNC_NORMAL, JS_ATOM_NULL,
                                           s->token.ptr, s->token.line_num))
                    goto fail;
                break;
            }
            goto hasexpr;

        case TOK_CLASS:
            if (!(decl_mask & DECL_MASK_OTHER)) {
                js_parse_error(s, "class declarations can't appear in single-statement context");
                goto fail;
            }
            if (js_parse_class(s, FALSE, JS_PARSE_EXPORT_NONE))
                return -1;
            break;

        case TOK_DEBUGGER:
            /* currently no debugger, so just skip the keyword */
            if (next_token(s))
                goto fail;
            if (js_parse_expect_semi(s))
                goto fail;
            break;

        case TOK_ENUM:
        case TOK_EXPORT:
        case TOK_EXTENDS:
            js_unsupported_keyword(s, s->token.u.ident.atom);
            goto fail;

        default:
        hasexpr:
            if (js_parse_expr(s))
                goto fail;
            if (s->cur_func->eval_ret_idx >= 0) {
                /* store the expression value so that it can be returned
               by eval() */
                emit_op(s, OP_put_loc);
                emit_u16(s, s->cur_func->eval_ret_idx);
            } else {
                emit_op(s, OP_drop); /* drop the result */
            }
            if (js_parse_expect_semi(s))
                goto fail;
            break;
    }
    done:
    JS_FreeAtom(ctx, label_name);
    return 0;
    fail:
    JS_FreeAtom(ctx, label_name);
    return -1;
}

/* 'name' is freed */
static JSModuleDef *js_new_module_def(JSContext *ctx, JSAtom name)
{
    JSModuleDef *m;
    m = js_mallocz(ctx, sizeof(*m));
    if (!m) {
        JS_FreeAtom(ctx, name);
        return NULL;
    }
    m->header.ref_count = 1;
    m->module_name = name;
    m->module_ns = JS_UNDEFINED;
    m->func_obj = JS_UNDEFINED;
    m->eval_exception = JS_UNDEFINED;
    m->meta_obj = JS_UNDEFINED;
    list_add_tail(&m->link, &ctx->loaded_modules);
    return m;
}

static void js_mark_module_def(JSRuntime *rt, JSModuleDef *m,
                               JS_MarkFunc *mark_func)
{
    int i;

    for(i = 0; i < m->export_entries_count; i++) {
        JSExportEntry *me = &m->export_entries[i];
        if (me->export_type == JS_EXPORT_TYPE_LOCAL &&
            me->u.local.var_ref) {
            mark_func(rt, &me->u.local.var_ref->header);
        }
    }

    JS_MarkValue(rt, m->module_ns, mark_func);
    JS_MarkValue(rt, m->func_obj, mark_func);
    JS_MarkValue(rt, m->eval_exception, mark_func);
    JS_MarkValue(rt, m->meta_obj, mark_func);
}

static void js_free_module_def(JSContext *ctx, JSModuleDef *m)
{
    int i;

    JS_FreeAtom(ctx, m->module_name);

    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        JS_FreeAtom(ctx, rme->module_name);
    }
    js_free(ctx, m->req_module_entries);

    for(i = 0; i < m->export_entries_count; i++) {
        JSExportEntry *me = &m->export_entries[i];
        if (me->export_type == JS_EXPORT_TYPE_LOCAL)
            free_var_ref(ctx->rt, me->u.local.var_ref);
        JS_FreeAtom(ctx, me->export_name);
        JS_FreeAtom(ctx, me->local_name);
    }
    js_free(ctx, m->export_entries);

    js_free(ctx, m->star_export_entries);

    for(i = 0; i < m->import_entries_count; i++) {
        JSImportEntry *mi = &m->import_entries[i];
        JS_FreeAtom(ctx, mi->import_name);
    }
    js_free(ctx, m->import_entries);

    JS_FreeValue(ctx, m->module_ns);
    JS_FreeValue(ctx, m->func_obj);
    JS_FreeValue(ctx, m->eval_exception);
    JS_FreeValue(ctx, m->meta_obj);
    list_del(&m->link);
    js_free(ctx, m);
}

static int add_req_module_entry(JSContext *ctx, JSModuleDef *m,
                                JSAtom module_name)
{
    JSReqModuleEntry *rme;
    int i;

    /* no need to add the module request if it is already present */
    for(i = 0; i < m->req_module_entries_count; i++) {
        rme = &m->req_module_entries[i];
        if (rme->module_name == module_name)
            return i;
    }

    if (js_resize_array(ctx, (void **)&m->req_module_entries,
                        sizeof(JSReqModuleEntry),
                        &m->req_module_entries_size,
                        m->req_module_entries_count + 1))
        return -1;
    rme = &m->req_module_entries[m->req_module_entries_count++];
    rme->module_name = JS_DupAtom(ctx, module_name);
    rme->module = NULL;
    return i;
}

static JSExportEntry *find_export_entry(JSContext *ctx, JSModuleDef *m,
                                        JSAtom export_name)
{
    JSExportEntry *me;
    int i;
    for(i = 0; i < m->export_entries_count; i++) {
        me = &m->export_entries[i];
        if (me->export_name == export_name)
            return me;
    }
    return NULL;
}

static JSExportEntry *add_export_entry2(JSContext *ctx,
                                        JSParseState *s, JSModuleDef *m,
                                        JSAtom local_name, JSAtom export_name,
                                        JSExportTypeEnum export_type)
{
    JSExportEntry *me;

    if (find_export_entry(ctx, m, export_name)) {
        char buf1[ATOM_GET_STR_BUF_SIZE];
        if (s) {
            js_parse_error(s, "duplicate exported name '%s'",
                           JS_AtomGetStr(ctx, buf1, sizeof(buf1), export_name));
        } else {
            JS_ThrowSyntaxErrorAtom(ctx, "duplicate exported name '%s'", export_name);
        }
        return NULL;
    }

    if (js_resize_array(ctx, (void **)&m->export_entries,
                        sizeof(JSExportEntry),
                        &m->export_entries_size,
                        m->export_entries_count + 1))
        return NULL;
    me = &m->export_entries[m->export_entries_count++];
    memset(me, 0, sizeof(*me));
    me->local_name = JS_DupAtom(ctx, local_name);
    me->export_name = JS_DupAtom(ctx, export_name);
    me->export_type = export_type;
    return me;
}

static JSExportEntry *add_export_entry(JSParseState *s, JSModuleDef *m,
                                       JSAtom local_name, JSAtom export_name,
                                       JSExportTypeEnum export_type)
{
    return add_export_entry2(s->ctx, s, m, local_name, export_name,
                             export_type);
}

static int add_star_export_entry(JSContext *ctx, JSModuleDef *m,
                                 int req_module_idx)
{
    JSStarExportEntry *se;

    if (js_resize_array(ctx, (void **)&m->star_export_entries,
                        sizeof(JSStarExportEntry),
                        &m->star_export_entries_size,
                        m->star_export_entries_count + 1))
        return -1;
    se = &m->star_export_entries[m->star_export_entries_count++];
    se->req_module_idx = req_module_idx;
    return 0;
}

/* create a C module */
JSModuleDef *JS_NewCModule(JSContext *ctx, const char *name_str,
                           JSModuleInitFunc *func)
{
    JSModuleDef *m;
    JSAtom name;
    name = JS_NewAtom(ctx, name_str);
    if (name == JS_ATOM_NULL)
        return NULL;
    m = js_new_module_def(ctx, name);
    m->init_func = func;
    return m;
}

int JS_AddModuleExport(JSContext *ctx, JSModuleDef *m, const char *export_name)
{
    JSExportEntry *me;
    JSAtom name;
    name = JS_NewAtom(ctx, export_name);
    if (name == JS_ATOM_NULL)
        return -1;
    me = add_export_entry2(ctx, NULL, m, JS_ATOM_NULL, name,
                           JS_EXPORT_TYPE_LOCAL);
    JS_FreeAtom(ctx, name);
    if (!me)
        return -1;
    else
        return 0;
}

int JS_SetModuleExport(JSContext *ctx, JSModuleDef *m, const char *export_name,
                       JSValue val)
{
    JSExportEntry *me;
    JSAtom name;
    name = JS_NewAtom(ctx, export_name);
    if (name == JS_ATOM_NULL)
        goto fail;
    me = find_export_entry(ctx, m, name);
    JS_FreeAtom(ctx, name);
    if (!me)
        goto fail;
    set_value(ctx, me->u.local.var_ref->pvalue, val);
    return 0;
    fail:
    JS_FreeValue(ctx, val);
    return -1;
}

void JS_SetModuleLoaderFunc(JSRuntime *rt,
                            JSModuleNormalizeFunc *module_normalize,
                            JSModuleLoaderFunc *module_loader, void *opaque)
{
    rt->module_normalize_func = module_normalize;
    rt->module_loader_func = module_loader;
    rt->module_loader_opaque = opaque;
}

/* default module filename normalizer */
static char *js_default_module_normalize_name(JSContext *ctx,
                                              const char *base_name,
                                              const char *name)
{
    char *filename, *p;
    const char *r;
    int len;

    if (name[0] != '.') {
        /* if no initial dot, the module name is not modified */
        return js_strdup(ctx, name);
    }

    p = strrchr(base_name, '/');
    if (p)
        len = p - base_name;
    else
        len = 0;

    filename = js_malloc(ctx, len + strlen(name) + 1 + 1);
    if (!filename)
        return NULL;
    memcpy(filename, base_name, len);
    filename[len] = '\0';

    /* we only normalize the leading '..' or '.' */
    r = name;
    for(;;) {
        if (r[0] == '.' && r[1] == '/') {
            r += 2;
        } else if (r[0] == '.' && r[1] == '.' && r[2] == '/') {
            /* remove the last path element of filename, except if "."
               or ".." */
            if (filename[0] == '\0')
                break;
            p = strrchr(filename, '/');
            if (!p)
                p = filename;
            else
                p++;
            if (!strcmp(p, ".") || !strcmp(p, ".."))
                break;
            if (p > filename)
                p--;
            *p = '\0';
            r += 3;
        } else {
            break;
        }
    }
    if (filename[0] != '\0')
        strcat(filename, "/");
    strcat(filename, r);
    //    printf("normalize: %s %s -> %s\n", base_name, name, filename);
    return filename;
}

static JSModuleDef *js_find_loaded_module(JSContext *ctx, JSAtom name)
{
    struct list_head *el;
    JSModuleDef *m;

    /* first look at the loaded modules */
    list_for_each(el, &ctx->loaded_modules) {
        m = list_entry(el, JSModuleDef, link);
        if (m->module_name == name)
            return m;
    }
    return NULL;
}

/* return NULL in case of exception (e.g. module could not be loaded) */
static JSModuleDef *js_host_resolve_imported_module(JSContext *ctx,
                                                    JSAtom base_module_name,
                                                    JSAtom module_name1)
{
    JSRuntime *rt = ctx->rt;
    JSModuleDef *m;
    char *cname;
    const char *base_cname, *cname1;
    JSAtom module_name;

    base_cname = JS_AtomToCString(ctx, base_module_name);
    if (!base_cname)
        return NULL;
    cname1 = JS_AtomToCString(ctx, module_name1);
    if (!cname1) {
        JS_FreeCString(ctx, base_cname);
        return NULL;
    }

    if (!rt->module_normalize_func) {
        cname = js_default_module_normalize_name(ctx, base_cname, cname1);
    } else {
        cname = rt->module_normalize_func(ctx, base_cname, cname1,
                                          rt->module_loader_opaque);
    }
    JS_FreeCString(ctx, base_cname);
    JS_FreeCString(ctx, cname1);
    if (!cname)
        return NULL;

    module_name = JS_NewAtom(ctx, cname);
    if (module_name == JS_ATOM_NULL) {
        js_free(ctx, cname);
        return NULL;
    }

    /* first look at the loaded modules */
    m = js_find_loaded_module(ctx, module_name);
    if (m) {
        js_free(ctx, cname);
        JS_FreeAtom(ctx, module_name);
        return m;
    }

    JS_FreeAtom(ctx, module_name);

    /* load the module */
    if (!rt->module_loader_func) {
        /* XXX: use a syntax error ? */
        JS_ThrowReferenceError(ctx, "could not load module '%s'",
                               cname);
        js_free(ctx, cname);
        return NULL;
    }

    m = rt->module_loader_func(ctx, cname, rt->module_loader_opaque);
    js_free(ctx, cname);
    return m;
}

typedef struct JSResolveEntry {
    JSModuleDef *module;
    JSAtom name;
} JSResolveEntry;

typedef struct JSResolveState {
    JSResolveEntry *array;
    int size;
    int count;
} JSResolveState;

static int find_resolve_entry(JSResolveState *s,
                              JSModuleDef *m, JSAtom name)
{
    int i;
    for(i = 0; i < s->count; i++) {
        JSResolveEntry *re = &s->array[i];
        if (re->module == m && re->name == name)
            return i;
    }
    return -1;
}

static int add_resolve_entry(JSContext *ctx, JSResolveState *s,
                             JSModuleDef *m, JSAtom name)
{
    JSResolveEntry *re;

    if (js_resize_array(ctx, (void **)&s->array,
                        sizeof(JSResolveEntry),
                        &s->size, s->count + 1))
        return -1;
    re = &s->array[s->count++];
    re->module = m;
    re->name = JS_DupAtom(ctx, name);
    return 0;
}

typedef enum JSResolveResultEnum {
    JS_RESOLVE_RES_EXCEPTION = -1, /* memory alloc error */
    JS_RESOLVE_RES_FOUND = 0,
    JS_RESOLVE_RES_NOT_FOUND,
    JS_RESOLVE_RES_CIRCULAR,
    JS_RESOLVE_RES_AMBIGUOUS,
} JSResolveResultEnum;

static JSResolveResultEnum js_resolve_export1(JSContext *ctx,
                                              JSModuleDef **pmodule,
                                              JSExportEntry **pme,
                                              JSModuleDef *m,
                                              JSAtom export_name,
                                              JSResolveState *s)
{
    JSExportEntry *me;

    *pmodule = NULL;
    *pme = NULL;
    if (find_resolve_entry(s, m, export_name) >= 0)
        return JS_RESOLVE_RES_CIRCULAR;
    if (add_resolve_entry(ctx, s, m, export_name) < 0)
        return JS_RESOLVE_RES_EXCEPTION;
    me = find_export_entry(ctx, m, export_name);
    if (me) {
        if (me->export_type == JS_EXPORT_TYPE_LOCAL) {
            /* local export */
            *pmodule = m;
            *pme = me;
            return JS_RESOLVE_RES_FOUND;
        } else {
            /* indirect export */
            JSModuleDef *m1;
            m1 = m->req_module_entries[me->u.req_module_idx].module;
            if (me->local_name == JS_ATOM__star_) {
                /* export ns from */
                *pmodule = m;
                *pme = me;
                return JS_RESOLVE_RES_FOUND;
            } else {
                return js_resolve_export1(ctx, pmodule, pme, m1,
                                          me->local_name, s);
            }
        }
    } else {
        if (export_name != JS_ATOM_default) {
            /* not found in direct or indirect exports: try star exports */
            int i;

            for(i = 0; i < m->star_export_entries_count; i++) {
                JSStarExportEntry *se = &m->star_export_entries[i];
                JSModuleDef *m1, *res_m;
                JSExportEntry *res_me;
                JSResolveResultEnum ret;

                m1 = m->req_module_entries[se->req_module_idx].module;
                ret = js_resolve_export1(ctx, &res_m, &res_me, m1,
                                         export_name, s);
                if (ret == JS_RESOLVE_RES_AMBIGUOUS ||
                    ret == JS_RESOLVE_RES_EXCEPTION) {
                    return ret;
                } else if (ret == JS_RESOLVE_RES_FOUND) {
                    if (*pme != NULL) {
                        if (*pmodule != res_m ||
                            res_me->local_name != (*pme)->local_name) {
                            *pmodule = NULL;
                            *pme = NULL;
                            return JS_RESOLVE_RES_AMBIGUOUS;
                        }
                    } else {
                        *pmodule = res_m;
                        *pme = res_me;
                    }
                }
            }
            if (*pme != NULL)
                return JS_RESOLVE_RES_FOUND;
        }
        return JS_RESOLVE_RES_NOT_FOUND;
    }
}

/* If the return value is JS_RESOLVE_RES_FOUND, return the module
  (*pmodule) and the corresponding local export entry
  (*pme). Otherwise return (NULL, NULL) */
static JSResolveResultEnum js_resolve_export(JSContext *ctx,
                                             JSModuleDef **pmodule,
                                             JSExportEntry **pme,
                                             JSModuleDef *m,
                                             JSAtom export_name)
{
    JSResolveState ss, *s = &ss;
    int i;
    JSResolveResultEnum ret;

    s->array = NULL;
    s->size = 0;
    s->count = 0;

    ret = js_resolve_export1(ctx, pmodule, pme, m, export_name, s);

    for(i = 0; i < s->count; i++)
        JS_FreeAtom(ctx, s->array[i].name);
    js_free(ctx, s->array);

    return ret;
}

static void js_resolve_export_throw_error(JSContext *ctx,
                                          JSResolveResultEnum res,
                                          JSModuleDef *m, JSAtom export_name)
{
    char buf1[ATOM_GET_STR_BUF_SIZE];
    char buf2[ATOM_GET_STR_BUF_SIZE];
    switch(res) {
        case JS_RESOLVE_RES_EXCEPTION:
            break;
        default:
        case JS_RESOLVE_RES_NOT_FOUND:
            JS_ThrowSyntaxError(ctx, "Could not find export '%s' in module '%s'",
                                JS_AtomGetStr(ctx, buf1, sizeof(buf1), export_name),
                                JS_AtomGetStr(ctx, buf2, sizeof(buf2), m->module_name));
            break;
        case JS_RESOLVE_RES_CIRCULAR:
            JS_ThrowSyntaxError(ctx, "circular reference when looking for export '%s' in module '%s'",
                                JS_AtomGetStr(ctx, buf1, sizeof(buf1), export_name),
                                JS_AtomGetStr(ctx, buf2, sizeof(buf2), m->module_name));
            break;
        case JS_RESOLVE_RES_AMBIGUOUS:
            JS_ThrowSyntaxError(ctx, "export '%s' in module '%s' is ambiguous",
                                JS_AtomGetStr(ctx, buf1, sizeof(buf1), export_name),
                                JS_AtomGetStr(ctx, buf2, sizeof(buf2), m->module_name));
            break;
    }
}


typedef enum {
    EXPORTED_NAME_AMBIGUOUS,
    EXPORTED_NAME_NORMAL,
    EXPORTED_NAME_NS,
} ExportedNameEntryEnum;

typedef struct ExportedNameEntry {
    JSAtom export_name;
    ExportedNameEntryEnum export_type;
    union {
        JSExportEntry *me; /* using when the list is built */
        JSVarRef *var_ref; /* EXPORTED_NAME_NORMAL */
        JSModuleDef *module; /* for EXPORTED_NAME_NS */
    } u;
} ExportedNameEntry;

typedef struct GetExportNamesState {
    JSModuleDef **modules;
    int modules_size;
    int modules_count;

    ExportedNameEntry *exported_names;
    int exported_names_size;
    int exported_names_count;
} GetExportNamesState;

static int find_exported_name(GetExportNamesState *s, JSAtom name)
{
    int i;
    for(i = 0; i < s->exported_names_count; i++) {
        if (s->exported_names[i].export_name == name)
            return i;
    }
    return -1;
}

static __exception int get_exported_names(JSContext *ctx,
                                          GetExportNamesState *s,
                                          JSModuleDef *m, BOOL from_star)
{
    ExportedNameEntry *en;
    int i, j;

    /* check circular reference */
    for(i = 0; i < s->modules_count; i++) {
        if (s->modules[i] == m)
            return 0;
    }
    if (js_resize_array(ctx, (void **)&s->modules, sizeof(s->modules[0]),
                        &s->modules_size, s->modules_count + 1))
        return -1;
    s->modules[s->modules_count++] = m;

    for(i = 0; i < m->export_entries_count; i++) {
        JSExportEntry *me = &m->export_entries[i];
        if (from_star && me->export_name == JS_ATOM_default)
            continue;
        j = find_exported_name(s, me->export_name);
        if (j < 0) {
            if (js_resize_array(ctx, (void **)&s->exported_names, sizeof(s->exported_names[0]),
                                &s->exported_names_size,
                                s->exported_names_count + 1))
                return -1;
            en = &s->exported_names[s->exported_names_count++];
            en->export_name = me->export_name;
            /* avoid a second lookup for simple module exports */
            if (from_star || me->export_type != JS_EXPORT_TYPE_LOCAL)
                en->u.me = NULL;
            else
                en->u.me = me;
        } else {
            en = &s->exported_names[j];
            en->u.me = NULL;
        }
    }
    for(i = 0; i < m->star_export_entries_count; i++) {
        JSStarExportEntry *se = &m->star_export_entries[i];
        JSModuleDef *m1;
        m1 = m->req_module_entries[se->req_module_idx].module;
        if (get_exported_names(ctx, s, m1, TRUE))
            return -1;
    }
    return 0;
}

/* Unfortunately, the spec gives a different behavior from GetOwnProperty ! */
static int js_module_ns_has(JSContext *ctx, JSValueConst obj, JSAtom atom)
{
    return (find_own_property1(JS_VALUE_GET_OBJ(obj), atom) != NULL);
}

static const JSClassExoticMethods js_module_ns_exotic_methods = {
        .has_property = js_module_ns_has,
};

static int exported_names_cmp(const void *p1, const void *p2, void *opaque)
{
    JSContext *ctx = opaque;
    const ExportedNameEntry *me1 = p1;
    const ExportedNameEntry *me2 = p2;
    JSValue str1, str2;
    int ret;

    /* XXX: should avoid allocation memory in atom comparison */
    str1 = JS_AtomToString(ctx, me1->export_name);
    str2 = JS_AtomToString(ctx, me2->export_name);
    if (JS_IsException(str1) || JS_IsException(str2)) {
        /* XXX: raise an error ? */
        ret = 0;
    } else {
        ret = js_string_compare(ctx, JS_VALUE_GET_STRING(str1),
                                JS_VALUE_GET_STRING(str2));
    }
    JS_FreeValue(ctx, str1);
    JS_FreeValue(ctx, str2);
    return ret;
}

static JSValue js_get_module_ns(JSContext *ctx, JSModuleDef *m);

static int js_module_ns_autoinit(JSContext *ctx, JSObject *p, JSAtom atom,
                                 void *opaque)
{
    JSModuleDef *m = opaque;
    JSValue val, this_val = JS_MKPTR(JS_TAG_OBJECT, p);

    val = js_get_module_ns(ctx, m);
    if (JS_IsException(val))
        return -1;
    if (JS_DefinePropertyValue(ctx, this_val, atom, val,
                               JS_PROP_ENUMERABLE | JS_PROP_WRITABLE) < 0)
        return -1;
    return 0;
}

static JSValue js_build_module_ns(JSContext *ctx, JSModuleDef *m)
{
    JSValue obj;
    JSObject *p;
    GetExportNamesState s_s, *s = &s_s;
    int i, ret;
    JSProperty *pr;

    obj = JS_NewObjectClass(ctx, JS_CLASS_MODULE_NS);
    if (JS_IsException(obj))
        return obj;
    p = JS_VALUE_GET_OBJ(obj);

    memset(s, 0, sizeof(*s));
    ret = get_exported_names(ctx, s, m, FALSE);
    js_free(ctx, s->modules);
    if (ret)
        goto fail;

    /* Resolve the exported names. The ambiguous exports are removed */
    for(i = 0; i < s->exported_names_count; i++) {
        ExportedNameEntry *en = &s->exported_names[i];
        JSResolveResultEnum res;
        JSExportEntry *res_me;
        JSModuleDef *res_m;

        if (en->u.me) {
            res_me = en->u.me; /* fast case: no resolution needed */
            res_m = m;
            res = JS_RESOLVE_RES_FOUND;
        } else {
            res = js_resolve_export(ctx, &res_m, &res_me, m,
                                    en->export_name);
        }
        if (res != JS_RESOLVE_RES_FOUND) {
            if (res != JS_RESOLVE_RES_AMBIGUOUS) {
                js_resolve_export_throw_error(ctx, res, m, en->export_name);
                goto fail;
            }
            en->export_type = EXPORTED_NAME_AMBIGUOUS;
        } else {
            if (res_me->local_name == JS_ATOM__star_) {
                en->export_type = EXPORTED_NAME_NS;
                en->u.module = res_m->req_module_entries[res_me->u.req_module_idx].module;
            } else {
                en->export_type = EXPORTED_NAME_NORMAL;
                if (res_me->u.local.var_ref) {
                    en->u.var_ref = res_me->u.local.var_ref;
                } else {
                    JSObject *p1 = JS_VALUE_GET_OBJ(res_m->func_obj);
                    p1 = JS_VALUE_GET_OBJ(res_m->func_obj);
                    en->u.var_ref = p1->u.func.var_refs[res_me->u.local.var_idx];
                }
            }
        }
    }

    /* sort the exported names */
    rqsort(s->exported_names, s->exported_names_count,
           sizeof(s->exported_names[0]), exported_names_cmp, ctx);

    for(i = 0; i < s->exported_names_count; i++) {
        ExportedNameEntry *en = &s->exported_names[i];
        switch(en->export_type) {
            case EXPORTED_NAME_NORMAL:
            {
                JSVarRef *var_ref = en->u.var_ref;
                pr = add_property(ctx, p, en->export_name,
                                  JS_PROP_ENUMERABLE | JS_PROP_WRITABLE |
                                  JS_PROP_VARREF);
                if (!pr)
                    goto fail;
                var_ref->header.ref_count++;
                pr->u.var_ref = var_ref;
            }
                break;
            case EXPORTED_NAME_NS:
                /* the exported namespace must be created on demand */
                if (JS_DefineAutoInitProperty(ctx, obj,
                                              en->export_name,
                                              JS_AUTOINIT_ID_MODULE_NS,
                                              en->u.module, JS_PROP_ENUMERABLE | JS_PROP_WRITABLE) < 0)
                    goto fail;
                break;
            default:
                break;
        }
    }

    js_free(ctx, s->exported_names);

    JS_DefinePropertyValue(ctx, obj, JS_ATOM_Symbol_toStringTag,
                           JS_AtomToString(ctx, JS_ATOM_Module),
                           0);

    p->extensible = FALSE;
    return obj;
    fail:
    js_free(ctx, s->exported_names);
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

static JSValue js_get_module_ns(JSContext *ctx, JSModuleDef *m)
{
    if (JS_IsUndefined(m->module_ns)) {
        JSValue val;
        val = js_build_module_ns(ctx, m);
        if (JS_IsException(val))
            return JS_EXCEPTION;
        m->module_ns = val;
    }
    return JS_DupValue(ctx, m->module_ns);
}

/* Load all the required modules for module 'm' */
static int js_resolve_module(JSContext *ctx, JSModuleDef *m)
{
    int i;
    JSModuleDef *m1;

    if (m->resolved)
        return 0;
#ifdef DUMP_MODULE_RESOLVE
    {
        char buf1[ATOM_GET_STR_BUF_SIZE];
        printf("resolving module '%s':\n", JS_AtomGetStr(ctx, buf1, sizeof(buf1), m->module_name));
    }
#endif
    m->resolved = TRUE;
    /* resolve each requested module */
    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        m1 = js_host_resolve_imported_module(ctx, m->module_name,
                                             rme->module_name);
        if (!m1)
            return -1;
        rme->module = m1;
        /* already done in js_host_resolve_imported_module() except if
           the module was loaded with JS_EvalBinary() */
        if (js_resolve_module(ctx, m1) < 0)
            return -1;
    }
    return 0;
}

static JSVarRef *js_create_module_var(JSContext *ctx, BOOL is_lexical)
{
    JSVarRef *var_ref;
    var_ref = js_malloc(ctx, sizeof(JSVarRef));
    if (!var_ref)
        return NULL;
    var_ref->header.ref_count = 1;
    if (is_lexical)
        var_ref->value = JS_UNINITIALIZED;
    else
        var_ref->value = JS_UNDEFINED;
    var_ref->pvalue = &var_ref->value;
    var_ref->is_detached = TRUE;
    add_gc_object(ctx->rt, &var_ref->header, JS_GC_OBJ_TYPE_VAR_REF);
    return var_ref;
}

/* Create the <eval> function associated with the module */
static int js_create_module_bytecode_function(JSContext *ctx, JSModuleDef *m)
{
    JSFunctionBytecode *b;
    int i;
    JSVarRef **var_refs;
    JSValue func_obj, bfunc;
    JSObject *p;

    bfunc = m->func_obj;
    func_obj = JS_NewObjectProtoClass(ctx, ctx->function_proto,
                                      JS_CLASS_BYTECODE_FUNCTION);

    if (JS_IsException(func_obj))
        return -1;
    b = JS_VALUE_GET_PTR(bfunc);

    p = JS_VALUE_GET_OBJ(func_obj);
    p->u.func.function_bytecode = b;
    b->header.ref_count++;
    p->u.func.home_object = NULL;
    p->u.func.var_refs = NULL;
    if (b->closure_var_count) {
        var_refs = js_mallocz(ctx, sizeof(var_refs[0]) * b->closure_var_count);
        if (!var_refs)
            goto fail;
        p->u.func.var_refs = var_refs;

        /* create the global variables. The other variables are
           imported from other modules */
        for(i = 0; i < b->closure_var_count; i++) {
            JSClosureVar *cv = &b->closure_var[i];
            JSVarRef *var_ref;
            if (cv->is_local) {
                var_ref = js_create_module_var(ctx, cv->is_lexical);
                if (!var_ref)
                    goto fail;
#ifdef DUMP_MODULE_RESOLVE
                printf("local %d: %p\n", i, var_ref);
#endif
                var_refs[i] = var_ref;
            }
        }
    }
    m->func_obj = func_obj;
    JS_FreeValue(ctx, bfunc);
    return 0;
    fail:
    JS_FreeValue(ctx, func_obj);
    return -1;
}

/* must be done before js_instantiate_module() because of cyclic references */
static int js_create_module_function(JSContext *ctx, JSModuleDef *m)
{
    BOOL is_c_module;
    int i;
    JSVarRef *var_ref;

    if (m->func_created)
        return 0;

    is_c_module = (m->init_func != NULL);

    if (is_c_module) {
        /* initialize the exported variables */
        for(i = 0; i < m->export_entries_count; i++) {
            JSExportEntry *me = &m->export_entries[i];
            if (me->export_type == JS_EXPORT_TYPE_LOCAL) {
                var_ref = js_create_module_var(ctx, FALSE);
                if (!var_ref)
                    return -1;
                me->u.local.var_ref = var_ref;
            }
        }
    } else {
        if (js_create_module_bytecode_function(ctx, m))
            return -1;
    }
    m->func_created = TRUE;

    /* do it on the dependencies */

    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        if (js_create_module_function(ctx, rme->module) < 0)
            return -1;
    }

    return 0;
}


/* Prepare a module to be executed by resolving all the imported
   variables. */
static int js_instantiate_module(JSContext *ctx, JSModuleDef *m)
{
    int i;
    JSImportEntry *mi;
    JSModuleDef *m1;
    JSVarRef **var_refs, *var_ref;
    JSObject *p;
    BOOL is_c_module;

    if (m->instantiated)
        return 0;
    m->instantiated = TRUE;

#ifdef DUMP_MODULE_RESOLVE
    {
        char buf1[ATOM_GET_STR_BUF_SIZE];
        printf("start instantiating module '%s':\n", JS_AtomGetStr(ctx, buf1, sizeof(buf1), m->module_name));
    }
#endif

    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        if (js_instantiate_module(ctx, rme->module) < 0)
            goto fail;
    }

#ifdef DUMP_MODULE_RESOLVE
    {
        char buf1[ATOM_GET_STR_BUF_SIZE];
        printf("instantiating module '%s':\n", JS_AtomGetStr(ctx, buf1, sizeof(buf1), m->module_name));
    }
#endif
    /* check the indirect exports */
    for(i = 0; i < m->export_entries_count; i++) {
        JSExportEntry *me = &m->export_entries[i];
        if (me->export_type == JS_EXPORT_TYPE_INDIRECT &&
            me->local_name != JS_ATOM__star_) {
            JSResolveResultEnum ret;
            JSExportEntry *res_me;
            JSModuleDef *res_m, *m1;
            m1 = m->req_module_entries[me->u.req_module_idx].module;
            ret = js_resolve_export(ctx, &res_m, &res_me, m1, me->local_name);
            if (ret != JS_RESOLVE_RES_FOUND) {
                js_resolve_export_throw_error(ctx, ret, m, me->export_name);
                goto fail;
            }
        }
    }

#ifdef DUMP_MODULE_RESOLVE
    {
        printf("exported bindings:\n");
        for(i = 0; i < m->export_entries_count; i++) {
            JSExportEntry *me = &m->export_entries[i];
            printf(" name="); print_atom(ctx, me->export_name);
            printf(" local="); print_atom(ctx, me->local_name);
            printf(" type=%d idx=%d\n", me->export_type, me->u.local.var_idx);
        }
    }
#endif

    is_c_module = (m->init_func != NULL);

    if (!is_c_module) {
        p = JS_VALUE_GET_OBJ(m->func_obj);
        var_refs = p->u.func.var_refs;

        for(i = 0; i < m->import_entries_count; i++) {
            mi = &m->import_entries[i];
#ifdef DUMP_MODULE_RESOLVE
            printf("import var_idx=%d name=", mi->var_idx);
            print_atom(ctx, mi->import_name);
            printf(": ");
#endif
            m1 = m->req_module_entries[mi->req_module_idx].module;
            if (mi->import_name == JS_ATOM__star_) {
                JSValue val;
                /* name space import */
                val = js_get_module_ns(ctx, m1);
                if (JS_IsException(val))
                    goto fail;
                set_value(ctx, &var_refs[mi->var_idx]->value, val);
#ifdef DUMP_MODULE_RESOLVE
                printf("namespace\n");
#endif
            } else {
                JSResolveResultEnum ret;
                JSExportEntry *res_me;
                JSModuleDef *res_m;
                JSObject *p1;

                ret = js_resolve_export(ctx, &res_m,
                                        &res_me, m1, mi->import_name);
                if (ret != JS_RESOLVE_RES_FOUND) {
                    js_resolve_export_throw_error(ctx, ret, m1, mi->import_name);
                    goto fail;
                }
                if (res_me->local_name == JS_ATOM__star_) {
                    JSValue val;
                    JSModuleDef *m2;
                    /* name space import from */
                    m2 = res_m->req_module_entries[res_me->u.req_module_idx].module;
                    val = js_get_module_ns(ctx, m2);
                    if (JS_IsException(val))
                        goto fail;
                    var_ref = js_create_module_var(ctx, TRUE);
                    if (!var_ref) {
                        JS_FreeValue(ctx, val);
                        goto fail;
                    }
                    set_value(ctx, &var_ref->value, val);
                    var_refs[mi->var_idx] = var_ref;
#ifdef DUMP_MODULE_RESOLVE
                    printf("namespace from\n");
#endif
                } else {
                    var_ref = res_me->u.local.var_ref;
                    if (!var_ref) {
                        p1 = JS_VALUE_GET_OBJ(res_m->func_obj);
                        var_ref = p1->u.func.var_refs[res_me->u.local.var_idx];
                    }
                    var_ref->header.ref_count++;
                    var_refs[mi->var_idx] = var_ref;
#ifdef DUMP_MODULE_RESOLVE
                    printf("local export (var_ref=%p)\n", var_ref);
#endif
                }
            }
        }

        /* keep the exported variables in the module export entries (they
           are used when the eval function is deleted and cannot be
           initialized before in case imports are exported) */
        for(i = 0; i < m->export_entries_count; i++) {
            JSExportEntry *me = &m->export_entries[i];
            if (me->export_type == JS_EXPORT_TYPE_LOCAL) {
                var_ref = var_refs[me->u.local.var_idx];
                var_ref->header.ref_count++;
                me->u.local.var_ref = var_ref;
            }
        }
    }

#ifdef DUMP_MODULE_RESOLVE
    printf("done instantiate\n");
#endif
    return 0;
    fail:
    return -1;
}

/* warning: the returned atom is not allocated */
static JSAtom js_get_script_or_module_name(JSContext *ctx)
{
    JSStackFrame *sf;
    JSFunctionBytecode *b;
    JSObject *p;
    /* XXX: currently we just use the filename of the englobing
       function. It does not work for eval(). Need to add a
       ScriptOrModule info in JSFunctionBytecode */
    sf = ctx->rt->current_stack_frame;
    assert(sf != NULL);
    assert(JS_VALUE_GET_TAG(sf->cur_func) == JS_TAG_OBJECT);
    p = JS_VALUE_GET_OBJ(sf->cur_func);
    assert(js_class_has_bytecode(p->class_id));
    b = p->u.func.function_bytecode;
    if (!b->has_debug)
        return JS_ATOM_NULL;
    return b->debug.filename;
}

JSAtom JS_GetModuleName(JSContext *ctx, JSModuleDef *m)
{
    return JS_DupAtom(ctx, m->module_name);
}

JSValue JS_GetImportMeta(JSContext *ctx, JSModuleDef *m)
{
    JSValue obj;
    /* allocate meta_obj only if requested to save memory */
    obj = m->meta_obj;
    if (JS_IsUndefined(obj)) {
        obj = JS_NewObjectProto(ctx, JS_NULL);
        if (JS_IsException(obj))
            return JS_EXCEPTION;
        m->meta_obj = obj;
    }
    return JS_DupValue(ctx, obj);
}

static JSValue js_import_meta(JSContext *ctx)
{
    JSAtom filename;
    JSModuleDef *m;

    filename = js_get_script_or_module_name(ctx);
    if (filename == JS_ATOM_NULL)
        goto fail;

    /* XXX: inefficient, need to add a module or script pointer in
       JSFunctionBytecode */
    m = js_find_loaded_module(ctx, filename);
    if (!m) {
        fail:
        JS_ThrowTypeError(ctx, "import.meta not supported in this context");
        return JS_EXCEPTION;
    }
    return JS_GetImportMeta(ctx, m);
}

static JSValue js_dynamic_import_job(JSContext *ctx,
                                     int argc, JSValueConst *argv)
{
    JSValueConst *resolving_funcs = argv;
    JSValueConst basename_val = argv[2];
    JSValueConst specifier = argv[3];
    JSModuleDef *m;
    JSAtom basename = JS_ATOM_NULL, filename;
    JSValue specifierString, ret, func_obj, err, ns;

    if (!JS_IsString(basename_val)) {
        JS_ThrowTypeError(ctx, "no function filename for import()");
        goto exception;
    }
    basename = JS_ValueToAtom(ctx, basename_val);
    if (basename == JS_ATOM_NULL)
        goto exception;

    specifierString = JS_ToString(ctx, specifier);
    if (JS_IsException(specifierString))
        goto exception;
    filename = JS_ValueToAtom(ctx, specifierString);
    JS_FreeValue(ctx, specifierString);
    if (filename == JS_ATOM_NULL)
        goto exception;

    m = js_host_resolve_imported_module(ctx, basename, filename);
    JS_FreeAtom(ctx, filename);
    if (!m) {
        goto exception;
    }

    if (js_resolve_module(ctx, m) < 0) {
        js_free_modules(ctx, JS_FREE_MODULE_NOT_RESOLVED);
        goto exception;
    }

    /* Evaluate the module code */
    func_obj = JS_DupValue(ctx, JS_MKPTR(JS_TAG_MODULE, m));
    ret = JS_EvalFunction(ctx, func_obj);
    if (JS_IsException(ret))
        goto exception;
    JS_FreeValue(ctx, ret);

    /* return the module namespace */
    ns = js_get_module_ns(ctx, m);
    if (JS_IsException(ret))
        goto exception;

    ret = JS_Call(ctx, resolving_funcs[0], JS_UNDEFINED,
                  1, (JSValueConst *)&ns);
    JS_FreeValue(ctx, ret); /* XXX: what to do if exception ? */
    JS_FreeValue(ctx, ns);
    JS_FreeAtom(ctx, basename);
    return JS_UNDEFINED;
    exception:

    err = JS_GetException(ctx);
    ret = JS_Call(ctx, resolving_funcs[1], JS_UNDEFINED,
                  1, (JSValueConst *)&err);
    JS_FreeValue(ctx, ret); /* XXX: what to do if exception ? */
    JS_FreeValue(ctx, err);
    JS_FreeAtom(ctx, basename);
    return JS_UNDEFINED;
}

static JSValue js_dynamic_import(JSContext *ctx, JSValueConst specifier)
{
    JSAtom basename;
    JSValue promise, resolving_funcs[2], basename_val;
    JSValueConst args[4];

    basename = js_get_script_or_module_name(ctx);
    if (basename == JS_ATOM_NULL)
        basename_val = JS_NULL;
    else
        basename_val = JS_AtomToValue(ctx, basename);
    if (JS_IsException(basename_val))
        return basename_val;

    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise)) {
        JS_FreeValue(ctx, basename_val);
        return promise;
    }

    args[0] = resolving_funcs[0];
    args[1] = resolving_funcs[1];
    args[2] = basename_val;
    args[3] = specifier;

    JS_EnqueueJob(ctx, js_dynamic_import_job, 4, args);

    JS_FreeValue(ctx, basename_val);
    JS_FreeValue(ctx, resolving_funcs[0]);
    JS_FreeValue(ctx, resolving_funcs[1]);
    return promise;
}

/* Run the <eval> function of the module and of all its requested
   modules. */
static JSValue js_evaluate_module(JSContext *ctx, JSModuleDef *m)
{
    JSModuleDef *m1;
    int i;
    JSValue ret_val;

    if (m->eval_mark)
        return JS_UNDEFINED; /* avoid cycles */

    if (m->evaluated) {
        /* if the module was already evaluated, rethrow the exception
           it raised */
        if (m->eval_has_exception) {
            return JS_Throw(ctx, JS_DupValue(ctx, m->eval_exception));
        } else {
            return JS_UNDEFINED;
        }
    }

    m->eval_mark = TRUE;

    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        m1 = rme->module;
        if (!m1->eval_mark) {
            ret_val = js_evaluate_module(ctx, m1);
            if (JS_IsException(ret_val)) {
                m->eval_mark = FALSE;
                return ret_val;
            }
            JS_FreeValue(ctx, ret_val);
        }
    }

    if (m->init_func) {
        /* C module init */
        if (m->init_func(ctx, m) < 0)
            ret_val = JS_EXCEPTION;
        else
            ret_val = JS_UNDEFINED;
    } else {
        ret_val = JS_CallFree(ctx, m->func_obj, JS_UNDEFINED, 0, NULL);
        m->func_obj = JS_UNDEFINED;
    }
    if (JS_IsException(ret_val)) {
        /* save the thrown exception value */
        m->eval_has_exception = TRUE;
        m->eval_exception = JS_DupValue(ctx, ctx->rt->current_exception);
    }
    m->eval_mark = FALSE;
    m->evaluated = TRUE;
    return ret_val;
}

static __exception JSAtom js_parse_from_clause(JSParseState *s)
{
JSAtom module_name;
if (!token_is_pseudo_keyword(s, JS_ATOM_from)) {
js_parse_error(s, "from clause expected");
return JS_ATOM_NULL;
}
if (next_token(s))
return JS_ATOM_NULL;
if (s->token.val != TOK_STRING) {
js_parse_error(s, "string expected");
return JS_ATOM_NULL;
}
module_name = JS_ValueToAtom(s->ctx, s->token.u.str.str);
if (module_name == JS_ATOM_NULL)
return JS_ATOM_NULL;
if (next_token(s)) {
JS_FreeAtom(s->ctx, module_name);
return JS_ATOM_NULL;
}
return module_name;
}

static __exception int js_parse_export(JSParseState *s)
{
    JSContext *ctx = s->ctx;
    JSModuleDef *m = s->cur_func->module;
    JSAtom local_name, export_name;
    int first_export, idx, i, tok;
    JSAtom module_name;
    JSExportEntry *me;

    if (next_token(s))
        return -1;

    tok = s->token.val;
    if (tok == TOK_CLASS) {
        return js_parse_class(s, FALSE, JS_PARSE_EXPORT_NAMED);
    } else if (tok == TOK_FUNCTION ||
               (token_is_pseudo_keyword(s, JS_ATOM_async) &&
                peek_token(s, TRUE) == TOK_FUNCTION)) {
        return js_parse_function_decl2(s, JS_PARSE_FUNC_STATEMENT,
                                       JS_FUNC_NORMAL, JS_ATOM_NULL,
                                       s->token.ptr, s->token.line_num,
                                       JS_PARSE_EXPORT_NAMED, NULL);
    }

    if (next_token(s))
        return -1;

    switch(tok) {
        case '{':
            first_export = m->export_entries_count;
            while (s->token.val != '}') {
                if (!token_is_ident(s->token.val)) {
                    js_parse_error(s, "identifier expected");
                    return -1;
                }
                local_name = JS_DupAtom(ctx, s->token.u.ident.atom);
                export_name = JS_ATOM_NULL;
                if (next_token(s))
                    goto fail;
                if (token_is_pseudo_keyword(s, JS_ATOM_as)) {
                    if (next_token(s))
                        goto fail;
                    if (!token_is_ident(s->token.val)) {
                        js_parse_error(s, "identifier expected");
                        goto fail;
                    }
                    export_name = JS_DupAtom(ctx, s->token.u.ident.atom);
                    if (next_token(s)) {
                        fail:
                        JS_FreeAtom(ctx, local_name);
                        fail1:
                        JS_FreeAtom(ctx, export_name);
                        return -1;
                    }
                } else {
                    export_name = JS_DupAtom(ctx, local_name);
                }
                me = add_export_entry(s, m, local_name, export_name,
                                      JS_EXPORT_TYPE_LOCAL);
                JS_FreeAtom(ctx, local_name);
                JS_FreeAtom(ctx, export_name);
                if (!me)
                    return -1;
                if (s->token.val != ',')
                    break;
                if (next_token(s))
                    return -1;
            }
            if (js_parse_expect(s, '}'))
                return -1;
            if (token_is_pseudo_keyword(s, JS_ATOM_from)) {
                module_name = js_parse_from_clause(s);
                if (module_name == JS_ATOM_NULL)
                    return -1;
                idx = add_req_module_entry(ctx, m, module_name);
                JS_FreeAtom(ctx, module_name);
                if (idx < 0)
                    return -1;
                for(i = first_export; i < m->export_entries_count; i++) {
                    me = &m->export_entries[i];
                    me->export_type = JS_EXPORT_TYPE_INDIRECT;
                    me->u.req_module_idx = idx;
                }
            }
            break;
        case '*':
            if (token_is_pseudo_keyword(s, JS_ATOM_as)) {
                /* export ns from */
                if (next_token(s))
                    return -1;
                if (!token_is_ident(s->token.val)) {
                    js_parse_error(s, "identifier expected");
                    return -1;
                }
                export_name = JS_DupAtom(ctx, s->token.u.ident.atom);
                if (next_token(s))
                    goto fail1;
                module_name = js_parse_from_clause(s);
                if (module_name == JS_ATOM_NULL)
                    goto fail1;
                idx = add_req_module_entry(ctx, m, module_name);
                JS_FreeAtom(ctx, module_name);
                if (idx < 0)
                    goto fail1;
                me = add_export_entry(s, m, JS_ATOM__star_, export_name,
                                      JS_EXPORT_TYPE_INDIRECT);
                JS_FreeAtom(ctx, export_name);
                if (!me)
                    return -1;
                me->u.req_module_idx = idx;
            } else {
                module_name = js_parse_from_clause(s);
                if (module_name == JS_ATOM_NULL)
                    return -1;
                idx = add_req_module_entry(ctx, m, module_name);
                JS_FreeAtom(ctx, module_name);
                if (idx < 0)
                    return -1;
                if (add_star_export_entry(ctx, m, idx) < 0)
                    return -1;
            }
            break;
        case TOK_DEFAULT:
            if (s->token.val == TOK_CLASS) {
                return js_parse_class(s, FALSE, JS_PARSE_EXPORT_DEFAULT);
            } else if (s->token.val == TOK_FUNCTION ||
                       (token_is_pseudo_keyword(s, JS_ATOM_async) &&
                        peek_token(s, TRUE) == TOK_FUNCTION)) {
                return js_parse_function_decl2(s, JS_PARSE_FUNC_STATEMENT,
                                               JS_FUNC_NORMAL, JS_ATOM_NULL,
                                               s->token.ptr, s->token.line_num,
                                               JS_PARSE_EXPORT_DEFAULT, NULL);
            } else {
                if (js_parse_assign_expr(s, TRUE))
                    return -1;
            }
            /* set the name of anonymous functions */
            set_object_name(s, JS_ATOM_default);

            /* store the value in the _default_ global variable and export
           it */
            local_name = JS_ATOM__default_;
            if (define_var(s, s->cur_func, local_name, JS_VAR_DEF_LET) < 0)
                return -1;
            emit_op(s, OP_scope_put_var_init);
            emit_atom(s, local_name);
            emit_u16(s, 0);

            if (!add_export_entry(s, m, local_name, JS_ATOM_default,
                                  JS_EXPORT_TYPE_LOCAL))
                return -1;
            break;
        case TOK_VAR:
        case TOK_LET:
        case TOK_CONST:
            return js_parse_var(s, TRUE, tok, TRUE);
        default:
            return js_parse_error(s, "invalid export syntax");
    }
    return js_parse_expect_semi(s);
}

static int add_closure_var(JSContext *ctx, JSFunctionDef *s,
                           BOOL is_local, BOOL is_arg,
                           int var_idx, JSAtom var_name,
                           BOOL is_const, BOOL is_lexical,
                           JSVarKindEnum var_kind);

static int add_import(JSParseState *s, JSModuleDef *m,
                      JSAtom local_name, JSAtom import_name)
{
    JSContext *ctx = s->ctx;
    int i, var_idx;
    JSImportEntry *mi;
    BOOL is_local;

    if (local_name == JS_ATOM_arguments || local_name == JS_ATOM_eval)
        return js_parse_error(s, "invalid import binding");

    if (local_name != JS_ATOM_default) {
        for (i = 0; i < s->cur_func->closure_var_count; i++) {
            if (s->cur_func->closure_var[i].var_name == local_name)
                return js_parse_error(s, "duplicate import binding");
        }
    }

    is_local = (import_name == JS_ATOM__star_);
    var_idx = add_closure_var(ctx, s->cur_func, is_local, FALSE,
                              m->import_entries_count,
                              local_name, TRUE, TRUE, FALSE);
    if (var_idx < 0)
        return -1;
    if (js_resize_array(ctx, (void **)&m->import_entries,
                        sizeof(JSImportEntry),
                        &m->import_entries_size,
                        m->import_entries_count + 1))
        return -1;
    mi = &m->import_entries[m->import_entries_count++];
    mi->import_name = JS_DupAtom(ctx, import_name);
    mi->var_idx = var_idx;
    return 0;
}

static __exception int js_parse_import(JSParseState *s)
{
    JSContext *ctx = s->ctx;
    JSModuleDef *m = s->cur_func->module;
    JSAtom local_name, import_name, module_name;
    int first_import, i, idx;

    if (next_token(s))
        return -1;

    first_import = m->import_entries_count;
    if (s->token.val == TOK_STRING) {
        module_name = JS_ValueToAtom(ctx, s->token.u.str.str);
        if (module_name == JS_ATOM_NULL)
            return -1;
        if (next_token(s)) {
            JS_FreeAtom(ctx, module_name);
            return -1;
        }
    } else {
        if (s->token.val == TOK_IDENT) {
            if (s->token.u.ident.is_reserved) {
                return js_parse_error_reserved_identifier(s);
            }
            /* "default" import */
            local_name = JS_DupAtom(ctx, s->token.u.ident.atom);
            import_name = JS_ATOM_default;
            if (next_token(s))
                goto fail;
            if (add_import(s, m, local_name, import_name))
                goto fail;
            JS_FreeAtom(ctx, local_name);

            if (s->token.val != ',')
                goto end_import_clause;
            if (next_token(s))
                return -1;
        }

        if (s->token.val == '*') {
            /* name space import */
            if (next_token(s))
                return -1;
            if (!token_is_pseudo_keyword(s, JS_ATOM_as))
                return js_parse_error(s, "expecting 'as'");
            if (next_token(s))
                return -1;
            if (!token_is_ident(s->token.val)) {
                js_parse_error(s, "identifier expected");
                return -1;
            }
            local_name = JS_DupAtom(ctx, s->token.u.ident.atom);
            import_name = JS_ATOM__star_;
            if (next_token(s))
                goto fail;
            if (add_import(s, m, local_name, import_name))
                goto fail;
            JS_FreeAtom(ctx, local_name);
        } else if (s->token.val == '{') {
            if (next_token(s))
                return -1;

            while (s->token.val != '}') {
                if (!token_is_ident(s->token.val)) {
                    js_parse_error(s, "identifier expected");
                    return -1;
                }
                import_name = JS_DupAtom(ctx, s->token.u.ident.atom);
                local_name = JS_ATOM_NULL;
                if (next_token(s))
                    goto fail;
                if (token_is_pseudo_keyword(s, JS_ATOM_as)) {
                    if (next_token(s))
                        goto fail;
                    if (!token_is_ident(s->token.val)) {
                        js_parse_error(s, "identifier expected");
                        goto fail;
                    }
                    local_name = JS_DupAtom(ctx, s->token.u.ident.atom);
                    if (next_token(s)) {
                        fail:
                        JS_FreeAtom(ctx, local_name);
                        JS_FreeAtom(ctx, import_name);
                        return -1;
                    }
                } else {
                    local_name = JS_DupAtom(ctx, import_name);
                }
                if (add_import(s, m, local_name, import_name))
                    goto fail;
                JS_FreeAtom(ctx, local_name);
                JS_FreeAtom(ctx, import_name);
                if (s->token.val != ',')
                    break;
                if (next_token(s))
                    return -1;
            }
            if (js_parse_expect(s, '}'))
                return -1;
        }
        end_import_clause:
        module_name = js_parse_from_clause(s);
        if (module_name == JS_ATOM_NULL)
            return -1;
    }
    idx = add_req_module_entry(ctx, m, module_name);
    JS_FreeAtom(ctx, module_name);
    if (idx < 0)
        return -1;
    for(i = first_import; i < m->import_entries_count; i++)
        m->import_entries[i].req_module_idx = idx;

    return js_parse_expect_semi(s);
}

static __exception int js_parse_source_element(JSParseState *s)
{
    JSFunctionDef *fd = s->cur_func;
    int tok;

    if (s->token.val == TOK_FUNCTION ||
        (token_is_pseudo_keyword(s, JS_ATOM_async) &&
         peek_token(s, TRUE) == TOK_FUNCTION)) {
        if (js_parse_function_decl(s, JS_PARSE_FUNC_STATEMENT,
                                   JS_FUNC_NORMAL, JS_ATOM_NULL,
                                   s->token.ptr, s->token.line_num))
            return -1;
    } else if (s->token.val == TOK_EXPORT && fd->module) {
        if (js_parse_export(s))
            return -1;
    } else if (s->token.val == TOK_IMPORT && fd->module &&
               ((tok = peek_token(s, FALSE)) != '(' && tok != '.'))  {
        /* the peek_token is needed to avoid confusion with ImportCall
           (dynamic import) or import.meta */
        if (js_parse_import(s))
            return -1;
    } else {
        if (js_parse_statement_or_decl(s, DECL_MASK_ALL))
            return -1;
    }
    return 0;
}

static JSFunctionDef *js_new_function_def(JSContext *ctx,
                                          JSFunctionDef *parent,
                                          BOOL is_eval,
                                          BOOL is_func_expr,
                                          const char *filename, int line_num)
{
    JSFunctionDef *fd;

    fd = js_mallocz(ctx, sizeof(*fd));
    if (!fd)
        return NULL;

    fd->ctx = ctx;
    init_list_head(&fd->child_list);

    /* insert in parent list */
    fd->parent = parent;
    fd->parent_cpool_idx = -1;
    if (parent) {
        list_add_tail(&fd->link, &parent->child_list);
        fd->js_mode = parent->js_mode;
        fd->parent_scope_level = parent->scope_level;
    }

    fd->is_eval = is_eval;
    fd->is_func_expr = is_func_expr;
    js_dbuf_init(ctx, &fd->byte_code);
    fd->last_opcode_pos = -1;
    fd->func_name = JS_ATOM_NULL;
    fd->var_object_idx = -1;
    fd->arguments_var_idx = -1;
    fd->func_var_idx = -1;
    fd->eval_ret_idx = -1;
    fd->this_var_idx = -1;
    fd->new_target_var_idx = -1;
    fd->this_active_func_var_idx = -1;
    fd->home_object_var_idx = -1;

    /* XXX: should distinguish arg, var and var object and body scopes */
    fd->scope_level = 0;  /* 0: var/arg scope, 1:body scope */
    fd->scope_first = -1;
    fd->scopes = fd->def_scope_array;
    fd->scope_size = countof(fd->def_scope_array);
    fd->scope_count = 1;
    fd->scopes[0].first = -1;
    fd->scopes[0].parent = -1;

    fd->filename = JS_NewAtom(ctx, filename);
    fd->line_num = line_num;

    js_dbuf_init(ctx, &fd->pc2line);
    //fd->pc2line_last_line_num = line_num;
    //fd->pc2line_last_pc = 0;
    fd->last_opcode_line_num = line_num;

    return fd;
}

static void free_bytecode_atoms(JSRuntime *rt,
                                const uint8_t *bc_buf, int bc_len,
                                BOOL use_short_opcodes)
{
    int pos, len, op;
    JSAtom atom;
    const JSOpCode *oi;

    pos = 0;
    while (pos < bc_len) {
        op = bc_buf[pos];
        if (use_short_opcodes)
            oi = &short_opcode_info(op);
        else
            oi = &opcode_info[op];

        len = oi->size;
        switch(oi->fmt) {
            case OP_FMT_atom:
            case OP_FMT_atom_u8:
            case OP_FMT_atom_u16:
            case OP_FMT_atom_label_u8:
            case OP_FMT_atom_label_u16:
                atom = get_u32(bc_buf + pos + 1);
                JS_FreeAtomRT(rt, atom);
                break;
            default:
                break;
        }
        pos += len;
    }
}

static void js_free_function_def(JSContext *ctx, JSFunctionDef *fd)
{
    int i;
    struct list_head *el, *el1;

    /* free the child functions */
    list_for_each_safe(el, el1, &fd->child_list) {
        JSFunctionDef *fd1;
        fd1 = list_entry(el, JSFunctionDef, link);
        js_free_function_def(ctx, fd1);
    }

    free_bytecode_atoms(ctx->rt, fd->byte_code.buf, fd->byte_code.size,
                        fd->use_short_opcodes);
    dbuf_free(&fd->byte_code);
    js_free(ctx, fd->jump_slots);
    js_free(ctx, fd->label_slots);
    js_free(ctx, fd->line_number_slots);

    for(i = 0; i < fd->cpool_count; i++) {
        JS_FreeValue(ctx, fd->cpool[i]);
    }
    js_free(ctx, fd->cpool);

    JS_FreeAtom(ctx, fd->func_name);

    for(i = 0; i < fd->var_count; i++) {
        JS_FreeAtom(ctx, fd->vars[i].var_name);
    }
    js_free(ctx, fd->vars);
    for(i = 0; i < fd->arg_count; i++) {
        JS_FreeAtom(ctx, fd->args[i].var_name);
    }
    js_free(ctx, fd->args);

    for(i = 0; i < fd->hoisted_def_count; i++) {
        JS_FreeAtom(ctx, fd->hoisted_def[i].var_name);
    }
    js_free(ctx, fd->hoisted_def);

    for(i = 0; i < fd->closure_var_count; i++) {
        JSClosureVar *cv = &fd->closure_var[i];
        JS_FreeAtom(ctx, cv->var_name);
    }
    js_free(ctx, fd->closure_var);

    if (fd->scopes != fd->def_scope_array)
        js_free(ctx, fd->scopes);

    JS_FreeAtom(ctx, fd->filename);
    dbuf_free(&fd->pc2line);

    js_free(ctx, fd->source);

    if (fd->parent) {
        /* remove in parent list */
        list_del(&fd->link);
    }
    js_free(ctx, fd);
}

#ifdef DUMP_BYTECODE
static const char *skip_lines(const char *p, int n) {
    while (n-- > 0 && *p) {
        while (*p && *p++ != '\n')
            continue;
    }
    return p;
}

static void print_lines(const char *source, int line, int line1) {
    const char *s = source;
    const char *p = skip_lines(s, line);
    if (*p) {
        while (line++ < line1) {
            p = skip_lines(s = p, 1);
            printf(";; %.*s", (int)(p - s), s);
            if (!*p) {
                if (p[-1] != '\n')
                    printf("\n");
                break;
            }
        }
    }
}

static void dump_byte_code(JSContext *ctx, int pass,
                           const uint8_t *tab, int len,
                           const JSVarDef *args, int arg_count,
                           const JSVarDef *vars, int var_count,
                           const JSClosureVar *closure_var, int closure_var_count,
                           const JSValue *cpool, uint32_t cpool_count,
                           const char *source, int line_num,
                           const LabelSlot *label_slots, JSFunctionBytecode *b)
{
    const JSOpCode *oi;
    int pos, pos_next, op, size, idx, addr, line, line1, in_source;
    uint8_t *bits = js_mallocz(ctx, len * sizeof(*bits));
    BOOL use_short_opcodes = (b != NULL);

    /* scan for jump targets */
    for (pos = 0; pos < len; pos = pos_next) {
        op = tab[pos];
        if (use_short_opcodes)
            oi = &short_opcode_info(op);
        else
            oi = &opcode_info[op];
        pos_next = pos + oi->size;
        if (op < OP_COUNT) {
            switch (oi->fmt) {
#if SHORT_OPCODES
            case OP_FMT_label8:
                pos++;
                addr = (int8_t)tab[pos];
                goto has_addr;
            case OP_FMT_label16:
                pos++;
                addr = (int16_t)get_u16(tab + pos);
                goto has_addr;
#endif
            case OP_FMT_atom_label_u8:
            case OP_FMT_atom_label_u16:
                pos += 4;
                /* fall thru */
            case OP_FMT_label:
            case OP_FMT_label_u16:
                pos++;
                addr = get_u32(tab + pos);
                goto has_addr;
            has_addr:
                if (pass == 1)
                    addr = label_slots[addr].pos;
                if (pass == 2)
                    addr = label_slots[addr].pos2;
                if (pass == 3)
                    addr += pos;
                if (addr >= 0 && addr < len)
                    bits[addr] |= 1;
                break;
            }
        }
    }
    in_source = 0;
    if (source) {
        /* Always print first line: needed if single line */
        print_lines(source, 0, 1);
        in_source = 1;
    }
    line1 = line = 1;
    pos = 0;
    while (pos < len) {
        op = tab[pos];
        if (source) {
            if (b) {
                line1 = find_line_num(ctx, b, pos) - line_num + 1;
            } else if (op == OP_line_num) {
                line1 = get_u32(tab + pos + 1) - line_num + 1;
            }
            if (line1 > line) {
                if (!in_source)
                    printf("\n");
                in_source = 1;
                print_lines(source, line, line1);
                line = line1;
                //bits[pos] |= 2;
            }
        }
        if (in_source)
            printf("\n");
        in_source = 0;
        if (op >= OP_COUNT) {
            printf("invalid opcode (0x%02x)\n", op);
            pos++;
            continue;
        }
        if (use_short_opcodes)
            oi = &short_opcode_info(op);
        else
            oi = &opcode_info[op];
        size = oi->size;
        if (pos + size > len) {
            printf("truncated opcode (0x%02x)\n", op);
            break;
        }
#if defined(DUMP_BYTECODE) && (DUMP_BYTECODE & 16)
        {
            int i, x, x0;
            x = x0 = printf("%5d ", pos);
            for (i = 0; i < size; i++) {
                if (i == 6) {
                    printf("\n%*s", x = x0, "");
                }
                x += printf(" %02X", tab[pos + i]);
            }
            printf("%*s", x0 + 20 - x, "");
        }
#endif
        if (bits[pos]) {
            printf("%5d:  ", pos);
        } else {
            printf("        ");
        }
        printf("%s", oi->name);
        pos++;
        switch(oi->fmt) {
        case OP_FMT_none_int:
            printf(" %d", op - OP_push_0);
            break;
        case OP_FMT_npopx:
            printf(" %d", op - OP_call0);
            break;
        case OP_FMT_u8:
            printf(" %u", get_u8(tab + pos));
            break;
        case OP_FMT_i8:
            printf(" %d", get_i8(tab + pos));
            break;
        case OP_FMT_u16:
        case OP_FMT_npop:
            printf(" %u", get_u16(tab + pos));
            break;
        case OP_FMT_npop_u16:
            printf(" %u,%u", get_u16(tab + pos), get_u16(tab + pos + 2));
            break;
        case OP_FMT_i16:
            printf(" %d", get_i16(tab + pos));
            break;
        case OP_FMT_i32:
            printf(" %d", get_i32(tab + pos));
            break;
        case OP_FMT_u32:
            printf(" %u", get_u32(tab + pos));
            break;
#if SHORT_OPCODES
        case OP_FMT_label8:
            addr = get_i8(tab + pos);
            goto has_addr1;
        case OP_FMT_label16:
            addr = get_i16(tab + pos);
            goto has_addr1;
#endif
        case OP_FMT_label:
            addr = get_u32(tab + pos);
            goto has_addr1;
        has_addr1:
            if (pass == 1)
                printf(" %u:%u", addr, label_slots[addr].pos);
            if (pass == 2)
                printf(" %u:%u", addr, label_slots[addr].pos2);
            if (pass == 3)
                printf(" %u", addr + pos);
            break;
        case OP_FMT_label_u16:
            addr = get_u32(tab + pos);
            if (pass == 1)
                printf(" %u:%u", addr, label_slots[addr].pos);
            if (pass == 2)
                printf(" %u:%u", addr, label_slots[addr].pos2);
            if (pass == 3)
                printf(" %u", addr + pos);
            printf(",%u", get_u16(tab + pos + 4));
            break;
#if SHORT_OPCODES
        case OP_FMT_const8:
            idx = get_u8(tab + pos);
            goto has_pool_idx;
#endif
        case OP_FMT_const:
            idx = get_u32(tab + pos);
            goto has_pool_idx;
        has_pool_idx:
            printf(" %u: ", idx);
            if (idx < cpool_count) {
                JS_DumpValue(ctx, cpool[idx]);
            }
            break;
        case OP_FMT_atom:
            printf(" ");
            print_atom(ctx, get_u32(tab + pos));
            break;
        case OP_FMT_atom_u8:
            printf(" ");
            print_atom(ctx, get_u32(tab + pos));
            printf(",%d", get_u8(tab + pos + 4));
            break;
        case OP_FMT_atom_u16:
            printf(" ");
            print_atom(ctx, get_u32(tab + pos));
            printf(",%d", get_u16(tab + pos + 4));
            break;
        case OP_FMT_atom_label_u8:
        case OP_FMT_atom_label_u16:
            printf(" ");
            print_atom(ctx, get_u32(tab + pos));
            addr = get_u32(tab + pos + 4);
            if (pass == 1)
                printf(",%u:%u", addr, label_slots[addr].pos);
            if (pass == 2)
                printf(",%u:%u", addr, label_slots[addr].pos2);
            if (pass == 3)
                printf(",%u", addr + pos + 4);
            if (oi->fmt == OP_FMT_atom_label_u8)
                printf(",%u", get_u8(tab + pos + 8));
            else
                printf(",%u", get_u16(tab + pos + 8));
            break;
        case OP_FMT_none_loc:
            idx = (op - OP_get_loc0) % 4;
            goto has_loc;
        case OP_FMT_loc8:
            idx = get_u8(tab + pos);
            goto has_loc;
        case OP_FMT_loc:
            idx = get_u16(tab + pos);
        has_loc:
            printf(" %d: ", idx);
            if (idx < var_count) {
                print_atom(ctx, vars[idx].var_name);
            }
            break;
        case OP_FMT_none_arg:
            idx = (op - OP_get_arg0) % 4;
            goto has_arg;
        case OP_FMT_arg:
            idx = get_u16(tab + pos);
        has_arg:
            printf(" %d: ", idx);
            if (idx < arg_count) {
                print_atom(ctx, args[idx].var_name);
            }
            break;
        case OP_FMT_none_var_ref:
            idx = (op - OP_get_var_ref0) % 4;
            goto has_var_ref;
        case OP_FMT_var_ref:
            idx = get_u16(tab + pos);
        has_var_ref:
            printf(" %d: ", idx);
            if (idx < closure_var_count) {
                print_atom(ctx, closure_var[idx].var_name);
            }
            break;
        default:
            break;
        }
        printf("\n");
        pos += oi->size - 1;
    }
    if (source) {
        if (!in_source)
            printf("\n");
        print_lines(source, line, INT32_MAX);
    }
    js_free(ctx, bits);
}

static __maybe_unused void dump_pc2line(JSContext *ctx, const uint8_t *buf, int len,
                                                 int line_num)
{
    const uint8_t *p_end, *p_next, *p;
    int pc, v;
    unsigned int op;

    if (len <= 0)
        return;

    printf("%5s %5s\n", "PC", "LINE");

    p = buf;
    p_end = buf + len;
    pc = 0;
    while (p < p_end) {
        op = *p++;
        if (op == 0) {
            v = unicode_from_utf8(p, p_end - p, &p_next);
            if (v < 0)
                goto fail;
            pc += v;
            p = p_next;
            v = unicode_from_utf8(p, p_end - p, &p_next);
            if (v < 0) {
            fail:
                printf("invalid pc2line encode pos=%d\n", (int)(p - buf));
                return;
            }
            if (!(v & 1)) {
                v = v >> 1;
            } else {
                v = -(v >> 1) - 1;
            }
            line_num += v;
            p = p_next;
        } else {
            op -= PC2LINE_OP_FIRST;
            pc += (op / PC2LINE_RANGE);
            line_num += (op % PC2LINE_RANGE) + PC2LINE_BASE;
        }
        printf("%5d %5d\n", pc, line_num);
    }
}

static __maybe_unused void js_dump_function_bytecode(JSContext *ctx, JSFunctionBytecode *b)
{
    int i;
    char atom_buf[ATOM_GET_STR_BUF_SIZE];
    const char *str;

    if (b->has_debug && b->debug.filename != JS_ATOM_NULL) {
        str = JS_AtomGetStr(ctx, atom_buf, sizeof(atom_buf), b->debug.filename);
        printf("%s:%d: ", str, b->debug.line_num);
    }

    str = JS_AtomGetStr(ctx, atom_buf, sizeof(atom_buf), b->func_name);
    printf("function: %s%s\n", &"*"[b->func_kind != JS_FUNC_GENERATOR], str);
    if (b->js_mode) {
        printf("  mode:");
        if (b->js_mode & JS_MODE_STRICT)
            printf(" strict");
#ifdef CONFIG_BIGNUM
        if (b->js_mode & JS_MODE_MATH)
            printf(" math");
#endif
        printf("\n");
    }
    if (b->arg_count && b->vardefs) {
        printf("  args:");
        for(i = 0; i < b->arg_count; i++) {
            printf(" %s", JS_AtomGetStr(ctx, atom_buf, sizeof(atom_buf),
                                        b->vardefs[i].var_name));
        }
        printf("\n");
    }
    if (b->var_count && b->vardefs) {
        printf("  locals:\n");
        for(i = 0; i < b->var_count; i++) {
            JSVarDef *vd = &b->vardefs[b->arg_count + i];
            printf("%5d: %s %s", i,
                   vd->var_kind == JS_VAR_CATCH ? "catch" :
                   (vd->var_kind == JS_VAR_FUNCTION_DECL ||
                    vd->var_kind == JS_VAR_NEW_FUNCTION_DECL) ? "function" :
                   vd->is_const ? "const" :
                   vd->is_lexical ? "let" : "var",
                   JS_AtomGetStr(ctx, atom_buf, sizeof(atom_buf), vd->var_name));
            if (vd->scope_level)
                printf(" [level:%d next:%d]", vd->scope_level, vd->scope_next);
            printf("\n");
        }
    }
    if (b->closure_var_count) {
        printf("  closure vars:\n");
        for(i = 0; i < b->closure_var_count; i++) {
            JSClosureVar *cv = &b->closure_var[i];
            printf("%5d: %s %s:%s%d %s\n", i,
                   JS_AtomGetStr(ctx, atom_buf, sizeof(atom_buf), cv->var_name),
                   cv->is_local ? "local" : "parent",
                   cv->is_arg ? "arg" : "loc", cv->var_idx,
                   cv->is_const ? "const" :
                   cv->is_lexical ? "let" : "var");
        }
    }
    printf("  stack_size: %d\n", b->stack_size);
    printf("  opcodes:\n");
    dump_byte_code(ctx, 3, b->byte_code_buf, b->byte_code_len,
                   b->vardefs, b->arg_count,
                   b->vardefs ? b->vardefs + b->arg_count : NULL, b->var_count,
                   b->closure_var, b->closure_var_count,
                   b->cpool, b->cpool_count,
                   b->has_debug ? b->debug.source : NULL,
                   b->has_debug ? b->debug.line_num : -1, NULL, b);
#if defined(DUMP_BYTECODE) && (DUMP_BYTECODE & 32)
    if (b->has_debug)
        dump_pc2line(ctx, b->debug.pc2line_buf, b->debug.pc2line_len, b->debug.line_num);
#endif
    printf("\n");
}
#endif

static int add_closure_var(JSContext *ctx, JSFunctionDef *s,
                           BOOL is_local, BOOL is_arg,
                           int var_idx, JSAtom var_name,
                           BOOL is_const, BOOL is_lexical,
                           JSVarKindEnum var_kind)
{
    JSClosureVar *cv;

    /* the closure variable indexes are currently stored on 16 bits */
    if (s->closure_var_count >= JS_MAX_LOCAL_VARS) {
        JS_ThrowInternalError(ctx, "too many closure variables");
        return -1;
    }

    if (js_resize_array(ctx, (void **)&s->closure_var,
                        sizeof(s->closure_var[0]),
                        &s->closure_var_size, s->closure_var_count + 1))
        return -1;
    cv = &s->closure_var[s->closure_var_count++];
    cv->is_local = is_local;
    cv->is_arg = is_arg;
    cv->is_const = is_const;
    cv->is_lexical = is_lexical;
    cv->var_kind = var_kind;
    cv->var_idx = var_idx;
    cv->var_name = JS_DupAtom(ctx, var_name);
    return s->closure_var_count - 1;
}

static int find_closure_var(JSContext *ctx, JSFunctionDef *s,
                            JSAtom var_name)
{
    int i;
    for(i = 0; i < s->closure_var_count; i++) {
        JSClosureVar *cv = &s->closure_var[i];
        if (cv->var_name == var_name)
            return i;
    }
    return -1;
}

/* 'fd' must be a parent of 's'. Create in 's' a closure referencing a
   local variable (is_local = TRUE) or a closure (is_local = FALSE) in
   'fd' */
static int get_closure_var2(JSContext *ctx, JSFunctionDef *s,
                            JSFunctionDef *fd, BOOL is_local,
                            BOOL is_arg, int var_idx, JSAtom var_name,
                            BOOL is_const, BOOL is_lexical,
                            JSVarKindEnum var_kind)
{
    int i;

    if (fd != s->parent) {
        var_idx = get_closure_var2(ctx, s->parent, fd, is_local,
                                   is_arg, var_idx, var_name,
                                   is_const, is_lexical, var_kind);
        if (var_idx < 0)
            return -1;
        is_local = FALSE;
    }
    for(i = 0; i < s->closure_var_count; i++) {
        JSClosureVar *cv = &s->closure_var[i];
        if (cv->var_idx == var_idx && cv->is_arg == is_arg &&
            cv->is_local == is_local)
            return i;
    }
    return add_closure_var(ctx, s, is_local, is_arg, var_idx, var_name,
                           is_const, is_lexical, var_kind);
}

static int get_closure_var(JSContext *ctx, JSFunctionDef *s,
                           JSFunctionDef *fd, BOOL is_arg,
                           int var_idx, JSAtom var_name,
                           BOOL is_const, BOOL is_lexical,
                           JSVarKindEnum var_kind)
{
    return get_closure_var2(ctx, s, fd, TRUE, is_arg,
                            var_idx, var_name, is_const, is_lexical,
                            var_kind);
}

static int get_with_scope_opcode(int op)
{
    if (op == OP_scope_get_var_undef)
        return OP_with_get_var;
    else
        return OP_with_get_var + (op - OP_scope_get_var);
}

static BOOL can_opt_put_ref_value(const uint8_t *bc_buf, int pos)
{
    int opcode = bc_buf[pos];
    return (bc_buf[pos + 1] == OP_put_ref_value &&
            (opcode == OP_insert3 ||
             opcode == OP_perm4 ||
             //opcode == OP_nop ||
             opcode == OP_rot3l));
}

static BOOL can_opt_put_global_ref_value(const uint8_t *bc_buf, int pos)
{
    int opcode = bc_buf[pos];
    return (bc_buf[pos + 1] == OP_put_ref_value &&
            (opcode == OP_insert3 ||
             opcode == OP_perm4 ||
             //opcode == OP_nop ||
             opcode == OP_rot3l));
}

static int optimize_scope_make_ref(JSContext *ctx, JSFunctionDef *s,
                                   DynBuf *bc, uint8_t *bc_buf,
                                   LabelSlot *ls, int pos_next,
                                   int get_op, int var_idx)
{
    int label_pos, end_pos, pos;

    /* XXX: should optimize `loc(a) += expr` as `expr add_loc(a)`
       but only if expr does not modify `a`.
       should scan the code between pos_next and label_pos
       for operations that can potentially change `a`:
       OP_scope_make_ref(a), function calls, jumps and gosub.
     */
    /* replace the reference get/put with normal variable
       accesses */
    if (bc_buf[pos_next] == OP_get_ref_value) {
        dbuf_putc(bc, get_op);
        dbuf_put_u16(bc, var_idx);
        pos_next++;
    }
    /* remove the OP_label to make room for replacement */
    /* label should have a refcount of 0 anyway */
    /* XXX: should avoid this patch by inserting nops in phase 1 */
    label_pos = ls->pos;
    pos = label_pos - 5;
    assert(bc_buf[pos] == OP_label);
    /* label points to an instruction pair:
       - insert3 / put_ref_value
       - perm4 / put_ref_value
       - rot3l / put_ref_value
       - nop / put_ref_value
     */
    end_pos = label_pos + 2;
    if (bc_buf[label_pos] == OP_insert3)
        bc_buf[pos++] = OP_dup;
    bc_buf[pos] = get_op + 1;
    put_u16(bc_buf + pos + 1, var_idx);
    pos += 3;
    /* pad with OP_nop */
    while (pos < end_pos)
        bc_buf[pos++] = OP_nop;
    return pos_next;
}

static int optimize_scope_make_global_ref(JSContext *ctx, JSFunctionDef *s,
                                          DynBuf *bc, uint8_t *bc_buf,
                                          LabelSlot *ls, int pos_next,
                                          JSAtom var_name)
{
    int label_pos, end_pos, pos, op;
    BOOL is_strict;
    is_strict = ((s->js_mode & JS_MODE_STRICT) != 0);

    /* replace the reference get/put with normal variable
       accesses */
    if (is_strict) {
        /* need to check if the variable exists before evaluating the right
           expression */
        /* XXX: need an extra OP_true if destructuring an array */
        dbuf_putc(bc, OP_check_var);
        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
    } else {
        /* XXX: need 2 extra OP_true if destructuring an array */
    }
    if (bc_buf[pos_next] == OP_get_ref_value) {
        dbuf_putc(bc, OP_get_var);
        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
        pos_next++;
    }
    /* remove the OP_label to make room for replacement */
    /* label should have a refcount of 0 anyway */
    /* XXX: should have emitted several OP_nop to avoid this kludge */
    label_pos = ls->pos;
    pos = label_pos - 5;
    assert(bc_buf[pos] == OP_label);
    end_pos = label_pos + 2;
    op = bc_buf[label_pos];
    if (is_strict) {
        switch(op) {
            case OP_insert3:
                op = OP_insert2;
                break;
            case OP_perm4:
                op = OP_perm3;
                break;
            case OP_rot3l:
                op = OP_swap;
                break;
            default:
                abort();
        }
        bc_buf[pos++] = op;
    } else {
        if (op == OP_insert3)
            bc_buf[pos++] = OP_dup;
    }
    if (is_strict) {
        bc_buf[pos] = OP_put_var_strict;
        /* XXX: need 1 extra OP_drop if destructuring an array */
    } else {
        bc_buf[pos] = OP_put_var;
        /* XXX: need 2 extra OP_drop if destructuring an array */
    }
    put_u32(bc_buf + pos + 1, JS_DupAtom(ctx, var_name));
    pos += 5;
    /* pad with OP_nop */
    while (pos < end_pos)
        bc_buf[pos++] = OP_nop;
    return pos_next;
}

static int add_var_this(JSContext *ctx, JSFunctionDef *fd)
{
    int idx;
    idx = add_var(ctx, fd, JS_ATOM_this);
    if (idx >= 0 && fd->is_derived_class_constructor) {
        JSVarDef *vd = &fd->vars[idx];
        /* XXX: should have is_this flag or var type */
        vd->is_lexical = 1; /* used to trigger 'uninitialized' checks
                               in a derived class constructor */
    }
    return idx;
}

static int resolve_pseudo_var(JSContext *ctx, JSFunctionDef *s,
                              JSAtom var_name)
{
    int var_idx;

    if (!s->has_this_binding)
        return -1;
    switch(var_name) {
        case JS_ATOM_home_object:
            /* 'home_object' pseudo variable */
            var_idx = s->home_object_var_idx = add_var(ctx, s, var_name);
            break;
        case JS_ATOM_this_active_func:
            /* 'this.active_func' pseudo variable */
            var_idx = s->this_active_func_var_idx = add_var(ctx, s, var_name);
            break;
        case JS_ATOM_new_target:
            /* 'new.target' pseudo variable */
            var_idx = s->new_target_var_idx = add_var(ctx, s, var_name);
            break;
        case JS_ATOM_this:
            /* 'this' pseudo variable */
            var_idx = s->this_var_idx = add_var_this(ctx, s);
            break;
        default:
            var_idx = -1;
            break;
    }
    return var_idx;
}

/* return the position of the next opcode */
static int resolve_scope_var(JSContext *ctx, JSFunctionDef *s,
                             JSAtom var_name, int scope_level, int op,
                             DynBuf *bc, uint8_t *bc_buf,
                             LabelSlot *ls, int pos_next, int arg_valid)
{
    int idx, var_idx, is_put;
    int label_done;
    BOOL is_func_var = FALSE;
    JSFunctionDef *fd;
    JSVarDef *vd;
    BOOL is_pseudo_var;

    label_done = -1;

    /* XXX: could be simpler to use a specific function to
       resolve the pseudo variables */
    is_pseudo_var = (var_name == JS_ATOM_home_object ||
                     var_name == JS_ATOM_this_active_func ||
                     var_name == JS_ATOM_new_target ||
                     var_name == JS_ATOM_this);

    /* resolve local scoped variables */
    var_idx = -1;
    for (idx = s->scopes[scope_level].first; idx >= 0;) {
        vd = &s->vars[idx];
        if (vd->var_name == var_name) {
            if (op == OP_scope_put_var || op == OP_scope_make_ref) {
                if (vd->is_const) {
                    dbuf_putc(bc, OP_throw_var);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    dbuf_putc(bc, JS_THROW_VAR_RO);
                    goto done;
                }
                is_func_var = vd->is_func_var;
            }
            var_idx = idx;
            break;
        } else
        if (vd->var_name == JS_ATOM__with_ && !is_pseudo_var) {
            dbuf_putc(bc, OP_get_loc);
            dbuf_put_u16(bc, idx);
            dbuf_putc(bc, get_with_scope_opcode(op));
            dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            label_done = new_label_fd(s, label_done);
            dbuf_put_u32(bc, label_done);
            dbuf_putc(bc, 1);
            update_label(s, label_done, 1);
            s->jump_size++;
        }
        idx = vd->scope_next;
    }
    if (var_idx < 0) {
        /* XXX: scoping issues:
           should not resolve vars from the function body during argument parse,
           `arguments` and function-name should not be hidden by later vars.
         */
        var_idx = find_var(ctx, s, var_name);
        if (var_idx >= 0) {
            if (scope_level == 0
                &&  (var_idx & ARGUMENT_VAR_OFFSET)
                &&  (var_idx - ARGUMENT_VAR_OFFSET) >= arg_valid) {
                /* referring to an uninitialized argument */
                dbuf_putc(bc, OP_throw_var);
                dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                dbuf_putc(bc, JS_THROW_VAR_UNINITIALIZED);
            }
            if (!(var_idx & ARGUMENT_VAR_OFFSET))
                is_func_var = s->vars[var_idx].is_func_var;
        }

        if (var_idx < 0 && is_pseudo_var)
            var_idx = resolve_pseudo_var(ctx, s, var_name);

        if (var_idx < 0 && var_name == JS_ATOM_arguments &&
            s->has_arguments_binding) {
            /* 'arguments' pseudo variable */
            var_idx = add_arguments_var(ctx, s, var_name);
        }
        if (var_idx < 0 && s->is_func_expr && var_name == s->func_name) {
            /* add a new variable with the function name */
            var_idx = add_func_var(ctx, s, var_name);
            is_func_var = TRUE;
        }
    }
    if (var_idx >= 0) {
        /* OP_scope_put_var_init is only used to initialize a
           lexical variable, so it is never used in a with or var object. It
           can be used with a closure (module global variable case). */
        switch (op) {
            case OP_scope_make_ref:
                if (is_func_var) {
                    /* Create a dummy object reference for the func_var */
                    dbuf_putc(bc, OP_object);
                    dbuf_putc(bc, OP_get_loc);
                    dbuf_put_u16(bc, var_idx);
                    dbuf_putc(bc, OP_define_field);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    dbuf_putc(bc, OP_push_atom_value);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                } else
                if (label_done == -1 && can_opt_put_ref_value(bc_buf, ls->pos)) {
                    int get_op;
                    if (var_idx & ARGUMENT_VAR_OFFSET) {
                        get_op = OP_get_arg;
                        var_idx -= ARGUMENT_VAR_OFFSET;
                    } else {
                        if (s->vars[var_idx].is_lexical)
                            get_op = OP_get_loc_check;
                        else
                            get_op = OP_get_loc;
                    }
                    pos_next = optimize_scope_make_ref(ctx, s, bc, bc_buf, ls,
                                                       pos_next, get_op, var_idx);
                } else {
                    /* Create a dummy object with a named slot that is
                   a reference to the local variable */
                    if (var_idx & ARGUMENT_VAR_OFFSET) {
                        dbuf_putc(bc, OP_make_arg_ref);
                        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                        dbuf_put_u16(bc, var_idx - ARGUMENT_VAR_OFFSET);
                    } else {
                        dbuf_putc(bc, OP_make_loc_ref);
                        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                        dbuf_put_u16(bc, var_idx);
                    }
                }
                break;
            case OP_scope_get_ref:
                dbuf_putc(bc, OP_undefined);
                /* fall thru */
            case OP_scope_get_var_undef:
            case OP_scope_get_var:
            case OP_scope_put_var:
            case OP_scope_put_var_init:
                is_put = (op == OP_scope_put_var || op == OP_scope_put_var_init);
                if (var_idx & ARGUMENT_VAR_OFFSET) {
                    dbuf_putc(bc, OP_get_arg + is_put);
                    dbuf_put_u16(bc, var_idx - ARGUMENT_VAR_OFFSET);
                    /* XXX: should test if argument reference needs TDZ check */
                } else {
                    if (is_put) {
                        if (s->vars[var_idx].is_lexical) {
                            if (op == OP_scope_put_var_init) {
                                /* 'this' can only be initialized once */
                                if (var_name == JS_ATOM_this)
                                    dbuf_putc(bc, OP_put_loc_check_init);
                                else
                                    dbuf_putc(bc, OP_put_loc);
                            } else {
                                dbuf_putc(bc, OP_put_loc_check);
                            }
                        } else {
                            dbuf_putc(bc, OP_put_loc);
                        }
                    } else {
                        if (s->vars[var_idx].is_lexical) {
                            dbuf_putc(bc, OP_get_loc_check);
                        } else {
                            dbuf_putc(bc, OP_get_loc);
                        }
                    }
                    dbuf_put_u16(bc, var_idx);
                }
                break;
            case OP_scope_delete_var:
                dbuf_putc(bc, OP_push_false);
                break;
        }
        goto done;
    }
    /* check eval object */
    if (s->var_object_idx >= 0 && !is_pseudo_var) {
        dbuf_putc(bc, OP_get_loc);
        dbuf_put_u16(bc, s->var_object_idx);
        dbuf_putc(bc, get_with_scope_opcode(op));
        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
        label_done = new_label_fd(s, label_done);
        dbuf_put_u32(bc, label_done);
        dbuf_putc(bc, 0);
        update_label(s, label_done, 1);
        s->jump_size++;
    }
    /* check parent scopes */
    for (fd = s; fd->parent;) {
        scope_level = fd->parent_scope_level;
        fd = fd->parent;
        if (scope_level == 0) {
            /* function is defined as part of the argument parsing: hide vars
               from the function body.
               XXX: variables created from argument destructuring might need
               to be visible, should refine this method.
             */
            var_idx = find_arg(ctx, fd, var_name);
            goto check_idx;
        }
        for (idx = fd->scopes[scope_level].first; idx >= 0;) {
            vd = &fd->vars[idx];
            if (vd->var_name == var_name) {
                if (op == OP_scope_put_var || op == OP_scope_make_ref) {
                    if (vd->is_const) {
                        dbuf_putc(bc, OP_throw_var);
                        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                        dbuf_putc(bc, JS_THROW_VAR_RO);
                        goto done;
                    }
                    is_func_var = vd->is_func_var;
                }
                var_idx = idx;
                break;
            } else if (vd->var_name == JS_ATOM__with_ && !is_pseudo_var) {
                vd->is_captured = 1;
                idx = get_closure_var(ctx, s, fd, FALSE, idx, vd->var_name, FALSE, FALSE, JS_VAR_NORMAL);
                if (idx >= 0) {
                    dbuf_putc(bc, OP_get_var_ref);
                    dbuf_put_u16(bc, idx);
                    dbuf_putc(bc, get_with_scope_opcode(op));
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    label_done = new_label_fd(s, label_done);
                    dbuf_put_u32(bc, label_done);
                    dbuf_putc(bc, 1);
                    update_label(s, label_done, 1);
                    s->jump_size++;
                }
            }
            idx = vd->scope_next;
        }
        if (var_idx >= 0)
            break;

        var_idx = find_var(ctx, fd, var_name);
        check_idx:
        if (var_idx >= 0) {
            if (!(var_idx & ARGUMENT_VAR_OFFSET))
                is_func_var = fd->vars[var_idx].is_func_var;
            break;
        }
        if (is_pseudo_var) {
            var_idx = resolve_pseudo_var(ctx, fd, var_name);
            if (var_idx >= 0)
                break;
        }
        if (var_name == JS_ATOM_arguments && fd->has_arguments_binding) {
            var_idx = add_arguments_var(ctx, fd, var_name);
            break;
        }
        if (fd->is_func_expr && fd->func_name == var_name) {
            /* add a new variable with the function name */
            var_idx = add_func_var(ctx, fd, var_name);
            is_func_var = TRUE;
            break;
        }

        /* check eval object */
        if (fd->var_object_idx >= 0 && !is_pseudo_var) {
            fd->vars[fd->var_object_idx].is_captured = 1;
            idx = get_closure_var(ctx, s, fd, FALSE,
                                  fd->var_object_idx, JS_ATOM__var_,
                                  FALSE, FALSE, JS_VAR_NORMAL);
            dbuf_putc(bc, OP_get_var_ref);
            dbuf_put_u16(bc, idx);
            dbuf_putc(bc, get_with_scope_opcode(op));
            dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            label_done = new_label_fd(s, label_done);
            dbuf_put_u32(bc, label_done);
            dbuf_putc(bc, 0);
            update_label(s, label_done, 1);
            s->jump_size++;
        }

        if (fd->is_eval)
            break; /* it it necessarily the top level function */
    }

    /* check direct eval scope (in the closure of the eval function
       which is necessarily at the top level) */
    if (!fd)
        fd = s;
    if (var_idx < 0 && fd->is_eval) {
        int idx1;
        for (idx1 = 0; idx1 < fd->closure_var_count; idx1++) {
            JSClosureVar *cv = &fd->closure_var[idx1];
            if (var_name == cv->var_name) {
                if (fd != s) {
                    idx = get_closure_var2(ctx, s, fd,
                                           FALSE,
                                           cv->is_arg, idx1,
                                           cv->var_name, cv->is_const,
                                           cv->is_lexical, cv->var_kind);
                } else {
                    idx = idx1;
                }
                goto has_idx;
            } else if ((cv->var_name == JS_ATOM__var_ ||
                        cv->var_name == JS_ATOM__with_) && !is_pseudo_var) {
                int is_with = (cv->var_name == JS_ATOM__with_);
                if (fd != s) {
                    idx = get_closure_var2(ctx, s, fd,
                                           FALSE,
                                           cv->is_arg, idx1,
                                           cv->var_name, FALSE, FALSE,
                                           JS_VAR_NORMAL);
                } else {
                    idx = idx1;
                }
                dbuf_putc(bc, OP_get_var_ref);
                dbuf_put_u16(bc, idx);
                dbuf_putc(bc, get_with_scope_opcode(op));
                dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                label_done = new_label_fd(s, label_done);
                dbuf_put_u32(bc, label_done);
                dbuf_putc(bc, is_with);
                update_label(s, label_done, 1);
                s->jump_size++;
            }
        }
    }

    if (var_idx >= 0) {
        /* find the corresponding closure variable */
        if (var_idx & ARGUMENT_VAR_OFFSET) {
            fd->args[var_idx - ARGUMENT_VAR_OFFSET].is_captured = 1;
            idx = get_closure_var(ctx, s, fd,
                                  TRUE, var_idx - ARGUMENT_VAR_OFFSET,
                                  var_name, FALSE, FALSE, JS_VAR_NORMAL);
        } else {
            fd->vars[var_idx].is_captured = 1;
            idx = get_closure_var(ctx, s, fd,
                                  FALSE, var_idx,
                                  var_name,
                                  fd->vars[var_idx].is_const,
                                  fd->vars[var_idx].is_lexical,
                                  fd->vars[var_idx].var_kind);
        }
        if (idx >= 0) {
            has_idx:
            if ((op == OP_scope_put_var || op == OP_scope_make_ref) &&
                s->closure_var[idx].is_const) {
                dbuf_putc(bc, OP_throw_var);
                dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                dbuf_putc(bc, JS_THROW_VAR_RO);
                goto done;
            }
            switch (op) {
                case OP_scope_make_ref:
                    if (is_func_var) {
                        /* Create a dummy object reference for the func_var */
                        dbuf_putc(bc, OP_object);
                        dbuf_putc(bc, OP_get_var_ref);
                        dbuf_put_u16(bc, idx);
                        dbuf_putc(bc, OP_define_field);
                        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                        dbuf_putc(bc, OP_push_atom_value);
                        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    } else
                    if (label_done == -1 &&
                        can_opt_put_ref_value(bc_buf, ls->pos)) {
                        int get_op;
                        if (s->closure_var[idx].is_lexical)
                            get_op = OP_get_var_ref_check;
                        else
                            get_op = OP_get_var_ref;
                        pos_next = optimize_scope_make_ref(ctx, s, bc, bc_buf, ls,
                                                           pos_next,
                                                           get_op, idx);
                    } else {
                        /* Create a dummy object with a named slot that is
                       a reference to the closure variable */
                        dbuf_putc(bc, OP_make_var_ref_ref);
                        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                        dbuf_put_u16(bc, idx);
                    }
                    break;
                case OP_scope_get_ref:
                    /* XXX: should create a dummy object with a named slot that is
                   a reference to the closure variable */
                    dbuf_putc(bc, OP_undefined);
                    /* fall thru */
                case OP_scope_get_var_undef:
                case OP_scope_get_var:
                case OP_scope_put_var:
                case OP_scope_put_var_init:
                    is_put = (op == OP_scope_put_var ||
                              op == OP_scope_put_var_init);
                    if (is_put) {
                        if (s->closure_var[idx].is_lexical) {
                            if (op == OP_scope_put_var_init) {
                                /* 'this' can only be initialized once */
                                if (var_name == JS_ATOM_this)
                                    dbuf_putc(bc, OP_put_var_ref_check_init);
                                else
                                    dbuf_putc(bc, OP_put_var_ref);
                            } else {
                                dbuf_putc(bc, OP_put_var_ref_check);
                            }
                        } else {
                            dbuf_putc(bc, OP_put_var_ref);
                        }
                    } else {
                        if (s->closure_var[idx].is_lexical) {
                            dbuf_putc(bc, OP_get_var_ref_check);
                        } else {
                            dbuf_putc(bc, OP_get_var_ref);
                        }
                    }
                    dbuf_put_u16(bc, idx);
                    break;
                case OP_scope_delete_var:
                    dbuf_putc(bc, OP_push_false);
                    break;
            }
            goto done;
        }
    }

    /* global variable access */

    switch (op) {
        case OP_scope_make_ref:
            if (label_done == -1 && can_opt_put_global_ref_value(bc_buf, ls->pos)) {
                pos_next = optimize_scope_make_global_ref(ctx, s, bc, bc_buf, ls,
                                                          pos_next, var_name);
            } else {
                dbuf_putc(bc, OP_make_var_ref);
                dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            }
            break;
        case OP_scope_get_ref:
            /* XXX: should create a dummy object with a named slot that is
           a reference to the global variable */
            dbuf_putc(bc, OP_undefined);
            dbuf_putc(bc, OP_get_var);
            dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            break;
        case OP_scope_get_var_undef:
        case OP_scope_get_var:
        case OP_scope_put_var:
            dbuf_putc(bc, OP_get_var_undef + (op - OP_scope_get_var_undef));
            dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            break;
        case OP_scope_put_var_init:
            dbuf_putc(bc, OP_put_var_init);
            dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            break;
        case OP_scope_delete_var:
            dbuf_putc(bc, OP_delete_var);
            dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            break;
    }
    done:
    if (label_done >= 0) {
        dbuf_putc(bc, OP_label);
        dbuf_put_u32(bc, label_done);
        s->label_slots[label_done].pos2 = bc->size;
    }
    return pos_next;
}

/* search in all scopes */
static int find_private_class_field_all(JSContext *ctx, JSFunctionDef *fd,
                                        JSAtom name, int scope_level)
{
    int idx;

    idx = fd->scopes[scope_level].first;
    while (idx >= 0) {
        if (fd->vars[idx].var_name == name)
            return idx;
        idx = fd->vars[idx].scope_next;
    }
    return -1;
}

static void get_loc_or_ref(DynBuf *bc, BOOL is_ref, int idx)
{
    /* if the field is not initialized, the error is catched when
       accessing it */
    if (is_ref)
        dbuf_putc(bc, OP_get_var_ref);
    else
        dbuf_putc(bc, OP_get_loc);
    dbuf_put_u16(bc, idx);
}

static int resolve_scope_private_field1(JSContext *ctx,
                                        BOOL *pis_ref, int *pvar_kind,
                                        JSFunctionDef *s,
                                        JSAtom var_name, int scope_level)
{
    int idx, var_kind;
    JSFunctionDef *fd;
    BOOL is_ref;

    fd = s;
    is_ref = FALSE;
    for(;;) {
        idx = find_private_class_field_all(ctx, fd, var_name, scope_level);
        if (idx >= 0) {
            var_kind = fd->vars[idx].var_kind;
            if (is_ref) {
                idx = get_closure_var(ctx, s, fd, FALSE, idx, var_name,
                                      TRUE, TRUE, JS_VAR_NORMAL);
                if (idx < 0)
                    return -1;
            }
            break;
        }
        scope_level = fd->parent_scope_level;
        if (!fd->parent) {
            if (fd->is_eval) {
                /* closure of the eval function (top level) */
                for (idx = 0; idx < fd->closure_var_count; idx++) {
                    JSClosureVar *cv = &fd->closure_var[idx];
                    if (cv->var_name == var_name) {
                        var_kind = cv->var_kind;
                        is_ref = TRUE;
                        if (fd != s) {
                            idx = get_closure_var2(ctx, s, fd,
                                                   FALSE,
                                                   cv->is_arg, idx,
                                                   cv->var_name, cv->is_const,
                                                   cv->is_lexical,
                                                   cv->var_kind);
                            if (idx < 0)
                                return -1;
                        }
                        goto done;
                    }
                }
            }
            /* XXX: no line number info */
            JS_ThrowSyntaxErrorAtom(ctx, "undefined private field '%s'",
                                    var_name);
            return -1;
        } else {
            fd = fd->parent;
        }
        is_ref = TRUE;
    }
    done:
    *pis_ref = is_ref;
    *pvar_kind = var_kind;
    return idx;
}

/* return 0 if OK or -1 if the private field could not be resolved */
static int resolve_scope_private_field(JSContext *ctx, JSFunctionDef *s,
                                       JSAtom var_name, int scope_level, int op,
                                       DynBuf *bc)
{
    int idx, var_kind;
    BOOL is_ref;

    idx = resolve_scope_private_field1(ctx, &is_ref, &var_kind, s,
                                       var_name, scope_level);
    if (idx < 0)
        return -1;
    assert(var_kind != JS_VAR_NORMAL);
    switch (op) {
        case OP_scope_get_private_field:
        case OP_scope_get_private_field2:
            switch(var_kind) {
                case JS_VAR_PRIVATE_FIELD:
                    if (op == OP_scope_get_private_field2)
                        dbuf_putc(bc, OP_dup);
                    get_loc_or_ref(bc, is_ref, idx);
                    dbuf_putc(bc, OP_get_private_field);
                    break;
                case JS_VAR_PRIVATE_METHOD:
                    get_loc_or_ref(bc, is_ref, idx);
                    dbuf_putc(bc, OP_check_brand);
                    if (op != OP_scope_get_private_field2)
                        dbuf_putc(bc, OP_nip);
                    break;
                case JS_VAR_PRIVATE_GETTER:
                case JS_VAR_PRIVATE_GETTER_SETTER:
                    if (op == OP_scope_get_private_field2)
                        dbuf_putc(bc, OP_dup);
                    get_loc_or_ref(bc, is_ref, idx);
                    dbuf_putc(bc, OP_check_brand);
                    dbuf_putc(bc, OP_call_method);
                    dbuf_put_u16(bc, 0);
                    break;
                case JS_VAR_PRIVATE_SETTER:
                    /* XXX: add clearer error message */
                    dbuf_putc(bc, OP_throw_var);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    dbuf_putc(bc, JS_THROW_VAR_RO);
                    break;
                default:
                    abort();
            }
            break;
        case OP_scope_put_private_field:
            switch(var_kind) {
                case JS_VAR_PRIVATE_FIELD:
                    get_loc_or_ref(bc, is_ref, idx);
                    dbuf_putc(bc, OP_put_private_field);
                    break;
                case JS_VAR_PRIVATE_METHOD:
                case JS_VAR_PRIVATE_GETTER:
                    /* XXX: add clearer error message */
                    dbuf_putc(bc, OP_throw_var);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    dbuf_putc(bc, JS_THROW_VAR_RO);
                    break;
                case JS_VAR_PRIVATE_SETTER:
                case JS_VAR_PRIVATE_GETTER_SETTER:
                {
                    JSAtom setter_name = get_private_setter_name(ctx, var_name);
                    if (setter_name == JS_ATOM_NULL)
                        return -1;
                    idx = resolve_scope_private_field1(ctx, &is_ref,
                                                       &var_kind, s,
                                                       setter_name, scope_level);
                    JS_FreeAtom(ctx, setter_name);
                    if (idx < 0)
                        return -1;
                    assert(var_kind == JS_VAR_PRIVATE_SETTER);
                    get_loc_or_ref(bc, is_ref, idx);
                    dbuf_putc(bc, OP_swap);
                    /* obj func value */
                    dbuf_putc(bc, OP_rot3r);
                    /* value obj func */
                    dbuf_putc(bc, OP_check_brand);
                    dbuf_putc(bc, OP_rot3l);
                    /* obj func value */
                    dbuf_putc(bc, OP_call_method);
                    dbuf_put_u16(bc, 1);
                }
                    break;
                default:
                    abort();
            }
            break;
        default:
            abort();
    }
    return 0;
}

static void mark_eval_captured_variables(JSContext *ctx, JSFunctionDef *s,
                                         int scope_level)
{
    int idx;
    JSVarDef *vd;

    for (idx = s->scopes[scope_level].first; idx >= 0;) {
        vd = &s->vars[idx];
        vd->is_captured = 1;
        idx = vd->scope_next;
    }
}

static void add_eval_variables(JSContext *ctx, JSFunctionDef *s)
{
    JSFunctionDef *fd;
    JSVarDef *vd;
    int i, scope_level, scope_idx;
    BOOL has_arguments_binding, has_this_binding;

    /* in non strict mode, variables are created in the caller's
       environment object */
    if (!s->is_eval && !(s->js_mode & JS_MODE_STRICT)) {
        s->var_object_idx = add_var(ctx, s, JS_ATOM__var_);
    }

    /* eval can potentially use 'arguments' so we must define it */
    has_this_binding = s->has_this_binding;
    if (has_this_binding) {
        if (s->this_var_idx < 0)
            s->this_var_idx = add_var_this(ctx, s);
        if (s->new_target_var_idx < 0)
            s->new_target_var_idx = add_var(ctx, s, JS_ATOM_new_target);
        if (s->is_derived_class_constructor && s->this_active_func_var_idx < 0)
            s->this_active_func_var_idx = add_var(ctx, s, JS_ATOM_this_active_func);
        if (s->has_home_object && s->home_object_var_idx < 0)
            s->home_object_var_idx = add_var(ctx, s, JS_ATOM_home_object);
    }
    has_arguments_binding = s->has_arguments_binding;
    if (has_arguments_binding)
        add_arguments_var(ctx, s, JS_ATOM_arguments);
    if (s->is_func_expr && s->func_name != JS_ATOM_NULL)
        add_func_var(ctx, s, s->func_name);

    /* eval can use all the variables of the enclosing functions, so
       they must be all put in the closure. The closure variables are
       ordered by scope. It works only because no closure are created
       before. */
    assert(s->is_eval || s->closure_var_count == 0);

    /* XXX: inefficient, but eval performance is less critical */
    fd = s;
    for(;;) {
        scope_level = fd->parent_scope_level;
        fd = fd->parent;
        if (!fd)
            break;
        scope_idx = fd->scopes[scope_level].first;
        /* add 'this' if it was not previously added */
        if (!has_this_binding && fd->has_this_binding) {
            if (fd->this_var_idx < 0)
                fd->this_var_idx = add_var_this(ctx, fd);
            if (fd->new_target_var_idx < 0)
                fd->new_target_var_idx = add_var(ctx, fd, JS_ATOM_new_target);
            if (fd->is_derived_class_constructor && fd->this_active_func_var_idx < 0)
                fd->this_active_func_var_idx = add_var(ctx, fd, JS_ATOM_this_active_func);
            if (fd->has_home_object && fd->home_object_var_idx < 0)
                fd->home_object_var_idx = add_var(ctx, fd, JS_ATOM_home_object);
            has_this_binding = TRUE;
        }
        /* add 'arguments' if it was not previously added */
        if (!has_arguments_binding && fd->has_arguments_binding) {
            add_arguments_var(ctx, fd, JS_ATOM_arguments);
            has_arguments_binding = TRUE;
        }
        /* add function name */
        if (fd->is_func_expr && fd->func_name != JS_ATOM_NULL)
            add_func_var(ctx, fd, fd->func_name);

        /* add lexical variables */
        while (scope_idx >= 0) {
            vd = &fd->vars[scope_idx];
            vd->is_captured = 1;
            get_closure_var(ctx, s, fd, FALSE, scope_idx,
                            vd->var_name, vd->is_const, vd->is_lexical, vd->var_kind);
            scope_idx = vd->scope_next;
        }
        /* add unscoped variables */
        for(i = 0; i < fd->arg_count; i++) {
            vd = &fd->args[i];
            if (vd->var_name != JS_ATOM_NULL) {
                get_closure_var(ctx, s, fd,
                                TRUE, i, vd->var_name, FALSE, FALSE,
                                JS_VAR_NORMAL);
            }
        }
        for(i = 0; i < fd->var_count; i++) {
            vd = &fd->vars[i];
            /* do not close top level last result */
            if (vd->scope_level == 0 &&
                vd->var_name != JS_ATOM__ret_ &&
                vd->var_name != JS_ATOM_NULL) {
                get_closure_var(ctx, s, fd,
                                FALSE, i, vd->var_name, FALSE, FALSE,
                                JS_VAR_NORMAL);
            }
        }
        if (fd->is_eval) {
            int idx;
            /* add direct eval variables (we are necessarily at the
               top level) */
            for (idx = 0; idx < fd->closure_var_count; idx++) {
                JSClosureVar *cv = &fd->closure_var[idx];
                get_closure_var2(ctx, s, fd,
                                 FALSE, cv->is_arg,
                                 idx, cv->var_name, cv->is_const,
                                 cv->is_lexical, cv->var_kind);
            }
        }
    }
}

/* for direct eval compilation: add references to the variables of the
   calling function */
static __exception int add_closure_variables(JSContext *ctx, JSFunctionDef *s,
                                             JSFunctionBytecode *b, int scope_idx)
{
    int i, count;
    JSVarDef *vd;

    count = b->arg_count + b->var_count + b->closure_var_count;
    s->closure_var = NULL;
    s->closure_var_count = 0;
    s->closure_var_size = count;
    if (count == 0)
        return 0;
    s->closure_var = js_malloc(ctx, sizeof(s->closure_var[0]) * count);
    if (!s->closure_var)
        return -1;
    /* Add lexical variables in scope at the point of evaluation */
    for (i = scope_idx; i >= 0;) {
        vd = &b->vardefs[b->arg_count + i];
        if (vd->scope_level > 0) {
            JSClosureVar *cv = &s->closure_var[s->closure_var_count++];
            cv->is_local = TRUE;
            cv->is_arg = FALSE;
            cv->is_const = vd->is_const;
            cv->is_lexical = vd->is_lexical;
            cv->var_kind = vd->var_kind;
            cv->var_idx = i;
            cv->var_name = JS_DupAtom(ctx, vd->var_name);
        }
        i = vd->scope_next;
    }
    /* Add argument variables */
    for(i = 0; i < b->arg_count; i++) {
        JSClosureVar *cv = &s->closure_var[s->closure_var_count++];
        vd = &b->vardefs[i];
        cv->is_local = TRUE;
        cv->is_arg = TRUE;
        cv->is_const = FALSE;
        cv->is_lexical = FALSE;
        cv->var_kind = JS_VAR_NORMAL;
        cv->var_idx = i;
        cv->var_name = JS_DupAtom(ctx, vd->var_name);
    }
    /* Add local non lexical variables */
    for(i = 0; i < b->var_count; i++) {
        vd = &b->vardefs[b->arg_count + i];
        if (vd->scope_level == 0 && vd->var_name != JS_ATOM__ret_) {
            JSClosureVar *cv = &s->closure_var[s->closure_var_count++];
            cv->is_local = TRUE;
            cv->is_arg = FALSE;
            cv->is_const = FALSE;
            cv->is_lexical = FALSE;
            cv->var_kind = JS_VAR_NORMAL;
            cv->var_idx = i;
            cv->var_name = JS_DupAtom(ctx, vd->var_name);
        }
    }
    for(i = 0; i < b->closure_var_count; i++) {
        JSClosureVar *cv0 = &b->closure_var[i];
        JSClosureVar *cv = &s->closure_var[s->closure_var_count++];
        cv->is_local = FALSE;
        cv->is_arg = cv0->is_arg;
        cv->is_const = cv0->is_const;
        cv->is_lexical = cv0->is_lexical;
        cv->var_kind = cv0->var_kind;
        cv->var_idx = i;
        cv->var_name = JS_DupAtom(ctx, cv0->var_name);
    }
    return 0;
}

typedef struct CodeContext {
    const uint8_t *bc_buf; /* code buffer */
    int bc_len;   /* length of the code buffer */
    int pos;      /* position past the matched code pattern */
    int line_num; /* last visited OP_line_num parameter or -1 */
    int op;
    int idx;
    int label;
    int val;
    JSAtom atom;
} CodeContext;

#define M2(op1, op2)            ((op1) | ((op2) << 8))
#define M3(op1, op2, op3)       ((op1) | ((op2) << 8) | ((op3) << 16))
#define M4(op1, op2, op3, op4)  ((op1) | ((op2) << 8) | ((op3) << 16) | ((op4) << 24))

static BOOL code_match(CodeContext *s, int pos, ...)
{
    const uint8_t *tab = s->bc_buf;
    int op, len, op1, line_num, pos_next;
    va_list ap;
    BOOL ret = FALSE;

    line_num = -1;
    va_start(ap, pos);

    for(;;) {
        op1 = va_arg(ap, int);
        if (op1 == -1) {
            s->pos = pos;
            s->line_num = line_num;
            ret = TRUE;
            break;
        }
        for (;;) {
            if (pos >= s->bc_len)
                goto done;
            op = tab[pos];
            len = opcode_info[op].size;
            pos_next = pos + len;
            if (pos_next > s->bc_len)
                goto done;
            if (op == OP_line_num) {
                line_num = get_u32(tab + pos + 1);
                pos = pos_next;
            } else {
                break;
            }
        }
        if (op != op1) {
            if (op1 == (uint8_t)op1 || !op)
                break;
            if (op != (uint8_t)op1
                &&  op != (uint8_t)(op1 >> 8)
                &&  op != (uint8_t)(op1 >> 16)
                &&  op != (uint8_t)(op1 >> 24)) {
                break;
            }
            s->op = op;
        }

        pos++;
        switch(opcode_info[op].fmt) {
            case OP_FMT_loc8:
            case OP_FMT_u8:
            {
                int idx = tab[pos];
                int arg = va_arg(ap, int);
                if (arg == -1) {
                    s->idx = idx;
                } else {
                    if (arg != idx)
                        goto done;
                }
                break;
            }
            case OP_FMT_u16:
            case OP_FMT_npop:
            case OP_FMT_loc:
            case OP_FMT_arg:
            case OP_FMT_var_ref:
            {
                int idx = get_u16(tab + pos);
                int arg = va_arg(ap, int);
                if (arg == -1) {
                    s->idx = idx;
                } else {
                    if (arg != idx)
                        goto done;
                }
                break;
            }
            case OP_FMT_i32:
            case OP_FMT_u32:
            case OP_FMT_label:
            case OP_FMT_const:
            {
                s->label = get_u32(tab + pos);
                break;
            }
            case OP_FMT_label_u16:
            {
                s->label = get_u32(tab + pos);
                s->val = get_u16(tab + pos + 4);
                break;
            }
            case OP_FMT_atom:
            {
                s->atom = get_u32(tab + pos);
                break;
            }
            case OP_FMT_atom_u8:
            {
                s->atom = get_u32(tab + pos);
                s->val = get_u8(tab + pos + 4);
                break;
            }
            case OP_FMT_atom_u16:
            {
                s->atom = get_u32(tab + pos);
                s->val = get_u16(tab + pos + 4);
                break;
            }
            case OP_FMT_atom_label_u8:
            {
                s->atom = get_u32(tab + pos);
                s->label = get_u32(tab + pos + 4);
                s->val = get_u8(tab + pos + 8);
                break;
            }
            default:
                break;
        }
        pos = pos_next;
    }
    done:
    va_end(ap);
    return ret;
}

static void instantiate_hoisted_definitions(JSContext *ctx, JSFunctionDef *s, DynBuf *bc)
{
    int i, idx, var_idx;

    /* add the hoisted functions and variables */
    for(i = 0; i < s->hoisted_def_count; i++) {
        JSHoistedDef *hf = &s->hoisted_def[i];
        int has_closure = 0;
        BOOL force_init = hf->force_init;
        if (s->is_global_var && hf->var_name != JS_ATOM_NULL) {
            /* we are in an eval, so the closure contains all the
               enclosing variables */
            /* If the outer function has a variable environment,
               create a property for the variable there */
            for(idx = 0; idx < s->closure_var_count; idx++) {
                JSClosureVar *cv = &s->closure_var[idx];
                if (cv->var_name == hf->var_name) {
                    has_closure = 2;
                    force_init = FALSE;
                    break;
                }
                if (cv->var_name == JS_ATOM__var_) {
                    dbuf_putc(bc, OP_get_var_ref);
                    dbuf_put_u16(bc, idx);
                    has_closure = 1;
                    force_init = TRUE;
                    break;
                }
            }
            if (!has_closure) {
                int flags;

                flags = 0;
                if (s->eval_type != JS_EVAL_TYPE_GLOBAL)
                    flags |= JS_PROP_CONFIGURABLE;
                if (hf->cpool_idx >= 0 && !hf->is_lexical) {
                    /* global function definitions need a specific handling */
                    dbuf_putc(bc, OP_fclosure);
                    dbuf_put_u32(bc, hf->cpool_idx);

                    dbuf_putc(bc, OP_define_func);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, hf->var_name));
                    dbuf_putc(bc, flags);

                    goto done_hoisted_def;
                } else {
                    if (hf->is_lexical) {
                        flags |= DEFINE_GLOBAL_LEX_VAR;
                        if (!hf->is_const)
                            flags |= JS_PROP_WRITABLE;
                    }
                    dbuf_putc(bc, OP_define_var);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, hf->var_name));
                    dbuf_putc(bc, flags);
                }
            }
        }
        if (hf->cpool_idx >= 0 || force_init) {
            if (hf->cpool_idx >= 0) {
                dbuf_putc(bc, OP_fclosure);
                dbuf_put_u32(bc, hf->cpool_idx);
                if (hf->var_name == JS_ATOM__default_) {
                    /* set default export function name */
                    dbuf_putc(bc, OP_set_name);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, JS_ATOM_default));
                }
            } else {
                dbuf_putc(bc, OP_undefined);
            }
            if (s->is_global_var) {
                if (has_closure == 2) {
                    dbuf_putc(bc, OP_put_var_ref);
                    dbuf_put_u16(bc, idx);
                } else if (has_closure == 1) {
                    dbuf_putc(bc, OP_define_field);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, hf->var_name));
                    dbuf_putc(bc, OP_drop);
                } else {
                    /* XXX: Check if variable is writable and enumerable */
                    dbuf_putc(bc, OP_put_var);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, hf->var_name));
                }
            } else {
                var_idx = hf->var_idx;
                if (var_idx & ARGUMENT_VAR_OFFSET) {
                    dbuf_putc(bc, OP_put_arg);
                    dbuf_put_u16(bc, var_idx - ARGUMENT_VAR_OFFSET);
                } else {
                    dbuf_putc(bc, OP_put_loc);
                    dbuf_put_u16(bc, var_idx);
                }
            }
        }
        done_hoisted_def:
        JS_FreeAtom(ctx, hf->var_name);
    }
    js_free(ctx, s->hoisted_def);
    s->hoisted_def = NULL;
    s->hoisted_def_count = 0;
    s->hoisted_def_size = 0;
}

static int skip_dead_code(JSFunctionDef *s, const uint8_t *bc_buf, int bc_len,
                          int pos, int *linep)
{
    int op, len, label;

    for (; pos < bc_len; pos += len) {
        op = bc_buf[pos];
        len = opcode_info[op].size;
        if (op == OP_line_num) {
            *linep = get_u32(bc_buf + pos + 1);
        } else
        if (op == OP_label) {
            label = get_u32(bc_buf + pos + 1);
            if (update_label(s, label, 0) > 0)
                break;
#if 0
            if (s->label_slots[label].first_reloc) {
                printf("line %d: unreferenced label %d:%d has relocations\n",
                       *linep, label, s->label_slots[label].pos2);
            }
#endif
            assert(s->label_slots[label].first_reloc == NULL);
        } else {
            /* XXX: output a warning for unreachable code? */
            JSAtom atom;
            switch(opcode_info[op].fmt) {
                case OP_FMT_label:
                case OP_FMT_label_u16:
                    label = get_u32(bc_buf + pos + 1);
                    update_label(s, label, -1);
                    break;
                case OP_FMT_atom_label_u8:
                case OP_FMT_atom_label_u16:
                    label = get_u32(bc_buf + pos + 5);
                    update_label(s, label, -1);
                    /* fall thru */
                case OP_FMT_atom:
                case OP_FMT_atom_u8:
                case OP_FMT_atom_u16:
                    atom = get_u32(bc_buf + pos + 1);
                    JS_FreeAtom(s->ctx, atom);
                    break;
                default:
                    break;
            }
        }
    }
    return pos;
}

static int get_label_pos(JSFunctionDef *s, int label)
{
    int i, pos;
    for (i = 0; i < 20; i++) {
        pos = s->label_slots[label].pos;
        for (;;) {
            switch (s->byte_code.buf[pos]) {
                case OP_line_num:
                case OP_label:
                    pos += 5;
                    continue;
                case OP_goto:
                    label = get_u32(s->byte_code.buf + pos + 1);
                    break;
                default:
                    return pos;
            }
            break;
        }
    }
    return pos;
}

/* convert global variable accesses to local variables or closure
   variables when necessary */
static __exception int resolve_variables(JSContext *ctx, JSFunctionDef *s)
{
    int pos, pos_next, bc_len, op, len, i, idx, arg_valid, line_num;
    uint8_t *bc_buf;
    JSAtom var_name;
    DynBuf bc_out;
    CodeContext cc;
    int scope;

    cc.bc_buf = bc_buf = s->byte_code.buf;
    cc.bc_len = bc_len = s->byte_code.size;
    js_dbuf_init(ctx, &bc_out);

    /* first pass for runtime checks (must be done before the
       variables are created) */
    if (s->is_global_var) {
        for(i = 0; i < s->hoisted_def_count; i++) {
            JSHoistedDef *hf = &s->hoisted_def[i];
            int flags;

            if (hf->var_name != JS_ATOM_NULL) {
                /* check if global variable (XXX: simplify) */
                for(idx = 0; idx < s->closure_var_count; idx++) {
                    JSClosureVar *cv = &s->closure_var[idx];
                    if (cv->var_name == hf->var_name) {
                        if (s->eval_type == JS_EVAL_TYPE_DIRECT &&
                            cv->is_lexical) {
                            /* Check if a lexical variable is
                               redefined as 'var'. XXX: Could abort
                               compilation here, but for consistency
                               with the other checks, we delay the
                               error generation. */
                            dbuf_putc(&bc_out, OP_throw_var);
                            dbuf_put_u32(&bc_out, JS_DupAtom(ctx, hf->var_name));
                            dbuf_putc(&bc_out, JS_THROW_VAR_REDECL);
                        }
                        goto next;
                    }
                    if (cv->var_name == JS_ATOM__var_)
                        goto next;
                }

                dbuf_putc(&bc_out, OP_check_define_var);
                dbuf_put_u32(&bc_out, JS_DupAtom(ctx, hf->var_name));
                flags = 0;
                if (hf->is_lexical)
                    flags |= DEFINE_GLOBAL_LEX_VAR;
                if (hf->cpool_idx >= 0)
                    flags |= DEFINE_GLOBAL_FUNC_VAR;
                dbuf_putc(&bc_out, flags);
            }
            next: ;
        }
    }

    arg_valid = 0;
    line_num = 0; /* avoid warning */
    for (pos = 0; pos < bc_len; pos = pos_next) {
        op = bc_buf[pos];
        len = opcode_info[op].size;
        pos_next = pos + len;
        switch(op) {
            case OP_line_num:
                line_num = get_u32(bc_buf + pos + 1);
                s->line_number_size++;
                goto no_change;

            case OP_set_arg_valid_upto:
                arg_valid = get_u16(bc_buf + pos + 1);
                break;
            case OP_eval: /* convert scope index to adjusted variable index */
            {
                int call_argc = get_u16(bc_buf + pos + 1);
                scope = get_u16(bc_buf + pos + 1 + 2);
                mark_eval_captured_variables(ctx, s, scope);
                dbuf_putc(&bc_out, op);
                dbuf_put_u16(&bc_out, call_argc);
                dbuf_put_u16(&bc_out, s->scopes[scope].first + 1);
            }
                break;
            case OP_apply_eval: /* convert scope index to adjusted variable index */
                scope = get_u16(bc_buf + pos + 1);
                mark_eval_captured_variables(ctx, s, scope);
                dbuf_putc(&bc_out, op);
                dbuf_put_u16(&bc_out, s->scopes[scope].first + 1);
                break;
            case OP_scope_get_var_undef:
            case OP_scope_get_var:
            case OP_scope_put_var:
            case OP_scope_delete_var:
            case OP_scope_get_ref:
            case OP_scope_put_var_init:
                var_name = get_u32(bc_buf + pos + 1);
                scope = get_u16(bc_buf + pos + 5);
                pos_next = resolve_scope_var(ctx, s, var_name, scope, op, &bc_out,
                                             NULL, NULL, pos_next, arg_valid);
                JS_FreeAtom(ctx, var_name);
                break;
            case OP_scope_make_ref:
            {
                int label;
                LabelSlot *ls;
                var_name = get_u32(bc_buf + pos + 1);
                label = get_u32(bc_buf + pos + 5);
                scope = get_u16(bc_buf + pos + 9);
                ls = &s->label_slots[label];
                ls->ref_count--;  /* always remove label reference */
                pos_next = resolve_scope_var(ctx, s, var_name, scope, op, &bc_out,
                                             bc_buf, ls, pos_next, arg_valid);
                JS_FreeAtom(ctx, var_name);
            }
                break;
            case OP_scope_get_private_field:
            case OP_scope_get_private_field2:
            case OP_scope_put_private_field:
            {
                int ret;
                var_name = get_u32(bc_buf + pos + 1);
                scope = get_u16(bc_buf + pos + 5);
                ret = resolve_scope_private_field(ctx, s, var_name, scope, op, &bc_out);
                if (ret < 0)
                    goto fail;
                JS_FreeAtom(ctx, var_name);
            }
                break;
            case OP_gosub:
                s->jump_size++;
                if (OPTIMIZE) {
                    /* remove calls to empty finalizers  */
                    int label;
                    LabelSlot *ls;

                    label = get_u32(bc_buf + pos + 1);
                    assert(label >= 0 && label < s->label_count);
                    ls = &s->label_slots[label];
                    if (code_match(&cc, ls->pos, OP_ret, -1)) {
                        ls->ref_count--;
                        break;
                    }
                }
                goto no_change;
            case OP_drop:
                if (0) {
                    /* remove drops before return_undef */
                    /* do not perform this optimization in pass2 because
                   it breaks patterns recognised in resolve_labels */
                    int pos1 = pos_next;
                    int line1 = line_num;
                    while (code_match(&cc, pos1, OP_drop, -1)) {
                        if (cc.line_num >= 0) line1 = cc.line_num;
                        pos1 = cc.pos;
                    }
                    if (code_match(&cc, pos1, OP_return_undef, -1)) {
                        pos_next = pos1;
                        if (line1 != -1 && line1 != line_num) {
                            line_num = line1;
                            s->line_number_size++;
                            dbuf_putc(&bc_out, OP_line_num);
                            dbuf_put_u32(&bc_out, line_num);
                        }
                        break;
                    }
                }
                goto no_change;
            case OP_insert3:
                if (OPTIMIZE) {
                    /* Transformation: insert3 put_array_el|put_ref_value drop -> put_array_el|put_ref_value */
                    if (code_match(&cc, pos_next, M2(OP_put_array_el, OP_put_ref_value), OP_drop, -1)) {
                        dbuf_putc(&bc_out, cc.op);
                        pos_next = cc.pos;
                        if (cc.line_num != -1 && cc.line_num != line_num) {
                            line_num = cc.line_num;
                            s->line_number_size++;
                            dbuf_putc(&bc_out, OP_line_num);
                            dbuf_put_u32(&bc_out, line_num);
                        }
                        break;
                    }
                }
                goto no_change;

            case OP_goto:
                s->jump_size++;
                /* fall thru */
            case OP_tail_call:
            case OP_tail_call_method:
            case OP_return:
            case OP_return_undef:
            case OP_throw:
            case OP_throw_var:
            case OP_ret:
                if (OPTIMIZE) {
                    /* remove dead code */
                    int line = -1;
                    dbuf_put(&bc_out, bc_buf + pos, len);
                    pos = skip_dead_code(s, bc_buf, bc_len, pos + len, &line);
                    pos_next = pos;
                    if (pos < bc_len && line >= 0 && line_num != line) {
                        line_num = line;
                        s->line_number_size++;
                        dbuf_putc(&bc_out, OP_line_num);
                        dbuf_put_u32(&bc_out, line_num);
                    }
                    break;
                }
                goto no_change;

            case OP_label:
            {
                int label;
                LabelSlot *ls;

                label = get_u32(bc_buf + pos + 1);
                assert(label >= 0 && label < s->label_count);
                ls = &s->label_slots[label];
                ls->pos2 = bc_out.size + opcode_info[op].size;
            }
                goto no_change;

            case OP_enter_scope:
            {
                int scope_idx, scope = get_u16(bc_buf + pos + 1);

                if (scope == 1) {
                    instantiate_hoisted_definitions(ctx, s, &bc_out);
                }

                for(scope_idx = s->scopes[scope].first; scope_idx >= 0;) {
                    JSVarDef *vd = &s->vars[scope_idx];
                    if (vd->scope_level == scope) {
                        if (vd->var_kind == JS_VAR_FUNCTION_DECL ||
                            vd->var_kind == JS_VAR_NEW_FUNCTION_DECL) {
                            /* Initialize lexical variable upon entering scope */
                            dbuf_putc(&bc_out, OP_fclosure);
                            dbuf_put_u32(&bc_out, vd->func_pool_or_scope_idx);
                            dbuf_putc(&bc_out, OP_put_loc);
                            dbuf_put_u16(&bc_out, scope_idx);
                        } else {
                            /* XXX: should check if variable can be used
                               before initialization */
                            dbuf_putc(&bc_out, OP_set_loc_uninitialized);
                            dbuf_put_u16(&bc_out, scope_idx);
                        }
                        scope_idx = vd->scope_next;
                    } else {
                        break;
                    }
                }
            }
                break;

            case OP_leave_scope:
            {
                int scope_idx, scope = get_u16(bc_buf + pos + 1);

                for(scope_idx = s->scopes[scope].first; scope_idx >= 0;) {
                    JSVarDef *vd = &s->vars[scope_idx];
                    if (vd->scope_level == scope) {
                        if (vd->is_captured) {
                            dbuf_putc(&bc_out, OP_close_loc);
                            dbuf_put_u16(&bc_out, scope_idx);
                        }
                        scope_idx = vd->scope_next;
                    } else {
                        break;
                    }
                }
            }
                break;

            case OP_set_name:
            {
                /* remove dummy set_name opcodes */
                JSAtom name = get_u32(bc_buf + pos + 1);
                if (name == JS_ATOM_NULL)
                    break;
            }
                goto no_change;

            case OP_if_false:
            case OP_if_true:
            case OP_catch:
                s->jump_size++;
                goto no_change;

            case OP_dup:
                if (OPTIMIZE) {
                    /* Transformation: dup if_false(l1) drop, l1: if_false(l2) -> if_false(l2) */
                    /* Transformation: dup if_true(l1) drop, l1: if_true(l2) -> if_true(l2) */
                    if (code_match(&cc, pos_next, M2(OP_if_false, OP_if_true), OP_drop, -1)) {
                        int lab0, lab1, op1, pos1, line1, pos2;
                        lab0 = lab1 = cc.label;
                        assert(lab1 >= 0 && lab1 < s->label_count);
                        op1 = cc.op;
                        pos1 = cc.pos;
                        line1 = cc.line_num;
                        while (code_match(&cc, (pos2 = get_label_pos(s, lab1)), OP_dup, op1, OP_drop, -1)) {
                            lab1 = cc.label;
                        }
                        if (code_match(&cc, pos2, op1, -1)) {
                            s->jump_size++;
                            update_label(s, lab0, -1);
                            update_label(s, cc.label, +1);
                            dbuf_putc(&bc_out, op1);
                            dbuf_put_u32(&bc_out, cc.label);
                            pos_next = pos1;
                            if (line1 != -1 && line1 != line_num) {
                                line_num = line1;
                                s->line_number_size++;
                                dbuf_putc(&bc_out, OP_line_num);
                                dbuf_put_u32(&bc_out, line_num);
                            }
                            break;
                        }
                    }
                }
                goto no_change;

            case OP_nop:
                /* remove erased code */
                break;
            case OP_set_class_name:
                /* only used during parsing */
                break;

            default:
            no_change:
                dbuf_put(&bc_out, bc_buf + pos, len);
                break;
        }
    }

    /* set the new byte code */
    dbuf_free(&s->byte_code);
    s->byte_code = bc_out;
    if (dbuf_error(&s->byte_code)) {
        JS_ThrowOutOfMemory(ctx);
        return -1;
    }
    return 0;
    fail:
    /* continue the copy to keep the atom refcounts consistent */
    /* XXX: find a better solution ? */
    for (; pos < bc_len; pos = pos_next) {
        op = bc_buf[pos];
        len = opcode_info[op].size;
        pos_next = pos + len;
        dbuf_put(&bc_out, bc_buf + pos, len);
    }
    dbuf_free(&s->byte_code);
    s->byte_code = bc_out;
    return -1;
}

/* the pc2line table gives a line number for each PC value */
static void add_pc2line_info(JSFunctionDef *s, uint32_t pc, int line_num)
{
    if (s->line_number_slots != NULL
        &&  s->line_number_count < s->line_number_size
        &&  pc >= s->line_number_last_pc
        &&  line_num != s->line_number_last) {
        s->line_number_slots[s->line_number_count].pc = pc;
        s->line_number_slots[s->line_number_count].line_num = line_num;
        s->line_number_count++;
        s->line_number_last_pc = pc;
        s->line_number_last = line_num;
    }
}

static void compute_pc2line_info(JSFunctionDef *s)
{
    if (!(s->js_mode & JS_MODE_STRIP) && s->line_number_slots) {
        int last_line_num = s->line_num;
        uint32_t last_pc = 0;
        int i;

        js_dbuf_init(s->ctx, &s->pc2line);
        for (i = 0; i < s->line_number_count; i++) {
            uint32_t pc = s->line_number_slots[i].pc;
            int line_num = s->line_number_slots[i].line_num;
            int diff_pc, diff_line;

            if (line_num < 0)
                continue;

            diff_pc = pc - last_pc;
            diff_line = line_num - last_line_num;
            if (diff_line == 0 || diff_pc < 0)
                continue;

            if (diff_line >= PC2LINE_BASE &&
                diff_line < PC2LINE_BASE + PC2LINE_RANGE &&
                diff_pc <= PC2LINE_DIFF_PC_MAX) {
                dbuf_putc(&s->pc2line, (diff_line - PC2LINE_BASE) +
                                       diff_pc * PC2LINE_RANGE + PC2LINE_OP_FIRST);
            } else {
                /* longer encoding */
                dbuf_putc(&s->pc2line, 0);
                dbuf_put_leb128(&s->pc2line, diff_pc);
                dbuf_put_sleb128(&s->pc2line, diff_line);
            }
            last_pc = pc;
            last_line_num = line_num;
        }
    }
}

static RelocEntry *add_reloc(JSContext *ctx, LabelSlot *ls, uint32_t addr, int size)
{
    RelocEntry *re;
    re = js_malloc(ctx, sizeof(*re));
    if (!re)
        return NULL;
    re->addr = addr;
    re->size = size;
    re->next = ls->first_reloc;
    ls->first_reloc = re;
    return re;
}

static BOOL code_has_label(CodeContext *s, int pos, int label)
{
    while (pos < s->bc_len) {
        int op = s->bc_buf[pos];
        if (op == OP_line_num) {
            pos += 5;
            continue;
        }
        if (op == OP_label) {
            int lab = get_u32(s->bc_buf + pos + 1);
            if (lab == label)
                return TRUE;
            pos += 5;
            continue;
        }
        if (op == OP_goto) {
            int lab = get_u32(s->bc_buf + pos + 1);
            if (lab == label)
                return TRUE;
        }
        break;
    }
    return FALSE;
}

/* return the target label, following the OP_goto jumps
   the first opcode at destination is stored in *pop
 */
static int find_jump_target(JSFunctionDef *s, int label, int *pop, int *pline)
{
    int i, pos, op;

    update_label(s, label, -1);
    for (i = 0; i < 10; i++) {
        assert(label >= 0 && label < s->label_count);
        pos = s->label_slots[label].pos2;
        for (;;) {
            switch(op = s->byte_code.buf[pos]) {
                case OP_line_num:
                    if (pline)
                        *pline = get_u32(s->byte_code.buf + pos + 1);
                    /* fall thru */
                case OP_label:
                    pos += opcode_info[op].size;
                    continue;
                case OP_goto:
                    label = get_u32(s->byte_code.buf + pos + 1);
                    break;
                case OP_drop:
                    /* ignore drop opcodes if followed by OP_return_undef */
                    while (s->byte_code.buf[++pos] == OP_drop)
                        continue;
                    if (s->byte_code.buf[pos] == OP_return_undef)
                        op = OP_return_undef;
                    /* fall thru */
                default:
                    goto done;
            }
            break;
        }
    }
    /* cycle detected, could issue a warning */
    done:
    *pop = op;
    update_label(s, label, +1);
    return label;
}

static void push_short_int(DynBuf *bc_out, int val)
{
#if SHORT_OPCODES
    if (val >= -1 && val <= 7) {
        dbuf_putc(bc_out, OP_push_0 + val);
        return;
    }
    if (val == (int8_t)val) {
        dbuf_putc(bc_out, OP_push_i8);
        dbuf_putc(bc_out, val);
        return;
    }
    if (val == (int16_t)val) {
        dbuf_putc(bc_out, OP_push_i16);
        dbuf_put_u16(bc_out, val);
        return;
    }
#endif
    dbuf_putc(bc_out, OP_push_i32);
    dbuf_put_u32(bc_out, val);
}

static void put_short_code(DynBuf *bc_out, int op, int idx)
{
#if SHORT_OPCODES
    if (idx < 4) {
        switch (op) {
        case OP_get_loc:
            dbuf_putc(bc_out, OP_get_loc0 + idx);
            return;
        case OP_put_loc:
            dbuf_putc(bc_out, OP_put_loc0 + idx);
            return;
        case OP_set_loc:
            dbuf_putc(bc_out, OP_set_loc0 + idx);
            return;
        case OP_get_arg:
            dbuf_putc(bc_out, OP_get_arg0 + idx);
            return;
        case OP_put_arg:
            dbuf_putc(bc_out, OP_put_arg0 + idx);
            return;
        case OP_set_arg:
            dbuf_putc(bc_out, OP_set_arg0 + idx);
            return;
        case OP_get_var_ref:
            dbuf_putc(bc_out, OP_get_var_ref0 + idx);
            return;
        case OP_put_var_ref:
            dbuf_putc(bc_out, OP_put_var_ref0 + idx);
            return;
        case OP_set_var_ref:
            dbuf_putc(bc_out, OP_set_var_ref0 + idx);
            return;
        case OP_call:
            dbuf_putc(bc_out, OP_call0 + idx);
            return;
        }
    }
    if (idx < 256) {
        switch (op) {
        case OP_get_loc:
            dbuf_putc(bc_out, OP_get_loc8);
            dbuf_putc(bc_out, idx);
            return;
        case OP_put_loc:
            dbuf_putc(bc_out, OP_put_loc8);
            dbuf_putc(bc_out, idx);
            return;
        case OP_set_loc:
            dbuf_putc(bc_out, OP_set_loc8);
            dbuf_putc(bc_out, idx);
            return;
        }
    }
#endif
    dbuf_putc(bc_out, op);
    dbuf_put_u16(bc_out, idx);
}

/* peephole optimizations and resolve goto/labels */
static __exception int resolve_labels(JSContext *ctx, JSFunctionDef *s)
{
    int pos, pos_next, bc_len, op, op1, len, i, line_num;
    const uint8_t *bc_buf;
    DynBuf bc_out;
    LabelSlot *label_slots, *ls;
    RelocEntry *re, *re_next;
    CodeContext cc;
    int label;
#if SHORT_OPCODES
    JumpSlot *jp;
#endif

    label_slots = s->label_slots;

    line_num = s->line_num;

    cc.bc_buf = bc_buf = s->byte_code.buf;
    cc.bc_len = bc_len = s->byte_code.size;
    js_dbuf_init(ctx, &bc_out);

#if SHORT_OPCODES
    if (s->jump_size) {
        s->jump_slots = js_mallocz(s->ctx, sizeof(*s->jump_slots) * s->jump_size);
        if (s->jump_slots == NULL)
            return -1;
    }
#endif
    /* XXX: Should skip this phase if not generating SHORT_OPCODES */
    if (s->line_number_size && !(s->js_mode & JS_MODE_STRIP)) {
        s->line_number_slots = js_mallocz(s->ctx, sizeof(*s->line_number_slots) * s->line_number_size);
        if (s->line_number_slots == NULL)
            return -1;
        s->line_number_last = s->line_num;
        s->line_number_last_pc = 0;
    }

    /* initialize the 'home_object' variable if needed */
    if (s->home_object_var_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_HOME_OBJECT);
        put_short_code(&bc_out, OP_put_loc, s->home_object_var_idx);
    }
    /* initialize the 'this.active_func' variable if needed */
    if (s->this_active_func_var_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_THIS_FUNC);
        put_short_code(&bc_out, OP_put_loc, s->this_active_func_var_idx);
    }
    /* initialize the 'new.target' variable if needed */
    if (s->new_target_var_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_NEW_TARGET);
        put_short_code(&bc_out, OP_put_loc, s->new_target_var_idx);
    }
    /* initialize the 'this' variable if needed. In a derived class
       constructor, this is initially uninitialized. */
    if (s->this_var_idx >= 0) {
        if (s->is_derived_class_constructor) {
            dbuf_putc(&bc_out, OP_set_loc_uninitialized);
            dbuf_put_u16(&bc_out, s->this_var_idx);
        } else {
            dbuf_putc(&bc_out, OP_push_this);
            put_short_code(&bc_out, OP_put_loc, s->this_var_idx);
        }
    }
    /* initialize the 'arguments' variable if needed */
    if (s->arguments_var_idx >= 0) {
        if ((s->js_mode & JS_MODE_STRICT) || !s->has_simple_parameter_list) {
            dbuf_putc(&bc_out, OP_special_object);
            dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_ARGUMENTS);
        } else {
            dbuf_putc(&bc_out, OP_special_object);
            dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_MAPPED_ARGUMENTS);
        }
        put_short_code(&bc_out, OP_put_loc, s->arguments_var_idx);
    }
    /* initialize a reference to the current function if needed */
    if (s->func_var_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_THIS_FUNC);
        put_short_code(&bc_out, OP_put_loc, s->func_var_idx);
    }
    /* initialize the variable environment object if needed */
    if (s->var_object_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_VAR_OBJECT);
        put_short_code(&bc_out, OP_put_loc, s->var_object_idx);
    }

    for (pos = 0; pos < bc_len; pos = pos_next) {
        int val;
        op = bc_buf[pos];
        len = opcode_info[op].size;
        pos_next = pos + len;
        switch(op) {
            case OP_line_num:
                /* line number info (for debug). We put it in a separate
               compressed table to reduce memory usage and get better
               performance */
                line_num = get_u32(bc_buf + pos + 1);
                break;

            case OP_label:
            {
                label = get_u32(bc_buf + pos + 1);
                assert(label >= 0 && label < s->label_count);
                ls = &label_slots[label];
                assert(ls->addr == -1);
                ls->addr = bc_out.size;
                /* resolve the relocation entries */
                for(re = ls->first_reloc; re != NULL; re = re_next) {
                    int diff = ls->addr - re->addr;
                    re_next = re->next;
                    switch (re->size) {
                        case 4:
                            put_u32(bc_out.buf + re->addr, diff);
                            break;
                        case 2:
                            assert(diff == (int16_t)diff);
                            put_u16(bc_out.buf + re->addr, diff);
                            break;
                        case 1:
                            assert(diff == (int8_t)diff);
                            put_u8(bc_out.buf + re->addr, diff);
                            break;
                    }
                    js_free(ctx, re);
                }
                ls->first_reloc = NULL;
            }
                break;

            case OP_call:
            case OP_call_method:
            {
                /* detect and transform tail calls */
                int argc;
                argc = get_u16(bc_buf + pos + 1);
                if (code_match(&cc, pos_next, OP_return, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    put_short_code(&bc_out, op + 1, argc);
                    pos_next = skip_dead_code(s, bc_buf, bc_len, cc.pos, &line_num);
                    break;
                }
                add_pc2line_info(s, bc_out.size, line_num);
                put_short_code(&bc_out, op, argc);
                break;
            }
                goto no_change;

            case OP_return:
            case OP_return_undef:
            case OP_return_async:
            case OP_throw:
            case OP_throw_var:
                pos_next = skip_dead_code(s, bc_buf, bc_len, pos_next, &line_num);
                goto no_change;

            case OP_goto:
                label = get_u32(bc_buf + pos + 1);
            has_goto:
                if (OPTIMIZE) {
                    int line1 = -1;
                    /* Use custom matcher because multiple labels can follow */
                    label = find_jump_target(s, label, &op1, &line1);
                    if (code_has_label(&cc, pos_next, label)) {
                        /* jump to next instruction: remove jump */
                        update_label(s, label, -1);
                        break;
                    }
                    if (op1 == OP_return || op1 == OP_return_undef || op1 == OP_throw) {
                        /* jump to return/throw: remove jump, append return/throw */
                        /* updating the line number obfuscates assembly listing */
                        //if (line1 >= 0) line_num = line1;
                        update_label(s, label, -1);
                        add_pc2line_info(s, bc_out.size, line_num);
                        dbuf_putc(&bc_out, op1);
                        pos_next = skip_dead_code(s, bc_buf, bc_len, pos_next, &line_num);
                        break;
                    }
                    /* XXX: should duplicate single instructions followed by goto or return */
                    /* For example, can match one of these followed by return:
                   push_i32 / push_const / push_atom_value / get_var /
                   undefined / null / push_false / push_true / get_ref_value /
                   get_loc / get_arg / get_var_ref
                 */
                }
                goto has_label;

            case OP_gosub:
                label = get_u32(bc_buf + pos + 1);
                if (0 && OPTIMIZE) {
                    label = find_jump_target(s, label, &op1, NULL);
                    if (op1 == OP_ret) {
                        update_label(s, label, -1);
                        /* empty finally clause: remove gosub */
                        break;
                    }
                }
                goto has_label;

            case OP_catch:
                label = get_u32(bc_buf + pos + 1);
                goto has_label;

            case OP_if_true:
            case OP_if_false:
                label = get_u32(bc_buf + pos + 1);
                if (OPTIMIZE) {
                    label = find_jump_target(s, label, &op1, NULL);
                    /* transform if_false/if_true(l1) label(l1) -> drop label(l1) */
                    if (code_has_label(&cc, pos_next, label)) {
                        update_label(s, label, -1);
                        dbuf_putc(&bc_out, OP_drop);
                        break;
                    }
                    /* transform if_false(l1) goto(l2) label(l1) -> if_false(l2) label(l1) */
                    if (code_match(&cc, pos_next, OP_goto, -1)) {
                        int pos1 = cc.pos;
                        int line1 = cc.line_num;
                        if (code_has_label(&cc, pos1, label)) {
                            if (line1 >= 0) line_num = line1;
                            pos_next = pos1;
                            update_label(s, label, -1);
                            label = cc.label;
                            op ^= OP_if_true ^ OP_if_false;
                        }
                    }
                }
            has_label:
                add_pc2line_info(s, bc_out.size, line_num);
                if (op == OP_goto) {
                    pos_next = skip_dead_code(s, bc_buf, bc_len, pos_next, &line_num);
                }
                assert(label >= 0 && label < s->label_count);
                ls = &label_slots[label];
#if SHORT_OPCODES
                jp = &s->jump_slots[s->jump_count++];
            jp->op = op;
            jp->size = 4;
            jp->pos = bc_out.size + 1;
            jp->label = label;

            if (ls->addr == -1) {
                int diff = ls->pos2 - pos - 1;
                if (diff < 128 && (op == OP_if_false || op == OP_if_true || op == OP_goto)) {
                    jp->size = 1;
                    jp->op = OP_if_false8 + (op - OP_if_false);
                    dbuf_putc(&bc_out, OP_if_false8 + (op - OP_if_false));
                    dbuf_putc(&bc_out, 0);
                    if (!add_reloc(ctx, ls, bc_out.size - 1, 1))
                        goto fail;
                    break;
                }
                if (diff < 32768 && op == OP_goto) {
                    jp->size = 2;
                    jp->op = OP_goto16;
                    dbuf_putc(&bc_out, OP_goto16);
                    dbuf_put_u16(&bc_out, 0);
                    if (!add_reloc(ctx, ls, bc_out.size - 2, 2))
                        goto fail;
                    break;
                }
            } else {
                int diff = ls->addr - bc_out.size - 1;
                if (diff == (int8_t)diff && (op == OP_if_false || op == OP_if_true || op == OP_goto)) {
                    jp->size = 1;
                    jp->op = OP_if_false8 + (op - OP_if_false);
                    dbuf_putc(&bc_out, OP_if_false8 + (op - OP_if_false));
                    dbuf_putc(&bc_out, diff);
                    break;
                }
                if (diff == (int16_t)diff && op == OP_goto) {
                    jp->size = 2;
                    jp->op = OP_goto16;
                    dbuf_putc(&bc_out, OP_goto16);
                    dbuf_put_u16(&bc_out, diff);
                    break;
                }
            }
#endif
                dbuf_putc(&bc_out, op);
                dbuf_put_u32(&bc_out, ls->addr - bc_out.size);
                if (ls->addr == -1) {
                    /* unresolved yet: create a new relocation entry */
                    if (!add_reloc(ctx, ls, bc_out.size - 4, 4))
                        goto fail;
                }
                break;
            case OP_with_get_var:
            case OP_with_put_var:
            case OP_with_delete_var:
            case OP_with_make_ref:
            case OP_with_get_ref:
            case OP_with_get_ref_undef:
            {
                JSAtom atom;
                int is_with;

                atom = get_u32(bc_buf + pos + 1);
                label = get_u32(bc_buf + pos + 5);
                is_with = bc_buf[pos + 9];
                if (OPTIMIZE) {
                    label = find_jump_target(s, label, &op1, NULL);
                }
                assert(label >= 0 && label < s->label_count);
                ls = &label_slots[label];
                add_pc2line_info(s, bc_out.size, line_num);
#if SHORT_OPCODES
                jp = &s->jump_slots[s->jump_count++];
                jp->op = op;
                jp->size = 4;
                jp->pos = bc_out.size + 5;
                jp->label = label;
#endif
                dbuf_putc(&bc_out, op);
                dbuf_put_u32(&bc_out, atom);
                dbuf_put_u32(&bc_out, ls->addr - bc_out.size);
                if (ls->addr == -1) {
                    /* unresolved yet: create a new relocation entry */
                    if (!add_reloc(ctx, ls, bc_out.size - 4, 4))
                        goto fail;
                }
                dbuf_putc(&bc_out, is_with);
            }
                break;

            case OP_drop:
                if (OPTIMIZE) {
                    /* remove useless drops before return */
                    if (code_match(&cc, pos_next, OP_return_undef, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        break;
                    }
                }
                goto no_change;

            case OP_null:
#if SHORT_OPCODES
                if (OPTIMIZE) {
                /* transform null strict_eq into is_null */
                if (code_match(&cc, pos_next, OP_strict_eq, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_is_null);
                    pos_next = cc.pos;
                    break;
                }
                /* transform null strict_neq if_false/if_true -> is_null if_true/if_false */
                if (code_match(&cc, pos_next, OP_strict_neq, M2(OP_if_false, OP_if_true), -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_is_null);
                    pos_next = cc.pos;
                    label = cc.label;
                    op = cc.op ^ OP_if_false ^ OP_if_true;
                    goto has_label;
                }
            }
#endif
                /* fall thru */
            case OP_push_false:
            case OP_push_true:
                if (OPTIMIZE) {
                    val = (op == OP_push_true);
                    if (code_match(&cc, pos_next, M2(OP_if_false, OP_if_true), -1)) {
                        has_constant_test:
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        if (val == cc.op - OP_if_false) {
                            /* transform null if_false(l1) -> goto l1 */
                            /* transform false if_false(l1) -> goto l1 */
                            /* transform true if_true(l1) -> goto l1 */
                            pos_next = cc.pos;
                            op = OP_goto;
                            label = cc.label;
                            goto has_goto;
                        } else {
                            /* transform null if_true(l1) -> nop */
                            /* transform false if_true(l1) -> nop */
                            /* transform true if_false(l1) -> nop */
                            pos_next = cc.pos;
                            update_label(s, cc.label, -1);
                            break;
                        }
                    }
                }
                goto no_change;

            case OP_push_i32:
                if (OPTIMIZE) {
                    /* transform i32(val) neg -> i32(-val) */
                    val = get_i32(bc_buf + pos + 1);
                    if ((val != INT32_MIN && val != 0)
                        &&  code_match(&cc, pos_next, OP_neg, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        if (code_match(&cc, cc.pos, OP_drop, -1)) {
                            if (cc.line_num >= 0) line_num = cc.line_num;
                        } else {
                            add_pc2line_info(s, bc_out.size, line_num);
                            push_short_int(&bc_out, -val);
                        }
                        pos_next = cc.pos;
                        break;
                    }
                    /* remove push/drop pairs generated by the parser */
                    if (code_match(&cc, pos_next, OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        pos_next = cc.pos;
                        break;
                    }
                    /* Optimize constant tests: `if (0)`, `if (1)`, `if (!0)`... */
                    if (code_match(&cc, pos_next, M2(OP_if_false, OP_if_true), -1)) {
                        val = (val != 0);
                        goto has_constant_test;
                    }
                    add_pc2line_info(s, bc_out.size, line_num);
                    push_short_int(&bc_out, val);
                    break;
                }
                goto no_change;

#if SHORT_OPCODES
                case OP_push_const:
        case OP_fclosure:
            if (OPTIMIZE) {
                int idx = get_u32(bc_buf + pos + 1);
                if (idx < 256) {
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_push_const8 + op - OP_push_const);
                    dbuf_putc(&bc_out, idx);
                    break;
                }
            }
            goto no_change;

        case OP_get_field:
            if (OPTIMIZE) {
                JSAtom atom = get_u32(bc_buf + pos + 1);
                if (atom == JS_ATOM_length) {
                    JS_FreeAtom(ctx, atom);
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_get_length);
                    break;
                }
            }
            goto no_change;
#endif
            case OP_push_atom_value:
                if (OPTIMIZE) {
                    JSAtom atom = get_u32(bc_buf + pos + 1);
                    /* remove push/drop pairs generated by the parser */
                    if (code_match(&cc, pos_next, OP_drop, -1)) {
                        JS_FreeAtom(ctx, atom);
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        pos_next = cc.pos;
                        break;
                    }
#if SHORT_OPCODES
                    if (atom == JS_ATOM_empty_string) {
                    JS_FreeAtom(ctx, atom);
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_push_empty_string);
                    break;
                }
#endif
                }
                goto no_change;

            case OP_to_propkey:
            case OP_to_propkey2:
                if (OPTIMIZE) {
                    /* remove redundant to_propkey/to_propkey2 opcodes when storing simple data */
                    if (code_match(&cc, pos_next, M3(OP_get_loc, OP_get_arg, OP_get_var_ref), -1, OP_put_array_el, -1)
                        ||  code_match(&cc, pos_next, M3(OP_push_i32, OP_push_const, OP_push_atom_value), OP_put_array_el, -1)
                        ||  code_match(&cc, pos_next, M4(OP_undefined, OP_null, OP_push_true, OP_push_false), OP_put_array_el, -1)) {
                        break;
                    }
                }
                goto no_change;

            case OP_undefined:
                if (OPTIMIZE) {
                    /* remove push/drop pairs generated by the parser */
                    if (code_match(&cc, pos_next, OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        pos_next = cc.pos;
                        break;
                    }
                    /* transform undefined return -> return_undefined */
                    if (code_match(&cc, pos_next, OP_return, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        add_pc2line_info(s, bc_out.size, line_num);
                        dbuf_putc(&bc_out, OP_return_undef);
                        pos_next = cc.pos;
                        break;
                    }
                    /* transform undefined if_true(l1)/if_false(l1) -> nop/goto(l1) */
                    if (code_match(&cc, pos_next, M2(OP_if_false, OP_if_true), -1)) {
                        val = 0;
                        goto has_constant_test;
                    }
#if SHORT_OPCODES
                    /* transform undefined strict_eq -> is_undefined */
                if (code_match(&cc, pos_next, OP_strict_eq, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_is_undefined);
                    pos_next = cc.pos;
                    break;
                }
                /* transform undefined strict_neq if_false/if_true -> is_undefined if_true/if_false */
                if (code_match(&cc, pos_next, OP_strict_neq, M2(OP_if_false, OP_if_true), -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_is_undefined);
                    pos_next = cc.pos;
                    label = cc.label;
                    op = cc.op ^ OP_if_false ^ OP_if_true;
                    goto has_label;
                }
#endif
                }
                goto no_change;

            case OP_insert2:
                if (OPTIMIZE) {
                    /* Transformation:
                   insert2 put_field(a) drop -> put_field(a)
                   insert2 put_var_strict(a) drop -> put_var_strict(a)
                */
                    if (code_match(&cc, pos_next, M2(OP_put_field, OP_put_var_strict), OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        add_pc2line_info(s, bc_out.size, line_num);
                        dbuf_putc(&bc_out, cc.op);
                        dbuf_put_u32(&bc_out, cc.atom);
                        pos_next = cc.pos;
                        break;
                    }
                }
                goto no_change;

            case OP_dup:
                if (OPTIMIZE) {
                    /* Transformation: dup put_x(n) drop -> put_x(n) */
                    int op1, line2 = -1;
                    /* Transformation: dup put_x(n) -> set_x(n) */
                    if (code_match(&cc, pos_next, M3(OP_put_loc, OP_put_arg, OP_put_var_ref), -1, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        op1 = cc.op + 1;  /* put_x -> set_x */
                        pos_next = cc.pos;
                        if (code_match(&cc, cc.pos, OP_drop, -1)) {
                            if (cc.line_num >= 0) line_num = cc.line_num;
                            op1 -= 1; /* set_x drop -> put_x */
                            pos_next = cc.pos;
                            if (code_match(&cc, cc.pos, op1 - 1, cc.idx, -1)) {
                                line2 = cc.line_num; /* delay line number update */
                                op1 += 1;   /* put_x(n) get_x(n) -> set_x(n) */
                                pos_next = cc.pos;
                            }
                        }
                        add_pc2line_info(s, bc_out.size, line_num);
                        put_short_code(&bc_out, op1, cc.idx);
                        if (line2 >= 0) line_num = line2;
                        break;
                    }
                }
                goto no_change;

            case OP_get_loc:
                if (OPTIMIZE) {
                    /* transformation:
                   get_loc(n) post_dec put_loc(n) drop -> dec_loc(n)
                   get_loc(n) post_inc put_loc(n) drop -> inc_loc(n)
                   get_loc(n) dec dup put_loc(n) drop -> dec_loc(n)
                   get_loc(n) inc dup put_loc(n) drop -> inc_loc(n)
                 */
                    int idx;
                    idx = get_u16(bc_buf + pos + 1);
                    if (idx >= 256)
                        goto no_change;
                    if (code_match(&cc, pos_next, M2(OP_post_dec, OP_post_inc), OP_put_loc, idx, OP_drop, -1) ||
                        code_match(&cc, pos_next, M2(OP_dec, OP_inc), OP_dup, OP_put_loc, idx, OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        add_pc2line_info(s, bc_out.size, line_num);
                        dbuf_putc(&bc_out, (cc.op == OP_inc || cc.op == OP_post_inc) ? OP_inc_loc : OP_dec_loc);
                        dbuf_putc(&bc_out, idx);
                        pos_next = cc.pos;
                        break;
                    }
                    /* transformation:
                   get_loc(n) push_atom_value(x) add dup put_loc(n) drop -> push_atom_value(x) add_loc(n)
                 */
                    if (code_match(&cc, pos_next, OP_push_atom_value, OP_add, OP_dup, OP_put_loc, idx, OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        add_pc2line_info(s, bc_out.size, line_num);
#if SHORT_OPCODES
                        if (cc.atom == JS_ATOM_empty_string) {
                        JS_FreeAtom(ctx, cc.atom);
                        dbuf_putc(&bc_out, OP_push_empty_string);
                    } else
#endif
                        {
                            dbuf_putc(&bc_out, OP_push_atom_value);
                            dbuf_put_u32(&bc_out, cc.atom);
                        }
                        dbuf_putc(&bc_out, OP_add_loc);
                        dbuf_putc(&bc_out, idx);
                        pos_next = cc.pos;
                        break;
                    }
                    /* transformation:
                   get_loc(n) push_i32(x) add dup put_loc(n) drop -> push_i32(x) add_loc(n)
                 */
                    if (code_match(&cc, pos_next, OP_push_i32, OP_add, OP_dup, OP_put_loc, idx, OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        add_pc2line_info(s, bc_out.size, line_num);
                        push_short_int(&bc_out, cc.label);
                        dbuf_putc(&bc_out, OP_add_loc);
                        dbuf_putc(&bc_out, idx);
                        pos_next = cc.pos;
                        break;
                    }
                    /* transformation: XXX: also do these:
                   get_loc(n) get_loc(x) add dup put_loc(n) drop -> get_loc(x) add_loc(n)
                   get_loc(n) get_arg(x) add dup put_loc(n) drop -> get_arg(x) add_loc(n)
                   get_loc(n) get_var_ref(x) add dup put_loc(n) drop -> get_var_ref(x) add_loc(n)
                 */
                    if (code_match(&cc, pos_next, M3(OP_get_loc, OP_get_arg, OP_get_var_ref), -1, OP_add, OP_dup, OP_put_loc, idx, OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        add_pc2line_info(s, bc_out.size, line_num);
                        put_short_code(&bc_out, cc.op, cc.idx);
                        dbuf_putc(&bc_out, OP_add_loc);
                        dbuf_putc(&bc_out, idx);
                        pos_next = cc.pos;
                        break;
                    }
                    add_pc2line_info(s, bc_out.size, line_num);
                    put_short_code(&bc_out, op, idx);
                    break;
                }
                goto no_change;
#if SHORT_OPCODES
                case OP_get_arg:
        case OP_get_var_ref:
            if (OPTIMIZE) {
                int idx;
                idx = get_u16(bc_buf + pos + 1);
                add_pc2line_info(s, bc_out.size, line_num);
                put_short_code(&bc_out, op, idx);
                break;
            }
            goto no_change;
#endif
            case OP_put_loc:
            case OP_put_arg:
            case OP_put_var_ref:
                if (OPTIMIZE) {
                    /* transformation: put_x(n) get_x(n) -> set_x(n) */
                    int idx;
                    idx = get_u16(bc_buf + pos + 1);
                    if (code_match(&cc, pos_next, op - 1, idx, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        add_pc2line_info(s, bc_out.size, line_num);
                        put_short_code(&bc_out, op + 1, idx);
                        pos_next = cc.pos;
                        break;
                    }
                    add_pc2line_info(s, bc_out.size, line_num);
                    put_short_code(&bc_out, op, idx);
                    break;
                }
                goto no_change;

            case OP_post_inc:
            case OP_post_dec:
                if (OPTIMIZE) {
                    /* transformation:
                   post_inc put_x drop -> inc put_x
                   post_inc perm3 put_field drop -> inc put_field
                   post_inc perm3 put_var_strict drop -> inc put_var_strict
                   post_inc perm4 put_array_el drop -> inc put_array_el
                 */
                    int op1, idx;
                    if (code_match(&cc, pos_next, M3(OP_put_loc, OP_put_arg, OP_put_var_ref), -1, OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        op1 = cc.op;
                        idx = cc.idx;
                        pos_next = cc.pos;
                        if (code_match(&cc, cc.pos, op1 - 1, idx, -1)) {
                            if (cc.line_num >= 0) line_num = cc.line_num;
                            op1 += 1;   /* put_x(n) get_x(n) -> set_x(n) */
                            pos_next = cc.pos;
                        }
                        add_pc2line_info(s, bc_out.size, line_num);
                        dbuf_putc(&bc_out, OP_dec + (op - OP_post_dec));
                        put_short_code(&bc_out, op1, idx);
                        break;
                    }
                    if (code_match(&cc, pos_next, OP_perm3, M2(OP_put_field, OP_put_var_strict), OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        add_pc2line_info(s, bc_out.size, line_num);
                        dbuf_putc(&bc_out, OP_dec + (op - OP_post_dec));
                        dbuf_putc(&bc_out, cc.op);
                        dbuf_put_u32(&bc_out, cc.atom);
                        pos_next = cc.pos;
                        break;
                    }
                    if (code_match(&cc, pos_next, OP_perm4, OP_put_array_el, OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        add_pc2line_info(s, bc_out.size, line_num);
                        dbuf_putc(&bc_out, OP_dec + (op - OP_post_dec));
                        dbuf_putc(&bc_out, OP_put_array_el);
                        pos_next = cc.pos;
                        break;
                    }
                }
                goto no_change;

            case OP_typeof:
                if (OPTIMIZE) {
                    /* simplify typeof tests */
                    if (code_match(&cc, pos_next, OP_push_atom_value, M4(OP_strict_eq, OP_strict_neq, OP_eq, OP_neq), -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        int op1 = (cc.op == OP_strict_eq || cc.op == OP_eq) ? OP_strict_eq : OP_strict_neq;
#if SHORT_OPCODES
                        int op2 = -1;
                    switch (cc.atom) {
                    case JS_ATOM_undefined:
                        op2 = OP_is_undefined;
                        break;
                    case JS_ATOM_function:
                        op2 = OP_is_function;
                        break;
                    }
                    if (op2 >= 0) {
                        /* transform typeof(s) == "<type>" into is_<type> */
                        if (op1 == OP_strict_eq) {
                            add_pc2line_info(s, bc_out.size, line_num);
                            dbuf_putc(&bc_out, op2);
                            JS_FreeAtom(ctx, cc.atom);
                            pos_next = cc.pos;
                            break;
                        }
                        if (op1 == OP_strict_neq && code_match(&cc, cc.pos, OP_if_false, -1)) {
                            /* transform typeof(s) != "<type>" if_false into is_<type> if_true */
                            if (cc.line_num >= 0) line_num = cc.line_num;
                            add_pc2line_info(s, bc_out.size, line_num);
                            dbuf_putc(&bc_out, op2);
                            JS_FreeAtom(ctx, cc.atom);
                            pos_next = cc.pos;
                            label = cc.label;
                            op = OP_if_true;
                            goto has_label;
                        }
                    }
#endif
                        if (cc.atom == JS_ATOM_undefined) {
                            /* transform typeof(s) == "undefined" into s === void 0 */
                            add_pc2line_info(s, bc_out.size, line_num);
                            dbuf_putc(&bc_out, OP_undefined);
                            dbuf_putc(&bc_out, op1);
                            JS_FreeAtom(ctx, cc.atom);
                            pos_next = cc.pos;
                            break;
                        }
                    }
                }
                goto no_change;

            default:
            no_change:
                add_pc2line_info(s, bc_out.size, line_num);
                dbuf_put(&bc_out, bc_buf + pos, len);
                break;
        }
    }

    /* check that there were no missing labels */
    for(i = 0; i < s->label_count; i++) {
        assert(label_slots[i].first_reloc == NULL);
    }
#if SHORT_OPCODES
    if (OPTIMIZE) {
        /* more jump optimizations */
        int patch_offsets = 0;
        for (i = 0, jp = s->jump_slots; i < s->jump_count; i++, jp++) {
            LabelSlot *ls;
            JumpSlot *jp1;
            int j, pos, diff, delta;

            delta = 3;
            switch (op = jp->op) {
            case OP_goto16:
                delta = 1;
                /* fall thru */
            case OP_if_false:
            case OP_if_true:
            case OP_goto:
                pos = jp->pos;
                diff = s->label_slots[jp->label].addr - pos;
                if (diff >= -128 && diff <= 127 + delta) {
                    //put_u8(bc_out.buf + pos, diff);
                    jp->size = 1;
                    if (op == OP_goto16) {
                        bc_out.buf[pos - 1] = jp->op = OP_goto8;
                    } else {
                        bc_out.buf[pos - 1] = jp->op = OP_if_false8 + (op - OP_if_false);
                    }
                    goto shrink;
                } else
                if (diff == (int16_t)diff && op == OP_goto) {
                    //put_u16(bc_out.buf + pos, diff);
                    jp->size = 2;
                    delta = 2;
                    bc_out.buf[pos - 1] = jp->op = OP_goto16;
                shrink:
                    /* XXX: should reduce complexity, using 2 finger copy scheme */
                    memmove(bc_out.buf + pos + jp->size, bc_out.buf + pos + jp->size + delta,
                            bc_out.size - pos - jp->size - delta);
                    bc_out.size -= delta;
                    patch_offsets++;
                    for (j = 0, ls = s->label_slots; j < s->label_count; j++, ls++) {
                        if (ls->addr > pos)
                            ls->addr -= delta;
                    }
                    for (j = i + 1, jp1 = jp + 1; j < s->jump_count; j++, jp1++) {
                        if (jp1->pos > pos)
                            jp1->pos -= delta;
                    }
                    for (j = 0; j < s->line_number_count; j++) {
                        if (s->line_number_slots[j].pc > pos)
                            s->line_number_slots[j].pc -= delta;
                    }
                    continue;
                }
                break;
            }
        }
        if (patch_offsets) {
            JumpSlot *jp1;
            int j;
            for (j = 0, jp1 = s->jump_slots; j < s->jump_count; j++, jp1++) {
                int diff1 = s->label_slots[jp1->label].addr - jp1->pos;
                switch (jp1->size) {
                case 1:
                    put_u8(bc_out.buf + jp1->pos, diff1);
                    break;
                case 2:
                    put_u16(bc_out.buf + jp1->pos, diff1);
                    break;
                case 4:
                    put_u32(bc_out.buf + jp1->pos, diff1);
                    break;
                }
            }
        }
    }
    js_free(ctx, s->jump_slots);
    s->jump_slots = NULL;
#endif
    js_free(ctx, s->label_slots);
    s->label_slots = NULL;
    /* XXX: should delay until copying to runtime bytecode function */
    compute_pc2line_info(s);
    js_free(ctx, s->line_number_slots);
    s->line_number_slots = NULL;
    /* set the new byte code */
    dbuf_free(&s->byte_code);
    s->byte_code = bc_out;
    s->use_short_opcodes = TRUE;
    if (dbuf_error(&s->byte_code)) {
        JS_ThrowOutOfMemory(ctx);
        return -1;
    }
    return 0;
    fail:
    /* XXX: not safe */
    dbuf_free(&bc_out);
    return -1;
}

/* compute the maximum stack size needed by the function */
typedef struct StackSizeState {
    int stack_len_max;
    uint16_t *stack_level_tab;
} StackSizeState;

static __exception int compute_stack_size_rec(JSContext *ctx,
                                              JSFunctionDef *fd,
                                              StackSizeState *s,
                                              int pos, int op, int stack_len)
{
    int bc_len, diff, n_pop, pos_next;
    const JSOpCode *oi;
    const uint8_t *bc_buf;

    if (stack_len > s->stack_len_max) {
        s->stack_len_max = stack_len;
        if (s->stack_len_max > JS_STACK_SIZE_MAX)
            goto stack_overflow;
    }
    bc_buf = fd->byte_code.buf;
    bc_len = fd->byte_code.size;
    for(;;) {
        if ((unsigned)pos >= bc_len)
            goto buf_overflow;
#if 0
        printf("%5d: %d\n", pos, stack_len);
#endif
        if (s->stack_level_tab[pos] != 0xffff) {
            /* already explored: check that the stack size is consistent */
            if (s->stack_level_tab[pos] != stack_len) {
                JS_ThrowInternalError(ctx, "unconsistent stack size: %d %d (pc=%d)",
                                      s->stack_level_tab[pos], stack_len, pos);
                return -1;
            } else {
                return 0;
            }
        } else {
            s->stack_level_tab[pos] = stack_len;
        }

        op = bc_buf[pos];
        if (op == 0 || op >= OP_COUNT) {
            JS_ThrowInternalError(ctx, "invalid opcode (op=%d, pc=%d)", op, pos);
            return -1;
        }
        oi = &short_opcode_info(op);
        pos_next = pos + oi->size;
        if (pos_next > bc_len) {
            buf_overflow:
            JS_ThrowInternalError(ctx, "bytecode buffer overflow (op=%d, pc=%d)", op, pos);
            return -1;
        }
        n_pop = oi->n_pop;
        /* call pops a variable number of arguments */
        if (oi->fmt == OP_FMT_npop || oi->fmt == OP_FMT_npop_u16) {
            n_pop += get_u16(bc_buf + pos + 1);
        } else {
#if SHORT_OPCODES
            if (oi->fmt == OP_FMT_npopx) {
                n_pop += op - OP_call0;
            }
#endif
        }

        if (stack_len < n_pop) {
            JS_ThrowInternalError(ctx, "stack underflow (op=%d, pc=%d)", op, pos);
            return -1;
        }
        stack_len += oi->n_push - n_pop;
        if (stack_len > s->stack_len_max) {
            s->stack_len_max = stack_len;
            if (s->stack_len_max > JS_STACK_SIZE_MAX)
                goto stack_overflow;
        }
        switch(op) {
            case OP_tail_call:
            case OP_tail_call_method:
            case OP_return:
            case OP_return_undef:
            case OP_return_async:
            case OP_throw:
            case OP_throw_var:
            case OP_ret:
                goto done;
            case OP_goto:
                diff = get_u32(bc_buf + pos + 1);
                pos_next = pos + 1 + diff;
                break;
#if SHORT_OPCODES
                case OP_goto16:
            diff = (int16_t)get_u16(bc_buf + pos + 1);
            pos_next = pos + 1 + diff;
            break;
        case OP_goto8:
            diff = (int8_t)bc_buf[pos + 1];
            pos_next = pos + 1 + diff;
            break;
        case OP_if_true8:
        case OP_if_false8:
            diff = (int8_t)bc_buf[pos + 1];
            if (compute_stack_size_rec(ctx, fd, s, pos + 1 + diff, op, stack_len))
                return -1;
            break;
#endif
            case OP_if_true:
            case OP_if_false:
            case OP_catch:
                diff = get_u32(bc_buf + pos + 1);
                if (compute_stack_size_rec(ctx, fd, s, pos + 1 + diff, op, stack_len))
                    return -1;
                break;
            case OP_gosub:
                diff = get_u32(bc_buf + pos + 1);
                if (compute_stack_size_rec(ctx, fd, s, pos + 1 + diff, op, stack_len + 1))
                    return -1;
                break;
            case OP_with_get_var:
            case OP_with_delete_var:
                diff = get_u32(bc_buf + pos + 5);
                if (compute_stack_size_rec(ctx, fd, s, pos + 5 + diff, op, stack_len + 1))
                    return -1;
                break;
            case OP_with_make_ref:
            case OP_with_get_ref:
            case OP_with_get_ref_undef:
                diff = get_u32(bc_buf + pos + 5);
                if (compute_stack_size_rec(ctx, fd, s, pos + 5 + diff, op, stack_len + 2))
                    return -1;
                break;
            case OP_with_put_var:
                diff = get_u32(bc_buf + pos + 5);
                if (compute_stack_size_rec(ctx, fd, s, pos + 5 + diff, op, stack_len - 1))
                    return -1;
                break;

            default:
                break;
        }
        pos = pos_next;
    }
    done:
    return 0;

    stack_overflow:
    JS_ThrowInternalError(ctx, "stack overflow (op=%d, pc=%d)", op, pos);
    return -1;
}

static __exception int compute_stack_size(JSContext *ctx,
                                          JSFunctionDef *fd,
                                          int *pstack_size)
{
    StackSizeState s_s, *s = &s_s;
    int bc_len, i, ret;

    bc_len = fd->byte_code.size;
    /* bc_len > 0 */
    s->stack_level_tab = js_malloc(ctx, sizeof(s->stack_level_tab[0]) * bc_len);
    if (!s->stack_level_tab)
        return -1;
    for(i = 0; i < bc_len; i++)
        s->stack_level_tab[i] = 0xffff;
    s->stack_len_max = 0;
    ret = compute_stack_size_rec(ctx, fd, s, 0, OP_invalid, 0);
    js_free(ctx, s->stack_level_tab);
    *pstack_size = s->stack_len_max;
    return ret;
}

static int add_module_variables(JSContext *ctx, JSFunctionDef *fd)
{
    int i, idx;
    JSModuleDef *m = fd->module;
    JSExportEntry *me;
    JSHoistedDef *hf;

    /* The imported global variables were added as closure variables
       in js_parse_import(). We add here the module global
       variables. */

    for(i = 0; i < fd->hoisted_def_count; i++) {
        hf = &fd->hoisted_def[i];
        if (add_closure_var(ctx, fd, TRUE, FALSE, i, hf->var_name, hf->is_const,
                            hf->is_lexical, FALSE) < 0)
            return -1;
    }

    /* resolve the variable names of the local exports */
    for(i = 0; i < m->export_entries_count; i++) {
        me = &m->export_entries[i];
        if (me->export_type == JS_EXPORT_TYPE_LOCAL) {
            idx = find_closure_var(ctx, fd, me->local_name);
            if (idx < 0) {
                JS_ThrowSyntaxErrorAtom(ctx, "exported variable '%s' does not exist",
                                        me->local_name);
                return -1;
            }
            me->u.local.var_idx = idx;
        }
    }
    return 0;
}

/* create a function object from a function definition. The function
   definition is freed. All the child functions are also created. It
   must be done this way to resolve all the variables. */
static JSValue js_create_function(JSContext *ctx, JSFunctionDef *fd)
{
    JSValue func_obj;
    JSFunctionBytecode *b;
    struct list_head *el, *el1;
    int stack_size, scope, idx;
    int function_size, byte_code_offset, cpool_offset;
    int closure_var_offset, vardefs_offset;

    /* recompute scope linkage */
    for (scope = 0; scope < fd->scope_count; scope++) {
        fd->scopes[scope].first = -1;
    }
    for (idx = 0; idx < fd->var_count; idx++) {
        JSVarDef *vd = &fd->vars[idx];
        vd->scope_next = fd->scopes[vd->scope_level].first;
        fd->scopes[vd->scope_level].first = idx;
    }
    for (scope = 2; scope < fd->scope_count; scope++) {
        JSVarScope *sd = &fd->scopes[scope];
        if (sd->first == -1)
            sd->first = fd->scopes[sd->parent].first;
    }
    for (idx = 0; idx < fd->var_count; idx++) {
        JSVarDef *vd = &fd->vars[idx];
        if (vd->scope_next == -1 && vd->scope_level > 1) {
            scope = fd->scopes[vd->scope_level].parent;
            vd->scope_next = fd->scopes[scope].first;
        }
    }

    /* if the function contains an eval call, the closure variables
       are used to compile the eval and they must be ordered by scope,
       so it is necessary to create the closure variables before any
       other variable lookup is done. */
    if (fd->has_eval_call)
        add_eval_variables(ctx, fd);

    /* add the module global variables in the closure */
    if (fd->module) {
        if (add_module_variables(ctx, fd))
            goto fail;
    }

    /* first create all the child functions */
    list_for_each_safe(el, el1, &fd->child_list) {
        JSFunctionDef *fd1;
        int cpool_idx;

        fd1 = list_entry(el, JSFunctionDef, link);
        cpool_idx = fd1->parent_cpool_idx;
        func_obj = js_create_function(ctx, fd1);
        if (JS_IsException(func_obj))
            goto fail;
        /* save it in the constant pool */
        assert(cpool_idx >= 0);
        fd->cpool[cpool_idx] = func_obj;
    }

#if defined(DUMP_BYTECODE) && (DUMP_BYTECODE & 4)
    if (!(fd->js_mode & JS_MODE_STRIP)) {
        printf("pass 1\n");
        dump_byte_code(ctx, 1, fd->byte_code.buf, fd->byte_code.size,
                       fd->args, fd->arg_count, fd->vars, fd->var_count,
                       fd->closure_var, fd->closure_var_count,
                       fd->cpool, fd->cpool_count, fd->source, fd->line_num,
                       fd->label_slots, NULL);
        printf("\n");
    }
#endif

    if (resolve_variables(ctx, fd))
        goto fail;

#if defined(DUMP_BYTECODE) && (DUMP_BYTECODE & 2)
    if (!(fd->js_mode & JS_MODE_STRIP)) {
        printf("pass 2\n");
        dump_byte_code(ctx, 2, fd->byte_code.buf, fd->byte_code.size,
                       fd->args, fd->arg_count, fd->vars, fd->var_count,
                       fd->closure_var, fd->closure_var_count,
                       fd->cpool, fd->cpool_count, fd->source, fd->line_num,
                       fd->label_slots, NULL);
        printf("\n");
    }
#endif

    if (resolve_labels(ctx, fd))
        goto fail;

    if (compute_stack_size(ctx, fd, &stack_size) < 0)
        goto fail;

    if (fd->js_mode & JS_MODE_STRIP) {
        function_size = offsetof(JSFunctionBytecode, debug);
    } else {
        function_size = sizeof(*b);
    }
    cpool_offset = function_size;
    function_size += fd->cpool_count * sizeof(*fd->cpool);
    vardefs_offset = function_size;
    if (!(fd->js_mode & JS_MODE_STRIP) || fd->has_eval_call) {
        function_size += (fd->arg_count + fd->var_count) * sizeof(*b->vardefs);
    }
    closure_var_offset = function_size;
    function_size += fd->closure_var_count * sizeof(*fd->closure_var);
    byte_code_offset = function_size;
    function_size += fd->byte_code.size;

    b = js_mallocz(ctx, function_size);
    if (!b)
        goto fail;
    b->header.ref_count = 1;

    b->byte_code_buf = (void *)((uint8_t*)b + byte_code_offset);
    b->byte_code_len = fd->byte_code.size;
    memcpy(b->byte_code_buf, fd->byte_code.buf, fd->byte_code.size);
    js_free(ctx, fd->byte_code.buf);
    fd->byte_code.buf = NULL;

    b->func_name = fd->func_name;
    if (fd->arg_count + fd->var_count > 0) {
        if ((fd->js_mode & JS_MODE_STRIP) && !fd->has_eval_call) {
            /* Strip variable definitions not needed at runtime */
            int i;
            for(i = 0; i < fd->var_count; i++) {
                JS_FreeAtom(ctx, fd->vars[i].var_name);
            }
            for(i = 0; i < fd->arg_count; i++) {
                JS_FreeAtom(ctx, fd->args[i].var_name);
            }
            for(i = 0; i < fd->closure_var_count; i++) {
                JS_FreeAtom(ctx, fd->closure_var[i].var_name);
                fd->closure_var[i].var_name = JS_ATOM_NULL;
            }
        } else {
            b->vardefs = (void *)((uint8_t*)b + vardefs_offset);
            memcpy(b->vardefs, fd->args, fd->arg_count * sizeof(fd->args[0]));
            memcpy(b->vardefs + fd->arg_count, fd->vars, fd->var_count * sizeof(fd->vars[0]));
        }
        b->var_count = fd->var_count;
        b->arg_count = fd->arg_count;
        b->defined_arg_count = fd->defined_arg_count;
        js_free(ctx, fd->args);
        js_free(ctx, fd->vars);
    }
    b->cpool_count = fd->cpool_count;
    if (b->cpool_count) {
        b->cpool = (void *)((uint8_t*)b + cpool_offset);
        memcpy(b->cpool, fd->cpool, b->cpool_count * sizeof(*b->cpool));
    }
    js_free(ctx, fd->cpool);
    fd->cpool = NULL;

    b->stack_size = stack_size;

    if (fd->js_mode & JS_MODE_STRIP) {
        JS_FreeAtom(ctx, fd->filename);
        dbuf_free(&fd->pc2line);    // probably useless
    } else {
        /* XXX: source and pc2line info should be packed at the end of the
           JSFunctionBytecode structure, avoiding allocation overhead
         */
        b->has_debug = 1;
        b->debug.filename = fd->filename;
        b->debug.line_num = fd->line_num;

        //DynBuf pc2line;
        //compute_pc2line_info(fd, &pc2line);
        //js_free(ctx, fd->line_number_slots)
        b->debug.pc2line_buf = js_realloc(ctx, fd->pc2line.buf, fd->pc2line.size);
        if (!b->debug.pc2line_buf)
            b->debug.pc2line_buf = fd->pc2line.buf;
        b->debug.pc2line_len = fd->pc2line.size;
        b->debug.source = fd->source;
        b->debug.source_len = fd->source_len;
    }
    if (fd->scopes != fd->def_scope_array)
        js_free(ctx, fd->scopes);

    b->closure_var_count = fd->closure_var_count;
    if (b->closure_var_count) {
        b->closure_var = (void *)((uint8_t*)b + closure_var_offset);
        memcpy(b->closure_var, fd->closure_var, b->closure_var_count * sizeof(*b->closure_var));
    }
    js_free(ctx, fd->closure_var);
    fd->closure_var = NULL;

    b->has_prototype = fd->has_prototype;
    b->has_simple_parameter_list = fd->has_simple_parameter_list;
    b->js_mode = fd->js_mode;
    b->is_derived_class_constructor = fd->is_derived_class_constructor;
    b->func_kind = fd->func_kind;
    b->need_home_object = (fd->home_object_var_idx >= 0 ||
                           fd->need_home_object);
    b->new_target_allowed = fd->new_target_allowed;
    b->super_call_allowed = fd->super_call_allowed;
    b->super_allowed = fd->super_allowed;
    b->arguments_allowed = fd->arguments_allowed;
    b->backtrace_barrier = fd->backtrace_barrier;
    b->realm = JS_DupContext(ctx);

    add_gc_object(ctx->rt, &b->header, JS_GC_OBJ_TYPE_FUNCTION_BYTECODE);

#if defined(DUMP_BYTECODE) && (DUMP_BYTECODE & 1)
    if (!(fd->js_mode & JS_MODE_STRIP)) {
        js_dump_function_bytecode(ctx, b);
    }
#endif

    if (fd->parent) {
        /* remove from parent list */
        list_del(&fd->link);
    }

    js_free(ctx, fd);
    return JS_MKPTR(JS_TAG_FUNCTION_BYTECODE, b);
    fail:
    js_free_function_def(ctx, fd);
    return JS_EXCEPTION;
}

static void free_function_bytecode(JSRuntime *rt, JSFunctionBytecode *b)
{
    int i;

#if 0
    {
        char buf[ATOM_GET_STR_BUF_SIZE];
        printf("freeing %s\n",
               JS_AtomGetStrRT(rt, buf, sizeof(buf), b->func_name));
    }
#endif
    free_bytecode_atoms(rt, b->byte_code_buf, b->byte_code_len, TRUE);

    if (b->vardefs) {
        for(i = 0; i < b->arg_count + b->var_count; i++) {
            JS_FreeAtomRT(rt, b->vardefs[i].var_name);
        }
    }
    for(i = 0; i < b->cpool_count; i++)
        JS_FreeValueRT(rt, b->cpool[i]);

    for(i = 0; i < b->closure_var_count; i++) {
        JSClosureVar *cv = &b->closure_var[i];
        JS_FreeAtomRT(rt, cv->var_name);
    }
    if (b->realm)
        JS_FreeContext(b->realm);

    JS_FreeAtomRT(rt, b->func_name);
    if (b->has_debug) {
        JS_FreeAtomRT(rt, b->debug.filename);
        js_free_rt(rt, b->debug.pc2line_buf);
        js_free_rt(rt, b->debug.source);
    }

    remove_gc_object(&b->header);
    if (rt->gc_phase == JS_GC_PHASE_REMOVE_CYCLES && b->header.ref_count != 0) {
        list_add_tail(&b->header.link, &rt->gc_zero_ref_count_list);
    } else {
        js_free_rt(rt, b);
    }
}

static __exception int js_parse_directives(JSParseState *s)
{
    char str[20];
    JSParsePos pos;
    BOOL has_semi;

    if (s->token.val != TOK_STRING)
        return 0;

    js_parse_get_pos(s, &pos);

    while(s->token.val == TOK_STRING) {
        /* Copy actual source string representation */
        snprintf(str, sizeof str, "%.*s",
                 (int)(s->buf_ptr - s->token.ptr - 2), s->token.ptr + 1);

        if (next_token(s))
            return -1;

        has_semi = FALSE;
        switch (s->token.val) {
            case ';':
                if (next_token(s))
                    return -1;
                has_semi = TRUE;
                break;
            case '}':
            case TOK_EOF:
                has_semi = TRUE;
                break;
            case TOK_NUMBER:
            case TOK_STRING:
            case TOK_TEMPLATE:
            case TOK_IDENT:
            case TOK_REGEXP:
            case TOK_DEC:
            case TOK_INC:
            case TOK_NULL:
            case TOK_FALSE:
            case TOK_TRUE:
            case TOK_IF:
            case TOK_RETURN:
            case TOK_VAR:
            case TOK_THIS:
            case TOK_DELETE:
            case TOK_TYPEOF:
            case TOK_NEW:
            case TOK_DO:
            case TOK_WHILE:
            case TOK_FOR:
            case TOK_SWITCH:
            case TOK_THROW:
            case TOK_TRY:
            case TOK_FUNCTION:
            case TOK_DEBUGGER:
            case TOK_WITH:
            case TOK_CLASS:
            case TOK_CONST:
            case TOK_ENUM:
            case TOK_EXPORT:
            case TOK_IMPORT:
            case TOK_SUPER:
            case TOK_INTERFACE:
            case TOK_LET:
            case TOK_PACKAGE:
            case TOK_PRIVATE:
            case TOK_PROTECTED:
            case TOK_PUBLIC:
            case TOK_STATIC:
                /* automatic insertion of ';' */
                if (s->got_lf)
                    has_semi = TRUE;
                break;
            default:
                break;
        }
        if (!has_semi)
            break;
        if (!strcmp(str, "use strict")) {
            s->cur_func->has_use_strict = TRUE;
            s->cur_func->js_mode |= JS_MODE_STRICT;
        }
#if !defined(DUMP_BYTECODE) || !(DUMP_BYTECODE & 8)
        else if (!strcmp(str, "use strip")) {
            s->cur_func->js_mode |= JS_MODE_STRIP;
        }
#endif
#ifdef CONFIG_BIGNUM
        else if (s->ctx->bignum_ext && !strcmp(str, "use math")) {
            s->cur_func->js_mode |= JS_MODE_MATH;
        }
#endif
    }
    return js_parse_seek_token(s, &pos);
}

static int js_parse_function_check_names(JSParseState *s, JSFunctionDef *fd,
                                         JSAtom func_name)
{
    JSAtom name;
    int i, idx;

    if (fd->js_mode & JS_MODE_STRICT) {
        if (!fd->has_simple_parameter_list && fd->has_use_strict) {
            return js_parse_error(s, "\"use strict\" not allowed in function with default or destructuring parameter");
        }
        if (func_name == JS_ATOM_eval || func_name == JS_ATOM_arguments) {
            return js_parse_error(s, "invalid function name in strict code");
        }
        for (idx = 0; idx < fd->arg_count; idx++) {
            name = fd->args[idx].var_name;

            if (name == JS_ATOM_eval || name == JS_ATOM_arguments) {
                return js_parse_error(s, "invalid argument name in strict code");
            }
        }
    }
    /* check async_generator case */
    if ((fd->js_mode & JS_MODE_STRICT)
        ||  !fd->has_simple_parameter_list
        ||  (fd->func_type == JS_PARSE_FUNC_METHOD && fd->func_kind == JS_FUNC_ASYNC)
        ||  fd->func_type == JS_PARSE_FUNC_ARROW
        ||  fd->func_type == JS_PARSE_FUNC_METHOD) {
        for (idx = 0; idx < fd->arg_count; idx++) {
            name = fd->args[idx].var_name;
            if (name != JS_ATOM_NULL) {
                for (i = 0; i < idx; i++) {
                    if (fd->args[i].var_name == name)
                        goto duplicate;
                }
                /* Check if argument name duplicates a destructuring parameter */
                /* XXX: should have a flag for such variables */
                for (i = 0; i < fd->var_count; i++) {
                    if (fd->vars[i].var_name == name)
                        goto duplicate;
                }
            }
        }
    }
    return 0;

    duplicate:
    return js_parse_error(s, "duplicate argument names not allowed in this context");
}

/* create a function to initialize class fields */
static JSFunctionDef *js_parse_function_class_fields_init(JSParseState *s)
{
    JSFunctionDef *fd;

    fd = js_new_function_def(s->ctx, s->cur_func, FALSE, FALSE,
                             s->filename, 0);
    if (!fd)
        return NULL;
    fd->func_name = JS_ATOM_NULL;
    fd->has_prototype = FALSE;
    fd->has_home_object = TRUE;

    fd->has_arguments_binding = FALSE;
    fd->has_this_binding = TRUE;
    fd->is_derived_class_constructor = FALSE;
    fd->new_target_allowed = TRUE;
    fd->super_call_allowed = FALSE;
    fd->super_allowed = fd->has_home_object;
    fd->arguments_allowed = FALSE;

    fd->func_kind = JS_FUNC_NORMAL;
    fd->func_type = JS_PARSE_FUNC_METHOD;
    return fd;
}

/* func_name must be JS_ATOM_NULL for JS_PARSE_FUNC_STATEMENT and
   JS_PARSE_FUNC_EXPR, JS_PARSE_FUNC_ARROW and JS_PARSE_FUNC_VAR */
static __exception int js_parse_function_decl2(JSParseState *s,
                                               JSParseFunctionEnum func_type,
                                               JSFunctionKindEnum func_kind,
                                               JSAtom func_name,
                                               const uint8_t *ptr,
                                               int function_line_num,
                                               JSParseExportEnum export_flag,
                                               JSFunctionDef **pfd)
{
    JSContext *ctx = s->ctx;
    JSFunctionDef *fd = s->cur_func;
    BOOL is_expr;
    int func_idx, lexical_func_idx = -1;
    BOOL has_opt_arg;
    BOOL create_func_var = FALSE;

    is_expr = (func_type != JS_PARSE_FUNC_STATEMENT &&
               func_type != JS_PARSE_FUNC_VAR);

    if (func_type == JS_PARSE_FUNC_STATEMENT ||
        func_type == JS_PARSE_FUNC_VAR ||
        func_type == JS_PARSE_FUNC_EXPR) {
        if (func_kind == JS_FUNC_NORMAL &&
            token_is_pseudo_keyword(s, JS_ATOM_async) &&
            peek_token(s, TRUE) != '\n') {
            if (next_token(s))
                return -1;
            func_kind = JS_FUNC_ASYNC;
        }
        if (next_token(s))
            return -1;
        if (s->token.val == '*') {
            if (next_token(s))
                return -1;
            func_kind |= JS_FUNC_GENERATOR;
        }

        if (s->token.val == TOK_IDENT) {
            if (s->token.u.ident.is_reserved ||
                (s->token.u.ident.atom == JS_ATOM_yield &&
                 func_type == JS_PARSE_FUNC_EXPR &&
                 (func_kind & JS_FUNC_GENERATOR)) ||
                (s->token.u.ident.atom == JS_ATOM_await &&
                 func_type == JS_PARSE_FUNC_EXPR &&
                 (func_kind & JS_FUNC_ASYNC))) {
                return js_parse_error_reserved_identifier(s);
            }
        }
        if (s->token.val == TOK_IDENT ||
            (((s->token.val == TOK_YIELD && !(fd->js_mode & JS_MODE_STRICT)) ||
              (s->token.val == TOK_AWAIT && !s->is_module)) &&
             func_type == JS_PARSE_FUNC_EXPR)) {
            func_name = JS_DupAtom(ctx, s->token.u.ident.atom);
            if (next_token(s)) {
                JS_FreeAtom(ctx, func_name);
                return -1;
            }
        } else {
            if (func_type != JS_PARSE_FUNC_EXPR &&
                export_flag != JS_PARSE_EXPORT_DEFAULT) {
                return js_parse_error(s, "function name expected");
            }
        }
    } else if (func_type != JS_PARSE_FUNC_ARROW) {
        func_name = JS_DupAtom(ctx, func_name);
    }

    if (fd->is_eval && fd->eval_type == JS_EVAL_TYPE_MODULE &&
        (func_type == JS_PARSE_FUNC_STATEMENT || func_type == JS_PARSE_FUNC_VAR)) {
        JSHoistedDef *hf;
        hf = find_hoisted_def(fd, func_name);
        /* XXX: should check scope chain */
        if (hf && hf->scope_level == fd->scope_level) {
            js_parse_error(s, "invalid redefinition of global identifier in module code");
            JS_FreeAtom(ctx, func_name);
            return -1;
        }
    }

    if (func_type == JS_PARSE_FUNC_VAR) {
        /* Create lexical name here so function closure contains it */
        if (!(fd->js_mode & JS_MODE_STRICT)
            &&  find_lexical_decl(ctx, fd, func_name, fd->scope_first, FALSE) < 0
            &&  !((func_idx = find_var(ctx, fd, func_name)) >= 0 && (func_idx & ARGUMENT_VAR_OFFSET))
            &&  !(func_name == JS_ATOM_arguments && fd->has_arguments_binding)) {
            create_func_var = TRUE;
        }
        if (fd->is_eval &&
            (fd->eval_type == JS_EVAL_TYPE_GLOBAL ||
             fd->eval_type == JS_EVAL_TYPE_MODULE) &&
            fd->scope_level == 1) {
            /* avoid creating a lexical variable in the global
               scope. XXX: check annex B */
            JSHoistedDef *hf;
            hf = find_hoisted_def(fd, func_name);
            /* XXX: should check scope chain */
            if (hf && hf->scope_level == fd->scope_level) {
                js_parse_error(s, "invalid redefinition of global identifier");
                JS_FreeAtom(ctx, func_name);
                return -1;
            }
        } else {
            /* Always create a lexical name, fail if at the same scope as
               existing name */
            /* Lexical variable will be initialized upon entering scope */
            lexical_func_idx = define_var(s, fd, func_name,
                                          func_kind != JS_FUNC_NORMAL ?
                                          JS_VAR_DEF_NEW_FUNCTION_DECL :
                                          JS_VAR_DEF_FUNCTION_DECL);
            if (lexical_func_idx < 0) {
                JS_FreeAtom(ctx, func_name);
                return -1;
            }
        }
    }

    fd = js_new_function_def(ctx, fd, FALSE, is_expr,
                             s->filename, function_line_num);
    if (!fd) {
        JS_FreeAtom(ctx, func_name);
        return -1;
    }
    if (pfd)
        *pfd = fd;
    s->cur_func = fd;
    fd->func_name = func_name;
    /* XXX: test !fd->is_generator is always false */
    fd->has_prototype = (func_type == JS_PARSE_FUNC_STATEMENT ||
                         func_type == JS_PARSE_FUNC_VAR ||
                         func_type == JS_PARSE_FUNC_EXPR) &&
                        func_kind == JS_FUNC_NORMAL;
    fd->has_home_object = (func_type == JS_PARSE_FUNC_METHOD ||
                           func_type == JS_PARSE_FUNC_GETTER ||
                           func_type == JS_PARSE_FUNC_SETTER ||
                           func_type == JS_PARSE_FUNC_CLASS_CONSTRUCTOR ||
                           func_type == JS_PARSE_FUNC_DERIVED_CLASS_CONSTRUCTOR);
    fd->has_arguments_binding = (func_type != JS_PARSE_FUNC_ARROW);
    fd->has_this_binding = fd->has_arguments_binding;
    fd->is_derived_class_constructor = (func_type == JS_PARSE_FUNC_DERIVED_CLASS_CONSTRUCTOR);
    if (func_type == JS_PARSE_FUNC_ARROW) {
        fd->new_target_allowed = fd->parent->new_target_allowed;
        fd->super_call_allowed = fd->parent->super_call_allowed;
        fd->super_allowed = fd->parent->super_allowed;
        fd->arguments_allowed = fd->parent->arguments_allowed;
    } else {
        fd->new_target_allowed = TRUE;
        fd->super_call_allowed = fd->is_derived_class_constructor;
        fd->super_allowed = fd->has_home_object;
        fd->arguments_allowed = TRUE;
    }

    /* fd->in_function_body == FALSE prevents yield/await during the parsing
       of the arguments in generator/async functions. They are parsed as
       regular identifiers for other function kinds. */
    fd->func_kind = func_kind;
    fd->func_type = func_type;

    if (func_type == JS_PARSE_FUNC_CLASS_CONSTRUCTOR ||
        func_type == JS_PARSE_FUNC_DERIVED_CLASS_CONSTRUCTOR) {
        /* error if not invoked as a constructor */
        emit_op(s, OP_check_ctor);
    }

    if (func_type == JS_PARSE_FUNC_CLASS_CONSTRUCTOR) {
        emit_class_field_init(s);
    }

    /* parse arguments */
    fd->has_simple_parameter_list = TRUE;
    has_opt_arg = FALSE;
    if (func_type == JS_PARSE_FUNC_ARROW && s->token.val == TOK_IDENT) {
        JSAtom name;
        if (s->token.u.ident.is_reserved) {
            js_parse_error_reserved_identifier(s);
            goto fail;
        }
        name = s->token.u.ident.atom;
        if (add_arg(ctx, fd, name) < 0)
            goto fail;
        fd->defined_arg_count = 1;
    } else {
        if (js_parse_expect(s, '('))
            goto fail;

        while (s->token.val != ')') {
            JSAtom name;
            BOOL rest = FALSE;
            int idx;

            if (s->token.val == TOK_ELLIPSIS) {
                fd->has_simple_parameter_list = FALSE;
                rest = TRUE;
                if (next_token(s))
                    goto fail;
            }
            if (s->token.val == '[' || s->token.val == '{') {
                fd->has_simple_parameter_list = FALSE;
                if (rest) {
                    emit_op(s, OP_rest);
                    emit_u16(s, fd->arg_count);
                } else {
                    /* unnamed arg for destructuring */
                    idx = add_arg(ctx, fd, JS_ATOM_NULL);
                    emit_op(s, OP_get_arg);
                    emit_u16(s, idx);
                }
                if (js_parse_destructuring_element(s, TOK_VAR, 1, TRUE, -1, TRUE))
                    goto fail;
            } else if (s->token.val == TOK_IDENT) {
                if (s->token.u.ident.is_reserved) {
                    js_parse_error_reserved_identifier(s);
                    goto fail;
                }
                name = s->token.u.ident.atom;
                if (name == JS_ATOM_yield && fd->func_kind == JS_FUNC_GENERATOR) {
                    js_parse_error_reserved_identifier(s);
                    goto fail;
                }
                idx = add_arg(ctx, fd, name);
                if (idx < 0)
                    goto fail;
                if (next_token(s))
                    goto fail;
                if (rest) {
                    emit_op(s, OP_rest);
                    emit_u16(s, idx);
                    emit_op(s, OP_put_arg);
                    emit_u16(s, idx);
                    fd->has_simple_parameter_list = FALSE;
                    has_opt_arg = TRUE;
                } else if (s->token.val == '=') {
                    fd->has_simple_parameter_list = FALSE;
                    has_opt_arg = TRUE;

                    if (next_token(s))
                        goto fail;

                    /* optimize `x = void 0` default value: no code needed */
                    if (s->token.val == TOK_VOID) {
                        JSParsePos pos;
                        js_parse_get_pos(s, &pos);
                        if (next_token(s))
                            goto fail;
                        if (s->token.val == TOK_NUMBER) {
                            if (next_token(s))
                                goto fail;
                            if (s->token.val == ',') {
                                if (next_token(s))
                                    goto fail;
                                continue;
                            }
                            if (s->token.val == ')') {
                                continue;
                            }
                        }
                        if (js_parse_seek_token(s, &pos))
                            goto fail;
                    }
#if 0
                    /* XXX: not correct for eval code */
                    /* Check for a default value of `undefined`
                       to omit default argument processing */
                    if (s->token.val == TOK_IDENT &&
                        s->token.u.ident.atom == JS_ATOM_undefined &&
                        fd->parent == NULL &&
                        ((tok = peek_token(s, FALSE)) == ',' || tok == ')')) {
                        if (next_token(s))  /* ignore undefined token */
                            goto fail;
                    } else
#endif
                    {
                        int label = new_label(s);
                        if (idx > 0) {
                            emit_op(s, OP_set_arg_valid_upto);
                            emit_u16(s, idx);
                        }
                        emit_op(s, OP_get_arg);
                        emit_u16(s, idx);
                        emit_op(s, OP_undefined);
                        emit_op(s, OP_strict_eq);
                        emit_goto(s, OP_if_false, label);
                        if (js_parse_assign_expr(s, TRUE))
                            goto fail;
                        set_object_name(s, name);
                        emit_op(s, OP_put_arg);
                        emit_u16(s, idx);
                        emit_label(s, label);
                    }
                } else if (!has_opt_arg) {
                    fd->defined_arg_count++;
                }
            } else {
                js_parse_error(s, "missing formal parameter");
                goto fail;
            }
            if (rest && s->token.val != ')') {
                js_parse_expect(s, ')');
                goto fail;
            }
            if (s->token.val == ')')
                break;
            if (js_parse_expect(s, ','))
                goto fail;
        }
        if ((func_type == JS_PARSE_FUNC_GETTER && fd->arg_count != 0) ||
            (func_type == JS_PARSE_FUNC_SETTER && fd->arg_count != 1)) {
            js_parse_error(s, "invalid number of arguments for getter or setter");
            goto fail;
        }
    }

    if (next_token(s))
        goto fail;

    /* generator function: yield after the parameters are evaluated */
    if (func_kind == JS_FUNC_GENERATOR ||
        func_kind == JS_FUNC_ASYNC_GENERATOR)
        emit_op(s, OP_initial_yield);

    /* in generators, yield expression is forbidden during the parsing
       of the arguments */
    fd->in_function_body = TRUE;
    push_scope(s);  /* enter body scope: fd->scope_level = 1 */

    if (s->token.val == TOK_ARROW) {
        if (next_token(s))
            goto fail;

        if (s->token.val != '{') {
            if (js_parse_function_check_names(s, fd, func_name))
                goto fail;

            if (js_parse_assign_expr(s, TRUE))
                goto fail;

            if (func_kind != JS_FUNC_NORMAL)
                emit_op(s, OP_return_async);
            else
                emit_op(s, OP_return);

            if (!(fd->js_mode & JS_MODE_STRIP)) {
                /* save the function source code */
                /* the end of the function source code is after the last
                   token of the function source stored into s->last_ptr */
                fd->source_len = s->last_ptr - ptr;
                fd->source = js_strndup(ctx, (const char *)ptr, fd->source_len);
                if (!fd->source)
                    goto fail;
            }
            goto done;
        }
    }

    if (js_parse_expect(s, '{'))
        goto fail;

    if (js_parse_directives(s))
        goto fail;

    /* in strict_mode, check function and argument names */
    if (js_parse_function_check_names(s, fd, func_name))
        goto fail;

    while (s->token.val != '}') {
        if (js_parse_source_element(s))
            goto fail;
    }
    if (!(fd->js_mode & JS_MODE_STRIP)) {
        /* save the function source code */
        fd->source_len = s->buf_ptr - ptr;
        fd->source = js_strndup(ctx, (const char *)ptr, fd->source_len);
        if (!fd->source)
            goto fail;
    }

    if (next_token(s)) {
        /* consume the '}' */
        goto fail;
    }

    /* in case there is no return, add one */
    if (js_is_live_code(s)) {
        emit_return(s, FALSE);
    }
    done:
    s->cur_func = fd->parent;

    /* create the function object */
    {
        int idx;
        JSAtom func_name = fd->func_name;

        /* the real object will be set at the end of the compilation */
        idx = cpool_add(s, JS_NULL);
        fd->parent_cpool_idx = idx;

        if (is_expr) {
            /* for constructors, no code needs to be generated here */
            if (func_type != JS_PARSE_FUNC_CLASS_CONSTRUCTOR &&
                func_type != JS_PARSE_FUNC_DERIVED_CLASS_CONSTRUCTOR) {
                /* OP_fclosure creates the function object from the bytecode
                   and adds the scope information */
                emit_op(s, OP_fclosure);
                emit_u32(s, idx);
                if (func_name == JS_ATOM_NULL) {
                    emit_op(s, OP_set_name);
                    emit_u32(s, JS_ATOM_NULL);
                }
            }
        } else if (func_type == JS_PARSE_FUNC_VAR) {
            emit_op(s, OP_fclosure);
            emit_u32(s, idx);
            if (create_func_var) {
                if (s->cur_func->is_global_var) {
                    JSHoistedDef *hf;
                    /* the global variable must be defined at the start of the
                       function */
                    hf = add_hoisted_def(ctx, s->cur_func, -1, func_name, -1, FALSE);
                    if (!hf)
                        goto fail;
                    /* it is considered as defined at the top level
                       (needed for annex B.3.3.4 and B.3.3.5
                       checks) */
                    hf->scope_level = 0;
                    hf->force_init = ((s->cur_func->js_mode & JS_MODE_STRICT) != 0);
                    /* store directly into global var, bypass lexical scope */
                    emit_op(s, OP_dup);
                    emit_op(s, OP_scope_put_var);
                    emit_atom(s, func_name);
                    emit_u16(s, 0);
                } else {
                    /* do not call define_var to bypass lexical scope check */
                    func_idx = find_var(ctx, s->cur_func, func_name);
                    if (func_idx < 0) {
                        func_idx = add_var(ctx, s->cur_func, func_name);
                        if (func_idx < 0)
                            goto fail;
                    }
                    /* store directly into local var, bypass lexical catch scope */
                    emit_op(s, OP_dup);
                    emit_op(s, OP_scope_put_var);
                    emit_atom(s, func_name);
                    emit_u16(s, 0);
                }
            }
            if (lexical_func_idx >= 0) {
                /* lexical variable will be initialized upon entering scope */
                s->cur_func->vars[lexical_func_idx].func_pool_or_scope_idx = idx;
                emit_op(s, OP_drop);
            } else {
                /* store function object into its lexical name */
                /* XXX: could use OP_put_loc directly */
                emit_op(s, OP_scope_put_var_init);
                emit_atom(s, func_name);
                emit_u16(s, s->cur_func->scope_level);
            }
        } else {
            if (!s->cur_func->is_global_var) {
                int var_idx = define_var(s, s->cur_func, func_name, JS_VAR_DEF_VAR);

                if (var_idx < 0)
                    goto fail;
                /* the variable will be assigned at the top of the function */
                if (!add_hoisted_def(ctx, s->cur_func, idx, JS_ATOM_NULL, var_idx, FALSE))
                    goto fail;
            } else {
                JSAtom func_var_name;
                if (func_name == JS_ATOM_NULL)
                    func_var_name = JS_ATOM__default_; /* export default */
                else
                    func_var_name = func_name;
                /* the variable will be assigned at the top of the function */
                if (!add_hoisted_def(ctx, s->cur_func, idx, func_var_name, -1, FALSE))
                    goto fail;
                if (export_flag != JS_PARSE_EXPORT_NONE) {
                    if (!add_export_entry(s, s->cur_func->module, func_var_name,
                                          export_flag == JS_PARSE_EXPORT_NAMED ? func_var_name : JS_ATOM_default, JS_EXPORT_TYPE_LOCAL))
                        goto fail;
                }
            }
        }
    }
    return 0;
    fail:
    s->cur_func = fd->parent;
    js_free_function_def(ctx, fd);
    if (pfd)
        *pfd = NULL;
    return -1;
}

static __exception int js_parse_function_decl(JSParseState *s,
                                              JSParseFunctionEnum func_type,
                                              JSFunctionKindEnum func_kind,
                                              JSAtom func_name,
                                              const uint8_t *ptr,
                                              int function_line_num)
{
    return js_parse_function_decl2(s, func_type, func_kind, func_name, ptr,
                                   function_line_num, JS_PARSE_EXPORT_NONE,
                                   NULL);
}

static __exception int js_parse_program(JSParseState *s)
{
    JSFunctionDef *fd = s->cur_func;
    int idx;

    if (next_token(s))
        return -1;

    if (js_parse_directives(s))
        return -1;

    fd->is_global_var = (fd->eval_type == JS_EVAL_TYPE_GLOBAL) ||
                        (fd->eval_type == JS_EVAL_TYPE_MODULE) ||
                        !(fd->js_mode & JS_MODE_STRICT);

    if (!s->is_module) {
        /* hidden variable for the return value */
        fd->eval_ret_idx = idx = add_var(s->ctx, fd, JS_ATOM__ret_);
        if (idx < 0)
            return -1;
    }

    while (s->token.val != TOK_EOF) {
        if (js_parse_source_element(s))
            return -1;
    }

    if (!s->is_module) {
        /* return the value of the hidden variable eval_ret_idx  */
        emit_op(s, OP_get_loc);
        emit_u16(s, fd->eval_ret_idx);

        emit_op(s, OP_return);
    } else {
        emit_op(s, OP_return_undef);
    }

    return 0;
}

static void js_parse_init(JSContext *ctx, JSParseState *s,
                          const char *input, size_t input_len,
                          const char *filename)
{
    memset(s, 0, sizeof(*s));
    s->ctx = ctx;
    s->filename = filename;
    s->line_num = 1;
    s->buf_ptr = (const uint8_t *)input;
    s->buf_end = s->buf_ptr + input_len;
    s->token.val = ' ';
    s->token.line_num = 1;
}

static JSValue JS_EvalFunctionInternal(JSContext *ctx, JSValue fun_obj,
                                       JSValueConst this_obj,
                                       JSVarRef **var_refs, JSStackFrame *sf)
{
    JSValue ret_val;
    uint32_t tag;

    tag = JS_VALUE_GET_TAG(fun_obj);
    if (tag == JS_TAG_FUNCTION_BYTECODE) {
        fun_obj = js_closure(ctx, fun_obj, var_refs, sf);
        ret_val = JS_CallFree(ctx, fun_obj, this_obj, 0, NULL);
    } else if (tag == JS_TAG_MODULE) {
        JSModuleDef *m;
        m = JS_VALUE_GET_PTR(fun_obj);
        /* the module refcount should be >= 2 */
        JS_FreeValue(ctx, fun_obj);
        if (js_create_module_function(ctx, m) < 0)
            goto fail;
        if (js_instantiate_module(ctx, m) < 0)
            goto fail;
        ret_val = js_evaluate_module(ctx, m);
        if (JS_IsException(ret_val)) {
            fail:
            js_free_modules(ctx, JS_FREE_MODULE_NOT_EVALUATED);
            return JS_EXCEPTION;
        }
    } else {
        JS_FreeValue(ctx, fun_obj);
        ret_val = JS_ThrowTypeError(ctx, "bytecode function expected");
    }
    return ret_val;
}

JSValue JS_EvalFunction(JSContext *ctx, JSValue fun_obj)
{
    return JS_EvalFunctionInternal(ctx, fun_obj, ctx->global_obj, NULL, NULL);
}

static void skip_shebang(JSParseState *s)
{
    const uint8_t *p = s->buf_ptr;
    int c;

    if (p[0] == '#' && p[1] == '!') {
        p += 2;
        while (p < s->buf_end) {
            if (*p == '\n' || *p == '\r') {
                break;
            } else if (*p >= 0x80) {
                c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p);
                if (c == CP_LS || c == CP_PS) {
                    break;
                } else if (c == -1) {
                    p++; /* skip invalid UTF-8 */
                }
            } else {
                p++;
            }
        }
        s->buf_ptr = p;
    }
}

/* 'input' must be zero terminated i.e. input[input_len] = '\0'. */
static JSValue __JS_EvalInternal(JSContext *ctx, JSValueConst this_obj,
                                 const char *input, size_t input_len,
                                 const char *filename, int flags, int scope_idx)
{
    JSParseState s1, *s = &s1;
    int err, js_mode, eval_type;
    JSValue fun_obj, ret_val;
    JSStackFrame *sf;
    JSVarRef **var_refs;
    JSFunctionBytecode *b;
    JSFunctionDef *fd;
    JSModuleDef *m;

    js_parse_init(ctx, s, input, input_len, filename);
    skip_shebang(s);

    eval_type = flags & JS_EVAL_TYPE_MASK;
    m = NULL;
    if (eval_type == JS_EVAL_TYPE_DIRECT) {
        JSObject *p;
        sf = ctx->rt->current_stack_frame;
        assert(sf != NULL);
        assert(JS_VALUE_GET_TAG(sf->cur_func) == JS_TAG_OBJECT);
        p = JS_VALUE_GET_OBJ(sf->cur_func);
        assert(js_class_has_bytecode(p->class_id));
        b = p->u.func.function_bytecode;
        var_refs = p->u.func.var_refs;
        js_mode = b->js_mode;
    } else {
        sf = NULL;
        b = NULL;
        var_refs = NULL;
        js_mode = 0;
        if (flags & JS_EVAL_FLAG_STRICT)
            js_mode |= JS_MODE_STRICT;
        if (flags & JS_EVAL_FLAG_STRIP)
            js_mode |= JS_MODE_STRIP;
        if (eval_type == JS_EVAL_TYPE_MODULE) {
            JSAtom module_name = JS_NewAtom(ctx, filename);
            if (module_name == JS_ATOM_NULL)
                return JS_EXCEPTION;
            m = js_new_module_def(ctx, module_name);
            if (!m)
                return JS_EXCEPTION;
            js_mode |= JS_MODE_STRICT;
        }
    }
    fd = js_new_function_def(ctx, NULL, TRUE, FALSE, filename, 1);
    if (!fd)
        goto fail1;
    s->cur_func = fd;
    fd->eval_type = eval_type;
    fd->has_this_binding = (eval_type != JS_EVAL_TYPE_DIRECT);
    fd->backtrace_barrier = ((flags & JS_EVAL_FLAG_BACKTRACE_BARRIER) != 0);
    if (eval_type == JS_EVAL_TYPE_DIRECT) {
        fd->new_target_allowed = b->new_target_allowed;
        fd->super_call_allowed = b->super_call_allowed;
        fd->super_allowed = b->super_allowed;
        fd->arguments_allowed = b->arguments_allowed;
    } else {
        fd->new_target_allowed = FALSE;
        fd->super_call_allowed = FALSE;
        fd->super_allowed = FALSE;
        fd->arguments_allowed = TRUE;
    }
    fd->js_mode = js_mode;
    fd->func_name = JS_DupAtom(ctx, JS_ATOM__eval_);
    if (b) {
        if (add_closure_variables(ctx, fd, b, scope_idx))
            goto fail;
    }
    fd->module = m;
    s->is_module = (m != NULL);
    s->allow_html_comments = !s->is_module;

    push_scope(s); /* body scope */

    err = js_parse_program(s);
    if (err) {
        fail:
        free_token(s, &s->token);
        js_free_function_def(ctx, fd);
        goto fail1;
    }

    /* create the function object and all the enclosed functions */
    fun_obj = js_create_function(ctx, fd);
    if (JS_IsException(fun_obj))
        goto fail1;
    /* Could add a flag to avoid resolution if necessary */
    if (m) {
        m->func_obj = fun_obj;
        if (js_resolve_module(ctx, m) < 0)
            goto fail1;
        fun_obj = JS_DupValue(ctx, JS_MKPTR(JS_TAG_MODULE, m));
    }
    if (flags & JS_EVAL_FLAG_COMPILE_ONLY) {
        ret_val = fun_obj;
    } else {
        ret_val = JS_EvalFunctionInternal(ctx, fun_obj, this_obj, var_refs, sf);
    }
    return ret_val;
    fail1:
    /* XXX: should free all the unresolved dependencies */
    if (m)
        js_free_module_def(ctx, m);
    return JS_EXCEPTION;
}

/* the indirection is needed to make 'eval' optional */
static JSValue JS_EvalInternal(JSContext *ctx, JSValueConst this_obj,
                               const char *input, size_t input_len,
                               const char *filename, int flags, int scope_idx)
{
    if (unlikely(!ctx->eval_internal)) {
        return JS_ThrowTypeError(ctx, "eval is not supported");
    }
    return ctx->eval_internal(ctx, this_obj, input, input_len, filename,
                              flags, scope_idx);
}

static JSValue JS_EvalObject(JSContext *ctx, JSValueConst this_obj,
                             JSValueConst val, int flags, int scope_idx)
{
    JSValue ret;
    const char *str;
    size_t len;

    if (!JS_IsString(val))
        return JS_DupValue(ctx, val);
    str = JS_ToCStringLen(ctx, &len, val);
    if (!str)
        return JS_EXCEPTION;
    ret = JS_EvalInternal(ctx, this_obj, str, len, "<input>", flags, scope_idx);
    JS_FreeCString(ctx, str);
    return ret;

}

JSValue JS_Eval(JSContext *ctx, const char *input, size_t input_len,
                const char *filename, int eval_flags)
{
    int eval_type = eval_flags & JS_EVAL_TYPE_MASK;
    JSValue ret;

    assert(eval_type == JS_EVAL_TYPE_GLOBAL ||
           eval_type == JS_EVAL_TYPE_MODULE);
    ret = JS_EvalInternal(ctx, ctx->global_obj, input, input_len, filename,
                          eval_flags, -1);
    return ret;
}

int JS_ResolveModule(JSContext *ctx, JSValueConst obj)
{
    if (JS_VALUE_GET_TAG(obj) == JS_TAG_MODULE) {
        JSModuleDef *m = JS_VALUE_GET_PTR(obj);
        if (js_resolve_module(ctx, m) < 0) {
            js_free_modules(ctx, JS_FREE_MODULE_NOT_RESOLVED);
            return -1;
        }
    }
    return 0;
}

/*******************************************************************/
/* object list */

typedef struct {
    JSObject *obj;
    uint32_t hash_next; /* -1 if no next entry */
} JSObjectListEntry;

/* XXX: reuse it to optimize weak references */
typedef struct {
    JSObjectListEntry *object_tab;
    int object_count;
    int object_size;
    uint32_t *hash_table;
    uint32_t hash_size;
} JSObjectList;

static void js_object_list_init(JSObjectList *s)
{
    memset(s, 0, sizeof(*s));
}

static uint32_t js_object_list_get_hash(JSObject *p, uint32_t hash_size)
{
    return ((uintptr_t)p * 3163) & (hash_size - 1);
}

static int js_object_list_resize_hash(JSContext *ctx, JSObjectList *s,
                                      uint32_t new_hash_size)
{
    JSObjectListEntry *e;
    uint32_t i, h, *new_hash_table;

    new_hash_table = js_malloc(ctx, sizeof(new_hash_table[0]) * new_hash_size);
    if (!new_hash_table)
        return -1;
    js_free(ctx, s->hash_table);
    s->hash_table = new_hash_table;
    s->hash_size = new_hash_size;

    for(i = 0; i < s->hash_size; i++) {
        s->hash_table[i] = -1;
    }
    for(i = 0; i < s->object_count; i++) {
        e = &s->object_tab[i];
        h = js_object_list_get_hash(e->obj, s->hash_size);
        e->hash_next = s->hash_table[h];
        s->hash_table[h] = i;
    }
    return 0;
}

/* the reference count of 'obj' is not modified. Return 0 if OK, -1 if
   memory error */
static int js_object_list_add(JSContext *ctx, JSObjectList *s, JSObject *obj)
{
    JSObjectListEntry *e;
    uint32_t h, new_hash_size;

    if (js_resize_array(ctx, (void *)&s->object_tab,
                        sizeof(s->object_tab[0]),
                        &s->object_size, s->object_count + 1))
        return -1;
    if (unlikely((s->object_count + 1) >= s->hash_size)) {
        new_hash_size = max_uint32(s->hash_size, 4);
        while (new_hash_size <= s->object_count)
            new_hash_size *= 2;
        if (js_object_list_resize_hash(ctx, s, new_hash_size))
            return -1;
    }
    e = &s->object_tab[s->object_count++];
    h = js_object_list_get_hash(obj, s->hash_size);
    e->obj = obj;
    e->hash_next = s->hash_table[h];
    s->hash_table[h] = s->object_count - 1;
    return 0;
}

/* return -1 if not present or the object index */
static int js_object_list_find(JSContext *ctx, JSObjectList *s, JSObject *obj)
{
    JSObjectListEntry *e;
    uint32_t h, p;

    /* must test empty size because there is no hash table */
    if (s->object_count == 0)
        return -1;
    h = js_object_list_get_hash(obj, s->hash_size);
    p = s->hash_table[h];
    while (p != -1) {
        e = &s->object_tab[p];
        if (e->obj == obj)
            return p;
        p = e->hash_next;
    }
    return -1;
}

static void js_object_list_end(JSContext *ctx, JSObjectList *s)
{
    js_free(ctx, s->object_tab);
    js_free(ctx, s->hash_table);
}

/*******************************************************************/
/* binary object writer & reader */

typedef enum BCTagEnum {
    BC_TAG_NULL = 1,
    BC_TAG_UNDEFINED,
    BC_TAG_BOOL_FALSE,
    BC_TAG_BOOL_TRUE,
    BC_TAG_INT32,
    BC_TAG_FLOAT64,
    BC_TAG_STRING,
    BC_TAG_OBJECT,
    BC_TAG_ARRAY,
    BC_TAG_BIG_INT,
    BC_TAG_BIG_FLOAT,
    BC_TAG_BIG_DECIMAL,
    BC_TAG_TEMPLATE_OBJECT,
    BC_TAG_FUNCTION_BYTECODE,
    BC_TAG_MODULE,
    BC_TAG_TYPED_ARRAY,
    BC_TAG_ARRAY_BUFFER,
    BC_TAG_SHARED_ARRAY_BUFFER,
    BC_TAG_DATE,
    BC_TAG_OBJECT_VALUE,
    BC_TAG_OBJECT_REFERENCE,
} BCTagEnum;

#ifdef CONFIG_BIGNUM
#define BC_BASE_VERSION 2
#else
#define BC_BASE_VERSION 1
#endif
#define BC_BE_VERSION 0x40
#ifdef WORDS_BIGENDIAN
#define BC_VERSION (BC_BASE_VERSION | BC_BE_VERSION)
#else
#define BC_VERSION BC_BASE_VERSION
#endif

typedef struct BCWriterState {
    JSContext *ctx;
    DynBuf dbuf;
    BOOL byte_swap : 8;
    BOOL allow_bytecode : 8;
    BOOL allow_sab : 8;
    BOOL allow_reference : 8;
    uint32_t first_atom;
    uint32_t *atom_to_idx;
    int atom_to_idx_size;
    JSAtom *idx_to_atom;
    int idx_to_atom_count;
    int idx_to_atom_size;
    uint8_t **sab_tab;
    int sab_tab_len;
    int sab_tab_size;
    /* list of referenced objects (used if allow_reference = TRUE) */
    JSObjectList object_list;
} BCWriterState;

#ifdef DUMP_READ_OBJECT
static const char * const bc_tag_str[] = {
    "invalid",
    "null",
    "undefined",
    "false",
    "true",
    "int32",
    "float64",
    "string",
    "object",
    "array",
    "bigint",
    "bigfloat",
    "bigdecimal",
    "template",
    "function",
    "module",
    "TypedArray",
    "ArrayBuffer",
    "SharedArrayBuffer",
    "Date",
    "ObjectValue",
    "ObjectReference",
};
#endif

static void bc_put_u8(BCWriterState *s, uint8_t v)
{
    dbuf_putc(&s->dbuf, v);
}

static void bc_put_u16(BCWriterState *s, uint16_t v)
{
    if (s->byte_swap)
        v = bswap16(v);
    dbuf_put_u16(&s->dbuf, v);
}

static __maybe_unused void bc_put_u32(BCWriterState *s, uint32_t v)
{
    if (s->byte_swap)
        v = bswap32(v);
    dbuf_put_u32(&s->dbuf, v);
}

static void bc_put_u64(BCWriterState *s, uint64_t v)
{
    if (s->byte_swap)
        v = bswap64(v);
    dbuf_put(&s->dbuf, (uint8_t *)&v, sizeof(v));
}

static void bc_put_leb128(BCWriterState *s, uint32_t v)
{
    dbuf_put_leb128(&s->dbuf, v);
}

static void bc_put_sleb128(BCWriterState *s, int32_t v)
{
    dbuf_put_sleb128(&s->dbuf, v);
}

static void bc_set_flags(uint32_t *pflags, int *pidx, uint32_t val, int n)
{
    *pflags = *pflags | (val << *pidx);
    *pidx += n;
}

static int bc_atom_to_idx(BCWriterState *s, uint32_t *pres, JSAtom atom)
{
    uint32_t v;

    if (atom < s->first_atom || __JS_AtomIsTaggedInt(atom)) {
        *pres = atom;
        return 0;
    }
    atom -= s->first_atom;
    if (atom < s->atom_to_idx_size && s->atom_to_idx[atom] != 0) {
        *pres = s->atom_to_idx[atom];
        return 0;
    }
    if (atom >= s->atom_to_idx_size) {
        int old_size, i;
        old_size = s->atom_to_idx_size;
        if (js_resize_array(s->ctx, (void **)&s->atom_to_idx,
                            sizeof(s->atom_to_idx[0]), &s->atom_to_idx_size,
                            atom + 1))
            return -1;
        /* XXX: could add a specific js_resize_array() function to do it */
        for(i = old_size; i < s->atom_to_idx_size; i++)
            s->atom_to_idx[i] = 0;
    }
    if (js_resize_array(s->ctx, (void **)&s->idx_to_atom,
                        sizeof(s->idx_to_atom[0]),
                        &s->idx_to_atom_size, s->idx_to_atom_count + 1))
        goto fail;

    v = s->idx_to_atom_count++;
    s->idx_to_atom[v] = atom + s->first_atom;
    v += s->first_atom;
    s->atom_to_idx[atom] = v;
    *pres = v;
    return 0;
    fail:
    *pres = 0;
    return -1;
}

static int bc_put_atom(BCWriterState *s, JSAtom atom)
{
    uint32_t v;

    if (__JS_AtomIsTaggedInt(atom)) {
        v = (__JS_AtomToUInt32(atom) << 1) | 1;
    } else {
        if (bc_atom_to_idx(s, &v, atom))
            return -1;
        v <<= 1;
    }
    bc_put_leb128(s, v);
    return 0;
}

static void bc_byte_swap(uint8_t *bc_buf, int bc_len)
{
    int pos, len, op, fmt;

    pos = 0;
    while (pos < bc_len) {
        op = bc_buf[pos];
        len = short_opcode_info(op).size;
        fmt = short_opcode_info(op).fmt;
        switch(fmt) {
            case OP_FMT_u16:
            case OP_FMT_i16:
            case OP_FMT_label16:
            case OP_FMT_npop:
            case OP_FMT_loc:
            case OP_FMT_arg:
            case OP_FMT_var_ref:
                put_u16(bc_buf + pos + 1,
                        bswap16(get_u16(bc_buf + pos + 1)));
                break;
            case OP_FMT_i32:
            case OP_FMT_u32:
            case OP_FMT_const:
            case OP_FMT_label:
            case OP_FMT_atom:
            case OP_FMT_atom_u8:
                put_u32(bc_buf + pos + 1,
                        bswap32(get_u32(bc_buf + pos + 1)));
                break;
            case OP_FMT_atom_u16:
            case OP_FMT_label_u16:
                put_u32(bc_buf + pos + 1,
                        bswap32(get_u32(bc_buf + pos + 1)));
                put_u16(bc_buf + pos + 1 + 4,
                        bswap16(get_u16(bc_buf + pos + 1 + 4)));
                break;
            case OP_FMT_atom_label_u8:
            case OP_FMT_atom_label_u16:
                put_u32(bc_buf + pos + 1,
                        bswap32(get_u32(bc_buf + pos + 1)));
                put_u32(bc_buf + pos + 1 + 4,
                        bswap32(get_u32(bc_buf + pos + 1 + 4)));
                if (fmt == OP_FMT_atom_label_u16) {
                    put_u16(bc_buf + pos + 1 + 4 + 4,
                            bswap16(get_u16(bc_buf + pos + 1 + 4 + 4)));
                }
                break;
            case OP_FMT_npop_u16:
                put_u16(bc_buf + pos + 1,
                        bswap16(get_u16(bc_buf + pos + 1)));
                put_u16(bc_buf + pos + 1 + 2,
                        bswap16(get_u16(bc_buf + pos + 1 + 2)));
                break;
            default:
                break;
        }
        pos += len;
    }
}

static int JS_WriteFunctionBytecode(BCWriterState *s,
                                    const uint8_t *bc_buf1, int bc_len)
{
    int pos, len, op;
    JSAtom atom;
    uint8_t *bc_buf;
    uint32_t val;

    bc_buf = js_malloc(s->ctx, bc_len);
    if (!bc_buf)
        return -1;
    memcpy(bc_buf, bc_buf1, bc_len);

    pos = 0;
    while (pos < bc_len) {
        op = bc_buf[pos];
        len = short_opcode_info(op).size;
        switch(short_opcode_info(op).fmt) {
            case OP_FMT_atom:
            case OP_FMT_atom_u8:
            case OP_FMT_atom_u16:
            case OP_FMT_atom_label_u8:
            case OP_FMT_atom_label_u16:
                atom = get_u32(bc_buf + pos + 1);
                if (bc_atom_to_idx(s, &val, atom))
                    goto fail;
                put_u32(bc_buf + pos + 1, val);
                break;
            default:
                break;
        }
        pos += len;
    }

    if (s->byte_swap)
        bc_byte_swap(bc_buf, bc_len);

    dbuf_put(&s->dbuf, bc_buf, bc_len);

    js_free(s->ctx, bc_buf);
    return 0;
    fail:
    js_free(s->ctx, bc_buf);
    return -1;
}

static void JS_WriteString(BCWriterState *s, JSString *p)
{
    int i;
    bc_put_leb128(s, ((uint32_t)p->len << 1) | p->is_wide_char);
    if (p->is_wide_char) {
        for(i = 0; i < p->len; i++)
            bc_put_u16(s, p->u.str16[i]);
    } else {
        dbuf_put(&s->dbuf, p->u.str8, p->len);
    }
}

#ifdef CONFIG_BIGNUM
static int JS_WriteBigNum(BCWriterState *s, JSValueConst obj)
{
    uint32_t tag, tag1;
    int64_t e;
    JSBigFloat *bf = JS_VALUE_GET_PTR(obj);
    bf_t *a = &bf->num;
    size_t len, i, n1, j;
    limb_t v;

    tag = JS_VALUE_GET_TAG(obj);
    switch(tag) {
    case JS_TAG_BIG_INT:
        tag1 = BC_TAG_BIG_INT;
        break;
    case JS_TAG_BIG_FLOAT:
        tag1 = BC_TAG_BIG_FLOAT;
        break;
    case JS_TAG_BIG_DECIMAL:
        tag1 = BC_TAG_BIG_DECIMAL;
        break;
    default:
        abort();
    }
    bc_put_u8(s, tag1);

    /* sign + exponent */
    if (a->expn == BF_EXP_ZERO)
        e = 0;
    else if (a->expn == BF_EXP_INF)
        e = 1;
    else if (a->expn == BF_EXP_NAN)
        e = 2;
    else if (a->expn >= 0)
        e = a->expn + 3;
    else
        e = a->expn;
    e = (e << 1) | a->sign;
    if (e < INT32_MIN || e > INT32_MAX) {
        JS_ThrowInternalError(s->ctx, "bignum exponent is too large");
        return -1;
    }
    bc_put_sleb128(s, e);

    /* mantissa */
    if (a->len != 0) {
        if (tag != JS_TAG_BIG_DECIMAL) {
            i = 0;
            while (i < a->len && a->tab[i] == 0)
                i++;
            assert(i < a->len);
            v = a->tab[i];
            n1 = sizeof(limb_t);
            while ((v & 0xff) == 0) {
                n1--;
                v >>= 8;
            }
            i++;
            len = (a->len - i) * sizeof(limb_t) + n1;
            if (len > INT32_MAX) {
                JS_ThrowInternalError(s->ctx, "bignum is too large");
                return -1;
            }
            bc_put_leb128(s, len);
            /* always saved in byte based little endian representation */
            for(j = 0; j < n1; j++) {
                dbuf_putc(&s->dbuf, v >> (j * 8));
            }
            for(; i < a->len; i++) {
                limb_t v = a->tab[i];
#if LIMB_BITS == 32
#ifdef WORDS_BIGENDIAN
                v = bswap32(v);
#endif
                dbuf_put_u32(&s->dbuf, v);
#else
#ifdef WORDS_BIGENDIAN
                v = bswap64(v);
#endif
                dbuf_put_u64(&s->dbuf, v);
#endif
            }
        } else {
            int bpos, d;
            uint8_t v8;
            size_t i0;

            /* little endian BCD */
            i = 0;
            while (i < a->len && a->tab[i] == 0)
                i++;
            assert(i < a->len);
            len = a->len * LIMB_DIGITS;
            v = a->tab[i];
            j = 0;
            while ((v % 10) == 0) {
                j++;
                v /= 10;
            }
            len -= j;
            assert(len > 0);
            if (len > INT32_MAX) {
                JS_ThrowInternalError(s->ctx, "bignum is too large");
                return -1;
            }
            bc_put_leb128(s, len);

            bpos = 0;
            v8 = 0;
            i0 = i;
            for(; i < a->len; i++) {
                if (i != i0) {
                    v = a->tab[i];
                    j = 0;
                }
                for(; j < LIMB_DIGITS; j++) {
                    d = v % 10;
                    v /= 10;
                    if (bpos == 0) {
                        v8 = d;
                        bpos = 1;
                    } else {
                        dbuf_putc(&s->dbuf, v8 | (d << 4));
                        bpos = 0;
                    }
                }
            }
            /* flush the last digit */
            if (bpos) {
                dbuf_putc(&s->dbuf, v8);
            }
        }
    }
    return 0;
}
#endif /* CONFIG_BIGNUM */

static int JS_WriteObjectRec(BCWriterState *s, JSValueConst obj);

static int JS_WriteFunctionTag(BCWriterState *s, JSValueConst obj)
{
    JSFunctionBytecode *b = JS_VALUE_GET_PTR(obj);
    uint32_t flags;
    int idx, i;

    bc_put_u8(s, BC_TAG_FUNCTION_BYTECODE);
    flags = idx = 0;
    bc_set_flags(&flags, &idx, b->has_prototype, 1);
    bc_set_flags(&flags, &idx, b->has_simple_parameter_list, 1);
    bc_set_flags(&flags, &idx, b->is_derived_class_constructor, 1);
    bc_set_flags(&flags, &idx, b->need_home_object, 1);
    bc_set_flags(&flags, &idx, b->func_kind, 2);
    bc_set_flags(&flags, &idx, b->new_target_allowed, 1);
    bc_set_flags(&flags, &idx, b->super_call_allowed, 1);
    bc_set_flags(&flags, &idx, b->super_allowed, 1);
    bc_set_flags(&flags, &idx, b->arguments_allowed, 1);
    bc_set_flags(&flags, &idx, b->has_debug, 1);
    bc_set_flags(&flags, &idx, b->backtrace_barrier, 1);
    assert(idx <= 16);
    bc_put_u16(s, flags);
    bc_put_u8(s, b->js_mode);
    bc_put_atom(s, b->func_name);

    bc_put_leb128(s, b->arg_count);
    bc_put_leb128(s, b->var_count);
    bc_put_leb128(s, b->defined_arg_count);
    bc_put_leb128(s, b->stack_size);
    bc_put_leb128(s, b->closure_var_count);
    bc_put_leb128(s, b->cpool_count);
    bc_put_leb128(s, b->byte_code_len);
    if (b->vardefs) {
        bc_put_leb128(s, b->arg_count + b->var_count);
        for(i = 0; i < b->arg_count + b->var_count; i++) {
            JSVarDef *vd = &b->vardefs[i];
            bc_put_atom(s, vd->var_name);
            bc_put_leb128(s, vd->scope_level);
            bc_put_leb128(s, vd->scope_next + 1);
            flags = idx = 0;
            bc_set_flags(&flags, &idx, vd->var_kind, 4);
            bc_set_flags(&flags, &idx, vd->is_func_var, 1);
            bc_set_flags(&flags, &idx, vd->is_const, 1);
            bc_set_flags(&flags, &idx, vd->is_lexical, 1);
            bc_set_flags(&flags, &idx, vd->is_captured, 1);
            assert(idx <= 8);
            bc_put_u8(s, flags);
        }
    } else {
        bc_put_leb128(s, 0);
    }

    for(i = 0; i < b->closure_var_count; i++) {
        JSClosureVar *cv = &b->closure_var[i];
        bc_put_atom(s, cv->var_name);
        bc_put_leb128(s, cv->var_idx);
        flags = idx = 0;
        bc_set_flags(&flags, &idx, cv->is_local, 1);
        bc_set_flags(&flags, &idx, cv->is_arg, 1);
        bc_set_flags(&flags, &idx, cv->is_const, 1);
        bc_set_flags(&flags, &idx, cv->is_lexical, 1);
        bc_set_flags(&flags, &idx, cv->var_kind, 4);
        assert(idx <= 8);
        bc_put_u8(s, flags);
    }

    if (JS_WriteFunctionBytecode(s, b->byte_code_buf, b->byte_code_len))
        goto fail;

    if (b->has_debug) {
        bc_put_atom(s, b->debug.filename);
        bc_put_leb128(s, b->debug.line_num);
        bc_put_leb128(s, b->debug.pc2line_len);
        dbuf_put(&s->dbuf, b->debug.pc2line_buf, b->debug.pc2line_len);
    }

    for(i = 0; i < b->cpool_count; i++) {
        if (JS_WriteObjectRec(s, b->cpool[i]))
            goto fail;
    }
    return 0;
    fail:
    return -1;
}

static int JS_WriteModule(BCWriterState *s, JSValueConst obj)
{
    JSModuleDef *m = JS_VALUE_GET_PTR(obj);
    int i;

    bc_put_u8(s, BC_TAG_MODULE);
    bc_put_atom(s, m->module_name);

    bc_put_leb128(s, m->req_module_entries_count);
    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        bc_put_atom(s, rme->module_name);
    }

    bc_put_leb128(s, m->export_entries_count);
    for(i = 0; i < m->export_entries_count; i++) {
        JSExportEntry *me = &m->export_entries[i];
        bc_put_u8(s, me->export_type);
        if (me->export_type == JS_EXPORT_TYPE_LOCAL) {
            bc_put_leb128(s, me->u.local.var_idx);
        } else {
            bc_put_leb128(s, me->u.req_module_idx);
            bc_put_atom(s, me->local_name);
        }
        bc_put_atom(s, me->export_name);
    }

    bc_put_leb128(s, m->star_export_entries_count);
    for(i = 0; i < m->star_export_entries_count; i++) {
        JSStarExportEntry *se = &m->star_export_entries[i];
        bc_put_leb128(s, se->req_module_idx);
    }

    bc_put_leb128(s, m->import_entries_count);
    for(i = 0; i < m->import_entries_count; i++) {
        JSImportEntry *mi = &m->import_entries[i];
        bc_put_leb128(s, mi->var_idx);
        bc_put_atom(s, mi->import_name);
        bc_put_leb128(s, mi->req_module_idx);
    }

    if (JS_WriteObjectRec(s, m->func_obj))
        goto fail;
    return 0;
    fail:
    return -1;
}

static int JS_WriteArray(BCWriterState *s, JSValueConst obj)
{
    JSObject *p = JS_VALUE_GET_OBJ(obj);
    uint32_t i, len;
    JSValue val;
    int ret;
    BOOL is_template;

    if (s->allow_bytecode && !p->extensible) {
        /* not extensible array: we consider it is a
           template when we are saving bytecode */
        bc_put_u8(s, BC_TAG_TEMPLATE_OBJECT);
        is_template = TRUE;
    } else {
        bc_put_u8(s, BC_TAG_ARRAY);
        is_template = FALSE;
    }
    if (js_get_length32(s->ctx, &len, obj))
        goto fail1;
    bc_put_leb128(s, len);
    for(i = 0; i < len; i++) {
        val = JS_GetPropertyUint32(s->ctx, obj, i);
        if (JS_IsException(val))
            goto fail1;
        ret = JS_WriteObjectRec(s, val);
        JS_FreeValue(s->ctx, val);
        if (ret)
            goto fail1;
    }
    if (is_template) {
        val = JS_GetProperty(s->ctx, obj, JS_ATOM_raw);
        if (JS_IsException(val))
            goto fail1;
        ret = JS_WriteObjectRec(s, val);
        JS_FreeValue(s->ctx, val);
        if (ret)
            goto fail1;
    }
    return 0;
    fail1:
    return -1;
}

static int JS_WriteObjectTag(BCWriterState *s, JSValueConst obj)
{
    JSObject *p = JS_VALUE_GET_OBJ(obj);
    uint32_t i, prop_count;
    JSShape *sh;
    JSShapeProperty *pr;
    int pass;
    JSAtom atom;

    bc_put_u8(s, BC_TAG_OBJECT);
    prop_count = 0;
    sh = p->shape;
    for(pass = 0; pass < 2; pass++) {
        if (pass == 1)
            bc_put_leb128(s, prop_count);
        for(i = 0, pr = get_shape_prop(sh); i < sh->prop_count; i++, pr++) {
            atom = pr->atom;
            if (atom != JS_ATOM_NULL &&
                JS_AtomIsString(s->ctx, atom) &&
                (pr->flags & JS_PROP_ENUMERABLE)) {
                if (pr->flags & JS_PROP_TMASK) {
                    JS_ThrowTypeError(s->ctx, "only value properties are supported");
                    goto fail;
                }
                if (pass == 0) {
                    prop_count++;
                } else {
                    bc_put_atom(s, atom);
                    if (JS_WriteObjectRec(s, p->prop[i].u.value))
                        goto fail;
                }
            }
        }
    }
    return 0;
    fail:
    return -1;
}

static int JS_WriteTypedArray(BCWriterState *s, JSValueConst obj)
{
    JSObject *p = JS_VALUE_GET_OBJ(obj);
    JSTypedArray *ta = p->u.typed_array;

    bc_put_u8(s, BC_TAG_TYPED_ARRAY);
    bc_put_u8(s, p->class_id - JS_CLASS_UINT8C_ARRAY);
    bc_put_leb128(s, p->u.array.count);
    bc_put_leb128(s, ta->offset);
    if (JS_WriteObjectRec(s, JS_MKPTR(JS_TAG_OBJECT, ta->buffer)))
        return -1;
    return 0;
}

static int JS_WriteArrayBuffer(BCWriterState *s, JSValueConst obj)
{
    JSObject *p = JS_VALUE_GET_OBJ(obj);
    JSArrayBuffer *abuf = p->u.array_buffer;
    if (abuf->detached) {
        JS_ThrowTypeErrorDetachedArrayBuffer(s->ctx);
        return -1;
    }
    bc_put_u8(s, BC_TAG_ARRAY_BUFFER);
    bc_put_leb128(s, abuf->byte_length);
    dbuf_put(&s->dbuf, abuf->data, abuf->byte_length);
    return 0;
}

static int JS_WriteSharedArrayBuffer(BCWriterState *s, JSValueConst obj)
{
    JSObject *p = JS_VALUE_GET_OBJ(obj);
    JSArrayBuffer *abuf = p->u.array_buffer;
    assert(!abuf->detached); /* SharedArrayBuffer are never detached */
    bc_put_u8(s, BC_TAG_SHARED_ARRAY_BUFFER);
    bc_put_leb128(s, abuf->byte_length);
    bc_put_u64(s, (uintptr_t)abuf->data);
    if (js_resize_array(s->ctx, (void **)&s->sab_tab, sizeof(s->sab_tab[0]),
                        &s->sab_tab_size, s->sab_tab_len + 1))
        return -1;
    /* keep the SAB pointer so that the user can clone it or free it */
    s->sab_tab[s->sab_tab_len++] = abuf->data;
    return 0;
}

static int JS_WriteObjectRec(BCWriterState *s, JSValueConst obj)
{
    uint32_t tag;

    if (js_check_stack_overflow(s->ctx->rt, 0)) {
        JS_ThrowStackOverflow(s->ctx);
        return -1;
    }

    tag = JS_VALUE_GET_NORM_TAG(obj);
    switch(tag) {
        case JS_TAG_NULL:
            bc_put_u8(s, BC_TAG_NULL);
            break;
        case JS_TAG_UNDEFINED:
            bc_put_u8(s, BC_TAG_UNDEFINED);
            break;
        case JS_TAG_BOOL:
            bc_put_u8(s, BC_TAG_BOOL_FALSE + JS_VALUE_GET_INT(obj));
            break;
        case JS_TAG_INT:
            bc_put_u8(s, BC_TAG_INT32);
            bc_put_sleb128(s, JS_VALUE_GET_INT(obj));
            break;
        case JS_TAG_FLOAT64:
        {
            JSFloat64Union u;
            bc_put_u8(s, BC_TAG_FLOAT64);
            u.d = JS_VALUE_GET_FLOAT64(obj);
            bc_put_u64(s, u.u64);
        }
            break;
        case JS_TAG_STRING:
        {
            JSString *p = JS_VALUE_GET_STRING(obj);
            bc_put_u8(s, BC_TAG_STRING);
            JS_WriteString(s, p);
        }
            break;
        case JS_TAG_FUNCTION_BYTECODE:
            if (!s->allow_bytecode)
                goto invalid_tag;
            if (JS_WriteFunctionTag(s, obj))
                goto fail;
            break;
        case JS_TAG_MODULE:
            if (!s->allow_bytecode)
                goto invalid_tag;
            if (JS_WriteModule(s, obj))
                goto fail;
            break;
        case JS_TAG_OBJECT:
        {
            JSObject *p = JS_VALUE_GET_OBJ(obj);
            int ret, idx;

            if (s->allow_reference) {
                idx = js_object_list_find(s->ctx, &s->object_list, p);
                if (idx >= 0) {
                    bc_put_u8(s, BC_TAG_OBJECT_REFERENCE);
                    bc_put_leb128(s, idx);
                    break;
                } else {
                    if (js_object_list_add(s->ctx, &s->object_list, p))
                        goto fail;
                }
            } else {
                if (p->tmp_mark) {
                    JS_ThrowTypeError(s->ctx, "circular reference");
                    goto fail;
                }
                p->tmp_mark = 1;
            }
            switch(p->class_id) {
                case JS_CLASS_ARRAY:
                    ret = JS_WriteArray(s, obj);
                    break;
                case JS_CLASS_OBJECT:
                    ret = JS_WriteObjectTag(s, obj);
                    break;
                case JS_CLASS_ARRAY_BUFFER:
                    ret = JS_WriteArrayBuffer(s, obj);
                    break;
                case JS_CLASS_SHARED_ARRAY_BUFFER:
                    if (!s->allow_sab)
                        goto invalid_tag;
                    ret = JS_WriteSharedArrayBuffer(s, obj);
                    break;
                case JS_CLASS_DATE:
                    bc_put_u8(s, BC_TAG_DATE);
                    ret = JS_WriteObjectRec(s, p->u.object_data);
                    break;
                case JS_CLASS_NUMBER:
                case JS_CLASS_STRING:
                case JS_CLASS_BOOLEAN:
#ifdef CONFIG_BIGNUM
                    case JS_CLASS_BIG_INT:
            case JS_CLASS_BIG_FLOAT:
            case JS_CLASS_BIG_DECIMAL:
#endif
                    bc_put_u8(s, BC_TAG_OBJECT_VALUE);
                    ret = JS_WriteObjectRec(s, p->u.object_data);
                    break;
                default:
                    if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                        p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                        ret = JS_WriteTypedArray(s, obj);
                    } else {
                        JS_ThrowTypeError(s->ctx, "unsupported object class");
                        ret = -1;
                    }
                    break;
            }
            p->tmp_mark = 0;
            if (ret)
                goto fail;
        }
            break;
#ifdef CONFIG_BIGNUM
            case JS_TAG_BIG_INT:
    case JS_TAG_BIG_FLOAT:
    case JS_TAG_BIG_DECIMAL:
        if (JS_WriteBigNum(s, obj))
            goto fail;
        break;
#endif
        default:
        invalid_tag:
            JS_ThrowInternalError(s->ctx, "unsupported tag (%d)", tag);
            goto fail;
    }
    return 0;

    fail:
    return -1;
}

/* create the atom table */
static int JS_WriteObjectAtoms(BCWriterState *s)
{
    JSRuntime *rt = s->ctx->rt;
    DynBuf dbuf1;
    int i, atoms_size;
    uint8_t version;

    dbuf1 = s->dbuf;
    js_dbuf_init(s->ctx, &s->dbuf);

    version = BC_VERSION;
    if (s->byte_swap)
        version ^= BC_BE_VERSION;
    bc_put_u8(s, version);

    bc_put_leb128(s, s->idx_to_atom_count);
    for(i = 0; i < s->idx_to_atom_count; i++) {
        JSAtomStruct *p = rt->atom_array[s->idx_to_atom[i]];
        JS_WriteString(s, p);
    }
    /* XXX: should check for OOM in above phase */

    /* move the atoms at the start */
    /* XXX: could just append dbuf1 data, but it uses more memory if
       dbuf1 is larger than dbuf */
    atoms_size = s->dbuf.size;
    if (dbuf_realloc(&dbuf1, dbuf1.size + atoms_size))
        goto fail;
    memmove(dbuf1.buf + atoms_size, dbuf1.buf, dbuf1.size);
    memcpy(dbuf1.buf, s->dbuf.buf, atoms_size);
    dbuf1.size += atoms_size;
    dbuf_free(&s->dbuf);
    s->dbuf = dbuf1;
    return 0;
    fail:
    dbuf_free(&dbuf1);
    return -1;
}

uint8_t *JS_WriteObject2(JSContext *ctx, size_t *psize, JSValueConst obj,
                         int flags, uint8_t ***psab_tab, size_t *psab_tab_len)
{
    BCWriterState ss, *s = &ss;

    memset(s, 0, sizeof(*s));
    s->ctx = ctx;
    /* XXX: byte swapped output is untested */
    s->byte_swap = ((flags & JS_WRITE_OBJ_BSWAP) != 0);
    s->allow_bytecode = ((flags & JS_WRITE_OBJ_BYTECODE) != 0);
    s->allow_sab = ((flags & JS_WRITE_OBJ_SAB) != 0);
    s->allow_reference = ((flags & JS_WRITE_OBJ_REFERENCE) != 0);
    /* XXX: could use a different version when bytecode is included */
    if (s->allow_bytecode)
        s->first_atom = JS_ATOM_END;
    else
        s->first_atom = 1;
    js_dbuf_init(ctx, &s->dbuf);
    js_object_list_init(&s->object_list);

    if (JS_WriteObjectRec(s, obj))
        goto fail;
    if (JS_WriteObjectAtoms(s))
        goto fail;
    js_object_list_end(ctx, &s->object_list);
    js_free(ctx, s->atom_to_idx);
    js_free(ctx, s->idx_to_atom);
    *psize = s->dbuf.size;
    if (psab_tab)
        *psab_tab = s->sab_tab;
    if (psab_tab_len)
        *psab_tab_len = s->sab_tab_len;
    return s->dbuf.buf;
    fail:
    js_object_list_end(ctx, &s->object_list);
    js_free(ctx, s->atom_to_idx);
    js_free(ctx, s->idx_to_atom);
    dbuf_free(&s->dbuf);
    *psize = 0;
    if (psab_tab)
        *psab_tab = NULL;
    if (psab_tab_len)
        *psab_tab_len = 0;
    return NULL;
}

uint8_t *JS_WriteObject(JSContext *ctx, size_t *psize, JSValueConst obj,
                        int flags)
{
    return JS_WriteObject2(ctx, psize, obj, flags, NULL, NULL);
}

typedef struct BCReaderState {
    JSContext *ctx;
    const uint8_t *buf_start, *ptr, *buf_end;
    uint32_t first_atom;
    uint32_t idx_to_atom_count;
    JSAtom *idx_to_atom;
    int error_state;
    BOOL allow_sab : 8;
    BOOL allow_bytecode : 8;
    BOOL is_rom_data : 8;
    BOOL allow_reference : 8;
    /* object references */
    JSObject **objects;
    int objects_count;
    int objects_size;

#ifdef DUMP_READ_OBJECT
    const uint8_t *ptr_last;
    int level;
#endif
} BCReaderState;

#ifdef DUMP_READ_OBJECT
static void __js_printf_like(2, 3) bc_read_trace(BCReaderState *s, const char *fmt, ...) {
    va_list ap;
    int i, n, n0;

    if (!s->ptr_last)
        s->ptr_last = s->buf_start;

    n = n0 = 0;
    if (s->ptr > s->ptr_last || s->ptr == s->buf_start) {
        n0 = printf("%04x: ", (int)(s->ptr_last - s->buf_start));
        n += n0;
    }
    for (i = 0; s->ptr_last < s->ptr; i++) {
        if ((i & 7) == 0 && i > 0) {
            printf("\n%*s", n0, "");
            n = n0;
        }
        n += printf(" %02x", *s->ptr_last++);
    }
    if (*fmt == '}')
        s->level--;
    if (n < 32 + s->level * 2) {
        printf("%*s", 32 + s->level * 2 - n, "");
    }
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    if (strchr(fmt, '{'))
        s->level++;
}
#else
#define bc_read_trace(...)
#endif

static int bc_read_error_end(BCReaderState *s)
{
    if (!s->error_state) {
        JS_ThrowSyntaxError(s->ctx, "read after the end of the buffer");
    }
    return s->error_state = -1;
}

static int bc_get_u8(BCReaderState *s, uint8_t *pval)
{
    if (unlikely(s->buf_end - s->ptr < 1)) {
        *pval = 0; /* avoid warning */
        return bc_read_error_end(s);
    }
    *pval = *s->ptr++;
    return 0;
}

static int bc_get_u16(BCReaderState *s, uint16_t *pval)
{
    if (unlikely(s->buf_end - s->ptr < 2)) {
        *pval = 0; /* avoid warning */
        return bc_read_error_end(s);
    }
    *pval = get_u16(s->ptr);
    s->ptr += 2;
    return 0;
}

static __maybe_unused int bc_get_u32(BCReaderState *s, uint32_t *pval)
{
    if (unlikely(s->buf_end - s->ptr < 4)) {
        *pval = 0; /* avoid warning */
        return bc_read_error_end(s);
    }
    *pval = get_u32(s->ptr);
    s->ptr += 4;
    return 0;
}

static int bc_get_u64(BCReaderState *s, uint64_t *pval)
{
    if (unlikely(s->buf_end - s->ptr < 8)) {
        *pval = 0; /* avoid warning */
        return bc_read_error_end(s);
    }
    *pval = get_u64(s->ptr);
    s->ptr += 8;
    return 0;
}

static int bc_get_leb128(BCReaderState *s, uint32_t *pval)
{
    int ret;
    ret = get_leb128(pval, s->ptr, s->buf_end);
    if (unlikely(ret < 0))
        return bc_read_error_end(s);
    s->ptr += ret;
    return 0;
}

static int bc_get_sleb128(BCReaderState *s, int32_t *pval)
{
    int ret;
    ret = get_sleb128(pval, s->ptr, s->buf_end);
    if (unlikely(ret < 0))
        return bc_read_error_end(s);
    s->ptr += ret;
    return 0;
}

/* XXX: used to read an `int` with a positive value */
static int bc_get_leb128_int(BCReaderState *s, int *pval)
{
    return bc_get_leb128(s, (uint32_t *)pval);
}

static int bc_get_leb128_u16(BCReaderState *s, uint16_t *pval)
{
    uint32_t val;
    if (bc_get_leb128(s, &val)) {
        *pval = 0;
        return -1;
    }
    *pval = val;
    return 0;
}

static int bc_get_buf(BCReaderState *s, uint8_t *buf, uint32_t buf_len)
{
    if (buf_len != 0) {
        if (unlikely(!buf || s->buf_end - s->ptr < buf_len))
            return bc_read_error_end(s);
        memcpy(buf, s->ptr, buf_len);
        s->ptr += buf_len;
    }
    return 0;
}

static int bc_idx_to_atom(BCReaderState *s, JSAtom *patom, uint32_t idx)
{
    JSAtom atom;

    if (__JS_AtomIsTaggedInt(idx)) {
        atom = idx;
    } else if (idx < s->first_atom) {
        atom = JS_DupAtom(s->ctx, idx);
    } else {
        idx -= s->first_atom;
        if (idx >= s->idx_to_atom_count) {
            JS_ThrowSyntaxError(s->ctx, "invalid atom index (pos=%u)",
                                (unsigned int)(s->ptr - s->buf_start));
            *patom = JS_ATOM_NULL;
            return s->error_state = -1;
        }
        atom = JS_DupAtom(s->ctx, s->idx_to_atom[idx]);
    }
    *patom = atom;
    return 0;
}

static int bc_get_atom(BCReaderState *s, JSAtom *patom)
{
    uint32_t v;
    if (bc_get_leb128(s, &v))
        return -1;
    if (v & 1) {
        *patom = __JS_AtomFromUInt32(v >> 1);
        return 0;
    } else {
        return bc_idx_to_atom(s, patom, v >> 1);
    }
}

static JSString *JS_ReadString(BCReaderState *s)
{
    uint32_t len;
    size_t size;
    BOOL is_wide_char;
    JSString *p;

    if (bc_get_leb128(s, &len))
        return NULL;
    is_wide_char = len & 1;
    len >>= 1;
    p = js_alloc_string(s->ctx, len, is_wide_char);
    if (!p) {
        s->error_state = -1;
        return NULL;
    }
    size = (size_t)len << is_wide_char;
    if ((s->buf_end - s->ptr) < size) {
        bc_read_error_end(s);
        js_free_string(s->ctx->rt, p);
        return NULL;
    }
    memcpy(p->u.str8, s->ptr, size);
    s->ptr += size;
    if (!is_wide_char) {
        p->u.str8[size] = '\0'; /* add the trailing zero for 8 bit strings */
    }
#ifdef DUMP_READ_OBJECT
    JS_DumpString(s->ctx->rt, p); printf("\n");
#endif
    return p;
}

static uint32_t bc_get_flags(uint32_t flags, int *pidx, int n)
{
    uint32_t val;
    /* XXX: this does not work for n == 32 */
    val = (flags >> *pidx) & ((1U << n) - 1);
    *pidx += n;
    return val;
}

static int JS_ReadFunctionBytecode(BCReaderState *s, JSFunctionBytecode *b,
                                   int byte_code_offset, uint32_t bc_len)
{
    uint8_t *bc_buf;
    int pos, len, op;
    JSAtom atom;
    uint32_t idx;

    if (s->is_rom_data) {
        /* directly use the input buffer */
        if (unlikely(s->buf_end - s->ptr < bc_len))
            return bc_read_error_end(s);
        bc_buf = (uint8_t *)s->ptr;
        s->ptr += bc_len;
    } else {
        bc_buf = (void *)((uint8_t*)b + byte_code_offset);
        if (bc_get_buf(s, bc_buf, bc_len))
            return -1;
    }
    b->byte_code_buf = bc_buf;

    pos = 0;
    while (pos < bc_len) {
        op = bc_buf[pos];
        len = short_opcode_info(op).size;
        switch(short_opcode_info(op).fmt) {
            case OP_FMT_atom:
            case OP_FMT_atom_u8:
            case OP_FMT_atom_u16:
            case OP_FMT_atom_label_u8:
            case OP_FMT_atom_label_u16:
                idx = get_u32(bc_buf + pos + 1);
                if (s->is_rom_data) {
                    /* just increment the reference count of the atom */
                    JS_DupAtom(s->ctx, (JSAtom)idx);
                } else {
                    if (bc_idx_to_atom(s, &atom, idx)) {
                        /* Note: the atoms will be freed up to this position */
                        b->byte_code_len = pos;
                        return -1;
                    }
                    put_u32(bc_buf + pos + 1, atom);
#ifdef DUMP_READ_OBJECT
                    bc_read_trace(s, "at %d, fixup atom: ", pos + 1); print_atom(s->ctx, atom); printf("\n");
#endif
                }
                break;
            default:
                break;
        }
        pos += len;
    }
    return 0;
}

#ifdef CONFIG_BIGNUM
static JSValue JS_ReadBigNum(BCReaderState *s, int tag)
{
    JSValue obj = JS_UNDEFINED;
    uint8_t v8;
    int32_t e;
    uint32_t len;
    limb_t l, i, n, j;
    JSBigFloat *p;
    limb_t v;
    bf_t *a;
    int bpos, d;

    p = js_new_bf(s->ctx);
    if (!p)
        goto fail;
    switch(tag) {
    case BC_TAG_BIG_INT:
        obj = JS_MKPTR(JS_TAG_BIG_INT, p);
        break;
    case BC_TAG_BIG_FLOAT:
        obj = JS_MKPTR(JS_TAG_BIG_FLOAT, p);
        break;
    case BC_TAG_BIG_DECIMAL:
        obj = JS_MKPTR(JS_TAG_BIG_DECIMAL, p);
        break;
    default:
        abort();
    }

    /* sign + exponent */
    if (bc_get_sleb128(s, &e))
        goto fail;

    a = &p->num;
    a->sign = e & 1;
    e >>= 1;
    if (e == 0)
        a->expn = BF_EXP_ZERO;
    else if (e == 1)
        a->expn = BF_EXP_INF;
    else if (e == 2)
        a->expn = BF_EXP_NAN;
    else if (e >= 3)
        a->expn = e - 3;
    else
        a->expn = e;

    /* mantissa */
    if (a->expn != BF_EXP_ZERO &&
        a->expn != BF_EXP_INF &&
        a->expn != BF_EXP_NAN) {
        if (bc_get_leb128(s, &len))
            goto fail;
        bc_read_trace(s, "len=%" PRId64 "\n", (int64_t)len);
        if (len == 0) {
            JS_ThrowInternalError(s->ctx, "invalid bignum length");
            goto fail;
        }
        if (tag != BC_TAG_BIG_DECIMAL)
            l = (len + sizeof(limb_t) - 1) / sizeof(limb_t);
        else
            l = (len + LIMB_DIGITS - 1) / LIMB_DIGITS;
        if (bf_resize(a, l)) {
            JS_ThrowOutOfMemory(s->ctx);
            goto fail;
        }
        if (tag != BC_TAG_BIG_DECIMAL) {
            n = len & (sizeof(limb_t) - 1);
            if (n != 0) {
                v = 0;
                for(i = 0; i < n; i++) {
                    if (bc_get_u8(s, &v8))
                        goto fail;
                    v |= (limb_t)v8 << ((sizeof(limb_t) - n + i) * 8);
                }
                a->tab[0] = v;
                i = 1;
            } else {
                i = 0;
            }
            for(; i < l; i++) {
#if LIMB_BITS == 32
                if (bc_get_u32(s, &v))
                    goto fail;
#ifdef WORDS_BIGENDIAN
                v = bswap32(v);
#endif
#else
                if (bc_get_u64(s, &v))
                    goto fail;
#ifdef WORDS_BIGENDIAN
                v = bswap64(v);
#endif
#endif
                a->tab[i] = v;
            }
        } else {
            bpos = 0;
            for(i = 0; i < l; i++) {
                if (i == 0 && (n = len % LIMB_DIGITS) != 0) {
                    j = LIMB_DIGITS - n;
                } else {
                    j = 0;
                }
                v = 0;
                for(; j < LIMB_DIGITS; j++) {
                    if (bpos == 0) {
                        if (bc_get_u8(s, &v8))
                            goto fail;
                        d = v8 & 0xf;
                        bpos = 1;
                    } else {
                        d = v8 >> 4;
                        bpos = 0;
                    }
                    if (d >= 10) {
                        JS_ThrowInternalError(s->ctx, "invalid digit");
                        goto fail;
                    }
                    v += mp_pow_dec[j] * d;
                }
                a->tab[i] = v;
            }
        }
    }
    bc_read_trace(s, "}\n");
    return obj;
 fail:
    JS_FreeValue(s->ctx, obj);
    return JS_EXCEPTION;
}
#endif /* CONFIG_BIGNUM */

static JSValue JS_ReadObjectRec(BCReaderState *s);

static int BC_add_object_ref1(BCReaderState *s, JSObject *p)
{
    if (s->allow_reference) {
        if (js_resize_array(s->ctx, (void *)&s->objects,
                            sizeof(s->objects[0]),
                            &s->objects_size, s->objects_count + 1))
            return -1;
        s->objects[s->objects_count++] = p;
    }
    return 0;
}

static int BC_add_object_ref(BCReaderState *s, JSValueConst obj)
{
    return BC_add_object_ref1(s, JS_VALUE_GET_OBJ(obj));
}

static JSValue JS_ReadFunctionTag(BCReaderState *s)
{
    JSContext *ctx = s->ctx;
    JSFunctionBytecode bc, *b;
    JSValue obj = JS_UNDEFINED;
    uint16_t v16;
    uint8_t v8;
    int idx, i, local_count;
    int function_size, cpool_offset, byte_code_offset;
    int closure_var_offset, vardefs_offset;

    memset(&bc, 0, sizeof(bc));
    bc.header.ref_count = 1;
    //bc.gc_header.mark = 0;

    if (bc_get_u16(s, &v16))
        goto fail;
    idx = 0;
    bc.has_prototype = bc_get_flags(v16, &idx, 1);
    bc.has_simple_parameter_list = bc_get_flags(v16, &idx, 1);
    bc.is_derived_class_constructor = bc_get_flags(v16, &idx, 1);
    bc.need_home_object = bc_get_flags(v16, &idx, 1);
    bc.func_kind = bc_get_flags(v16, &idx, 2);
    bc.new_target_allowed = bc_get_flags(v16, &idx, 1);
    bc.super_call_allowed = bc_get_flags(v16, &idx, 1);
    bc.super_allowed = bc_get_flags(v16, &idx, 1);
    bc.arguments_allowed = bc_get_flags(v16, &idx, 1);
    bc.has_debug = bc_get_flags(v16, &idx, 1);
    bc.backtrace_barrier = bc_get_flags(v16, &idx, 1);
    bc.read_only_bytecode = s->is_rom_data;
    if (bc_get_u8(s, &v8))
        goto fail;
    bc.js_mode = v8;
    if (bc_get_atom(s, &bc.func_name))  //@ atom leak if failure
        goto fail;
    if (bc_get_leb128_u16(s, &bc.arg_count))
        goto fail;
    if (bc_get_leb128_u16(s, &bc.var_count))
        goto fail;
    if (bc_get_leb128_u16(s, &bc.defined_arg_count))
        goto fail;
    if (bc_get_leb128_u16(s, &bc.stack_size))
        goto fail;
    if (bc_get_leb128_int(s, &bc.closure_var_count))
        goto fail;
    if (bc_get_leb128_int(s, &bc.cpool_count))
        goto fail;
    if (bc_get_leb128_int(s, &bc.byte_code_len))
        goto fail;
    if (bc_get_leb128_int(s, &local_count))
        goto fail;

    if (bc.has_debug) {
        function_size = sizeof(*b);
    } else {
        function_size = offsetof(JSFunctionBytecode, debug);
    }
    cpool_offset = function_size;
    function_size += bc.cpool_count * sizeof(*bc.cpool);
    vardefs_offset = function_size;
    function_size += local_count * sizeof(*bc.vardefs);
    closure_var_offset = function_size;
    function_size += bc.closure_var_count * sizeof(*bc.closure_var);
    byte_code_offset = function_size;
    if (!bc.read_only_bytecode) {
        function_size += bc.byte_code_len;
    }

    b = js_mallocz(ctx, function_size);
    if (!b)
        return JS_EXCEPTION;

    memcpy(b, &bc, offsetof(JSFunctionBytecode, debug));
    b->header.ref_count = 1;
    add_gc_object(ctx->rt, &b->header, JS_GC_OBJ_TYPE_FUNCTION_BYTECODE);

    obj = JS_MKPTR(JS_TAG_FUNCTION_BYTECODE, b);

#ifdef DUMP_READ_OBJECT
    bc_read_trace(s, "name: "); print_atom(s->ctx, b->func_name); printf("\n");
#endif
    bc_read_trace(s, "args=%d vars=%d defargs=%d closures=%d cpool=%d\n",
                  b->arg_count, b->var_count, b->defined_arg_count,
                  b->closure_var_count, b->cpool_count);
    bc_read_trace(s, "stack=%d bclen=%d locals=%d\n",
                  b->stack_size, b->byte_code_len, local_count);

    if (local_count != 0) {
        bc_read_trace(s, "vars {\n");
        b->vardefs = (void *)((uint8_t*)b + vardefs_offset);
        for(i = 0; i < local_count; i++) {
            JSVarDef *vd = &b->vardefs[i];
            if (bc_get_atom(s, &vd->var_name))
                goto fail;
            if (bc_get_leb128_int(s, &vd->scope_level))
                goto fail;
            if (bc_get_leb128_int(s, &vd->scope_next))
                goto fail;
            vd->scope_next--;
            if (bc_get_u8(s, &v8))
                goto fail;
            idx = 0;
            vd->var_kind = bc_get_flags(v8, &idx, 4);
            vd->is_func_var = bc_get_flags(v8, &idx, 1);
            vd->is_const = bc_get_flags(v8, &idx, 1);
            vd->is_lexical = bc_get_flags(v8, &idx, 1);
            vd->is_captured = bc_get_flags(v8, &idx, 1);
#ifdef DUMP_READ_OBJECT
            bc_read_trace(s, "name: "); print_atom(s->ctx, vd->var_name); printf("\n");
#endif
        }
        bc_read_trace(s, "}\n");
    }
    if (b->closure_var_count != 0) {
        bc_read_trace(s, "closure vars {\n");
        b->closure_var = (void *)((uint8_t*)b + closure_var_offset);
        for(i = 0; i < b->closure_var_count; i++) {
            JSClosureVar *cv = &b->closure_var[i];
            int var_idx;
            if (bc_get_atom(s, &cv->var_name))
                goto fail;
            if (bc_get_leb128_int(s, &var_idx))
                goto fail;
            cv->var_idx = var_idx;
            if (bc_get_u8(s, &v8))
                goto fail;
            idx = 0;
            cv->is_local = bc_get_flags(v8, &idx, 1);
            cv->is_arg = bc_get_flags(v8, &idx, 1);
            cv->is_const = bc_get_flags(v8, &idx, 1);
            cv->is_lexical = bc_get_flags(v8, &idx, 1);
            cv->var_kind = bc_get_flags(v8, &idx, 4);
#ifdef DUMP_READ_OBJECT
            bc_read_trace(s, "name: "); print_atom(s->ctx, cv->var_name); printf("\n");
#endif
        }
        bc_read_trace(s, "}\n");
    }
    {
        bc_read_trace(s, "bytecode {\n");
        if (JS_ReadFunctionBytecode(s, b, byte_code_offset, b->byte_code_len))
            goto fail;
        bc_read_trace(s, "}\n");
    }
    if (b->has_debug) {
        /* read optional debug information */
        bc_read_trace(s, "debug {\n");
        if (bc_get_atom(s, &b->debug.filename))
            goto fail;
        if (bc_get_leb128_int(s, &b->debug.line_num))
            goto fail;
        if (bc_get_leb128_int(s, &b->debug.pc2line_len))
            goto fail;
        if (b->debug.pc2line_len) {
            b->debug.pc2line_buf = js_mallocz(ctx, b->debug.pc2line_len);
            if (!b->debug.pc2line_buf)
                goto fail;
            if (bc_get_buf(s, b->debug.pc2line_buf, b->debug.pc2line_len))
                goto fail;
        }
#ifdef DUMP_READ_OBJECT
            bc_read_trace(s, "filename: "); print_atom(s->ctx, b->debug.filename); printf("\n");
#endif
        bc_read_trace(s, "}\n");
    }
    if (b->cpool_count != 0) {
        bc_read_trace(s, "cpool {\n");
        b->cpool = (void *)((uint8_t*)b + cpool_offset);
        for(i = 0; i < b->cpool_count; i++) {
            JSValue val;
            val = JS_ReadObjectRec(s);
            if (JS_IsException(val))
                goto fail;
            b->cpool[i] = val;
        }
        bc_read_trace(s, "}\n");
    }
    b->realm = JS_DupContext(ctx);
    return obj;
    fail:
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

static JSValue JS_ReadModule(BCReaderState *s)
{
    JSContext *ctx = s->ctx;
    JSValue obj;
    JSModuleDef *m = NULL;
    JSAtom module_name;
    int i;
    uint8_t v8;

    if (bc_get_atom(s, &module_name))
        goto fail;
#ifdef DUMP_READ_OBJECT
    bc_read_trace(s, "name: "); print_atom(s->ctx, module_name); printf("\n");
#endif
    m = js_new_module_def(ctx, module_name);
    if (!m)
        goto fail;
    obj = JS_DupValue(ctx, JS_MKPTR(JS_TAG_MODULE, m));
    if (bc_get_leb128_int(s, &m->req_module_entries_count))
        goto fail;
    if (m->req_module_entries_count != 0) {
        m->req_module_entries_size = m->req_module_entries_count;
        m->req_module_entries = js_mallocz(ctx, sizeof(m->req_module_entries[0]) * m->req_module_entries_size);
        if (!m->req_module_entries)
            goto fail;
        for(i = 0; i < m->req_module_entries_count; i++) {
            JSReqModuleEntry *rme = &m->req_module_entries[i];
            if (bc_get_atom(s, &rme->module_name))
                goto fail;
        }
    }

    if (bc_get_leb128_int(s, &m->export_entries_count))
        goto fail;
    if (m->export_entries_count != 0) {
        m->export_entries_size = m->export_entries_count;
        m->export_entries = js_mallocz(ctx, sizeof(m->export_entries[0]) * m->export_entries_size);
        if (!m->export_entries)
            goto fail;
        for(i = 0; i < m->export_entries_count; i++) {
            JSExportEntry *me = &m->export_entries[i];
            if (bc_get_u8(s, &v8))
                goto fail;
            me->export_type = v8;
            if (me->export_type == JS_EXPORT_TYPE_LOCAL) {
                if (bc_get_leb128_int(s, &me->u.local.var_idx))
                    goto fail;
            } else {
                if (bc_get_leb128_int(s, &me->u.req_module_idx))
                    goto fail;
                if (bc_get_atom(s, &me->local_name))
                    goto fail;
            }
            if (bc_get_atom(s, &me->export_name))
                goto fail;
        }
    }

    if (bc_get_leb128_int(s, &m->star_export_entries_count))
        goto fail;
    if (m->star_export_entries_count != 0) {
        m->star_export_entries_size = m->star_export_entries_count;
        m->star_export_entries = js_mallocz(ctx, sizeof(m->star_export_entries[0]) * m->star_export_entries_size);
        if (!m->star_export_entries)
            goto fail;
        for(i = 0; i < m->star_export_entries_count; i++) {
            JSStarExportEntry *se = &m->star_export_entries[i];
            if (bc_get_leb128_int(s, &se->req_module_idx))
                goto fail;
        }
    }

    if (bc_get_leb128_int(s, &m->import_entries_count))
        goto fail;
    if (m->import_entries_count != 0) {
        m->import_entries_size = m->import_entries_count;
        m->import_entries = js_mallocz(ctx, sizeof(m->import_entries[0]) * m->import_entries_size);
        if (!m->import_entries)
            goto fail;
        for(i = 0; i < m->import_entries_count; i++) {
            JSImportEntry *mi = &m->import_entries[i];
            if (bc_get_leb128_int(s, &mi->var_idx))
                goto fail;
            if (bc_get_atom(s, &mi->import_name))
                goto fail;
            if (bc_get_leb128_int(s, &mi->req_module_idx))
                goto fail;
        }
    }

    m->func_obj = JS_ReadObjectRec(s);
    if (JS_IsException(m->func_obj))
        goto fail;
    return obj;
    fail:
    if (m) {
        js_free_module_def(ctx, m);
    }
    return JS_EXCEPTION;
}

static JSValue JS_ReadObjectTag(BCReaderState *s)
{
    JSContext *ctx = s->ctx;
    JSValue obj;
    uint32_t prop_count, i;
    JSAtom atom;
    JSValue val;
    int ret;

    obj = JS_NewObject(ctx);
    if (BC_add_object_ref(s, obj))
        goto fail;
    if (bc_get_leb128(s, &prop_count))
        goto fail;
    for(i = 0; i < prop_count; i++) {
        if (bc_get_atom(s, &atom))
            goto fail;
#ifdef DUMP_READ_OBJECT
        bc_read_trace(s, "propname: "); print_atom(s->ctx, atom); printf("\n");
#endif
        val = JS_ReadObjectRec(s);
        if (JS_IsException(val)) {
            JS_FreeAtom(ctx, atom);
            goto fail;
        }
        ret = JS_DefinePropertyValue(ctx, obj, atom, val, JS_PROP_C_W_E);
        JS_FreeAtom(ctx, atom);
        if (ret < 0)
            goto fail;
    }
    return obj;
    fail:
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

static JSValue JS_ReadArray(BCReaderState *s, int tag)
{
    JSContext *ctx = s->ctx;
    JSValue obj;
    uint32_t len, i;
    JSValue val;
    int ret, prop_flags;
    BOOL is_template;

    obj = JS_NewArray(ctx);
    if (BC_add_object_ref(s, obj))
        goto fail;
    is_template = (tag == BC_TAG_TEMPLATE_OBJECT);
    if (bc_get_leb128(s, &len))
        goto fail;
    for(i = 0; i < len; i++) {
        val = JS_ReadObjectRec(s);
        if (JS_IsException(val))
            goto fail;
        if (is_template)
            prop_flags = JS_PROP_ENUMERABLE;
        else
            prop_flags = JS_PROP_C_W_E;
        ret = JS_DefinePropertyValueUint32(ctx, obj, i, val,
                                           prop_flags);
        if (ret < 0)
            goto fail;
    }
    if (is_template) {
        val = JS_ReadObjectRec(s);
        if (JS_IsException(val))
            goto fail;
        if (!JS_IsUndefined(val)) {
            ret = JS_DefinePropertyValue(ctx, obj, JS_ATOM_raw, val, 0);
            if (ret < 0)
                goto fail;
        }
        JS_PreventExtensions(ctx, obj);
    }
    return obj;
    fail:
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

static JSValue JS_ReadTypedArray(BCReaderState *s)
{
    JSContext *ctx = s->ctx;
    JSValue obj = JS_UNDEFINED, array_buffer = JS_UNDEFINED;
    uint8_t array_tag;
    JSValueConst args[3];
    uint32_t offset, len, idx;

    if (bc_get_u8(s, &array_tag))
        return JS_EXCEPTION;
    if (array_tag >= JS_TYPED_ARRAY_COUNT)
        return JS_ThrowTypeError(ctx, "invalid typed array");
    if (bc_get_leb128(s, &len))
        return JS_EXCEPTION;
    if (bc_get_leb128(s, &offset))
        return JS_EXCEPTION;
    /* XXX: this hack could be avoided if the typed array could be
       created before the array buffer */
    idx = s->objects_count;
    if (BC_add_object_ref1(s, NULL))
        goto fail;
    array_buffer = JS_ReadObjectRec(s);
    if (JS_IsException(array_buffer))
        return JS_EXCEPTION;
    if (!js_get_array_buffer(ctx, array_buffer)) {
        JS_FreeValue(ctx, array_buffer);
        return JS_EXCEPTION;
    }
    args[0] = array_buffer;
    args[1] = JS_NewInt64(ctx, offset);
    args[2] = JS_NewInt64(ctx, len);
    obj = js_typed_array_constructor(ctx, JS_UNDEFINED,
                                     3, args,
                                     JS_CLASS_UINT8C_ARRAY + array_tag);
    if (JS_IsException(obj))
        goto fail;
    if (s->allow_reference) {
        s->objects[idx] = JS_VALUE_GET_OBJ(obj);
    }
    JS_FreeValue(ctx, array_buffer);
    return obj;
    fail:
    JS_FreeValue(ctx, array_buffer);
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

static JSValue JS_ReadArrayBuffer(BCReaderState *s)
{
    JSContext *ctx = s->ctx;
    uint32_t byte_length;
    JSValue obj;

    if (bc_get_leb128(s, &byte_length))
        return JS_EXCEPTION;
    if (unlikely(s->buf_end - s->ptr < byte_length)) {
        bc_read_error_end(s);
        return JS_EXCEPTION;
    }
    obj = JS_NewArrayBufferCopy(ctx, s->ptr, byte_length);
    if (JS_IsException(obj))
        goto fail;
    if (BC_add_object_ref(s, obj))
        goto fail;
    s->ptr += byte_length;
    return obj;
    fail:
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

static JSValue JS_ReadSharedArrayBuffer(BCReaderState *s)
{
    JSContext *ctx = s->ctx;
    uint32_t byte_length;
    uint8_t *data_ptr;
    JSValue obj;
    uint64_t u64;

    if (bc_get_leb128(s, &byte_length))
        return JS_EXCEPTION;
    if (bc_get_u64(s, &u64))
        return JS_EXCEPTION;
    data_ptr = (uint8_t *)(uintptr_t)u64;
    /* the SharedArrayBuffer is cloned */
    obj = js_array_buffer_constructor3(ctx, JS_UNDEFINED, byte_length,
                                       JS_CLASS_SHARED_ARRAY_BUFFER,
                                       data_ptr,
                                       NULL, NULL, FALSE);
    if (JS_IsException(obj))
        goto fail;
    if (BC_add_object_ref(s, obj))
        goto fail;
    return obj;
    fail:
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

static JSValue JS_ReadDate(BCReaderState *s)
{
    JSContext *ctx = s->ctx;
    JSValue val, obj = JS_UNDEFINED;

    val = JS_ReadObjectRec(s);
    if (JS_IsException(val))
        goto fail;
    if (!JS_IsNumber(val)) {
        JS_ThrowTypeError(ctx, "Number tag expected for date");
        goto fail;
    }
    obj = JS_NewObjectProtoClass(ctx, ctx->class_proto[JS_CLASS_DATE],
                                 JS_CLASS_DATE);
    if (JS_IsException(obj))
        goto fail;
    if (BC_add_object_ref(s, obj))
        goto fail;
    JS_SetObjectData(ctx, obj, val);
    return obj;
    fail:
    JS_FreeValue(ctx, val);
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

static JSValue JS_ReadObjectValue(BCReaderState *s)
{
    JSContext *ctx = s->ctx;
    JSValue val, obj = JS_UNDEFINED;

    val = JS_ReadObjectRec(s);
    if (JS_IsException(val))
        goto fail;
    obj = JS_ToObject(ctx, val);
    if (JS_IsException(obj))
        goto fail;
    if (BC_add_object_ref(s, obj))
        goto fail;
    JS_FreeValue(ctx, val);
    return obj;
    fail:
    JS_FreeValue(ctx, val);
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

static JSValue JS_ReadObjectRec(BCReaderState *s)
{
    JSContext *ctx = s->ctx;
    uint8_t tag;
    JSValue obj = JS_UNDEFINED;

    if (js_check_stack_overflow(ctx->rt, 0))
        return JS_ThrowStackOverflow(ctx);

    if (bc_get_u8(s, &tag))
        return JS_EXCEPTION;

    bc_read_trace(s, "%s {\n", bc_tag_str[tag]);

    switch(tag) {
        case BC_TAG_NULL:
            obj = JS_NULL;
            break;
        case BC_TAG_UNDEFINED:
            obj = JS_UNDEFINED;
            break;
        case BC_TAG_BOOL_FALSE:
        case BC_TAG_BOOL_TRUE:
            obj = JS_NewBool(ctx, tag - BC_TAG_BOOL_FALSE);
            break;
        case BC_TAG_INT32:
        {
            int32_t val;
            if (bc_get_sleb128(s, &val))
                return JS_EXCEPTION;
            bc_read_trace(s, "%d\n", val);
            obj = JS_NewInt32(ctx, val);
        }
            break;
        case BC_TAG_FLOAT64:
        {
            JSFloat64Union u;
            if (bc_get_u64(s, &u.u64))
                return JS_EXCEPTION;
            bc_read_trace(s, "%g\n", u.d);
            obj = __JS_NewFloat64(ctx, u.d);
        }
            break;
        case BC_TAG_STRING:
        {
            JSString *p;
            p = JS_ReadString(s);
            if (!p)
                return JS_EXCEPTION;
            obj = JS_MKPTR(JS_TAG_STRING, p);
        }
            break;
        case BC_TAG_FUNCTION_BYTECODE:
            if (!s->allow_bytecode)
                goto invalid_tag;
            obj = JS_ReadFunctionTag(s);
            break;
        case BC_TAG_MODULE:
            if (!s->allow_bytecode)
                goto invalid_tag;
            obj = JS_ReadModule(s);
            break;
        case BC_TAG_OBJECT:
            obj = JS_ReadObjectTag(s);
            break;
        case BC_TAG_ARRAY:
        case BC_TAG_TEMPLATE_OBJECT:
            obj = JS_ReadArray(s, tag);
            break;
        case BC_TAG_TYPED_ARRAY:
            obj = JS_ReadTypedArray(s);
            break;
        case BC_TAG_ARRAY_BUFFER:
            obj = JS_ReadArrayBuffer(s);
            break;
        case BC_TAG_SHARED_ARRAY_BUFFER:
            if (!s->allow_sab || !ctx->rt->sab_funcs.sab_dup)
                goto invalid_tag;
            obj = JS_ReadSharedArrayBuffer(s);
            break;
        case BC_TAG_DATE:
            obj = JS_ReadDate(s);
            break;
        case BC_TAG_OBJECT_VALUE:
            obj = JS_ReadObjectValue(s);
            break;
#ifdef CONFIG_BIGNUM
            case BC_TAG_BIG_INT:
    case BC_TAG_BIG_FLOAT:
    case BC_TAG_BIG_DECIMAL:
        obj = JS_ReadBigNum(s, tag);
        break;
#endif
        case BC_TAG_OBJECT_REFERENCE:
        {
            uint32_t val;
            if (!s->allow_reference)
                return JS_ThrowSyntaxError(ctx, "object references are not allowed");
            if (bc_get_leb128(s, &val))
                return JS_EXCEPTION;
            bc_read_trace(s, "%u\n", val);
            if (val >= s->objects_count) {
                return JS_ThrowSyntaxError(ctx, "invalid object reference (%u >= %u)",
                                           val, s->objects_count);
            }
            obj = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, s->objects[val]));
        }
            break;
        default:
        invalid_tag:
            return JS_ThrowSyntaxError(ctx, "invalid tag (tag=%d pos=%u)",
                                       tag, (unsigned int)(s->ptr - s->buf_start));
    }
    bc_read_trace(s, "}\n");
    return obj;
}

static int JS_ReadObjectAtoms(BCReaderState *s)
{
    uint8_t v8;
    JSString *p;
    int i;
    JSAtom atom;

    if (bc_get_u8(s, &v8))
        return -1;
    /* XXX: could support byte swapped input */
    if (v8 != BC_VERSION) {
        JS_ThrowSyntaxError(s->ctx, "invalid version (%d expected=%d)",
                            v8, BC_VERSION);
        return -1;
    }
    if (bc_get_leb128(s, &s->idx_to_atom_count))
        return -1;

    bc_read_trace(s, "%d atom indexes {\n", s->idx_to_atom_count);

    if (s->idx_to_atom_count != 0) {
        s->idx_to_atom = js_mallocz(s->ctx, s->idx_to_atom_count *
                                            sizeof(s->idx_to_atom[0]));
        if (!s->idx_to_atom)
            return s->error_state = -1;
    }
    for(i = 0; i < s->idx_to_atom_count; i++) {
        p = JS_ReadString(s);
        if (!p)
            return -1;
        atom = JS_NewAtomStr(s->ctx, p);
        if (atom == JS_ATOM_NULL)
            return s->error_state = -1;
        s->idx_to_atom[i] = atom;
        if (s->is_rom_data && (atom != (i + s->first_atom)))
            s->is_rom_data = FALSE; /* atoms must be relocated */
    }
    bc_read_trace(s, "}\n");
    return 0;
}

static void bc_reader_free(BCReaderState *s)
{
    int i;
    if (s->idx_to_atom) {
        for(i = 0; i < s->idx_to_atom_count; i++) {
            JS_FreeAtom(s->ctx, s->idx_to_atom[i]);
        }
        js_free(s->ctx, s->idx_to_atom);
    }
    js_free(s->ctx, s->objects);
}

JSValue JS_ReadObject(JSContext *ctx, const uint8_t *buf, size_t buf_len,
                      int flags)
{
    BCReaderState ss, *s = &ss;
    JSValue obj;

    ctx->binary_object_count += 1;
    ctx->binary_object_size += buf_len;

    memset(s, 0, sizeof(*s));
    s->ctx = ctx;
    s->buf_start = buf;
    s->buf_end = buf + buf_len;
    s->ptr = buf;
    s->allow_bytecode = ((flags & JS_READ_OBJ_BYTECODE) != 0);
    s->is_rom_data = ((flags & JS_READ_OBJ_ROM_DATA) != 0);
    s->allow_sab = ((flags & JS_READ_OBJ_SAB) != 0);
    s->allow_reference = ((flags & JS_READ_OBJ_REFERENCE) != 0);
    if (s->allow_bytecode)
        s->first_atom = JS_ATOM_END;
    else
        s->first_atom = 1;
    if (JS_ReadObjectAtoms(s)) {
        obj = JS_EXCEPTION;
    } else {
        obj = JS_ReadObjectRec(s);
    }
    bc_reader_free(s);
    return obj;
}
#endif //QUICKJS_QUICKJS_PARSER_H
