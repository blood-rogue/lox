#ifndef TOML_H
#define TOML_H
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif
#ifdef __cplusplus
#define TOML_EXTERN extern "C"
#else
#define TOML_EXTERN extern
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct toml_table_t toml_table_t;
typedef struct toml_array_t toml_array_t;
typedef struct toml_value_t toml_value_t;
typedef struct toml_timestamp_t toml_timestamp_t;
typedef struct toml_keyval_t toml_keyval_t;
typedef struct toml_arritem_t toml_arritem_t;

struct toml_table_t {
    const char *key;
    int keylen;
    bool implicit;
    bool readonly;

    int nkval;
    toml_keyval_t **kval;
    int narr;
    toml_array_t **arr;
    int ntab;
    toml_table_t **tab;
};

struct toml_array_t {
    const char *key;
    int keylen;
    int kind;
    int type;

    int nitem;
    toml_arritem_t *item;
};
struct toml_arritem_t {
    int valtype;
    char *val;
    toml_array_t *arr;
    toml_table_t *tab;
};

struct toml_keyval_t {
    const char *key;
    int keylen;
    const char *val;
};

struct toml_value_t {
    bool ok;
    union {
        toml_timestamp_t *ts;
        char *s;
        int sl;
        bool b;
        int64_t i;
        double d;
    } u;
};

struct toml_timestamp_t {
    char kind;
    int year, month, day;
    int hour, minute, second, millisec;
    char *z;
};

TOML_EXTERN toml_table_t *toml_parse(char *toml, char *errbuf, int errbufsz);
TOML_EXTERN toml_table_t *toml_parse_file(FILE *fp, char *errbuf, int errbufsz);
TOML_EXTERN void toml_free(toml_table_t *table);

TOML_EXTERN int toml_table_len(const toml_table_t *table);
TOML_EXTERN const char *toml_table_key(const toml_table_t *table, int keyidx, int *keylen);
TOML_EXTERN toml_value_t toml_table_string(const toml_table_t *table, const char *key);
TOML_EXTERN toml_value_t toml_table_bool(const toml_table_t *table, const char *key);
TOML_EXTERN toml_value_t toml_table_int(const toml_table_t *table, const char *key);
TOML_EXTERN toml_value_t toml_table_double(const toml_table_t *table, const char *key);
TOML_EXTERN toml_value_t toml_table_timestamp(const toml_table_t *table, const char *key);
TOML_EXTERN toml_array_t *toml_table_array(const toml_table_t *table, const char *key);
TOML_EXTERN toml_table_t *toml_table_table(const toml_table_t *table, const char *key);

TOML_EXTERN int toml_array_len(const toml_array_t *array);
TOML_EXTERN toml_value_t toml_array_string(const toml_array_t *array, int idx);
TOML_EXTERN toml_value_t toml_array_bool(const toml_array_t *array, int idx);
TOML_EXTERN toml_value_t toml_array_int(const toml_array_t *array, int idx);
TOML_EXTERN toml_value_t toml_array_double(const toml_array_t *array, int idx);
TOML_EXTERN toml_value_t toml_array_timestamp(const toml_array_t *array, int idx);
TOML_EXTERN toml_array_t *toml_array_array(const toml_array_t *array, int idx);
TOML_EXTERN toml_table_t *toml_array_table(const toml_array_t *array, int idx);

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN8(sz)   (((sz) + 7) & ~7)
#define calloc(x, y) error - forbidden - use CALLOC instead
static void *CALLOC(size_t nmemb, size_t sz) {
    int nb = ALIGN8(sz) * nmemb;
    void *p = malloc(nb);
    if (p) {
        memset(p, 0, nb);
    }
    return p;
}

#undef strdup
#define strdup(x) error - forbidden - use STRDUP instead
static char *STRDUP(const char *s) {
    int len = strlen(s);
    char *p = malloc(len + 1);
    if (p) {
        memcpy(p, s, len);
        p[len] = 0;
    }
    return p;
}

#undef strndup
#define strndup(x) error - forbiden - use STRNDUP instead
static char *STRNDUP(const char *s, size_t n) {
    size_t len = strnlen(s, n);
    char *p = malloc(len + 1);
    if (p) {
        memcpy(p, s, len);
        p[len] = 0;
    }
    return p;
}

typedef const char *toml_unparsed_t;
toml_unparsed_t toml_table_unparsed(const toml_table_t *table, const char *key);
toml_unparsed_t toml_array_unparsed(const toml_array_t *array, int idx);
int toml_value_string(toml_unparsed_t s, char **ret, int *len);
int toml_value_bool(toml_unparsed_t s, bool *ret);
int toml_value_int(toml_unparsed_t s, int64_t *ret);
int toml_value_double(toml_unparsed_t s, double *ret);
int toml_value_timestamp(toml_unparsed_t s, toml_timestamp_t *ret);

int read_unicode_escape(int64_t code, char buf[6]) {
    if (0xd800 <= code && code <= 0xdfff)
        return -1;
    if (0x10FFFF < code)
        return -1;
    if (code < 0)
        return -1;
    if (code <= 0x7F) {
        buf[0] = (unsigned char)code;
        return 1;
    }
    if (code <= 0x000007FF) {
        buf[0] = (unsigned char)(0xc0 | (code >> 6));
        buf[1] = (unsigned char)(0x80 | (code & 0x3f));
        return 2;
    }
    if (code <= 0x0000FFFF) {
        buf[0] = (unsigned char)(0xe0 | (code >> 12));
        buf[1] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
        buf[2] = (unsigned char)(0x80 | (code & 0x3f));
        return 3;
    }
    if (code <= 0x001FFFFF) {
        buf[0] = (unsigned char)(0xf0 | (code >> 18));
        buf[1] = (unsigned char)(0x80 | ((code >> 12) & 0x3f));
        buf[2] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
        buf[3] = (unsigned char)(0x80 | (code & 0x3f));
        return 4;
    }
    if (code <= 0x03FFFFFF) {
        buf[0] = (unsigned char)(0xf8 | (code >> 24));
        buf[1] = (unsigned char)(0x80 | ((code >> 18) & 0x3f));
        buf[2] = (unsigned char)(0x80 | ((code >> 12) & 0x3f));
        buf[3] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
        buf[4] = (unsigned char)(0x80 | (code & 0x3f));
        return 5;
    }
    if (code <= 0x7FFFFFFF) {
        buf[0] = (unsigned char)(0xfc | (code >> 30));
        buf[1] = (unsigned char)(0x80 | ((code >> 24) & 0x3f));
        buf[2] = (unsigned char)(0x80 | ((code >> 18) & 0x3f));
        buf[3] = (unsigned char)(0x80 | ((code >> 12) & 0x3f));
        buf[4] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
        buf[5] = (unsigned char)(0x80 | (code & 0x3f));
        return 6;
    }
    return -1;
}

static inline void xfree(const void *x) {
    if (x)
        free((void *)(intptr_t)x);
}

enum tokentype_t {
    INVALID,
    DOT,
    COMMA,
    EQUAL,
    LBRACE,
    RBRACE,
    NEWLINE,
    LBRACKET,
    RBRACKET,
    STRING,
};
typedef enum tokentype_t tokentype_t;

typedef struct token_t token_t;
struct token_t {
    tokentype_t tok;
    int lineno;
    char *ptr;
    int len;
    int eof;
};

typedef struct context_t context_t;
struct context_t {
    char *start;
    char *stop;
    char *errbuf;
    int errbufsz;

