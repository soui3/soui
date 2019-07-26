#include <souistd.h>
#include <valueAnimator/ValueAnimator.h>
#include <animation/SInterpolatorImpl.h>
#include <helper/stime.h>

namespace SOUI{


	IValueAnimator::IValueAnimator()
	{
		sDurationScale = 1.0f;
		mStartTime = -1;

		mStartTimeCommitted;

		mSeekFraction = -1;

		mReversing;
		mOverallFraction = 0.f;

		mCurrentFraction = 0.f;

		mLastFrameTime = -1;

		mFirstFrameTime = -1;

		mRunning = false;

		mStarted = false;

		mStartListenersCalled = false;

		mInitialized = false;

		mAnimationEndRequested = false;



		mDuration = 300;

		mStartDelay = 0;

		mRepeatCount = 0;

		mRepeatMode = IAnimation::RESTART;
		mInterpolator.Attach(new SLinearInterpolator());
	}

	void IValueAnimator::addAnimationCallback()
	{
		mContainer->RegisterTimelineHandler(this);
	}

	void IValueAnimator::removeAnimationCallback()
	{
		mContainer->UnregisterTimelineHandler(this);
	}

	void IValueAnimator::OnNextFrame()
	{
		doAnimationFrame(STime::GetCurrentTimeMs());
	}

	void IValueAnimator::animateValue(float fraction)
	{
		fraction = mInterpolator->getInterpolation(fraction);
		mCurrentFraction = fraction;
		onEvaluateValue(fraction);
		int numListeners = mUpdateListeners.GetCount();
		for (int i = 0; i < numListeners; ++i) {
			mUpdateListeners[i]->onAnimationUpdate(this);
		}
	}

	float IValueAnimator::getAnimatedFraction() const
	{
		return mCurrentFraction;
	}

	bool IValueAnimator::doAnimationFrame(uint64_t frameTime)
	{
		if (mStartTime < 0) {
			// First frame. If there is start delay, start delay count down will happen *after* this
			// frame.
			mStartTime = mReversing ? frameTime : frameTime + (long) (mStartDelay * sDurationScale);
		}

		if (!mRunning) {
			// If not running, that means the animation is in the start delay phase of a forward
			// running animation. In the case of reversing, we want to run start delay in the end.
			if (mStartTime > frameTime && mSeekFraction == -1) {
				// This is when no seek fraction is set during start delay. If developers change the
				// seek fraction during the delay, animation will start from the seeked position
				// right away.
				return false;
			} else {
				// If mRunning is not set by now, that means non-zero start delay,
				// no seeking, not reversing. At this point, start delay has passed.
				mRunning = true;
				startAnimation();
			}
		}

		if (mLastFrameTime < 0) {
			if (mSeekFraction >= 0) {
				long seekTime = (long) (getScaledDuration() * mSeekFraction);
				mStartTime = frameTime - seekTime;
				mSeekFraction = -1;
			}
			mStartTimeCommitted = false; // allow start time to be compensated for jank
		}
		mLastFrameTime = frameTime;
		// The frame time might be before the start time during the first frame of
		// an animation.  The "current time" must always be on or after the start
		// time to avoid animating frames at negative time intervals.  In practice, this
		// is very rare and only happens when seeking backwards.
		uint64_t currentTime = smax(frameTime, mStartTime);
		bool finished = animateBasedOnTime(currentTime);

		if (finished) {
			endAnimation();
		}
		return finished;
	}

	bool IValueAnimator::isInitialized()
	{
		return mInitialized;
	}

	void IValueAnimator::skipToEndValue(bool inReverse)
	{
		float endFraction = inReverse ? 0.f : 1.f;
		if (mRepeatCount % 2 == 1 && mRepeatMode == IAnimation::REVERSE) {
			// This would end on fraction = 0
			endFraction = 0.f;
		}
		animateValue(endFraction);
	}

