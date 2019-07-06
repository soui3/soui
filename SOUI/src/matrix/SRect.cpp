
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <souistd.h>
#include "matrix/SRect.h"

namespace SOUI{

void SRect::sort() {
    if (fLeft > fRight) {
        STSwap<float>(fLeft, fRight);
    }
    if (fTop > fBottom) {
        STSwap<float>(fTop, fBottom);
    }
}

void SRect::toQuad(SPoint quad[4]) const {
    SASSERT(quad);

    quad[0].set(fLeft, fTop);
    quad[1].set(fRight, fTop);
    quad[2].set(fRight, fBottom);
    quad[3].set(fLeft, fBottom);
}

bool SRect::setBoundsCheck(const SPoint pts[], int count) {
    SASSERT((pts && count > 0) || count == 0);

    bool isFinite = true;

    if (count <= 0) {
		memset(this, 0, sizeof(SRect));
    } else {
        float    l, t, r, b;

        l = r = pts[0].fX;
        t = b = pts[0].fY;

        // If all of the points are finite, accum should stay 0. If we encounter
        // a NaN or infinity, then accum should become NaN.
        float accum = 0;
        accum *= l; accum *= t;

        for (int i = 1; i < count; i++) {
            float x = pts[i].fX;
            float y = pts[i].fY;

            accum *= x; accum *= y;

            // we use if instead of if/else, so we can generate min/max
            // float instructions (at least on SSE)
            if (x < l) l = x;
            if (x > r) r = x;

            if (y < t) t = y;
            if (y > b) b = y;
        }

        SASSERT(!accum || !SFloatIsFinite(accum));
        if (accum) {
            l = t = r = b = 0;
            isFinite = false;
        }
        this->set(l, t, r, b);
    }

    return isFinite;
}


// http://www.blackpawn.com/texts/pointinpoly/default.html
// return true if pt is inside triangle; false if outside or on the line
bool STriangle::contains(const SPoint& pt) const {
	// Compute vectors
	SPoint v0 = fPts[2] - fPts[0];
	SPoint v1 = fPts[1] - fPts[0];
	SPoint v2 = pt - fPts[0];

	// Compute dot products
	double dot00 = v0.dot(v0);
	double dot01 = v0.dot(v1);
	double dot02 = v0.dot(v2);
	double dot11 = v1.dot(v1);
	double dot12 = v1.dot(v2);

	double w = dot00 * dot11 - dot01 * dot01;
	if (w == 0) {
		return false;
	}
	double wSign = w < 0 ? -1 : 1;
	double u = (dot11 * dot02 - dot01 * dot12) * wSign;
	if (u <= 0) {
		return false;
	}
	double v = (dot00 * dot12 - dot01 * dot02) * wSign;
	if (v <= 0) {
		return false;
	}
	return u + v < w * wSign;
}

bool SQuad::contains(const SPoint & pt) const
{
	bool bRet = false;
	if(!bRet)
	{
		STriangle tri;
		tri.fPts[0] = fPts[0];
		tri.fPts[1] = fPts[1];
		tri.fPts[2] = fPts[2];
		bRet = tri.contains(pt);
	}
	if (!bRet)
	{
		STriangle tri;
		tri.fPts[0] = fPts[0];
		tri.fPts[1] = fPts[2];
		tri.fPts[2] = fPts[3];
		bRet = tri.contains(pt);
	}
	return bRet;
}

}//end of namespace SOUI
