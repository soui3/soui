/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include <souistd.h>
#include "matrix/SMatrix.h"
#include "matrix/SFloatBits.h"

#include <stddef.h>

// In a few places, we performed the following
//      a * b + c * d + e
// as
//      a * b + (c * d + e)
//
// sdot and scross are indended to capture these compound operations into a
// function, with an eye toward considering upscaling the intermediates to
// doubles for more precision (as we do in concat and invert).
//
// However, these few lines that performed the last add before the "dot", cause
// tiny image differences, so we guard that change until we see the impact on
// chrome's layouttests.
//
#define SK_LEGACY_MATRIX_MATH_ORDER

namespace SOUI{
static inline float SkDoubleToFloat(double x) {
    return static_cast<float>(x);
}

SMatrix::SMatrix(const float data[9])
{
	memcpy(fMat,data,9*sizeof(float));
	setTypeMask(kUnknown_Mask);
}

/*      [scale-x    skew-x      trans-x]   [X]   [X']
        [skew-y     scale-y     trans-y] * [Y] = [Y']
        [persp-0    persp-1     persp-2]   [1]   [1 ]
*/

void SMatrix::reset() {
    fMat[kMScaleX] = fMat[kMScaleY] = fMat[kMPersp2] = 1;
    fMat[kMSkewX]  = fMat[kMSkewY] =
    fMat[kMTransX] = fMat[kMTransY] =
    fMat[kMPersp0] = fMat[kMPersp1] = 0;

    this->setTypeMask(kIdentity_Mask | kRectStaysRect_Mask);
}

// this guy aligns with the masks, so we can compute a mask from a varaible 0/1
enum {
    kTranslate_Shift,
    kScale_Shift,
    kAffine_Shift,
    kPerspective_Shift,
    kRectStaysRect_Shift
};

static const int32_t kScalar1Int = 0x3f800000;

uint8_t SMatrix::computePerspectiveTypeMask() const {
    // Benchmarking suggests that replacing this set of SFloatAs2sCompliment
    // is a win, but replacing those below is not. We don't yet understand
    // that result.
    if (fMat[kMPersp0] != 0 || fMat[kMPersp1] != 0 || fMat[kMPersp2] != 1) {
        // If this is a perspective transform, we return true for all other
        // transform flags - this does not disable any optimizations, respects
        // the rule that the type mask must be conservative, and speeds up
        // type mask computation.
        return SkToU8(kORableMasks);
    }

    return SkToU8(kOnlyPerspectiveValid_Mask | kUnknown_Mask);
}

uint8_t SMatrix::computeTypeMask() const {
    unsigned mask = 0;

    if (fMat[kMPersp0] != 0 || fMat[kMPersp1] != 0 || fMat[kMPersp2] != 1) {
        // Once it is determined that that this is a perspective transform,
        // all other flags are moot as far as optimizations are concerned.
        return SkToU8(kORableMasks);
    }

    if (fMat[kMTransX] != 0 || fMat[kMTransY] != 0) {
        mask |= kTranslate_Mask;
    }

    int m00 = SFloatAs2sCompliment(fMat[SMatrix::kMScaleX]);
    int m01 = SFloatAs2sCompliment(fMat[SMatrix::kMSkewX]);
    int m10 = SFloatAs2sCompliment(fMat[SMatrix::kMSkewY]);
    int m11 = SFloatAs2sCompliment(fMat[SMatrix::kMScaleY]);

    if (m01 | m10) {
        // The skew components may be scale-inducing, unless we are dealing
        // with a pure rotation.  Testing for a pure rotation is expensive,
        // so we opt for being conservative by always setting the scale bit.
        // along with affine.
        // By doing this, we are also ensuring that matrices have the same
        // type masks as their inverses.
        mask |= kAffine_Mask | kScale_Mask;

        // For rectStaysRect, in the affine case, we only need check that
        // the primary diagonal is all zeros and that the secondary diagonal
        // is all non-zero.

        // map non-zero to 1
        m01 = m01 != 0;
        m10 = m10 != 0;

        int dp0 = 0 == (m00 | m11) ;  // true if both are 0
        int ds1 = m01 & m10;        // true if both are 1

        mask |= (dp0 & ds1) << kRectStaysRect_Shift;
    } else {
        // Only test for scale explicitly if not affine, since affine sets the
        // scale bit.
        if ((m00 - kScalar1Int) | (m11 - kScalar1Int)) {
            mask |= kScale_Mask;
        }

        // Not affine, therefore we already know secondary diagonal is
        // all zeros, so we just need to check that primary diagonal is
        // all non-zero.

        // map non-zero to 1
        m00 = m00 != 0;
        m11 = m11 != 0;

        // record if the (p)rimary diagonal is all non-zero
        mask |= (m00 & m11) << kRectStaysRect_Shift;
    }

    return SkToU8(mask);
}

///////////////////////////////////////////////////////////////////////////////

bool operator==(const SMatrix& a, const SMatrix& b) {
    const float* ma = a.fMat;
    const float* mb = b.fMat;

    return  ma[0] == mb[0] && ma[1] == mb[1] && ma[2] == mb[2] &&
            ma[3] == mb[3] && ma[4] == mb[4] && ma[5] == mb[5] &&
            ma[6] == mb[6] && ma[7] == mb[7] && ma[8] == mb[8];
}

///////////////////////////////////////////////////////////////////////////////

// helper function to determine if upper-left 2x2 of matrix is degenerate
static inline bool is_degenerate_2x2(float scaleX, float skewX,
                                     float skewY,  float scaleY) {
    float perp_dot = scaleX*scaleY - skewX*skewY;
    return SFloatNearlyZero(perp_dot, SK_ScalarNearlyZero*SK_ScalarNearlyZero);
}

///////////////////////////////////////////////////////////////////////////////

bool SMatrix::isSimilarity(float tol) const {
    // if identity or translate matrix
    TypeMask mask = this->getType();
    if (mask <= kTranslate_Mask) {
        return true;
    }
    if (mask & kPerspective_Mask) {
        return false;
    }

    float mx = fMat[kMScaleX];
    float my = fMat[kMScaleY];
    // if no skew, can just compare scale factors
    if (!(mask & kAffine_Mask)) {
        return !SFloatNearlyZero(mx) && SFloatNearlyEqual(SFloatAbs(mx), SFloatAbs(my));
    }
    float sx = fMat[kMSkewX];
    float sy = fMat[kMSkewY];

    if (is_degenerate_2x2(mx, sx, sy, my)) {
        return false;
    }

    // upper 2x2 is rotation/reflection + uniform scale if basis vectors
    // are 90 degree rotations of each other
    return (SFloatNearlyEqual(mx, my, tol) && SFloatNearlyEqual(sx, -sy, tol))
        || (SFloatNearlyEqual(mx, -my, tol) && SFloatNearlyEqual(sx, sy, tol));
}

bool SMatrix::preservesRightAngles(float tol) const {
    TypeMask mask = this->getType();

    if (mask <= kTranslate_Mask) {
        // identity, translate and/or scale
        return true;
    }
    if (mask & kPerspective_Mask) {
        return false;
    }

    SASSERT(mask & (kAffine_Mask | kScale_Mask));

    float mx = fMat[kMScaleX];
    float my = fMat[kMScaleY];
    float sx = fMat[kMSkewX];
    float sy = fMat[kMSkewY];

    if (is_degenerate_2x2(mx, sx, sy, my)) {
        return false;
    }

    // upper 2x2 is scale + rotation/reflection if basis vectors are orthogonal
    SVector2D vec[2];
    vec[0].set(mx, sy);
    vec[1].set(sx, my);

    return SFloatNearlyZero(vec[0].dot(vec[1]), SFloatSquare(tol));
}

///////////////////////////////////////////////////////////////////////////////

static inline float sdot(float a, float b, float c, float d) {
    return a * b + c * d;
}

static inline float sdot(float a, float b, float c, float d,
                             float e, float f) {
    return a * b + c * d + e * f;
}

static inline float scross(float a, float b, float c, float d) {
    return a * b - c * d;
}

void SMatrix::setTranslate(float dx, float dy) {
    if (dx || dy) {
        fMat[kMTransX] = dx;
        fMat[kMTransY] = dy;

        fMat[kMScaleX] = fMat[kMScaleY] = fMat[kMPersp2] = 1;
        fMat[kMSkewX]  = fMat[kMSkewY] =
        fMat[kMPersp0] = fMat[kMPersp1] = 0;

        this->setTypeMask(kTranslate_Mask | kRectStaysRect_Mask);
    } else {
        this->reset();
    }
}

void SMatrix::preTranslate(float dx, float dy) {
    if (!dx && !dy) {
        return;
    }

    if (this->hasPerspective()) {
        SMatrix    m;
        m.setTranslate(dx, dy);
        this->preConcat(m);
    } else {
        fMat[kMTransX] += sdot(fMat[kMScaleX], dx, fMat[kMSkewX], dy);
        fMat[kMTransY] += sdot(fMat[kMSkewY], dx, fMat[kMScaleY], dy);
        this->setTypeMask(kUnknown_Mask | kOnlyPerspectiveValid_Mask);
    }
}

void SMatrix::postTranslate(float dx, float dy) {
    if (!dx && !dy) {
        return;
    }

    if (this->hasPerspective()) {
        SMatrix    m;
        m.setTranslate(dx, dy);
        this->postConcat(m);
    } else {
        fMat[kMTransX] += dx;
        fMat[kMTransY] += dy;
        this->setTypeMask(kUnknown_Mask | kOnlyPerspectiveValid_Mask);
    }
}

///////////////////////////////////////////////////////////////////////////////

void SMatrix::setScale(float sx, float sy, float px, float py) {
    if (1 == sx && 1 == sy) {
        this->reset();
    } else {
        fMat[kMScaleX] = sx;
        fMat[kMScaleY] = sy;
        fMat[kMTransX] = px - sx * px;
        fMat[kMTransY] = py - sy * py;
        fMat[kMPersp2] = 1;

        fMat[kMSkewX]  = fMat[kMSkewY] =
        fMat[kMPersp0] = fMat[kMPersp1] = 0;

        this->setTypeMask(kScale_Mask | kTranslate_Mask | kRectStaysRect_Mask);
    }
}

void SMatrix::setScale(float sx, float sy) {
    if (1 == sx && 1 == sy) {
        this->reset();
    } else {
        fMat[kMScaleX] = sx;
        fMat[kMScaleY] = sy;
        fMat[kMPersp2] = 1;

        fMat[kMTransX] = fMat[kMTransY] =
        fMat[kMSkewX]  = fMat[kMSkewY] =
        fMat[kMPersp0] = fMat[kMPersp1] = 0;

        this->setTypeMask(kScale_Mask | kRectStaysRect_Mask);
    }
}

bool SMatrix::setIDiv(int divx, int divy) {
    if (!divx || !divy) {
        return false;
    }
    this->setScale(SFloatInvert(divx), SFloatInvert(divy));
    return true;
}

void SMatrix::preScale(float sx, float sy, float px, float py) {
    if (1 == sx && 1 == sy) {
        return;
    }

    SMatrix    m;
    m.setScale(sx, sy, px, py);
    this->preConcat(m);
}

void SMatrix::preScale(float sx, float sy) {
    if (1 == sx && 1 == sy) {
        return;
    }

    // the assumption is that these multiplies are very cheap, and that
    // a full concat and/or just computing the matrix type is more expensive.
    // Also, the fixed-point case checks for overflow, but the float doesn't,
    // so we can get away with these blind multiplies.

    fMat[kMScaleX] *= sx;
    fMat[kMSkewY]  *= sx;
    fMat[kMPersp0] *= sx;

    fMat[kMSkewX]  *= sy;
    fMat[kMScaleY] *= sy;
    fMat[kMPersp1] *= sy;

    this->orTypeMask(kScale_Mask);
}

void SMatrix::postScale(float sx, float sy, float px, float py) {
    if (1 == sx && 1 == sy) {
        return;
    }
    SMatrix    m;
    m.setScale(sx, sy, px, py);
    this->postConcat(m);
}

void SMatrix::postScale(float sx, float sy) {
    if (1 == sx && 1 == sy) {
        return;
    }
    SMatrix    m;
    m.setScale(sx, sy);
    this->postConcat(m);
}

// this guy perhaps can go away, if we have a fract/high-precision way to
// scale matrices
bool SMatrix::postIDiv(int divx, int divy) {
    if (divx == 0 || divy == 0) {
        return false;
    }

    const float invX = 1.f / divx;
    const float invY = 1.f / divy;

    fMat[kMScaleX] *= invX;
    fMat[kMSkewX]  *= invX;
    fMat[kMTransX] *= invX;

    fMat[kMScaleY] *= invY;
    fMat[kMSkewY]  *= invY;
    fMat[kMTransY] *= invY;

    this->setTypeMask(kUnknown_Mask);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////

void SMatrix::setSinCos(float sinV, float cosV,
                         float px, float py) {
    const float oneMinusCosV = 1 - cosV;

    fMat[kMScaleX]  = cosV;
    fMat[kMSkewX]   = -sinV;
    fMat[kMTransX]  = sdot(sinV, py, oneMinusCosV, px);

    fMat[kMSkewY]   = sinV;
    fMat[kMScaleY]  = cosV;
    fMat[kMTransY]  = sdot(-sinV, px, oneMinusCosV, py);

    fMat[kMPersp0] = fMat[kMPersp1] = 0;
    fMat[kMPersp2] = 1;

    this->setTypeMask(kUnknown_Mask | kOnlyPerspectiveValid_Mask);
}

float SMatrix::SFloatSinCos(float radians, float* cosValue) {
	float sinValue = sk_float_sin(radians);

	if (cosValue) {
		*cosValue = sk_float_cos(radians);
		if (SFloatNearlyZero(*cosValue)) {
			*cosValue = 0;
		}
	}

	if (SFloatNearlyZero(sinValue)) {
		sinValue = 0;
	}
	return sinValue;
}

void SMatrix::setSinCos(float sinV, float cosV) {
    fMat[kMScaleX]  = cosV;
    fMat[kMSkewX]   = -sinV;
    fMat[kMTransX]  = 0;

    fMat[kMSkewY]   = sinV;
    fMat[kMScaleY]  = cosV;
    fMat[kMTransY]  = 0;

    fMat[kMPersp0] = fMat[kMPersp1] = 0;
    fMat[kMPersp2] = 1;

    this->setTypeMask(kUnknown_Mask | kOnlyPerspectiveValid_Mask);
}

void SMatrix::setRotate(float degrees, float px, float py) {
    float sinV, cosV;
    sinV = SFloatSinCos(SkDegreesToRadians(degrees), &cosV);
    this->setSinCos(sinV, cosV, px, py);
}

void SMatrix::setRotate(float degrees) {
    float sinV, cosV;
    sinV = SFloatSinCos(SkDegreesToRadians(degrees), &cosV);
    this->setSinCos(sinV, cosV);
}

void SMatrix::preRotate(float degrees, float px, float py) {
    SMatrix    m;
    m.setRotate(degrees, px, py);
    this->preConcat(m);
}

void SMatrix::preRotate(float degrees) {
    SMatrix    m;
    m.setRotate(degrees);
    this->preConcat(m);
}

void SMatrix::postRotate(float degrees, float px, float py) {
    SMatrix    m;
    m.setRotate(degrees, px, py);
    this->postConcat(m);
}

void SMatrix::postRotate(float degrees) {
    SMatrix    m;
    m.setRotate(degrees);
    this->postConcat(m);
}

////////////////////////////////////////////////////////////////////////////////////

void SMatrix::setSkew(float sx, float sy, float px, float py) {
    fMat[kMScaleX]  = 1;
    fMat[kMSkewX]   = sx;
    fMat[kMTransX]  = -sx * py;

    fMat[kMSkewY]   = sy;
    fMat[kMScaleY]  = 1;
    fMat[kMTransY]  = -sy * px;

    fMat[kMPersp0] = fMat[kMPersp1] = 0;
    fMat[kMPersp2] = 1;

    this->setTypeMask(kUnknown_Mask | kOnlyPerspectiveValid_Mask);
}

void SMatrix::setSkew(float sx, float sy) {
    fMat[kMScaleX]  = 1;
    fMat[kMSkewX]   = sx;
    fMat[kMTransX]  = 0;

    fMat[kMSkewY]   = sy;
    fMat[kMScaleY]  = 1;
    fMat[kMTransY]  = 0;

    fMat[kMPersp0] = fMat[kMPersp1] = 0;
    fMat[kMPersp2] = 1;

    this->setTypeMask(kUnknown_Mask | kOnlyPerspectiveValid_Mask);
}

void SMatrix::preSkew(float sx, float sy, float px, float py) {
    SMatrix    m;
    m.setSkew(sx, sy, px, py);
    this->preConcat(m);
}

void SMatrix::preSkew(float sx, float sy) {
    SMatrix    m;
    m.setSkew(sx, sy);
    this->preConcat(m);
}

void SMatrix::postSkew(float sx, float sy, float px, float py) {
    SMatrix    m;
    m.setSkew(sx, sy, px, py);
    this->postConcat(m);
}

void SMatrix::postSkew(float sx, float sy) {
    SMatrix    m;
    m.setSkew(sx, sy);
    this->postConcat(m);
}

///////////////////////////////////////////////////////////////////////////////

bool SMatrix::setRectToRect(const SRect& src, const SRect& dst,
                             ScaleToFit align)
{
    if (src.isEmpty()) {
        this->reset();
        return false;
    }

    if (dst.isEmpty()) {
        memset(fMat,0, 8 * sizeof(float));
        this->setTypeMask(kScale_Mask | kRectStaysRect_Mask);
    } else {
        float    tx, sx = dst.width() / src.width();
        float    ty, sy = dst.height() / src.height();
        bool        xLarger = false;

        if (align != kFill_ScaleToFit) {
            if (sx > sy) {
                xLarger = true;
                sx = sy;
            } else {
                sy = sx;
            }
        }

        tx = dst.fLeft - src.fLeft * sx;
        ty = dst.fTop - src.fTop * sy;
        if (align == kCenter_ScaleToFit || align == kEnd_ScaleToFit) {
            float diff;

            if (xLarger) {
                diff = dst.width() - src.width() * sy;
            } else {
                diff = dst.height() - src.height() * sy;
            }

            if (align == kCenter_ScaleToFit) {
                diff = SFloatHalf(diff);
            }

            if (xLarger) {
                tx += diff;
            } else {
                ty += diff;
            }
        }

        fMat[kMScaleX] = sx;
        fMat[kMScaleY] = sy;
        fMat[kMTransX] = tx;
        fMat[kMTransY] = ty;
        fMat[kMSkewX]  = fMat[kMSkewY] =
        fMat[kMPersp0] = fMat[kMPersp1] = 0;

        unsigned mask = kRectStaysRect_Mask;
        if (sx != 1 || sy != 1) {
            mask |= kScale_Mask;
        }
        if (tx || ty) {
            mask |= kTranslate_Mask;
        }
        this->setTypeMask(mask);
    }
    // shared cleanup
    fMat[kMPersp2] = 1;
    return true;
}

///////////////////////////////////////////////////////////////////////////////

static inline float muladdmul(float a, float b, float c, float d) {
    return SkDoubleToFloat((double)a * b + (double)c * d);
}

static inline float rowcol3(const float row[], const float col[]) {
    return row[0] * col[0] + row[1] * col[3] + row[2] * col[6];
}

static void normalize_perspective(float mat[9]) {
    if (SFloatAbs(mat[SMatrix::kMPersp2]) > 1) {
        for (int i = 0; i < 9; i++)
            mat[i] = SFloatHalf(mat[i]);
    }
}

void SMatrix::setConcat(const SMatrix& a, const SMatrix& b) {
    TypeMask aType = a.getPerspectiveTypeMaskOnly();
    TypeMask bType = b.getPerspectiveTypeMaskOnly();

    if (a.isTriviallyIdentity()) {
        *this = b;
    } else if (b.isTriviallyIdentity()) {
        *this = a;
    } else {
        SMatrix tmp;

        if ((aType | bType) & kPerspective_Mask) {
            tmp.fMat[kMScaleX] = rowcol3(&a.fMat[0], &b.fMat[0]);
            tmp.fMat[kMSkewX]  = rowcol3(&a.fMat[0], &b.fMat[1]);
            tmp.fMat[kMTransX] = rowcol3(&a.fMat[0], &b.fMat[2]);
            tmp.fMat[kMSkewY]  = rowcol3(&a.fMat[3], &b.fMat[0]);
            tmp.fMat[kMScaleY] = rowcol3(&a.fMat[3], &b.fMat[1]);
            tmp.fMat[kMTransY] = rowcol3(&a.fMat[3], &b.fMat[2]);
            tmp.fMat[kMPersp0] = rowcol3(&a.fMat[6], &b.fMat[0]);
            tmp.fMat[kMPersp1] = rowcol3(&a.fMat[6], &b.fMat[1]);
            tmp.fMat[kMPersp2] = rowcol3(&a.fMat[6], &b.fMat[2]);

            normalize_perspective(tmp.fMat);
            tmp.setTypeMask(kUnknown_Mask);
        } else {    // not perspective
            tmp.fMat[kMScaleX] = muladdmul(a.fMat[kMScaleX],
                                           b.fMat[kMScaleX],
                                           a.fMat[kMSkewX],
                                           b.fMat[kMSkewY]);

            tmp.fMat[kMSkewX]  = muladdmul(a.fMat[kMScaleX],
                                           b.fMat[kMSkewX],
                                           a.fMat[kMSkewX],
                                           b.fMat[kMScaleY]);

            tmp.fMat[kMTransX] = muladdmul(a.fMat[kMScaleX],
                                           b.fMat[kMTransX],
                                           a.fMat[kMSkewX],
                                           b.fMat[kMTransY]);

            tmp.fMat[kMTransX] += a.fMat[kMTransX];

            tmp.fMat[kMSkewY]  = muladdmul(a.fMat[kMSkewY],
                                           b.fMat[kMScaleX],
                                           a.fMat[kMScaleY],
                                           b.fMat[kMSkewY]);

            tmp.fMat[kMScaleY] = muladdmul(a.fMat[kMSkewY],
                                           b.fMat[kMSkewX],
                                           a.fMat[kMScaleY],
                                           b.fMat[kMScaleY]);

            tmp.fMat[kMTransY] = muladdmul(a.fMat[kMSkewY],
                                           b.fMat[kMTransX],
                                           a.fMat[kMScaleY],
                                           b.fMat[kMTransY]);

            tmp.fMat[kMTransY] += a.fMat[kMTransY];
            tmp.fMat[kMPersp0] = tmp.fMat[kMPersp1] = 0;
            tmp.fMat[kMPersp2] = 1;
            //SkDebugf("Concat mat non-persp type: %d\n", tmp.getType());
            //SASSERT(!(tmp.getType() & kPerspective_Mask));
            tmp.setTypeMask(kUnknown_Mask | kOnlyPerspectiveValid_Mask);
        }
        *this = tmp;
    }
}

void SMatrix::preConcat(const SMatrix& mat) {
    // check for identity first, so we don't do a needless copy of ourselves
    // to ourselves inside setConcat()
    if(!mat.isIdentity()) {
        this->setConcat(*this, mat);
    }
}

void SMatrix::postConcat(const SMatrix& mat) {
    // check for identity first, so we don't do a needless copy of ourselves
    // to ourselves inside setConcat()
    if (!mat.isIdentity()) {
        this->setConcat(mat, *this);
    }
}

///////////////////////////////////////////////////////////////////////////////

/*  Matrix inversion is very expensive, but also the place where keeping
    precision may be most important (here and matrix concat). Hence to avoid
    bitmap blitting artifacts when walking the inverse, we use doubles for
    the intermediate math, even though we know that is more expensive.
 */

static inline float scross_dscale(float a, float b,
                                     float c, float d, double scale) {
    return SkDoubleToScalar(scross(a, b, c, d) * scale);
}

static inline double dcross(double a, double b, double c, double d) {
    return a * b - c * d;
}

static inline float dcross_dscale(double a, double b,
                                     double c, double d, double scale) {
    return SkDoubleToScalar(dcross(a, b, c, d) * scale);
}

static double sk_inv_determinant(const float mat[9], int isPerspective) {
    double det;

    if (isPerspective) {
        det = mat[SMatrix::kMScaleX] *
              dcross(mat[SMatrix::kMScaleY], mat[SMatrix::kMPersp2],
                     mat[SMatrix::kMTransY], mat[SMatrix::kMPersp1])
              +
              mat[SMatrix::kMSkewX]  *
              dcross(mat[SMatrix::kMTransY], mat[SMatrix::kMPersp0],
                     mat[SMatrix::kMSkewY],  mat[SMatrix::kMPersp2])
              +
              mat[SMatrix::kMTransX] *
              dcross(mat[SMatrix::kMSkewY],  mat[SMatrix::kMPersp1],
                     mat[SMatrix::kMScaleY], mat[SMatrix::kMPersp0]);
    } else {
        det = dcross(mat[SMatrix::kMScaleX], mat[SMatrix::kMScaleY],
                     mat[SMatrix::kMSkewX], mat[SMatrix::kMSkewY]);
    }

    // Since the determinant is on the order of the cube of the matrix members,
    // compare to the cube of the default nearly-zero constant (although an
    // estimate of the condition number would be better if it wasn't so expensive).
    if (SFloatNearlyZero((float)det, SK_ScalarNearlyZero * SK_ScalarNearlyZero * SK_ScalarNearlyZero)) {
        return 0;
    }
    return 1.0 / det;
}

void SMatrix::SetAffineIdentity(float affine[6]) {
    affine[kAScaleX] = 1;
    affine[kASkewY] = 0;
    affine[kASkewX] = 0;
    affine[kAScaleY] = 1;
    affine[kATransX] = 0;
    affine[kATransY] = 0;
}

bool SMatrix::asAffine(float affine[6]) const {
    if (this->hasPerspective()) {
        return false;
    }
    if (affine) {
        affine[kAScaleX] = this->fMat[kMScaleX];
        affine[kASkewY] = this->fMat[kMSkewY];
        affine[kASkewX] = this->fMat[kMSkewX];
        affine[kAScaleY] = this->fMat[kMScaleY];
        affine[kATransX] = this->fMat[kMTransX];
        affine[kATransY] = this->fMat[kMTransY];
    }
    return true;
}

bool SMatrix::invertNonIdentity(SMatrix* inv) const {
    SASSERT(!this->isIdentity());

    TypeMask mask = this->getType();

    if (0 == (mask & ~(kScale_Mask | kTranslate_Mask))) {
        bool invertible = true;
        if (inv) {
            if (mask & kScale_Mask) {
                float invX = fMat[kMScaleX];
                float invY = fMat[kMScaleY];
                if (0 == invX || 0 == invY) {
                    return false;
                }
                invX = SFloatInvert(invX);
                invY = SFloatInvert(invY);

                // Must be careful when writing to inv, since it may be the
                // same memory as this.

                inv->fMat[kMSkewX] = inv->fMat[kMSkewY] =
                inv->fMat[kMPersp0] = inv->fMat[kMPersp1] = 0;

                inv->fMat[kMScaleX] = invX;
                inv->fMat[kMScaleY] = invY;
                inv->fMat[kMPersp2] = 1;
                inv->fMat[kMTransX] = -fMat[kMTransX] * invX;
                inv->fMat[kMTransY] = -fMat[kMTransY] * invY;

                inv->setTypeMask(mask | kRectStaysRect_Mask);
            } else {
                // translate only
                inv->setTranslate(-fMat[kMTransX], -fMat[kMTransY]);
            }
        } else {    // inv is NULL, just check if we're invertible
            if (!fMat[kMScaleX] || !fMat[kMScaleY]) {
                invertible = false;
            }
        }
        return invertible;
    }

    int    isPersp = mask & kPerspective_Mask;
    double scale = sk_inv_determinant(fMat, isPersp);

    if (scale == 0) { // underflow
        return false;
    }

    if (inv) {
        SMatrix tmp;
        if (inv == this) {
            inv = &tmp;
        }

        if (isPersp) {
            inv->fMat[kMScaleX] = scross_dscale(fMat[kMScaleY], fMat[kMPersp2], fMat[kMTransY], fMat[kMPersp1], scale);
            inv->fMat[kMSkewX]  = scross_dscale(fMat[kMTransX], fMat[kMPersp1], fMat[kMSkewX],  fMat[kMPersp2], scale);
            inv->fMat[kMTransX] = scross_dscale(fMat[kMSkewX],  fMat[kMTransY], fMat[kMTransX], fMat[kMScaleY], scale);

            inv->fMat[kMSkewY]  = scross_dscale(fMat[kMTransY], fMat[kMPersp0], fMat[kMSkewY],  fMat[kMPersp2], scale);
            inv->fMat[kMScaleY] = scross_dscale(fMat[kMScaleX], fMat[kMPersp2], fMat[kMTransX], fMat[kMPersp0], scale);
            inv->fMat[kMTransY] = scross_dscale(fMat[kMTransX], fMat[kMSkewY],  fMat[kMScaleX], fMat[kMTransY], scale);

            inv->fMat[kMPersp0] = scross_dscale(fMat[kMSkewY],  fMat[kMPersp1], fMat[kMScaleY], fMat[kMPersp0], scale);
            inv->fMat[kMPersp1] = scross_dscale(fMat[kMSkewX],  fMat[kMPersp0], fMat[kMScaleX], fMat[kMPersp1], scale);
            inv->fMat[kMPersp2] = scross_dscale(fMat[kMScaleX], fMat[kMScaleY], fMat[kMSkewX],  fMat[kMSkewY],  scale);
        } else {   // not perspective
            inv->fMat[kMScaleX] = SkDoubleToScalar(fMat[kMScaleY] * scale);
            inv->fMat[kMSkewX]  = SkDoubleToScalar(-fMat[kMSkewX] * scale);
            inv->fMat[kMTransX] = dcross_dscale(fMat[kMSkewX], fMat[kMTransY], fMat[kMScaleY], fMat[kMTransX], scale);

            inv->fMat[kMSkewY]  = SkDoubleToScalar(-fMat[kMSkewY] * scale);
            inv->fMat[kMScaleY] = SkDoubleToScalar(fMat[kMScaleX] * scale);
            inv->fMat[kMTransY] = dcross_dscale(fMat[kMSkewY], fMat[kMTransX], fMat[kMScaleX], fMat[kMTransY], scale);

            inv->fMat[kMPersp0] = 0;
            inv->fMat[kMPersp1] = 0;
            inv->fMat[kMPersp2] = 1;
        }

        inv->setTypeMask(fTypeMask);

        if (inv == &tmp) {
            *(SMatrix*)this = tmp;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////

void SMatrix::Identity_pts(const SMatrix& m, SPoint dst[],
                            const SPoint src[], int count) {
    SASSERT(m.getType() == 0);

    if (dst != src && count > 0)
        memcpy(dst, src, count * sizeof(SPoint));
}

void SMatrix::Trans_pts(const SMatrix& m, SPoint dst[],
                         const SPoint src[], int count) {
    SASSERT(m.getType() == kTranslate_Mask);

    if (count > 0) {
        float tx = m.fMat[kMTransX];
        float ty = m.fMat[kMTransY];
        do {
            dst->fY = src->fY + ty;
            dst->fX = src->fX + tx;
            src += 1;
            dst += 1;
        } while (--count);
    }
}

void SMatrix::Scale_pts(const SMatrix& m, SPoint dst[],
                         const SPoint src[], int count) {
    SASSERT(m.getType() == kScale_Mask);

    if (count > 0) {
        float mx = m.fMat[kMScaleX];
        float my = m.fMat[kMScaleY];
        do {
            dst->fY = src->fY * my;
            dst->fX = src->fX * mx;
            src += 1;
            dst += 1;
        } while (--count);
    }
}

void SMatrix::ScaleTrans_pts(const SMatrix& m, SPoint dst[],
                              const SPoint src[], int count) {
    SASSERT(m.getType() == (kScale_Mask | kTranslate_Mask));

    if (count > 0) {
        float mx = m.fMat[kMScaleX];
        float my = m.fMat[kMScaleY];
        float tx = m.fMat[kMTransX];
        float ty = m.fMat[kMTransY];
        do {
            dst->fY = src->fY * my + ty;
            dst->fX = src->fX * mx + tx;
            src += 1;
            dst += 1;
        } while (--count);
    }
}

void SMatrix::Rot_pts(const SMatrix& m, SPoint dst[],
                       const SPoint src[], int count) {
    SASSERT((m.getType() & (kPerspective_Mask | kTranslate_Mask)) == 0);

    if (count > 0) {
        float mx = m.fMat[kMScaleX];
        float my = m.fMat[kMScaleY];
        float kx = m.fMat[kMSkewX];
        float ky = m.fMat[kMSkewY];
        do {
            float sy = src->fY;
            float sx = src->fX;
            src += 1;
            dst->fY = sdot(sx, ky, sy, my);
            dst->fX = sdot(sx, mx, sy, kx);
            dst += 1;
        } while (--count);
    }
}

void SMatrix::RotTrans_pts(const SMatrix& m, SPoint dst[],
                            const SPoint src[], int count) {
    SASSERT(!m.hasPerspective());

    if (count > 0) {
        float mx = m.fMat[kMScaleX];
        float my = m.fMat[kMScaleY];
        float kx = m.fMat[kMSkewX];
        float ky = m.fMat[kMSkewY];
        float tx = m.fMat[kMTransX];
        float ty = m.fMat[kMTransY];
        do {
            float sy = src->fY;
            float sx = src->fX;
            src += 1;
#ifdef SK_LEGACY_MATRIX_MATH_ORDER
            dst->fY = sx * ky + (sy * my + ty);
            dst->fX = sx * mx + (sy * kx + tx);
#else
            dst->fY = sdot(sx, ky, sy, my) + ty;
            dst->fX = sdot(sx, mx, sy, kx) + tx;
#endif
            dst += 1;
        } while (--count);
    }
}

void SMatrix::Persp_pts(const SMatrix& m, SPoint dst[],
                         const SPoint src[], int count) {
    SASSERT(m.hasPerspective());

    if (count > 0) {
        do {
            float sy = src->fY;
            float sx = src->fX;
            src += 1;

            float x = sdot(sx, m.fMat[kMScaleX], sy, m.fMat[kMSkewX])  + m.fMat[kMTransX];
            float y = sdot(sx, m.fMat[kMSkewY],  sy, m.fMat[kMScaleY]) + m.fMat[kMTransY];
#ifdef SK_LEGACY_MATRIX_MATH_ORDER
            float z = sx * m.fMat[kMPersp0] + (sy * m.fMat[kMPersp1] + m.fMat[kMPersp2]);
#else
            float z = sdot(sx, m.fMat[kMPersp0], sy, m.fMat[kMPersp1]) + m.fMat[kMPersp2];
#endif
            if (z) {
                z = SFloatFastInvert(z);
            }

            dst->fY = y * z;
            dst->fX = x * z;
            dst += 1;
        } while (--count);
    }
}

const SMatrix::MapPtsProc SMatrix::gMapPtsProcs[] = {
    SMatrix::Identity_pts, SMatrix::Trans_pts,
    SMatrix::Scale_pts,    SMatrix::ScaleTrans_pts,
    SMatrix::Rot_pts,      SMatrix::RotTrans_pts,
    SMatrix::Rot_pts,      SMatrix::RotTrans_pts,
    // repeat the persp proc 8 times
    SMatrix::Persp_pts,    SMatrix::Persp_pts,
    SMatrix::Persp_pts,    SMatrix::Persp_pts,
    SMatrix::Persp_pts,    SMatrix::Persp_pts,
    SMatrix::Persp_pts,    SMatrix::Persp_pts
};

void SMatrix::mapPoints(SPoint dst[], const SPoint src[], int count) const {
    SASSERT((dst && src && count > 0) || 0 == count);
    // no partial overlap
    SASSERT(src == dst || &dst[count] <= &src[0] || &src[count] <= &dst[0]);

    this->getMapPtsProc()(*this, dst, src, count);
}

///////////////////////////////////////////////////////////////////////////////

static int32_t SkAbs32(int32_t value) {
	if (value < 0) {
		value = -value;
	}
	return value;
}

void SMatrix::mapHomogeneousPoints(float dst[], const float src[], int count) const {
    SASSERT((dst && src && count > 0) || 0 == count);
    // no partial overlap
    SASSERT(src == dst || SkAbs32((int32_t)(src - dst)) >= 3*count);

    if (count > 0) {
        if (this->isIdentity()) {
            memcpy(dst, src, 3*count*sizeof(float));
            return;
        }
        do {
            float sx = src[0];
            float sy = src[1];
            float sw = src[2];
            src += 3;

            float x = sdot(sx, fMat[kMScaleX], sy, fMat[kMSkewX],  sw, fMat[kMTransX]);
            float y = sdot(sx, fMat[kMSkewY],  sy, fMat[kMScaleY], sw, fMat[kMTransY]);
            float w = sdot(sx, fMat[kMPersp0], sy, fMat[kMPersp1], sw, fMat[kMPersp2]);

            dst[0] = x;
            dst[1] = y;
            dst[2] = w;
            dst += 3;
        } while (--count);
    }
}

///////////////////////////////////////////////////////////////////////////////

void SMatrix::mapVectors(SPoint dst[], const SPoint src[], int count) const {
    if (this->hasPerspective()) {
        SPoint origin;

        MapXYProc proc = this->getMapXYProc();
        proc(*this, 0, 0, &origin);

        for (int i = count - 1; i >= 0; --i) {
            SPoint tmp;

            proc(*this, src[i].fX, src[i].fY, &tmp);
            dst[i].set(tmp.fX - origin.fX, tmp.fY - origin.fY);
        }
    } else {
        SMatrix tmp = *this;

        tmp.fMat[kMTransX] = tmp.fMat[kMTransY] = 0;
        tmp.clearTypeMask(kTranslate_Mask);
        tmp.mapPoints(dst, src, count);
    }
}

bool SMatrix::mapRect(SRect* dst, const SRect& src) const {
    SASSERT(dst);

    if (this->rectStaysRect()) {
        this->mapPoints((SPoint*)dst, (const SPoint*)&src, 2);
        dst->sort();
        return true;
    } else {
        SPoint quad[4];

        src.toQuad(quad);
        this->mapPoints(quad, quad, 4);
        dst->set(quad, 4);
        return false;
    }
}

float SMatrix::mapRadius(float radius) const {
    SVector2D    vec[2];

    vec[0].set(radius, 0);
    vec[1].set(0, radius);
    this->mapVectors(vec, 2);

    float d0 = vec[0].length();
    float d1 = vec[1].length();

    // return geometric mean
    return SFloatSqrt(d0 * d1);
}

///////////////////////////////////////////////////////////////////////////////

void SMatrix::Persp_xy(const SMatrix& m, float sx, float sy,
                        SPoint* pt) {
    SASSERT(m.hasPerspective());

    float x = sdot(sx, m.fMat[kMScaleX], sy, m.fMat[kMSkewX])  + m.fMat[kMTransX];
    float y = sdot(sx, m.fMat[kMSkewY],  sy, m.fMat[kMScaleY]) + m.fMat[kMTransY];
    float z = sdot(sx, m.fMat[kMPersp0], sy, m.fMat[kMPersp1]) + m.fMat[kMPersp2];
    if (z) {
        z = SFloatFastInvert(z);
    }
    pt->fX = x * z;
    pt->fY = y * z;
}

void SMatrix::RotTrans_xy(const SMatrix& m, float sx, float sy,
                           SPoint* pt) {
    SASSERT((m.getType() & (kAffine_Mask | kPerspective_Mask)) == kAffine_Mask);

#ifdef SK_LEGACY_MATRIX_MATH_ORDER
    pt->fX = sx * m.fMat[kMScaleX] + (sy * m.fMat[kMSkewX]  +  m.fMat[kMTransX]);
    pt->fY = sx * m.fMat[kMSkewY]  + (sy * m.fMat[kMScaleY] + m.fMat[kMTransY]);
#else
    pt->fX = sdot(sx, m.fMat[kMScaleX], sy, m.fMat[kMSkewX])  + m.fMat[kMTransX];
    pt->fY = sdot(sx, m.fMat[kMSkewY],  sy, m.fMat[kMScaleY]) + m.fMat[kMTransY];
#endif
}

void SMatrix::Rot_xy(const SMatrix& m, float sx, float sy,
                      SPoint* pt) {
    SASSERT((m.getType() & (kAffine_Mask | kPerspective_Mask))== kAffine_Mask);
    SASSERT(0 == m.fMat[kMTransX]);
    SASSERT(0 == m.fMat[kMTransY]);

#ifdef SK_LEGACY_MATRIX_MATH_ORDER
    pt->fX = sx * m.fMat[kMScaleX] + (sy * m.fMat[kMSkewX]  + m.fMat[kMTransX]);
    pt->fY = sx * m.fMat[kMSkewY]  + (sy * m.fMat[kMScaleY] + m.fMat[kMTransY]);
#else
    pt->fX = sdot(sx, m.fMat[kMScaleX], sy, m.fMat[kMSkewX])  + m.fMat[kMTransX];
    pt->fY = sdot(sx, m.fMat[kMSkewY],  sy, m.fMat[kMScaleY]) + m.fMat[kMTransY];
#endif
}

void SMatrix::ScaleTrans_xy(const SMatrix& m, float sx, float sy,
                             SPoint* pt) {
    SASSERT((m.getType() & (kScale_Mask | kAffine_Mask | kPerspective_Mask))
             == kScale_Mask);

    pt->fX = sx * m.fMat[kMScaleX] + m.fMat[kMTransX];
    pt->fY = sy * m.fMat[kMScaleY] + m.fMat[kMTransY];
}

void SMatrix::Scale_xy(const SMatrix& m, float sx, float sy,
                        SPoint* pt) {
    SASSERT((m.getType() & (kScale_Mask | kAffine_Mask | kPerspective_Mask))
             == kScale_Mask);
    SASSERT(0 == m.fMat[kMTransX]);
    SASSERT(0 == m.fMat[kMTransY]);

    pt->fX = sx * m.fMat[kMScaleX];
    pt->fY = sy * m.fMat[kMScaleY];
}

void SMatrix::Trans_xy(const SMatrix& m, float sx, float sy,
                        SPoint* pt) {
    SASSERT(m.getType() == kTranslate_Mask);

    pt->fX = sx + m.fMat[kMTransX];
    pt->fY = sy + m.fMat[kMTransY];
}

void SMatrix::Identity_xy(const SMatrix& m, float sx, float sy,
                           SPoint* pt) {
    SASSERT(0 == m.getType());

    pt->fX = sx;
    pt->fY = sy;
}

const SMatrix::MapXYProc SMatrix::gMapXYProcs[] = {
    SMatrix::Identity_xy, SMatrix::Trans_xy,
    SMatrix::Scale_xy,    SMatrix::ScaleTrans_xy,
    SMatrix::Rot_xy,      SMatrix::RotTrans_xy,
    SMatrix::Rot_xy,      SMatrix::RotTrans_xy,
    // repeat the persp proc 8 times
    SMatrix::Persp_xy,    SMatrix::Persp_xy,
    SMatrix::Persp_xy,    SMatrix::Persp_xy,
    SMatrix::Persp_xy,    SMatrix::Persp_xy,
    SMatrix::Persp_xy,    SMatrix::Persp_xy
};

///////////////////////////////////////////////////////////////////////////////

// if its nearly zero (just made up 26, perhaps it should be bigger or smaller)
#define PerspNearlyZero(x)  SFloatNearlyZero(x, (1.0f / (1 << 26)))


///////////////////////////////////////////////////////////////////////////////

static inline bool checkForZero(float x) {
    return x*x == 0;
}

static inline bool poly_to_point(SPoint* pt, const SPoint poly[], int count) {
    float   x = 1, y = 1;
    SPoint pt1, pt2;

    if (count > 1) {
        pt1.fX = poly[1].fX - poly[0].fX;
        pt1.fY = poly[1].fY - poly[0].fY;
        y = SPoint::Length(pt1.fX, pt1.fY);
        if (checkForZero(y)) {
            return false;
        }
        switch (count) {
            case 2:
                break;
            case 3:
                pt2.fX = poly[0].fY - poly[2].fY;
                pt2.fY = poly[2].fX - poly[0].fX;
                goto CALC_X;
            default:
                pt2.fX = poly[0].fY - poly[3].fY;
                pt2.fY = poly[3].fX - poly[0].fX;
            CALC_X:
                x = sdot(pt1.fX, pt2.fX, pt1.fY, pt2.fY) / y;
                break;
        }
    }
    pt->set(x, y);
    return true;
}

bool SMatrix::Poly2Proc(const SPoint srcPt[], SMatrix* dst,
                         const SPoint& scale) {
    float invScale = 1 / scale.fY;

    dst->fMat[kMScaleX] = (srcPt[1].fY - srcPt[0].fY) * invScale;
    dst->fMat[kMSkewY] = (srcPt[0].fX - srcPt[1].fX) * invScale;
    dst->fMat[kMPersp0] = 0;
    dst->fMat[kMSkewX] = (srcPt[1].fX - srcPt[0].fX) * invScale;
    dst->fMat[kMScaleY] = (srcPt[1].fY - srcPt[0].fY) * invScale;
    dst->fMat[kMPersp1] = 0;
    dst->fMat[kMTransX] = srcPt[0].fX;
    dst->fMat[kMTransY] = srcPt[0].fY;
    dst->fMat[kMPersp2] = 1;
    dst->setTypeMask(kUnknown_Mask);
    return true;
}

bool SMatrix::Poly3Proc(const SPoint srcPt[], SMatrix* dst,
                         const SPoint& scale) {
    float invScale = 1 / scale.fX;
    dst->fMat[kMScaleX] = (srcPt[2].fX - srcPt[0].fX) * invScale;
    dst->fMat[kMSkewY] = (srcPt[2].fY - srcPt[0].fY) * invScale;
    dst->fMat[kMPersp0] = 0;

    invScale = 1 / scale.fY;
    dst->fMat[kMSkewX] = (srcPt[1].fX - srcPt[0].fX) * invScale;
    dst->fMat[kMScaleY] = (srcPt[1].fY - srcPt[0].fY) * invScale;
    dst->fMat[kMPersp1] = 0;

    dst->fMat[kMTransX] = srcPt[0].fX;
    dst->fMat[kMTransY] = srcPt[0].fY;
    dst->fMat[kMPersp2] = 1;
    dst->setTypeMask(kUnknown_Mask);
    return true;
}

bool SMatrix::Poly4Proc(const SPoint srcPt[], SMatrix* dst,
                         const SPoint& scale) {
    float   a1, a2;
    float   x0, y0, x1, y1, x2, y2;

    x0 = srcPt[2].fX - srcPt[0].fX;
    y0 = srcPt[2].fY - srcPt[0].fY;
    x1 = srcPt[2].fX - srcPt[1].fX;
    y1 = srcPt[2].fY - srcPt[1].fY;
    x2 = srcPt[2].fX - srcPt[3].fX;
    y2 = srcPt[2].fY - srcPt[3].fY;

    /* check if abs(x2) > abs(y2) */
    if ( x2 > 0 ? y2 > 0 ? x2 > y2 : x2 > -y2 : y2 > 0 ? -x2 > y2 : x2 < y2) {
        float denom = SFloatMulDiv(x1, y2, x2) - y1;
        if (checkForZero(denom)) {
            return false;
        }
        a1 = (SFloatMulDiv(x0 - x1, y2, x2) - y0 + y1) / denom;
    } else {
        float denom = x1 - SFloatMulDiv(y1, x2, y2);
        if (checkForZero(denom)) {
            return false;
        }
        a1 = (x0 - x1 - SFloatMulDiv(y0 - y1, x2, y2)) / denom;
    }

    /* check if abs(x1) > abs(y1) */
    if ( x1 > 0 ? y1 > 0 ? x1 > y1 : x1 > -y1 : y1 > 0 ? -x1 > y1 : x1 < y1) {
        float denom = y2 - SFloatMulDiv(x2, y1, x1);
        if (checkForZero(denom)) {
            return false;
        }
        a2 = (y0 - y2 - SFloatMulDiv(x0 - x2, y1, x1)) / denom;
    } else {
        float denom = SFloatMulDiv(y2, x1, y1) - x2;
        if (checkForZero(denom)) {
            return false;
        }
        a2 = (SFloatMulDiv(y0 - y2, x1, y1) - x0 + x2) / denom;
    }

    float invScale = SFloatInvert(scale.fX);
    dst->fMat[kMScaleX] = (a2 * srcPt[3].fX + srcPt[3].fX - srcPt[0].fX) * invScale;
    dst->fMat[kMSkewY]  = (a2 * srcPt[3].fY + srcPt[3].fY - srcPt[0].fY) * invScale;
    dst->fMat[kMPersp0] = a2 * invScale;

    invScale = SFloatInvert(scale.fY);
    dst->fMat[kMSkewX]  = (a1 * srcPt[1].fX + srcPt[1].fX - srcPt[0].fX) * invScale;
    dst->fMat[kMScaleY] = (a1 * srcPt[1].fY + srcPt[1].fY - srcPt[0].fY) * invScale;
    dst->fMat[kMPersp1] = a1 * invScale;

    dst->fMat[kMTransX] = srcPt[0].fX;
    dst->fMat[kMTransY] = srcPt[0].fY;
    dst->fMat[kMPersp2] = 1;
    dst->setTypeMask(kUnknown_Mask);
    return true;
}

typedef bool (*PolyMapProc)(const SPoint[], SMatrix*, const SPoint&);

/*  Taken from Rob Johnson's original sample code in QuickDraw GX
*/
bool SMatrix::setPolyToPoly(const SPoint src[], const SPoint dst[],
                             int count) {
    if ((unsigned)count > 4) {
//        SkDebugf("--- SkMatrix::setPolyToPoly count out of range %d\n", count);
        return false;
    }

    if (0 == count) {
        this->reset();
        return true;
    }
    if (1 == count) {
        this->setTranslate(dst[0].fX - src[0].fX, dst[0].fY - src[0].fY);
        return true;
    }

    SPoint scale;
    if (!poly_to_point(&scale, src, count) ||
            SFloatNearlyZero(scale.fX) ||
            SFloatNearlyZero(scale.fY)) {
        return false;
    }

    static const PolyMapProc gPolyMapProcs[] = {
        SMatrix::Poly2Proc, SMatrix::Poly3Proc, SMatrix::Poly4Proc
    };
    PolyMapProc proc = gPolyMapProcs[count - 2];

    SMatrix tempMap, result;
    tempMap.setTypeMask(kUnknown_Mask);

    if (!proc(src, &tempMap, scale)) {
        return false;
    }
    if (!tempMap.invert(&result)) {
        return false;
    }
    if (!proc(dst, &tempMap, scale)) {
        return false;
    }
    this->setConcat(tempMap, result);
    return true;
}

///////////////////////////////////////////////////////////////////////////////

enum MinMaxOrBoth {
    kMin_MinMaxOrBoth,
    kMax_MinMaxOrBoth,
    kBoth_MinMaxOrBoth
};

template <MinMaxOrBoth MIN_MAX_OR_BOTH> bool get_scale_factor(SMatrix::TypeMask typeMask,
                                                              const float m[9],
                                                              float results[/*1 or 2*/]) {
    if (typeMask & SMatrix::kPerspective_Mask) {
        return false;
    }
    if (SMatrix::kIdentity_Mask == typeMask) {
        results[0] = SK_Scalar1;
        if (kBoth_MinMaxOrBoth == MIN_MAX_OR_BOTH) {
            results[1] = SK_Scalar1;
        }
        return true;
    }
    if (!(typeMask & SMatrix::kAffine_Mask)) {
        if (kMin_MinMaxOrBoth == MIN_MAX_OR_BOTH) {
             results[0] = SkMinScalar(SFloatAbs(m[SMatrix::kMScaleX]),
                                      SFloatAbs(m[SMatrix::kMScaleY]));
        } else if (kMax_MinMaxOrBoth == MIN_MAX_OR_BOTH) {
             results[0] = SkMaxScalar(SFloatAbs(m[SMatrix::kMScaleX]),
                                      SFloatAbs(m[SMatrix::kMScaleY]));
        } else {
            results[0] = SFloatAbs(m[SMatrix::kMScaleX]);
            results[1] = SFloatAbs(m[SMatrix::kMScaleY]);
             if (results[0] > results[1]) {
                 STSwap(results[0], results[1]);
             }
        }
        return true;
    }
    // ignore the translation part of the matrix, just look at 2x2 portion.
    // compute singular values, take largest or smallest abs value.
    // [a b; b c] = A^T*A
    float a = sdot(m[SMatrix::kMScaleX], m[SMatrix::kMScaleX],
                      m[SMatrix::kMSkewY],  m[SMatrix::kMSkewY]);
    float b = sdot(m[SMatrix::kMScaleX], m[SMatrix::kMSkewX],
                      m[SMatrix::kMScaleY], m[SMatrix::kMSkewY]);
    float c = sdot(m[SMatrix::kMSkewX],  m[SMatrix::kMSkewX],
                      m[SMatrix::kMScaleY], m[SMatrix::kMScaleY]);
    // eigenvalues of A^T*A are the squared singular values of A.
    // characteristic equation is det((A^T*A) - l*I) = 0
    // l^2 - (a + c)l + (ac-b^2)
    // solve using quadratic equation (divisor is non-zero since l^2 has 1 coeff
    // and roots are guaranteed to be pos and real).
    float bSqd = b * b;
    // if upper left 2x2 is orthogonal save some math
    if (bSqd <= SK_ScalarNearlyZero*SK_ScalarNearlyZero) {
        if (kMin_MinMaxOrBoth == MIN_MAX_OR_BOTH) {
            results[0] = SkMinScalar(a, c);
        } else if (kMax_MinMaxOrBoth == MIN_MAX_OR_BOTH) {
            results[0] = SkMaxScalar(a, c);
        } else {
            results[0] = a;
            results[1] = c;
            if (results[0] > results[1]) {
                STSwap(results[0], results[1]);
            }
        }
    } else {
        float aminusc = a - c;
        float apluscdiv2 = SFloatHalf(a + c);
        float x = SFloatHalf(SFloatSqrt(aminusc * aminusc + 4 * bSqd));
        if (kMin_MinMaxOrBoth == MIN_MAX_OR_BOTH) {
            results[0] = apluscdiv2 - x;
        } else if (kMax_MinMaxOrBoth == MIN_MAX_OR_BOTH) {
            results[0] = apluscdiv2 + x;
        } else {
            results[0] = apluscdiv2 - x;
            results[1] = apluscdiv2 + x;
        }
    }
    SASSERT(results[0] >= 0);
    results[0] = SFloatSqrt(results[0]);
    if (kBoth_MinMaxOrBoth == MIN_MAX_OR_BOTH) {
        SASSERT(results[1] >= 0);
        results[1] = SFloatSqrt(results[1]);
    }
    return true;
}

float SMatrix::getMinScale() const {
    float factor;
    if (get_scale_factor<kMin_MinMaxOrBoth>(this->getType(), fMat, &factor)) {
        return factor;
    } else {
        return -1;
    }
}

float SMatrix::getMaxScale() const {
    float factor;
    if (get_scale_factor<kMax_MinMaxOrBoth>(this->getType(), fMat, &factor)) {
        return factor;
    } else {
        return -1;
    }
}

bool SMatrix::getMinMaxScales(float scaleFactors[2]) const {
    return get_scale_factor<kBoth_MinMaxOrBoth>(this->getType(), fMat, scaleFactors);
}

namespace {

struct PODMatrix {
    float matrix[9];
    uint32_t typemask;

    const SMatrix& asSkMatrix() const { return *reinterpret_cast<const SMatrix*>(this); }
};
}  // namespace

const SMatrix& SMatrix::I() {
    static const PODMatrix identity = { {SK_Scalar1, 0, 0,
                                         0, SK_Scalar1, 0,
                                         0, 0, SK_Scalar1 },
                                       kIdentity_Mask | kRectStaysRect_Mask};
    SASSERT(identity.asSkMatrix().isIdentity());
    return identity.asSkMatrix();
}

const SMatrix& SMatrix::InvalidMatrix() {

    static const PODMatrix invalid =
        { {SK_ScalarMax, SK_ScalarMax, SK_ScalarMax,
           SK_ScalarMax, SK_ScalarMax, SK_ScalarMax,
           SK_ScalarMax, SK_ScalarMax, SK_ScalarMax },
         kTranslate_Mask | kScale_Mask | kAffine_Mask | kPerspective_Mask };
    return invalid.asSkMatrix();
}

///////////////////////////////////////////////////////////////////////////////

float SMatrix::GetScaleX() const
{
	return fMat[kMScaleX];
}

float SMatrix::GetScaleY() const
{
	return fMat[kMScaleY];
}

float SMatrix::GetSkewX() const
{
	return fMat[kMSkewX];
}

float SMatrix::GetSkewY() const
{
	return fMat[kMSkewY];
}

float SMatrix::GetTranslateX() const
{
	return fMat[kMTransX];
}

float SMatrix::GetTranslateY() const
{
	return fMat[kMTransY];
}

float SMatrix::GetPersp0() const
{
	return fMat[kMPersp0];
}

float SMatrix::GetPersp1() const
{
	return fMat[kMPersp1];
}

float SMatrix::GetPersp2() const
{
	return fMat[kMPersp2];
}

float SMatrix::GetValue(Index idx) const
{
	return fMat[idx];
}

const float * SMatrix::GetData() const
{
	return fMat;
}

float * SMatrix::GetData()
{
	return fMat;
}


void SMatrix::SetScaleX(float v)
{
	set(kMScaleX,v);
}

void SMatrix::SetScaleY(float v)
{
	set(kMScaleY,v);
}

void SMatrix::SetSkewX(float v)
{
	set(kMSkewX,v);
}

void SMatrix::SetSkewY(float v)
{
	set(kMSkewY,v);
}

void SMatrix::SetTranslateX(float v)
{
	set(kMTransX,v);
}

void SMatrix::SetTranslateY(float v)
{
	set(kMTransY,v);
}

void SMatrix::SetPersp0(float v)
{
	set(kMPersp0,v);
}

void SMatrix::SetPersp1(float v)
{
	set(kMPersp1,v);
}

void SMatrix::SetPersp2(float v)
{
	set(kMPersp2,v);
}

void SMatrix::SetValue(Index idx, float v)
{
	set(idx, v);
}

void SMatrix::SetData(const float fMat[9])
{
	memcpy(this->fMat, fMat, sizeof(this->fMat));
	this->setTypeMask(kUnknown_Mask);
}


void SMatrix::Clear()
{
	reset();
}


void SMatrix::Concat(const IxForm * src)
{
	SMatrix mSrc(src->GetData());
	postConcat(mSrc);
}



/*!
\fn SMatrix &SMatrix::operator *=(const SMatrix &matrix)
\overload

Returns the result of multiplying this matrix by the given \a
matrix.
*/

SMatrix &SMatrix::operator *=(const SMatrix &src)
{
	Concat(&src);
	return *this;
}

/*!
\fn SMatrix SMatrix::operator *(const SMatrix &matrix) const

Returns the result of multiplying this matrix by the given \a
matrix.

Note that matrix multiplication is not commutative, i.e. a*b !=
b*a.
*/

SMatrix SMatrix::operator *(const SMatrix &m) const
{
	SMatrix ret(*this);
	ret.postConcat(m);
	return ret;
}

/*!
Assigns the given \a matrix's values to this matrix.
*/
SMatrix &SMatrix::operator=(const SMatrix &src)
{
	memcpy(fMat, src.fMat, sizeof(fMat));
	return *this;
}
}//end of namespace SOUI
