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

#pragma once
#include <animation/Animation.h>
#include <souicoll.h>

/**
* Represents a group of Animations that should be played together.
* The transformation of each individual animation are composed 
* together into a single transform. 
* If AnimationSet sets any properties that its children also set
* (for example, duration or fillBefore), the values of AnimationSet
* override the child values.
*
* <p>The way that AnimationSet inherits behavior from Animation is important to
* understand. Some of the Animation attributes applied to AnimationSet affect the
* AnimationSet itself, some are pushed down to the children, and some are ignored,
* as follows:
* <ul>
*     <li>duration, repeatMode, fillBefore, fillAfter: These properties, when set
*     on an AnimationSet object, will be pushed down to all child animations.</li>
*     <li>repeatCount, fillEnabled: These properties are ignored for AnimationSet.</li>
*     <li>startOffset, shareInterpolator: These properties apply to the AnimationSet itself.</li>
* </ul>
* Starting with {@link android.os.Build.VERSION_CODES#ICE_CREAM_SANDWICH},
* the behavior of these properties is the same in XML resources and at runtime (prior to that
* release, the values set in XML were ignored for AnimationSet). That is, calling
* <code>setDuration(500)</code> on an AnimationSet has the same effect as declaring
* <code>android:duration="500"</code> in an XML resource for an AnimationSet object.</p>
*/

namespace SOUI{

	class SOUI_EXP SAnimationSet : public SAnimation {
		SOUI_CLASS_NAME(SAnimationSet,L"set")
		enum{
			PROPERTY_FILL_AFTER_MASK         = 0x1,
			PROPERTY_FILL_BEFORE_MASK        = 0x2,
			PROPERTY_REPEAT_MODE_MASK        = 0x4,
			PROPERTY_START_OFFSET_MASK       = 0x8,
			PROPERTY_SHARE_INTERPOLATOR_MASK = 0x10,
			PROPERTY_DURATION_MASK           = 0x20,
			PROPERTY_MORPH_MATRIX_MASK       = 0x40,
			PROPERTY_CHANGE_BOUNDS_MASK      = 0x80,
		};

	private:
		int mFlags;
		bool mDirty;
		bool mHasAlpha;

		SArray<SAutoRefPtr<IAnimation> > mAnimations;
		SArray<int> mStoredOffsets;
		long mLastEnd;

		/**
		* Constructor to use when building an AnimationSet from code
		* 
		* @param shareInterpolator Pass true if all of the animations in this set
		*        should use the interpolator associated with this AnimationSet.
		*        Pass false if each animation should use its own interpolator.
		*/
	public: SAnimationSet(bool shareInterpolator=true) {
				setFlag(PROPERTY_SHARE_INTERPOLATOR_MASK, shareInterpolator);
				init();
			}

	protected:
		void copy(const IAnimation *src)
		{
			SAnimation::copy(src);
			const SAnimationSet * src2 = sobj_cast<const SAnimationSet>(src);
			mAnimations.Copy(src2->mAnimations);
			mFlags = src2->mFlags;
		}


	private: void setFlag(int mask, bool value) {
				 if (value) {
					 mFlags |= mask;
				 } else {
					 mFlags &= ~mask;
				 }
			 }

	private: void init() {
				 mFlags = 0;
			 }

	public: void setFillAfter(bool fillAfter) {
				mFlags |= PROPERTY_FILL_AFTER_MASK;
				SAnimation::setFillAfter(fillAfter);
			}

	public: void setFillBefore(bool fillBefore) {
				mFlags |= PROPERTY_FILL_BEFORE_MASK;
				SAnimation::setFillBefore(fillBefore);
			}

	public: void setRepeatMode(RepeatMode repeatMode) {
				mFlags |= PROPERTY_REPEAT_MODE_MASK;
				SAnimation::setRepeatMode(repeatMode);
			}