    token_t tok;
    toml_table_t *root;
    toml_table_t *curtab;

    struct {
        int top;
        char *key[10];
        int keylen[10];
        token_t tok[10];
    } tpath;
};

#define STRINGIFY(x) #x
#define TOSTRING(x)  STRINGIFY(x)
#define FLINE        __FILE__ ":" TOSTRING(__LINE__)

static int next_token(context_t *ctx, bool dotisspecial);

static int e_outofmemory(context_t *ctx, const char *fline) {
    snprintf(ctx->errbuf, ctx->errbufsz, "ERROR: out of memory (%s)", fline);
    return -1;
}

static int e_internal(context_t *ctx, const char *fline) {
    snprintf(ctx->errbuf, ctx->errbufsz, "internal error (%s)", fline);
    return -1;
}

static int e_syntax(context_t *ctx, int lineno, const char *msg) {
    snprintf(ctx->errbuf, ctx->errbufsz, "line %d: %s", lineno, msg);
    return -1;
}

static int e_badkey(context_t *ctx, int lineno) {
    snprintf(ctx->errbuf, ctx->errbufsz, "line %d: bad key", lineno);
    return -1;
}

static int e_keyexists(context_t *ctx, int lineno) {
    snprintf(ctx->errbuf, ctx->errbufsz, "line %d: key exists", lineno);
    return -1;
}

static int e_forbid(context_t *ctx, int lineno, const char *msg) {
    snprintf(ctx->errbuf, ctx->errbufsz, "line %d: %s", lineno, msg);
    return -1;
}

static void *expand(void *p, int sz, int newsz) {
    void *s = malloc(newsz);
    if (!s)
        return 0;

    if (p) {
        memcpy(s, p, sz);
        free(p);
    }
    return s;
}

static void **expand_ptrarr(void **p, int n) {
    void **s = malloc((n + 1) * sizeof(void *));
    if (!s)
        return 0;

    s[n] = 0;
    if (p) {
        memcpy(s, p, n * sizeof(void *));
        free(p);
    }
    return s;
}

static toml_arritem_t *expand_arritem(toml_arritem_t *p, int n) {
    toml_arritem_t *pp = expand(p, n * sizeof(*p), (n + 1) * sizeof(*p));
    if (!pp)
        return 0;

    memset(&pp[n], 0, sizeof(pp[n]));
    return pp;
}

static uint8_t const u8_length[] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 3, 4};
#define u8length(s) u8_length[(((uint8_t *)(s))[0] & 0xFF) >> 4];

static char *norm_lit_str(
    const char *src,
    int srclen,
    int *len,
    bool multiline,
    bool is_key,
    char *errbuf,
    int errbufsz) {
    const char *sp = src;
    const char *sq = src + srclen;
    char *dst = 0;
    int max = 0;
    int off = 0;

    for (;;) {
        if (off >= max - 10) {
            int newmax = max + 50;
            char *x = expand(dst, max, newmax);
            if (!x) {
                xfree(dst);
                snprintf(errbuf, errbufsz, "out of memory");
                return 0;
            }
            dst = x;
            max = newmax;
        }

        if (sp >= sq)
            break;

        uint8_t l = u8length(sp);
        if (l == 0) {
            xfree(dst);
            snprintf(errbuf, errbufsz, "invalid UTF-8 at byte pos %d", off);
            return 0;
        }
        if (l > 1) {
            for (int i = 0; i < l; i++) {
                char ch = *sp++;
                if ((ch & 0x80) != 0x80) {
                    xfree(dst);
                    snprintf(errbuf, errbufsz, "invalid UTF-8 at byte pos %d", off);
                    return 0;
                }
                dst[off++] = ch;
            }
            continue;
        }

        char ch = *sp++;
        if (is_key && ch == '\n') {
            xfree(dst);
            snprintf(errbuf, errbufsz, "literal newlines not allowed in key");
            return 0;
        }

        if ((0 <= ch && ch <= 0x08) || (0x0a <= ch && ch <= 0x1f) || ch == 0x7f) {
            if (!(multiline && (ch == '\r' || ch == '\n'))) {
                xfree(dst);
                snprintf(errbuf, errbufsz, "invalid char U+%04x", ch);
                return 0;
            }
        }

        dst[off++] = ch;
    }

    *len = off;
    dst[off++] = 0;
    return dst;
}

static char *norm_basic_str(
    const char *src,
    int srclen,
    int *len,
    bool multiline,
    bool is_key,
    char *errbuf,
    int errbufsz) {
    const char *sp = src;
    const char *sq = src + srclen;
    char *dst = 0;
    int max = 0;
    int off = 0;

    for (;;) {
        if (off >= max - 10) {
            int newmax = max + 50;
            char *x = expand(dst, max, newmax);
            if (!x) {
                xfree(dst);
                snprintf(errbuf, errbufsz, "out of memory");
                return 0;
            }
            dst = x;
            max = newmax;
        }

        if (sp >= sq)
            break;

        uint8_t l = u8length(sp);
        if (l == 0) {
            xfree(dst);
            snprintf(errbuf, errbufsz, "invalid UTF-8 at byte pos %d", off);
            return 0;
        }
        if (l > 1) {
            for (int i = 0; i < l; i++) {
                char ch = *sp++;
                if ((ch & 0x80) != 0x80) {
                    xfree(dst);
                    snprintf(errbuf, errbufsz, "invalid UTF-8 at byte pos %d", off);
                    return 0;
                }
                dst[off++] = ch;
            }
            continue;
        }

        char ch = *sp++;
        if (is_key && ch == '\n') {
            xfree(dst);
            snprintf(errbuf, errbufsz, "literal newlines not allowed in key");
            return 0;
        }
        if (ch != '\\') {
            if ((ch >= 0 && ch <= 0x08) || (ch >= 0x0a && ch <= 0x1f) || ch == 0x7f) {
                if (!(multiline && (ch == '\r' || ch == '\n'))) {
                    xfree(dst);
                    snprintf(errbuf, errbufsz, "invalid char U+%04x", ch);
                    return 0;
                }
            }

            dst[off++] = ch;
            continue;
        }

        if (sp >= sq) {
            snprintf(errbuf, errbufsz, "last backslash is invalid");
            xfree(dst);
            return 0;
        }

        if (multiline) {
            if (sp[strspn(sp, " \t\r")] == '\n') {
                sp += strspn(sp, " \t\r\n");
                continue;
            }
        }

        ch = *sp++;
        switch (ch) {
            case 'u':
            case 'U':
                {
                    int64_t ucs = 0;
                    int nhex = (ch == 'u' ? 4 : 8);
                    for (int i = 0; i < nhex; i++) {
                        if (sp >= sq) {
                            snprintf(errbuf, errbufsz, "\\%c expects %d hex chars", ch, nhex);
                            xfree(dst);
                            return 0;
                        }
                        ch = *sp++;
                        int v = -1;
                        if ('0' <= ch && ch <= '9')
                            v = ch - '0';
                        else if ('A' <= ch && ch <= 'F')
                            v = ch - 'A' + 10;
                        else if ('a' <= ch && ch <= 'f')
                            v = (ch ^ 0x20) - 'A' + 10;
                        if (v == -1) {
                            snprintf(errbuf, errbufsz, "invalid hex chars for \\u or \\U");
                            xfree(dst);
                            return 0;
                        }
                        ucs = ucs * 16 + v;
                    }
                    int n = read_unicode_escape(ucs, &dst[off]);
                    if (n == -1) {
                        snprintf(errbuf, errbufsz, "illegal ucs code in \\u or \\U");
                        xfree(dst);
                        return 0;
                    }
                    off += n;
                };
                continue;
            case 'b':
                ch = '\b';
                break;
            case 't':
                ch = '\t';
                break;
            case 'n':
                ch = '\n';
                break;
            case 'f':
                ch = '\f';
                break;
            case 'r':
                ch = '\r';
                break;
            case '"':
                ch = '"';
                break;
            case '\\':
                ch = '\\';
                break;
            default:
                snprintf(errbuf, errbufsz, "illegal escape char \\%c", ch);
                xfree(dst);
                return 0;
        }

        dst[off++] = ch;
    }

    *len = off;
    dst[off++] = 0;
    return dst;
}

