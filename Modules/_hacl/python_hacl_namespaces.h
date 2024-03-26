#ifndef _PYTHON_HACL_NAMESPACES_H
#define _PYTHON_HACL_NAMESPACES_H

/*
 * C's excuse for namespaces: Use globally unique names to avoid linkage
 * conflicts with builds linking or dynamically loading other code potentially
 * using HACL* libraries.
 *
 * To make sure this is effective: cd Modules && nm -a *.o | grep Hacl
 */

#define Hacl_Hash_SHA2_state_sha2_224_s python_hashlib_Hacl_Hash_SHA2_state_sha2_224_s
#define Hacl_Hash_SHA2_state_sha2_224 python_hashlib_Hacl_Hash_SHA2_state_sha2_224
#define Hacl_Hash_SHA2_state_sha2_256 python_hashlib_Hacl_Hash_SHA2_state_sha2_256
#define Hacl_Hash_SHA2_state_sha2_384_s python_hashlib_Hacl_Hash_SHA2_state_sha2_384_s
#define Hacl_Hash_SHA2_state_sha2_384 python_hashlib_Hacl_Hash_SHA2_state_sha2_384
#define Hacl_Hash_SHA2_state_sha2_512 python_hashlib_Hacl_Hash_SHA2_state_sha2_512
#define Hacl_Hash_SHA2_malloc_256 python_hashlib_Hacl_Hash_SHA2_malloc_256
#define Hacl_Hash_SHA2_malloc_224 python_hashlib_Hacl_Hash_SHA2_malloc_224
#define Hacl_Hash_SHA2_malloc_512 python_hashlib_Hacl_Hash_SHA2_malloc_512
#define Hacl_Hash_SHA2_malloc_384 python_hashlib_Hacl_Hash_SHA2_malloc_384
#define Hacl_Hash_SHA2_copy_256 python_hashlib_Hacl_Hash_SHA2_copy_256
#define Hacl_Hash_SHA2_copy_224 python_hashlib_Hacl_Hash_SHA2_copy_224
#define Hacl_Hash_SHA2_copy_512 python_hashlib_Hacl_Hash_SHA2_copy_512
#define Hacl_Hash_SHA2_copy_384 python_hashlib_Hacl_Hash_SHA2_copy_384
#define Hacl_Hash_SHA2_init_256 python_hashlib_Hacl_Hash_SHA2_init_256
#define Hacl_Hash_SHA2_init_224 python_hashlib_Hacl_Hash_SHA2_init_224
#define Hacl_Hash_SHA2_init_512 python_hashlib_Hacl_Hash_SHA2_init_512
#define Hacl_Hash_SHA2_init_384 python_hashlib_Hacl_Hash_SHA2_init_384
#define Hacl_SHA2_Scalar32_sha512_init python_hashlib_Hacl_SHA2_Scalar32_sha512_init
#define Hacl_Hash_SHA2_update_256 python_hashlib_Hacl_Hash_SHA2_update_256
#define Hacl_Hash_SHA2_update_224 python_hashlib_Hacl_Hash_SHA2_update_224
#define Hacl_Hash_SHA2_update_512 python_hashlib_Hacl_Hash_SHA2_update_512
#define Hacl_Hash_SHA2_update_384 python_hashlib_Hacl_Hash_SHA2_update_384
#define Hacl_Hash_SHA2_digest_256 python_hashlib_Hacl_Hash_SHA2_digest_256
#define Hacl_Hash_SHA2_digest_224 python_hashlib_Hacl_Hash_SHA2_digest_224
#define Hacl_Hash_SHA2_digest_512 python_hashlib_Hacl_Hash_SHA2_digest_512
#define Hacl_Hash_SHA2_digest_384 python_hashlib_Hacl_Hash_SHA2_digest_384
#define Hacl_Hash_SHA2_free_256 python_hashlib_Hacl_Hash_SHA2_free_256
#define Hacl_Hash_SHA2_free_224 python_hashlib_Hacl_Hash_SHA2_free_224
#define Hacl_Hash_SHA2_free_512 python_hashlib_Hacl_Hash_SHA2_free_512
#define Hacl_Hash_SHA2_free_384 python_hashlib_Hacl_Hash_SHA2_free_384
#define Hacl_Hash_SHA2_sha256 python_hashlib_Hacl_Hash_SHA2_sha256
#define Hacl_Hash_SHA2_sha224 python_hashlib_Hacl_Hash_SHA2_sha224
#define Hacl_Hash_SHA2_sha512 python_hashlib_Hacl_Hash_SHA2_sha512
#define Hacl_Hash_SHA2_sha384 python_hashlib_Hacl_Hash_SHA2_sha384

