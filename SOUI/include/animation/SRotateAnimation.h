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

#include <animation/SAnimation.h>


SNSBEGIN
/**
* An animation that controls the rotation of an object. This rotation takes
* place in the X-Y plane. You can specify the point to use for the center of
* the rotation, where (0,0) is the top left point. If not specified, (0,0) is
* the default rotation point.
*
*/
class SOUI_EXP SRotateAnimation : public SAnimation {
	SOUI_CLASS_NAME(SRotateAnimation, L"rotate")

private:
	float mFromDegrees;
	float mToDegrees;

	ValueType mPivotXType;
	ValueType mPivotYType;
	float mPivotXValue;
	float mPivotYValue;

	float mPivotX;
	float mPivotY;


public:

	/**
	* Constructor to use when building a RotateAnimation from code.
	* Default pivotX/pivotY point is (0,0).
	*
	* @param fromDegrees Rotation offset to apply at the start of the
	*        animation.
	*
	* @param toDegrees Rotation offset to apply at the end of the animation.
	*/
	SRotateAnimation();

	void init(float fromDegrees, float toDegrees, float pivotX=0.0f, float pivotY=0.0f);

	/**
	* Constructor to use when building a RotateAnimation from code
	*
	* @param fromDegrees Rotation offset to apply at the start of the
	*        animation.
	*
	* @param toDegrees Rotation offset to apply at the end of the animation.
	*
	* @param pivotXType Specifies how pivotXValue should be interpreted. One of
	*        Animation.ABSOLUTE, Animation.RELATIVE_TO_SELF, or
	*        Animation.RELATIVE_TO_PARENT.
	* @param pivotXValue The X coordinate of the point about which the object
	*        is being rotated, specified as an absolute number where 0 is the
	*        left edge. This value can either be an absolute number if
	*        pivotXType is ABSOLUTE, or a percentage (where 1.0 is 100%)
	*        otherwise.
	* @param pivotYType Specifies how pivotYValue should be interpreted. One of
	*        Animation.ABSOLUTE, Animation.RELATIVE_TO_SELF, or
	*        Animation.RELATIVE_TO_PARENT.
	* @param pivotYValue The Y coordinate of the point about which the object
	*        is being rotated, specified as an absolute number where 0 is the
	*        top edge. This value can either be an absolute number if
	*        pivotYType is ABSOLUTE, or a percentage (where 1.0 is 100%)
	*        otherwise.
	*/
	void init(float fromDegrees, float toDegrees, ValueType pivotXType, float pivotXValue,
		ValueType pivotYType, float pivotYValue);

	STDMETHOD_(void,initialize)(THIS_ int width, int height, int parentWidth, int parentHeight) OVERRIDE;

	/**
	* Called at the end of constructor methods to initialize, if possible, values for
	* the pivot point. This is only possible for ABSOLUTE pivot values.
	*/
private:
	void initializePivotPoint();

protected:
	STDMETHOD_(void,applyTransformation)(THIS_ float interpolatedTime, STransformation & t) OVERRIDE;

	STDMETHOD_(void,copy)(THIS_ const IAnimation *src) OVERRIDE;
public:
	SOUI_ATTRS_BEGIN()
		ATTR_FLOAT(L"fromDegrees", mFromDegrees, FALSE)
		ATTR_FLOAT(L"toDegrees", mToDegrees, FALSE)
		ATTR_VALUE_DESC(L"pivotX", mPivotXType, mPivotXValue)
		ATTR_VALUE_DESC(L"pivotY", mPivotYType, mPivotYValue)
	SOUI_ATTRS_END()
};

SNSEND