static char *normalize_key(context_t *ctx, token_t strtok, int *keylen) {
    const char *sp = strtok.ptr;
    const char *sq = strtok.ptr + strtok.len;
    int lineno = strtok.lineno;
    int ch = *sp;
    char *ret;

    if (ch == '\'' || ch == '\"') {
        bool multiline = (sp[1] == ch && sp[2] == ch);
        if (multiline)
            sp += 3, sq -= 3;
        else
            sp++, sq--;

        char ebuf[80];
        if (ch == '\'')
            ret = norm_lit_str(sp, sq - sp, keylen, multiline, true, ebuf, sizeof(ebuf));
        else
            ret = norm_basic_str(sp, sq - sp, keylen, multiline, true, ebuf, sizeof(ebuf));
        if (!ret) {
            e_syntax(ctx, lineno, ebuf);
            return 0;
        }
        return ret;
    }

    *keylen = 0;
    for (const char *c = sp; c != sq; c++) {
        *keylen = *keylen + 1;
        if (isalnum(*c) || *c == '_' || *c == '-')
            continue;
        e_badkey(ctx, lineno);
        return 0;
    }

    if (!(ret = STRNDUP(sp, sq - sp))) {
        e_outofmemory(ctx, FLINE);
        return 0;
    }
    return ret;
}

static int check_key(
    toml_table_t *tab,
    const char *key,
    toml_keyval_t **ret_val,
    toml_array_t **ret_arr,
    toml_table_t **ret_tab) {
    int i;
    void *dummy;

    if (!ret_tab)
        ret_tab = (toml_table_t **)&dummy;
    if (!ret_arr)
        ret_arr = (toml_array_t **)&dummy;
    if (!ret_val)
        ret_val = (toml_keyval_t **)&dummy;

    *ret_tab = 0;
    *ret_arr = 0;
    *ret_val = 0;

    for (i = 0; i < tab->nkval; i++) {
        if (strcmp(key, tab->kval[i]->key) == 0) {
            *ret_val = tab->kval[i];
            return 'v';
        }
    }
    for (i = 0; i < tab->narr; i++) {
        if (strcmp(key, tab->arr[i]->key) == 0) {
            *ret_arr = tab->arr[i];
            return 'a';
        }
    }
    for (i = 0; i < tab->ntab; i++) {
        if (strcmp(key, tab->tab[i]->key) == 0) {
            *ret_tab = tab->tab[i];
            return 't';
        }
    }
    return 0;
}

static int key_kind(toml_table_t *tab, const char *key) { return check_key(tab, key, 0, 0, 0); }

static toml_keyval_t *create_keyval_in_table(context_t *ctx, toml_table_t *tab, token_t keytok) {
    int keylen;
    char *newkey = normalize_key(ctx, keytok, &keylen);
    if (!newkey)
        return 0;

    toml_keyval_t *dest = 0;
    if (key_kind(tab, newkey)) {
        xfree(newkey);
        e_keyexists(ctx, keytok.lineno);
        return 0;
    }

    int n = tab->nkval;
    toml_keyval_t **base;
    if ((base = (toml_keyval_t **)expand_ptrarr((void **)tab->kval, n)) == 0) {
        xfree(newkey);
        e_outofmemory(ctx, FLINE);
        return 0;
    }
    tab->kval = base;

    if ((base[n] = (toml_keyval_t *)CALLOC(1, sizeof(*base[n]))) == 0) {
        xfree(newkey);
        e_outofmemory(ctx, FLINE);
        return 0;
    }

    dest = tab->kval[tab->nkval++];
    dest->key = newkey;
    dest->keylen = keylen;
    return dest;
}

static toml_table_t *create_keytable_in_table(context_t *ctx, toml_table_t *tab, token_t keytok) {
    int keylen;
    char *newkey = normalize_key(ctx, keytok, &keylen);
    if (!newkey)
        return 0;

    toml_table_t *dest = 0;
    if (check_key(tab, newkey, 0, 0, &dest)) {
        xfree(newkey);

        if (dest && dest->implicit) {
            dest->implicit = false;
            return dest;
        }
        e_keyexists(ctx, keytok.lineno);
        return 0;
    }

    int n = tab->ntab;
    toml_table_t **base;
    if ((base = (toml_table_t **)expand_ptrarr((void **)tab->tab, n)) == 0) {
        xfree(newkey);
        e_outofmemory(ctx, FLINE);
        return 0;
    }
    tab->tab = base;

    if ((base[n] = (toml_table_t *)CALLOC(1, sizeof(*base[n]))) == 0) {
        xfree(newkey);
        e_outofmemory(ctx, FLINE);
        return 0;
    }

    dest = tab->tab[tab->ntab++];
    dest->key = newkey;
    dest->keylen = keylen;
    return dest;
}

static toml_array_t *
create_keyarray_in_table(context_t *ctx, toml_table_t *tab, token_t keytok, char kind) {
    int keylen;
    char *newkey = normalize_key(ctx, keytok, &keylen);
    if (!newkey)
        return 0;

    if (key_kind(tab, newkey)) {
        xfree(newkey);
        e_keyexists(ctx, keytok.lineno);
        return 0;
    }

    int n = tab->narr;
    toml_array_t **base;
    if ((base = (toml_array_t **)expand_ptrarr((void **)tab->arr, n)) == 0) {
        xfree(newkey);
        e_outofmemory(ctx, FLINE);
        return 0;
    }
    tab->arr = base;

    if ((base[n] = (toml_array_t *)CALLOC(1, sizeof(*base[n]))) == 0) {
        xfree(newkey);
        e_outofmemory(ctx, FLINE);
        return 0;
    }
    toml_array_t *dest = tab->arr[tab->narr++];

    dest->keylen = keylen;
    dest->key = newkey;
    dest->kind = kind;
    return dest;
}

static toml_arritem_t *create_value_in_array(context_t *ctx, toml_array_t *parent) {
    const int n = parent->nitem;
    toml_arritem_t *base = expand_arritem(parent->item, n);
    if (!base) {
        e_outofmemory(ctx, FLINE);
        return 0;
    }
    parent->item = base;
    parent->nitem++;
    return &parent->item[n];
}

static toml_array_t *create_array_in_array(context_t *ctx, toml_array_t *parent) {
    const int n = parent->nitem;
    toml_arritem_t *base = expand_arritem(parent->item, n);
    if (!base) {
        e_outofmemory(ctx, FLINE);
        return 0;
    }
    toml_array_t *ret = (toml_array_t *)CALLOC(1, sizeof(toml_array_t));
    if (!ret) {
        e_outofmemory(ctx, FLINE);
        return 0;
    }
    base[n].arr = ret;
    parent->item = base;
    parent->nitem++;
    return ret;
}

