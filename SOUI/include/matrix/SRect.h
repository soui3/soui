
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
/** \struct SIRect

    SkIRect holds four 32 bit integer coordinates for a rectangle
*/
struct SOUI_EXP SIRect {
    int32_t fLeft, fTop, fRight, fBottom;

    int left() const { return fLeft; }
    int top() const { return fTop; }
    int right() const { return fRight; }
    int bottom() const { return fBottom; }

    /** return the left edge of the rect */
    int x() const { return fLeft; }
    /** return the top edge of the rect */
    int y() const { return fTop; }
    /**
     *  Returns the rectangle's width. This does not check for a valid rect
     *  (i.e. left <= right) so the result may be negative.
     */
    int width() const { return fRight - fLeft; }

    /**
     *  Returns the rectangle's height. This does not check for a valid rect
     *  (i.e. top <= bottom) so the result may be negative.
     */
    int height() const { return fBottom - fTop; }

    /**
     *  Since the center of an integer rect may fall on a factional value, this
     *  method is defined to return (right + left) >> 1.
     *
     *  This is a specific "truncation" of the average, which is different than
     *  (right + left) / 2 when the sum is negative.
     */
    int centerX() const { return (fRight + fLeft) >> 1; }

    /**
     *  Since the center of an integer rect may fall on a factional value, this
     *  method is defined to return (bottom + top) >> 1
     *
     *  This is a specific "truncation" of the average, which is different than
     *  (bottom + top) / 2 when the sum is negative.
     */
    int centerY() const { return (fBottom + fTop) >> 1; }

    /**
     *  Return true if the rectangle's width or height are <= 0
     */
    bool isEmpty() const { return fLeft >= fRight || fTop >= fBottom; }

    bool isLargest() const { return SK_MinS32 == fLeft &&
                                    SK_MinS32 == fTop &&
                                    SK_MaxS32 == fRight &&
                                    SK_MaxS32 == fBottom; }

    friend bool operator==(const SIRect& a, const SIRect& b) {
        return !memcmp(&a, &b, sizeof(a));
    }

    friend bool operator!=(const SIRect& a, const SIRect& b) {
        return !(a == b);
    }

    bool is16Bit() const {
        return  SkIsS16(fLeft) && SkIsS16(fTop) &&
                SkIsS16(fRight) && SkIsS16(fBottom);
    }

    /** Set the rectangle to (0,0,0,0)
    */
    void setEmpty() { memset(this, 0, sizeof(*this)); }

    void set(int32_t left, int32_t top, int32_t right, int32_t bottom) {
        fLeft   = left;
        fTop    = top;
        fRight  = right;
        fBottom = bottom;
    }
    // alias for set(l, t, r, b)
    void setLTRB(int32_t left, int32_t top, int32_t right, int32_t bottom) {
        this->set(left, top, right, bottom);
    }

    void setXYWH(int32_t x, int32_t y, int32_t width, int32_t height) {
        fLeft = x;
        fTop = y;
        fRight = x + width;
        fBottom = y + height;
    }

    /**
     *  Make the largest representable rectangle
     */
    void setLargest() {
        fLeft = fTop = SK_MinS32;
        fRight = fBottom = SK_MaxS32;
    }

    /**
     *  Make the largest representable rectangle, but inverted (e.g. fLeft will
     *  be max 32bit and right will be min 32bit).
     */
    void setLargestInverted() {
        fLeft = fTop = SK_MaxS32;
        fRight = fBottom = SK_MinS32;
    }


    /** Offset set the rectangle by adding dx to its left and right,
        and adding dy to its top and bottom.
    */
    void offset(int32_t dx, int32_t dy) {
        fLeft   += dx;
        fTop    += dy;
        fRight  += dx;
        fBottom += dy;
    }

    void offset(const SkIPoint& delta) {
        this->offset(delta.fX, delta.fY);
    }

    /**
     *  Offset this rect such its new x() and y() will equal newX and newY.
     */
    void offsetTo(int32_t newX, int32_t newY) {
        fRight += newX - fLeft;
        fBottom += newY - fTop;
        fLeft = newX;
        fTop = newY;
    }

    /** Inset the rectangle by (dx,dy). If dx is positive, then the sides are moved inwards,
        making the rectangle narrower. If dx is negative, then the sides are moved outwards,
        making the rectangle wider. The same holds true for dy and the top and bottom.
    */
    void inset(int32_t dx, int32_t dy) {
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
    void outset(int32_t dx, int32_t dy)  { this->inset(-dx, -dy); }

    /** Swap top/bottom or left/right if there are flipped.
        This can be called if the edges are computed separately,
        and may have crossed over each other.
        When this returns, left <= right && top <= bottom
    */
    void sort();

    static const SIRect&  EmptyIRect() {
        static const SIRect gEmpty = { 0, 0, 0, 0 };
        return gEmpty;
    }
};

/** \struct SkRect
*/
struct SOUI_EXP SRect {
    float    fLeft, fTop, fRight, fBottom;

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
    float    centerX() const { return SkScalarHalf(fLeft + fRight); }
    float    centerY() const { return SkScalarHalf(fTop + fBottom); }

    friend bool operator==(const SRect& a, const SRect& b) {
        return SkScalarsEqual((float*)&a, (float*)&b, 4);
    }

    friend bool operator!=(const SRect& a, const SRect& b) {
        return !SkScalarsEqual((float*)&a, (float*)&b, 4);
    }

    /** return the 4 points that enclose the rectangle (top-left, top-right, bottom-right,
        bottom-left). TODO: Consider adding param to control whether quad is CW or CCW.
     */
    void toQuad(SPoint quad[4]) const;

    /** Set this rectangle to the empty rectangle (0,0,0,0)
    */
    void setEmpty() { memset(this, 0, sizeof(*this)); }

    void set(const SIRect& src) {
        fLeft   = SkIntToScalar(src.fLeft);
        fTop    = SkIntToScalar(src.fTop);
        fRight  = SkIntToScalar(src.fRight);
        fBottom = SkIntToScalar(src.fBottom);
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
     *  cast-safe way to treat the rect as an array of (4) SkScalars.
     */
    const float* asScalars() const { return &fLeft; }


};

}//end of namespace SOUI

#endif
