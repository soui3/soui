#include "souistd.h"
#include "animation\SInterpolatorImpl.h"
#include "layout/SLayoutSize.h" //for SLayoutSize::fequal
#pragma warning (push)
#pragma warning (disable: 4985) // disable the warning message during the include
#include <math.h>               // this is where I would normally get the warning message
#pragma warning (pop)

SNSBEGIN

static const float PI= 3.1415926f;

//////////////////////////////////////////////////////////////////////////
float SLinearInterpolator::getInterpolation(float input) const
{
	return input;
}


//////////////////////////////////////////////////////////////////////////
float SAccelerateInterpolator::getInterpolation(float input) const
{
	if (SLayoutSize::fequal(mFactor , 1.0f)) {
		return input * input;
	} else {
		return (float)pow((double)input, mDoubleFactor);
	}
}

SAccelerateInterpolator::SAccelerateInterpolator(float factor):mFactor(factor),mDoubleFactor(2*factor)
{
}



//////////////////////////////////////////////////////////////////////////

float SDecelerateInterpolator::getInterpolation(float input) const
{
	float result;
	if (SLayoutSize::fequal(mFactor , 1.0f)) {
		result = (float)(1.0f - (1.0f - input) * (1.0f - input));
	} else {
		result = (float)(1.0f - pow((1.0f - input), 2 * mFactor));
	}
	return result;
}

SDecelerateInterpolator::SDecelerateInterpolator(float factor):mFactor(factor)
{
}

//////////////////////////////////////////////////////////////////////////
float SAccelerateDecelerateInterpolator::getInterpolation(float input) const
{
	return (float)(cos((input + 1) * PI) / 2.0f) + 0.5f;
}


//////////////////////////////////////////////////////////////////////////
float SAnticipateInterpolator::getInterpolation(float t) const
{
	// a(t) = t * t * ((tension + 1) * t - tension)
	return t * t * ((mTension + 1) * t - mTension);
}

SAnticipateInterpolator::SAnticipateInterpolator(float tension):mTension(tension)
{
}

SAnticipateInterpolator::SAnticipateInterpolator():mTension(2.0f)
{
}


//////////////////////////////////////////////////////////////////////////
float SAnticipateOvershootInterpolator::getInterpolation(float t) const
{
	// a(t, s) = t * t * ((s + 1) * t - s)
	// o(t, s) = t * t * ((s + 1) * t + s)
	// f(t) = 0.5 * a(t * 2, tension * extraTension), when t < 0.5
	// f(t) = 0.5 * (o(t * 2 - 2, tension * extraTension) + 2), when t <= 1.0
	if (t < 0.5f) return 0.5f * a(t * 2.0f, getTensionMulti());
	else return 0.5f * (o(t * 2.0f - 2.0f, getTensionMulti()) + 2.0f);
}

float SAnticipateOvershootInterpolator::o(float t, float s)
{
	return t * t * ((s + 1) * t + s);
}

float SAnticipateOvershootInterpolator::a(float t, float s)
{
	return t * t * ((s + 1) * t - s);
}

SAnticipateOvershootInterpolator::SAnticipateOvershootInterpolator(float tension, float extraTension):
mTension(tension),mExtraTension(extraTension)
{
}


//////////////////////////////////////////////////////////////////////////
float SBounceInterpolator::getInterpolation(float t) const
{
	// _b(t) = t * t * 8
	// bs(t) = _b(t) for t < 0.3535
	// bs(t) = _b(t - 0.54719) + 0.7 for t < 0.7408
	// bs(t) = _b(t - 0.8526) + 0.9 for t < 0.9644
	// bs(t) = _b(t - 1.0435) + 0.95 for t <= 1.0
	// b(t) = bs(t * 1.1226)
	t *= 1.1226f;
	if (t < 0.3535f) return bounce(t);
	else if (t < 0.7408f) return bounce(t - 0.54719f) + 0.7f;
	else if (t < 0.9644f) return bounce(t - 0.8526f) + 0.9f;
	else return bounce(t - 1.0435f) + 0.95f;
}

float SBounceInterpolator::bounce(float t)
{
	return t * t * 8.0f;
}


//////////////////////////////////////////////////////////////////////////
float SCycleInterpolator::getInterpolation(float input) const
{
	return (float)(sin(2 * mCycles * PI * input));
}

SCycleInterpolator::SCycleInterpolator(float cycles):mCycles(cycles)
{
}

//////////////////////////////////////////////////////////////////////////

