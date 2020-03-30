#include "souistd.h"
#include "control/SDateTimePicker.h"
#include "helper/STime.h"

namespace SOUI
{
	SDateTimePicker::SDateTimePicker()
		: m_pDropDownWnd(NULL)
		, m_pSkinBtn(GETBUILTINSKIN(SKIN_SYS_DROPBTN))
		, m_eSelDateType(eDT_NULL)
		, m_nNumWidth(0)
		, m_nCharWidth(0)
		, m_pCalendar(NULL)
		, m_nDropWidth(220)
		, m_bTimeEnable(true)
		, m_wCharNum(0)
		, m_dwBtnState(WndState_Normal)
		, m_crCue(RGBA(0xcc,0xcc,0xcc,0xff))
		, m_strCue(this)
	{
		m_evtSet.addEvent(EVENTID(EventDateTimeChanged));
		m_pNcSkin = GETBUILTINSKIN(SKIN_SYS_BORDER);
		m_style.SetAttribute(L"margin", L"1", TRUE);
		m_style.SetAlign(DT_LEFT);
		m_style.SetVAlign(DT_VCENTER);

		m_bFocusable = TRUE;

		GetLocalTime(&m_sysTime);

		m_crSelBg = RGBA(0,120,215,255);
		m_crSelText = RGBA(255,255,255,255);
	}

	SDateTimePicker::~SDateTimePicker()
	{
		if(NULL != m_pCalendar)
		{
			m_pCalendar->SSendMessage(WM_DESTROY);
			delete m_pCalendar;
		}
	}

	BOOL SDateTimePicker::CreateChildren(pugi::xml_node xmlNode)
	{
		m_pCalendar = (SCalendar*)SApplication::getSingleton().CreateWindowByName(SCalendar::GetClassName());
		if (NULL == m_pCalendar)
			return FALSE;
		m_pCalendar->SetContainer(GetContainer());

		pugi::xml_node xmlCal = xmlNode.child(L"calstyle");
		if(xmlCal)
			m_pCalendar->InitFromXml(xmlCal);

		m_pCalendar->SetAttribute(L"pos", L"0,0,-0,-0", TRUE);
		m_pCalendar->SetOwner(this);    //chain notify message to SDateTimePicker

		m_pCalendar->GetEventSet()->subscribeEvent(&SDateTimePicker::OnDateChanged, this);
		m_pCalendar->GetEventSet()->subscribeEvent(&SDateTimePicker::OnDateCmd, this);
		return TRUE;
	}

	bool SDateTimePicker::OnDateChanged(EventCalendarExChanged* pEvt)
	{
		if (NULL == pEvt) return true;

		m_sysTime.wDay = pEvt->iNewDay;
		m_sysTime.wMonth = pEvt->iNewMonth;
		m_sysTime.wYear = pEvt->iNewYear;

		Invalidate();

		EventDateTimeChanged e(this);
		e.newTime = m_sysTime;
		FireEvent(e);

		return true;
	}

	bool SDateTimePicker::OnDateCmd(EventCmd* pEvt)
	{
		CloseUp();
		return true;
	}

	SWindow* SDateTimePicker::GetDropDownOwner()
	{
		return this;
	}

	void SDateTimePicker::OnCreateDropDown(SDropDownWnd* pDropDown)
	{
		m_pCalendar->SetShowType(SHOW_MONTH);
		m_pCalendar->SetDate(m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay);
		//pDropDown->SetAttribute(L"sendWheel2Hover", L"1", TRUE);
		pDropDown->InsertChild(m_pCalendar);
		pDropDown->UpdateChildrenPosition();

		m_pCalendar->SetVisible(TRUE);
		m_pCalendar->SetFocus();

		m_dwBtnState = WndState_PushDown;
		CRect rcBtn;
		GetDropBtnRect(&rcBtn);
		InvalidateRect(rcBtn); 
	}

	void SDateTimePicker::OnDestroyDropDown(SDropDownWnd* pDropDown)
	{
		pDropDown->RemoveChild(m_pCalendar);

		m_pCalendar->SetVisible(FALSE);
		m_pCalendar->SetContainer(GetContainer());

		m_dwBtnState = WndState_Normal;
		m_pDropDownWnd = NULL;
	}