	public: void setStartOffset(long startOffset) {
				mFlags |= PROPERTY_START_OFFSET_MASK;
				SAnimation::setStartOffset(startOffset);
			}

			/**
			* @hide
			*/
	public: bool hasAlpha() {
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

			/**
			* <p>Sets the duration of every child animation.</p>
			*
			* @param durationMillis the duration of the animation, in milliseconds, for
			*        every child in this set
			*/
	public: void setDuration(long durationMillis) {
				mFlags |= PROPERTY_DURATION_MASK;
				SAnimation::setDuration(durationMillis);
				mLastEnd = mStartOffset + mDuration;
			}

			/**
			* Add a child animation to this animation set.
			* The transforms of the child animations are applied in the order
			* that they were added
			* @param a Animation to add.
			*/
	public: void addAnimation(IAnimation  *a) {
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

			/**
			* Sets the start time of this animation and all child animations
			* 
			* @see android.view.animation.Animation#setStartTime(long)
			*/
	public: void setStartTime(int64_t startTimeMillis) {
				SAnimation::setStartTime(startTimeMillis);

				int count = mAnimations.GetCount();

				for (int i = 0; i < count; i++) {
					mAnimations[i]->setStartTime(startTimeMillis);
				}
			}

	public: int64_t getStartTime() const{
				int64_t startTime = STime::GetCurrentTimeMs()+10000000;

				int count = mAnimations.GetCount();

				for (int i = 0; i < count; i++) {
					IAnimation *a = mAnimations.GetAt(i);
					startTime = smin(startTime, a->getStartTime());
				}

				return startTime;
			}

			/**
			* The duration of an AnimationSet is defined to be the 
			* duration of the longest child animation.
			* 
			* @see android.view.animation.Animation#getDuration()
			*/
	public: long getDuration() {
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

			/**
			* The duration hint of an animation set is the maximum of the duration
			* hints of all of its component animations.
			* 
			* @see android.view.animation.Animation#computeDurationHint
			*/
	public: long computeDurationHint() {
				long duration = 0;
				int count = mAnimations.GetCount();
				for (int i = count - 1; i >= 0; --i) {
					long d = mAnimations[i]->computeDurationHint();
					if (d > duration) duration = d;
				}
				return duration;
			}



			/**
			* The transformation of an animation set is the concatenation of all of its
			* component animations.
			* 
			* @see android.view.animation.Animation#getTransformation
			*/
	public: bool getTransformation(int64_t currentTime, Transformation &t) {
				int count = mAnimations.GetCount();

				bool more = false;
				bool started = false;
				bool ended = true;

				t.clear();

				for (int i = count - 1; i >= 0; --i) {
					IAnimation * a = mAnimations[i];

					Transformation temp;
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

			/**
			* @see android.view.animation.Animation#scaleCurrentDuration(float)
			*/
	public: void scaleCurrentDuration(float scale) {
				int count = mAnimations.GetCount();
				for (int i = 0; i < count; i++) {
					mAnimations[i]->scaleCurrentDuration(scale);
				}
			}

			/**
			* @see android.view.animation.Animation#initialize(int, int, int, int)
			*/
	public: void initialize(int width, int height, int parentWidth, int parentHeight) {
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

	public: void reset() {
				SAnimation::reset();
				restoreChildrenStartOffset();
			}

			/**
			* @hide
			*/
			void restoreChildrenStartOffset() {
				if (mStoredOffsets.GetCount() != mAnimations.GetCount())
					return;

				for (UINT i = 0; i < mAnimations.GetCount(); i++) {
					mAnimations[i]->setStartOffset(mStoredOffsets[i]);
				}
			}

			protected:
				BOOL InitFromXml(pugi::xml_node xmlNode)
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

				SOUI_ATTRS_BEGIN()
					ATTR_BIT(L"shareInterpolator",mFlags, PROPERTY_SHARE_INTERPOLATOR_MASK,FALSE)
				SOUI_ATTRS_END()
	};

}
