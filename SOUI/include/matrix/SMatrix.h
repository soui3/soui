
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef _SMatrix_DEFINED_
#define _SMatrix_DEFINED_

#include "SRect.h"

#include <interface/render-i.h>

namespace SOUI{

/** \class SkMatrix

    The SkMatrix class holds a 3x3 matrix for transforming coordinates.
    SkMatrix does not have a constructor, so it must be explicitly initialized
    using either reset() - to construct an identity matrix, or one of the set
    functions (e.g. setTranslate, setRotate, etc.).
*/
class SOUI_EXP SMatrix : public IxForm{
public:
	SMatrix(){
		reset();
	}
	SMatrix(const float data[9]);

public:
	virtual float GetScaleX() const;

	virtual float GetScaleY() const;

	virtual float GetSkewX() const;

	virtual float GetSkewY() const;

	virtual float GetTranslateX() const;

	virtual float GetTranslateY() const;

	virtual float GetPersp0() const;

	virtual float GetPersp1() const;

	virtual float GetValue(Index idx) const;

	virtual const float * GetData() const;

	virtual float * GetData();

	virtual float GetPersp2() const;

	virtual void SetScaleX(float v);

	virtual void SetScaleY(float v);

	virtual void SetSkewX(float v);

	virtual void SetSkewY(float v);

	virtual void SetTranslateX(float v);

	virtual void SetTranslateY(float v);

	virtual void SetPersp0(float v);

	virtual void SetPersp1(float v);

	virtual void SetPersp2(float v);

	virtual void SetValue(Index idx,float v);

	virtual void SetData(const float fMat[9]);

	virtual void Clear();

	virtual void Concat(const IxForm * src);

public:
	SMatrix &operator*=(const SMatrix &src);
	SMatrix operator*(const SMatrix &src) const;

	SMatrix &operator=(const SMatrix &src);

	friend bool operator==(const SMatrix& a, const SMatrix& b);
	friend bool operator!=(const SMatrix& a, const SMatrix& b) {
		return !(a == b);
	}

public:
	SMatrix & translate(float dx, float dy)
	{
		postTranslate(dx, dy);
		return *this;
	}

	SMatrix & scale(float sx, float sy)
	{
		postScale(sx, sy);
		return *this;
	}

	SMatrix & shear(float sh, float sv)
	{
		postSkew(sh, sv);
		return *this;
	}

	SMatrix & rotate(float deg)
	{
		postRotate(deg);
		return *this;
	}

public:
    /** Enum of bit fields for the mask return by getType().
        Use this to identify the complexity of the matrix.
    */
    enum TypeMask {
        kIdentity_Mask      = 0,
        kTranslate_Mask     = 0x01,  //!< set if the matrix has translation
        kScale_Mask         = 0x02,  //!< set if the matrix has X or Y scale
        kAffine_Mask        = 0x04,  //!< set if the matrix skews or rotates
        kPerspective_Mask   = 0x08   //!< set if the matrix is in perspective
    };

    /** Returns a bitfield describing the transformations the matrix may
        perform. The bitfield is computed conservatively, so it may include
        false positives. For example, when kPerspective_Mask is true, all
        other bits may be set to true even in the case of a pure perspective
        transform.
   */
    TypeMask getType() const {
        if (fTypeMask & kUnknown_Mask) {
            fTypeMask = this->computeTypeMask();
        }
        // only return the public masks
        return (TypeMask)(fTypeMask & 0xF);
    }

    /** Returns true if the matrix is identity.
    */
    bool isIdentity() const {
        return this->getType() == 0;
    }

    bool isScaleTranslate() const {
        return !(this->getType() & ~(kScale_Mask | kTranslate_Mask));
    }

    /** Returns true if will map a rectangle to another rectangle. This can be
        true if the matrix is identity, scale-only, or rotates a multiple of
        90 degrees.
    */
    bool rectStaysRect() const {
        if (fTypeMask & kUnknown_Mask) {
            fTypeMask = this->computeTypeMask();
        }
        return (fTypeMask & kRectStaysRect_Mask) != 0;
    }
    // alias for rectStaysRect()
    bool preservesAxisAlignment() const { return this->rectStaysRect(); }

