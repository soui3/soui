
#include <souistd.h>
#include <animation/AnimationSet.h>
#include <SApp.h>
#include <helper/stime.h>

namespace SOUI {



	BOOL SAnimationSet::InitFromXml(pugi::xml_node xmlNode)
	{
		SAnimation::InitFromXml(xmlNode);
		pugi::xml_node xmlChild = xmlNode.first_child();
		while (xmlChild)
		{
			IAnimation * ani = SApplication::getSingletonPtr()->CreateAnimationByName(xmlChild.name());
			if (ani)
			{
				ani->InitFromXml(xmlChild);
				addAnimation(ani);
				ani->Release();
			}
			xmlChild = xmlChild.next_sibling();
		}
		return TRUE;
	}

	HRESULT SAnimationSet::OnAttrDuration(const SStringW & value, BOOL bLoading)
	{
		long iValue = _wtoi(value);
		setDuration(iValue);
		return S_FALSE;
	}

	HRESULT SAnimationSet::OnAttrFillBefore(const SStringW & value, BOOL bLoading)
	{
		bool bValue = STRINGASBOOL(value);
		setFillBefore(bValue);
		return S_FALSE;
	}

	HRESULT SAnimationSet::OnAttrFillAfter(const SStringW & value, BOOL bLoading)
	{
		bool bValue = STRINGASBOOL(value);
		setFillAfter(bValue);
		return S_FALSE;
	}

	HRESULT SAnimationSet::OnAttrStartOffset(const SStringW & value, BOOL bLoading)
	{
		long iValue = _wtoi(value);
		setStartOffset(iValue);
		return S_FALSE;
	}

	void SAnimationSet::initialize(int width, int height, int parentWidth, int parentHeight)
	{
		bool durationSet = (mFlags & PROPERTY_DURATION_MASK) == PROPERTY_DURATION_MASK;
		bool fillAfterSet = (mFlags & PROPERTY_FILL_AFTER_MASK) == PROPERTY_FILL_AFTER_MASK;
		bool fillBeforeSet = (mFlags & PROPERTY_FILL_BEFORE_MASK) == PROPERTY_FILL_BEFORE_MASK;
		bool shareInterpolator = (mFlags & PROPERTY_SHARE_INTERPOLATOR_MASK)
			== PROPERTY_SHARE_INTERPOLATOR_MASK;

		if (shareInterpolator) {
			ensureInterpolator();
		}


		long duration = mDuration;
		bool fillAfter = mFillAfter;
		bool fillBefore = mFillBefore;
		RepeatMode repeatMode = mRepeatMode;
		IInterpolator *interpolator = mInterpolator;
		long startOffset = mStartOffset;


		for (UINT i = 0; i < mAnimations.GetCount(); i++) {
			IAnimation* a = mAnimations[i];
			if (durationSet) {
				a->setDuration(duration);
			}
			if (fillAfterSet) {
				a->setFillAfter(fillAfter);
			}
			if (fillBeforeSet) {
				a->setFillBefore(fillBefore);
			}
			if (shareInterpolator) {
				a->setInterpolator(interpolator);
			}
			a->initialize(width, height, parentWidth, parentHeight);
		}
	}

	void SAnimationSet::scaleCurrentDuration(float scale)
	{
		int count = mAnimations.GetCount();
		for (int i = 0; i < count; i++) {
			mAnimations[i]->scaleCurrentDuration(scale);
		}
	}

	bool SAnimationSet::getTransformation(int64_t currentTime, STransformation &t)
	{
		if (mStartTime == -1) {
			mStartTime = currentTime;
		}

		if (!mStarted) {
			mStarted = true;
			fireAnimationStart();
		}
		t.clear();
		int64_t startOffset = getStartOffset();
		if (currentTime < (mStartTime + startOffset))
		{
			return true;
		}
		int count = mAnimations.GetCount();
		if (!mChildStarted) {
			mChildStarted = true;
			for (int i = count - 1; i >= 0; --i) {
				IAnimation * a = mAnimations[i];
				a->setStartTime(mStartTime + startOffset);
			}
		}

		bool more = false;
		bool ended = true;

		for (int i = count - 1; i >= 0; --i) {
			IAnimation * a = mAnimations[i];

			STransformation temp;
			more = a->getTransformation(currentTime, temp, getScaleFactor()) || more;
			t.compose(temp);

			ended = a->hasEnded() && ended;
		}

		if (ended || (currentTime- mStartTime) >= mLastEnd) {
			SLOG_INFO("ended:"<<ended<<" time span:"<<(currentTime- mStartTime)<<" cur time:"<<currentTime<<" start time:"<<mStartTime);
			if (mRepeatCount == mRepeated || isCanceled()) {
				mEnded = true;
				mChildStarted = false;
				fireAnimationEnd();
			}
			else {
				if (mRepeatCount > 0) {
					mRepeated++;
				}
				mChildStarted = false;
				mStartTime = -1;
				more = true;
				fireAnimationRepeat();
			}
		}

		return more;
	}



	long SAnimationSet::getDuration() const
	{
		int count = mAnimations.GetCount();
		long duration = 0;

		bool durationSet = (mFlags & PROPERTY_DURATION_MASK) == PROPERTY_DURATION_MASK;
		if (durationSet) {
			duration = mDuration;
		} else {
			for (int i = 0; i < count; i++) {
				duration = smax(duration, mAnimations[i]->computeDurationHint());
			}
		}

		return duration;
	}

	void SAnimationSet::addAnimation(IAnimation *a)
	{
		mAnimations.Add(a);

		if ((mFlags & PROPERTY_DURATION_MASK) == PROPERTY_DURATION_MASK) {
			mLastEnd = mStartOffset + mDuration;
		} else {
			long duration = a->computeDurationHint();
			if (mAnimations.GetCount() == 1) {
				mDuration = duration;
			} else {
				mDuration = smax(mDuration,duration);
			}
			mLastEnd = mStartOffset + mDuration;
		}

		mDirty = true;
	}

	void SAnimationSet::setDuration(long durationMillis)
	{
		mFlags |= PROPERTY_DURATION_MASK;
		SAnimation::setDuration(durationMillis);
		mLastEnd = mStartOffset + mDuration;
	}

	bool SAnimationSet::hasAlpha()
	{
		if (mDirty) {
			mDirty = mHasAlpha = false;

			int count = mAnimations.GetCount();
			for (int i = 0; i < count; i++) {
				if (mAnimations.GetAt(i)->hasAlpha()) {
					mHasAlpha = true;
					break;
				}
			}
		}

		return mHasAlpha;
	}

	void SAnimationSet::setFillBefore(bool fillBefore)
	{
		mFlags |= PROPERTY_FILL_BEFORE_MASK;
		SAnimation::setFillBefore(fillBefore);
	}

	void SAnimationSet::setFillAfter(bool fillAfter)
	{
		mFlags |= PROPERTY_FILL_AFTER_MASK;
		SAnimation::setFillAfter(fillAfter);
	}

	void SAnimationSet::init()
	{
		mFlags = 0;
		mDirty = false;
		mHasAlpha = false;
		mChildStarted = false;
	}

	void SAnimationSet::setFlag(int mask, bool value)
	{
		if (value) {
			mFlags |= mask;
		} else {
			mFlags &= ~mask;
		}
	}

	SAnimationSet::SAnimationSet(bool shareInterpolator/*=true*/)
	{
		setFlag(PROPERTY_SHARE_INTERPOLATOR_MASK, shareInterpolator);
		init();
	}

}
