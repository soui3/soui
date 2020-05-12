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

#include <interface/SAnimation-i.h>
#include <helper/obj-ref-impl.hpp>
#include <animation/SInterpolatorImpl.h>
#include <animation/STransformation.h>
#include <sobject/sobject.hpp>

#define ATTR_VALUE_DESC(attribname, varType,varValue)			\
	if (0 == strAttribName.CompareNoCase(attribname))           \
	{                                                           \
	SValueDescription desc = SOUI::SValueDescription::parseValue(strValue);\
	varType=desc.type; varValue=desc.value;					\
	hRet = S_FALSE;							                \
	}                                                           \
	else                                                        \


SNSBEGIN
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
	static SValueDescription parseValue(const SStringW & value);
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
	ZAdjustment mZAdjustment;

	/**
	* scalefactor to apply to pivot points, etc. during animation. Subclasses retrieve the
	* value via getScaleFactor().
	*/
	float mScaleFactor;

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

	ULONG_PTR mUserData;

	SStringW mName;

	int      mID;
public:
	SAnimation();

	~SAnimation();

	STDMETHOD_(LPCWSTR,GetName)(THIS) SCONST OVERRIDE
	{
		return mName;
	}

	STDMETHOD_(int,GetID)(THIS) SCONST OVERRIDE
	{
		return mID;
	}

	STDMETHOD_(IAnimation *,clone)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,copy)(THIS_ const IAnimation *src) OVERRIDE;

	STDMETHOD_(void,reset)(THIS) OVERRIDE;

	STDMETHOD_(void,cancel)(THIS) OVERRIDE;

	STDMETHOD_(void,setInterpolator)(THIS_ IInterpolator *i) OVERRIDE;

	STDMETHOD_(void,setDuration)(THIS_ long durationMillis) OVERRIDE;

	STDMETHOD_(void,scaleCurrentDuration)(THIS_ float scale) OVERRIDE;

	STDMETHOD_(void,setFillBefore)(THIS_ bool bFill) OVERRIDE;

	STDMETHOD_(void,setFillAfter)(THIS_ bool bFill) OVERRIDE;

	STDMETHOD_(bool,getFillBefore)(THIS) SCONST OVERRIDE;

	STDMETHOD_(bool,getFillAfter)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,setStartOffset)(THIS_ long offset) OVERRIDE;

	STDMETHOD_(bool,isFillEnabled)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,setFillEnabled)(THIS_ bool fillEnabled) OVERRIDE;

	STDMETHOD_(void,setStartTime)(THIS_ int64_t startTimeMillis) OVERRIDE;

	STDMETHOD_(void,start)(THIS) OVERRIDE;

	STDMETHOD_(void,startNow)(THIS) OVERRIDE;

	STDMETHOD_(void,setRepeatMode)(THIS_ RepeatMode repeatMode) OVERRIDE;

	STDMETHOD_(void,setRepeatCount)(THIS_ int repeatCount) OVERRIDE;

	STDMETHOD_(void,setZAdjustment)(THIS_ ZAdjustment zAdjustment) OVERRIDE;

	STDMETHOD_(IInterpolator*,getInterpolator)(THIS) SCONST OVERRIDE;

	STDMETHOD_(int64_t,getStartTime)(THIS) SCONST OVERRIDE;

	STDMETHOD_(long,getDuration)(THIS) SCONST OVERRIDE;

	STDMETHOD_(long,getStartOffset)(THIS) SCONST OVERRIDE;

	STDMETHOD_(RepeatMode,getRepeatMode)(THIS) SCONST OVERRIDE;

	STDMETHOD_(int,getRepeatCount)(THIS) SCONST OVERRIDE;

	STDMETHOD_(ZAdjustment,getZAdjustment)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,setAnimationListener)(THIS_ IAnimationListener* listener) OVERRIDE;

	STDMETHOD_(long,computeDurationHint)(THIS) SCONST OVERRIDE;

	STDMETHOD_(bool,getTransformation)(THIS_ uint64_t currentTime, STransformation & outTransformation,
		float scale) OVERRIDE;

	STDMETHOD_(bool,getTransformation)(THIS_ int64_t currentTime, STransformation & outTransformation) OVERRIDE;

	STDMETHOD_(bool,hasStarted)(THIS) SCONST OVERRIDE;

	STDMETHOD_(bool,hasEnded)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,applyTransformation)(THIS_ float interpolatedTime, STransformation & t) OVERRIDE;

	STDMETHOD_(bool,hasAlpha)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,initialize)(THIS_ int width, int height, int parentWidth, int parentHeight) OVERRIDE;

	STDMETHOD_(void,setUserData)(THIS_ ULONG_PTR data) OVERRIDE;

	STDMETHOD_(ULONG_PTR,getUserData)(THIS) SCONST OVERRIDE;

protected:
	bool isCanceled();

	void fireAnimationStart();

	void fireAnimationRepeat();

	void fireAnimationEnd();

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
	int resolveSize(ValueType type, float value, int size, int parentSize);

	/**
	* Gurantees that this animation has an interpolator. Will use
	* a AccelerateDecelerateInterpolator is nothing else was specified.
	*/
	void ensureInterpolator();

	/**
	* The scale factor is set by the call to <code>getTransformation</code>. Overrides of
	* {@link #getTransformation(long, STransformation, float)} will get this value
	* directly. Overrides of {@link #applyTransformation(float, STransformation)} can
	* call this method to get the value.
	*
	* @return float The scale factor that should be applied to pre-scaled values in
	* an Animation such as the pivot points in {@link ScaleAnimation} and {@link RotateAnimation}.
	*/
	float getScaleFactor();
public:
	SOUI_ATTRS_BEGIN()
		ATTR_STRINGW(L"name", mName, FALSE)
		ATTR_INT(L"id", mID, FALSE)
		ATTR_INT(L"duration", mDuration, FALSE)
		ATTR_INT(L"startOffset", mStartOffset, FALSE)
		ATTR_BOOL(L"fillEnable", mFillEnabled, FALSE)
		ATTR_BOOL(L"fillBefore", mFillBefore, FALSE)
		ATTR_BOOL(L"fillAfter", mFillAfter, FALSE)
		ATTR_INT(L"repeatCount", mRepeatCount, FALSE)
		ATTR_ENUM_BEGIN(L"zAdjustment",ZAdjustment,FALSE)
		ATTR_ENUM_VALUE(L"normal", ZORDER_NORMAL)
		ATTR_ENUM_VALUE(L"top", ZORDER_TOP)
		ATTR_ENUM_VALUE(L"bottom", ZORDER_BOTTOM)
		ATTR_ENUM_END(mZAdjustment)
		ATTR_ENUM_BEGIN(L"repeatMode", RepeatMode, FALSE)
		ATTR_ENUM_VALUE(L"restart", RESTART)
		ATTR_ENUM_VALUE(L"reverse", REVERSE)
		ATTR_ENUM_END(mRepeatMode)
		ATTR_INTERPOLATOR(L"interpolator", mInterpolator, FALSE)
		ATTR_CHAIN_PTR(mInterpolator, 0)
	SOUI_ATTRS_END()
};

SNSEND