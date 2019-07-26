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
#include "TypeEvaluator.h"

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
		uint64_t mStartTime;

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
		float mSeekFraction;

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
		float mOverallFraction ;

		/**
		* Tracks current elapsed/eased fraction, for querying in getAnimatedFraction().
		* This is calculated by interpolating the fraction (range: [0, 1]) in the current iteration.
		*/
		float mCurrentFraction ;

		/**
		* Tracks the time (in milliseconds) when the last frame arrived.
		*/
		uint64_t mLastFrameTime;

		/**
		* Tracks the time (in milliseconds) when the first frame arrived. Note the frame may arrive
		* during the start delay.
		*/
		uint64_t mFirstFrameTime ;

		/**
		* Additional playing state to indicate whether an animator has been start()'d. There is
		* some lag between a call to start() and the first animation frame. We should still note
		* that the animation has been started, even if it's first animation frame has not yet
		* happened, and reflect that state in isRunning().
		* Note that delayed animations are different: they are not started until their first
		* animation frame, which occurs after their delay elapses.
		*/
		bool mRunning ;

		/**
		* Additional playing state to indicate whether an animator has been start()'d, whether or
		* not there is a nonzero startDelay.
		*/
		bool mStarted;

		/**
		* Tracks whether we've notified listeners of the onAnimationStart() event. This can be
		* complex to keep track of since we notify listeners at different times depending on
		* startDelay and whether start() was called before end().
		*/
		bool mStartListenersCalled ;

		/**
		* Flag that denotes whether the animation is set up and ready to go. Used to
		* set up animation that has not yet been started.
		*/
		bool mInitialized;

		/**
		* Flag that tracks whether animation has been requested to end.
		*/
		bool mAnimationEndRequested ;

		// How long the animation should last in ms
		long mDuration ;

		// The amount of time in ms to delay starting the animation after start() is called. Note
		// that this start delay is unscaled. When there is a duration scale set on the animator, the
		// scaling factor will be applied to this delay.
		long mStartDelay;

		// The number of times the animation will repeat. The default is 0, which means the animation
		// will play only once
		int mRepeatCount ;


		float sDurationScale;
		/**
		* The type of repetition that will occur when repeatMode is nonzero. RESTART means the
		* animation will start from the beginning on every new cycle. REVERSE means the animation
		* will reverse directions on each iteration.
		*/
		IAnimation::RepeatMode mRepeatMode;


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

	public:
		/**
		* Creates a new IValueAnimator object. This default constructor is primarily for
		* use internally; the factory methods which take parameters are more generally
		* useful.
		*/
		IValueAnimator();

		/**
		* Sets the length of the animation. The default duration is 300 milliseconds.
		*
		* @param duration The length of the animation, in milliseconds. This value cannot
		* be negative.
		* @return IValueAnimator The object called with setDuration(). This return
		* value makes it easier to compose statements together that construct and then set the
		* duration, as in <code>IValueAnimator.ofInt(0, 10).setDuration(500).start()</code>.
		*/
		void setDuration(long duration);

	private:
		long getScaledDuration();

	public:
		/**
		* Gets the length of the animation. The default duration is 300 milliseconds.
		*
		* @return The length of the animation, in milliseconds.
		*/
		long getDuration();

		long getTotalDuration();

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
		void setCurrentPlayTime(long playTime);

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
		void setCurrentFraction(float fraction);

	private:
		/**
		* Calculates current iteration based on the overall fraction. The overall fraction will be
		* in the range of [0, mRepeatCount + 1]. Both current iteration and fraction in the current
		* iteration can be derived from it.
		*/
		int getCurrentIteration(float fraction);

		/**
		* Calculates the fraction of the current iteration, taking into account whether the animation
		* should be played backwards. E.g. When the animation is played backwards in an iteration,
		* the fraction for that iteration will go from 1.f to 0.f.
		*/
		float getCurrentIterationFraction(float fraction, bool inReverse);

		/**
		* Clamps fraction into the correct range: [0, mRepeatCount + 1]. If repeat count is infinite,
		* no upper bound will be set for the fraction.
		*
		* @param fraction fraction to be clamped
		* @return fraction clamped into the range of [0, mRepeatCount + 1]
		*/
		float clampFraction(float fraction);

		/**
		* Calculates the direction of animation playing (i.e. forward or backward), based on 1)
		* whether the entire animation is being reversed, 2) repeat mode applied to the current
		* iteration.
		*/
		bool shouldPlayBackward(int iteration, bool inReverse);

	public:
		/**
		* Gets the current position of the animation in time, which is equal to the current
		* time minus the time that the animation started. An animation that is not yet started will
		* return a value of zero, unless the animation has has its play time set via
		* {@link #setCurrentPlayTime(long)} or {@link #setCurrentFraction(float)}, in which case
		* it will return the time that was set.
		*
		* @return The current position in time of the animation.
		*/
		long getCurrentPlayTime();

		/**
		* The amount of time, in milliseconds, to delay starting the animation after
		* {@link #start()} is called.
		*
		* @return the number of milliseconds to delay running the animation
		*/
		long getStartDelay();

		/**
		* The amount of time, in milliseconds, to delay starting the animation after
		* {@link #start()} is called. Note that the start delay should always be non-negative. Any
		* negative start delay will be clamped to 0 on N and above.
		*
		* @param startDelay The amount of the delay, in milliseconds
		*/
		void setStartDelay(long startDelay);


		/**
		* Sets how many times the animation should be repeated. If the repeat
		* count is 0, the animation is never repeated. If the repeat count is
		* greater than 0 or {@link #INFINITE}, the repeat mode will be taken
		* into account. The repeat count is 0 by default.
		*
		* @param value the number of times the animation should be repeated
		*/
		void setRepeatCount(int value);
		/**
		* Defines how many times the animation should repeat. The default value
		* is 0.
		*
		* @return the number of times the animation should repeat, or {@link #INFINITE}
		*/
		int getRepeatCount();

		/**
		* Defines what this animation should do when it reaches the end. This
		* setting is applied only when the repeat count is either greater than
		* 0 or {@link #INFINITE}. Defaults to {@link #RESTART}.
		*
		* @param value {@link #RESTART} or {@link #REVERSE}
		*/
		void setRepeatMode(IAnimation::RepeatMode value);

		/**
		* Defines what this animation should do when it reaches the end.
		*
		* @return either one of {@link #REVERSE} or {@link #RESTART}
		*/
		IAnimation::RepeatMode getRepeatMode();

		/**
		* Adds a listener to the set of listeners that are sent update events through the life of
		* an animation. This method is called on all listeners for every frame of the animation,
		* after the values for the animation have been calculated.
		*
		* @param listener the listener to be added to the current set of listeners for this animation.
		*/
		void addUpdateListener(IAnimatorUpdateListener * listener);

		/**
		* Removes all listeners from the set listening to frame updates for this animation.
		*/
		void removeAllUpdateListeners();

		/**
		* Removes a listener from the set listening to frame updates for this animation.
		*
		* @param listener the listener to be removed from the current set of update listeners
		* for this animation.
		*/
		void removeUpdateListener(IAnimatorUpdateListener *listener);


		/**
		* The time interpolator used in calculating the elapsed fraction of this animation. The
		* interpolator determines whether the animation runs with linear or non-linear motion,
		* such as acceleration and deceleration. The default value is
		* {@link android.view.animation.AccelerateDecelerateInterpolator}
		*
		* @param value the interpolator to be used by this animation. A value of <code>null</code>
		* will result in linear interpolation.
		*/
		void setInterpolator(IInterpolator * value);

		/**
		* Returns the timing interpolator that this IValueAnimator uses.
		*
		* @return The timing interpolator for this IValueAnimator.
		*/
		IInterpolator * getInterpolator();

		void addListener(IAnimatorListener * p);

		void removeListener(IAnimatorListener * p);
	private:
		void notifyStartListeners();

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
		void start(bool playBackwards);

	public:
		void start(ITimelineHandlersMgr *pContainer);

		void end();

		bool isRunning();

		bool isStarted();

		/**
		* Plays the IValueAnimator in reverse. If the animation is already running,
		* it will stop itself and play backwards from the point reached when reverse was called.
		* If the animation is not currently running, then it will start from the end and
		* play backwards. This behavior is only set for the current animation; future playing
		* of the animation will use the default behavior of playing forward.
		*/
		void reverse();

		/**
		* @hide
		*/
		bool canReverse();

	private:
		/**
		* Called internally to end an animation by removing it from the animations list. Must be
		* called on the UI thread.
		*/
		void endAnimation();

		/**
		* Called internally to start an animation by adding it to the active animations list. Must be
		* called on the UI thread.
		*/
		void startAnimation();

		/**
		* Internal only: This tracks whether the animation has gotten on the animation loop. Note
		* this is different than {@link #isRunning()} in that the latter tracks the time after start()
		* is called (or after start delay if any), which may be before the animation loop starts.
		*/
		bool isPulsingInternal();

	public:
		/**
		* Applies an adjustment to the animation to compensate for jank between when
		* the animation first ran and when the frame was drawn.
		* @hide
		*/
		void commitAnimationFrame(long frameTime);

	private:
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
		bool animateBasedOnTime(uint64_t currentTime);

		/**
		* Internal use only.
		*
		* This method does not modify any fields of the animation. It should be called when seeking
		* in an AnimatorSet. When the last play time and current play time are of different repeat
		* iterations,
		* {@link android.view.animation.Animation.AnimationListener#onAnimationRepeat(Animation)}
		* will be called.
		*/
		void animateBasedOnPlayTime(long currentPlayTime, long lastPlayTime, bool inReverse);

		/**
		* Internal use only.
		* Skips the animation value to end/start, depending on whether the play direction is forward
		* or backward.
		*
		* @param inReverse whether the end value is based on a reverse direction. If yes, this is
		*                  equivalent to skip to start value in a forward playing direction.
		*/
		void skipToEndValue(bool inReverse);

		bool isInitialized();

		/**
		* Processes a frame of the animation, adjusting the start time if needed.
		*
		* @param frameTime The frame time.
		* @return true if the animation has ended.
		* @hide
		*/
		bool doAnimationFrame(uint64_t frameTime);

	public:
		/**
		* Returns the current animation fraction, which is the elapsed/interpolated fraction used in
		* the most recent frame update on the animation.
		*
		* @return Elapsed/interpolated fraction of the animation.
		*/
		float getAnimatedFraction() const;

	private:
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
		void animateValue(float fraction);

		void removeAnimationCallback();
		void addAnimationCallback();
	protected:
		virtual void onEvaluateValue(float fraction) = 0;

		virtual void OnNextFrame() override;
	};


	template<class T>
	class  TValueAnimator : public TObjRefImpl<SObjectImpl<IValueAnimator> >
	{
		SOUI_CLASS_NAME_EX(TValueAnimator<T>, L"valueAnimator", ValueAnimator)
	protected:
		TypeEvaluator<T> mValueEvaluator;
		T				 mValue;

		int		mID;
		SStringW mName;
	public:
		TValueAnimator(T from, T to) : mValueEvaluator(from, to) {

		}

		LPCWSTR GetName() const
		{
			return mName;
		}

		int GetID() const
		{
			return mID;
		}

	protected:
		void onEvaluateValue(float fraction)
		{
			mValue = mValueEvaluator.evaluate(fraction);
		}

	public:
		SOUI_ATTRS_BEGIN()
			ATTR_STRINGW(L"name",mName,FALSE)
			ATTR_INT(L"id",mID,FALSE)
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
