#include "souistd.h"
#include "control/SMenuBar.h"

#define TIMER_POP	10


namespace SOUI
{
	HHOOK SMenuBar::m_hMsgHook = NULL;
	SMenuBar* SMenuBar::m_pMenuBar = NULL;

	const wchar_t XmlBtnStyle[] = L"btnStyle";
	const wchar_t XmlMenus[] = L"menus";

	class SMenuBarItem :
		public SButton,
		public SMenu
	{
		SOUI_CLASS_NAME(SMenuBarItem, L"menuItem")
			friend class SMenuBar;
	public:
		SMenuBarItem(SMenuBar* pHostMenu);
		~SMenuBarItem();

		void SetData(ULONG_PTR data) { m_data = data; }
		ULONG_PTR GetData() { return m_data; }

		bool IsMenuLoaded() const;
	protected:
		UINT PopMenu();

		HRESULT OnAttrSrc(const SStringW& strValue, BOOL bLoading);

		virtual void WINAPI OnFinalRelease() { delete this; }
		STDMETHOD_(SIZE,GetDesiredSize)(THIS_ int nParentWid, int nParentHei);
		bool OnCmd(IEvtArgs* e);

		void OnTimer(UINT_PTR timerID);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_TIMER(OnTimer)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
			ATTR_CUSTOM(L"src", OnAttrSrc)
		SOUI_ATTRS_END()

		ULONG_PTR m_data;
		SMenuBar* m_pHostMenu;
		BOOL m_bIsRegHotKey;
		int m_iIndex;
		TCHAR m_cAccessKey;
	};

