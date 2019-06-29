
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

        SASSERT(!accum || !SkScalarIsFinite(accum));
        if (accum) {
            l = t = r = b = 0;
            isFinite = false;
        }
        this->set(l, t, r, b);
    }

    return isFinite;
}

}//end of namespace SOUI
