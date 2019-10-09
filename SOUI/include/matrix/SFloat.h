/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _SFloat_DEFINED_
#define _SFloat_DEFINED_

#include <stdint.h>
#include <math.h>

#define sk_float_sqrt(x)        sqrtf(x)
#define sk_float_sin(x)         sinf(x)
#define sk_float_cos(x)         cosf(x)
#define sk_float_tan(x)         tanf(x)
#define sk_float_floor(x)       floorf(x)
#define sk_float_ceil(x)        ceilf(x)
#define sk_float_abs(x)         (float)::fabs(x)

#define sk_float_floor2int(x)   (int)sk_float_floor(x)
#define sk_float_round2int(x)   (int)sk_float_floor((x) + 0.5f)
#define sk_float_ceil2int(x)    (int)sk_float_ceil(x)

//#define sk_float_rsqrt(x)		sqrt(x)
//#define SK_SUPPORT_DEPRECATED_SCALARROUND


/** SK_Scalar1 is defined to be 1.0 represented as an float
*/
#define SK_Scalar1              (1.0f)
/** SK_Scalar1 is defined to be 1/2 represented as an float
*/
#define SK_ScalarHalf           (0.5f)
/** SK_ScalarInfinity is defined to be infinity as an float
*/
#define SK_ScalarInfinity       SK_FloatInfinity
/** SK_ScalarNegativeInfinity is defined to be negative infinity as an float
*/
#define SK_ScalarNegativeInfinity       SK_FloatNegativeInfinity
/** SK_ScalarMax is defined to be the largest value representable as an float
*/
#define SK_ScalarMax            (3.402823466e+38f)
/** SK_ScalarMin is defined to be the smallest value representable as an float
*/
#define SK_ScalarMin            (-SK_ScalarMax)
/** SK_ScalarNaN is defined to be 'Not a Number' as an float
*/
#define SK_ScalarNaN            SK_FloatNaN

/** SkIntToScalar(n) returns its integer argument as an float
*/
#define SkIntToScalar(n)        ((float)(n))
/** SkFixedToScalar(n) returns its SkFixed argument as an float
*/
#define SkFixedToScalar(x)      SkFixedToFloat(x)
/** SFloatToFixed(n) returns its float argument as an SkFixed
*/
#define SFloatToFixed(x)      SkFloatToFixed(x)

#define SFloatToFloat(n)      (n)
#ifndef SK_SCALAR_TO_FLOAT_EXCLUDED
#define SkFloatToScalar(n)      (n)
#endif

#define SFloatToDouble(n)      (double)(n)
#define SkDoubleToScalar(n)      (float)(n)

/** SFloatFraction(x) returns the signed fractional part of the argument
*/
#define SFloatFraction(x)     sk_float_mod(x, 1.0f)

#define SFloatFloorToScalar(x)    sk_float_floor(x)
#define SFloatCeilToScalar(x)     sk_float_ceil(x)
#define SFloatRoundToScalar(x)    sk_float_floor((x) + 0.5f)

#define SFloatFloorToInt(x)       sk_float_floor2int(x)
#define SFloatCeilToInt(x)        sk_float_ceil2int(x)
#define SFloatRoundToInt(x)       sk_float_round2int(x)
#define SFloatTruncToInt(x)       static_cast<int>(x)


#define SK_ANNOTATE_UNPROTECTED_READ(x) (x)
#define SK_ANNOTATE_UNPROTECTED_WRITE(ptr, val) *(ptr) = (val)


/** Returns the absolute value of the specified float
*/
#define SFloatAbs(x)          sk_float_abs(x)
/** Return x with the sign of y
 */
#define SFloatCopySign(x, y)  sk_float_copysign(x, y)
/** Returns the product of two SFloats
*/
#define SFloatMul(a, b)       ((float)(a) * (b))
/** Returns the product of two SFloats plus a third float
*/
#define SFloatMulAdd(a, b, c) ((float)(a) * (b) + (c))
/** Returns the quotient of two SFloats (a/b)
*/
#define SFloatDiv(a, b)       ((float)(a) / (b))
/** Returns the mod of two SFloats (a mod b)
*/
#define SFloatMod(x,y)        sk_float_mod(x,y)
/** Returns the product of the first two arguments, divided by the third argument
*/
#define SFloatMulDiv(a, b, c) ((float)(a) * (b) / (c))
/** Returns the multiplicative inverse of the float (1/x)
*/
#define SFloatInvert(x)       (SK_Scalar1 / (x))
#define SFloatFastInvert(x)   (SK_Scalar1 / (x))
/** Returns the square root of the float
*/
#define SFloatSqrt(x)         sk_float_sqrt(x)
/** Returns b to the e
*/
#define SFloatPow(b, e)       sk_float_pow(b, e)
/** Returns the average of two SFloats (a+b)/2
*/
#define SFloatAve(a, b)       (((a) + (b)) * 0.5f)
/** Returns one half of the specified float
*/
#define SFloatHalf(a)         ((a) * 0.5f)

