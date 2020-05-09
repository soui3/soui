
#include <souistd.h>
#include <animation/SRotateAnimation.h>

SNSBEGIN
SRotateAnimation::SRotateAnimation()
{
	mFromDegrees = 0.0f;
	mToDegrees = 0.0f;
	mPivotX = 0.0f;
	mPivotY = 0.0f;
}


void SRotateAnimation::initialize(int width, int height, int parentWidth, int parentHeight) 
{
	mPivotX = (float)resolveSize(mPivotXType, mPivotXValue, width, parentWidth);
	mPivotY = (float)resolveSize(mPivotYType, mPivotYValue, height, parentHeight);
}

void SRotateAnimation::applyTransformation(float interpolatedTime, STransformation & t)
{
	float degrees = mFromDegrees + ((mToDegrees - mFromDegrees) * interpolatedTime);
	float scale = getScaleFactor();

	if (mPivotX == 0.0f && mPivotY == 0.0f) {
		t.getMatrix().setRotate(degrees);
	}
	else {
		t.getMatrix().setRotate(degrees, mPivotX * scale, mPivotY * scale);
	}
	t.setTransformationType(STransformation::TYPE_MATRIX);
}

void SRotateAnimation::copy(const IAnimation * src)
{
	const SRotateAnimation *src2 = sobj_cast<const SRotateAnimation>(src);
	if (!src2) return;
	SAnimation::copy(src);
	mFromDegrees = src2->mFromDegrees;
	mToDegrees = src2->mToDegrees;
	mPivotXType = src2->mPivotXType;
	mPivotYType = src2->mPivotYType;
	mPivotXValue = src2->mPivotXValue;
	mPivotYValue = src2->mPivotYValue;
	initializePivotPoint();
}

void SRotateAnimation::initializePivotPoint()
{
	if (mPivotXType == ABSOLUTE_VALUE) {
		mPivotX = mPivotXValue;
	}
	if (mPivotYType == ABSOLUTE_VALUE) {
		mPivotY = mPivotYValue;
	}
}

void SRotateAnimation::init(float fromDegrees, float toDegrees, ValueType pivotXType, float pivotXValue, ValueType pivotYType, float pivotYValue) 
{
	mFromDegrees = fromDegrees;
	mToDegrees = toDegrees;

	mPivotXValue = pivotXValue;
	mPivotXType = pivotXType;
	mPivotYValue = pivotYValue;
	mPivotYType = pivotYType;
	initializePivotPoint();
}

void SRotateAnimation::init(float fromDegrees, float toDegrees, float pivotX, float pivotY)
{
	mFromDegrees = fromDegrees;
	mToDegrees = toDegrees;

	mPivotXType = ABSOLUTE_VALUE;
	mPivotYType = ABSOLUTE_VALUE;
	mPivotXValue = pivotX;
	mPivotYValue = pivotY;
	initializePivotPoint();
}

SNSEND