#define Hacl_Hash_MD5_malloc python_hashlib_Hacl_Hash_MD5_malloc
#define Hacl_Hash_MD5_init python_hashlib_Hacl_Hash_MD5_init
#define Hacl_Hash_MD5_update python_hashlib_Hacl_Hash_MD5_update
#define Hacl_Hash_MD5_digest python_hashlib_Hacl_Hash_MD5_digest
#define Hacl_Hash_MD5_free python_hashlib_Hacl_Hash_MD5_free
#define Hacl_Hash_MD5_copy python_hashlib_Hacl_Hash_MD5_copy
#define Hacl_Hash_MD5_hash python_hashlib_Hacl_Hash_MD5_hash

#define Hacl_Hash_SHA1_malloc python_hashlib_Hacl_Hash_SHA1_malloc
#define Hacl_Hash_SHA1_init python_hashlib_Hacl_Hash_SHA1_init
#define Hacl_Hash_SHA1_update python_hashlib_Hacl_Hash_SHA1_update
#define Hacl_Hash_SHA1_digest python_hashlib_Hacl_Hash_SHA1_digest
#define Hacl_Hash_SHA1_free python_hashlib_Hacl_Hash_SHA1_free
#define Hacl_Hash_SHA1_copy python_hashlib_Hacl_Hash_SHA1_copy
#define Hacl_Hash_SHA1_hash python_hashlib_Hacl_Hash_SHA1_hash

#define Hacl_Hash_SHA3_update_last_sha3 python_hashlib_Hacl_Hash_SHA3_update_last_sha3
#define Hacl_Hash_SHA3_update_multi_sha3 python_hashlib_Hacl_Hash_SHA3_update_multi_sha3
#define Hacl_Impl_SHA3_absorb_inner python_hashlib_Hacl_Impl_SHA3_absorb_inner
#define Hacl_Impl_SHA3_keccak python_hashlib_Hacl_Impl_SHA3_keccak
#define Hacl_Impl_SHA3_loadState python_hashlib_Hacl_Impl_SHA3_loadState
#define Hacl_Impl_SHA3_squeeze python_hashlib_Hacl_Impl_SHA3_squeeze
#define Hacl_Impl_SHA3_state_permute python_hashlib_Hacl_Impl_SHA3_state_permute
#define Hacl_SHA3_sha3_224 python_hashlib_Hacl_SHA3_sha3_224
#define Hacl_SHA3_sha3_256 python_hashlib_Hacl_SHA3_sha3_256
#define Hacl_SHA3_sha3_384 python_hashlib_Hacl_SHA3_sha3_384
#define Hacl_SHA3_sha3_512 python_hashlib_Hacl_SHA3_sha3_512
#define Hacl_SHA3_shake128_hacl python_hashlib_Hacl_SHA3_shake128_hacl
#define Hacl_SHA3_shake256_hacl python_hashlib_Hacl_SHA3_shake256_hacl
#define Hacl_Hash_SHA3_block_len python_hashlib_Hacl_Hash_SHA3_block_len
#define Hacl_Hash_SHA3_copy python_hashlib_Hacl_Hash_SHA3_copy
#define Hacl_Hash_SHA3_digest python_hashlib_Hacl_Hash_SHA3_digest
#define Hacl_Hash_SHA3_free python_hashlib_Hacl_Hash_SHA3_free
#define Hacl_Hash_SHA3_get_alg python_hashlib_Hacl_Hash_SHA3_get_alg
#define Hacl_Hash_SHA3_hash_len python_hashlib_Hacl_Hash_SHA3_hash_len
#define Hacl_Hash_SHA3_is_shake python_hashlib_Hacl_Hash_SHA3_is_shake
#define Hacl_Hash_SHA3_malloc python_hashlib_Hacl_Hash_SHA3_malloc
#define Hacl_Hash_SHA3_reset python_hashlib_Hacl_Hash_SHA3_reset
#define Hacl_Hash_SHA3_update python_hashlib_Hacl_Hash_SHA3_update
#define Hacl_Hash_SHA3_squeeze python_hashlib_Hacl_Hash_SHA3_squeeze

#endif  // _PYTHON_HACL_NAMESPACES_H
