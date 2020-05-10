#pragma once

#include "interface/sinterpolator-i.h"
#include <unknown/obj-ref-impl.hpp>

SNSBEGIN

class SOUI_EXP SInterpolatorBase : public TObjRefImpl<SObjectImpl<IInterpolator>>
{
	SOUI_CLASS_NAME_EX(SInterpolatorBase,L"interpolator_base",Interpolator)
};

class SOUI_EXP SLinearInterpolator: public SInterpolatorBase
{
	SOUI_CLASS_NAME(SLinearInterpolator,L"Linear")
public:
	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST OVERRIDE;
};

class SOUI_EXP SAccelerateInterpolator: public SInterpolatorBase
{
	SOUI_CLASS_NAME(SAccelerateInterpolator,L"Accelerate")

private: 
	float mFactor;
	double mDoubleFactor;


	/**
	* Constructor
	*
	* @param factor Degree to which the animation should be eased. Seting
	*        factor to 1.0f produces a y=x^2 parabola. Increasing factor above
	*        1.0f  exaggerates the ease-in effect (i.e., it starts even
	*        slower and ends evens faster)
	*/
public: 
	SAccelerateInterpolator(float factor=1.0f);

	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST OVERRIDE;

	SOUI_ATTRS_BEGIN()
		ATTR_FLOAT(L"factor",mFactor,FALSE)
	SOUI_ATTRS_END()
};

class SOUI_EXP SDecelerateInterpolator: public SInterpolatorBase {
	SOUI_CLASS_NAME(SDecelerateInterpolator,L"Decelerate")

private: 
	float mFactor;

	/**
	* Constructor
	*
	* @param factor Degree to which the animation should be eased. Setting factor to 1.0f produces
	*        an upside-down y=x^2 parabola. Increasing factor above 1.0f makes exaggerates the
	*        ease-out effect (i.e., it starts even faster and ends evens slower)
	*/
public: 
	SDecelerateInterpolator(float factor=1.0f);

	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST OVERRIDE;

	SOUI_ATTRS_BEGIN()
		ATTR_FLOAT(L"factor",mFactor,FALSE)
	SOUI_ATTRS_END()
};

class SOUI_EXP SAccelerateDecelerateInterpolator: public SInterpolatorBase
{
	SOUI_CLASS_NAME(SAccelerateDecelerateInterpolator,L"AccelerateDecelerate")
public:
	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST OVERRIDE;
};

class SOUI_EXP SAnticipateInterpolator: public SInterpolatorBase {
	SOUI_CLASS_NAME(SAnticipateInterpolator,L"Anticipate")

private: 
	float mTension;

public: 
	SAnticipateInterpolator();

	/**
	* @param tension Amount of anticipation. When tension equals 0.0f, there is
	*                no anticipation and the interpolator becomes a simple
	*                acceleration interpolator.
	*/
	SAnticipateInterpolator(float tension);


	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST OVERRIDE;

	SOUI_ATTRS_BEGIN()
		ATTR_FLOAT(L"tension",mTension,FALSE)
	SOUI_ATTRS_END()
};

class SOUI_EXP SAnticipateOvershootInterpolator : public SInterpolatorBase{
	SOUI_CLASS_NAME(SAnticipateOvershootInterpolator,L"AnticipateOvershoot")
private:
	float mTension;
	float mExtraTension;

	/**
	* @param tension Amount of anticipation/overshoot. When tension equals 0.0f,
	*                there is no anticipation/overshoot and the interpolator becomes
	*                a simple acceleration/deceleration interpolator.
	* @param extraTension Amount by which to multiply the tension. For instance,
	*                     to get the same overshoot as an OvershootInterpolator with
	*                     a tension of 2.0f, you would use an extraTension of 1.5f.
	*/
public: 
	SAnticipateOvershootInterpolator(float tension=2.0f, float extraTension=1.5f);
private: 
	static float a(float t, float s);
	static float o(float t, float s);
	float getTensionMulti() const{
		return mTension*mExtraTension;
	}
public:
	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST OVERRIDE;

