
#include <souistd.h>
#include <animation/AlphaAnimation.h>

namespace SOUI {



	void SAlphaAnimation::copy(const IAnimation *src)
	{
		SAnimation::copy(src);
		const SAlphaAnimation *src2 = sobj_cast<const SAlphaAnimation>(src);
		mFromAlpha = src2->mFromAlpha;
		mToAlpha = src2->mToAlpha;
	}

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
