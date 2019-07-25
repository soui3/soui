
#include <souistd.h>
#include <animation/AlphaAnimation.h>

namespace SOUI {

	bool SAlphaAnimation::hasAlpha() const 
	{
		return true;
	}

	void SAlphaAnimation::applyTransformation(float interpolatedTime, STransformation & t)
	{
		float alpha = mFromAlpha;
		t.setAlpha((BYTE)((alpha + ((mToAlpha - alpha) * interpolatedTime))*255));
	}

	SAlphaAnimation::SAlphaAnimation(float fromAlpha/*=0.0f*/, float toAlpha/*=0.0f*/)
	{
		mFromAlpha = fromAlpha;
		mToAlpha = toAlpha;
	}

}