	void IValueAnimator::animateBasedOnPlayTime(long currentPlayTime, long lastPlayTime, bool inReverse)
	{
		if (currentPlayTime < 0 || lastPlayTime < 0) {
			return;//error
		}
		// Check whether repeat callback is needed only when repeat count is non-zero
		if (mRepeatCount > 0) {
			int iteration = (int) (currentPlayTime / mDuration);
			int lastIteration = (int) (lastPlayTime / mDuration);

			// Clamp iteration to [0, mRepeatCount]
			iteration = smin(iteration, mRepeatCount);
			lastIteration = smin(lastIteration, mRepeatCount);

			if (iteration != lastIteration) {
				int numListeners = mListeners.GetCount();
				for (int i = 0; i < numListeners; ++i) {
					mListeners[i]->onAnimationRepeat(this);
				}
			}
		}

		if (mRepeatCount != INFINITE && currentPlayTime >= (mRepeatCount + 1) * mDuration) {
			skipToEndValue(inReverse);
		} else {
			// Find the current fraction:
			float fraction = currentPlayTime / (float) mDuration;
			fraction = getCurrentIterationFraction(fraction, inReverse);
			animateValue(fraction);
		}
	}

	bool IValueAnimator::animateBasedOnTime(uint64_t currentTime)
	{
		bool done = false;
		if (mRunning) {
			long scaledDuration = getScaledDuration();
			float fraction = scaledDuration > 0 ?
				(float)(currentTime - mStartTime) / scaledDuration : 1.f;
			float lastFraction = mOverallFraction;
			bool newIteration = (int) fraction > (int) lastFraction;
			bool lastIterationFinished = (fraction >= mRepeatCount + 1) &&
				(mRepeatCount != INFINITE);
			if (scaledDuration == 0) {
				// 0 duration animator, ignore the repeat count and skip to the end
				done = true;
			} else if (newIteration && !lastIterationFinished) {
				// Time to repeat
				SArray<IAnimatorListener*> tmpListeners = mListeners;
				for (UINT i = 0; i < tmpListeners.GetCount(); i++) {
					tmpListeners[i]->onAnimationRepeat(this);
				}
			} else if (lastIterationFinished) {
				done = true;
			}
			mOverallFraction = clampFraction(fraction);
			float currentIterationFraction = getCurrentIterationFraction(
				mOverallFraction, mReversing);
			animateValue(currentIterationFraction);
		}
		return done;
	}

	void IValueAnimator::commitAnimationFrame(long frameTime)
	{
		if (!mStartTimeCommitted) {
			mStartTimeCommitted = true;
			uint64_t adjustment = frameTime - mLastFrameTime;
			if (adjustment > 0) {
				mStartTime += adjustment;
			}
		}
	}

	bool IValueAnimator::isPulsingInternal()
	{
		return mLastFrameTime >= 0;
	}

	void IValueAnimator::startAnimation()
	{
		mAnimationEndRequested = false;
		mRunning = true;
		if (mSeekFraction >= 0) {
			mOverallFraction = mSeekFraction;
		} else {
			mOverallFraction = 0.f;
		}
		notifyStartListeners();
	}

	void IValueAnimator::endAnimation()
	{
		if (mAnimationEndRequested) {
			return;
		}
		removeAnimationCallback();

		mAnimationEndRequested = true;
		bool notify = (mStarted || mRunning);
		if (notify && !mRunning) {
			// If it's not yet running, then start listeners weren't called. Call them now.
			notifyStartListeners();
		}
		mRunning = false;
		mStarted = false;
		mStartListenersCalled = false;
		mLastFrameTime = -1;
		mFirstFrameTime = -1;
		mStartTime = -1;
		AddRef();
		if (notify) {
			SArray<IAnimatorListener*> tmpListeners = mListeners;
			int numListeners = tmpListeners.GetCount();
			for (int i = 0; i < numListeners; ++i) {
				tmpListeners[i]->onAnimationEnd(this);
			}
		}
		mReversing = false;
		Release();
	}

	bool IValueAnimator::canReverse()
	{
		return true;
	}