static toml_table_t *create_table_in_array(context_t *ctx, toml_array_t *parent) {
    int n = parent->nitem;
    toml_arritem_t *base = expand_arritem(parent->item, n);
    if (!base) {
        e_outofmemory(ctx, FLINE);
        return 0;
    }
    toml_table_t *ret = (toml_table_t *)CALLOC(1, sizeof(toml_table_t));
    if (!ret) {
        e_outofmemory(ctx, FLINE);
        return 0;
    }
    base[n].tab = ret;
    parent->item = base;
    parent->nitem++;
    return ret;
}

static int skip_newlines(context_t *ctx, bool isdotspecial) {
    while (ctx->tok.tok == NEWLINE) {
        if (next_token(ctx, isdotspecial))
            return -1;
        if (ctx->tok.eof)
            break;
    }
    return 0;
}

static int parse_keyval(context_t *ctx, toml_table_t *tab);

static inline int eat_token(context_t *ctx, tokentype_t typ, bool isdotspecial, const char *fline) {
    if (ctx->tok.tok != typ)
        return e_internal(ctx, fline);

    if (next_token(ctx, isdotspecial))
        return -1;

    return 0;
}

static int parse_inline_table(context_t *ctx, toml_table_t *tab) {
    if (eat_token(ctx, LBRACE, 1, FLINE))
        return -1;

    for (;;) {
        if (ctx->tok.tok == NEWLINE)
            return e_syntax(ctx, ctx->tok.lineno, "newline not allowed in inline table");

        if (ctx->tok.tok == RBRACE)
            break;

        if (ctx->tok.tok != STRING)
            return e_syntax(ctx, ctx->tok.lineno, "expect a string");

        if (parse_keyval(ctx, tab))
            return -1;

        if (ctx->tok.tok == NEWLINE)
            return e_syntax(ctx, ctx->tok.lineno, "newline not allowed in inline table");

        if (ctx->tok.tok == COMMA) {
            if (eat_token(ctx, COMMA, 1, FLINE))
                return -1;
            continue;
        }
        break;
    }

    if (eat_token(ctx, RBRACE, 1, FLINE))
        return -1;
    tab->readonly = 1;
    return 0;
}

static int valtype(const char *val) {
    toml_timestamp_t ts;
    if (*val == '\'' || *val == '"')
        return 's';
    if (toml_value_bool(val, false) == 0)
        return 'b';
    if (toml_value_int(val, 0) == 0)
        return 'i';
    if (toml_value_double(val, 0) == 0)
        return 'd';
    if (toml_value_timestamp(val, &ts) == 0) {
        if (ts.year && ts.hour)
            return 'T';
        if (ts.year)
            return 'D';
        return 't';
    }
    return 'u';
}

static int parse_array(context_t *ctx, toml_array_t *arr) {
    if (eat_token(ctx, LBRACKET, 0, FLINE))
        return -1;

    for (;;) {
        if (skip_newlines(ctx, 0))
            return -1;

        if (ctx->tok.tok == RBRACKET)
            break;

        switch (ctx->tok.tok) {
            case STRING:
                {
                    if (arr->kind == 0)
                        arr->kind = 'v';
                    else if (arr->kind != 'v')
                        arr->kind = 'm';

                    char *val = ctx->tok.ptr;
                    int vlen = ctx->tok.len;

                    toml_arritem_t *newval = create_value_in_array(ctx, arr);
                    if (!newval)
                        return e_outofmemory(ctx, FLINE);

                    if (!(newval->val = STRNDUP(val, vlen)))
                        return e_outofmemory(ctx, FLINE);

                    newval->valtype = valtype(newval->val);

                    if (arr->nitem == 1)
                        arr->type = newval->valtype;
                    else if (arr->type != newval->valtype)
                        arr->type = 'm';

                    if (eat_token(ctx, ctx->tok.tok, 0, FLINE))
                        return -1;
                    break;
                }
            case LBRACKET:
                {
                    if (arr->kind == 0)
                        arr->kind = 'a';
                    else if (arr->kind != 'a')
                        arr->kind = 'm';

                    toml_array_t *subarr = create_array_in_array(ctx, arr);
                    if (!subarr)
                        return -1;
                    if (parse_array(ctx, subarr))
                        return -1;
                    break;
                }
            case LBRACE:
                {
                    if (arr->kind == 0)
                        arr->kind = 't';
                    else if (arr->kind != 't')
                        arr->kind = 'm';

                    toml_table_t *subtab = create_table_in_array(ctx, arr);
                    if (!subtab)
                        return -1;
                    if (parse_inline_table(ctx, subtab))
                        return -1;
                    break;
                }
            default:
                return e_syntax(ctx, ctx->tok.lineno, "syntax error");
        }

        if (skip_newlines(ctx, 0))
            return -1;

        if (ctx->tok.tok == COMMA) {
            if (eat_token(ctx, COMMA, 0, FLINE))
                return -1;
            continue;
        }
        break;
    }

    if (eat_token(ctx, RBRACKET, 1, FLINE))
        return -1;
    return 0;
}

static int parse_keyval(context_t *ctx, toml_table_t *tab) {
    if (tab->readonly) {
        return e_forbid(ctx, ctx->tok.lineno, "cannot insert new entry into existing table");
    }

    token_t key = ctx->tok;
    if (eat_token(ctx, STRING, 1, FLINE))
        return -1;

    if (ctx->tok.tok == DOT) {
        toml_table_t *subtab = 0;
        {
            int keylen;
            char *subtabstr = normalize_key(ctx, key, &keylen);
            if (!subtabstr)
                return -1;

            subtab = toml_table_table(tab, subtabstr);
            if (subtab)
                subtab->keylen = keylen;
            xfree(subtabstr);
        }
        if (!subtab) {
            subtab = create_keytable_in_table(ctx, tab, key);
            if (!subtab)
                return -1;
        }
        if (next_token(ctx, true))
            return -1;
        if (parse_keyval(ctx, subtab))
            return -1;
        return 0;
    }

    if (ctx->tok.tok != EQUAL)
        return e_syntax(ctx, ctx->tok.lineno, "missing =");

    if (next_token(ctx, false))
        return -1;

    switch (ctx->tok.tok) {
        case STRING:
            {
                toml_keyval_t *keyval = create_keyval_in_table(ctx, tab, key);
                if (!keyval)
                    return -1;
                token_t val = ctx->tok;

                assert(keyval->val == 0);
                if (!(keyval->val = STRNDUP(val.ptr, val.len)))
                    return e_outofmemory(ctx, FLINE);

                if (next_token(ctx, true))
                    return -1;

                return 0;
            }
        case LBRACKET:
            {
                toml_array_t *arr = create_keyarray_in_table(ctx, tab, key, 0);
                if (!arr)
                    return -1;
                if (parse_array(ctx, arr))
                    return -1;
                return 0;
            }
        case LBRACE:
            {
                toml_table_t *nxttab = create_keytable_in_table(ctx, tab, key);
                if (!nxttab)
                    return -1;
                if (parse_inline_table(ctx, nxttab))
                    return -1;
                return 0;
            }
        default:
            return e_syntax(ctx, ctx->tok.lineno, "syntax error");
    }
    return 0;
}

typedef struct tabpath_t tabpath_t;
struct tabpath_t {
    int cnt;
    token_t key[10];
};