	void SDateTimePicker::GetDropBtnRect(LPRECT pBtnRc, LPRECT pSkinRc)
	{
		SIZE szBtn = m_pSkinBtn->GetSkinSize();
		GetClientRect(pBtnRc);
		pBtnRc->left = pBtnRc->right - (pBtnRc->bottom - pBtnRc->top);			// 取 右边的正方形 为  按钮 区域

		if(NULL == pSkinRc)
			return ;

		int n = (pBtnRc->bottom - pBtnRc->top - szBtn.cy) / 2;			// 按钮皮肤 则 要居中

		pSkinRc->right = pBtnRc->right - n;
		pSkinRc->left = pSkinRc->right - szBtn.cx;

		pSkinRc->top = pBtnRc->top + n;
		pSkinRc->bottom = pSkinRc->top + szBtn.cy;
	}

	SStringT SDateTimePicker::ToFormatText(EnDateType eType, WORD wNum)
	{
		SStringT szNum;
		if (eType == m_eSelDateType)						// 选中项
		{
			if (eDT_Year == eType)
			{
				if(m_wCharNum < 10)
					szNum.Format(_T("%d"), wNum);
				else if (m_wCharNum < 100)
					szNum.Format(_T("%02d"), wNum);
				else if (m_wCharNum < 1000)
					szNum.Format(_T("%03d"), wNum);
				else
					szNum.Format(_T("%04d"), wNum);
			}
			else
			{
				if (m_wCharNum > 0)
					szNum.Format(_T("%d"), wNum);
				else
					szNum.Format(_T("%02d"), wNum);
			}
		}
		else
		{
			if (eDT_Year == eType)
			{
				szNum.Format(_T("%04d"), wNum);
			}
			else
			{
				szNum.Format(_T("%02d"), wNum);
			}
		}

		return szNum;
	}

	void SDateTimePicker::Draw(EnDateType eType, IRenderTarget* pRT, WORD wNum, CRect& rcText)
	{
		SStringT szNum = ToFormatText(eType, wNum);

		if(eType != m_eSelDateType)
		{
			pRT->DrawText(szNum, szNum.GetLength(), rcText, GetTextAlign());
			return ;
		}

		// 选中 的 填充 字体颜色 变 白
		pRT->FillSolidRect(rcText, m_crSelBg);

		COLORREF old = pRT->SetTextColor(m_crSelText);
		pRT->DrawText(szNum, szNum.GetLength(), rcText, GetTextAlign());
		pRT->SetTextColor(old);
	}

