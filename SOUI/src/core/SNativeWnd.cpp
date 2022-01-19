#include "souistd.h"
#include "core/SNativeWnd.h"

SNSBEGIN

SNativeWndHelper::SNativeWndHelper(HINSTANCE hInst,LPCTSTR pszClassName,BOOL bImeApp)
        :m_hInst(hInst)
        ,m_sharePtr(NULL)
		,m_atom(NULL)
    {
        InitializeCriticalSection(&m_cs);
        m_hHeap=HeapCreate(HEAP_CREATE_ENABLE_EXECUTE,0,0);
		if(bImeApp)
			m_atom = SNativeWnd::RegisterSimpleWnd2(m_hInst, pszClassName);
		else
			m_atom=SNativeWnd::RegisterSimpleWnd(m_hInst,pszClassName);
    }

    SNativeWndHelper::~SNativeWndHelper()
    {
        if(m_hHeap) HeapDestroy(m_hHeap);
        DeleteCriticalSection(&m_cs);
        if(m_atom) UnregisterClass((LPCTSTR)m_atom,m_hInst);
    }

    void SNativeWndHelper::LockSharePtr(void *p)
    {
        EnterCriticalSection(&m_cs);
        m_sharePtr=p;
    }

    void SNativeWndHelper::UnlockSharePtr()
    {
        LeaveCriticalSection(&m_cs);
    }

//////////////////////////////////////////////////////////////////////////
SNativeWnd::SNativeWnd()
    :m_bDestoryed(FALSE)
    ,m_pCurrentMsg(NULL)
    ,m_hWnd(NULL)
    ,m_pfnSuperWindowProc(::DefWindowProc)
    ,m_pThunk(NULL)
{
	m_evtSet.addEvent(EVENTID(EventNativeMsg));
}

SNativeWnd::~SNativeWnd(void)
{
}

ATOM SNativeWnd::RegisterSimpleWnd( HINSTANCE hInst,LPCTSTR pszSimpleWndName )
{
    WNDCLASSEX wcex = {sizeof(WNDCLASSEX),0};
    wcex.cbSize           = sizeof(WNDCLASSEX);
    wcex.style            = CS_HREDRAW | CS_VREDRAW| CS_DBLCLKS ;
    wcex.lpfnWndProc      = StartWindowProc; // 第一个处理函数
    wcex.hInstance        = hInst;
    wcex.hCursor          = ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground    = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszClassName    = pszSimpleWndName;
    return ::RegisterClassEx(&wcex);
}

ATOM SNativeWnd::RegisterSimpleWnd2(HINSTANCE hInst, LPCTSTR pszSimpleWndName)
{
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX),0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_IME;
	wcex.lpfnWndProc = StartWindowProc; // 第一个处理函数
	wcex.hInstance = hInst;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = pszSimpleWndName;
	return ::RegisterClassEx(&wcex);
}

HWND SNativeWnd::CreateWindow(LPCTSTR lpWindowName, DWORD dwStyle,DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent,LPVOID lpParam )
{
    SNativeWndHelper::getSingletonPtr()->LockSharePtr(this);

    m_pThunk=(tagThunk*)HeapAlloc(SNativeWndHelper::getSingletonPtr()->GetHeap(),HEAP_ZERO_MEMORY,sizeof(tagThunk));
    // 在::CreateWindow返回之前会去调StarWindowProc函数
    HWND hWnd= ::CreateWindowEx(dwExStyle,(LPCTSTR)SNativeWndHelper::getSingletonPtr()->GetSimpleWndAtom(), lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, 0, SNativeWndHelper::getSingletonPtr()->GetAppInstance(), lpParam);
    SNativeWndHelper::getSingletonPtr()->UnlockSharePtr();
    if(!hWnd)
    {
        HeapFree(SNativeWndHelper::getSingletonPtr()->GetHeap(),0,m_pThunk);
        m_pThunk=NULL;
    }
    return hWnd;
}

