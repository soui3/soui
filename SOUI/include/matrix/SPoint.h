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

struct SOUI_EXP SPoint {
    float    fX, fY;

    static SPoint Make(float x, float y) {
        SPoint pt;
        pt.set(x, y);
        return pt;
    }

	static SPoint IMake(int32_t x, int32_t y)
	{
		SPoint pt;
		pt.iset(x, y);
		return pt;
	}

	static SPoint IMake(const POINT &src)
	{
		SPoint pt;
		pt.iset(src.x, src.y);
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
    void iset(const POINT & p) {
        fX = SkIntToScalar(p.x);
        fY = SkIntToScalar(p.y);
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

	POINT toPoint() const
	{
		POINT pt = {SFloatRoundToInt(fX),SFloatRoundToInt(fY)};
		return pt;
	}
};

typedef SPoint SVector2D;
}//end of namespace SOUI

#endif
