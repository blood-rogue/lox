#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

#include "builtins.h"

static ObjModule *_regex_module = NULL;

static BuiltinResult _regex_match(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    pcre2_code *re;
    PCRE2_SIZE erroffset;
    int errcode;
    uint8_t buffer[128];

    int rc;

    const char *pattern = argv_0->chars;
    size_t pattern_size = argv_0->raw_length;

    const char *subject = argv_1->chars;
    size_t subject_size = argv_1->raw_length;
    uint32_t options = 0;

    re = pcre2_compile((const uint8_t *)pattern, pattern_size, options, &errcode, &erroffset, NULL);
    if (re == NULL) {
        pcre2_get_error_message(errcode, buffer, 120);
        return ERR((char *)buffer);
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    rc = pcre2_match(re, (uint8_t *)subject, subject_size, 0, options, match_data, NULL);

    if (rc < 0) {
        switch (rc) {
            case PCRE2_ERROR_NOMATCH:
                return OK(new_nil());
            default:
                return ERR("Matching error.");
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

    return OK(instance);
}

ObjModule *get_regex_module(int count, UNUSED(char **, parts)) {
    CHECK_PART_COUNT(0)

    if (_regex_module == NULL) {
        ObjModule *module = new_module(new_string("regex", 5));

        SET_BUILTIN_FN_MEMBER("match", _regex_match);

        _regex_module = module;
    }

    return _regex_module;
}