float SOvershootInterpolator::getInterpolation(float t) const
{
	// _o(t) = t * t * ((tension + 1) * t + tension)
	// o(t) = _o(t - 1) + 1
	t -= 1.0f;
	return t * t * ((mTension + 1) * t + mTension) + 1.0f;
}

SOvershootInterpolator::SOvershootInterpolator(float tension):mTension(tension)
{
}


///////////////////////////////////////////////////////////////////////////////////
// This governs how accurate the approximation of the Path is.
SPathInterpolator::SPathInterpolator() {
}

SPathInterpolator::SPathInterpolator(float controlX, float controlY) {
	initQuad(controlX, controlY);
}

SPathInterpolator::SPathInterpolator(float controlX1, float controlY1, float controlX2, float controlY2) {
	initCubic(controlX1, controlY1, controlX2, controlY2);
}

void SPathInterpolator::initQuad(float controlX, float controlY) {
	SAutoRefPtr<IPath> path;
	GETRENDERFACTORY->CreatePath(&path);
	if (path)
	{
		path->moveTo(0, 0);
		path->quadTo(controlX, controlY, 1.f, 1.f);
		initPath(path);
	}
}

void SPathInterpolator::initCubic(float x1, float y1, float x2, float y2) {
	SAutoRefPtr<IPath> path;
	GETRENDERFACTORY->CreatePath(&path);
	if (path)
	{
		path->moveTo(0, 0);
		path->cubicTo(x1, y1, x2, y2, 1.f, 1.f);
		initPath(path);
	}
}

static const float PRECISION = 0.002f;

void SPathInterpolator::initPath(IPath * path) {
	IPathInfo * pathInfo = path->approximate(PRECISION);
	int numPoints = pathInfo->pointNumber();
	const float * pointComponents = pathInfo->data();

	int nLen = numPoints*3;
	if (pointComponents[1] != 0 || pointComponents[2] != 0
		|| pointComponents[nLen - 2] != 1
		|| pointComponents[nLen - 1] != 1) {
			goto error;
	}

	mPt.SetCount(numPoints);
	float prevX = 0;
	float prevFraction = 0;
	int componentIndex = 0;
	for (int i = 0; i < numPoints; i++) {
		float fraction = pointComponents[componentIndex++];
		float x = pointComponents[componentIndex++];
		float y = pointComponents[componentIndex++];
		if (fraction == prevFraction && x != prevX) {
			goto error;
		}
		if (x < prevX) {
			goto error;
		}
		mPt[i].fX = x;
		mPt[i].fY = y;
		prevX = x;
		prevFraction = fraction;
	}
error:
	pathInfo->Release();
	mPt.RemoveAll();
}

float SPathInterpolator::getInterpolation(float t) const{
	if (t <= 0) {
		return 0;
	}
	else if (t >= 1) {
		return 1;
	}
	// Do a binary search for the correct x to interpolate between.
	int startIndex = 0;
	int endIndex = mPt.GetCount() - 1;

	while (endIndex - startIndex > 1) {
		int midIndex = (startIndex + endIndex) / 2;
		if (t < mPt[midIndex].fX) {
			endIndex = midIndex;
		}
		else {
			startIndex = midIndex;
		}
	}

	float xRange = mPt[endIndex].fX - mPt[startIndex].fX;
	if (xRange == 0) {
		return mPt[startIndex].fX;
	}

	float tInRange = t - mPt[startIndex].fX;
	float fraction = tInRange / xRange;

	float startY = mPt[startIndex].fY;
	float endY = mPt[endIndex].fY;
	return startY + (fraction * (endY - startY));
	return 0;
}

void SPathInterpolator::OnInitFinished(IXmlNode * pNode)
{
	pugi::xml_node xmlNode(pNode);
	fPoint pts[2];
	bool hasCtrl1 = false, hasCtrl2 = false;
	if (xmlNode.attribute(L"controlX1") && xmlNode.attribute(L"controlY1"))
	{
		hasCtrl1 = true;
		pts[0].fX = xmlNode.attribute(L"controlX1").as_float();
		pts[0].fY = xmlNode.attribute(L"controlY1").as_float();
	}
	if (xmlNode.attribute(L"controlX2") && xmlNode.attribute(L"controlY2"))
	{
		hasCtrl2 = true;
		pts[1].fX = xmlNode.attribute(L"controlX2").as_float();
		pts[1].fY = xmlNode.attribute(L"controlY2").as_float();
	}
	if (hasCtrl1 && hasCtrl2)
	{
		initCubic(pts[0].fX, pts[0].fY, pts[1].fX, pts[1].fY);
	}
	else if (hasCtrl1)
	{
		initQuad(pts[0].fX, pts[0].fY);
	}
}

SNSEND