HWND SNativeWnd::GetHwnd( )
{
	return m_hWnd;
}


void SNativeWnd::OnFinalMessage( HWND hWnd )
{
    if(m_pThunk)
    {
        HeapFree(SNativeWndHelper::getSingletonPtr()->GetHeap(),0,m_pThunk);
        m_pThunk=NULL;
    }
	m_hWnd = NULL;
}

LRESULT CALLBACK SNativeWnd::WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    SNativeWnd* pThis = (SNativeWnd*)hWnd; // 强转为对象指针
    MSG msg= {pThis->m_hWnd, uMsg, wParam, lParam};
    const MSG* pOldMsg = pThis->m_pCurrentMsg;
    pThis->m_pCurrentMsg = &msg;
    // pass to the message map to process
    LRESULT lRes;
	{
		EventNativeMsg evt(pThis);
		evt.hWnd = hWnd;
		evt.uMsg = uMsg;
		evt.wParam = wParam;
		evt.lParam = lParam;
		evt.result = 0;
		pThis->m_evtSet.FireEvent(&evt);
		if(!evt.IsBubbleUp())
			return evt.result;
	}

    BOOL bRet = pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0);
    // restore saved value for the current message
    SASSERT(pThis->m_pCurrentMsg == &msg);

    // do the default processing if message was not handled
    if(!bRet)
    {
        if(uMsg != WM_NCDESTROY)
            lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
        else
        {
            // unsubclass, if needed
            LONG_PTR pfnWndProc = ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC);
            lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
            if(pThis->m_pfnSuperWindowProc != ::DefWindowProc && ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC) == pfnWndProc)
                ::SetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC, (LONG_PTR)pThis->m_pfnSuperWindowProc);
            // mark window as destryed
            pThis->m_bDestoryed=TRUE;
        }

    }
    if((pThis->m_bDestoryed) && pOldMsg== NULL)
    {
        // clear out window handle
        HWND hWndThis = pThis->m_hWnd;
        pThis->m_hWnd = NULL;
        pThis->m_bDestoryed =FALSE;
        // clean up after window is destroyed
        pThis->m_pCurrentMsg = pOldMsg;
        pThis->OnFinalMessage(hWndThis);
    }
    else
    {
        pThis->m_pCurrentMsg = pOldMsg;
    }
    return lRes;
}

LRESULT CALLBACK SNativeWnd::StartWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    SNativeWnd* pThis=(SNativeWnd*)SNativeWndHelper::getSingletonPtr()->GetSharePtr();

    pThis->m_hWnd=hWnd;
    // 初始化Thunk，做了两件事:1、mov指令替换hWnd为对象指针，2、jump指令跳转到WindowProc
    pThis->m_pThunk->Init((DWORD_PTR)WindowProc, pThis);

    // 得到Thunk指针
    WNDPROC pProc = (WNDPROC)pThis->m_pThunk->GetCodeAddress();
    // 调用下面的语句后，以后消息来了，都由pProc处理
    ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pProc);

    return pProc(hWnd, uMsg, wParam, lParam);
}

BOOL SNativeWnd::SubclassWindow( HWND hWnd )
{
    SASSERT(::IsWindow(hWnd));
    // Allocate the thunk structure here, where we can fail gracefully.
    m_pThunk=(tagThunk*)HeapAlloc(SNativeWndHelper::getSingletonPtr()->GetHeap(),HEAP_ZERO_MEMORY,sizeof(tagThunk));
    m_pThunk->Init((DWORD_PTR)WindowProc, this);
    WNDPROC pProc = (WNDPROC)m_pThunk->GetCodeAddress();
    WNDPROC pfnWndProc = (WNDPROC)::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pProc);
    if(pfnWndProc == NULL)
    {
        HeapFree(SNativeWndHelper::getSingletonPtr()->GetHeap(),0,m_pThunk);
        m_pThunk=NULL;
        return FALSE;
    }
    m_pfnSuperWindowProc = pfnWndProc;
    m_hWnd = hWnd;
    return TRUE;
}


