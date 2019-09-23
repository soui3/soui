/*
* Copyright (C) 2006 The Android Open Source Project
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
* Abstraction for an Animation that can be applied to Views, Surfaces, or
* other objects. See the {@link android.view.animation animation package
* description file}.
*/
#pragma once

#include <interface/sinterpolator-i.h>
#include <unknown/obj-ref-i.h>
#include <animation/STransformation.h>
#include <sobject/sobject-i.h>

namespace SOUI {
	struct SOUI_EXP IAnimation : IObjRef , IObject {
		/**
		* <p>An animation listener receives notifications from an animation.
		* Notifications indicate animation related events, such as the end or the
		* repetition of the animation.</p>
		*/
		struct IAnimationListener {
			/**
			* <p>Notifies the start of the animation.</p>
			*
			* @param animation The started animation.
			*/
			virtual void onAnimationStart(IAnimation * animation) = 0;

			/**
			* <p>Notifies the end of the animation. This callback is not invoked
			* for animations with repeat count set to INFINITE.</p>
			*
			* @param animation The animation which reached its end.
			*/
			virtual void onAnimationEnd(IAnimation * animation) = 0;

			/**
			* <p>Notifies the repetition of the animation.</p>
			*
			* @param animation The animation which was repeated.
			*/
			virtual void onAnimationRepeat(IAnimation * animation) = 0;
		};

		enum RepeatMode {
			RESTART = 1,
			REVERSE = 2,
		};

		/**
		* Can be used as the start time to indicate the start time should be the current
		* time when {@link #getTransformation(long, STransformation)} is invoked for the
		* first animation frame. This can is useful for short animations.
		*/
		enum {
			START_ON_FIRST_FRAME = -1,
		};

		enum ValueType{
			/**
			* The specified dimension is an ABSOLUTE_VALUE number of pixels.
			*/
			ABSOLUTE_VALUE = 0,
			/**
			* The specified dimension holds a float and should be multiplied by the
			* height or width of the parent of the object being animated.
			*/
			RELATIVE_TO_SELF = 1,
			/**
			* The specified dimension holds a float and should be multiplied by the
			* height or width of the parent of the object being animated.
			*/
			RELATIVE_TO_PARENT = 2,
		};

		enum ZAdjustment{
			/**
			* Requests that the content being animated be kept in its current Z
			* order.
			*/
			ZORDER_NORMAL = 0,
			/**
			* Requests that the content being animated be forced on top of all other
			* content for the duration of the animation.
			*/
			ZORDER_TOP = 1,
			/**
			* Requests that the content being animated be forced under all other
			* content for the duration of the animation.
			*/
			ZORDER_BOTTOM = -1
		};

		/**
		* Reset the initialization state of this animation.
		*
		* @see #initialize(int, int, int, int)
		*/
		virtual void reset() = 0;

		/**
		* Cancel the animation. Cancelling an animation invokes the animation
		* listener, if set, to notify the end of the animation.
		*
		* If you cancel an animation manually, you must call {@link #reset()}
		* before starting the animation again.
		*
		* @see #reset()
		* @see #start()
		* @see #startNow()
		*/
		virtual void cancel() = 0;

		/**
		* Sets the acceleration curve for this animation. Defaults to a linear
		* interpolation.
		*
		* @param i The interpolator which defines the acceleration curve
		* @attr ref android.R.styleable#Animation_interpolator
		*/
		virtual void setInterpolator(IInterpolator *i) = 0;

		/**
		* How long this animation should last. The duration cannot be negative.
		*
		* @param durationMillis Duration in milliseconds
		*
		* @throws java.lang.IllegalArgumentException if the duration is < 0
		*
		* @attr ref android.R.styleable#Animation_duration
		*/
		virtual void setDuration(long durationMillis) = 0;

		/**
		* How much to scale the duration by.
		*
		* @param scale The amount to scale the duration.
		*/
		virtual void scaleCurrentDuration(float scale) = 0;

