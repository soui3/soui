/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _STypes_DEFINED_
#define _STypes_DEFINED_

#include <memory>
#include <assert.h>

#ifndef RC_INVOKED

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char        int_least8_t;
typedef short              int_least16_t;
typedef int                int_least32_t;
typedef long long          int_least64_t;
typedef unsigned char      uint_least8_t;
typedef unsigned short     uint_least16_t;
typedef unsigned int       uint_least32_t;
typedef unsigned long long uint_least64_t;

typedef signed char        int_fast8_t;
typedef int                int_fast16_t;
typedef int                int_fast32_t;
typedef long long          int_fast64_t;
typedef unsigned char      uint_fast8_t;
typedef unsigned int       uint_fast16_t;
typedef unsigned int       uint_fast32_t;
typedef unsigned long long uint_fast64_t;

typedef long long          intmax_t;
typedef unsigned long long uintmax_t;

#endif // RC_INVOKED


namespace SOUI {

/**
 *  Fast type for signed 8 bits. Use for parameter passing and local variables,
 *  not for storage.
 */
typedef int S8CPU;

/**
 *  Fast type for unsigned 8 bits. Use for parameter passing and local
 *  variables, not for storage
 */
typedef unsigned U8CPU;

/**
 *  Fast type for signed 16 bits. Use for parameter passing and local variables,
 *  not for storage
 */
typedef int S16CPU;

/**
 *  Fast type for unsigned 16 bits. Use for parameter passing and local
 *  variables, not for storage
 */
typedef unsigned U16CPU;

/**
 *  Meant to be faster than bool (doesn't promise to be 0 or 1,
 *  just 0 or non-zero
 */
typedef int SkBool;

/**
 *  Meant to be a small version of bool, for storage purposes. Will be 0 or 1
 */
typedef uint8_t SkBool8;

#define SkToS8(x)   ((int8_t)(x))
#define SkToU8(x)   ((uint8_t)(x))
#define SkToS16(x)  ((int16_t)(x))
#define SkToU16(x)  ((uint16_t)(x))
#define SkToS32(x)  ((int32_t)(x))
#define SkToU32(x)  ((uint32_t)(x))
#define SkToInt(x)  ((int)(x))
#define SkToUInt(x) ((unsigned)(x))
#define SkToSizeT(x) ((size_t)(x))

/** Returns 0 or 1 based on the condition
*/
#define SkToBool(cond)  ((cond) != 0)

#define SK_MaxS16   32767
#define SK_MinS16   -32767
#define SK_MaxU16   0xFFFF
#define SK_MinU16   0
#define SK_MaxS32   0x7FFFFFFF
#define SK_MinS32   -SK_MaxS32
#define SK_MaxU32   0xFFFFFFFF
#define SK_MinU32   0
#define SK_NaN32    (1 << 31)

/** Returns true if the value can be represented with signed 16bits
 */
static inline bool SkIsS16(long x) {
    return (int16_t)x == x;
}

/** Returns true if the value can be represented with unsigned 16bits
 */
static inline bool SkIsU16(long x) {
    return (uint16_t)x == x;
}

//////////////////////////////////////////////////////////////////////////////
#ifndef SK_OFFSETOF
    #define SK_OFFSETOF(type, field)    (size_t)((char*)&(((type*)1)->field) - (char*)1)
#endif

/** Returns the number of entries in an array (not a pointer)
*/
#define SK_ARRAY_COUNT(array)       (sizeof(array) / sizeof(array[0]))

#define SkAlign2(x)     (((x) + 1) >> 1 << 1)
#define SkIsAlign2(x)   (0 == ((x) & 1))

#define SkAlign4(x)     (((x) + 3) >> 2 << 2)
#define SkIsAlign4(x)   (0 == ((x) & 3))

#define SkAlign8(x)     (((x) + 7) >> 3 << 3)
#define SkIsAlign8(x)   (0 == ((x) & 7))

#define SkAlignPtr(x)   (sizeof(void*) == 8 ?   SkAlign8(x) :   SkAlign4(x))
#define SkIsAlignPtr(x) (sizeof(void*) == 8 ? SkIsAlign8(x) : SkIsAlign4(x))

typedef uint32_t SkFourByteTag;
#define SkSetFourByteTag(a, b, c, d)    (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

/** 32 bit integer to hold a unicode value
*/
typedef int32_t SkUnichar;
/** 32 bit value to hold a millisecond count
*/
typedef uint32_t SkMSec;
/** 1 second measured in milliseconds
*/
#define SK_MSec1 1000
/** maximum representable milliseconds
*/
#define SK_MSecMax 0x7FFFFFFF
/** Returns a < b for milliseconds, correctly handling wrap-around from 0xFFFFFFFF to 0
*/
#define SkMSec_LT(a, b)     ((int32_t)(a) - (int32_t)(b) < 0)
/** Returns a <= b for milliseconds, correctly handling wrap-around from 0xFFFFFFFF to 0
*/
#define SkMSec_LE(a, b)     ((int32_t)(a) - (int32_t)(b) <= 0)

/** The generation IDs in Skia reserve 0 has an invalid marker.
 */
#define SK_InvalidGenID     0
/** The unique IDs in Skia reserve 0 has an invalid marker.
 */
#define SK_InvalidUniqueID  0

/****************************************************************************
    The rest of these only build with C++
*/
#ifdef __cplusplus

/** Generic swap function. Classes with efficient swaps should specialize this function to take
    their fast path. This function is used by SkTSort. */
template <typename T> inline void STSwap(T& a, T& b) {
    T c(a);
    a = b;
    b = c;
}



#endif /* C++ */
}//end of namespace SOUI

#endif
