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


#endif // RC_INVOKED


namespace SOUI {


#define SiToU8(x)   ((uint8_t)(x))

/** Returns 0 or 1 based on the condition
*/
#define SToBool(cond)  ((cond) != 0)

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
