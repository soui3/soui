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
#include <animation/STransformation.h>
#include <interface/sobject-i.h>

SNSBEGIN

interface IAnimation;

#undef INTERFACE
#define INTERFACE IAnimationListener
DECLARE_INTERFACE(IAnimationListener)
{
	/**
	* <p>Notifies the start of the animation.</p>
	*
	* @param animation The started animation.
	*/
	STDMETHOD_(void,onAnimationStart)(THIS_ IAnimation * animation) PURE;

	/**
	* <p>Notifies the end of the animation. This callback is not invoked
	* for animations with repeat count set to INFINITE.</p>
	*
	* @param animation The animation which reached its end.
	*/
	STDMETHOD_(void,onAnimationEnd)(THIS_ IAnimation * animation) PURE;

	/**
	* <p>Notifies the repetition of the animation.</p>
	*
	* @param animation The animation which was repeated.
	*/
	STDMETHOD_(void,onAnimationRepeat)(THIS_ IAnimation * animation) PURE;
};


#undef INTERFACE
#define INTERFACE IAnimation
DECLARE_INTERFACE_(IAnimation,IObject)
{
		//!添加引用
	/*!
	*/
	STDMETHOD_(long,AddRef) (THIS) PURE;

	//!释放引用
	/*!
	*/
	STDMETHOD_(long,Release) (THIS) PURE;

	//!释放对象
	/*!
	*/
	STDMETHOD_(void,OnFinalRelease) (THIS) PURE;

	/**
	* IsClass
	* @brief    判断this是不是属于指定的类型
	* @param    LPCWSTR lpszName --  测试类型名
	* @return   BOOL -- true是测试类型
	* Describe  
	*/    
	STDMETHOD_(BOOL,IsClass)(THIS_ LPCWSTR lpszName) SCONST PURE;

	/**
	* GetObjectClass
	* @brief    获得类型名
	* @return   LPCWSTR -- 类型名
	* Describe  这是一个虚函数，注意与GetClassName的区别。
	*/    
	STDMETHOD_(LPCWSTR,GetObjectClass)(THIS_) SCONST PURE;

	/**
	* GetObjectType
	* @brief    获得对象类型
	* @return   int -- 对象类型
	* Describe  这是一个虚函数，注意与GetClassType的区别。
	*/    
	STDMETHOD_(int,GetObjectType)(THIS)  SCONST PURE;

	/**
	* GetID
	* @brief    获取对象ID
	* @return   int -- 对象ID
	* Describe  
	*/    
	STDMETHOD_(int,GetID)(THIS) SCONST PURE;

	/**
	* GetName
	* @brief    获取对象Name
	* @return   LPCWSTR -- 对象Name
	* Describe  
	*/    
	STDMETHOD_(LPCWSTR,GetName)(THIS) SCONST PURE;

	/**
	* InitFromXml
	* @brief    从XML结节初始化SObject对象
	* @param    pugi::xml_node --  XML结节
	* @return   BOOL -- 成功返回TRUE
	* Describe  
	*/    
	STDMETHOD_(BOOL,InitFromXml)(THIS_ IXmlNode * xmlNode ) PURE;

	/**
	* OnInitFinished
	* @brief    属性初始化完成处理接口
	* @param    pugi::xml_node xmlNode --  属性节点
	* @return   void
	* Describe  
	*/    
	STDMETHOD_(void,OnInitFinished)(THIS_ IXmlNode* xmlNode) PURE;

	/**
	* SetAttributeA
	* @brief    设置一个对象属性
	* @param    const IStringA * strAttribName --  属性名
	* @param    const IStringA * strValue --  属性值
	* @param    BOOL bLoading --  对象创建时由系统调用标志
	* @return   HRESULT -- 处理处理结果
	* Describe  
	*/    
	STDMETHOD_(HRESULT,SetAttributeA)(THIS_ const IStringA * strAttribName, const IStringA *  strValue, BOOL bLoading) PURE;

	/**
	* SetAttributeW
	* @brief    设置一个对象属性
	* @param    const IStringA *strAttribName --  属性名
	* @param    const IStringA *strValue --  属性值
	* @param    BOOL bLoading --  对象创建时由系统调用标志
	* @return   HRESULT -- 处理处理结果
	* Describe  
	*/    
	STDMETHOD_(HRESULT,SetAttributeW)(THIS_ const IStringW *  strAttribName, const IStringW *  strValue, BOOL bLoading) PURE;

	/**
	* SetAttribute
	* @brief    设置一个对象属性
	* @param    LPCWSTR pszAttr --  属性名
	* @param    LPCWSTR pszValue --  属性值
	* @param    BOOL bLoading --  对象创建时由系统调用标志
	* @return   HRESULT -- 处理处理结果
	* Describe  
	*/    
	STDMETHOD_(HRESULT,SetAttribute)(THIS_ LPCWSTR pszAttr, LPCWSTR pszValue, BOOL bLoading) PURE;

	/**
	* GetAttribute
	* @brief    通过属性名查询属性值
	* @param    const SStringW & strAttr --  属性名
	* @param    IStringW * pValue -- 属性值
	* @return   BOOL, TRUE:获取成功，FALSE:获取失败，属性不存在
	* Describe  默认返回空
	*/    
	STDMETHOD_(BOOL,GetAttribute)(THIS_ const IStringW * strAttr, IStringW * pValue) SCONST PURE;

	/**
	* OnAttribute
	* @brief    属性处理后调用的方法
	* @param    const SStringW & strAttribName --  属性名
	* @param    const SStringW & strValue --  属性名
	* @param    BOOL bLoading --  对象创建时由系统调用标志
	* @param    HRESULT hr --  属性处理结果
	* Describe  不做处理，直接返回
	*/    
	STDMETHOD_(HRESULT,AfterAttribute)(THIS_ const IStringW * strAttribName,const IStringW * strValue, BOOL bLoading, HRESULT hr) PURE;

	/**
	* DefAttributeProc
	* @brief    默认属性处理函数
	* @param    const SStringW & strAttribName --  属性名
	* @param	 const SStringW & strValue --属性值
	* @param    BOOL bLoading -- 从XML初始化标志
	* @return   HRESULT -- S_OK:刷新UI， S_FALSE:成功但不刷新UI，其它：失败
	* Describe  在SetAttribute中没有处理一个属性时转到本方法处理。
	*/  
	STDMETHOD_(HRESULT,DefAttributeProc)(THIS_ const IStringW * strAttribName,const IStringW * strValue, BOOL bLoading) PURE;

	/**
	* <p>An animation listener receives notifications from an animation.
	* Notifications indicate animation related events, such as the end or the
	* repetition of the animation.</p>
	*/
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
		ZORDER_NORMAL =0,
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

	STDMETHOD_(IAnimation *,clone)(THIS) SCONST PURE;

	STDMETHOD_(void,copy)(THIS_ const IAnimation *src) PURE;

	/**
	* Reset the initialization state of this animation.
	*
	* @see #initialize(int, int, int, int)
	*/
	STDMETHOD_(void,reset)(THIS) PURE;

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
	STDMETHOD_(void,cancel)(THIS) PURE;

	/**
	* Sets the acceleration curve for this animation. Defaults to a linear
	* interpolation.
	*
	* @param i The interpolator which defines the acceleration curve
	* @attr ref android.R.styleable#Animation_interpolator
	*/
	STDMETHOD_(void,setInterpolator)(THIS_ IInterpolator *i) PURE;

	/**
	* How long this animation should last. The duration cannot be negative.
	*
	* @param durationMillis Duration in milliseconds
	*
	* @throws java.lang.IllegalArgumentException if the duration is < 0
	*
	* @attr ref android.R.styleable#Animation_duration
	*/
	STDMETHOD_(void,setDuration)(THIS_ long durationMillis) PURE;

	/**
	* How much to scale the duration by.
	*
	* @param scale The amount to scale the duration.
	*/
	STDMETHOD_(void,scaleCurrentDuration)(THIS_ float scale) PURE;

	STDMETHOD_(void,setFillBefore)(THIS_ bool bFill) PURE;

	STDMETHOD_(void,setFillAfter)(THIS_ bool bFill) PURE;

	STDMETHOD_(bool,getFillBefore)(THIS) SCONST PURE;

	STDMETHOD_(bool,getFillAfter)(THIS) SCONST PURE;

	STDMETHOD_(void,setStartOffset)(THIS_ long offset) PURE;

	/**
	* If fillEnabled is true, this animation will apply the value of fillBefore.
	*
	* @return true if the animation will take fillBefore into account
	* @attr ref android.R.styleable#Animation_fillEnabled
	*/
	STDMETHOD_(bool,isFillEnabled)(THIS) SCONST PURE;

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
	STDMETHOD_(void,setFillEnabled)(THIS_ bool fillEnabled) PURE;

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
	STDMETHOD_(void,setStartTime)(THIS_ int64_t startTimeMillis) PURE;

	/**
	* Convenience method to start the animation the first time
	* {@link #getTransformation(long, STransformation)} is invoked.
	*/
	STDMETHOD_(void,start)(THIS) PURE;

	/**
	* Convenience method to start the animation at the current time in
	* milliseconds.
	*/
	STDMETHOD_(void,startNow)(THIS) PURE;

	/**
	* Defines what this animation should do when it reaches the end. This
	* setting is applied only when the repeat count is either greater than
	* 0 or {@link #INFINITE}. Defaults to {@link #RESTART}.
	*
	* @param repeatMode {@link #RESTART} or {@link #REVERSE}
	* @attr ref android.R.styleable#Animation_repeatMode
	*/
	STDMETHOD_(void,setRepeatMode)(THIS_ RepeatMode repeatMode) PURE;

	/**
	* Sets how many times the animation should be repeated. If the repeat
	* count is 0, the animation is never repeated. If the repeat count is
	* greater than 0 or {@link #INFINITE}, the repeat mode will be taken
	* into account. The repeat count is 0 by default.
	*
	* @param repeatCount the number of times the animation should be repeated
	* @attr ref android.R.styleable#Animation_repeatCount
	*/
	STDMETHOD_(void,setRepeatCount)(THIS_ int repeatCount) PURE;


	/**
	* Set the Z ordering mode to use while running the animation.
	*
	* @param zAdjustment The desired mode, one of {@link #ZORDER_NORMAL},
	* {@link #ZORDER_TOP}, or {@link #ZORDER_BOTTOM}.
	* @attr ref android.R.styleable#Animation_zAdjustment
	*/
	STDMETHOD_(void,setZAdjustment)(THIS_ ZAdjustment zAdjustment) PURE;


	/**
	* Gets the acceleration curve type for this animation.
	*
	* @return the {@link Interpolator} associated to this animation
	* @attr ref android.R.styleable#Animation_interpolator
	*/
	STDMETHOD_(IInterpolator*,getInterpolator)(THIS) SCONST PURE;

	/**
	* When this animation should start. If the animation has not startet yet,
	* this method might return {@link #START_ON_FIRST_FRAME}.
	*
	* @return the time in milliseconds when the animation should start or
	*         {@link #START_ON_FIRST_FRAME}
	*/
	STDMETHOD_(int64_t,getStartTime)(THIS) SCONST PURE;

	/**
	* How long this animation should last
	*
	* @return the duration in milliseconds of the animation
	* @attr ref android.R.styleable#Animation_duration
	*/
	STDMETHOD_(long,getDuration)(THIS) SCONST PURE;

	/**
	* When this animation should start, relative to StartTime
	*
	* @return the start offset in milliseconds
	* @attr ref android.R.styleable#Animation_startOffset
	*/
	STDMETHOD_(long,getStartOffset)(THIS) SCONST PURE;

	/**
	* Defines what this animation should do when it reaches the end.
	*
	* @return either one of {@link #REVERSE} or {@link #RESTART}
	* @attr ref android.R.styleable#Animation_repeatMode
	*/
	STDMETHOD_(RepeatMode,getRepeatMode)(THIS) SCONST PURE;

	/**
	* Defines how many times the animation should repeat. The default value
	* is 0.
	*
	* @return the number of times the animation should repeat, or {@link #INFINITE}
	* @attr ref android.R.styleable#Animation_repeatCount
	*/
	STDMETHOD_(int,getRepeatCount)(THIS) SCONST PURE;

	/**
	* Returns the Z ordering mode to use while running the animation as
	* previously set by {@link #setZAdjustment}.
	*
	* @return Returns one of {@link #ZORDER_NORMAL},
	* {@link #ZORDER_TOP}, or {@link #ZORDER_BOTTOM}.
	* @attr ref android.R.styleable#Animation_zAdjustment
	*/
	STDMETHOD_(ZAdjustment,getZAdjustment)(THIS) SCONST PURE;


	/**
	* <p>Binds an animation listener to this animation. The animation listener
	* is notified of animation events such as the end of the animation or the
	* repetition of the animation.</p>
	*
	* @param listener the animation listener to be notified
	*/
	STDMETHOD_(void,setAnimationListener)(THIS_ IAnimationListener* listener) PURE;

	/**
	* Compute a hint at how long the entire animation may last, in milliseconds.
	* Animations can be written to cause themselves to run for a different
	* duration than what is computed here, but generally this should be
	* accurate.
	*/
	STDMETHOD_(long,computeDurationHint)(THIS) SCONST PURE;
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
	STDMETHOD_(bool,getTransformation)(THIS_ uint64_t currentTime, STransformation & outTransformation,
		float scale) PURE;

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
	STDMETHOD_(bool,getTransformation)(THIS_ int64_t currentTime, STransformation & outTransformation) PURE;


	/**
	* <p>Indicates whether this animation has started or not.</p>
	*
	* @return true if the animation has started, false otherwise
	*/
	STDMETHOD_(bool,hasStarted)(THIS) SCONST PURE;

	/**
	* <p>Indicates whether this animation has ended or not.</p>
	*
	* @return true if the animation has ended, false otherwise
	*/
	STDMETHOD_(bool,hasEnded)(THIS) SCONST PURE;

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
	STDMETHOD_(void,applyTransformation)(THIS_ float interpolatedTime, STransformation & t) PURE;

	/**
	* Return true if this animation changes the view's alpha property.
	*
	* @hide
	*/
	STDMETHOD_(bool,hasAlpha)(THIS) SCONST PURE;

	STDMETHOD_(void,initialize)(THIS_ int width, int height, int parentWidth, int parentHeight) PURE;

	STDMETHOD_(void,setUserData)(THIS_ ULONG_PTR data) PURE;

	STDMETHOD_(ULONG_PTR,getUserData)(THIS) SCONST PURE;
};


SNSEND