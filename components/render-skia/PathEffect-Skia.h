#pragma once

#include <interface/SPathEffect-i.h>
#include <helper/obj-ref-impl.hpp>
#include <effects/SkCornerPathEffect.h>
#include <effects/SkDashPathEffect.h>
#include <effects/SkDiscretePathEffect.h>

namespace SOUI
{
	class SPathEffect_Corner : public TObjRefImpl<ICornerPathEffect>
	{
	public:
		SPathEffect_Corner();
		~SPathEffect_Corner();

		STDMETHOD_(void,Init)(THIS_ float radius) OVERRIDE;

		STDMETHOD_(void *,GetRealPathEffect)(THIS) OVERRIDE;
	private:
		SkCornerPathEffect * m_skPathEffect;
	};

	class SPathEffect_Dash : public TObjRefImpl<IDashPathEffect>
	{
	public:
		SPathEffect_Dash();
		~SPathEffect_Dash();

		STDMETHOD_(void,Init)(THIS_ float intervals[], int count, float phase) OVERRIDE;
		STDMETHOD_(void *,GetRealPathEffect)(THIS) OVERRIDE;

	private:
		SkDashPathEffect *m_skPathEffect;
	};

	class SPathEffect_Discrete : public TObjRefImpl<IDiscretePathEffect>
	{
	public:
		SPathEffect_Discrete();
		~SPathEffect_Discrete();

		STDMETHOD_(void,Init)(THIS_ float segmentLength, float deviation) OVERRIDE;

		STDMETHOD_(void *,GetRealPathEffect)(THIS) OVERRIDE;

	private:
		SkDiscretePathEffect *m_skPathEffect;
	};


}

