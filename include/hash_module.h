#include <openssl/evp.h>

#include "object.h"

ObjModule *get_hash_module(int, char **);
ObjClass *get_hash_sha1_class();
ObjClass *get_hash_sha2_class();
ObjClass *get_hash_sha3_class();
ObjClass *get_hash_md5_class();
ObjClass *get_hash_md4_class();
ObjClass *get_hash_shake128_class();
ObjClass *get_hash_shake256_class();

BuiltinResult md_digest(const char *, uint8_t *, int);

typedef struct {
    EVP_MD_CTX *ctx;
    EVP_MD *md;
} MdCtx;

BuiltinResult md_init(const char *, ObjInstance *);
BuiltinResult md_update(int, Obj **, Obj *);
BuiltinResult md_finish(int, Obj **, Obj *);