	void IValueAnimator::reverse()
	{
		if (isPulsingInternal()) {
			uint64_t currentTime = STime::GetCurrentTimeMs();
			uint64_t currentPlayTime = currentTime - mStartTime;
			uint64_t timeLeft = getScaledDuration() - currentPlayTime;
			mStartTime = currentTime - timeLeft;
			mStartTimeCommitted = true; // do not allow start time to be compensated for jank
			mReversing = !mReversing;
		} else if (mStarted) {
			mReversing = !mReversing;
			end();
		} else {
			start(true);
		}
	}

	bool IValueAnimator::isStarted()
	{
		return mStarted;
	}

	bool IValueAnimator::isRunning()
	{
		return mRunning;
	}

	void IValueAnimator::end()
	{
		if (!mRunning) {
			// Special case if the animation has not yet started; get it ready for ending
			startAnimation();
			mStarted = true;
		}
		animateValue(shouldPlayBackward(mRepeatCount, mReversing) ? 0.f : 1.f);
		endAnimation();
	}

	void IValueAnimator::start(ITimelineHandlersMgr *pContainer)
	{
		mContainer = pContainer;
		start(false);
	}

	void IValueAnimator::start(bool playBackwards)
	{
		SASSERT(mInterpolator);
		mReversing = playBackwards;
		// Special case: reversing from seek-to-0 should act as if not seeked at all.
		if (playBackwards && mSeekFraction != -1 && mSeekFraction != 0) {
			if (mRepeatCount == INFINITE) {
				// Calculate the fraction of the current iteration.
				float fraction = (float) (mSeekFraction - floor(mSeekFraction));
				mSeekFraction = 1 - fraction;
			} else {
				mSeekFraction = 1 + mRepeatCount - mSeekFraction;
			}
		}
		mStarted = true;
		mRunning = false;
		mAnimationEndRequested = false;
		// Resets mLastFrameTime when start() is called, so that if the animation was running,
		// calling start() would put the animation in the
		// started-but-not-yet-reached-the-first-frame phase.
		mLastFrameTime = -1;
		mFirstFrameTime = -1;
		mStartTime = -1;
		addAnimationCallback();

		if (mStartDelay == 0 || mSeekFraction >= 0 || mReversing) {
			// If there's no start delay, init the animation and notify start listeners right away
			// to be consistent with the previous behavior. Otherwise, postpone this until the first
			// frame after the start delay.
			startAnimation();
			if (mSeekFraction == -1) {
				// No seek, start at play time 0. Note that the reason we are not using fraction 0
				// is because for animations with 0 duration, we want to be consistent with pre-N
				// behavior: skip to the value immediately.
				setCurrentPlayTime(0);
			} else {
				setCurrentFraction(mSeekFraction);
			}
		}
	}

	void IValueAnimator::notifyStartListeners()
	{
		AddRef();
		SArray<IAnimatorListener *> tmpListeners = mListeners;
		int numListeners = tmpListeners.GetCount();
		for (int i = 0; i < numListeners; ++i) {
			tmpListeners[i]->onAnimationStart(this);
		}
		mStartListenersCalled = true;
		Release();
	}

	void IValueAnimator::removeListener(IAnimatorListener * p)
	{
		int iFind = mListeners.Find(p);
		if (iFind != -1)
		{
			mListeners.RemoveAt(iFind);
		}
	}

	void IValueAnimator::addListener(IAnimatorListener * p)
	{
		mListeners.Add(p);
	}

	IInterpolator * IValueAnimator::getInterpolator()
	{
		return mInterpolator;
	}

	void IValueAnimator::setInterpolator(IInterpolator * value)
	{
		mInterpolator = value;
	}

	void IValueAnimator::removeUpdateListener(IAnimatorUpdateListener *listener)
	{
		int iFind = mUpdateListeners.Find(listener);
		if (iFind != -1) mUpdateListeners.RemoveAt(iFind);
	}

	void IValueAnimator::removeAllUpdateListeners()
	{
		mUpdateListeners.RemoveAll();
	}

	void IValueAnimator::addUpdateListener(IAnimatorUpdateListener * listener)
	{
		mUpdateListeners.Add(listener);
	}

	IAnimation::RepeatMode IValueAnimator::getRepeatMode()
	{
		return mRepeatMode;
	}

	void IValueAnimator::setRepeatMode(IAnimation::RepeatMode value)
	{
		mRepeatMode = value;
	}

