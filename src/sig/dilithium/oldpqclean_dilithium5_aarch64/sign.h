
/*
 * This file is dual licensed
 * under Apache 2.0 (https://www.apache.org/licenses/LICENSE-2.0.html)
 * or public domain at https://github.com/pq-crystals/dilithium
 */

#ifndef SIGN_H
#define SIGN_H
#include "params.h"
#include "poly.h"
#include "polyvec.h"
#include <stddef.h>
#include <stdint.h>



#define challenge DILITHIUM_NAMESPACE(challenge)
void challenge(poly *c, const uint8_t seed[SEEDBYTES]);

#define crypto_sign_keypair DILITHIUM_NAMESPACE(crypto_sign_keypair)
int crypto_sign_keypair(uint8_t *pk, uint8_t *sk);

#define crypto_sign_keypair_from_fseed DILITHIUM_NAMESPACE(keypair_from_fseed)
int crypto_sign_keypair_from_fseed(uint8_t *pk, uint8_t *sk, const uint8_t *seed);

#define crypto_sign_pubkey_from_privkey DILITHIUM_NAMESPACE(pubkey_from_privkey)
int crypto_sign_pubkey_from_privkey(uint8_t *pk, const uint8_t *sk);

#define crypto_sign_signature DILITHIUM_NAMESPACE(crypto_sign_signature)
int crypto_sign_signature(uint8_t *sig, size_t *siglen,
                          const uint8_t *m, size_t mlen,
                          const uint8_t *sk);

#define crypto_sign DILITHIUM_NAMESPACETOP
int crypto_sign(uint8_t *sm, size_t *smlen,
                const uint8_t *m, size_t mlen,
                const uint8_t *sk);

#define crypto_sign_verify DILITHIUM_NAMESPACE(crypto_sign_verify)
int crypto_sign_verify(const uint8_t *sig, size_t siglen,
                       const uint8_t *m, size_t mlen,
                       const uint8_t *pk);

#define crypto_sign_open DILITHIUM_NAMESPACE(crypto_sign_open)
int crypto_sign_open(uint8_t *m, size_t *mlen,
                     const uint8_t *sm, size_t smlen,
                     const uint8_t *pk);

#endif
