#pragma once
namespace SOUI{

	SEVENT_BEGIN(EventPath,EVT_EXTERNAL_BEGIN+23451)
		float fLength;
	SEVENT_END()

	class SPathView : public SWindow
	{
		SOUI_CLASS_NAME(SPathView,L"pathView")
	public:
		SPathView(void);
		~SPathView(void);

		void AddPoint(const POINT *pts,int nPts);

		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"lineWidth",m_nLineWidth,TRUE)
			ATTR_INT(L"lineStyle",m_nLineStyle,TRUE)
			ATTR_COLOR(L"lineColor",m_crLine,TRUE)
			ATTR_INT(L"frameSize",m_nFrameSize,TRUE)
			ATTR_FLOAT(L"cornerRadius",m_fCornerRadius,TRUE)
		SOUI_ATTRS_END()
	protected:
		void OnLButtonDown(UINT nFlags,CPoint pt);
		void OnRButtonDown(UINT nFlags, CPoint point);
		void OnPaint(IRenderTarget *pRT);
		void OnPathChanged();
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_RBUTTONDOWN(OnRButtonDown)
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()
	protected:
		int m_nLineWidth;
		int m_nLineStyle;
		COLORREF m_crLine;
		int m_nFrameSize;
		float m_fCornerRadius;

		SArray<POINT> m_lstPoints;
		SAutoRefPtr<IPath> m_path;
		SAutoRefPtr<IPathMeasure> m_pathMeasure;
	};


}