static int fill_tabpath(context_t *ctx) {
    for (int i = 0; i < ctx->tpath.top; i++) {
        char **p = &ctx->tpath.key[i];
        xfree(*p);
        *p = 0;
    }
    ctx->tpath.top = 0;

    for (;;) {
        if (ctx->tpath.top >= 10)
            return e_syntax(ctx, ctx->tok.lineno, "table path is too deep; max allowed is 10.");
        if (ctx->tok.tok != STRING)
            return e_syntax(ctx, ctx->tok.lineno, "invalid or missing key");

        int keylen;
        char *key = normalize_key(ctx, ctx->tok, &keylen);
        if (!key)
            return -1;
        ctx->tpath.tok[ctx->tpath.top] = ctx->tok;
        ctx->tpath.key[ctx->tpath.top] = key;
        ctx->tpath.keylen[ctx->tpath.top] = keylen;
        ctx->tpath.top++;

        if (next_token(ctx, true))
            return -1;

        if (ctx->tok.tok == RBRACKET)
            break;
        if (ctx->tok.tok != DOT)
            return e_syntax(ctx, ctx->tok.lineno, "invalid key");
        if (next_token(ctx, true))
            return -1;
    }

    if (ctx->tpath.top <= 0)
        return e_syntax(ctx, ctx->tok.lineno, "empty table selector");
    return 0;
}

static int walk_tabpath(context_t *ctx) {
    toml_table_t *curtab = ctx->root;

    for (int i = 0; i < ctx->tpath.top; i++) {
        const char *key = ctx->tpath.key[i];
        int keylen = ctx->tpath.keylen[i];

        toml_keyval_t *nextval = 0;
        toml_array_t *nextarr = 0;
        toml_table_t *nexttab = 0;
        switch (check_key(curtab, key, &nextval, &nextarr, &nexttab)) {
            case 't':
                break;
            case 'a':
                if (nextarr->kind != 't')
                    return e_internal(ctx, FLINE);

                if (nextarr->nitem == 0)
                    return e_internal(ctx, FLINE);

                nexttab = nextarr->item[nextarr->nitem - 1].tab;
                break;
            case 'v':
                return e_keyexists(ctx, ctx->tpath.tok[i].lineno);
            default:
                {
                    int n = curtab->ntab;
                    toml_table_t **base = (toml_table_t **)expand_ptrarr((void **)curtab->tab, n);
                    if (base == 0)
                        return e_outofmemory(ctx, FLINE);

                    curtab->tab = base;

                    if ((base[n] = (toml_table_t *)CALLOC(1, sizeof(*base[n]))) == 0)
                        return e_outofmemory(ctx, FLINE);

                    if ((base[n]->key = STRDUP(key)) == 0)
                        return e_outofmemory(ctx, FLINE);
                    base[n]->keylen = keylen;

                    nexttab = curtab->tab[curtab->ntab++];

                    nexttab->implicit = true;
                };
                break;
        }
        curtab = nexttab;
    }

    ctx->curtab = curtab;
    return 0;
}

static int parse_select(context_t *ctx) {
    assert(ctx->tok.tok == LBRACKET);

    int llb = (ctx->tok.ptr + 1 < ctx->stop && ctx->tok.ptr[1] == '[');

    if (eat_token(ctx, LBRACKET, 1, FLINE))
        return -1;
    if (llb) {
        assert(ctx->tok.tok == LBRACKET);
        if (eat_token(ctx, LBRACKET, 1, FLINE))
            return -1;
    }

    if (fill_tabpath(ctx))
        return -1;

    token_t z = ctx->tpath.tok[ctx->tpath.top - 1];
    xfree(ctx->tpath.key[ctx->tpath.top - 1]);
    ctx->tpath.top--;

    if (walk_tabpath(ctx))
        return -1;

    if (!llb) {
        toml_table_t *curtab = create_keytable_in_table(ctx, ctx->curtab, z);
        if (!curtab)
            return -1;
        ctx->curtab = curtab;
    } else {
        toml_array_t *arr = 0;
        {
            int keylen;
            char *zstr = normalize_key(ctx, z, &keylen);
            if (!zstr)
                return -1;
            arr = toml_table_array(ctx->curtab, zstr);
            if (arr)
                arr->keylen = keylen;
            xfree(zstr);
        }
        if (!arr) {
            arr = create_keyarray_in_table(ctx, ctx->curtab, z, 't');
            if (!arr)
                return -1;
        }
        if (arr->kind != 't')
            return e_syntax(ctx, z.lineno, "array mismatch");

        toml_table_t *dest;
        {
            toml_table_t *t = create_table_in_array(ctx, arr);
            if (!t)
                return -1;

            if ((t->key = STRDUP("__anon__")) == 0)
                return e_outofmemory(ctx, FLINE);

            dest = t;
        }

        ctx->curtab = dest;
    }

    if (ctx->tok.tok != RBRACKET) {
        return e_syntax(ctx, ctx->tok.lineno, "expects ]");
    }
    if (llb) {
        if (!(ctx->tok.ptr + 1 < ctx->stop && ctx->tok.ptr[1] == ']')) {
            return e_syntax(ctx, ctx->tok.lineno, "expects ]]");
        }
        if (eat_token(ctx, RBRACKET, 1, FLINE))
            return -1;
    }

    if (eat_token(ctx, RBRACKET, 1, FLINE))
        return -1;
    if (ctx->tok.tok != NEWLINE)
        return e_syntax(ctx, ctx->tok.lineno, "extra chars after ] or ]]");
    return 0;
}

toml_table_t *toml_parse(char *toml, char *errbuf, int errbufsz) {
    context_t ctx;

    if (errbufsz <= 0)
        errbufsz = 0;
    if (errbufsz > 0)
        errbuf[0] = 0;

    memset(&ctx, 0, sizeof(ctx));
    ctx.start = toml;
    ctx.stop = ctx.start + strlen(toml);
    ctx.errbuf = errbuf;
    ctx.errbufsz = errbufsz;

    ctx.tok.tok = NEWLINE;
    ctx.tok.lineno = 1;
    ctx.tok.ptr = toml;
    ctx.tok.len = 0;

    if ((ctx.root = CALLOC(1, sizeof(*ctx.root))) == 0) {
        e_outofmemory(&ctx, FLINE);
        return 0;
    }

    ctx.curtab = ctx.root;

    for (token_t tok = ctx.tok; !tok.eof; tok = ctx.tok) {
        switch (tok.tok) {
            case NEWLINE:
                if (next_token(&ctx, true))
                    goto fail;
                break;

            case STRING:
                if (parse_keyval(&ctx, ctx.curtab))
                    goto fail;

                if (ctx.tok.tok != NEWLINE) {
                    e_syntax(&ctx, ctx.tok.lineno, "extra chars after value");
                    goto fail;
                }

                if (eat_token(&ctx, NEWLINE, 1, FLINE))
                    goto fail;
                break;

            case LBRACKET:
                if (parse_select(&ctx))
                    goto fail;
                break;

            default:
                e_syntax(&ctx, tok.lineno, "syntax error");
                goto fail;
        }
    }

    for (int i = 0; i < ctx.tpath.top; i++)
        xfree(ctx.tpath.key[i]);
    return ctx.root;

fail:

    for (int i = 0; i < ctx.tpath.top; i++)
        xfree(ctx.tpath.key[i]);
    toml_free(ctx.root);
    return 0;
}

