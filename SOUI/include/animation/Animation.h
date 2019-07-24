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

#include <interface/Animation-i.h>
#include <unknown/obj-ref-impl.hpp>
#include <animation/SInterpolatorImpl.h>
#include <animation/Transformation.h>
#include <sobject/sobject.hpp>

#define ATTR_VALUE_DESC(attribname, varType,varValue)			\
    if (0 == strAttribName.CompareNoCase(attribname))           \
    {                                                           \
		SValueDescription desc = SOUI::SValueDescription::parseValue(strValue);\
		varType=desc.type; varValue=desc.value;					\
        hRet = S_FALSE;							                \
    }                                                           \
    else                                                        \


namespace SOUI {

	/**
	* Utility class to parse a string description of a size.
	*/
	class SOUI_EXP SValueDescription {
	public:
		/**
		* One of Animation.ABSOLUTE_VALUE, Animation.RELATIVE_TO_SELF, or
		* Animation.RELATIVE_TO_PARENT.
		*/
		IAnimation::ValueType type;

		/**
		* The absolute or relative dimension for this Description.
		*/
		float value;

		/**
		* Size descriptions can appear inthree forms:
		* <ol>
		* <li>An absolute size. This is represented by a number.</li>
		* <li>A size relative to the size of the object being animated. This
		* is represented by a number followed by "%".</li> *
		* <li>A size relative to the size of the parent of object being
		* animated. This is represented by a number followed by "%p".</li>
		* </ol>
		* @param value The typed value to parse
		* @return The parsed version of the description
		*/
		static SValueDescription parseValue(const SStringW & value) {
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
	};

	class SOUI_EXP SAnimation : public TObjRefImpl<SObjectImpl<IAnimation>> {
		SOUI_CLASS_NAME_EX(SAnimation, L"animation", Animation)
	protected:
		/**
		* Set by {@link #getTransformation(long, STransformation)} when the animation ends.
		*/
		bool mEnded;

		/**
		* Set by {@link #getTransformation(long, STransformation)} when the animation starts.
		*/
		bool mStarted;

		/**
		* Set by {@link #getTransformation(long, STransformation)} when the animation repeats
		* in REVERSE mode.
		*/
		bool mCycleFlip;


		/**
		* The time in milliseconds at which the animation must start;
		*/
		int64_t mStartTime;

		/**
		* The delay in milliseconds after which the animation must start. When the
		* start offset is > 0, the start time of the animation is startTime + startOffset.
		*/
		long mStartOffset;

		/**
		* The duration of one animation cycle in milliseconds.
		*/
		long mDuration;

		/**
		* The number of times the animation must repeat. By default, an animation repeats
		* indefinitely.
		*/
		int mRepeatCount;

		/**
		* Indicates how many times the animation was repeated.
		*/
		int mRepeated;

		/**
		* The behavior of the animation when it repeats. The repeat mode is either
		* {@link #RESTART} or {@link #REVERSE}.
		*
		*/
		RepeatMode mRepeatMode;

		/**
		* The interpolator used by the animation to smooth the movement.
		*/
		SAutoRefPtr<IInterpolator> mInterpolator;

		/**
		* The animation listener to be notified when the animation starts, ends or repeats.
		*/
		IAnimationListener * mListener;

		/**
		* Desired Z order mode during animation.
		*/
	protected:
		int mZAdjustment;

		/**
		* Desired background color behind animation.
		*/
		int mBackgroundColor;

		/**
		* scalefactor to apply to pivot points, etc. during animation. Subclasses retrieve the
		* value via getScaleFactor().
		*/
		float mScaleFactor;


		bool mMore;
		bool mOneMoreTime;

		STransformation  mTransformation;

		/**
		* Indicates whether the animation transformation should be applied before the
		* animation starts. The value of this variable is only relevant if mFillEnabled is true;
		* otherwise it is assumed to be true.
		*/
		bool mFillBefore;

		/**
		* Indicates whether the animation transformation should be applied after the
		* animation ends.
		*/
		bool mFillAfter;

		/**
		* Indicates whether fillBefore should be taken into account.
		*/
		bool mFillEnabled;

		/**
		* Creates a new animation with a duration of 0ms, the default interpolator, with
		* fillBefore set to true and fillAfter set to false
		*/
	public:
		SAnimation();

