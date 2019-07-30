
#include <souistd.h>
#include <animation/Animation.h>
#include <helper/stime.h>

namespace SOUI {



	SValueDescription SValueDescription::parseValue(const SStringW & value)
	{
		SValueDescription d;
		if (value.IsEmpty()) {
			d.type = IAnimation::ABSOLUTE_VALUE;
			d.value = 0.0f;
		}
		else if (value.EndsWith(L"%", true)) {
			d.type = IAnimation::RELATIVE_TO_SELF;
			d.value = (float)_wtof(value.Left(value.GetLength() - 1))/100;
		}
		else if (value.EndsWith(L"%p", true)) {
			d.type = IAnimation::RELATIVE_TO_PARENT;
			d.value = (float)_wtof(value.Left(value.GetLength() - 2))/100;
		}
		else
		{
			d.type = IAnimation::ABSOLUTE_VALUE;
			d.value = (float)_wtof(value);
		}
		return d;
	}


	////////////////////////////////////////////////////////////////////
	bool SAnimation::hasAlpha() const
	{
		return false;
	}

	int SAnimation::resolveSize(ValueType type, float value, int size, int parentSize)
	{
		switch (type) {
		case RELATIVE_TO_SELF:
			return (int)(size * value);
		case RELATIVE_TO_PARENT:
			return (int)(parentSize * value);
		case ABSOLUTE_VALUE:
		default:
			return (int)value;
		}
	}

	void SAnimation::applyTransformation(float interpolatedTime, STransformation & t)
	{

	}

	bool SAnimation::hasEnded() const
	{
		return mEnded;
	}

	bool SAnimation::hasStarted() const
	{
		return mStarted;
	}

	void SAnimation::fireAnimationEnd()
	{
		if (mListener != NULL) {
			mListener->onAnimationEnd(this);
		}
	}

	void SAnimation::fireAnimationRepeat()
	{
		if (mListener != NULL) {
			mListener->onAnimationRepeat(this);
		}
	}

	void SAnimation::fireAnimationStart()
	{
		if (mListener != NULL) {
			mListener->onAnimationStart(this);
		}
	}

	bool SAnimation::isCanceled()
	{
		return mStartTime == -2;
	}

	bool SAnimation::getTransformation(int64_t currentTime, STransformation & outTransformation)
	{
		if (mStartTime == -1) {
			mStartTime = currentTime;
		}

		int64_t startOffset = getStartOffset();
		long duration = mDuration;
		float normalizedTime;
		if (duration != 0) {
			normalizedTime = ((float)(currentTime - (mStartTime + startOffset))) /
				(float)duration;
		}
		else {
			// time is a step-change with a zero duration
			normalizedTime = currentTime < mStartTime ? 0.0f : 1.0f;
		}

		bool expired = normalizedTime >= 1.0f || isCanceled();
		mMore = !expired;

		if (!mFillEnabled || mRepeatCount!=0) normalizedTime = smax(smin(normalizedTime, 1.0f), 0.0f);

		if ((normalizedTime >= 0.0f || mFillBefore) && (normalizedTime <= 1.0f || mFillAfter)) {
			if (!mStarted) {
				mStarted = true;
				fireAnimationStart();
			}

			if (mFillEnabled) normalizedTime = smax(smin(normalizedTime, 1.0f), 0.0f);

			if (mCycleFlip) {
				normalizedTime = 1.0f - normalizedTime;
			}

			float interpolatedTime = mInterpolator->getInterpolation(normalizedTime);
			//outTransformation.clear();
			applyTransformation(interpolatedTime, outTransformation);
		}

		if (expired) {
			if (mRepeatCount == mRepeated || isCanceled()) {
				if (!mEnded) {
					mEnded = true;
					fireAnimationEnd();
				}
			}
			else {
				if (mRepeatCount > 0) {
					mRepeated++;
				}

				if (mRepeatMode == REVERSE) {
					mCycleFlip = !mCycleFlip;
				}

				mStartTime = -1;
				mMore = true;

				fireAnimationRepeat();
			}
		}

		if (!mMore && mOneMoreTime) {
			mOneMoreTime = false;
			return true;
		}

		return mMore;
	}