    /**
     *  Returns true if the matrix contains perspective elements.
     */
    bool hasPerspective() const {
        return SkToBool(this->getPerspectiveTypeMaskOnly() &
                        kPerspective_Mask);
    }

    /** Returns true if the matrix contains only translation, rotation/reflection or uniform scale
        Returns false if other transformation types are included or is degenerate
     */
    bool isSimilarity(float tol = SK_ScalarNearlyZero) const;

    /** Returns true if the matrix contains only translation, rotation/reflection or scale
        (non-uniform scale is allowed).
        Returns false if other transformation types are included or is degenerate
     */
    bool preservesRightAngles(float tol = SK_ScalarNearlyZero) const;

    /** Affine arrays are in column major order
        because that's how PDF and XPS like it.
     */
    enum {
        kAScaleX,
        kASkewY,
        kASkewX,
        kAScaleY,
        kATransX,
        kATransY
    };

    float operator[](int index) const {
        SASSERT((unsigned)index < 9);
        return fMat[index];
    }

    float get(int index) const {
        SASSERT((unsigned)index < 9);
        return fMat[index];
    }

    float& operator[](int index) {
        SASSERT((unsigned)index < 9);
        this->setTypeMask(kUnknown_Mask);
        return fMat[index];
    }

    void set(int index, float value) {
        SASSERT((unsigned)index < 9);
        fMat[index] = value;
        this->setTypeMask(kUnknown_Mask);
    }


    /** Set the matrix to identity
    */
    void reset();
    // alias for reset()
    void setIdentity() { this->reset(); }

    /** Set the matrix to translate by (dx, dy).
    */
    void setTranslate(float dx, float dy);
    void setTranslate(const SVector2D& v) { this->setTranslate(v.fX, v.fY); }

    /** Set the matrix to scale by sx and sy, with a pivot point at (px, py).
        The pivot point is the coordinate that should remain unchanged by the
        specified transformation.
    */
    void setScale(float sx, float sy, float px, float py);
    /** Set the matrix to scale by sx and sy.
    */
    void setScale(float sx, float sy);
    /** Set the matrix to scale by 1/divx and 1/divy. Returns false and doesn't
        touch the matrix if either divx or divy is zero.
    */
    bool setIDiv(int divx, int divy);
    /** Set the matrix to rotate by the specified number of degrees, with a
        pivot point at (px, py). The pivot point is the coordinate that should
        remain unchanged by the specified transformation.
    */
    void setRotate(float degrees, float px, float py);
    /** Set the matrix to rotate about (0,0) by the specified number of degrees.
    */
    void setRotate(float degrees);
    /** Set the matrix to rotate by the specified sine and cosine values, with
        a pivot point at (px, py). The pivot point is the coordinate that
        should remain unchanged by the specified transformation.
    */
    void setSinCos(float sinValue, float cosValue,
                   float px, float py);
    /** Set the matrix to rotate by the specified sine and cosine values.
    */
    void setSinCos(float sinValue, float cosValue);
    /** Set the matrix to skew by sx and sy, with a pivot point at (px, py).
        The pivot point is the coordinate that should remain unchanged by the
        specified transformation.
    */
    void setSkew(float kx, float ky, float px, float py);
    /** Set the matrix to skew by sx and sy.
    */
    void setSkew(float kx, float ky);
    /** Set the matrix to the concatenation of the two specified matrices.
        Either of the two matrices may also be the target matrix.
        *this = a * b;
    */
    void setConcat(const SMatrix& a, const SMatrix& b);

