
#include <souistd.h>
#include <animation/TranslateAnimation.h>
namespace SOUI {



	void STranslateAnimation::copy(const IAnimation *src)
	{
		SAnimation::copy(src);
		const STranslateAnimation * src2 = sobj_cast<const STranslateAnimation>(src);
		mFromXType = src2->mFromXType;
		mFromXValue = src2->mFromXValue;
		mToXType = src2->mToXType;
		mToXValue = src2->mToXValue;
		mFromYType = src2->mFromYType;
		mFromYValue = src2->mFromYValue;
		mToYType = src2->mToYType;
		mToYValue = src2->mToYValue;
	}

	void STranslateAnimation::initialize(int width, int height, int parentWidth, int parentHeight)
	{
		mFromXDelta = (float)resolveSize(mFromXType, mFromXValue, width, parentWidth);
		mToXDelta = (float)resolveSize(mToXType, mToXValue, width, parentWidth);
		mFromYDelta = (float)resolveSize(mFromYType, mFromYValue, height, parentHeight);
		mToYDelta = (float)resolveSize(mToYType, mToYValue, height, parentHeight);
	}

	void STranslateAnimation::applyTransformation(float interpolatedTime, STransformation& t)
	{
		float dx = mFromXDelta;
		float dy = mFromYDelta;
		if (mFromXDelta != mToXDelta) {
			dx = mFromXDelta + ((mToXDelta - mFromXDelta) * interpolatedTime);
		}
		if (mFromYDelta != mToYDelta) {
			dy = mFromYDelta + ((mToYDelta - mFromYDelta) * interpolatedTime);
		}
		t.getMatrix().setTranslate(dx, dy);
		t.setTransformationType(STransformation::TYPE_MATRIX);
	}

	STranslateAnimation::STranslateAnimation(ValueType fromXType, float fromXValue, ValueType toXType, float toXValue, ValueType fromYType, float fromYValue, ValueType toYType, float toYValue)
	{
		mFromXValue = fromXValue;
		mToXValue = toXValue;
		mFromYValue = fromYValue;
		mToYValue = toYValue;

		mFromXType = fromXType;
		mToXType = toXType;
		mFromYType = fromYType;
		mToYType = toYType;
	}

	STranslateAnimation::STranslateAnimation(float fromXDelta/*=0.0f*/, float toXDelta/*=0.0f*/, float fromYDelta/*=0.0f*/, float toYDelta/*=0.0f*/)
	{
		mFromXValue = fromXDelta;
		mToXValue = toXDelta;
		mFromYValue = fromYDelta;
		mToYValue = toYDelta;

		mFromXType = ABSOLUTE_VALUE;
		mToXType = ABSOLUTE_VALUE;
		mFromYType = ABSOLUTE_VALUE;
		mToYType = ABSOLUTE_VALUE;
	}

}
