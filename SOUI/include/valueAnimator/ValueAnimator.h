/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/**
 * This class provides a simple timing engine for running animations
 * which calculate animated values and set them on target objects.
 *
 * <p>There is a single timing pulse that all animations use. It runs in a
 * custom handler to ensure that property changes happen on the UI thread.</p>
 *
 * <p>By default, IValueAnimator uses non-linear time interpolation, via the
 * {@link AccelerateDecelerateInterpolator} class, which accelerates into and decelerates
 * out of an animation. This behavior can be changed by calling
 * {@link IValueAnimator#setInterpolator(TimeInterpolator)}.</p>
 *
 * <p>Animators can be created from either code or resource files. Here is an example
 * of a IValueAnimator resource file:</p>
 *
 * {@sample development/samples/ApiDemos/res/anim/animator.xml ValueAnimatorResources}
 *
 * <p>Starting from API 23, it is also possible to use a combination of {@link PropertyValuesHolder}
 * and {@link Keyframe} resource tags to create a multi-step animation.
 * Note that you can specify explicit fractional values (from 0 to 1) for
 * each keyframe to determine when, in the overall duration, the animation should arrive at that
 * value. Alternatively, you can leave the fractions off and the keyframes will be equally
 * distributed within the total duration:</p>
 *
 * {@sample development/samples/ApiDemos/res/anim/value_animator_pvh_kf.xml
 * ValueAnimatorKeyframeResources}
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about animating with {@code IValueAnimator}, read the
 * <a href="{@docRoot}guide/topics/graphics/prop-animation.html#value-animator">Property
 * Animation</a> developer guide.</p>
 * </div>
 */

#include <interface/Animation-i.h>
#include <interface/STimelineHandler-i.h>
#include <animation/SInterpolatorImpl.h>
#include "TypeEvaluator.h"
#include <helper/stime.h>
 /**
 * Internal constants
 */
#define  sDurationScale  1.0f

namespace SOUI{

class SOUI_EXP IValueAnimator  : public IObjRef, public IObject, ITimelineHandler{
public:
	struct IAnimatorUpdateListener
	{
		virtual void onAnimationUpdate(IValueAnimator *pAnimator) = 0;
	};

	struct IAnimatorListener {
		/**
		* <p>Notifies the start of the animation.</p>
		*
		* @param animation The started animation.
		*/
		virtual void onAnimationStart(IValueAnimator * pAnimator) = 0;

		/**
		* <p>Notifies the end of the animation. This callback is not invoked
		* for animations with repeat count set to INFINITE.</p>
		*
		* @param animation The animation which reached its end.
		*/
		virtual void onAnimationEnd(IValueAnimator * pAnimator) = 0;

		/**
		* <p>Notifies the repetition of the animation.</p>
		*
		* @param animation The animation which was repeated.
		*/
		virtual void onAnimationRepeat(IValueAnimator * pAnimator) = 0;
	};
protected:
    /**
     * The first time that the animation's animateFrame() method is called. This time is used to
     * determine elapsed time (and therefore the elapsed fraction) in subsequent calls
     * to animateFrame().
     *
     * Whenever mStartTime is set, you must also update mStartTimeCommitted.
     */
	uint64_t mStartTime = -1;

    /**
     * When true, the start time has been firmly committed as a chosen reference point in
     * time by which the progress of the animation will be evaluated.  When false, the
     * start time may be updated when the first animation frame is committed so as
     * to compensate for jank that may have occurred between when the start time was
     * initialized and when the frame was actually drawn.
     *
     * This flag is generally set to false during the first frame of the animation
     * when the animation playing state transitions from STOPPED to RUNNING or
     * resumes after having been paused.  This flag is set to true when the start time
     * is firmly committed and should not be further compensated for jank.
     */
    bool mStartTimeCommitted;

    /**
     * Set when setCurrentPlayTime() is called. If negative, animation is not currently seeked
     * to a value.
     */
    float mSeekFraction = -1;

    /**
     * Flag to indicate whether this animator is playing in reverse mode, specifically
     * by being started or interrupted by a call to reverse(). This flag is different than
     * mPlayingBackwards, which indicates merely whether the current iteration of the
     * animator is playing in reverse. It is used in corner cases to determine proper end
     * behavior.
     */
    bool mReversing;

    /**
     * Tracks the overall fraction of the animation, ranging from 0 to mRepeatCount + 1
     */
    float mOverallFraction = 0.f;