    /** Preconcats the matrix with the specified translation.
        M' = M * T(dx, dy)
    */
    void preTranslate(float dx, float dy);
    /** Preconcats the matrix with the specified scale.
        M' = M * S(sx, sy, px, py)
    */
    void preScale(float sx, float sy, float px, float py);
    /** Preconcats the matrix with the specified scale.
        M' = M * S(sx, sy)
    */
    void preScale(float sx, float sy);
    /** Preconcats the matrix with the specified rotation.
        M' = M * R(degrees, px, py)
    */
    void preRotate(float degrees, float px, float py);
    /** Preconcats the matrix with the specified rotation.
        M' = M * R(degrees)
    */
    void preRotate(float degrees);
    /** Preconcats the matrix with the specified skew.
        M' = M * K(kx, ky, px, py)
    */
    void preSkew(float kx, float ky, float px, float py);
    /** Preconcats the matrix with the specified skew.
        M' = M * K(kx, ky)
    */
    void preSkew(float kx, float ky);
    /** Preconcats the matrix with the specified matrix.
        M' = M * other
    */
    void preConcat(const SMatrix& other);

    /** Postconcats the matrix with the specified translation.
        M' = T(dx, dy) * M
    */
    void postTranslate(float dx, float dy);
    /** Postconcats the matrix with the specified scale.
        M' = S(sx, sy, px, py) * M
    */
    void postScale(float sx, float sy, float px, float py);
    /** Postconcats the matrix with the specified scale.
        M' = S(sx, sy) * M
    */
    void postScale(float sx, float sy);
    /** Postconcats the matrix by dividing it by the specified integers.
        M' = S(1/divx, 1/divy, 0, 0) * M
    */
    bool postIDiv(int divx, int divy);
    /** Postconcats the matrix with the specified rotation.
        M' = R(degrees, px, py) * M
    */
    void postRotate(float degrees, float px, float py);
    /** Postconcats the matrix with the specified rotation.
        M' = R(degrees) * M
    */
    void postRotate(float degrees);
    /** Postconcats the matrix with the specified skew.
        M' = K(kx, ky, px, py) * M
    */
    void postSkew(float kx, float ky, float px, float py);
    /** Postconcats the matrix with the specified skew.
        M' = K(kx, ky) * M
    */
    void postSkew(float kx, float ky);
    /** Postconcats the matrix with the specified matrix.
        M' = other * M
    */
    void postConcat(const SMatrix& other);

    enum ScaleToFit {
        /**
         * Scale in X and Y independently, so that src matches dst exactly.
         * This may change the aspect ratio of the src.
         */
        kFill_ScaleToFit,
        /**
         * Compute a scale that will maintain the original src aspect ratio,
         * but will also ensure that src fits entirely inside dst. At least one
         * axis (X or Y) will fit exactly. kStart aligns the result to the
         * left and top edges of dst.
         */
        kStart_ScaleToFit,
        /**
         * Compute a scale that will maintain the original src aspect ratio,
         * but will also ensure that src fits entirely inside dst. At least one
         * axis (X or Y) will fit exactly. The result is centered inside dst.
         */
        kCenter_ScaleToFit,
        /**
         * Compute a scale that will maintain the original src aspect ratio,
         * but will also ensure that src fits entirely inside dst. At least one
         * axis (X or Y) will fit exactly. kEnd aligns the result to the
         * right and bottom edges of dst.
         */
        kEnd_ScaleToFit
    };

    /** Set the matrix to the scale and translate values that map the source
        rectangle to the destination rectangle, returning true if the the result
        can be represented.
        @param src the source rectangle to map from.
        @param dst the destination rectangle to map to.
        @param stf the ScaleToFit option
        @return true if the matrix can be represented by the rectangle mapping.
    */
    bool setRectToRect(const SRect& src, const SRect& dst, ScaleToFit stf);

    /** Set the matrix such that the specified src points would map to the
        specified dst points. count must be within [0..4].
        @param src  The array of src points
        @param dst  The array of dst points
        @param count The number of points to use for the transformation
        @return true if the matrix was set to the specified transformation
    */
    bool setPolyToPoly(const SPoint src[], const SPoint dst[], int count);

    /** If this matrix can be inverted, return true and if inverse is not null,
        set inverse to be the inverse of this matrix. If this matrix cannot be
        inverted, ignore inverse and return false
    */
    bool invert(SMatrix* inverse) const {
        // Allow the trivial case to be inlined.
        if (this->isIdentity()) {
            if (inverse) {
                inverse->reset();
            }
            return true;
        }
        return this->invertNonIdentity(inverse);
    }