HWND SNativeWnd::UnsubclassWindow( BOOL bForce /*= FALSE*/ )
{
    SASSERT(m_hWnd != NULL);

    WNDPROC pOurProc = (WNDPROC)m_pThunk->GetCodeAddress();
    WNDPROC pActiveProc = (WNDPROC)::GetWindowLongPtr(m_hWnd, GWLP_WNDPROC);

    HWND hWnd = NULL;
    if (bForce || pOurProc == pActiveProc)
    {
        if(!::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_pfnSuperWindowProc))
            return NULL;

        m_pfnSuperWindowProc = ::DefWindowProc;
        hWnd = m_hWnd;
        m_hWnd = NULL;
    }
    return hWnd;
}

LRESULT SNativeWnd::ForwardNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LRESULT lResult = 0;
    switch(uMsg)
    {
    case WM_COMMAND:
    case WM_NOTIFY:
    case WM_PARENTNOTIFY:
    case WM_DRAWITEM:
    case WM_MEASUREITEM:
    case WM_COMPAREITEM:
    case WM_DELETEITEM:
    case WM_VKEYTOITEM:
    case WM_CHARTOITEM:
    case WM_HSCROLL:
    case WM_VSCROLL:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
        bHandled = TRUE;
        lResult = ::SendMessage(GetParent(),uMsg, wParam, lParam);
        break;
    default:
        bHandled = FALSE;
        break;
    }
    return lResult;
}

LRESULT SNativeWnd::ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    HWND hWndChild = NULL;

    switch(uMsg)
    {
    case WM_COMMAND:
        if(lParam != NULL)    // not from a menu
            hWndChild = (HWND)lParam;
        break;
    case WM_NOTIFY:
        hWndChild = ((LPNMHDR)lParam)->hwndFrom;
        break;
    case WM_PARENTNOTIFY:
        switch(LOWORD(wParam))
        {
        case WM_CREATE:
        case WM_DESTROY:
            hWndChild = (HWND)lParam;
            break;
        default:
            hWndChild = GetDlgItem(m_hWnd,HIWORD(wParam));
            break;
        }
        break;
    case WM_DRAWITEM:
        if(wParam)    // not from a menu
            hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
        break;
    case WM_MEASUREITEM:
        if(wParam)    // not from a menu
            hWndChild = GetDlgItem(m_hWnd,((LPMEASUREITEMSTRUCT)lParam)->CtlID);
        break;
    case WM_COMPAREITEM:
        if(wParam)    // not from a menu
            hWndChild =  ((LPCOMPAREITEMSTRUCT)lParam)->hwndItem;
        break;
    case WM_DELETEITEM:
        if(wParam)    // not from a menu
            hWndChild =  ((LPDELETEITEMSTRUCT)lParam)->hwndItem;

        break;
    case WM_VKEYTOITEM:
    case WM_CHARTOITEM:
    case WM_HSCROLL:
    case WM_VSCROLL:
        hWndChild = (HWND)lParam;
        break;
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
        hWndChild = (HWND)lParam;
        break;
    default:
        break;
    }

    if(hWndChild == NULL)
    {
        bHandled = FALSE;
        return 1;
    }
    bHandled = TRUE;
    SASSERT(::IsWindow(hWndChild));
    return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
}

BOOL SNativeWnd::DefaultReflectionHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
    switch(uMsg)
    {
    case OCM_COMMAND:
    case OCM_NOTIFY:
    case OCM_PARENTNOTIFY:
    case OCM_DRAWITEM:
    case OCM_MEASUREITEM:
    case OCM_COMPAREITEM:
    case OCM_DELETEITEM:
    case OCM_VKEYTOITEM:
    case OCM_CHARTOITEM:
    case OCM_HSCROLL:
    case OCM_VSCROLL:
    case OCM_CTLCOLORBTN:
    case OCM_CTLCOLORDLG:
    case OCM_CTLCOLOREDIT:
    case OCM_CTLCOLORLISTBOX:
    case OCM_CTLCOLORMSGBOX:
    case OCM_CTLCOLORSCROLLBAR:
    case OCM_CTLCOLORSTATIC:
        lResult = ::DefWindowProc(hWnd, uMsg - OCM__BASE, wParam, lParam);
        return TRUE;
    default:
        break;
    }
    return FALSE;
}

