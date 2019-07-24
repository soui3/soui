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

#include <animation/Animation.h>

namespace SOUI {
	/**
	 * An animation that controls the position of an object. See the
	 * {@link android.view.animation full package} description for details and
	 * sample code.
	 *
	 */
	class SOUI_EXP STranslateAnimation : public SAnimation {
		SOUI_CLASS_NAME(STranslateAnimation,L"translate")

	private:
		ValueType mFromXType;
		ValueType mToXType;

		ValueType mFromYType;
		ValueType mToYType;
	protected:
		/** @hide */
		float mFromXValue;
		/** @hide */
		float mToXValue;

		/** @hide */
		float mFromYValue;
		/** @hide */
		float mToYValue;

		/** @hide */
		float mFromXDelta;
		/** @hide */
		float mToXDelta;
		/** @hide */
		float mFromYDelta;
		/** @hide */
		float mToYDelta;
		/**
		 * Constructor to use when building a TranslateAnimation from code
		 *
		 * @param fromXDelta Change in X coordinate to apply at the start of the
		 *        animation
		 * @param toXDelta Change in X coordinate to apply at the end of the
		 *        animation
		 * @param fromYDelta Change in Y coordinate to apply at the start of the
		 *        animation
		 * @param toYDelta Change in Y coordinate to apply at the end of the
		 *        animation
		 */
		public: STranslateAnimation(float fromXDelta=0.0f, float toXDelta=0.0f, float fromYDelta=0.0f, float toYDelta=0.0f) {
			mFromXValue = fromXDelta;
			mToXValue = toXDelta;
			mFromYValue = fromYDelta;
			mToYValue = toYDelta;

			mFromXType = ABSOLUTE_VALUE;
			mToXType = ABSOLUTE_VALUE;
			mFromYType = ABSOLUTE_VALUE;
			mToYType = ABSOLUTE_VALUE;
		}

		/**
		 * Constructor to use when building a TranslateAnimation from code
		 *
		 * @param fromXType Specifies how fromXValue should be interpreted. One of
		 *        Animation.ABSOLUTE, Animation.RELATIVE_TO_SELF, or
		 *        Animation.RELATIVE_TO_PARENT.
		 * @param fromXValue Change in X coordinate to apply at the start of the
		 *        animation. This value can either be an absolute number if fromXType
		 *        is ABSOLUTE, or a percentage (where 1.0 is 100%) otherwise.
		 * @param toXType Specifies how toXValue should be interpreted. One of
		 *        Animation.ABSOLUTE, Animation.RELATIVE_TO_SELF, or
		 *        Animation.RELATIVE_TO_PARENT.
		 * @param toXValue Change in X coordinate to apply at the end of the
		 *        animation. This value can either be an absolute number if toXType
		 *        is ABSOLUTE, or a percentage (where 1.0 is 100%) otherwise.
		 * @param fromYType Specifies how fromYValue should be interpreted. One of
		 *        Animation.ABSOLUTE, Animation.RELATIVE_TO_SELF, or
		 *        Animation.RELATIVE_TO_PARENT.
		 * @param fromYValue Change in Y coordinate to apply at the start of the
		 *        animation. This value can either be an absolute number if fromYType
		 *        is ABSOLUTE, or a percentage (where 1.0 is 100%) otherwise.
		 * @param toYType Specifies how toYValue should be interpreted. One of
		 *        Animation.ABSOLUTE, Animation.RELATIVE_TO_SELF, or
		 *        Animation.RELATIVE_TO_PARENT.
		 * @param toYValue Change in Y coordinate to apply at the end of the
		 *        animation. This value can either be an absolute number if toYType
		 *        is ABSOLUTE, or a percentage (where 1.0 is 100%) otherwise.
		 */
		public: STranslateAnimation(ValueType fromXType, float fromXValue, ValueType toXType, float toXValue,
			ValueType fromYType, float fromYValue, ValueType toYType, float toYValue) {

			mFromXValue = fromXValue;
			mToXValue = toXValue;
			mFromYValue = fromYValue;
			mToYValue = toYValue;

			mFromXType = fromXType;
			mToXType = toXType;
			mFromYType = fromYType;
			mToYType = toYType;
		}


		protected: void applyTransformation(float interpolatedTime, STransformation& t) {
			float dx = mFromXDelta;
			float dy = mFromYDelta;
			if (mFromXDelta != mToXDelta) {
				dx = mFromXDelta + ((mToXDelta - mFromXDelta) * interpolatedTime);
			}
			if (mFromYDelta != mToYDelta) {
				dy = mFromYDelta + ((mToYDelta - mFromYDelta) * interpolatedTime);
			}
			t.getMatrix().setTranslate(dx, dy);
			t.setTransformationType(STransformation::TYPE_MATRIX);
		}

		public: void initialize(int width, int height, int parentWidth, int parentHeight) {
			mFromXDelta = (float)resolveSize(mFromXType, mFromXValue, width, parentWidth);
			mToXDelta = (float)resolveSize(mToXType, mToXValue, width, parentWidth);
			mFromYDelta = (float)resolveSize(mFromYType, mFromYValue, height, parentHeight);
			mToYDelta = (float)resolveSize(mToYType, mToYValue, height, parentHeight);
		}

				protected: 
					virtual void copy(const IAnimation *src) override
				{
					SAnimation::copy(src);
					const STranslateAnimation * src2 = sobj_cast<const STranslateAnimation>(src);
					mFromXType = src2->mFromXType;
					mFromXValue = src2->mFromXValue;
					mToXType = src2->mToXType;
					mToXValue = src2->mToXValue;
					mFromYType = src2->mFromYType;
					mFromYValue = src2->mFromYValue;
					mToYType = src2->mToYType;
					mToYValue = src2->mToYValue;
					}

	SOUI_ATTRS_BEGIN()
		ATTR_VALUE_DESC(L"fromXDelta", mFromXType, mFromXValue)
		ATTR_VALUE_DESC(L"toXDelta", mToXType,mToXValue)
		ATTR_VALUE_DESC(L"fromYDelta", mFromYType, mFromYValue)
		ATTR_VALUE_DESC(L"toYDelta", mToYType, mToYValue)
	SOUI_ATTRS_END()
	};

}