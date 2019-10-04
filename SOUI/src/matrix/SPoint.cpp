
/*
 * Copyright 2008 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include <souistd.h>
#include "matrix/SPoint.h"

namespace SOUI{


float SPoint::Length(float dx, float dy) {
    float mag2 = dx * dx + dy * dy;
    if (SFloatIsFinite(mag2)) {
        return sk_float_sqrt(mag2);
    } else {
        double xx = dx;
        double yy = dy;
        return (float)sqrt(xx * xx + yy * yy);
    }
}

void SPoint::iset(int32_t x, int32_t y)
{
	fX = SkIntToScalar(x);
	fY = SkIntToScalar(y);
}

void SPoint::iset(const POINT & p)
{
	fX = SkIntToScalar(p.x);
	fY = SkIntToScalar(p.y);
}

POINT SPoint::toPoint() const
{
	POINT pt = {SFloatRoundToInt(fX),SFloatRoundToInt(fY)};
	return pt;
}

float SPoint::dot(const SPoint& vec) const
{
	return DotProduct(*this, vec);
}

float SPoint::DotProduct(const SPoint& a, const SPoint& b)
{
	return a.fX * b.fX + a.fY * b.fY;
}

}//end of namespace SOUI