LRESULT SNativeWnd::DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return ::CallWindowProc(m_pfnSuperWindowProc,m_hWnd, uMsg, wParam, lParam);
}

LRESULT SNativeWnd::DefWindowProc()
{
	const MSG* pMsg = m_pCurrentMsg;
	LRESULT lRes = 0;
	if (pMsg != NULL)
		lRes = DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
	return lRes;
}

BOOL SNativeWnd::CenterWindow(HWND hWndCenter /*= NULL*/)
{
	SASSERT(::IsWindow(m_hWnd));

	// determine owner window to center against
	DWORD dwStyle = GetStyle();
	if(hWndCenter == NULL)
	{
		if(dwStyle & WS_CHILD)
			hWndCenter = ::GetParent(m_hWnd);
		else
			hWndCenter = ::GetWindow(m_hWnd, GW_OWNER);

		if(hWndCenter == NULL)
			hWndCenter = ::GetActiveWindow();
	}

	// get coordinates of the window relative to its parent
	RECT rcDlg;
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea;
	RECT rcCenter;
	HWND hWndParent;
	if(!(dwStyle & WS_CHILD))
	{
		// don't center against invisible or minimized windows
		if(hWndCenter != NULL)
		{
			DWORD dwStyleCenter = (DWORD)::GetWindowLongPtr(hWndCenter, GWL_STYLE);
			if(!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
				hWndCenter = NULL;
		}

		// center within screen coordinates
#if WINVER < 0x0500
		::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
#else
		HMONITOR hMonitor = NULL;
		if(hWndCenter != NULL)
		{
			hMonitor = ::MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST);
		}
		else
		{
			hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
		}

		MONITORINFO minfo;
		minfo.cbSize = sizeof(MONITORINFO);
		::GetMonitorInfo(hMonitor, &minfo);

		rcArea = minfo.rcWork;
#endif
		if(hWndCenter == NULL)
			rcCenter = rcArea;
		else
			::GetWindowRect(hWndCenter, &rcCenter);
	}
	else
	{
		// center within parent client coordinates
		hWndParent = ::GetParent(m_hWnd);
		SASSERT(::IsWindow(hWndParent));

		::GetClientRect(hWndParent, &rcArea);
		SASSERT(::IsWindow(hWndCenter));
		::GetClientRect(hWndCenter, &rcCenter);
		::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
	}

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// if the dialog is outside the screen, move it inside
	if(xLeft + DlgWidth > rcArea.right)
		xLeft = rcArea.right - DlgWidth;
	if(xLeft < rcArea.left)
		xLeft = rcArea.left;

	if(yTop + DlgHeight > rcArea.bottom)
		yTop = rcArea.bottom - DlgHeight;
	if(yTop < rcArea.top)
		yTop = rcArea.top;

	// map screen coordinates to child coordinates
	return ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL SNativeWnd::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags /*= 0*/)
{
	SASSERT(::IsWindow(m_hWnd));

	DWORD dwStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if(dwStyle == dwNewStyle)
		return FALSE;

	::SetWindowLongPtr(m_hWnd, GWL_STYLE, dwNewStyle);
	if(nFlags != 0)
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}

	return TRUE;
}

BOOL SNativeWnd::ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags /*= 0*/)
{
	SASSERT(::IsWindow(m_hWnd));

	DWORD dwStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if(dwStyle == dwNewStyle)
		return FALSE;

	::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, dwNewStyle);
	if(nFlags != 0)
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}

	return TRUE;
}

BOOL SNativeWnd::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID /*= 0*/)
{
	return FALSE;
}