		virtual void setFillBefore(bool bFill) = 0;

		virtual void setFillAfter(bool bFill) = 0;

		virtual bool getFillBefore() const = 0;

		virtual bool getFillAfter() const = 0;

		virtual void setStartOffset(long offset) = 0;

		/**
		* If fillEnabled is true, this animation will apply the value of fillBefore.
		*
		* @return true if the animation will take fillBefore into account
		* @attr ref android.R.styleable#Animation_fillEnabled
		*/
		virtual bool isFillEnabled() const = 0;

		/**
		* If fillEnabled is true, the animation will apply the value of fillBefore.
		* Otherwise, fillBefore is ignored and the animation
		* transformation is always applied until the animation ends.
		*
		* @param fillEnabled true if the animation should take the value of fillBefore into account
		* @attr ref android.R.styleable#Animation_fillEnabled
		*
		* @see #setFillBefore(boolean)
		* @see #setFillAfter(boolean)
		*/
		virtual void setFillEnabled(bool fillEnabled) = 0;

		/**
		* When this animation should start. When the start time is set to
		* {@link #START_ON_FIRST_FRAME}, the animation will start the first time
		* {@link #getTransformation(long, STransformation)} is invoked. The time passed
		* to this method should be obtained by calling
		* {@link AnimationUtils#currentAnimationTimeMillis()} instead of
		* {@link System#currentTimeMillis()}.
		*
		* @param startTimeMillis the start time in milliseconds
		*/
		virtual void setStartTime(int64_t startTimeMillis) = 0;

		/**
		* Convenience method to start the animation the first time
		* {@link #getTransformation(long, STransformation)} is invoked.
		*/
		virtual void start() = 0;

		/**
		* Convenience method to start the animation at the current time in
		* milliseconds.
		*/
		virtual void startNow() = 0;

		/**
		* Defines what this animation should do when it reaches the end. This
		* setting is applied only when the repeat count is either greater than
		* 0 or {@link #INFINITE}. Defaults to {@link #RESTART}.
		*
		* @param repeatMode {@link #RESTART} or {@link #REVERSE}
		* @attr ref android.R.styleable#Animation_repeatMode
		*/
		virtual void setRepeatMode(RepeatMode repeatMode) = 0;

		/**
		* Sets how many times the animation should be repeated. If the repeat
		* count is 0, the animation is never repeated. If the repeat count is
		* greater than 0 or {@link #INFINITE}, the repeat mode will be taken
		* into account. The repeat count is 0 by default.
		*
		* @param repeatCount the number of times the animation should be repeated
		* @attr ref android.R.styleable#Animation_repeatCount
		*/
		virtual void setRepeatCount(int repeatCount) = 0;


		/**
		* Set the Z ordering mode to use while running the animation.
		*
		* @param zAdjustment The desired mode, one of {@link #ZORDER_NORMAL},
		* {@link #ZORDER_TOP}, or {@link #ZORDER_BOTTOM}.
		* @attr ref android.R.styleable#Animation_zAdjustment
		*/
		virtual void setZAdjustment(ZAdjustment zAdjustment) = 0;


		/**
		* Gets the acceleration curve type for this animation.
		*
		* @return the {@link Interpolator} associated to this animation
		* @attr ref android.R.styleable#Animation_interpolator
		*/
		virtual IInterpolator* getInterpolator() const = 0;

		/**
		* When this animation should start. If the animation has not startet yet,
		* this method might return {@link #START_ON_FIRST_FRAME}.
		*
		* @return the time in milliseconds when the animation should start or
		*         {@link #START_ON_FIRST_FRAME}
		*/
		virtual int64_t getStartTime() const = 0;

		/**
		* How long this animation should last
		*
		* @return the duration in milliseconds of the animation
		* @attr ref android.R.styleable#Animation_duration
		*/
		virtual long getDuration() const = 0;

