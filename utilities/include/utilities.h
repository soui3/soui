#pragma once

#include "utilities-def.h"
#include <windows.h>

SNSBEGIN

int UTILITIES_API RectWidth(LPCRECT rc) ;
int UTILITIES_API RectHeight(LPCRECT rc) ;

void UTILITIES_API SHiMetricToPixel(const SIZEL * lpSizeInHiMetric, LPSIZEL lpSizeInPix);
void UTILITIES_API SPixelToHiMetric(const SIZEL * lpSizeInPix, LPSIZEL lpSizeInHiMetric);

SNSEND