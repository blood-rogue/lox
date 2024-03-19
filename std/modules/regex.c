#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

#include "builtins.h"

static ObjModule *_regex_module = NULL;

static BuiltinResult regex(char *pattern, size_t pattern_size, char *subject, size_t subject_size) {
    PCRE2_SIZE erroffset;
    int errcode;
    uint8_t buffer[128];

    uint32_t options = 0;

    pcre2_code *re =
        pcre2_compile((const uint8_t *)pattern, pattern_size, options, &errcode, &erroffset, NULL);

    if (re == NULL) {
        pcre2_get_error_message(errcode, buffer, 120);
        ERR("PCRE2 error: %s", (char *)buffer)
    }

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
    pcre2_code_free(re);

    OK(instance);
}

static BuiltinResult _regex_search(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    return regex(argv_0->chars, argv_0->raw_length, argv_1->chars, argv_1->raw_length);
}

static BuiltinResult _regex_match(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    char *pattern = malloc(argv_0->raw_length + 2);
    snprintf(pattern, argv_0->raw_length + 2, "^%.*s", argv_0->raw_length, argv_0->chars);

    return regex(pattern, argv_0->raw_length + 1, argv_1->chars, argv_1->raw_length);
}

static BuiltinResult _regex_fullmatch(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    char *pattern = malloc(argv_0->raw_length + 3);
    snprintf(pattern, argv_0->raw_length + 3, "^%.*s$", argv_0->raw_length, argv_0->chars);

    return regex(pattern, argv_0->raw_length + 2, argv_1->chars, argv_1->raw_length);
}

static BuiltinResult _regex_findall(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    const char *pattern = argv_0->chars;
    size_t pattern_size = argv_0->raw_length;

    const char *subject = argv_1->chars;
    size_t subject_size = argv_1->raw_length;

    PCRE2_SIZE erroffset;
    int errcode;
    uint8_t buffer[128];

    uint32_t options = 0;

    pcre2_code *re =
        pcre2_compile((const uint8_t *)pattern, pattern_size, options, &errcode, &erroffset, NULL);

    if (re == NULL) {
        pcre2_get_error_message(errcode, buffer, 120);
        ERR("PCRE2 error: %s", (char *)buffer)
    }

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
    pcre2_code_free(re);

    OK(matches);
}

static BuiltinResult _regex_compile(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    const char *pattern = argv_0->chars;
    size_t pattern_size = argv_0->raw_length;

    PCRE2_SIZE erroffset;
    int errcode;
    uint8_t buffer[128];

    uint32_t options = 0;

    pcre2_code *re =
        pcre2_compile((const uint8_t *)pattern, pattern_size, options, &errcode, &erroffset, NULL);

    if (re == NULL) {
        pcre2_get_error_message(errcode, buffer, 120);
        ERR("PCRE2 error: %s", (char *)buffer)
    }

    ObjInstance *instance = new_instance(get_regex_pattern_class());
    SET_FIELD("$$internal", new_native_struct(re, (FreeFn)pcre2_code_free));

    OK(instance);
}

ObjModule *get_regex_module(int count, UNUSED(char **, parts)) {
    CHECK_PART_COUNT

    if (_regex_module == NULL) {
        ObjModule *module = new_module("regex");

        SET_BUILTIN_FN_MEMBER("search", _regex_search);
        SET_BUILTIN_FN_MEMBER("match", _regex_match);
        SET_BUILTIN_FN_MEMBER("fullmatch", _regex_fullmatch);
        SET_BUILTIN_FN_MEMBER("findall", _regex_findall);
        SET_BUILTIN_FN_MEMBER("compile", _regex_compile);

        _regex_module = module;
    }

    return _regex_module;
}
