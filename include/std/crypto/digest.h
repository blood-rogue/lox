#ifndef lox_crypto_module_digest_h
#define lox_crypto_module_digest_h

#include <openssl/evp.h>

#include "object.h"

ObjModule *get_crypto_digest_module();
ObjClass *get_crypto_digest_sha1_class();
ObjClass *get_crypto_digest_sha2_class();
ObjClass *get_crypto_digest_sha3_class();
ObjClass *get_crypto_digest_md5_class();
ObjClass *get_crypto_digest_md4_class();
ObjClass *get_crypto_digest_shake128_class();
ObjClass *get_crypto_digest_shake256_class();
ObjClass *get_crypto_digest_sm3_class();
ObjClass *get_crypto_digest_ripemd160_class();
ObjClass *get_crypto_digest_whirlpool_class();
ObjClass *get_crypto_digest_blake2b512_class();
ObjClass *get_crypto_digest_blake2s256_class();

BuiltinResult md_digest(const char *, uint8_t *, int);

typedef struct {
    EVP_MD_CTX *ctx;
    EVP_MD *md;
} MdCtx;

BuiltinResult md_init(const char *, ObjInstance *);
BuiltinResult md_update(int, Obj **, Obj *);
BuiltinResult md_finish(int, Obj **, Obj *);

#endif // lox_crypto_module_digest_h
