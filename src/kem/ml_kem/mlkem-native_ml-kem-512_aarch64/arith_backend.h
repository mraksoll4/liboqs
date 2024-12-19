/*
 * Copyright (c) 2024 The mlkem-native project authors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef MLKEM_NATIVE_ARITH_IMPL_H
#error Only one ARITH assembly profile can be defined -- did you include multiple profiles?
#else
#define MLKEM_NATIVE_ARITH_IMPL_H

#include "common.h"

#if defined(MLKEM_NATIVE_ARITH_BACKEND_IMPL)
#include MLKEM_NATIVE_ARITH_BACKEND_IMPL

/* Include to enforce consistency of API and implementation,
 * and conduct sanity checks on the backend.
 *
 * Keep this _after_ the inclusion of the backend; otherwise,
 * the sanity checks won't have an effect. */
#include "api.h"
#endif

#endif /* MLKEM_NATIVE_ARITH_IMPL_H */