	SOUI_ATTRS_BEGIN()
		ATTR_FLOAT(L"tension",mTension,FALSE)
		ATTR_FLOAT(L"extraTension",mExtraTension,FALSE)
	SOUI_ATTRS_END()
};


class SOUI_EXP SBounceInterpolator : public SInterpolatorBase{
	SOUI_CLASS_NAME(SBounceInterpolator,L"Bounce")

private: 
	static float bounce(float t);

public: 
	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST OVERRIDE;
};


class SOUI_EXP SCycleInterpolator : public SInterpolatorBase{
	SOUI_CLASS_NAME(SCycleInterpolator,L"Cycle")

private: 
	float mCycles;

public: 
	SCycleInterpolator(float cycles=1.0f);

	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST OVERRIDE;

	SOUI_ATTRS_BEGIN()
		ATTR_FLOAT(L"cycles",mCycles,FALSE)
	SOUI_ATTRS_END()

};

class SOUI_EXP SOvershootInterpolator: public SInterpolatorBase{
	SOUI_CLASS_NAME(SOvershootInterpolator,L"Overshoot")

private:  
	float mTension;


	/**
	* @param tension Amount of overshoot. When tension equals 0.0f, there is
	*                no overshoot and the interpolator becomes a simple
	*                deceleration interpolator.
	*/
public: 
	SOvershootInterpolator(float tension=2.0f);


	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST OVERRIDE;

	SOUI_ATTRS_BEGIN()
		ATTR_FLOAT(L"tension",mTension,FALSE)
	SOUI_ATTRS_END()

};

class SPathInterpolator : public SInterpolatorBase {
	SOUI_CLASS_NAME(SPathInterpolator, L"path")

private:
	SArray<fPoint> mPt;
public:
	/**
	* Create an interpolator for an arbitrary <code>Path</code>. The <code>Path</code>
	* must begin at <code>(0, 0)</code> and end at <code>(1, 1)</code>.
	*
	* @param path The <code>Path</code> to use to make the line representing the interpolator.
	*/
	SPathInterpolator();

	/**
	* Create an interpolator for a quadratic Bezier curve. The end points
	* <code>(0, 0)</code> and <code>(1, 1)</code> are assumed.
	*
	* @param controlX The x coordinate of the quadratic Bezier control point.
	* @param controlY The y coordinate of the quadratic Bezier control point.
	*/
	SPathInterpolator(float controlX, float controlY);

	/**
	* Create an interpolator for a cubic Bezier curve.  The end points
	* <code>(0, 0)</code> and <code>(1, 1)</code> are assumed.
	*
	* @param controlX1 The x coordinate of the first control point of the cubic Bezier.
	* @param controlY1 The y coordinate of the first control point of the cubic Bezier.
	* @param controlX2 The x coordinate of the second control point of the cubic Bezier.
	* @param controlY2 The y coordinate of the second control point of the cubic Bezier.
	*/
	SPathInterpolator(float controlX1, float controlY1, float controlX2, float controlY2);

private:
	void initQuad(float controlX, float controlY);

	void initCubic(float x1, float y1, float x2, float y2);

	void initPath(IPath * path);

public:
	/**
	* Using the line in the Path in this interpolator that can be described as
	* <code>y = f(x)</code>, finds the y coordinate of the line given <code>t</code>
	* as the x coordinate. Values less than 0 will always return 0 and values greater
	* than 1 will always return 1.
	*
	* @param t Treated as the x coordinate along the line.
	* @return The y coordinate of the Path along the line where x = <code>t</code>.
	* @see Interpolator#getInterpolation(float)
	*/
	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST OVERRIDE;

protected:
	STDMETHOD_(void,OnInitFinished)(THIS_ IXmlNode * xmlNode) OVERRIDE;
};

SNSEND