#pragma once

#include <interface/sinterpolator-i.h>
namespace SOUI
{
	class SInterpolatorView : public SSliderBar
		,protected ITimelineHandler
	{
		SOUI_CLASS_NAME(SInterpolatorView,L"interpolatorView")
	public:
		SInterpolatorView(void);
		~SInterpolatorView(void);

		void SetInterpolator(IInterpolator *pInterpolator);
	public:
		int OnCreate(void*);
		void OnDestroy();
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_CREATE(OnCreate)
			MSG_WM_DESTROY(OnDestroy)
		SOUI_MSG_MAP_END()
	protected:
		STDMETHOD_(void,OnNextFrame)(THIS_) OVERRIDE;
		
		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"speed",m_nSpeed,FALSE)
		SOUI_ATTRS_END()
		
		SAutoRefPtr<IInterpolator> m_interpolator;
		int			m_nSpeed;
		int			m_iFrame;
		float		m_fTime;
	};
}
