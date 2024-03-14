#include "builtins.h"

static ObjModule *_compress_module = NULL;

ObjModule *get_compress_module(int count, char **parts) {
    if (_compress_module == NULL) {
        ObjModule *module = new_module(new_string("compress", 8));

        SET_MEMBER("zlib", get_compress_zlib_module());
        SET_MEMBER("brotli", get_compress_brotli_module());
        SET_MEMBER("lzf", get_compress_lzf_module());
        SET_MEMBER("lz4", get_compress_lz4_module());
        SET_MEMBER("zstd", get_compress_zstd_module());

        SET_INT_MEMBER("NO_COMPRESSION", 0);
        SET_INT_MEMBER("BEST_SPEED", 1);
        SET_INT_MEMBER("BEST_COMPRESSION", 9);
        SET_INT_MEMBER("DEFAULT_COMPRESSION", -1);
        SET_INT_MEMBER("HUFFMAN_ONLY", 2);

        _compress_module = module;
    }

    if (count == 0)
        return _compress_module;
    else if (count == 1) {
        if (strcmp(parts[0], "zlib") == 0)
            return get_compress_zlib_module();
        else if (strcmp(parts[0], "brotli") == 0)
            return get_compress_brotli_module();
        else if (strcmp(parts[0], "lzf") == 0)
            return get_compress_lzf_module();
        else if (strcmp(parts[0], "lz4") == 0)
            return get_compress_lz4_module();
        else if (strcmp(parts[0], "zstd") == 0)
            return get_compress_zstd_module();

        return NULL;
    }

    return NULL;
}