    /**
     * Tracks current elapsed/eased fraction, for querying in getAnimatedFraction().
     * This is calculated by interpolating the fraction (range: [0, 1]) in the current iteration.
     */
    float mCurrentFraction = 0.f;

    /**
     * Tracks the time (in milliseconds) when the last frame arrived.
     */
	uint64_t mLastFrameTime = -1;

    /**
     * Tracks the time (in milliseconds) when the first frame arrived. Note the frame may arrive
     * during the start delay.
     */
	uint64_t mFirstFrameTime = -1;

    /**
     * Additional playing state to indicate whether an animator has been start()'d. There is
     * some lag between a call to start() and the first animation frame. We should still note
     * that the animation has been started, even if it's first animation frame has not yet
     * happened, and reflect that state in isRunning().
     * Note that delayed animations are different: they are not started until their first
     * animation frame, which occurs after their delay elapses.
     */
    bool mRunning = false;

    /**
     * Additional playing state to indicate whether an animator has been start()'d, whether or
     * not there is a nonzero startDelay.
     */
    bool mStarted = false;

    /**
     * Tracks whether we've notified listeners of the onAnimationStart() event. This can be
     * complex to keep track of since we notify listeners at different times depending on
     * startDelay and whether start() was called before end().
     */
	bool mStartListenersCalled = false;

    /**
     * Flag that denotes whether the animation is set up and ready to go. Used to
     * set up animation that has not yet been started.
     */
    bool mInitialized = false;

    /**
     * Flag that tracks whether animation has been requested to end.
     */
    bool mAnimationEndRequested = false;

    //
    // Backing variables
    //

    // How long the animation should last in ms
    long mDuration = 300;

    // The amount of time in ms to delay starting the animation after start() is called. Note
    // that this start delay is unscaled. When there is a duration scale set on the animator, the
    // scaling factor will be applied to this delay.
    long mStartDelay = 0;

    // The number of times the animation will repeat. The default is 0, which means the animation
    // will play only once
    int mRepeatCount = 0;

    /**
     * The type of repetition that will occur when repeatMode is nonzero. RESTART means the
     * animation will start from the beginning on every new cycle. REVERSE means the animation
     * will reverse directions on each iteration.
     */
	IAnimation::RepeatMode mRepeatMode = IAnimation::RESTART;


    /**
     * The time interpolator to be used. The elapsed fraction of the animation will be passed
     * through this interpolator to calculate the interpolated fraction, which is then used to
     * calculate the animated values.
     */

    SAutoRefPtr<IInterpolator> mInterpolator;

    /**
     * The set of listeners to be sent events through the life of an animation.
     */
    SArray<IAnimatorUpdateListener*> mUpdateListeners;
	SArray<IAnimatorListener*> mListeners;

	ITimelineHandlersMgr * mContainer;

    /**
     * Creates a new IValueAnimator object. This default constructor is primarily for
     * use internally; the factory methods which take parameters are more generally
     * useful.
     */
	public:IValueAnimator(){
		mInterpolator.Attach(new SLinearInterpolator());
    }
	
    /**
     * Sets the length of the animation. The default duration is 300 milliseconds.
     *
     * @param duration The length of the animation, in milliseconds. This value cannot
     * be negative.
     * @return IValueAnimator The object called with setDuration(). This return
     * value makes it easier to compose statements together that construct and then set the
     * duration, as in <code>IValueAnimator.ofInt(0, 10).setDuration(500).start()</code>.
     */
    public: void setDuration(long duration) {
        if (duration >= 0) {
			mDuration = duration;
        }
    }

    private:long getScaledDuration() {
        return (long)(mDuration * sDurationScale);
    }

    /**
     * Gets the length of the animation. The default duration is 300 milliseconds.
     *
     * @return The length of the animation, in milliseconds.
     */
    public:long getDuration() {
        return mDuration;
    }

    public:long getTotalDuration() {
        if (mRepeatCount == -1) {
			return 100000;//todo: DURATION_INFINITE;
        } else {
            return mStartDelay + (mDuration * (mRepeatCount + 1));
        }
    }

    /**
     * Sets the position of the animation to the specified point in time. This time should
     * be between 0 and the total duration of the animation, including any repetition. If
     * the animation has not yet been started, then it will not advance forward after it is
     * set to this time; it will simply set the time to this value and perform any appropriate
     * actions based on that time. If the animation is already running, then setCurrentPlayTime()
     * will set the current playing time to this value and continue playing from that point.
     *
     * @param playTime The time, in milliseconds, to which the animation is advanced or rewound.
     */
    public:void setCurrentPlayTime(long playTime) {
        float fraction = mDuration > 0 ? (float) playTime / mDuration : 1;
        setCurrentFraction(fraction);
    }

