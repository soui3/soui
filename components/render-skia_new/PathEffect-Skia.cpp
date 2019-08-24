#include "stdafx.h"
#include "PathEffect-Skia.h"

namespace SOUI
{
	SPathEffect_Corner::SPathEffect_Corner()
	{
	}
	SPathEffect_Corner::~SPathEffect_Corner()
	{
	}

	void SPathEffect_Corner::Init(float radius)
	{
		m_skPathEffect = SkCornerPathEffect::Make(radius);
	}

	void* SPathEffect_Corner::GetRealPathEffect()
	{
		return nullptr;
	}


	//////////////////////////////////////////////////////////////////////////

	SPathEffect_Dash::SPathEffect_Dash()
	{
	}

	SPathEffect_Dash::~SPathEffect_Dash()
	{		
	}


	void SPathEffect_Dash::Init(float intervals[], int count, float phase)
	{
		m_skPathEffect = SkDashPathEffect::Make(intervals, count, phase);
	}

	void* SPathEffect_Dash::GetRealPathEffect()
	{
		return m_skPathEffect.get();
	}


	//////////////////////////////////////////////////////////////////////////

	void* SPathEffect_Discrete::GetRealPathEffect()
	{
		return m_skPathEffect.get();
	}

	void SPathEffect_Discrete::Init(float segmentLength, float deviation)
	{
		m_skPathEffect = SkDiscretePathEffect::Make(segmentLength, deviation);
	}

	SPathEffect_Discrete::~SPathEffect_Discrete()
	{
	}

	SPathEffect_Discrete::SPathEffect_Discrete() {}
}