    /** Fills the passed array with affine identity values
        in column major order.
        @param affine  The array to fill with affine identity values.
        Must not be NULL.
    */
    static void SetAffineIdentity(float affine[6]);

    /** Fills the passed array with the affine values in column major order.
        If the matrix is a perspective transform, returns false
        and does not change the passed array.
        @param affine  The array to fill with affine values. Ignored if NULL.
    */
    bool asAffine(float affine[6]) const;

    /** Apply this matrix to the array of points specified by src, and write
        the transformed points into the array of points specified by dst.
        dst[] = M * src[]
        @param dst  Where the transformed coordinates are written. It must
                    contain at least count entries
        @param src  The original coordinates that are to be transformed. It
                    must contain at least count entries
        @param count The number of points in src to read, and then transform
                     into dst.
    */
    void mapPoints(SPoint dst[], const SPoint src[], int count) const;

    /** Apply this matrix to the array of points, overwriting it with the
        transformed values.
        dst[] = M * pts[]
        @param pts  The points to be transformed. It must contain at least
                    count entries
        @param count The number of points in pts.
    */
    void mapPoints(SPoint pts[], int count) const {
        this->mapPoints(pts, pts, count);
    }

    /** Like mapPoints but with custom byte stride between the points. Stride
     *  should be a multiple of sizeof(float).
     */
    void mapPointsWithStride(SPoint pts[], size_t stride, int count) const {
        SASSERT(stride >= sizeof(SPoint));
        SASSERT(0 == stride % sizeof(float));
        for (int i = 0; i < count; ++i) {
            this->mapPoints(pts, pts, 1);
            pts = (SPoint*)((intptr_t)pts + stride);
        }
    }

    /** Like mapPoints but with custom byte stride between the points.
    */
    void mapPointsWithStride(SPoint dst[], SPoint src[],
                             size_t stride, int count) const {
        SASSERT(stride >= sizeof(SPoint));
        SASSERT(0 == stride % sizeof(float));
        for (int i = 0; i < count; ++i) {
            this->mapPoints(dst, src, 1);
            src = (SPoint*)((intptr_t)src + stride);
            dst = (SPoint*)((intptr_t)dst + stride);
        }
    }

    /** Apply this matrix to the array of homogeneous points, specified by src,
        where a homogeneous point is defined by 3 contiguous scalar values,
        and write the transformed points into the array of scalars specified by dst.
        dst[] = M * src[]
        @param dst  Where the transformed coordinates are written. It must
                    contain at least 3 * count entries
        @param src  The original coordinates that are to be transformed. It
                    must contain at least 3 * count entries
        @param count The number of triples (homogeneous points) in src to read,
                     and then transform into dst.
    */
    void mapHomogeneousPoints(float dst[], const float src[], int count) const;

    void mapXY(float x, float y, SPoint* result) const {
        SASSERT(result);
        this->getMapXYProc()(*this, x, y, result);
    }

    /** Apply this matrix to the array of vectors specified by src, and write
        the transformed vectors into the array of vectors specified by dst.
        This is similar to mapPoints, but ignores any translation in the matrix.
        @param dst  Where the transformed coordinates are written. It must
                    contain at least count entries
        @param src  The original coordinates that are to be transformed. It
                    must contain at least count entries
        @param count The number of vectors in src to read, and then transform
                     into dst.
    */
    void mapVectors(SVector2D dst[], const SVector2D src[], int count) const;

    /** Apply this matrix to the array of vectors specified by src, and write
        the transformed vectors into the array of vectors specified by dst.
        This is similar to mapPoints, but ignores any translation in the matrix.
        @param vecs The vectors to be transformed. It must contain at least
                    count entries
        @param count The number of vectors in vecs.
    */
    void mapVectors(SVector2D vecs[], int count) const {
        this->mapVectors(vecs, vecs, count);
    }

    /** Apply this matrix to the src rectangle, and write the transformed
        rectangle into dst. This is accomplished by transforming the 4 corners
        of src, and then setting dst to the bounds of those points.
        @param dst  Where the transformed rectangle is written.
        @param src  The original rectangle to be transformed.
        @return the result of calling rectStaysRect()
    */
    bool mapRect(SRect* dst, const SRect& src) const;