	SMenuBarItem::SMenuBarItem(SMenuBar* pHostMenu) :
		m_data(0),
		m_pHostMenu(pHostMenu),
		m_bIsRegHotKey(FALSE),
		m_iIndex(-1),
		m_cAccessKey(0)
	{
		m_bDrawFocusRect = FALSE;
		GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&SMenuBarItem::OnCmd, this));
	}

	SMenuBarItem::~SMenuBarItem()
	{
	}

	bool SMenuBarItem::IsMenuLoaded() const
	{
		return true;
	}

	UINT SMenuBarItem::PopMenu()
	{
		if (m_pHostMenu->m_pNowMenu != NULL)
			return 0;
		m_pHostMenu->m_bIsShow = TRUE;
		m_pHostMenu->m_pNowMenu = this;
		m_pHostMenu->m_iNowMenu = m_iIndex;

		// 把弹出事件发送过去
		EventPopMenu evt_pop(m_pHostMenu);
		evt_pop.m_index = m_iIndex;
		evt_pop.m_pMenu = this;
		FireEvent(evt_pop);

		SetCheck(TRUE);

		CRect rcHost;
		::GetWindowRect(m_pHostMenu->m_hWnd, rcHost);
		CRect rcMenu = GetClientRect();

		if (SMenuBar::m_hMsgHook == NULL)
			SMenuBar::m_hMsgHook = ::SetWindowsHookEx(WH_MSGFILTER,
				SMenuBar::MenuSwitch, NULL, GetCurrentThreadId());// m_bLoop may become TRUE

		int iRet = 0;
		iRet = TrackPopupMenu(TPM_RETURNCMD,
			rcHost.left + rcMenu.left, rcHost.top + rcMenu.bottom + 2, m_pHostMenu->m_hWnd);

		SetCheck(FALSE);
		m_pHostMenu->m_bIsShow = FALSE;
		if (m_pHostMenu->m_pNowMenu != this || iRet == 0)
		{
			m_pHostMenu->m_pNowMenu = NULL;
			m_pHostMenu->m_iNowMenu = -1;
			return iRet;
		}
		m_pHostMenu->m_iNowMenu = -1;
		m_pHostMenu->m_pNowMenu = NULL;

		// uninstall hook
		::UnhookWindowsHookEx(SMenuBar::m_hMsgHook);
		SMenuBar::m_hMsgHook = NULL;

		// 把选择事件发送过去
		EventSelectMenu evt_sel(m_pHostMenu);
		evt_sel.m_id = iRet;
		evt_sel.m_pMenu = this;
		FireEvent(evt_sel);

		return iRet;
	}

	HRESULT SMenuBarItem::OnAttrSrc(const SStringW& strValue, BOOL bLoading)
	{
		return LoadMenu(S_CW2T(strValue)) ? S_OK : E_INVALIDARG;
	}

	SIZE SMenuBarItem::GetDesiredSize(int nParentWid, int nParentHei)
	{
		CSize size = SWindow::GetDesiredSize(nParentWid, nParentHei);
		size.cx += 13;
		size.cy += 3;
		return size;
	}

	bool SMenuBarItem::OnCmd(IEvtArgs* e)
	{
		if (!::IsWindow(m_pHostMenu->m_hWnd))
			m_pHostMenu->m_hWnd = GetContainer()->GetHostHwnd();

		e->SetBubbleUp(false);
		PopMenu();
		return true;
	}

	void SMenuBarItem::OnTimer(UINT_PTR timerID)
	{
		if (timerID == TIMER_POP)
		{
			if (!m_pHostMenu->m_bIsShow)
			{
				PopMenu();
				KillTimer(timerID);
			}
		}
	}

	SMenuBar::SMenuBar() :
		m_bIsShow(FALSE),
		m_hWnd(NULL),
		m_pNowMenu(NULL),
		m_iNowMenu(-1)
	{
		m_evtSet.addEvent(EVENTID(EventSelectMenu));
		SMenuBar::m_pMenuBar = this;
	}

	SMenuBar::~SMenuBar()
	{
		if (SMenuBar::m_hMsgHook)
		{
			::UnhookWindowsHookEx(SMenuBar::m_hMsgHook);
			SMenuBar::m_hMsgHook = NULL;
		}
	}
	BOOL SMenuBar::Insert(LPCTSTR pszTitle, LPCTSTR pszResName, int iPos)
	{
		if (!pszResName)
			return FALSE;

		SMenuBarItem* pNewMenu = new SMenuBarItem(this);
		SASSERT(pNewMenu);
		InsertChild(pNewMenu);

		SXmlNode xmlBtnStyle = m_xmlStyle.root().child(XmlBtnStyle);
		if (xmlBtnStyle)
			pNewMenu->InitFromXml(&xmlBtnStyle);

		if (pszTitle)
			pNewMenu->SetWindowText(pszTitle);

		pNewMenu->SetAttribute(L"src", S_CT2W(pszResName));
		pNewMenu->SetWindowText(pszTitle);

		if (!pNewMenu->IsMenuLoaded())
		{
			DestroyChild(pNewMenu);
			return FALSE;
		}

		SStringT strText = pszTitle;
		int nPos = strText.ReverseFind('&');
		if (nPos > -1)
			pNewMenu->SetAttribute(L"accel", SStringW().Format(L"alt+%c", strText[nPos + 1]));

		if (iPos < 0) iPos = m_lstMenuItem.GetCount();
		m_lstMenuItem.InsertAt(iPos, pNewMenu);

		pNewMenu->m_iIndex = iPos;
		for (size_t i = iPos + 1; i < m_lstMenuItem.GetCount(); i++)
		{
			m_lstMenuItem[i]->m_iIndex++;
		}
		return TRUE;
	}

	BOOL SMenuBar::Insert(IXmlNode * pNode, int iPos)
	{
		SMenuBarItem* pNewMenu = new SMenuBarItem(this);
		SASSERT(pNewMenu);
		InsertChild(pNewMenu);

		SXmlNode xmlBtnStyle = m_xmlStyle.root().child(XmlBtnStyle);
		if (xmlBtnStyle)
			pNewMenu->InitFromXml(&xmlBtnStyle);

		pNewMenu->InitFromXml(pNode);
		if (!pNewMenu->IsMenuLoaded())
		{
			DestroyChild(pNewMenu);
			return FALSE;
		}
		
		SXmlNode xmlNode(pNode);
		SStringW strText = xmlNode.first_child().value();
		int nPos = strText.ReverseFind(L'&');
		if (nPos > -1)
			pNewMenu->SetAttribute(L"accel", SStringW().Format(L"alt+%c", strText[nPos + 1]));

		if (iPos < 0) iPos = m_lstMenuItem.GetCount();
		m_lstMenuItem.InsertAt(iPos, pNewMenu);

		pNewMenu->m_iIndex = iPos;
		for (size_t i = iPos + 1; i < m_lstMenuItem.GetCount(); i++)
		{
			m_lstMenuItem[i]->m_iIndex++;
		}
		return TRUE;
	}

	SMenu* SMenuBar::GetMenu(DWORD dwPos)
	{
		if (dwPos >= m_lstMenuItem.GetCount())
			return NULL;
		return m_lstMenuItem[dwPos];
	}
	int SMenuBar::HitTest(CPoint pt)
	{
		for (size_t i = 0; i < m_lstMenuItem.GetCount(); i++)
		{
			SMenuBarItem* pItem = m_lstMenuItem[i];
			CRect rcItem = pItem->GetClientRect();
			if (rcItem.PtInRect(pt))
				return i;
		}
		return -1;
	}
	SMenuBarItem* SMenuBar::GetMenuItem(DWORD dwPos)
	{
		if (dwPos >= m_lstMenuItem.GetCount())
			return NULL;
		return m_lstMenuItem[dwPos];
	}

	BOOL SMenuBar::CreateChildren(SXmlNode xmlNode)
	{
		SXmlNode xmlBtnStyle = xmlNode.child(XmlBtnStyle);
		if (xmlBtnStyle)
		{
			m_xmlStyle.root().append_copy(xmlBtnStyle);
		}
		SXmlNode xmlTMenus = xmlNode.child(XmlMenus);
		if (xmlTMenus)
		{
			for (SXmlNode xmlChild = xmlTMenus.first_child(); xmlChild; xmlChild = xmlChild.next_sibling())
			{
				if (_wcsicmp(xmlChild.name(), SMenuBarItem::GetClassName()) != 0)
					continue;
				Insert(&xmlChild);
			}
		}

		return TRUE;
	}

	LRESULT SMenuBar::MenuSwitch(int code, WPARAM wParam, LPARAM lParam)
	{
		if (code == MSGF_MENU)
		{
			MSG msg = *(MSG*)lParam;
			int nMsg = msg.message;
			switch (nMsg)
			{
			case WM_MOUSEMOVE:
			{
				CPoint pt = msg.lParam;
				if (SMenuBar::m_pMenuBar->m_ptMouse != pt &&
					SMenuBar::m_pMenuBar->m_iNowMenu != -1)
				{
					SMenuBar::m_pMenuBar->m_ptMouse = pt;
					::ScreenToClient(SMenuBar::m_pMenuBar->m_hWnd, &pt);
					int nIndex = SMenuBar::m_pMenuBar->HitTest(pt);
					if (nIndex != -1)
					{
						SMenuBarItem* menuItem = SMenuBar::m_pMenuBar->GetMenuItem(nIndex);
						if (menuItem && SMenuBar::m_pMenuBar->m_iNowMenu != nIndex)
						{
							SMenuBar::m_pMenuBar->m_pNowMenu = menuItem;
							SMenuBar::m_pMenuBar->m_iNowMenu = nIndex;
							::PostMessage(msg.hwnd, WM_CANCELMODE, 0, 0);
							menuItem->SetTimer(TIMER_POP, 10);
							return TRUE;
						}
					}
				}
				break;
			}
			case WM_KEYDOWN:
			{
				TCHAR vKey = msg.wParam;
				if (SMenuBar::m_pMenuBar->m_iNowMenu == -1)
					return TRUE;
				if (vKey == VK_LEFT)
				{
					int nRevIndex = SMenuBar::m_pMenuBar->m_iNowMenu - 1;
					if (nRevIndex < 0) nRevIndex = SMenuBar::m_pMenuBar->m_lstMenuItem.GetCount() - 1;
					SMenuBarItem* menuItem = SMenuBar::m_pMenuBar->m_lstMenuItem[nRevIndex];
					if (menuItem)
					{
						SMenuBar::m_pMenuBar->m_pNowMenu = menuItem;
						SMenuBar::m_pMenuBar->m_iNowMenu = nRevIndex;
						::PostMessage(SMenuBar::m_pMenuBar->m_hWnd, WM_KEYDOWN, VK_ESCAPE, 0);
						menuItem->SetTimer(TIMER_POP, 10);
						return TRUE;
					}
				}
				else if (vKey == VK_RIGHT)
				{
					int nNextIndex = SMenuBar::m_pMenuBar->m_iNowMenu + 1;
					if (nNextIndex >= (int)SMenuBar::m_pMenuBar->m_lstMenuItem.GetCount()) nNextIndex = 0;
					SMenuBarItem* menuItem = SMenuBar::m_pMenuBar->GetMenuItem(nNextIndex);
					if (menuItem)
					{
						SMenuBar::m_pMenuBar->m_pNowMenu = menuItem;
						SMenuBar::m_pMenuBar->m_iNowMenu = nNextIndex;
						::PostMessage(SMenuBar::m_pMenuBar->m_hWnd, WM_KEYDOWN, VK_ESCAPE, 0);
						::PostMessage(msg.hwnd, WM_CANCELMODE, 0, 0);
						menuItem->SetTimer(TIMER_POP, 10);
						return TRUE;
					}
				}
			}
			}
		}
		return CallNextHookEx(m_hMsgHook, code, wParam, lParam);
	}
}