		void copy(const IAnimation *src);

		IAnimation * clone() const;

		/**
		* Reset the initialization state of this animation.
		*
		* @see #initialize(int, int, int, int)
		*/
	public: void initialize(int width, int height, int parentWidth, int parentHeight);

	public: void reset();

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
	public: void cancel();

			/**
			* Sets the acceleration curve for this animation. Defaults to a linear
			* interpolation.
			*
			* @param i The interpolator which defines the acceleration curve
			* @attr ref android.R.styleable#Animation_interpolator
			*/
	public: void setInterpolator(IInterpolator *i);

			/**
			* How long this animation should last. The duration cannot be negative.
			*
			* @param durationMillis Duration in milliseconds
			*
			* @throws java.lang.IllegalArgumentException if the duration is < 0
			*
			* @attr ref android.R.styleable#Animation_duration
			*/
	public: void setDuration(long durationMillis);

			/**
			* How much to scale the duration by.
			*
			* @param scale The amount to scale the duration.
			*/
	public: void scaleCurrentDuration(float scale);

	public: void setFillBefore(bool bFill);
	public: bool getFillBefore() const;

	public: void setFillAfter(bool bFill);

	public: bool getFillAfter() const;

			bool isFillEnabled() const;

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
			void setFillEnabled(bool fillEnabled);
	public: void setStartOffset(long offset);
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
	public: void setStartTime(int64_t startTimeMillis);

			/**
			* Convenience method to start the animation the first time
			* {@link #getTransformation(long, STransformation)} is invoked.
			*/
	public: void start();

			/**
			* Convenience method to start the animation at the current time in
			* milliseconds.
			*/
	public: void startNow();

			/**
			* Defines what this animation should do when it reaches the end. This
			* setting is applied only when the repeat count is either greater than
			* 0 or {@link #INFINITE}. Defaults to {@link #RESTART}.
			*
			* @param repeatMode {@link #RESTART} or {@link #REVERSE}
			* @attr ref android.R.styleable#Animation_repeatMode
			*/
	public: void setRepeatMode(RepeatMode repeatMode);

			/**
			* Sets how many times the animation should be repeated. If the repeat
			* count is 0, the animation is never repeated. If the repeat count is
			* greater than 0 or {@link #INFINITE}, the repeat mode will be taken
			* into account. The repeat count is 0 by default.
			*
			* @param repeatCount the number of times the animation should be repeated
			* @attr ref android.R.styleable#Animation_repeatCount
			*/
	public: void setRepeatCount(int repeatCount);


			/**
			* Set the Z ordering mode to use while running the animation.
			*
			* @param zAdjustment The desired mode, one of {@link #ZORDER_NORMAL},
			* {@link #ZORDER_TOP}, or {@link #ZORDER_BOTTOM}.
			* @attr ref android.R.styleable#Animation_zAdjustment
			*/
	public: void setZAdjustment(int zAdjustment);

			/**
			* The scale factor is set by the call to <code>getTransformation</code>. Overrides of
			* {@link #getTransformation(long, STransformation, float)} will get this value
			* directly. Overrides of {@link #applyTransformation(float, STransformation)} can
			* call this method to get the value.
			*
			* @return float The scale factor that should be applied to pre-scaled values in
			* an Animation such as the pivot points in {@link ScaleAnimation} and {@link RotateAnimation}.
			*/
	protected: float getScaleFactor();


			   /**
			   * Gets the acceleration curve type for this animation.
			   *
			   * @return the {@link Interpolator} associated to this animation
			   * @attr ref android.R.styleable#Animation_interpolator
			   */
	public: IInterpolator* getInterpolator() const;

			/**
			* When this animation should start. If the animation has not startet yet,
			* this method might return {@link #START_ON_FIRST_FRAME}.
			*
			* @return the time in milliseconds when the animation should start or
			*         {@link #START_ON_FIRST_FRAME}
			*/
	public: int64_t getStartTime() const;

			/**
			* How long this animation should last
			*
			* @return the duration in milliseconds of the animation
			* @attr ref android.R.styleable#Animation_duration
			*/
	public: long getDuration() const;