BOOL SNativeWnd::SubscribeEvent(const IEvtSlot *pSlot)
{
	return m_evtSet.subscribeEvent(EventNativeMsg::EventID,pSlot);
}

BOOL SNativeWnd::UnsubscribeEvent(const IEvtSlot *pSlot)
{
	return m_evtSet.unsubscribeEvent(EventNativeMsg::EventID,pSlot);
}

BOOL SNativeWnd::UpdateLayeredWindow(HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc,COLORREF crKey, BLENDFUNCTION *pblend,DWORD dwFlags)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::UpdateLayeredWindow(m_hWnd,hdcDst,pptDst,psize,hdcSrc,pptSrc,crKey,pblend,dwFlags);
}

BOOL SNativeWnd::SetLayeredWindowAttributes(COLORREF crKey,BYTE bAlpha,DWORD dwFlags)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SetLayeredWindowAttributes(m_hWnd,crKey,bAlpha,dwFlags);
}

int SNativeWnd::SetWindowRgn(HRGN hRgn,BOOL bRedraw/*=TRUE*/)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SetWindowRgn(m_hWnd,hRgn,bRedraw);
}

BOOL SNativeWnd::ShowWindow(int nCmdShow)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::ShowWindow(m_hWnd,nCmdShow);
}

BOOL SNativeWnd::MoveWindow2(LPCRECT lpRect, BOOL bRepaint /*= TRUE*/)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::MoveWindow(m_hWnd, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
}

BOOL SNativeWnd::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint /*= TRUE*/)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::MoveWindow(m_hWnd, x, y, nWidth, nHeight, bRepaint);
}

BOOL SNativeWnd::IsWindowVisible() const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::IsWindowVisible(m_hWnd);
}

BOOL SNativeWnd::IsZoomed() const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::IsZoomed(m_hWnd);
}

BOOL SNativeWnd::IsIconic() const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::IsIconic(m_hWnd);
}

int SNativeWnd::GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::GetWindowText(m_hWnd, lpszStringBuf, nMaxCount);
}

BOOL SNativeWnd::SetWindowText(LPCTSTR lpszString)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SetWindowText(m_hWnd, lpszString);
}

BOOL SNativeWnd::SendNotifyMessage(UINT message, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SendNotifyMessage(m_hWnd, message, wParam, lParam);
}

BOOL SNativeWnd::PostMessage(UINT message, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::PostMessage(m_hWnd,message,wParam,lParam);
}

LRESULT SNativeWnd::SendMessage(UINT message, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SendMessage(m_hWnd,message,wParam,lParam);
}

HWND SNativeWnd::SetFocus()
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SetFocus(m_hWnd);
}

HWND SNativeWnd::SetCapture()
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SetCapture(m_hWnd);
}

BOOL SNativeWnd::ShowCaret()
{
	SASSERT(::IsWindow(m_hWnd));
	return ::ShowCaret(m_hWnd);
}

BOOL SNativeWnd::HideCaret()
{
	SASSERT(::IsWindow(m_hWnd));
	return ::HideCaret(m_hWnd);
}

BOOL SNativeWnd::CreateCaret(HBITMAP hBitmap)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::CreateCaret(m_hWnd, hBitmap, 0, 0);
}

int SNativeWnd::ReleaseDC(HDC hDC)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::ReleaseDC(m_hWnd, hDC);
}

HDC SNativeWnd::GetWindowDC()
{
	SASSERT(::IsWindow(m_hWnd));
	return ::GetWindowDC(m_hWnd);
}

HDC SNativeWnd::GetDC()
{
	SASSERT(::IsWindow(m_hWnd));
	return ::GetDC(m_hWnd);
}

BOOL SNativeWnd::KillTimer(UINT_PTR nIDEvent)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::KillTimer(m_hWnd, nIDEvent);
}

UINT_PTR SNativeWnd::SetTimer(UINT_PTR nIDEvent, UINT nElapse, void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD) /*= NULL*/)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SetTimer(m_hWnd, nIDEvent, nElapse, (TIMERPROC)lpfnTimer);
}