toml_table_t *toml_parse_file(FILE *fp, char *errbuf, int errbufsz) {
    int bufsz = 0;
    char *buf = 0;
    int off = 0;
    int inc = 1024;

    while (!feof(fp)) {
        if (bufsz == 1024 * 20)
            inc = 1024 * 20;
        if (off == bufsz) {
            int xsz = bufsz + inc;
            char *x = expand(buf, bufsz, xsz);
            if (!x) {
                snprintf(errbuf, errbufsz, "out of memory");
                xfree(buf);
                return 0;
            }
            buf = x;
            bufsz = xsz;
        }

        errno = 0;
        int n = fread(buf + off, 1, bufsz - off, fp);
        if (ferror(fp)) {
            snprintf(errbuf, errbufsz, "%s", (errno ? strerror(errno) : "Error reading file"));
            xfree(buf);
            return 0;
        }
        off += n;
    }

    if (off == bufsz) {
        int xsz = bufsz + 1;
        char *x = expand(buf, bufsz, xsz);
        if (!x) {
            snprintf(errbuf, errbufsz, "out of memory");
            xfree(buf);
            return 0;
        }
        buf = x;
        bufsz = xsz;
    }
    buf[off] = 0;

    toml_table_t *ret = toml_parse(buf, errbuf, errbufsz);
    xfree(buf);
    return ret;
}

static void xfree_kval(toml_keyval_t *p) {
    if (!p)
        return;
    xfree(p->key);
    xfree(p->val);
    xfree(p);
}

static void xfree_tab(toml_table_t *p);

static void xfree_arr(toml_array_t *p) {
    if (!p)
        return;

    xfree(p->key);
    const int n = p->nitem;
    for (int i = 0; i < n; i++) {
        toml_arritem_t *a = &p->item[i];
        if (a->val)
            xfree(a->val);
        else if (a->arr)
            xfree_arr(a->arr);
        else if (a->tab)
            xfree_tab(a->tab);
    }
    xfree(p->item);
    xfree(p);
}

static void xfree_tab(toml_table_t *p) {
    if (!p)
        return;

    xfree(p->key);

    for (int i = 0; i < p->nkval; i++)
        xfree_kval(p->kval[i]);
    xfree(p->kval);

    for (int i = 0; i < p->narr; i++)
        xfree_arr(p->arr[i]);
    xfree(p->arr);

    for (int i = 0; i < p->ntab; i++)
        xfree_tab(p->tab[i]);
    xfree(p->tab);

    xfree(p);
}

void toml_free(toml_table_t *tab) { xfree_tab(tab); }

static void set_token(context_t *ctx, tokentype_t tok, int lineno, char *ptr, int len) {
    token_t t;
    t.tok = tok;
    t.lineno = lineno;
    t.ptr = ptr;
    t.len = len;
    t.eof = 0;
    ctx->tok = t;
}

static void set_eof(context_t *ctx, int lineno) {
    set_token(ctx, NEWLINE, lineno, ctx->stop, 0);
    ctx->tok.eof = 1;
}

static int scan_digits(const char *p, int n) {
    int ret = 0;
    for (; n > 0 && isdigit(*p); n--, p++) {
        ret = 10 * ret + (*p - '0');
    }
    return n ? -1 : ret;
}

static int scan_date(const char *p, int *YY, int *MM, int *DD) {
    int year, month, day;
    year = scan_digits(p, 4);
    month = (year >= 0 && p[4] == '-') ? scan_digits(p + 5, 2) : -1;
    day = (month >= 0 && p[7] == '-') ? scan_digits(p + 8, 2) : -1;
    if (YY)
        *YY = year;
    if (MM)
        *MM = month;
    if (DD)
        *DD = day;
    return (year >= 0 && month >= 0 && day >= 0) ? 0 : -1;
}

static int scan_time(const char *p, int *hh, int *mm, int *ss) {
    int hour = scan_digits(p, 2);
    int minute = (hour >= 0 && p[2] == ':') ? scan_digits(p + 3, 2) : -1;
    int second = (minute >= 0 && p[5] == ':') ? scan_digits(p + 6, 2) : -1;
    if (hh)
        *hh = hour;
    if (mm)
        *mm = minute;
    if (ss)
        *ss = second;
    return (hour >= 0 && minute >= 0 && second >= 0) ? 0 : -1;
}

static int scan_string(context_t *ctx, char *p, int lineno, bool dotisspecial) {
    char *orig = p;

    if (strncmp(p, "'''", 3) == 0) {
        char *q = p + 3;
        while (true) {
            q = strstr(q, "'''");
            if (q == 0)
                return e_syntax(ctx, lineno, "unterminated triple-s-quote");
            int i = 0;
            while (q[3] == '\'') {
                i++;
                if (i >= 3)
                    return e_syntax(ctx, lineno, "too many ''' in triple-s-quote");
                q++;
            }
            break;
        }
        set_token(ctx, STRING, lineno, orig, q + 3 - orig);
        return 0;
    }

    if (strncmp(p, "\"\"\"", 3) == 0) {
        char *q = p + 3;
        while (true) {
            q = strstr(q, "\"\"\"");
            if (q == 0)
                return e_syntax(ctx, lineno, "unterminated triple-d-quote");
            if (q[-1] == '\\') {
                q++;
                continue;
            }
            int i = 0;
            while (q[3] == '\"') {
                i++;
                if (i >= 3)
                    return e_syntax(ctx, lineno, "too many \"\"\" in triple-d-quote");
                q++;
            }
            break;
        }

        int hexreq = 0;
        bool escape = false;
        for (p += 3; p < q; p++) {
            if (escape) {
                escape = false;
                if (strchr("btnfr\"\\", *p))
                    continue;
                if (*p == 'u') {
                    hexreq = 4;
                    continue;
                }
                if (*p == 'U') {
                    hexreq = 8;
                    continue;
                }
                if (p[strspn(p, " \t\r")] == '\n')
                    continue;
                return e_syntax(ctx, lineno, "bad escape char");
            }
            if (hexreq) {
                hexreq--;
                if (strchr("0123456789ABCDEFabcdef", *p))
                    continue;
                return e_syntax(ctx, lineno, "expect hex char");
            }
            if (*p == '\\') {
                escape = true;
                continue;
            }
        }
        if (escape)
            return e_syntax(ctx, lineno, "expect an escape char");
        if (hexreq)
            return e_syntax(ctx, lineno, "expected more hex char");

        set_token(ctx, STRING, lineno, orig, q + 3 - orig);
        return 0;
    }

    if (*p == '\'') {
        for (p++; *p && *p != '\n' && *p != '\''; p++)
            ;
        if (*p != '\'')
            return e_syntax(ctx, lineno, "unterminated s-quote");

        set_token(ctx, STRING, lineno, orig, p + 1 - orig);
        return 0;
    }

    if (*p == '\"') {
        int hexreq = 0;
        bool escape = false;
        for (p++; *p; p++) {
            if (escape) {
                escape = false;
                if (strchr("btnfr\"\\", *p))
                    continue;
                if (*p == 'u') {
                    hexreq = 4;
                    continue;
                }
                if (*p == 'U') {
                    hexreq = 8;
                    continue;
                }
                return e_syntax(ctx, lineno, "bad escape char");
            }
            if (hexreq) {
                hexreq--;
                if (strchr("0123456789ABCDEFabcdef", *p))
                    continue;
                return e_syntax(ctx, lineno, "expect hex char");
            }
            if (*p == '\\') {
                escape = true;
                continue;
            }
            if (*p == '\n')
                break;
            if (*p == '"')
                break;
        }
        if (*p != '"')
            return e_syntax(ctx, lineno, "unterminated quote");

        set_token(ctx, STRING, lineno, orig, p + 1 - orig);
        return 0;
    }

    if (scan_date(p, 0, 0, 0) == 0 || scan_time(p, 0, 0, 0) == 0) {
        p += strspn(p, "0123456789.:+-Tt Zz");
        for (; p[-1] == ' '; p--)
            ;
        set_token(ctx, STRING, lineno, orig, p - orig);
        return 0;
    }

    for (; *p && *p != '\n'; p++) {
        int ch = *p;
        if (ch == '.' && dotisspecial)
            break;
        if ('A' <= ch && ch <= 'Z')
            continue;
        if ('a' <= ch && ch <= 'z')
            continue;
        if (strchr("0123456789+-_.", ch))
            continue;
        break;
    }

    set_token(ctx, STRING, lineno, orig, p - orig);
    return 0;
}

