
#include <souistd.h>
#include <animation/AlphaAnimation.h>

namespace SOUI {

	SAlphaAnimation::SAlphaAnimation() :mFromAlpha(0.f), mToAlpha(0.0f)
	{
	}

	void SAlphaAnimation::init(float fromAlpha, float toAlpha)
	{
		mFromAlpha = fromAlpha;
		mToAlpha = toAlpha;
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

	void SAlphaAnimation::copy(const IAnimation * src)
	{
		const SAlphaAnimation *src2 = sobj_cast<const SAlphaAnimation>(src);
		if (!src2) return;
		SAnimation::copy(src);
		mFromAlpha = src2->mFromAlpha;
		mToAlpha = src2->mToAlpha;
	}

}