	bool SAnimation::getTransformation(uint64_t currentTime, STransformation & outTransformation, float scale)
	{
		mScaleFactor = scale;
		return getTransformation(currentTime, outTransformation);
	}

	long SAnimation::computeDurationHint() const
	{
		return (getStartOffset() + getDuration()) * (getRepeatCount() + 1);
	}

	void SAnimation::ensureInterpolator()
	{
		if (!mInterpolator) {
			mInterpolator.Attach(new SAccelerateDecelerateInterpolator());
		}
	}

	void SAnimation::setAnimationListener(IAnimationListener* listener)
	{
		mListener = listener;
	}

	int SAnimation::getZAdjustment() const
	{
		return mZAdjustment;
	}

	int SAnimation::getRepeatCount() const
	{
		return mRepeatCount;
	}

	IAnimation::RepeatMode SAnimation::getRepeatMode() const
	{
		return mRepeatMode;
	}

	long SAnimation::getStartOffset() const
	{
		return mStartOffset;
	}

	long SAnimation::getDuration() const
	{
		return mDuration;
	}

	int64_t SAnimation::getStartTime() const
	{
		return mStartTime;
	}

	IInterpolator* SAnimation::getInterpolator() const
	{
		return mInterpolator;
	}

	float SAnimation::getScaleFactor()
	{
		return mScaleFactor;
	}

	void SAnimation::setZAdjustment(int zAdjustment)
	{
		mZAdjustment = zAdjustment;
	}

	void SAnimation::setRepeatCount(int repeatCount)
	{
		if (repeatCount < 0) {
			repeatCount = INFINITE;
		}
		mRepeatCount = repeatCount;
	}

	void SAnimation::setRepeatMode(RepeatMode repeatMode)
	{
		mRepeatMode = repeatMode;
	}

	void SAnimation::startNow()
	{
		setStartTime(STime::GetCurrentTimeMs());
	}

	void SAnimation::start()
	{
		setStartTime(-1);
	}

	void SAnimation::setStartTime(int64_t startTimeMillis)
	{
		mStartTime = startTimeMillis;
		mStarted = mEnded = false;
		mCycleFlip = false;
		mRepeated = 0;
		mMore = true;
	}

	void SAnimation::setStartOffset(long offset)
	{
		mStartOffset = offset;
	}

	void SAnimation::setFillEnabled(bool fillEnabled)
	{
		mFillEnabled = fillEnabled;
	}

	bool SAnimation::isFillEnabled() const
	{
		return mFillEnabled;
	}

	bool SAnimation::getFillAfter() const
	{
		return mFillAfter;
	}

	void SAnimation::setFillAfter(bool bFill)
	{
		mFillAfter = bFill;
	}

	bool SAnimation::getFillBefore() const
	{
		return mFillBefore;
	}

	void SAnimation::setFillBefore(bool bFill)
	{
		mFillBefore = bFill;
	}

	void SAnimation::scaleCurrentDuration(float scale)
	{
		mDuration = (long)(mDuration * scale);
		mStartOffset = (long)(mStartOffset * scale);
	}

	void SAnimation::setDuration(long durationMillis)
	{
		mDuration = durationMillis;
	}

	void SAnimation::setInterpolator(IInterpolator *i)
	{
		mInterpolator = i;
	}

	void SAnimation::cancel()
	{
		if (mStarted && !mEnded)
		{
			mStartTime = -2;
			mEnded = true;
			fireAnimationEnd();
		}
	}

	void SAnimation::reset()
	{
		mEnded = false;

		mStarted = false;

		mStartTime = -1;

		mStartOffset = 0;
		mDuration = 0;
		mRepeatCount = 0;
		mRepeated = 0;
		mRepeatMode = RESTART;
		mListener = NULL;
		mScaleFactor = 1.0f;


		mMore = true;
		mOneMoreTime = true;


		mFillBefore = false;

		mFillAfter = false;

		mFillEnabled = true;

		mUserData = 0;
		ensureInterpolator();
	}

	void SAnimation::initialize(int width, int height, int parentWidth, int parentHeight)
	{
	}

	SAnimation::SAnimation():mID(0)
	{
		reset();
	}

	void SAnimation::setUserData(ULONG_PTR data)
	{
		mUserData = data;
	}

	ULONG_PTR SAnimation::getUserData() const
	{
		return mUserData;
	}

}
