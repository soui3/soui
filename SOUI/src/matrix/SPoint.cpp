
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
    if (SkScalarIsFinite(mag2)) {
        return sk_float_sqrt(mag2);
    } else {
        double xx = dx;
        double yy = dy;
        return (float)sqrt(xx * xx + yy * yy);
    }
}
}//end of namespace SOUI
