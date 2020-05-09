
#include <souistd.h>
#include <animation/SAnimation.h>
#include <helper/stime.h>

SNSBEGIN


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
	bool bMore = !expired;

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
		outTransformation.clear();
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
			else
			{
				mRepeated = 1;
			}

			if (mRepeatMode == REVERSE) {
				mCycleFlip = !mCycleFlip;
			}

			mStartTime = currentTime;
			bMore = true;

			fireAnimationRepeat();
		}
	}

	return bMore;
}

bool SAnimation::getTransformation(uint64_t currentTime, STransformation & outTransformation, float scale)
{
	mScaleFactor = scale;
	return getTransformation(currentTime, outTransformation);
}

long SAnimation::computeDurationHint() const
{
	return getStartOffset() + getDuration() * (getRepeatCount() + 1);
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

IAnimation::ZAdjustment SAnimation::getZAdjustment() const
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
	return mRepeated == 0 ? mStartOffset : 0;
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

void SAnimation::setZAdjustment(ZAdjustment zAdjustment)
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
	mZAdjustment = ZORDER_NORMAL;
	mListener = NULL;
	mScaleFactor = 1.0f;

	mFillBefore = false;

	mFillAfter = true;

	mFillEnabled = true;

	mUserData = 0;
	ensureInterpolator();
}

void SAnimation::copy(const IAnimation * src)
{
	const SAnimation * src2 = (SAnimation*)src;
	mStartOffset = src2->mStartOffset;
	mDuration = src2->mDuration;
	mRepeatCount = src2->mRepeatCount;
	mRepeatMode = src2->mRepeatMode;
	mZAdjustment = src2->mZAdjustment;
	mScaleFactor = src2->mScaleFactor;
	mFillBefore = src2->mFillBefore;
	mFillAfter = src2->mFillAfter;
	mFillEnabled = src2->mFillEnabled;
	mInterpolator = src2->mInterpolator;

	mUserData = src2->mUserData;
	mID = src2->mID;
	mName = src2->mName;
}

IAnimation * SAnimation::clone() const
{
	IAnimation *pRet = SApplication::getSingletonPtr()->CreateAnimationByName(GetClassName());
	if (pRet)
	{
		pRet->copy(this);
	}
	return pRet;
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

SAnimation::~SAnimation()
{
}

SNSEND