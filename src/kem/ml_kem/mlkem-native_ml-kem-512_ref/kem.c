/*
 * Copyright (c) 2024 The mlkem-native project authors
 * SPDX-License-Identifier: Apache-2.0
 */
#include "kem.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "indcpa.h"
#include "randombytes.h"
#include "symmetric.h"
#include "verify.h"

#if defined(CBMC)
/* Redeclaration with contract needed for CBMC only */
int memcmp(const void *str1, const void *str2, size_t n)
__contract__(
  requires(memory_no_alias(str1, n))
  requires(memory_no_alias(str2, n))
);
#endif

/*************************************************
 * Name:        check_pk
 *
 * Description: Implements modulus check mandated by FIPS203,
 *              i.e., ensures that coefficients are in [0,q-1].
 *              Described in Section 7.2 of FIPS203.
 *
 * Arguments:   - const uint8_t *pk: pointer to input public key
 *                (an already allocated array of MLKEM_PUBLICKEYBYTES bytes)
 **
 * Returns 0 on success, and -1 on failure
 **************************************************/
static int check_pk(const uint8_t pk[MLKEM_PUBLICKEYBYTES])
{
  polyvec p;
  uint8_t p_reencoded[MLKEM_POLYVECBYTES];
  polyvec_frombytes(&p, pk);
  polyvec_reduce(&p);
  polyvec_tobytes(p_reencoded, &p);
  /* Data is public, so a variable-time memcmp() is OK */
  if (memcmp(pk, p_reencoded, MLKEM_POLYVECBYTES))
  {
    return -1;
  }
  return 0;
}

/*************************************************
 * Name:        check_sk
 *
 * Description: Implements public key hash check mandated by FIPS203,
 *              i.e., ensures that
 *              sk[768𝑘+32 ∶ 768𝑘+64] = H(pk)= H(sk[384𝑘 : 768𝑘+32])
 *              Described in Section 7.3 of FIPS203.
 *
 * Arguments:   - const uint8_t *sk: pointer to input private key
 *                (an already allocated array of MLKEM_SECRETKEYBYTES bytes)
 *
 * Returns 0 on success, and -1 on failure
 **************************************************/
static int check_sk(const uint8_t sk[MLKEM_SECRETKEYBYTES])
{
  uint8_t test[MLKEM_SYMBYTES];
  /*
   * The parts of `sk` being hashed and compared here are public, so
   * no public information is leaked through the runtime or the return value
   * of this function.
   */
  hash_h(test, sk + MLKEM_INDCPA_SECRETKEYBYTES, MLKEM_PUBLICKEYBYTES);
  if (memcmp(sk + MLKEM_SECRETKEYBYTES - 2 * MLKEM_SYMBYTES, test,
             MLKEM_SYMBYTES))
  {
    return -1;
  }
  return 0;
}

int crypto_kem_keypair_derand(uint8_t *pk, uint8_t *sk, const uint8_t *coins)
{
  indcpa_keypair_derand(pk, sk, coins);
  memcpy(sk + MLKEM_INDCPA_SECRETKEYBYTES, pk, MLKEM_PUBLICKEYBYTES);
  hash_h(sk + MLKEM_SECRETKEYBYTES - 2 * MLKEM_SYMBYTES, pk,
         MLKEM_PUBLICKEYBYTES);
  /* Value z for pseudo-random output on reject */
  memcpy(sk + MLKEM_SECRETKEYBYTES - MLKEM_SYMBYTES, coins + MLKEM_SYMBYTES,
         MLKEM_SYMBYTES);
  return 0;
}

int crypto_kem_keypair(uint8_t *pk, uint8_t *sk)
{
  ALIGN uint8_t coins[2 * MLKEM_SYMBYTES];
  randombytes(coins, 2 * MLKEM_SYMBYTES);
  crypto_kem_keypair_derand(pk, sk, coins);
  return 0;
}

int crypto_kem_enc_derand(uint8_t *ct, uint8_t *ss, const uint8_t *pk,
                          const uint8_t *coins)
{
  ALIGN uint8_t buf[2 * MLKEM_SYMBYTES];
  /* Will contain key, coins */
  ALIGN uint8_t kr[2 * MLKEM_SYMBYTES];

  if (check_pk(pk))
  {
    return -1;
  }

  memcpy(buf, coins, MLKEM_SYMBYTES);

  /* Multitarget countermeasure for coins + contributory KEM */
  hash_h(buf + MLKEM_SYMBYTES, pk, MLKEM_PUBLICKEYBYTES);
  hash_g(kr, buf, 2 * MLKEM_SYMBYTES);

  /* coins are in kr+MLKEM_SYMBYTES */
  indcpa_enc(ct, buf, pk, kr + MLKEM_SYMBYTES);

  memcpy(ss, kr, MLKEM_SYMBYTES);
  return 0;
}

int crypto_kem_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk)
{
  ALIGN uint8_t coins[MLKEM_SYMBYTES];
  randombytes(coins, MLKEM_SYMBYTES);
  return crypto_kem_enc_derand(ct, ss, pk, coins);
}

int crypto_kem_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk)
{
  uint8_t fail;
  ALIGN uint8_t buf[2 * MLKEM_SYMBYTES];
  /* Will contain key, coins */
  ALIGN uint8_t kr[2 * MLKEM_SYMBYTES];
  const uint8_t *pk = sk + MLKEM_INDCPA_SECRETKEYBYTES;

  if (check_sk(sk))
  {
    return -1;
  }

  indcpa_dec(buf, ct, sk);

  /* Multitarget countermeasure for coins + contributory KEM */
  memcpy(buf + MLKEM_SYMBYTES, sk + MLKEM_SECRETKEYBYTES - 2 * MLKEM_SYMBYTES,
         MLKEM_SYMBYTES);
  hash_g(kr, buf, 2 * MLKEM_SYMBYTES);

  /* Recompute and compare ciphertext */
  {
    /* Temporary buffer */
    ALIGN uint8_t cmp[MLKEM_CIPHERTEXTBYTES];
    /* coins are in kr+MLKEM_SYMBYTES */
    indcpa_enc(cmp, buf, pk, kr + MLKEM_SYMBYTES);
    fail = ct_memcmp(ct, cmp, MLKEM_CIPHERTEXTBYTES);
  }

  /* Compute rejection key */
  {
    /* Temporary buffer */
    ALIGN uint8_t tmp[MLKEM_SYMBYTES + MLKEM_CIPHERTEXTBYTES];
    memcpy(tmp, sk + MLKEM_SECRETKEYBYTES - MLKEM_SYMBYTES, MLKEM_SYMBYTES);
    memcpy(tmp + MLKEM_SYMBYTES, ct, MLKEM_CIPHERTEXTBYTES);
    hash_j(ss, tmp, sizeof(tmp));
  }

  /* Copy true key to return buffer if fail is 0 */
  ct_cmov_zero(ss, kr, MLKEM_SYMBYTES, fail);

  return 0;
}
