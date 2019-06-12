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
		virtual const IInterpolator * GetScrollInterpolator() const = 0;
		virtual int  GetScrollFadeFrames() const = 0;
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

		const IInterpolator * GetInterpolator() const;

		BYTE GetAlpha() const;

		int GetFadeStep() const;

		DWORD GetPartState(int iPart) const;
	protected:
		virtual void OnNextFrame();

	private:
		enum FADEMODE{
			FADEOUT=-1,
			FADE_STOP =0,
			FADEIN =1,
		};

		IScrollBarHost * m_pCB;
		bool m_bVert;
		int	 m_iFrame;
		FADEMODE m_fadeMode;
		int	 m_iHitPart;
		int  m_iClickPart;
		CPoint m_ptClick;
		CPoint m_ptCursor;
	};


}