		/**
		* When this animation should start, relative to StartTime
		*
		* @return the start offset in milliseconds
		* @attr ref android.R.styleable#Animation_startOffset
		*/
		virtual long getStartOffset() const = 0;

		/**
		* Defines what this animation should do when it reaches the end.
		*
		* @return either one of {@link #REVERSE} or {@link #RESTART}
		* @attr ref android.R.styleable#Animation_repeatMode
		*/
		virtual RepeatMode getRepeatMode() const = 0;

		/**
		* Defines how many times the animation should repeat. The default value
		* is 0.
		*
		* @return the number of times the animation should repeat, or {@link #INFINITE}
		* @attr ref android.R.styleable#Animation_repeatCount
		*/
		virtual int getRepeatCount() const = 0;

		/**
		* Returns the Z ordering mode to use while running the animation as
		* previously set by {@link #setZAdjustment}.
		*
		* @return Returns one of {@link #ZORDER_NORMAL},
		* {@link #ZORDER_TOP}, or {@link #ZORDER_BOTTOM}.
		* @attr ref android.R.styleable#Animation_zAdjustment
		*/
		virtual ZAdjustment getZAdjustment() const = 0;


		/**
		* <p>Binds an animation listener to this animation. The animation listener
		* is notified of animation events such as the end of the animation or the
		* repetition of the animation.</p>
		*
		* @param listener the animation listener to be notified
		*/
		virtual void setAnimationListener(IAnimationListener* listener) = 0;

		/**
		* Compute a hint at how long the entire animation may last, in milliseconds.
		* Animations can be written to cause themselves to run for a different
		* duration than what is computed here, but generally this should be
		* accurate.
		*/
		virtual long computeDurationHint() const = 0;
		/**
		 * Gets the transformation to apply at a specified point in time. Implementations of this
		 * method should always replace the specified STransformation or document they are doing
		 * otherwise.
		 *
		 * @param currentTime Where we are in the animation. This is wall clock time.
		 * @param outTransformation A transformation object that is provided by the
		 *        caller and will be filled in by the animation.
		 * @param scale Scaling factor to apply to any inputs to the transform operation, such
		 *        pivot points being rotated or scaled around.
		 * @return True if the animation is still running
		 */
		virtual bool getTransformation(uint64_t currentTime, STransformation & outTransformation,
			float scale) = 0;

		/**
		* Gets the transformation to apply at a specified point in time. Implementations of this
		* method should always replace the specified STransformation or document they are doing
		* otherwise.
		*
		* @param currentTime Where we are in the animation. This is wall clock time.
		* @param outTransformation A transformation object that is provided by the
		*        caller and will be filled in by the animation.
		* @return True if the animation is still running
		*/
		virtual bool getTransformation(int64_t currentTime, STransformation & outTransformation) = 0;


		/**
		* <p>Indicates whether this animation has started or not.</p>
		*
		* @return true if the animation has started, false otherwise
		*/
		virtual bool hasStarted() const = 0;

		/**
		* <p>Indicates whether this animation has ended or not.</p>
		*
		* @return true if the animation has ended, false otherwise
		*/
		virtual bool hasEnded() const = 0;

		/**
		* Helper for getTransformation. Subclasses should implement this to apply
		* their transforms given an interpolation value.  Implementations of this
		* method should always replace the specified STransformation or document
		* they are doing otherwise.
		*
		* @param interpolatedTime The value of the normalized time (0.0 to 1.0)
		*        after it has been run through the interpolation function.
		* @param t The STransformation object to fill in with the current
		*        transforms.
		*/
		virtual void applyTransformation(float interpolatedTime, STransformation & t) = 0;

		/**
		* Return true if this animation changes the view's alpha property.
		*
		* @hide
		*/
		virtual bool hasAlpha() const = 0;

		virtual void initialize(int width, int height, int parentWidth, int parentHeight) = 0;

		virtual void setUserData(ULONG_PTR data) = 0;

		virtual ULONG_PTR getUserData() const = 0;
	};


}