			/**
			* When this animation should start, relative to StartTime
			*
			* @return the start offset in milliseconds
			* @attr ref android.R.styleable#Animation_startOffset
			*/
	public: long getStartOffset() const;

			/**
			* Defines what this animation should do when it reaches the end.
			*
			* @return either one of {@link #REVERSE} or {@link #RESTART}
			* @attr ref android.R.styleable#Animation_repeatMode
			*/
	public: RepeatMode getRepeatMode() const;

			/**
			* Defines how many times the animation should repeat. The default value
			* is 0.
			*
			* @return the number of times the animation should repeat, or {@link #INFINITE}
			* @attr ref android.R.styleable#Animation_repeatCount
			*/
	public: int getRepeatCount() const;

			/**
			* Returns the Z ordering mode to use while running the animation as
			* previously set by {@link #setZAdjustment}.
			*
			* @return Returns one of {@link #ZORDER_NORMAL},
			* {@link #ZORDER_TOP}, or {@link #ZORDER_BOTTOM}.
			* @attr ref android.R.styleable#Animation_zAdjustment
			*/
	public: int getZAdjustment() const;


			/**
			* <p>Binds an animation listener to this animation. The animation listener
			* is notified of animation events such as the end of the animation or the
			* repetition of the animation.</p>
			*
			* @param listener the animation listener to be notified
			*/
	public: 
		void setAnimationListener(IAnimationListener* listener);
	
			/**
			* Gurantees that this animation has an interpolator. Will use
			* a AccelerateDecelerateInterpolator is nothing else was specified.
			*/
	protected: void ensureInterpolator();

			   /**
			   * Compute a hint at how long the entire animation may last, in milliseconds.
			   * Animations can be written to cause themselves to run for a different
			   * duration than what is computed here, but generally this should be
			   * accurate.
			   */
	public: long computeDurationHint() const;
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
	public: bool getTransformation(uint64_t currentTime, STransformation & outTransformation,
		float scale);
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
	public: bool getTransformation(int64_t currentTime, STransformation & outTransformation);

	private: bool isCanceled();

	private: void fireAnimationStart();

	private: void fireAnimationRepeat();

	private: void fireAnimationEnd();

			 /**
			 * <p>Indicates whether this animation has started or not.</p>
			 *
			 * @return true if the animation has started, false otherwise
			 */
	public: bool hasStarted() const;

			/**
			* <p>Indicates whether this animation has ended or not.</p>
			*
			* @return true if the animation has ended, false otherwise
			*/
	public: bool hasEnded() const;

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
	protected:
		virtual void applyTransformation(float interpolatedTime, STransformation & t);

		/**
		* Convert the information in the description of a size to an actual
		* dimension
		*
		* @param type One of Animation.ABSOLUTE_VALUE, Animation.RELATIVE_TO_SELF, or
		*             Animation.RELATIVE_TO_PARENT.
		* @param value The dimension associated with the type parameter
		* @param size The size of the object being animated
		* @param parentSize The size of the parent of the object being animated
		* @return The dimension to use for the animation
		*/
	protected: 
	int resolveSize(ValueType type, float value, int size, int parentSize);

	public:
		/**
		* Return true if this animation changes the view's alpha property.
		*
		* @hide
		*/
		virtual bool hasAlpha() const;

	public:
		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"duration",mDuration,FALSE)
			ATTR_INT(L"startOffset",mStartOffset,FALSE)
			ATTR_BOOL(L"fillEnable",mFillEnabled,FALSE)
			ATTR_BOOL(L"fillBefore",mFillBefore,FALSE)
			ATTR_BOOL(L"fillAfter",mFillAfter,FALSE)
			ATTR_INT(L"repeatCount",mRepeatCount,FALSE)
			ATTR_ENUM_BEGIN(L"repeatMode", RepeatMode,FALSE)
				ATTR_ENUM_VALUE(L"restart", RESTART)
				ATTR_ENUM_VALUE(L"reverse", REVERSE)
			ATTR_ENUM_END(mRepeatMode)
			ATTR_INTERPOLATOR(L"interpolator", mInterpolator, FALSE)
			ATTR_CHAIN_PTR(mInterpolator,0)
		SOUI_ATTRS_END()
	};


}