    /** Apply this matrix to the rectangle, and write the transformed rectangle
        back into it. This is accomplished by transforming the 4 corners of
        rect, and then setting it to the bounds of those points
        @param rect The rectangle to transform.
        @return the result of calling rectStaysRect()
    */
    bool mapRect(SRect* rect) const {
        return this->mapRect(rect, *rect);
    }

    /** Apply this matrix to the src rectangle, and write the four transformed
        points into dst. The points written to dst will be the original top-left, top-right,
        bottom-right, and bottom-left points transformed by the matrix.
        @param dst  Where the transformed quad is written.
        @param rect The original rectangle to be transformed.
    */
    void mapRectToQuad(SPoint dst[4], const SRect& rect) const {
        // This could potentially be faster if we only transformed each x and y of the rect once.
        rect.toQuad(dst);
        this->mapPoints(dst, 4);
    }

    /** Return the mean radius of a circle after it has been mapped by
        this matrix. NOTE: in perspective this value assumes the circle
        has its center at the origin.
    */
    float mapRadius(float radius) const;

    typedef void (*MapXYProc)(const SMatrix& mat, float x, float y,
                                 SPoint* result);

    static MapXYProc GetMapXYProc(TypeMask mask) {
        SASSERT((mask & ~kAllMasks) == 0);
        return gMapXYProcs[mask & kAllMasks];
    }

    MapXYProc getMapXYProc() const {
        return GetMapXYProc(this->getType());
    }

    typedef void (*MapPtsProc)(const SMatrix& mat, SPoint dst[],
                                  const SPoint src[], int count);

    static MapPtsProc GetMapPtsProc(TypeMask mask) {
        SASSERT((mask & ~kAllMasks) == 0);
        return gMapPtsProcs[mask & kAllMasks];
    }

    MapPtsProc getMapPtsProc() const {
        return GetMapPtsProc(this->getType());
    }

	/** Efficient comparison of two matrices. It distinguishes between zero and
     *  negative zero. It will return false when the sign of zero values is the
     *  only difference between the two matrices. It considers NaN values to be
     *  equal to themselves. So a matrix full of NaNs is "cheap equal" to
     *  another matrix full of NaNs iff the NaN values are bitwise identical
     *  while according to strict the strict == test a matrix with a NaN value
     *  is equal to nothing, including itself.
     */
    bool cheapEqualTo(const SMatrix& m) const {
        return 0 == memcmp(fMat, m.fMat, sizeof(fMat));
    }

    enum {
        // writeTo/readFromMemory will never return a value larger than this
        kMaxFlattenSize = 9 * sizeof(float) + sizeof(uint32_t)
    };

    /**
     * Calculates the minimum scaling factor of the matrix as computed from the SVD of the upper
     * left 2x2. If the matrix has perspective -1 is returned.
     *
     * @return minumum scale factor
     */
    float getMinScale() const;

    /**
     * Calculates the maximum scaling factor of the matrix as computed from the SVD of the upper
     * left 2x2. If the matrix has perspective -1 is returned.
     *
     * @return maximum scale factor
     */
    float getMaxScale() const;

    /**
     * Gets both the min and max scale factors. The min scale factor is scaleFactors[0] and the max
     * is scaleFactors[1]. If the matrix has perspective false will be returned and scaleFactors
     * will be unchanged.
     */
    bool getMinMaxScales(float scaleFactors[2]) const;

    /**
     *  Return a reference to a const identity matrix
     */
    static const SMatrix& I();

    /**
     *  Return a reference to a const matrix that is "invalid", one that could
     *  never be used.
     */
    static const SMatrix& InvalidMatrix();

    /**
     * Return the concatenation of two matrices, a * b.
     */
    static SMatrix Concat(const SMatrix& a, const SMatrix& b) {
        SMatrix result;
        result.setConcat(a, b);
        return result;
    }