#define SK_ScalarSqrt2          1.41421356f
#define SK_ScalarPI             3.14159265f
#define SK_ScalarTanPIOver8     0.414213562f
#define SK_ScalarRoot2Over2     0.707106781f

#define SkDegreesToRadians(degrees) ((degrees) * (SK_ScalarPI / 180))
#define SkRadiansToDegrees(radians) ((radians) * (180 / SK_ScalarPI))
#define SFloatSin(radians)    (float)sk_float_sin(radians)
#define SFloatCos(radians)    (float)sk_float_cos(radians)
#define SFloatTan(radians)    (float)sk_float_tan(radians)
#define SFloatASin(val)   (float)sk_float_asin(val)
#define SFloatACos(val)   (float)sk_float_acos(val)
#define SFloatATan2(y, x) (float)sk_float_atan2(y,x)
#define SFloatExp(x)  (float)sk_float_exp(x)
#define SFloatLog(x)  (float)sk_float_log(x)

namespace SOUI{
/**
 *  Variant of SFloatRoundToInt, that performs the rounding step (adding 0.5) explicitly using
 *  double, to avoid possibly losing the low bit(s) of the answer before calling floor().
 *
 *  This routine will likely be slower than SFloatRoundToInt(), and should only be used when the
 *  extra precision is known to be valuable.
 *
 *  In particular, this catches the following case:
 *      float x = 0.49999997;
 *      int ix = SFloatRoundToInt(x);
 *      SASSERT(0 == ix);    // <--- fails
 *      ix = SkDScalarRoundToInt(x);
 *      SASSERT(0 == ix);    // <--- succeeds
 */
inline int SkDScalarRoundToInt(float x) {
    double xx = x;
    xx += 0.5;
    return (int)floor(xx);
}
inline float SkMaxScalar(float a, float b) { return a > b ? a : b; }
inline float SkMinScalar(float a, float b) { return a < b ? a : b; }

/** SFloatIsNaN(n) returns true if argument is not a number
*/
inline bool SFloatIsNaN(float x) { return x != x; }

/** Returns true if x is not NaN and not infinite */
inline bool SFloatIsFinite(float x) {
	// We rely on the following behavior of infinities and nans
	// 0 * finite --> 0
	// 0 * infinity --> NaN
	// 0 * NaN --> NaN
	float prod = x * 0;
	// At this point, prod will either be NaN or 0
	// Therefore we can return (prod == prod) or (0 == prod).
	return prod == prod;
}

/** Returns the value pinned between 0 and max inclusive
*/
inline float SFloatClampMax(float x, float max) {
	return x < 0 ? 0 : x > max ? max : x;
}
/** Returns the value pinned between min and max inclusive
*/
inline float SFloatPin(float x, float min, float max) {
	return x < min ? min : x > max ? max : x;
}
/** Returns the specified float squared (x*x)
*/
inline float SFloatSquare(float x) { return x * x; }

inline bool SFloatIsInt(float x) {
    return x == (float)(int)x;
}

/**
 *  Returns -1 || 0 || 1 depending on the sign of value:
 *  -1 if x < 0
 *   0 if x == 0
 *   1 if x > 0
 */
inline int SFloatSignAsInt(float x) {
    return x < 0 ? -1 : (x > 0);
}

// Scalar result version of above
inline float SFloatSignAsScalar(float x) {
    return x < 0 ? -SK_Scalar1 : ((x > 0) ? SK_Scalar1 : 0);
}

#define SK_ScalarNearlyZero         (SK_Scalar1 / (1 << 12))

inline bool SFloatNearlyZero(float x,
                                    float tolerance = SK_ScalarNearlyZero) {
    SASSERT(tolerance >= 0);
    return SFloatAbs(x) <= tolerance;
}

inline bool SFloatNearlyEqual(float x, float y,
                                     float tolerance = SK_ScalarNearlyZero) {
    SASSERT(tolerance >= 0);
    return SFloatAbs(x-y) <= tolerance;
}

/** Linearly interpolate between A and B, based on t.
    If t is 0, return A
    If t is 1, return B
    else interpolate.
    t must be [0..SK_Scalar1]
*/
inline float SFloatInterp(float A, float B, float t) {
    SASSERT(t >= 0 && t <= SK_Scalar1);
    return A + (B - A) * t;
}


/*
 *  Helper to compare an array of scalars.
 */
inline bool SFloatsEqual(const float a[], const float b[], int n) {
    SASSERT(n >= 0);
    for (int i = 0; i < n; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

}//end of namespace SOUI

#endif
