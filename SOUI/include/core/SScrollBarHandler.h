#pragma once

namespace SOUI
{

	struct IScrollBarHost
	{
		enum kSbConst{
			Timer_Wait = 100,
			Timer_Go = 101,
			kTime_Wait = 200,
			kTime_Go = 100,
		};

		virtual CRect GetScrollBarRect(bool bVert) const = 0;
		virtual ISkinObj* GetScrollBarSkin(bool bVert) const = 0;
		virtual const SCROLLINFO * GetScrollBarInfo(bool bVert) const = 0;
		virtual int GetScrollBarArrowSize(bool bVert) const = 0;
		virtual void UpdateScrollBar(bool bVert, int iPart) = 0;
		virtual ISwndContainer * GetScrollBarContainer() = 0;
		virtual bool IsScrollBarEnable(bool bVert) const = 0;
		virtual void OnScrollThumbTrackPos(bool bVert,int nPos) = 0;
		virtual void OnScrollCommand(bool bVert, int iCmd) = 0;
		virtual void OnScrollSetTimer(bool bVert, char id, UINT uElapse) = 0;
		virtual void OnScrollKillTimer(bool bVert, char id) = 0;
	};

	class SOUI_EXP SScrollBarHandler :  ITimelineHandler
	{
	public:
		enum {
			kSbRail = 100,
		};

		SScrollBarHandler(IScrollBarHost *pCB);

	public:
		CRect GetPartRect(int iPart) const;

		int GetHitPart() const;
		
		int GetClickPart() const;

		bool IsVertical() const;

		void SetVertical(bool bVert);

		void OnMouseHover(CPoint pt);

		void OnMouseDown(CPoint pt);

		void OnMouseUp(CPoint pt);

		void OnMouseMove(CPoint pt);

		void OnMouseLeave();

		void OnDraw(IRenderTarget *pRT, int iPart) const;

		void OnTimer(char id);
	protected:
		int HitTest(CPoint pt) const;

		ISwndContainer * GetContainer();

		BYTE GetAlpha() const;

		int GetFadeStep() const;

		DWORD GetPartState(int iPart) const;
	protected:
		virtual void OnNextFrame();

	public:
		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"speed",m_nSpeed,FALSE)
			ATTR_BOOL(L"vertical", m_bVert, TRUE)
			ATTR_INTERPOLATOR(L"interpolator", m_interpolator,FALSE)
			ATTR_CHAIN_PTR(m_interpolator,0)
		SOUI_ATTRS_BREAK()
	private:
		enum FADEMODE{
			FADEOUT=-1,
			FADE_STOP =0,
			FADEIN =1,
		};

		IScrollBarHost * m_pCB;
		bool m_bVert;
		SAutoRefPtr<IInterpolator> m_interpolator; //default to null, which means no animation.
		int  m_nSpeed;
		int	 m_iFrame;
		FADEMODE m_fadeMode;
		int	 m_iHitPart;
		int  m_iClickPart;
		CPoint m_ptClick;
		CPoint m_ptCursor;
	};


}
