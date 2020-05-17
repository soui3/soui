#pragma once

#include "interface/stooltip-i.h"
#include "core/SNativeWnd.h"

namespace SOUI
{
    class STipCtrl : public IToolTip
                   , public SNativeWnd 
    {
    public:
        STipCtrl(void);
        virtual ~STipCtrl(void);

        BOOL Create();

	public://IToolTip
		STDMETHOD_(BOOL,PreTranslateMessage)(THIS_ MSG* pMsg) OVERRIDE;

		STDMETHOD_(void,UpdateTip)(THIS_ const TIPID *id, RECT rc,LPCTSTR pszTip, int nScale) OVERRIDE;

		STDMETHOD_(void,ClearTip)(THIS) OVERRIDE;

		STDMETHOD_(void,RelayEvent)(THIS_ const MSG *pMsg) OVERRIDE;
	protected:
        virtual void OnFinalMessage(HWND hWnd);

        void OnTimer(UINT_PTR idEvent);
        void OnPaint(HDC dc);
        void ShowTip(BOOL bShow);

        BEGIN_MSG_MAP_EX(STipCtrl)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_TIMER(OnTimer)
            REFLECT_NOTIFICATIONS_EX()
        END_MSG_MAP()

    protected:
        int            m_nDelay;
        int            m_nShowSpan;
        SStringT       m_strTip;
        int            m_nScale;
        CRect          m_rcTarget;
        HFONT          m_font;

        TIPID          m_id;
    };
}