static int next_token(context_t *ctx, bool dotisspecial) {
    char *p = ctx->tok.ptr;
    int lineno = ctx->tok.lineno;
    for (int i = 0; i < ctx->tok.len; i++)
        if (*p++ == '\n')
            lineno++;

    while (p < ctx->stop) {
        if (*p == '#') {
            for (p++; p < ctx->stop && *p != '\n'; p++)
                ;
            continue;
        }

        if (dotisspecial && *p == '.') {
            set_token(ctx, DOT, lineno, p, 1);
            return 0;
        }

        switch (*p) {
            case ',':
                set_token(ctx, COMMA, lineno, p, 1);
                return 0;
            case '=':
                set_token(ctx, EQUAL, lineno, p, 1);
                return 0;
            case '{':
                set_token(ctx, LBRACE, lineno, p, 1);
                return 0;
            case '}':
                set_token(ctx, RBRACE, lineno, p, 1);
                return 0;
            case '[':
                set_token(ctx, LBRACKET, lineno, p, 1);
                return 0;
            case ']':
                set_token(ctx, RBRACKET, lineno, p, 1);
                return 0;
            case '\n':
                set_token(ctx, NEWLINE, lineno, p, 1);
                return 0;
            case '\r':
            case ' ':
            case '\t':
                p++;
                continue;
        }

        return scan_string(ctx, p, lineno, dotisspecial);
    }

    set_eof(ctx, lineno);
    return 0;
}

const char *toml_table_key(const toml_table_t *tab, int keyidx, int *keylen) {
    if (keyidx < tab->nkval) {
        *keylen = tab->kval[keyidx]->keylen;
        return tab->kval[keyidx]->key;
    }
    if ((keyidx -= tab->nkval) < tab->narr) {
        *keylen = tab->arr[keyidx]->keylen;
        return tab->arr[keyidx]->key;
    }
    if ((keyidx -= tab->narr) < tab->ntab) {
        *keylen = tab->tab[keyidx]->keylen;
        return tab->tab[keyidx]->key;
    }
    *keylen = 0;
    return 0;
}

toml_unparsed_t toml_table_unparsed(const toml_table_t *tab, const char *key) {
    for (int i = 0; i < tab->nkval; i++)
        if (strcmp(key, tab->kval[i]->key) == 0)
            return tab->kval[i]->val;
    return 0;
}

toml_array_t *toml_table_array(const toml_table_t *tab, const char *key) {
    for (int i = 0; i < tab->narr; i++)
        if (strcmp(key, tab->arr[i]->key) == 0)
            return tab->arr[i];
    return 0;
}

toml_table_t *toml_table_table(const toml_table_t *tab, const char *key) {
    for (int i = 0; i < tab->ntab; i++)
        if (strcmp(key, tab->tab[i]->key) == 0)
            return tab->tab[i];
    return 0;
}

toml_unparsed_t toml_array_unparsed(const toml_array_t *arr, int idx) {
    return (0 <= idx && idx < arr->nitem) ? arr->item[idx].val : 0;
}

int toml_table_len(const toml_table_t *tbl) { return tbl->nkval + tbl->narr + tbl->ntab; }

int toml_array_len(const toml_array_t *arr) { return arr->nitem; }

toml_array_t *toml_array_array(const toml_array_t *arr, int idx) {
    return (0 <= idx && idx < arr->nitem) ? arr->item[idx].arr : 0;
}

toml_table_t *toml_array_table(const toml_array_t *arr, int idx) {
    return (0 <= idx && idx < arr->nitem) ? arr->item[idx].tab : 0;
}

static int parse_millisec(const char *p, const char **endp);

bool is_leap(int y) { return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0); }

int toml_value_timestamp(toml_unparsed_t src_, toml_timestamp_t *ret) {
    if (!src_)
        return -1;

    const char *p = src_;
    bool must_parse_time = false;

    memset(ret, 0, sizeof(*ret));

    if (scan_date(p, &ret->year, &ret->month, &ret->day) == 0) {
        if (ret->month < 1 || ret->day < 1 || ret->month > 12 || ret->day > 31)
            return -1;
        if (ret->month == 2 && ret->day > (is_leap(ret->year) ? 29 : 28))
            return -1;
        ret->kind = 'D';

        p += 10;
        if (*p) {
            if (*p != 'T' && *p != 't' && *p != ' ')
                return -1;
            must_parse_time = true;
            p++;
        }
    }

    if (scan_time(p, &ret->hour, &ret->minute, &ret->second) == 0) {
        if (ret->second < 0 || ret->minute < 0 || ret->hour < 0 || ret->hour > 23 ||
            ret->minute > 59 || ret->second > 60)
            return -1;
        ret->kind = (ret->kind == 'D' ? 'l' : 't');

        p += 8;
        if (*p == '.') {
            p++;
            const char *qq;
            ret->millisec = parse_millisec(p, &qq);
            p = qq;
        }

        if (*p) {
            ret->kind = 'd';
            char *z = malloc(10);
            ret->z = z;
            if (*p == 'Z' || *p == 'z') {
                *z++ = 'Z';
                p++;
                *z = 0;
            } else if (*p == '+' || *p == '-') {
                *z++ = *p++;

                if (!(isdigit(p[0]) && isdigit(p[1])))
                    return -1;
                *z++ = *p++;
                *z++ = *p++;

                if (*p == ':') {
                    *z++ = *p++;
                    if (!(isdigit(p[0]) && isdigit(p[1])))
                        return -1;
                    *z++ = *p++;
                    *z++ = *p++;
                }

                *z = 0;
            }
        }
    }
    if (*p != 0)
        return -1;
    if (must_parse_time && ret->kind == 'D')
        return -1;
    return 0;
}

int toml_value_bool(toml_unparsed_t src, bool *ret_) {
    if (!src)
        return -1;
    bool dummy;
    bool *ret = ret_ ? ret_ : &dummy;

    if (strcmp(src, "true") == 0) {
        *ret = true;
        return 0;
    }
    if (strcmp(src, "false") == 0) {
        *ret = false;
        return 0;
    }
    return -1;
}

