
/*
 * This file is dual licensed
 * under Apache 2.0 (https://www.apache.org/licenses/LICENSE-2.0.html)
 * or public domain at https://github.com/pq-crystals/dilithium
 */

#ifndef PQCLEAN_DILITHIUM2_AARCH64_API_H
#define PQCLEAN_DILITHIUM2_AARCH64_API_H


#include <stddef.h>
#include <stdint.h>

#define PQCLEAN_DILITHIUM2_AARCH64_CRYPTO_PUBLICKEYBYTES 1312
#define PQCLEAN_DILITHIUM2_AARCH64_CRYPTO_SECRETKEYBYTES 2528
#define PQCLEAN_DILITHIUM2_AARCH64_CRYPTO_BYTES 2420
#define PQCLEAN_DILITHIUM2_AARCH64_CRYPTO_ALGNAME "Dilithium2"

int PQCLEAN_DILITHIUM2_AARCH64_crypto_sign_keypair(uint8_t *pk, uint8_t *sk);

int PQCLEAN_DILITHIUM2_AARCH64_crypto_sign_keypair_from_fseed(uint8_t *pk, uint8_t *sk, const uint8_t *seed);

int PQCLEAN_DILITHIUM2_AARCH64_crypto_sign_pubkey_from_privkey(uint8_t *pk, const uint8_t *sk);

int PQCLEAN_DILITHIUM2_AARCH64_crypto_sign_signature(
    uint8_t *sig, size_t *siglen,
    const uint8_t *m, size_t mlen, const uint8_t *sk);

int PQCLEAN_DILITHIUM2_AARCH64_crypto_sign_verify(
    const uint8_t *sig, size_t siglen,
    const uint8_t *m, size_t mlen, const uint8_t *pk);

int PQCLEAN_DILITHIUM2_AARCH64_crypto_sign(
    uint8_t *sm, size_t *smlen,
    const uint8_t *m, size_t mlen, const uint8_t *sk);

int PQCLEAN_DILITHIUM2_AARCH64_crypto_sign_open(
    uint8_t *m, size_t *mlen,
    const uint8_t *sm, size_t smlen, const uint8_t *pk);


#endif