	int IValueAnimator::getRepeatCount()
	{
		return mRepeatCount;
	}

	void IValueAnimator::setRepeatCount(int value)
	{
		mRepeatCount = value;
	}

	void IValueAnimator::setStartDelay(long startDelay)
	{
		// Clamp start delay to non-negative range.
		if (startDelay < 0) {
			startDelay = 0;
		}
		mStartDelay = startDelay;
	}

	long IValueAnimator::getStartDelay()
	{
		return mStartDelay;
	}

	long IValueAnimator::getCurrentPlayTime()
	{
		if (!mInitialized || (!mStarted && mSeekFraction < 0)) {
			return 0;
		}
		if (mSeekFraction >= 0) {
			return (long) (mDuration * mSeekFraction);
		}
		return (long) ((STime::GetCurrentTimeMs() - mStartTime) / sDurationScale);
	}

	bool IValueAnimator::shouldPlayBackward(int iteration, bool inReverse)
	{
		if (iteration > 0 && mRepeatMode == IAnimation::REVERSE &&
			(iteration < (mRepeatCount + 1) || mRepeatCount == -1)) {
				// if we were seeked to some other iteration in a reversing animator,
				// figure out the correct direction to start playing based on the iteration
				if (inReverse) {
					return (iteration % 2) == 0;
				} else {
					return (iteration % 2) != 0;
				}
		} else {
			return inReverse;
		}
	}

	float IValueAnimator::clampFraction(float fraction)
	{
		if (fraction < 0) {
			fraction = 0;
		} else if (mRepeatCount != INFINITE) {
			fraction = smin(fraction, mRepeatCount + 1);
		}
		return fraction;
	}

	float IValueAnimator::getCurrentIterationFraction(float fraction, bool inReverse)
	{
		fraction = clampFraction(fraction);
		int iteration = getCurrentIteration(fraction);
		float currentFraction = fraction - iteration;
		return shouldPlayBackward(iteration, inReverse) ? 1.f - currentFraction : currentFraction;
	}

	int IValueAnimator::getCurrentIteration(float fraction)
	{
		fraction = clampFraction(fraction);
		// If the overall fraction is a positive integer, we consider the current iteration to be
		// complete. In other words, the fraction for the current iteration would be 1, and the
		// current iteration would be overall fraction - 1.
		double iteration = floor(fraction);
		if (fraction == iteration && fraction > 0) {
			iteration--;
		}
		return (int) iteration;
	}

	void IValueAnimator::setCurrentFraction(float fraction)
	{
		fraction = clampFraction(fraction);
		mStartTimeCommitted = true; // do not allow start time to be compensated for jank
		if (isPulsingInternal()) {
			long seekTime = (long) (getScaledDuration() * fraction);
			uint64_t currentTime = STime::GetCurrentTimeMs();
			// Only modify the start time when the animation is running. Seek fraction will ensure
			// non-running animations skip to the correct start time.
			mStartTime = currentTime - seekTime;
		} else {
			// If the animation loop hasn't started, or during start delay, the startTime will be
			// adjusted once the delay has passed based on seek fraction.
			mSeekFraction = fraction;
		}
		mOverallFraction = fraction;
		float currentIterationFraction = getCurrentIterationFraction(fraction, mReversing);
		animateValue(currentIterationFraction);
	}

	void IValueAnimator::setCurrentPlayTime(long playTime)
	{
		float fraction = mDuration > 0 ? (float) playTime / mDuration : 1;
		setCurrentFraction(fraction);
	}

	long IValueAnimator::getTotalDuration()
	{
		if (mRepeatCount == -1) {
			return 100000;//todo: DURATION_INFINITE;
		} else {
			return mStartDelay + (mDuration * (mRepeatCount + 1));
		}
	}

	long IValueAnimator::getDuration()
	{
		return mDuration;
	}

	long IValueAnimator::getScaledDuration()
	{
		return (long)(mDuration * sDurationScale);
	}

	void IValueAnimator::setDuration(long duration)
	{
		if (duration >= 0) {
			mDuration = duration;
		}
	}


}