    /**
     * Testing routine; the matrix's type cache should never need to be
     * manually invalidated during normal use.
     */
    void dirtyMatrixTypeCache() {
        this->setTypeMask(kUnknown_Mask);
    }

private:
    enum {
        /** Set if the matrix will map a rectangle to another rectangle. This
            can be true if the matrix is scale-only, or rotates a multiple of
            90 degrees.

            This bit will be set on identity matrices
        */
        kRectStaysRect_Mask = 0x10,

        /** Set if the perspective bit is valid even though the rest of
            the matrix is Unknown.
        */
        kOnlyPerspectiveValid_Mask = 0x40,

        kUnknown_Mask = 0x80,

        kORableMasks =  kTranslate_Mask |
                        kScale_Mask |
                        kAffine_Mask |
                        kPerspective_Mask,

        kAllMasks = kTranslate_Mask |
                    kScale_Mask |
                    kAffine_Mask |
                    kPerspective_Mask |
                    kRectStaysRect_Mask
    };

    float         fMat[9];
    mutable uint32_t fTypeMask;

    uint8_t computeTypeMask() const;
    uint8_t computePerspectiveTypeMask() const;

    void setTypeMask(int mask) {
        // allow kUnknown or a valid mask
        SASSERT(kUnknown_Mask == mask || (mask & kAllMasks) == mask ||
                 ((kUnknown_Mask | kOnlyPerspectiveValid_Mask) & mask)
                 == (kUnknown_Mask | kOnlyPerspectiveValid_Mask));
        fTypeMask = SkToU8(mask);
    }

    void orTypeMask(int mask) {
        SASSERT((mask & kORableMasks) == mask);
        fTypeMask = SkToU8(fTypeMask | mask);
    }

    void clearTypeMask(int mask) {
        // only allow a valid mask
        SASSERT((mask & kAllMasks) == mask);
        fTypeMask = fTypeMask & ~mask;
    }

    TypeMask getPerspectiveTypeMaskOnly() const {
        if ((fTypeMask & kUnknown_Mask) &&
            !(fTypeMask & kOnlyPerspectiveValid_Mask)) {
            fTypeMask = this->computePerspectiveTypeMask();
        }
        return (TypeMask)(fTypeMask & 0xF);
    }

    /** Returns true if we already know that the matrix is identity;
        false otherwise.
    */
    bool isTriviallyIdentity() const {
        if (fTypeMask & kUnknown_Mask) {
            return false;
        }
        return ((fTypeMask & 0xF) == 0);
    }

    bool  invertNonIdentity(SMatrix* inverse) const;

    static bool Poly2Proc(const SPoint[], SMatrix*, const SPoint& scale);
    static bool Poly3Proc(const SPoint[], SMatrix*, const SPoint& scale);
    static bool Poly4Proc(const SPoint[], SMatrix*, const SPoint& scale);

    static void Identity_xy(const SMatrix&, float, float, SPoint*);
    static void Trans_xy(const SMatrix&, float, float, SPoint*);
    static void Scale_xy(const SMatrix&, float, float, SPoint*);
    static void ScaleTrans_xy(const SMatrix&, float, float, SPoint*);
    static void Rot_xy(const SMatrix&, float, float, SPoint*);
    static void RotTrans_xy(const SMatrix&, float, float, SPoint*);
    static void Persp_xy(const SMatrix&, float, float, SPoint*);

    static const MapXYProc gMapXYProcs[];

    static void Identity_pts(const SMatrix&, SPoint[], const SPoint[], int);
    static void Trans_pts(const SMatrix&, SPoint dst[], const SPoint[], int);
    static void Scale_pts(const SMatrix&, SPoint dst[], const SPoint[], int);
    static void ScaleTrans_pts(const SMatrix&, SPoint dst[], const SPoint[],
                               int count);
    static void Rot_pts(const SMatrix&, SPoint dst[], const SPoint[], int);
    static void RotTrans_pts(const SMatrix&, SPoint dst[], const SPoint[],
                             int count);
    static void Persp_pts(const SMatrix&, SPoint dst[], const SPoint[], int);
    static const MapPtsProc gMapPtsProcs[];


    friend class SkPerspIter;
public:
	static float SkScalarSinCos(float radians, float* cosValue);
};


}//end of namespace SOUI

#endif
