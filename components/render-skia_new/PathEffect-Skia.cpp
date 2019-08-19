#include "stdafx.h"
#include "PathEffect-Skia.h"

namespace SOUI
{
#ifndef SKIA_NEW
	SPathEffect_Corner::SPathEffect_Corner() :m_skPathEffect(NULL)
	{
	}
#else 
	SPathEffect_Corner::SPathEffect_Corner()
	{
	}
#endif // !SKIA_NEW	
	SPathEffect_Corner::~SPathEffect_Corner()
	{
#ifndef SKIA_NEW
		if (m_skPathEffect) m_skPathEffect->unref();
#endif // !SKIA_NEW	
	}

	void SPathEffect_Corner::Init(float radius)
	{
#ifdef SKIA_NEW
		m_skPathEffect = SkCornerPathEffect::Make(radius);
#else
		if (m_skPathEffect)
		{
			m_skPathEffect->unref();
			m_skPathEffect = NULL;
		}
		m_skPathEffect = SkCornerPathEffect::Create(radius);
#endif
	}

	void* SPathEffect_Corner::GetRealPathEffect()
	{
#ifndef SKIA_NEW
		if (m_skPathEffect) m_skPathEffect->unref();
#endif // !SKIA_NEW	
	}


	//////////////////////////////////////////////////////////////////////////
#ifndef SKIA_NEW
	SPathEffect_Dash::SPathEffect_Dash() :m_skPathEffect(NULL)
	{

	}
#else
	SPathEffect_Dash::SPathEffect_Dash(){}
#endif // !SKIA_NEW

	SPathEffect_Dash::~SPathEffect_Dash()
	{
#ifndef SKIA_NEW
		if (m_skPathEffect) m_skPathEffect->unref();
#endif // !SKIA_NEW		
	}


	void SPathEffect_Dash::Init(float intervals[], int count, float phase)
	{
#ifdef SKIA_NEW
		m_skPathEffect = SkDashPathEffect::Make(intervals, count, phase);
#else
		if (m_skPathEffect)
		{
			m_skPathEffect->unref();
			m_skPathEffect = NULL;
		}
		m_skPathEffect = SkDashPathEffect::Create(intervals, count, phase).get();
#endif
	}

	void* SPathEffect_Dash::GetRealPathEffect()
	{
#ifdef SKIA_NEW
		return m_skPathEffect.get();
#else
		return m_skPathEffect;
#endif
	}


	//////////////////////////////////////////////////////////////////////////

	void* SPathEffect_Discrete::GetRealPathEffect()
	{

#ifdef SKIA_NEW
		return m_skPathEffect.get();
#else
		return m_skPathEffect;
#endif
	}

	void SPathEffect_Discrete::Init(float segmentLength, float deviation)
	{
#ifdef SKIA_NEW
		m_skPathEffect = SkDiscretePathEffect::Make(segmentLength, deviation);
#else
		if (m_skPathEffect)
		{
			m_skPathEffect->unref();
			m_skPathEffect = NULL;
		}
		m_skPathEffect = SkDiscretePathEffect::Create(segmentLength, deviation).get();
#endif
	}

	SPathEffect_Discrete::~SPathEffect_Discrete()
	{
#ifndef SKIA_NEW
		if (m_skPathEffect) m_skPathEffect->unref();
#endif // !SKIA_NEW	
	}
#ifndef SKIA_NEW
	SPathEffect_Discrete::SPathEffect_Discrete() :m_skPathEffect(NULL)
	{

	}
#else
	SPathEffect_Discrete::SPathEffect_Discrete() {}

#endif // !SKIA_NEW	
}