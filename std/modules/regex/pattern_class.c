#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

#include "builtins.h"

static ObjClass *_regex_pattern_class = NULL;

static NativeResult regex(pcre2_code *re, char *subject, size_t subject_size) {
    uint32_t options = 0;

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_match(re, (uint8_t *)subject, subject_size, 0, options, match_data, NULL);

    if (rc < 0) {
        switch (rc) {
            case PCRE2_ERROR_NOMATCH:
                OK(new_nil());
            default:
                ERR("Matching error.")
        }
    }

    ObjList *groups = new_list(NULL, 0);

    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
    for (int i = 0; i < rc; i++) {
        const char *start = subject + ovector[2 * i];
        PCRE2_SIZE slen = ovector[2 * i + 1] - ovector[2 * i];
        write_array(&groups->elems, AS_OBJ(new_string((char *)start, (int)slen)));
    }

    ObjInstance *instance = new_instance(get_regex_match_class());
    SET_FIELD("groups", groups);

    pcre2_match_data_free(match_data);

    OK(instance);
}

static NativeResult _regex_pattern_search(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    ObjInstance *re_instance = AS_INSTANCE(caller);
    GET_INTERNAL(pcre2_code *, re);

    return regex(re, argv_0->chars, argv_0->raw_length);
}

static NativeResult _regex_pattern_findall(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    ObjInstance *re_instance = AS_INSTANCE(caller);
    GET_INTERNAL(pcre2_code *, re);

    const char *subject = argv_0->chars;
    size_t subject_size = argv_0->raw_length;

    uint32_t options = 0;

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_match(re, (uint8_t *)subject, subject_size, 0, options, match_data, NULL);

    if (rc < 0) {
        switch (rc) {
            case PCRE2_ERROR_NOMATCH:
                OK(new_nil());
            default:
                ERR("Matching error.")
        }
    }

    ObjList *matches = new_list(NULL, 0);
    ObjList *groups = new_list(NULL, 0);

    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
    for (int i = 0; i < rc; i++) {
        const char *start = subject + ovector[2 * i];
        PCRE2_SIZE slen = ovector[2 * i + 1] - ovector[2 * i];
        write_array(&groups->elems, AS_OBJ(new_string((char *)start, (int)slen)));
    }

    ObjInstance *instance = new_instance(get_regex_match_class());
    SET_FIELD("groups", groups);

    write_array(&matches->elems, AS_OBJ(instance));

    bool cont = true;

    while (cont) {
        subject_size -= ovector[1];
        subject += ovector[1];

        rc = pcre2_match(re, (uint8_t *)subject, subject_size, 0, options, match_data, NULL);

        if (rc < 0) {
            switch (rc) {
                case PCRE2_ERROR_NOMATCH:
                    cont = false;
                    break;
                default:
                    ERR("Matching error.")
            }
        } else {
            ObjList *groups = new_list(NULL, 0);

            ovector = pcre2_get_ovector_pointer(match_data);
            for (int i = 0; i < rc; i++) {
                const char *start = subject + ovector[2 * i];
                PCRE2_SIZE slen = ovector[2 * i + 1] - ovector[2 * i];
                write_array(&groups->elems, AS_OBJ(new_string((char *)start, (int)slen)));
            }

            ObjInstance *instance = new_instance(get_regex_match_class());
            SET_FIELD("groups", groups);

            write_array(&matches->elems, AS_OBJ(instance));
        }
    }

    pcre2_match_data_free(match_data);

    OK(matches);
}

static NativeResult _regex_pattern_free(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *re_instance = AS_INSTANCE(caller);
    GET_INTERNAL(pcre2_code *, re);

    pcre2_code_free(re);

    native->ptr = NULL;

    OK(new_nil());
}

ObjClass *get_regex_pattern_class() {
    if (_regex_pattern_class == NULL) {
        ObjClass *klass = new_builtin_class("Pattern");

        SET_BUILTIN_FN_METHOD("search", _regex_pattern_search);
        SET_BUILTIN_FN_METHOD("findall", _regex_pattern_findall);
        SET_BUILTIN_FN_METHOD("free", _regex_pattern_free);

        _regex_pattern_class = klass;
    }

    return _regex_pattern_class;
}