	void SDateTimePicker::OnPaint(IRenderTarget* pRT)
	{
		SPainter painter;

		BeforePaint(pRT, painter);

		CRect rcText;
		GetTextRect(rcText);
		if(m_sysTime.wYear==0)
		{
			COLORREF crOld = pRT->SetTextColor(m_crCue);
			pRT->DrawText(m_strCue.GetText(FALSE),m_strCue.GetText(FALSE).GetLength(),&rcText,GetTextAlign());
			pRT->SetTextColor(crOld);
		}else
		{
			if(0 == m_nNumWidth)
			{
				SIZE sz;
				pRT->MeasureText(_T("5"), 1, &sz);
				m_nNumWidth = sz.cx;
				m_nNumHeight = sz.cy;

				pRT->MeasureText(_T("-"), 1, &sz);
				m_nCharWidth = sz.cx;

			}
			rcText.OffsetRect(0, (rcText.Height() - m_nNumHeight)/2 + 1);
			rcText.bottom = rcText.top + m_nNumHeight;

			// year
			rcText.right = rcText.left + m_nNumWidth * 4;
			Draw(eDT_Year, pRT, m_sysTime.wYear, rcText);

			rcText.left = rcText.right;
			rcText.right = rcText.left + m_nCharWidth;
			pRT->DrawText(_T("-"), -1, rcText, GetTextAlign());

			//month
			rcText.left = rcText.right;
			rcText.right = rcText.left + m_nNumWidth * 2;
			Draw(eDT_Month, pRT, m_sysTime.wMonth, rcText);

			rcText.left = rcText.right;
			rcText.right = rcText.left + m_nCharWidth;
			pRT->DrawText(_T("-"), -1, rcText, GetTextAlign());

			//day
			rcText.left = rcText.right;
			rcText.right = rcText.left + m_nNumWidth * 2;
			Draw(eDT_Day, pRT, m_sysTime.wDay, rcText);

			if (m_bTimeEnable)
			{
				// hour
				rcText.left = rcText.right + m_nCharWidth;
				rcText.right = rcText.left + m_nNumWidth * 2;
				Draw(eDT_Hour, pRT, m_sysTime.wHour, rcText);

				rcText.left = rcText.right;
				rcText.right = rcText.left + m_nCharWidth;
				pRT->DrawText(_T(":"), -1, rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				// minute
				rcText.left = rcText.right;
				rcText.right = rcText.left + m_nNumWidth * 2;
				Draw(eDT_Minute, pRT, m_sysTime.wMinute, rcText);

				rcText.left = rcText.right;
				rcText.right = rcText.left + m_nCharWidth;
				pRT->DrawText(_T(":"), -1, rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				// second
				rcText.left = rcText.right;
				rcText.right = rcText.left + m_nNumWidth * 2;
				//strText.Format(_T("%02d"), m_sysTime.wSecond);
				Draw(eDT_Second, pRT, m_sysTime.wSecond, rcText);
			}
		}
		AfterPaint(pRT, painter);

		CRect rcBtn;
		CRect rcSkin;
		GetDropBtnRect(&rcBtn, &rcSkin);
		if(WndState_Hover == m_dwBtnState && m_pNcSkin)
		{
			rcBtn += CRect(1,1,1,1);
			m_pNcSkin->DrawByIndex(pRT, rcBtn, 1);
		}
		SASSERT(m_pSkinBtn);
		m_pSkinBtn->DrawByState(pRT, rcSkin, m_dwBtnState);
	}

	bool SDateTimePicker::CalcPopupRect(int nWidth, CRect& rcPopup)
	{
		CRect rcWnd = GetWindowRect();
		GetContainer()->FrameToHost(rcWnd);

		ClientToScreen(GetContainer()->GetHostHwnd(), (LPPOINT)&rcWnd);
		ClientToScreen(GetContainer()->GetHostHwnd(), ((LPPOINT)&rcWnd)+1);

		HMONITOR hMonitor = ::MonitorFromWindow(GetContainer()->GetHostHwnd(), MONITOR_DEFAULTTONULL);
		CRect rcMonitor;
		if (hMonitor)
		{
			MONITORINFO mi = {sizeof(MONITORINFO)};
			::GetMonitorInfo(hMonitor, &mi);
			rcMonitor = mi.rcMonitor;
		}
		else
		{
			rcMonitor.right = GetSystemMetrics(   SM_CXSCREEN   );   
			rcMonitor.bottom = GetSystemMetrics(   SM_CYSCREEN   );
		}

		int nHeight = nWidth * 4 / 5;

		if(rcWnd.bottom+nHeight <= rcMonitor.bottom)
		{
			rcPopup.SetRect(rcWnd.right-nWidth, rcWnd.bottom, rcWnd.right, rcWnd.bottom+nHeight);
			return true;
		}


		rcPopup.SetRect(rcWnd.right- nWidth, rcWnd.top-nHeight, rcWnd.right, rcWnd.top);
		return false;		
	}

	SDateTimePicker::EnDateType SDateTimePicker::HitTest(CPoint pt)
	{
		CRect rcText;
		GetTextRect(rcText);
		int nWidth = pt.x - rcText.left;

		EnDateType eSelType = eDT_NULL;

		if (nWidth < (4 * m_nNumWidth + m_nCharWidth))
		{
			eSelType = eDT_Year;			// year
		}
		else if (nWidth < (6 * m_nNumWidth + 2 * m_nCharWidth))
		{
			eSelType = eDT_Month;			// month
		}
		else if (!m_bTimeEnable || nWidth < (8 * m_nNumWidth + 3 * m_nCharWidth))
		{
			eSelType = eDT_Day;			// day
		}
		else if (nWidth < (10 * m_nNumWidth + 4 * m_nCharWidth))
		{
			eSelType = eDT_Hour;			// hour
		}
		else if (nWidth < (12 * m_nNumWidth + 5 * m_nCharWidth))
		{
			eSelType = eDT_Minute;			// minute
		}
		else
		{
			eSelType = eDT_Second;			// second
		}

		return eSelType;
	}

	void SDateTimePicker::SetTime(const SYSTEMTIME& sysTime)
	{
		m_sysTime = sysTime;
		Invalidate();
	}

	void SDateTimePicker::SetTime(WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute, WORD wSecond)
	{
		m_sysTime.wYear = wYear;
		m_sysTime.wMonth = wMonth;
		m_sysTime.wDay = wDay;
		m_sysTime.wHour = wHour;
		m_sysTime.wMinute = wMinute;
		m_sysTime.wSecond = wSecond;
		Invalidate();
	}

	void SDateTimePicker::GetTime(SYSTEMTIME& sysTime)
	{
		sysTime = m_sysTime;
	}

	SStringT SDateTimePicker::GetWindowText(BOOL bRawText)
	{
		(bRawText);
		SStringT szText;
		szText.Format(_T("%04d-%02d-%02d"), m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay);
		if(m_bTimeEnable)
			szText.AppendFormat(_T(" %02d:%02d:%02d"), m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond);

		return szText;
	}

	void SDateTimePicker::OnLButtonDown(UINT nFlags, CPoint pt)
	{
		__super::OnLButtonDown(nFlags,pt);
		if(WndState_PushDown == m_dwBtnState) return;

		m_wCharNum = 0;

		CRect rcBtn;
		GetDropBtnRect(&rcBtn);
		if(!rcBtn.PtInRect(pt))				// 没有点击 按钮 
		{
			__super::OnLButtonDown(nFlags, pt);

			EnDateType  eSelType = HitTest(pt);
			if(m_eSelDateType != eSelType)
			{
				m_eSelDateType = eSelType;
				Invalidate();
			}

			return ;
		}

		m_eSelDateType = eDT_NULL;
		Invalidate();

		if(NULL != m_pDropDownWnd)
			return ;

		m_pDropDownWnd = new SDropDownWnd_ComboBox(this);		// 直接 用这个 处理 wheel 滚动
		CRect rcPopup;
		SLayoutSize nWid;
		nWid.setSize((float)m_nDropWidth, SLayoutSize::dp);
		bool bDown = CalcPopupRect(nWid.toPixelSize(GetScale()), rcPopup);
		m_pDropDownWnd->Create(rcPopup, 0);

		//if(m_nAnimTime>0)
		//	m_pDropDownWnd->AnimateHostWindow(m_nAnimTime,AW_SLIDE|(bDown?AW_VER_POSITIVE:AW_VER_NEGATIVE));
		//else
		m_pDropDownWnd->SetWindowPos(HWND_TOP,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		m_pDropDownWnd->SNativeWnd::SetCapture();

	}

	void SDateTimePicker::OnMouseMove(UINT nFlags, CPoint pt)
	{
		if(WndState_PushDown == m_dwBtnState)
			return;

		__super::OnMouseHover(nFlags, pt);

		CRect rcBtn;
		GetDropBtnRect(&rcBtn);

		if(rcBtn.PtInRect(pt))
		{
			m_dwBtnState = WndState_Hover;
			InvalidateRect(rcBtn);
		}
		else if(WndState_Hover == m_dwBtnState)
		{
			m_dwBtnState = WndState_Normal;
			InvalidateRect(rcBtn);
		}
	}

	void SDateTimePicker::OnMouseLeave()
	{
		if(WndState_PushDown == m_dwBtnState) 
			return;

		if(GetState() & WndState_Hover) 
			__super::OnMouseLeave();

		if(WndState_Hover == m_dwBtnState)
		{
			m_dwBtnState = WndState_Normal;

			CRect rcBtn;
			GetDropBtnRect(&rcBtn);
			InvalidateRect(rcBtn);
		}
	}

	BOOL SDateTimePicker::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		TimeWheel(zDelta > 0);		
		return TRUE;
	}

	void SDateTimePicker::TimeWheel(bool bUp)
	{
		switch (m_eSelDateType)
		{
		case eDT_Year:
			bUp ? ++m_sysTime.wYear : --m_sysTime.wYear;
			break;
		case eDT_Month:
			{
				CircluNum(bUp, m_sysTime.wMonth, 1, 12);
				break;
			}
		case eDT_Day:
			{
				int nDays = SCalendarCore::GetDaysOfMonth(m_sysTime.wYear, m_sysTime.wMonth);
				CircluNum(bUp, m_sysTime.wDay, 1, nDays);
				break;
			}
		case eDT_Hour:
			{
				CircluNum(bUp, m_sysTime.wHour, 0, 23);
				break;
			}
		case eDT_Minute:
			{
				CircluNum(bUp, m_sysTime.wMinute, 0, 59);
				break;
			}
		case eDT_Second:
			{
				CircluNum(bUp, m_sysTime.wSecond, 0, 59);
				break;
			}
		default:
			break;
		}

		if (!SCalendarCore::DateCheck(m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay))
		{
			WORD nDays = SCalendarCore::GetDaysOfMonth(m_sysTime.wYear, m_sysTime.wMonth);
			m_sysTime.wDay = nDays;
		}

		m_wCharNum = 0;
		Invalidate();
	}

	void SDateTimePicker::CircluNum(bool bUp, WORD& wNum, WORD wMin/*=1*/, WORD wMax/*=0*/)
	{
		if (bUp)
		{
			if (wNum >= wMax)
				wNum = wMin;
			else
				++wNum;
		}
		else
		{
			if (wNum <= wMin)
				wNum = wMax;
			else
				--wNum;
		}
	}

	void SDateTimePicker::OnKeyDown(TCHAR nChar, UINT nRepCnt, UINT nFlags)
	{
		if (37 == nChar)				// <-
		{
			if (eDT_Year == m_eSelDateType)
				return;

			m_eSelDateType = static_cast<EnDateType>((int)m_eSelDateType - 1);
			m_wCharNum = 0;
			Invalidate();
		}
		else if (39 == nChar)
		{
			if (eDT_Second == m_eSelDateType)
				return;

			m_eSelDateType = static_cast<EnDateType>((int)m_eSelDateType + 1);
			m_wCharNum = 0;
			Invalidate();
		}
		else if (38 == nChar)
			TimeWheel(true);
		else if(40 == nChar)
			TimeWheel(false);		
	}

	void SDateTimePicker::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if(nChar > '9' || nChar < '0')
			return ;

		if(eDT_NULL == m_eSelDateType)
			return ;
		// 这个 不写了   谁帮我写了吧

		int nNum = nChar - 48;
		if (m_wCharNum > 0)
		{
			m_wCharNum *= 10;
			m_wCharNum += nNum;
		}
		else
		{
			m_wCharNum = nNum;
		}

		switch(m_eSelDateType)
		{
		case eDT_Year :
			m_sysTime.wYear = m_wCharNum;
			break;
		case eDT_Month :
			if (m_wCharNum <= 0)
			{
				return;
			}
			else if (m_wCharNum > 12)
				m_wCharNum = nNum;
			else if (!SCalendarCore::DateCheck(m_sysTime.wYear, m_wCharNum, m_sysTime.wDay))
				m_sysTime.wDay = SCalendarCore::GetDaysOfMonth(m_sysTime.wYear, m_wCharNum);
			m_sysTime.wMonth = m_wCharNum;
			break;
		case eDT_Day :
			if (m_wCharNum <= 0)
			{
				return;
			}
			if (!SCalendarCore::DateCheck(m_sysTime.wYear, m_sysTime.wMonth, m_wCharNum))
				m_wCharNum = nNum;
			m_sysTime.wDay = m_wCharNum;
			break;
		case eDT_Hour :
			if (m_wCharNum > 23 || m_wCharNum < 0)
				m_wCharNum = nNum;
			m_sysTime.wHour = m_wCharNum;
			break;
		case eDT_Minute :
			if (m_wCharNum > 59 || m_wCharNum < 0)
				m_wCharNum = nNum;
			m_sysTime.wMinute = m_wCharNum;
			break;
		case eDT_Second :
			if (m_wCharNum > 59 || m_wCharNum < 0)
				m_wCharNum = nNum;
			m_sysTime.wSecond = m_wCharNum;
			break;
		default:
			break;
		}

		if (eDT_NULL != m_eSelDateType)
			Invalidate();
	}

	void SDateTimePicker::OnDestroy()
	{
		CloseUp();
		__super::OnDestroy();
	}

	void SDateTimePicker::OnSetFocus(SWND wndOld)
	{
		__super::OnSetFocus(wndOld);
	}

	void SDateTimePicker::OnKillFocus(SWND wndFocus)
	{
		__super::OnKillFocus(wndFocus);
		CloseUp();
		m_wCharNum = 0;
		if (eDT_NULL != m_eSelDateType)
		{
			m_eSelDateType = eDT_NULL;
			Invalidate();
		}
	}

	void SDateTimePicker::CloseUp()
	{
		if(NULL != m_pDropDownWnd)
		{
			m_pDropDownWnd->EndDropDown(IDCANCEL);
		}
	}

	void SDateTimePicker::Clear()
	{
		memset(&m_sysTime,0,sizeof(m_sysTime));
		Invalidate();
	}

	LRESULT SDateTimePicker::OnAttrCueText(const SStringW &strValue,BOOL bLoading)
	{
		SStringW strTmp=GETSTRING(strValue); 
		m_strCue.SetText(S_CW2T(strTmp));
		Clear();//default to show cue.
		return bLoading ? S_FALSE : S_OK;
	}
} // SOUI