    /**
     * Sets the position of the animation to the specified fraction. This fraction should
     * be between 0 and the total fraction of the animation, including any repetition. That is,
     * a fraction of 0 will position the animation at the beginning, a value of 1 at the end,
     * and a value of 2 at the end of a reversing animator that repeats once. If
     * the animation has not yet been started, then it will not advance forward after it is
     * set to this fraction; it will simply set the fraction to this value and perform any
     * appropriate actions based on that fraction. If the animation is already running, then
     * setCurrentFraction() will set the current fraction to this value and continue
     * playing from that point. {@link Animator.AnimatorListener} events are not called
     * due to changing the fraction; those events are only processed while the animation
     * is running.
     *
     * @param fraction The fraction to which the animation is advanced or rewound. Values
     * outside the range of 0 to the maximum fraction for the animator will be clamped to
     * the correct range.
     */
    public:void setCurrentFraction(float fraction) {
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

    /**
     * Calculates current iteration based on the overall fraction. The overall fraction will be
     * in the range of [0, mRepeatCount + 1]. Both current iteration and fraction in the current
     * iteration can be derived from it.
     */
    private:int getCurrentIteration(float fraction) {
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

    /**
     * Calculates the fraction of the current iteration, taking into account whether the animation
     * should be played backwards. E.g. When the animation is played backwards in an iteration,
     * the fraction for that iteration will go from 1.f to 0.f.
     */
    private:float getCurrentIterationFraction(float fraction, bool inReverse) {
        fraction = clampFraction(fraction);
        int iteration = getCurrentIteration(fraction);
        float currentFraction = fraction - iteration;
        return shouldPlayBackward(iteration, inReverse) ? 1.f - currentFraction : currentFraction;
    }

    /**
     * Clamps fraction into the correct range: [0, mRepeatCount + 1]. If repeat count is infinite,
     * no upper bound will be set for the fraction.
     *
     * @param fraction fraction to be clamped
     * @return fraction clamped into the range of [0, mRepeatCount + 1]
     */
    private:float clampFraction(float fraction) {
        if (fraction < 0) {
            fraction = 0;
        } else if (mRepeatCount != INFINITE) {
            fraction = smin(fraction, mRepeatCount + 1);
        }
        return fraction;
    }

    /**
     * Calculates the direction of animation playing (i.e. forward or backward), based on 1)
     * whether the entire animation is being reversed, 2) repeat mode applied to the current
     * iteration.
     */
    private:bool shouldPlayBackward(int iteration, bool inReverse) {
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

    /**
     * Gets the current position of the animation in time, which is equal to the current
     * time minus the time that the animation started. An animation that is not yet started will
     * return a value of zero, unless the animation has has its play time set via
     * {@link #setCurrentPlayTime(long)} or {@link #setCurrentFraction(float)}, in which case
     * it will return the time that was set.
     *
     * @return The current position in time of the animation.
     */
    public:long getCurrentPlayTime() {
        if (!mInitialized || (!mStarted && mSeekFraction < 0)) {
            return 0;
        }
        if (mSeekFraction >= 0) {
            return (long) (mDuration * mSeekFraction);
        }
        float durationScale = sDurationScale == 0 ? 1 : sDurationScale;
        return (long) ((STime::GetCurrentTimeMs() - mStartTime) / durationScale);
    }

    /**
     * The amount of time, in milliseconds, to delay starting the animation after
     * {@link #start()} is called.
     *
     * @return the number of milliseconds to delay running the animation
     */
    public:long getStartDelay() {
        return mStartDelay;
    }

    /**
     * The amount of time, in milliseconds, to delay starting the animation after
     * {@link #start()} is called. Note that the start delay should always be non-negative. Any
     * negative start delay will be clamped to 0 on N and above.
     *
     * @param startDelay The amount of the delay, in milliseconds
     */
    public:void setStartDelay(long startDelay) {
        // Clamp start delay to non-negative range.
        if (startDelay < 0) {
            startDelay = 0;
        }
        mStartDelay = startDelay;
    }


    /**
     * Sets how many times the animation should be repeated. If the repeat
     * count is 0, the animation is never repeated. If the repeat count is
     * greater than 0 or {@link #INFINITE}, the repeat mode will be taken
     * into account. The repeat count is 0 by default.
     *
     * @param value the number of times the animation should be repeated
     */
    public:void setRepeatCount(int value) {
        mRepeatCount = value;
    }
    /**
     * Defines how many times the animation should repeat. The default value
     * is 0.
     *
     * @return the number of times the animation should repeat, or {@link #INFINITE}
     */
    public:int getRepeatCount() {
        return mRepeatCount;
    }

    /**
     * Defines what this animation should do when it reaches the end. This
     * setting is applied only when the repeat count is either greater than
     * 0 or {@link #INFINITE}. Defaults to {@link #RESTART}.
     *
     * @param value {@link #RESTART} or {@link #REVERSE}
     */
	public:void setRepeatMode(IAnimation::RepeatMode value) {
        mRepeatMode = value;
    }

    /**
     * Defines what this animation should do when it reaches the end.
     *
     * @return either one of {@link #REVERSE} or {@link #RESTART}
     */
    public:IAnimation::RepeatMode getRepeatMode() {
        return mRepeatMode;
    }

    /**
     * Adds a listener to the set of listeners that are sent update events through the life of
     * an animation. This method is called on all listeners for every frame of the animation,
     * after the values for the animation have been calculated.
     *
     * @param listener the listener to be added to the current set of listeners for this animation.
     */
    public:void addUpdateListener(IAnimatorUpdateListener * listener) {
        mUpdateListeners.Add(listener);
    }

    /**
     * Removes all listeners from the set listening to frame updates for this animation.
     */
    public:void removeAllUpdateListeners() {
        mUpdateListeners.RemoveAll();
    }

    /**
     * Removes a listener from the set listening to frame updates for this animation.
     *
     * @param listener the listener to be removed from the current set of update listeners
     * for this animation.
     */
    public:void removeUpdateListener(IAnimatorUpdateListener *listener) {
        int iFind = mUpdateListeners.Find(listener);
		if (iFind != -1) mUpdateListeners.RemoveAt(iFind);
    }


    /**
     * The time interpolator used in calculating the elapsed fraction of this animation. The
     * interpolator determines whether the animation runs with linear or non-linear motion,
     * such as acceleration and deceleration. The default value is
     * {@link android.view.animation.AccelerateDecelerateInterpolator}
     *
     * @param value the interpolator to be used by this animation. A value of <code>null</code>
     * will result in linear interpolation.
     */
    public:void setInterpolator(IInterpolator * value) {
		mInterpolator = value;
	}

    /**
     * Returns the timing interpolator that this IValueAnimator uses.
     *
     * @return The timing interpolator for this IValueAnimator.
     */
    public:IInterpolator * getInterpolator() {
        return mInterpolator;
    }

		   void addListener(IAnimatorListener * p)
		   {
			   mListeners.Add(p);
	}

		   void removeListener(IAnimatorListener * p)
		   {
			   int iFind = mListeners.Find(p);
			   if (iFind != -1)
			   {
				   mListeners.RemoveAt(iFind);
			   }
		   }
    private:void notifyStartListeners() {
		AddRef();
		SArray<IAnimatorListener *> tmpListeners = mListeners;
		int numListeners = tmpListeners.GetCount();
		for (int i = 0; i < numListeners; ++i) {
			tmpListeners[i]->onAnimationStart(this);
		}
		mStartListenersCalled = true;
		Release();
    }

    /**
     * Start the animation playing. This version of start() takes a bool flag that indicates
     * whether the animation should play in reverse. The flag is usually false, but may be set
     * to true if called from the reverse() method.
     *
     * <p>The animation started by calling this method will be run on the thread that called
     * this method. This thread should have a Looper on it (a runtime exception will be thrown if
     * this is not the case). Also, if the animation will animate
     * properties of objects in the view hierarchy, then the calling thread should be the UI
     * thread for that view hierarchy.</p>
     *
     * @param playBackwards Whether the IValueAnimator should start playing in reverse.
     */
    private:void start(bool playBackwards) {
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

    public:void start(ITimelineHandlersMgr *pContainer) {
		mContainer = pContainer;
        start(false);
    }

    public:void end() {
        if (!mRunning) {
            // Special case if the animation has not yet started; get it ready for ending
            startAnimation();
            mStarted = true;
        }
        animateValue(shouldPlayBackward(mRepeatCount, mReversing) ? 0.f : 1.f);
        endAnimation();
    }

    public:bool isRunning() {
        return mRunning;
    }

    public:bool isStarted() {
        return mStarted;
    }

    /**
     * Plays the IValueAnimator in reverse. If the animation is already running,
     * it will stop itself and play backwards from the point reached when reverse was called.
     * If the animation is not currently running, then it will start from the end and
     * play backwards. This behavior is only set for the current animation; future playing
     * of the animation will use the default behavior of playing forward.
     */
    public:void reverse() {
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

    /**
     * @hide
     */
    public:bool canReverse() {
        return true;
    }

    /**
     * Called internally to end an animation by removing it from the animations list. Must be
     * called on the UI thread.
     */
    private:void endAnimation() {
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

    /**
     * Called internally to start an animation by adding it to the active animations list. Must be
     * called on the UI thread.
     */
    private:void startAnimation() {
        mAnimationEndRequested = false;
        mRunning = true;
        if (mSeekFraction >= 0) {
            mOverallFraction = mSeekFraction;
        } else {
            mOverallFraction = 0.f;
        }
		notifyStartListeners();
	}

    /**
     * Internal only: This tracks whether the animation has gotten on the animation loop. Note
     * this is different than {@link #isRunning()} in that the latter tracks the time after start()
     * is called (or after start delay if any), which may be before the animation loop starts.
     */
    private:bool isPulsingInternal() {
        return mLastFrameTime >= 0;
    }

    /**
     * Applies an adjustment to the animation to compensate for jank between when
     * the animation first ran and when the frame was drawn.
     * @hide
     */
    public:void commitAnimationFrame(long frameTime) {
        if (!mStartTimeCommitted) {
            mStartTimeCommitted = true;
            uint64_t adjustment = frameTime - mLastFrameTime;
            if (adjustment > 0) {
                mStartTime += adjustment;
            }
        }
    }

    /**
     * This internal function processes a single animation frame for a given animation. The
     * currentTime parameter is the timing pulse sent by the handler, used to calculate the
     * elapsed duration, and therefore
     * the elapsed fraction, of the animation. The return value indicates whether the animation
     * should be ended (which happens when the elapsed time of the animation exceeds the
     * animation's duration, including the repeatCount).
     *
     * @param currentTime The current time, as tracked by the static timing handler
     * @return true if the animation's duration, including any repetitions due to
     * <code>repeatCount</code> has been exceeded and the animation should be ended.
     */
    bool animateBasedOnTime(uint64_t currentTime) {
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

    /**
     * Internal use only.
     *
     * This method does not modify any fields of the animation. It should be called when seeking
     * in an AnimatorSet. When the last play time and current play time are of different repeat
     * iterations,
     * {@link android.view.animation.Animation.AnimationListener#onAnimationRepeat(Animation)}
     * will be called.
     */
    void animateBasedOnPlayTime(long currentPlayTime, long lastPlayTime, bool inReverse) {
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

    /**
     * Internal use only.
     * Skips the animation value to end/start, depending on whether the play direction is forward
     * or backward.
     *
     * @param inReverse whether the end value is based on a reverse direction. If yes, this is
     *                  equivalent to skip to start value in a forward playing direction.
     */
    void skipToEndValue(bool inReverse) {
        float endFraction = inReverse ? 0.f : 1.f;
        if (mRepeatCount % 2 == 1 && mRepeatMode == IAnimation::REVERSE) {
            // This would end on fraction = 0
            endFraction = 0.f;
        }
        animateValue(endFraction);
    }

    bool isInitialized() {
        return mInitialized;
    }

    /**
     * Processes a frame of the animation, adjusting the start time if needed.
     *
     * @param frameTime The frame time.
     * @return true if the animation has ended.
     * @hide
     */
    bool doAnimationFrame(uint64_t frameTime) {
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

    /**
     * Returns the current animation fraction, which is the elapsed/interpolated fraction used in
     * the most recent frame update on the animation.
     *
     * @return Elapsed/interpolated fraction of the animation.
     */
    public:float getAnimatedFraction() const{
        return mCurrentFraction;
    }

    /**
     * This method is called with the elapsed fraction of the animation during every
     * animation frame. This function turns the elapsed fraction into an interpolated fraction
     * and then into an animated value (from the evaluator. The function is called mostly during
     * animation updates, but it is also called when the <code>end()</code>
     * function is called, to set the value on the property.
     *
     * <p>Overrides of this method must call the superclass to perform the calculation
     * of the animated value.</p>
     *
     * @param fraction The elapsed fraction of the animation.
     */
    void animateValue(float fraction) {
        fraction = mInterpolator->getInterpolation(fraction);
        mCurrentFraction = fraction;
		onEvaluateValue(fraction);
		int numListeners = mUpdateListeners.GetCount();
		for (int i = 0; i < numListeners; ++i) {
			mUpdateListeners[i]->onAnimationUpdate(this);
		}
    }

	virtual void onEvaluateValue(float fraction) = 0;

	virtual void OnNextFrame() override
	{
		doAnimationFrame(STime::GetCurrentTimeMs());
	}

	private:
		void removeAnimationCallback()
		{
			mContainer->UnregisterTimelineHandler(this);
		}
		void addAnimationCallback()
		{
			mContainer->RegisterTimelineHandler(this);
		}
};

template<class T>
class SOUI_EXP TValueAnimator : public TObjRefImpl<SObjectImpl<IValueAnimator> >
{
	SOUI_CLASS_NAME_EX(TValueAnimator<T>, L"valueAnimator", ValueAnimator)
protected:
	TypeEvaluator<T> mValueEvaluator;
	T				 mValue;

public:
	TValueAnimator(T from, T to) : mValueEvaluator(from, to) {

	}

protected:
	void onEvaluateValue(float fraction)
	{
		mValue = mValueEvaluator.evaluate(fraction);
	}

public:
	SOUI_ATTRS_BEGIN()
		ATTR_INT(L"duration",mDuration,FALSE)
		ATTR_INT(L"repeatCount",mRepeatCount,FALSE)
		ATTR_ENUM_BEGIN(L"repeatMode",IAnimation::RepeatMode,FALSE)
			ATTR_ENUM_VALUE(L"reverse",IAnimation::REVERSE)
			ATTR_ENUM_VALUE(L"restart", IAnimation::RESTART)
		ATTR_ENUM_END(mRepeatMode)
		ATTR_INTERPOLATOR(L"interpolator",mInterpolator,FALSE)
		ATTR_CHAIN_PTR(mInterpolator,0)
	SOUI_ATTRS_END()
};

class SOUI_EXP SIntAnimator : public TValueAnimator<int> {
	SOUI_CLASS_NAME(SIntAnimator, L"IntAnimator")
public:
	SIntAnimator():TValueAnimator<int>(0,1)
	{
	}

	int getValue() const
	{
		return mValue;
	}

public:
	SOUI_ATTRS_BEGIN()
		ATTR_INT(L"valueFrom",mValueEvaluator.mStart,FALSE)
		ATTR_INT(L"valueTo", mValueEvaluator.mEnd, FALSE)
	SOUI_ATTRS_END()
};

class SOUI_EXP SFloatAnimator : public TValueAnimator<float> {
	SOUI_CLASS_NAME(SFloatAnimator, L"FloatAnimator")
public:
	SFloatAnimator() :TValueAnimator<float>(0.f, 1.f)
	{
	}
	float getValue() const
	{
		return mValue;
	}

public:
	SOUI_ATTRS_BEGIN()
		ATTR_FLOAT(L"valueFrom", mValueEvaluator.mStart, FALSE)
		ATTR_FLOAT(L"valueTo", mValueEvaluator.mEnd, FALSE)
	SOUI_ATTRS_END()
};

class SOUI_EXP SColorAnimator : public TValueAnimator<COLORREF> {
	SOUI_CLASS_NAME(SColorAnimator, L"ColorAnimator")
public:
	SColorAnimator():TValueAnimator<COLORREF>(0, 0)
	{

	}
	COLORREF getValue() const
	{
		return mValue;
	}
protected:
	HRESULT OnAttrFrom(const SStringW & strValue, BOOL bLoading)
	{
		COLORREF cr = GETCOLOR(strValue);
		mValueEvaluator.setStart(cr);
		return S_FALSE;
	}
	HRESULT OnAttrTo(const SStringW & strValue, BOOL bLoading)
	{
		COLORREF cr = GETCOLOR(strValue);
		mValueEvaluator.setEnd(cr);
		return S_FALSE;
	}

	SOUI_ATTRS_BEGIN()
		ATTR_CUSTOM(L"valueFrom", OnAttrFrom)
		ATTR_CUSTOM(L"valueTo", OnAttrTo)
	SOUI_ATTRS_END()
};


}
