
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

	void SAnimationSet::restoreChildrenStartOffset()
	{
		if (mStoredOffsets.GetCount() != mAnimations.GetCount())
			return;

		for (UINT i = 0; i < mAnimations.GetCount(); i++) {
			mAnimations[i]->setStartOffset(mStoredOffsets[i]);
		}
	}

	void SAnimationSet::reset()
	{
		SAnimation::reset();
		restoreChildrenStartOffset();
	}

	void SAnimationSet::initialize(int width, int height, int parentWidth, int parentHeight)
	{
		bool durationSet = (mFlags & PROPERTY_DURATION_MASK) == PROPERTY_DURATION_MASK;
		bool fillAfterSet = (mFlags & PROPERTY_FILL_AFTER_MASK) == PROPERTY_FILL_AFTER_MASK;
		bool fillBeforeSet = (mFlags & PROPERTY_FILL_BEFORE_MASK) == PROPERTY_FILL_BEFORE_MASK;
		bool repeatModeSet = (mFlags & PROPERTY_REPEAT_MODE_MASK) == PROPERTY_REPEAT_MODE_MASK;
		bool shareInterpolator = (mFlags & PROPERTY_SHARE_INTERPOLATOR_MASK)
			== PROPERTY_SHARE_INTERPOLATOR_MASK;
		bool startOffsetSet = (mFlags & PROPERTY_START_OFFSET_MASK)
			== PROPERTY_START_OFFSET_MASK;

		if (shareInterpolator) {
			ensureInterpolator();
		}


		long duration = mDuration;
		bool fillAfter = mFillAfter;
		bool fillBefore = mFillBefore;
		RepeatMode repeatMode = mRepeatMode;
		IInterpolator *interpolator = mInterpolator;
		long startOffset = mStartOffset;

		mStoredOffsets.SetCount(mAnimations.GetCount());

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
			if (repeatModeSet) {
				a->setRepeatMode(repeatMode);
			}
			if (shareInterpolator) {
				a->setInterpolator(interpolator);
			}
			if (startOffsetSet) {
				long offset = a->getStartOffset();
				a->setStartOffset(offset + startOffset);
				mStoredOffsets[i] = offset;
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
		int count = mAnimations.GetCount();

		bool more = false;
		bool started = false;
		bool ended = true;

		t.clear();

		for (int i = count - 1; i >= 0; --i) {
			IAnimation * a = mAnimations[i];

			STransformation temp;
			more = a->getTransformation(currentTime, temp, getScaleFactor()) || more;
			t.compose(temp);

			started = started || a->hasStarted();
			ended = a->hasEnded() && ended;
		}

		if (started && !mStarted) {
			if (mListener != NULL) {
				mListener->onAnimationStart(this);
			}
			mStarted = true;
		}

		if (ended != mEnded) {
			if (mListener != NULL) {
				mListener->onAnimationEnd(this);
			}
			mEnded = ended;
		}

		return more;
	}

	long SAnimationSet::computeDurationHint()
	{
		long duration = 0;
		int count = mAnimations.GetCount();
		for (int i = count - 1; i >= 0; --i) {
			long d = mAnimations[i]->computeDurationHint();
			if (d > duration) duration = d;
		}
		return duration;
	}

	long SAnimationSet::getDuration()
	{
		int count = mAnimations.GetCount();
		long duration = 0;

		bool durationSet = (mFlags & PROPERTY_DURATION_MASK) == PROPERTY_DURATION_MASK;
		if (durationSet) {
			duration = mDuration;
		} else {
			for (int i = 0; i < count; i++) {
				duration = smax(duration, mAnimations[i]->getDuration());
			}
		}

		return duration;
	}

	int64_t SAnimationSet::getStartTime() const
	{
		int64_t startTime = STime::GetCurrentTimeMs()+10000000;

		int count = mAnimations.GetCount();

		for (int i = 0; i < count; i++) {
			IAnimation *a = mAnimations.GetAt(i);
			startTime = smin(startTime, a->getStartTime());
		}

		return startTime;
	}

	void SAnimationSet::setStartTime(int64_t startTimeMillis)
	{
		SAnimation::setStartTime(startTimeMillis);

		int count = mAnimations.GetCount();

		for (int i = 0; i < count; i++) {
			mAnimations[i]->setStartTime(startTimeMillis);
		}
	}

	void SAnimationSet::addAnimation(IAnimation *a)
	{
		mAnimations.Add(a);

		if ((mFlags & PROPERTY_DURATION_MASK) == PROPERTY_DURATION_MASK) {
			mLastEnd = mStartOffset + mDuration;
		} else {
			if (mAnimations.GetCount() == 1) {
				mDuration = a->getStartOffset() + a->getDuration();
				mLastEnd = mStartOffset + mDuration;
			} else {
				mLastEnd = smax(mLastEnd, a->getStartOffset() + a->getDuration());
				mDuration = mLastEnd - mStartOffset;
			}
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

	void SAnimationSet::setStartOffset(long startOffset)
	{
		mFlags |= PROPERTY_START_OFFSET_MASK;
		SAnimation::setStartOffset(startOffset);
	}

	void SAnimationSet::setRepeatMode(RepeatMode repeatMode)
	{
		mFlags |= PROPERTY_REPEAT_MODE_MASK;
		SAnimation::setRepeatMode(repeatMode);
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
	}

	void SAnimationSet::setFlag(int mask, bool value)
	{
		if (value) {
			mFlags |= mask;
		} else {
			mFlags &= ~mask;
		}
	}

	void SAnimationSet::copy(const IAnimation *src)
	{
		SAnimation::copy(src);
		const SAnimationSet * src2 = sobj_cast<const SAnimationSet>(src);
		mAnimations.Copy(src2->mAnimations);
		mFlags = src2->mFlags;
	}

	SAnimationSet::SAnimationSet(bool shareInterpolator/*=true*/)
	{
		setFlag(PROPERTY_SHARE_INTERPOLATOR_MASK, shareInterpolator);
		init();
	}

}