int SNativeWnd::MapWindowRect(HWND hWndTo, LPRECT lpRect) const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::MapWindowPoints(m_hWnd, hWndTo, (LPPOINT)lpRect, 2);
}

int SNativeWnd::MapWindowPoints(HWND hWndTo, LPPOINT lpPoint, UINT nCount) const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::MapWindowPoints(m_hWnd, hWndTo, lpPoint, nCount);
}

BOOL SNativeWnd::ScreenToClient2(LPRECT lpRect) const
{
	SASSERT(::IsWindow(m_hWnd));
	if(!::ScreenToClient(m_hWnd, (LPPOINT)lpRect))
		return FALSE;
	return ::ScreenToClient(m_hWnd, ((LPPOINT)lpRect)+1);
}

BOOL SNativeWnd::ScreenToClient(LPPOINT lpPoint) const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::ScreenToClient(m_hWnd, lpPoint);
}

BOOL SNativeWnd::ClientToScreen2(LPRECT lpRect) const
{
	SASSERT(::IsWindow(m_hWnd));
	if(!::ClientToScreen(m_hWnd, (LPPOINT)lpRect))
		return FALSE;
	return ::ClientToScreen(m_hWnd, ((LPPOINT)lpRect)+1);
}

BOOL SNativeWnd::ClientToScreen(LPPOINT lpPoint) const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::ClientToScreen(m_hWnd, lpPoint);
}

BOOL SNativeWnd::GetClientRect(LPRECT lpRect) const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::GetClientRect(m_hWnd, lpRect);
}

BOOL SNativeWnd::GetWindowRect(LPRECT lpRect) const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::GetWindowRect(m_hWnd, lpRect);
}

BOOL SNativeWnd::InvalidateRect(LPCRECT lpRect, BOOL bErase /*= TRUE*/)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::InvalidateRect(m_hWnd, lpRect, bErase);
}

BOOL SNativeWnd::Invalidate(BOOL bErase /*= TRUE*/)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::InvalidateRect(m_hWnd, NULL, bErase);
}

BOOL SNativeWnd::IsWindow()
{
	return ::IsWindow(m_hWnd);
}

BOOL SNativeWnd::DestroyWindow()
{
	SASSERT(::IsWindow(m_hWnd));
	return ::DestroyWindow(m_hWnd);
}

BOOL SNativeWnd::SetWindowPos(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SetWindowPos(m_hWnd, hWndInsertAfter, x, y, cx, cy, nFlags);
}

BOOL SNativeWnd::IsWindowEnabled() const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::IsWindowEnabled(m_hWnd);
}

HWND SNativeWnd::SetParent(HWND hWndNewParent)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SetParent(m_hWnd, hWndNewParent);
}

HWND SNativeWnd::GetParent()
{
	SASSERT(::IsWindow(m_hWnd));
	return ::GetParent(m_hWnd);
}

LONG_PTR SNativeWnd::SetWindowLongPtr(int nIndex, LONG_PTR dwNewLong)
{
	SASSERT(::IsWindow(m_hWnd));
	return ::SetWindowLongPtr(m_hWnd, nIndex, dwNewLong);
}

LONG_PTR SNativeWnd::GetWindowLongPtr(int nIndex) const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::GetWindowLongPtr(m_hWnd, nIndex);
}

DWORD SNativeWnd::GetExStyle() const
{
	SASSERT(::IsWindow(m_hWnd));
	return (DWORD)::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
}

DWORD SNativeWnd::GetStyle() const
{
	SASSERT(::IsWindow(m_hWnd));
	return (DWORD)::GetWindowLongPtr(m_hWnd, GWL_STYLE);
}

int SNativeWnd::GetDlgCtrlID() const
{
	SASSERT(::IsWindow(m_hWnd));
	return ::GetDlgCtrlID(m_hWnd);
}

const MSG * SNativeWnd::GetCurrentMessage() const
{
	return m_pCurrentMsg;
}

SNSEND