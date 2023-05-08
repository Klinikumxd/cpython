/* Copyright (c) INRIA and Microsoft Corporation. All rights reserved.
   Licensed under the Apache 2.0 License. */

#ifndef __KRML_TARGET_H
#define __KRML_TARGET_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <limits.h>
#include <assert.h>

/* Since KaRaMeL emits the inline keyword unconditionally, we follow the
 * guidelines at https://gcc.gnu.org/onlinedocs/gcc/Inline.html and make this
 * __inline__ to ensure the code compiles with -std=c90 and earlier. */
#ifdef __GNUC__
#  define inline __inline__
#endif

/******************************************************************************/
/* Macros that KaRaMeL will generate.                                         */
/******************************************************************************/

/* For "bare" targets that do not have a C stdlib, the user might want to use
 * [-add-early-include '"mydefinitions.h"'] and override these. */
#ifndef KRML_HOST_PRINTF
#  define KRML_HOST_PRINTF printf
#endif

#if (                                                                          \
    (defined __STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) &&             \
    (!(defined KRML_HOST_EPRINTF)))
#  define KRML_HOST_EPRINTF(...) fprintf(stderr, __VA_ARGS__)
#elif !(defined KRML_HOST_EPRINTF) && defined(_MSC_VER)
#  define KRML_HOST_EPRINTF(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifndef KRML_HOST_EXIT
#  define KRML_HOST_EXIT exit
#endif

#ifndef KRML_HOST_MALLOC
#  define KRML_HOST_MALLOC malloc
#endif

#ifndef KRML_HOST_CALLOC
#  define KRML_HOST_CALLOC calloc
#endif

#ifndef KRML_HOST_FREE
#  define KRML_HOST_FREE free
#endif

#ifndef KRML_HOST_IGNORE
#  define KRML_HOST_IGNORE(x) (void)(x)
#endif

/* In FStar.Buffer.fst, the size of arrays is uint32_t, but it's a number of
 * *elements*. Do an ugly, run-time check (some of which KaRaMeL can eliminate).
 */
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 4))
#  define _KRML_CHECK_SIZE_PRAGMA                                              \
    _Pragma("GCC diagnostic ignored \"-Wtype-limits\"")
#else
#  define _KRML_CHECK_SIZE_PRAGMA
#endif

#define KRML_CHECK_SIZE(size_elt, sz)                                          \
  do {                                                                         \
    _KRML_CHECK_SIZE_PRAGMA                                                    \
    if (((size_t)(sz)) > ((size_t)(SIZE_MAX / (size_elt)))) {                  \
      KRML_HOST_PRINTF(                                                        \
          "Maximum allocatable size exceeded, aborting before overflow at "    \
          "%s:%d\n",                                                           \
          __FILE__, __LINE__);                                                 \
      KRML_HOST_EXIT(253);                                                     \
    }                                                                          \
  } while (0)

/* Macros for prettier unrolling of loops */
#define KRML_LOOP1(i, n, x) { \
  x \
  i += n; \
}

#define KRML_LOOP2(i, n, x) \
  KRML_LOOP1(i, n, x) \
  KRML_LOOP1(i, n, x)

#define KRML_LOOP3(i, n, x) \
  KRML_LOOP2(i, n, x) \
  KRML_LOOP1(i, n, x)

#define KRML_LOOP4(i, n, x) \
  KRML_LOOP2(i, n, x) \
  KRML_LOOP2(i, n, x)

#define KRML_LOOP5(i, n, x) \
  KRML_LOOP4(i, n, x) \
  KRML_LOOP1(i, n, x)

#define KRML_LOOP6(i, n, x) \
  KRML_LOOP4(i, n, x) \
  KRML_LOOP2(i, n, x)

#define KRML_LOOP7(i, n, x) \
  KRML_LOOP4(i, n, x) \
  KRML_LOOP3(i, n, x)

#define KRML_LOOP8(i, n, x) \
  KRML_LOOP4(i, n, x) \
  KRML_LOOP4(i, n, x)

#define KRML_LOOP9(i, n, x) \
  KRML_LOOP8(i, n, x) \
  KRML_LOOP1(i, n, x)

#define KRML_LOOP10(i, n, x) \
  KRML_LOOP8(i, n, x) \
  KRML_LOOP2(i, n, x)

#define KRML_LOOP11(i, n, x) \
  KRML_LOOP8(i, n, x) \
  KRML_LOOP3(i, n, x)

#define KRML_LOOP12(i, n, x) \
  KRML_LOOP8(i, n, x) \
  KRML_LOOP4(i, n, x)

#define KRML_LOOP13(i, n, x) \
  KRML_LOOP8(i, n, x) \
  KRML_LOOP5(i, n, x)

#define KRML_LOOP14(i, n, x) \
  KRML_LOOP8(i, n, x) \
  KRML_LOOP6(i, n, x)

#define KRML_LOOP15(i, n, x) \
  KRML_LOOP8(i, n, x) \
  KRML_LOOP7(i, n, x)

#define KRML_LOOP16(i, n, x) \
  KRML_LOOP8(i, n, x) \
  KRML_LOOP8(i, n, x)

#define KRML_UNROLL_FOR(i, z, n, k, x) do { \
  uint32_t i = z; \
  KRML_LOOP##n(i, k, x) \
} while (0)

#define KRML_ACTUAL_FOR(i, z, n, k, x) \
  do { \
    for (uint32_t i = z; i < n; i += k) { \
      x \
    } \
  } while (0)

#ifndef KRML_UNROLL_MAX
#define KRML_UNROLL_MAX 16
#endif

/* 1 is the number of loop iterations, i.e. (n - z)/k as evaluated by krml */
#if 0 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR0(i, z, n, k, x)
#else
#define KRML_MAYBE_FOR0(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 1 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR1(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 1, k, x)
#else
#define KRML_MAYBE_FOR1(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 2 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR2(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 2, k, x)
#else
#define KRML_MAYBE_FOR2(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 3 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR3(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 3, k, x)
#else
#define KRML_MAYBE_FOR3(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 4 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR4(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 4, k, x)
#else
#define KRML_MAYBE_FOR4(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 5 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR5(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 5, k, x)
#else
#define KRML_MAYBE_FOR5(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 6 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR6(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 6, k, x)
#else
#define KRML_MAYBE_FOR6(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 7 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR7(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 7, k, x)
#else
#define KRML_MAYBE_FOR7(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 8 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR8(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 8, k, x)
#else
#define KRML_MAYBE_FOR8(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 9 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR9(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 9, k, x)
#else
#define KRML_MAYBE_FOR9(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 10 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR10(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 10, k, x)
#else
#define KRML_MAYBE_FOR10(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 11 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR11(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 11, k, x)
#else
#define KRML_MAYBE_FOR11(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 12 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR12(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 12, k, x)
#else
#define KRML_MAYBE_FOR12(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 13 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR13(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 13, k, x)
#else
#define KRML_MAYBE_FOR13(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 14 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR14(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 14, k, x)
#else
#define KRML_MAYBE_FOR14(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 15 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR15(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 15, k, x)
#else
#define KRML_MAYBE_FOR15(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif

#if 16 <= KRML_UNROLL_MAX
#define KRML_MAYBE_FOR16(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 16, k, x)
#else
#define KRML_MAYBE_FOR16(i, z, n, k, x) KRML_ACTUAL_FOR(i, z, n, k, x)
#endif
#endif