int toml_value_int(toml_unparsed_t src, int64_t *ret_) {
    if (!src)
        return -1;

    char buf[100];
    char *p = buf;
    char *q = p + sizeof(buf);
    const char *s = src;
    int base = 0;
    int64_t dummy;
    int64_t *ret = ret_ ? ret_ : &dummy;
    bool have_sign = false;

    if (s[0] == '+' || s[0] == '-') {
        have_sign = true;
        *p++ = *s++;
    }

    if (s[0] == '_')
        return -1;

    if (s[0] == '0') {
        switch (s[1]) {
            case 'x':
                base = 16;
                s += 2;
                break;
            case 'o':
                base = 8;
                s += 2;
                break;
            case 'b':
                base = 2;
                s += 2;
                break;
            case '\0':
                return *ret = 0, 0;
            default:
                if (s[1])
                    return -1;
        }
        if (!*s)
            return -1;
        if (have_sign)
            return -1;
        if (s[0] == '_')
            return -1;
    }

    while (*s && p < q) {
        int ch = *s++;
        if (ch == '_') {
            if (s[0] == '_')
                return -1;
            if (s[0] == '\0')
                return -1;
            continue;
        }
        *p++ = ch;
    }

    if (*s || p == q)
        return -1;

    *p = 0;

    char *endp;
    errno = 0;
    *ret = strtoll(buf, &endp, base);
    return (errno || *endp) ? -1 : 0;
}

int toml_value_double(toml_unparsed_t src, double *ret_) {
    if (!src)
        return -1;

    char buf[100];
    char *p = buf;
    char *q = p + sizeof(buf);
    const char *s = src;
    double dummy;
    double *ret = ret_ ? ret_ : &dummy;
    bool have_us = false;

    if (s[0] == '+' || s[0] == '-')
        *p++ = *s++;

    if (s[0] == '_')
        return -1;

    {
        char *dot = strchr(s, '.');
        if (dot) {
            if (dot == s || !isdigit(dot[-1]) || !isdigit(dot[1]))
                return -1;
        }
    }

    if (s[0] == '0' && s[1] && !strchr("eE.", s[1]))
        return -1;

    while (*s && p < q) {
        int ch = *s++;
        if (ch == '_') {
            have_us = true;
            if (s[0] == '_')
                return -1;
            if (s[0] == 'e')
                return -1;
            if (s[0] == 0)
                return -1;
            continue;
        }
        if (ch == 'I' || ch == 'N' || ch == 'F' || ch == 'A')
            return -1;
        if (ch == 'e' && s[0] == '_')
            return -1;
        *p++ = ch;
    }
    if (*s || p == q)
        return -1;

    *p = 0;

    char *endp;
    errno = 0;
    *ret = strtod(buf, &endp);
    if (errno || *endp)
        return -1;
    if (have_us && (isnan(*ret) || isinf(*ret)))
        return -1;
    return 0;
}

int toml_value_string(toml_unparsed_t src, char **ret, int *len) {
    bool multiline = false;
    const char *sp;
    const char *sq;

    *ret = 0;
    if (!src)
        return -1;

    int qchar = src[0];
    int srclen = strlen(src);
    if (!(qchar == '\'' || qchar == '"')) {
        return -1;
    }

    if (qchar == src[1] && qchar == src[2]) {
        multiline = true;
        sp = src + 3;
        sq = src + srclen - 3;

        if (!(sp <= sq && sq[0] == qchar && sq[1] == qchar && sq[2] == qchar))
            return -1;

        if (sp[0] == '\n')
            sp++;
        else if (sp[0] == '\r' && sp[1] == '\n')
            sp += 2;
    } else {
        sp = src + 1;
        sq = src + srclen - 1;
        if (!(sp <= sq && *sq == qchar))
            return -1;
    }

    if (qchar == '\'')
        *ret = norm_lit_str(sp, sq - sp, len, multiline, false, 0, 0);
    else
        *ret = norm_basic_str(sp, sq - sp, len, multiline, false, 0, 0);
    return *ret ? 0 : -1;
}

toml_value_t toml_array_string(const toml_array_t *arr, int idx) {
    toml_value_t ret;
    memset(&ret, 0, sizeof(ret));
    ret.ok = (toml_value_string(toml_array_unparsed(arr, idx), &ret.u.s, &ret.u.sl) == 0);
    return ret;
}

toml_value_t toml_array_bool(const toml_array_t *arr, int idx) {
    toml_value_t ret;
    memset(&ret, 0, sizeof(ret));
    ret.ok = (toml_value_bool(toml_array_unparsed(arr, idx), &ret.u.b) == 0);
    return ret;
}

toml_value_t toml_array_int(const toml_array_t *arr, int idx) {
    toml_value_t ret;
    memset(&ret, 0, sizeof(ret));
    ret.ok = (toml_value_int(toml_array_unparsed(arr, idx), &ret.u.i) == 0);
    return ret;
}

toml_value_t toml_array_double(const toml_array_t *arr, int idx) {
    toml_value_t ret;
    memset(&ret, 0, sizeof(ret));
    ret.ok = (toml_value_double(toml_array_unparsed(arr, idx), &ret.u.d) == 0);
    return ret;
}

toml_value_t toml_array_timestamp(const toml_array_t *arr, int idx) {
    toml_timestamp_t ts;
    toml_value_t ret;
    memset(&ret, 0, sizeof(ret));
    ret.ok = (toml_value_timestamp(toml_array_unparsed(arr, idx), &ts) == 0);
    if (ret.ok) {
        ret.ok = !!(ret.u.ts = malloc(sizeof(*ret.u.ts)));
        if (ret.ok)
            *ret.u.ts = ts;
    }
    return ret;
}

toml_value_t toml_table_string(const toml_table_t *tbl, const char *key) {
    toml_value_t ret;
    memset(&ret, 0, sizeof(ret));
    toml_unparsed_t raw = toml_table_unparsed(tbl, key);
    if (raw)
        ret.ok = (toml_value_string(raw, &ret.u.s, &ret.u.sl) == 0);
    return ret;
}

toml_value_t toml_table_bool(const toml_table_t *tbl, const char *key) {
    toml_value_t ret;
    memset(&ret, 0, sizeof(ret));
    ret.ok = (toml_value_bool(toml_table_unparsed(tbl, key), &ret.u.b) == 0);
    return ret;
}

toml_value_t toml_table_int(const toml_table_t *tbl, const char *key) {
    toml_value_t ret;
    memset(&ret, 0, sizeof(ret));
    ret.ok = (toml_value_int(toml_table_unparsed(tbl, key), &ret.u.i) == 0);
    return ret;
}

toml_value_t toml_table_double(const toml_table_t *tbl, const char *key) {
    toml_value_t ret;
    memset(&ret, 0, sizeof(ret));
    ret.ok = (toml_value_double(toml_table_unparsed(tbl, key), &ret.u.d) == 0);
    return ret;
}

toml_value_t toml_table_timestamp(const toml_table_t *tbl, const char *key) {
    toml_timestamp_t ts;
    toml_value_t ret;
    memset(&ret, 0, sizeof(ret));
    ret.ok = (toml_value_timestamp(toml_table_unparsed(tbl, key), &ts) == 0);
    if (ret.ok) {
        ret.ok = !!(ret.u.ts = malloc(sizeof(*ret.u.ts)));
        if (ret.ok)
            *ret.u.ts = ts;
    }
    return ret;
}

static int parse_millisec(const char *p, const char **endp) {
    int ret = 0;
    int unit = 100;
    for (; '0' <= *p && *p <= '9'; p++, unit /= 10)
        ret += (*p - '0') * unit;
    *endp = p;
    return ret;
}
#endif
