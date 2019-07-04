/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _SPoint_DEFINED_
#define _SPoint_DEFINED_

#include "Sfloat.h"

namespace SOUI{
/** \struct SkIPoint16

    SkIPoint holds two 16 bit integer coordinates
*/
struct SkIPoint16 {
    int16_t fX, fY;

    static SkIPoint16 Make(int x, int y) {
        SkIPoint16 pt;
        pt.set(x, y);
        return pt;
    }

    int16_t x() const { return fX; }
    int16_t y() const { return fY; }

    void set(int x, int y) {
        fX = SkToS16(x);
        fY = SkToS16(y);
    }
};

/** \struct SkIPoint

    SkIPoint holds two 32 bit integer coordinates
*/
struct SkIPoint {
    int32_t fX, fY;

    static SkIPoint Make(int32_t x, int32_t y) {
        SkIPoint pt;
        pt.set(x, y);
        return pt;
    }

    int32_t x() const { return fX; }
    int32_t y() const { return fY; }
    void setX(int32_t x) { fX = x; }
    void setY(int32_t y) { fY = y; }

    /**
     *  Returns true iff fX and fY are both zero.
     */
    bool isZero() const { return (fX | fY) == 0; }

    /**
     *  Set both fX and fY to zero. Same as set(0, 0)
     */
    void setZero() { fX = fY = 0; }

    /** Set the x and y values of the point. */
    void set(int32_t x, int32_t y) { fX = x; fY = y; }


    /** Return a new point whose X and Y coordinates are the negative of the
        original point's
    */
    SkIPoint operator-() const {
        SkIPoint neg;
        neg.fX = -fX;
        neg.fY = -fY;
        return neg;
    }

    /** Add v's coordinates to this point's */
    void operator+=(const SkIPoint& v) {
        fX += v.fX;
        fY += v.fY;
    }

    /** Subtract v's coordinates from this point's */
    void operator-=(const SkIPoint& v) {
        fX -= v.fX;
        fY -= v.fY;
    }

    /** Returns true if the point's coordinates equal (x,y) */
    bool equals(int32_t x, int32_t y) const {
        return fX == x && fY == y;
    }

    friend bool operator==(const SkIPoint& a, const SkIPoint& b) {
        return a.fX == b.fX && a.fY == b.fY;
    }

    friend bool operator!=(const SkIPoint& a, const SkIPoint& b) {
        return a.fX != b.fX || a.fY != b.fY;
    }

    /** Returns a new point whose coordinates are the difference between
        a and b (i.e. a - b)
    */
    friend SkIPoint operator-(const SkIPoint& a, const SkIPoint& b) {
        SkIPoint v;
        v.set(a.fX - b.fX, a.fY - b.fY);
        return v;
    }

    /** Returns a new point whose coordinates are the sum of a and b (a + b)
    */
    friend SkIPoint operator+(const SkIPoint& a, const SkIPoint& b) {
        SkIPoint v;
        v.set(a.fX + b.fX, a.fY + b.fY);
        return v;
    }
};

struct SOUI_EXP SPoint {
    float    fX, fY;

    static SPoint Make(float x, float y) {
        SPoint pt;
        pt.set(x, y);
        return pt;
    }

    float x() const { return fX; }
    float y() const { return fY; }

    /**
     *  Returns true iff fX and fY are both zero.
     */
    bool isZero() const { return (0 == fX) & (0 == fY); }

    /** Set the point's X and Y coordinates */
    void set(float x, float y) { fX = x; fY = y; }

    /** Set the point's X and Y coordinates by automatically promoting (x,y) to
        float values.
    */
    void iset(int32_t x, int32_t y) {
        fX = SkIntToScalar(x);
        fY = SkIntToScalar(y);
    }

    /** Set the point's X and Y coordinates by automatically promoting p's
        coordinates to float values.
    */
    void iset(const SkIPoint& p) {
        fX = SkIntToScalar(p.fX);
        fY = SkIntToScalar(p.fY);
    }

    /** Return the euclidian distance from (0,0) to the point
    */
    float length() const { return SPoint::Length(fX, fY); }
    /** Returns a new point whose coordinates are the negative of the point's
    */
    SPoint operator-() const {
        SPoint neg;
        neg.fX = -fX;
        neg.fY = -fY;
        return neg;
    }

    /** Add v's coordinates to the point's
    */
    void operator+=(const SPoint& v) {
        fX += v.fX;
        fY += v.fY;
    }

    /** Subtract v's coordinates from the point's
    */
    void operator-=(const SPoint& v) {
        fX -= v.fX;
        fY -= v.fY;
    }

    friend bool operator==(const SPoint& a, const SPoint& b) {
        return a.fX == b.fX && a.fY == b.fY;
    }

    friend bool operator!=(const SPoint& a, const SPoint& b) {
        return a.fX != b.fX || a.fY != b.fY;
    }

    /** Returns a new point whose coordinates are the difference between
        a's and b's (a - b)
    */
    friend SPoint operator-(const SPoint& a, const SPoint& b) {
        SPoint v;
        v.set(a.fX - b.fX, a.fY - b.fY);
        return v;
    }

    /** Returns a new point whose coordinates are the sum of a's and b's (a + b)
    */
    friend SPoint operator+(const SPoint& a, const SPoint& b) {
        SPoint v;
        v.set(a.fX + b.fX, a.fY + b.fY);
        return v;
    }
    /** Returns the euclidian distance from (0,0) to (x,y)
    */
    static float Length(float x, float y);

    /** Returns the dot product of a and b, treating them as 2D vectors
    */
    static float DotProduct(const SPoint& a, const SPoint& b) {
        return a.fX * b.fX + a.fY * b.fY;
    }
    float dot(const SPoint& vec) const {
        return DotProduct(*this, vec);
    }
};

typedef SPoint SVector2D;
}//end of namespace SOUI

#endif
