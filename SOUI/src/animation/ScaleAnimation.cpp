
#include <souistd.h>
#include <animation/ScaleAnimation.h>

namespace SOUI {



	void SScaleAnimation::initialize(int width, int height, int parentWidth, int parentHeight)
	{
		mPivotX = (float)resolveSize(mPivotXType, mPivotXValue, width, parentWidth);
		mPivotY = (float)resolveSize(mPivotYType, mPivotYValue, height, parentHeight);
	}

	void SScaleAnimation::applyTransformation(float interpolatedTime, STransformation & t)
	{
		float sx = 1.0f;
		float sy = 1.0f;
		float scale = getScaleFactor();

		if (mFromX != 1.0f || mToX != 1.0f) {
			sx = mFromX + ((mToX - mFromX) * interpolatedTime);
		}
		if (mFromY != 1.0f || mToY != 1.0f) {
			sy = mFromY + ((mToY - mFromY) * interpolatedTime);
		}

		if (mPivotX == 0 && mPivotY == 0) {
			t.getMatrix().setScale(sx, sy);
		}
		else {
			t.getMatrix().setScale(sx, sy, scale * mPivotX, scale * mPivotY);
		}
		t.setTransformationType(STransformation::TYPE_MATRIX);
	}

	void SScaleAnimation::initializePivotPoint()
	{
		if (mPivotXType == ABSOLUTE_VALUE) {
			mPivotX = mPivotXValue;
		}
		if (mPivotYType == ABSOLUTE_VALUE) {
			mPivotY = mPivotYValue;
		}
	}

	void SScaleAnimation::init(float fromX, float toX, float fromY, float toY, ValueType pivotXType, float pivotXValue, ValueType pivotYType, float pivotYValue)
	{
		mFromX = fromX;
		mToX = toX;
		mFromY = fromY;
		mToY = toY;

		mPivotXValue = pivotXValue;
		mPivotXType = pivotXType;
		mPivotYValue = pivotYValue;
		mPivotYType = pivotYType;
		initializePivotPoint();
	}

	void SScaleAnimation::init(float fromX, float toX, float fromY, float toY, float pivotX, float pivotY)
	{
		mFromX = fromX;
		mToX = toX;
		mFromY = fromY;
		mToY = toY;

		mPivotXType = ABSOLUTE_VALUE;
		mPivotYType = ABSOLUTE_VALUE;
		mPivotXValue = pivotX;
		mPivotYValue = pivotY;
		initializePivotPoint();
	}

	SScaleAnimation::SScaleAnimation()
	{
		init();
	}

	void SScaleAnimation::init(float fromX/*=0.0f*/, float toX /*= 0.0f*/, float fromY /*= 0.0f*/, float toY /*= 0.0f*/)
	{
		mFromX = fromX;
		mToX = toX;
		mFromY = fromY;
		mToY = toY;
		mPivotX = 0;
		mPivotY = 0;
	}

}
