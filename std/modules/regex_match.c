#include "builtins.h"

static ObjClass *_regex_match_class = NULL;

ObjClass *get_regex_match_class() {
    if (_regex_match_class == NULL) {
        ObjClass *klass = new_class(new_string("Match", 5));

        _regex_match_class = klass;
    }

    return _regex_match_class;
}
