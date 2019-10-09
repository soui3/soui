
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef _SRect_DEFINED_
#define _SRect_DEFINED_

#include "SPoint.h"

namespace SOUI{

template <typename T> inline void STSwap(T& a, T& b) {
	T c(a);
	a = b;
	b = c;
}

/** \struct SkRect
*/
struct SOUI_EXP SRect : public fRect {
	static SRect IMake(const RECT & src)
	{
		SRect rc;
		rc.iset(src);
		return rc;
	}


    /**
     *  Return true if the rectangle's width or height are <= 0
     */
    bool isEmpty() const { return fLeft >= fRight || fTop >= fBottom; }

    bool isLargest() const { return SK_ScalarMin == fLeft &&
                                    SK_ScalarMin == fTop &&
                                    SK_ScalarMax == fRight &&
                                    SK_ScalarMax == fBottom; }

    /**
     *  Returns true iff all values in the rect are finite. If any are
     *  infinite or NaN (or SK_FixedNaN when float is fixed) then this
     *  returns false.
     */
    bool isFinite() const {
        float accum = 0;
        accum *= fLeft;
        accum *= fTop;
        accum *= fRight;
        accum *= fBottom;

        // accum is either NaN or it is finite (zero).
        SASSERT(0 == accum || !(accum == accum));

        // value==value will be true iff value is not NaN
        // TODO: is it faster to say !accum or accum==accum?
        return accum == accum;
    }

    float    x() const { return fLeft; }
    float    y() const { return fTop; }
    float    left() const { return fLeft; }
    float    top() const { return fTop; }
    float    right() const { return fRight; }
    float    bottom() const { return fBottom; }
    float    width() const { return fRight - fLeft; }
    float    height() const { return fBottom - fTop; }
    float    centerX() const { return SFloatHalf(fLeft + fRight); }
    float    centerY() const { return SFloatHalf(fTop + fBottom); }

    friend bool operator==(const SRect& a, const SRect& b) {
        return SFloatsEqual((float*)&a, (float*)&b, 4);
    }

    friend bool operator!=(const SRect& a, const SRect& b) {
        return !SFloatsEqual((float*)&a, (float*)&b, 4);
    }

    /** return the 4 points that enclose the rectangle (top-left, top-right, bottom-right,
        bottom-left). TODO: Consider adding param to control whether quad is CW or CCW.
     */
    void toQuad(SPoint quad[4]) const;

    /** Set this rectangle to the empty rectangle (0,0,0,0)
    */
    void setEmpty() { memset(this, 0, sizeof(*this)); }

	void iset(const RECT & src)
	{
		fLeft = SkIntToScalar(src.left);
		fTop = SkIntToScalar(src.top);
		fRight = SkIntToScalar(src.right);
		fBottom = SkIntToScalar(src.bottom);
	}

	RECT toRect() const
	{
		RECT rc;
		rc.left = sk_float_floor2int(fLeft);
		rc.top = sk_float_floor2int(fTop);
		rc.right = sk_float_ceil2int(fRight);
		rc.bottom = sk_float_ceil2int(fBottom);
		return rc;
	}

    void set(float left, float top, float right, float bottom) {
        fLeft   = left;
        fTop    = top;
        fRight  = right;
        fBottom = bottom;
    }

    /** Initialize the rect with the 4 specified integers. The routine handles
        converting them to scalars (by calling SkIntToScalar)
     */
    void iset(int left, int top, int right, int bottom) {
        fLeft   = SkIntToScalar(left);
        fTop    = SkIntToScalar(top);
        fRight  = SkIntToScalar(right);
        fBottom = SkIntToScalar(bottom);
    }

    /** Set this rectangle to be the bounds of the array of points.
        If the array is empty (count == 0), then set this rectangle
        to the empty rectangle (0,0,0,0)
    */
    void set(const SPoint pts[], int count) {
        // set() had been checking for non-finite values, so keep that behavior
        // for now. Now that we have setBoundsCheck(), we may decide to make
        // set() be simpler/faster, and not check for those.
        (void)this->setBoundsCheck(pts, count);
    }

    // alias for set(pts, count)
    void setBounds(const SPoint pts[], int count) {
        (void)this->setBoundsCheck(pts, count);
    }

    /**
     *  Compute the bounds of the array of points, and set this rect to that
     *  bounds and return true... unless a non-finite value is encountered,
     *  in which case this rect is set to empty and false is returned.
     */
    bool setBoundsCheck(const SPoint pts[], int count);

    void set(const SPoint& p0, const SPoint& p1) {
        fLeft =   SkMinScalar(p0.fX, p1.fX);
        fRight =  SkMaxScalar(p0.fX, p1.fX);
        fTop =    SkMinScalar(p0.fY, p1.fY);
        fBottom = SkMaxScalar(p0.fY, p1.fY);
    }

    /** Offset set the rectangle by adding dx to its left and right,
        and adding dy to its top and bottom.
    */
    void offset(float dx, float dy) {
        fLeft   += dx;
        fTop    += dy;
        fRight  += dx;
        fBottom += dy;
    }

    void offset(const SPoint& delta) {
        this->offset(delta.fX, delta.fY);
    }

    /**
     *  Offset this rect such its new x() and y() will equal newX and newY.
     */
    void offsetTo(float newX, float newY) {
        fRight += newX - fLeft;
        fBottom += newY - fTop;
        fLeft = newX;
        fTop = newY;
    }

    /** Inset the rectangle by (dx,dy). If dx is positive, then the sides are
        moved inwards, making the rectangle narrower. If dx is negative, then
        the sides are moved outwards, making the rectangle wider. The same holds
         true for dy and the top and bottom.
    */
    void inset(float dx, float dy)  {
        fLeft   += dx;
        fTop    += dy;
        fRight  -= dx;
        fBottom -= dy;
    }

   /** Outset the rectangle by (dx,dy). If dx is positive, then the sides are
       moved outwards, making the rectangle wider. If dx is negative, then the
       sides are moved inwards, making the rectangle narrower. The same holds
       true for dy and the top and bottom.
    */
    void outset(float dx, float dy)  { this->inset(-dx, -dy); }

    /**
     *  Swap top/bottom or left/right if there are flipped (i.e. if width()
     *  or height() would have returned a negative value.) This should be called
     *  if the edges are computed separately, and may have crossed over each
     *  other. When this returns, left <= right && top <= bottom
     */
    void sort();

    /**
     *  cast-safe way to treat the rect as an array of (4) SFloats.
     */
    const float* asScalars() const { return &fLeft; }


};

struct SOUI_EXP STriangle
{
	SPoint fPts[3];
	bool contains(const SPoint & pt) const;
};

struct SOUI_EXP SQuad
{
	SPoint fPts[4];
	bool contains(const SPoint & pt) const;
};

}//end of namespace SOUI

#endif
