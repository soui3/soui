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
#include <animation/SAnimation.h>
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

SNSBEGIN

class SOUI_EXP SAnimationSet : public SAnimation {
	SOUI_CLASS_NAME(SAnimation,L"set")
	enum{
			PROPERTY_FILL_AFTER_MASK         = 0x1,
			PROPERTY_FILL_BEFORE_MASK        = 0x2,
			PROPERTY_DURATION_MASK = 0x04,
			PROPERTY_SHARE_INTERPOLATOR_MASK = 0x08,
	};

private:
	int mFlags;
	bool mDirty;
	bool mHasAlpha;

	SArray<SAutoRefPtr<IAnimation> > mAnimations;
	bool mChildStarted;
	long mLastEnd;

	/**
	* Constructor to use when building an AnimationSet from code
	* 
	* @param shareInterpolator Pass true if all of the animations in this set
	*        should use the interpolator associated with this AnimationSet.
	*        Pass false if each animation should use its own interpolator.
	*/
public: 
	SAnimationSet(bool shareInterpolator=true);
	/**
	* Add a child animation to this animation set.
	* The transforms of the child animations are applied in the order
	* that they were added
	* @param a Animation to add.
	*/
	void addAnimation(IAnimation  *a);

	bool hasAlpha();
private: 
	void setFlag(int mask, bool value);
	void init();
public: 
	STDMETHOD_(void,setFillAfter)(THIS_ bool bFill) OVERRIDE;

	STDMETHOD_(void,setFillBefore)(THIS_ bool bFill) OVERRIDE;

	/**
	* <p>Sets the duration of every child animation.</p>
	*
	* @param durationMillis the duration of the animation, in milliseconds, for
	*        every child in this set
	*/
	STDMETHOD_(void,setDuration)(THIS_ long durationMillis) OVERRIDE;


	/**
	* The duration of an AnimationSet is defined to be the 
	* duration of the longest child animation.
	* 
	* @see android.view.animation.Animation#getDuration()
	*/
	STDMETHOD_(long,getDuration)(THIS) SCONST OVERRIDE;


	/**
	* The transformation of an animation set is the concatenation of all of its
	* component animations.
	* 
	* @see android.view.animation.Animation#getTransformation
	*/
	STDMETHOD_(bool,getTransformation)(THIS_ int64_t currentTime, STransformation & outTransformation) OVERRIDE;

	/**
	* @see android.view.animation.Animation#scaleCurrentDuration(float)
	*/
	STDMETHOD_(void,scaleCurrentDuration)(THIS_ float scale) OVERRIDE;

	/**
	* @see android.view.animation.Animation#initialize(int, int, int, int)
	*/
	STDMETHOD_(void,initialize)(THIS_ int width, int height, int parentWidth, int parentHeight) OVERRIDE;

	STDMETHOD_(void,copy)(THIS_ const IAnimation *src) OVERRIDE;
protected:
	STDMETHOD_(BOOL,InitFromXml)(THIS_ IXmlNode *pNode) OVERRIDE;

	HRESULT OnAttrDuration(const SStringW & value, BOOL bLoading);
	HRESULT OnAttrFillBefore(const SStringW & value, BOOL bLoading);
	HRESULT OnAttrFillAfter(const SStringW & value, BOOL bLoading);
	HRESULT OnAttrStartOffset(const SStringW & value, BOOL bLoading);

	SOUI_ATTRS_BEGIN()
		ATTR_BIT(L"shareInterpolator", mFlags, PROPERTY_SHARE_INTERPOLATOR_MASK, FALSE)
		ATTR_CUSTOM(L"duration", OnAttrDuration)
		ATTR_CUSTOM(L"fillBefore", OnAttrFillBefore)
		ATTR_CUSTOM(L"fillAfter", OnAttrFillAfter)
		ATTR_CUSTOM(L"startOffset", OnAttrStartOffset)
	SOUI_ATTRS_END()
};

SNSEND