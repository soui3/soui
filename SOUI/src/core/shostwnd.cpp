#include "souistd.h"

#include "SApp.h"
#include "core/SHostWnd.h"
#include "helper/SAutoBuf.h"
#include "helper/SColor.h"
#include "helper/SplitString.h"

#include "../updatelayeredwindow/SUpdateLayeredWindow.h"

namespace SOUI
{

#define KConstDummyPaint    0x80000000


//////////////////////////////////////////////////////////////////////////
//    SDummyWnd
//////////////////////////////////////////////////////////////////////////
	class SDummyWnd : public SNativeWnd
	{
	public:
		SDummyWnd(SHostWnd* pOwner) :m_pOwner(pOwner)
		{
		}

		void OnPaint(HDC dc);

		BEGIN_MSG_MAP_EX(SDummyWnd)
			MSG_WM_PAINT(OnPaint)
			END_MSG_MAP()
	private:
		SHostWnd *m_pOwner;
	};

void SDummyWnd::OnPaint( HDC dc )
{
    PAINTSTRUCT ps;
    ::BeginPaint(m_hWnd, &ps);
    ::EndPaint(m_hWnd, &ps);
    m_pOwner->OnPrint(NULL,KConstDummyPaint);
}


//////////////////////////////////////////////////////////////////////////
//    SHostWndAttr
//////////////////////////////////////////////////////////////////////////
SHostWndAttr::SHostWndAttr(void)
	: m_hAppIconSmall(NULL)
	, m_hAppIconBig(NULL)
	, m_strTitle(this)
{
	Init();
}

SHostWndAttr::~SHostWndAttr()
{
	if (m_hAppIconSmall) DestroyIcon(m_hAppIconSmall);
	if (m_hAppIconBig) DestroyIcon(m_hAppIconBig);
}

void SHostWndAttr::Init()
{
	m_bResizable = FALSE;
	m_bTranslucent = FALSE;
	m_bAppWnd = FALSE;
	m_bToolWnd = FALSE;
	m_byWndType = WT_UNDEFINE;
	m_bAllowSpy = TRUE;
	m_bSendWheel2Hover = FALSE;
	m_byAlpha = (0xFF);
	m_dwStyle = (0);
	m_dwExStyle = (0);
	if (m_hAppIconSmall) DestroyIcon(m_hAppIconSmall);
	if (m_hAppIconBig) DestroyIcon(m_hAppIconBig);
	m_hAppIconSmall = (NULL);
	m_hAppIconBig = (NULL);
}

const SStringW & SHostWndAttr::GetTrCtx() const{
	return m_strTrCtx;
}

CRect SHostWndAttr::GetMaxInset(int nScale) const
{
	CRect rcRet;
	rcRet.left = m_rcMaxInset[0].toPixelSize(nScale);
	rcRet.top = m_rcMaxInset[1].toPixelSize(nScale);
	rcRet.right = m_rcMaxInset[2].toPixelSize(nScale);
	rcRet.bottom = m_rcMaxInset[3].toPixelSize(nScale);
	return rcRet;
}

CRect SHostWndAttr::GetMargin(int nScale) const
{
	CRect rcRet;
	rcRet.left = m_rcMargin[0].toPixelSize(nScale);
	rcRet.top = m_rcMargin[1].toPixelSize(nScale);
	rcRet.right = m_rcMargin[2].toPixelSize(nScale);
	rcRet.bottom = m_rcMargin[3].toPixelSize(nScale);
	return rcRet;
}

CSize SHostWndAttr::GetMinSize(int nScale) const
{
	CSize szRet;
	szRet.cx = m_szMin[0].toPixelSize(nScale);
	szRet.cy = m_szMin[1].toPixelSize(nScale);
	return szRet;
}

//////////////////////////////////////////////////////////////////////////
// SHostWnd
//////////////////////////////////////////////////////////////////////////
SHostWnd::SHostWnd( LPCTSTR pszResName /*= NULL*/ )
: m_strXmlLayout(pszResName)
, m_bTrackFlag(FALSE)
, m_bNeedRepaint(FALSE)
, m_bNeedAllRepaint(TRUE)
, m_pTipCtrl(NULL)
, m_dummyWnd(NULL)
, m_bRendering(FALSE)
, m_nScale(100)
, m_szAppSetted(0,0)
, m_nAutoSizing(0)
, m_bResizing(false)
{
    m_msgMouse.message = 0;
    m_privateStylePool.Attach(new SStylePool);
    m_privateSkinPool.Attach(new SSkinPool);
	m_privateTemplatePool.Attach(new STemplatePool);
    SetContainer(this);

    m_evtSet.addEvent(EVENTID(EventInit));
    m_evtSet.addEvent(EVENTID(EventExit));
}

SHostWnd::~SHostWnd()
{
    GETSTYLEPOOLMGR->PopStylePool(m_privateStylePool);
    GETSKINPOOLMGR->PopSkinPool(m_privateSkinPool);
	GETTEMPLATEPOOLMR->PopTemplatePool(m_privateTemplatePool);
}

HWND SHostWnd::Create(HWND hWndParent,DWORD dwStyle,DWORD dwExStyle, int x, int y, int nWidth, int nHeight)
{
    if (NULL != m_hWnd)
        return m_hWnd;
	m_nAutoSizing++;
    HWND hWnd = SNativeWnd::Create(_T("HOSTWND"),dwStyle,dwExStyle, x,y,nWidth,nHeight,hWndParent,NULL);
	m_nAutoSizing--;
	if(!hWnd) return NULL;

    if(nWidth==0 || nHeight==0) CenterWindow(hWndParent);
    return hWnd;
}

HWND SHostWnd::Create(HWND hWndParent,int x,int y,int nWidth,int nHeight)
{
    return Create(hWndParent, WS_CLIPCHILDREN | WS_TABSTOP | WS_OVERLAPPED,0,x,y,nWidth,nHeight);
}

bool SHostWnd::onRootResize( EventArgs *e )
{
	if (!m_bResizing)
	{
		EventSwndSize *e2 = sobj_cast<EventSwndSize>(e);
		m_nAutoSizing++;
		SetWindowPos(NULL, 0, 0, e2->szWnd.cx, e2->szWnd.cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );
		m_nAutoSizing--;
	}
	return true;
}

BOOL SHostWnd::InitFromXml(pugi::xml_node xmlNode)
{
    if(!xmlNode)
    {
        SASSERT_FMTA(FALSE,"Null XML node");
        return FALSE;
    }
    if(!SNativeWnd::IsWindow()) return FALSE;
    
    //free old script module
    if(m_pScriptModule)
    {
        EventExit evt(this);
        FireEvent(evt);
        m_pScriptModule = NULL;
    }

    //为了能够重入，先销毁原有的SOUI窗口
    SSendMessage(WM_DESTROY);   
    //create new script module
    SApplication::getSingleton().CreateScriptModule(&m_pScriptModule);
    

    if(m_privateStylePool->GetCount())
    {
        m_privateStylePool->RemoveAll();
        GETSTYLEPOOLMGR->PopStylePool(m_privateStylePool);
    }
    m_privateStylePool->Init(xmlNode.child(L"style"));
    if(m_privateStylePool->GetCount())
    {
        GETSTYLEPOOLMGR->PushStylePool(m_privateStylePool);
    }
    if(m_privateSkinPool->GetCount())
    {
        m_privateSkinPool->RemoveAll();
        GETSKINPOOLMGR->PopSkinPool(m_privateSkinPool);
    }
    m_privateSkinPool->LoadSkins(xmlNode.child(L"skin"));//从xmlNode加加载私有skin
    if(m_privateSkinPool->GetCount())
    {
        GETSKINPOOLMGR->PushSkinPool(m_privateSkinPool);
    }

	if(m_privateTemplatePool->GetCount())
	{
		m_privateTemplatePool->RemoveAll();
		GETTEMPLATEPOOLMR->PopTemplatePool(m_privateTemplatePool);
	}
	m_privateTemplatePool->Init(xmlNode.child(L"template"));
	if(m_privateTemplatePool->GetCount())
	{
		GETTEMPLATEPOOLMR->PushTemplatePool(m_privateTemplatePool);
	}
    //加载脚本数据
    pugi::xml_node xmlScript = xmlNode.child(L"script");
    if(m_pScriptModule && xmlScript)
    {
        pugi::xml_attribute attrSrc = xmlScript.attribute(L"src");
        if(attrSrc)
        {
            SStringT strSrc = S_CW2T(attrSrc.value());
            SStringTList lstSrc;
            if(ParseResID(strSrc,lstSrc)==2)
            {
                size_t dwSize = SApplication::getSingleton().GetRawBufferSize(lstSrc[0],lstSrc[1]);
                if(dwSize)
                {
                    SAutoBuf buff(dwSize);
                    SApplication::getSingleton().GetRawBuffer(lstSrc[0],lstSrc[1],buff,dwSize);
                    m_pScriptModule->executeScriptBuffer(buff,dwSize);
                }
            }
        }else
        {
            //从script节点的cdata中获取脚本
            SStringW strScript = xmlScript.child_value();
            if(!strScript.IsEmpty())
            {
                SStringA utf8Script = S_CW2A(strScript,CP_UTF8);
                m_pScriptModule->executeScriptBuffer(utf8Script,utf8Script.GetLength());
            }
        }
    }

    DWORD dwStyle =SNativeWnd::GetStyle();
    DWORD dwExStyle  = SNativeWnd::GetExStyle();
    
    m_hostAttr.Init();
    m_hostAttr.InitFromXml(xmlNode);

    if (m_hostAttr.m_bResizable)
    {
        dwStyle |= WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
    }
    else
    {
        dwStyle |= WS_MINIMIZEBOX;
    }
    if(m_hostAttr.m_bAppWnd)
    {
        dwStyle |= WS_SYSMENU ;
        dwExStyle |= WS_EX_APPWINDOW;
    }else if(m_hostAttr.m_bToolWnd)
    {
        dwExStyle |= WS_EX_TOOLWINDOW;
    }
    if(m_hostAttr.m_bTranslucent)
    {
        dwExStyle |= WS_EX_LAYERED;
    }
    
    if(m_hostAttr.m_dwStyle!=0) dwStyle=m_hostAttr.m_dwStyle&(~WS_VISIBLE);
    if(m_hostAttr.m_dwExStyle != 0) dwExStyle =m_hostAttr.m_dwExStyle;
    
    ModifyStyle(0,dwStyle);
    ModifyStyleEx(0,dwExStyle);
	SStringT strTitle = m_hostAttr.m_strTitle.GetText(FALSE);
    SNativeWnd::SetWindowText(strTitle);
    
    if(m_hostAttr.m_bTranslucent)
    {
        SetWindowLongPtr(GWL_EXSTYLE, GetWindowLongPtr(GWL_EXSTYLE) | WS_EX_LAYERED);
		m_dummyWnd = new SDummyWnd(this);
        m_dummyWnd->Create(strTitle,WS_POPUP,WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,0,0,10,10,NULL,NULL);
        m_dummyWnd->SetWindowLongPtr(GWL_EXSTYLE,m_dummyWnd->GetWindowLongPtr(GWL_EXSTYLE) | WS_EX_LAYERED);
        ::SetLayeredWindowAttributes(m_dummyWnd->m_hWnd,0,0,LWA_ALPHA);
        m_dummyWnd->ShowWindow(SW_SHOWNOACTIVATE);
    }else if(dwExStyle & WS_EX_LAYERED || m_hostAttr.m_byAlpha!=0xFF)
    {
        if(!(dwExStyle & WS_EX_LAYERED)) ModifyStyleEx(0,WS_EX_LAYERED);
        ::SetLayeredWindowAttributes(m_hWnd,0,m_hostAttr.m_byAlpha,LWA_ALPHA);
    }
    if(m_hostAttr.m_hAppIconSmall)
    {
        SendMessage(WM_SETICON,FALSE,(LPARAM)m_hostAttr.m_hAppIconSmall);
    }
    if(m_hostAttr.m_hAppIconBig)
    {
        SendMessage(WM_SETICON,TRUE,(LPARAM)m_hostAttr.m_hAppIconBig);
    }

	//compatible with 2.x width and height properties in soui element.
	pugi::xml_attribute attrWid = xmlNode.attribute(L"width");
	pugi::xml_attribute attrHei = xmlNode.attribute(L"height");
	pugi::xml_node xmlRoot = xmlNode.child(L"root");
	if (attrWid && !xmlRoot.attribute(attrWid.name()))
		xmlRoot.append_copy(attrWid);
	if (attrHei && !xmlRoot.attribute(attrHei.name()))
		xmlRoot.append_copy(attrHei);

    SWindow::InitFromXml(xmlNode.child(L"root"));
    BuildWndTreeZorder();

	int nWidth = m_szAppSetted.cx;
	int nHeight = m_szAppSetted.cy;
	if(nWidth <= 0 || nHeight <= 0)
	{//计算出root大小		
		if (nWidth <= 0)
		{
			nWidth = SIZE_WRAP_CONTENT;
		}
		if (nHeight <= 0)
		{
			nHeight = SIZE_WRAP_CONTENT;
		}

		CSize szRoot = GetDesiredSize(nWidth, nHeight);
		if(nWidth == SIZE_WRAP_CONTENT) 
			nWidth = szRoot.cx;
		if(nHeight == SIZE_WRAP_CONTENT)
			nHeight = szRoot.cy;
		GetEventSet()->subscribeEvent(EventSwndSize::EventID, Subscriber(&SHostWnd::onRootResize, this));
	}

	if(nWidth != m_szAppSetted.cx || nHeight != m_szAppSetted.cy)
	{
		m_nAutoSizing++;
		SetWindowPos(NULL,0,0,nWidth,nHeight,SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
		m_nAutoSizing--;
	}

    CRect rcClient;
    SNativeWnd::GetClientRect(&rcClient);
    OnRelayout(rcClient);

    //设置重绘标记
    m_bNeedAllRepaint = TRUE;
    m_bNeedRepaint = TRUE;
    m_rgnInvalidate->Clear();
    
    EventInit evt(this);
    FireEvent(evt);

    return TRUE;
}

void SHostWnd::OnWindowTextChanged(LPCTSTR pszTitle)
{
	if (m_dummyWnd)
	{
		m_dummyWnd->SetWindowText(pszTitle);
	}
}

void SHostWnd::_Redraw()
{
    m_bNeedAllRepaint = TRUE;
    m_bNeedRepaint = TRUE;
    m_rgnInvalidate->Clear();
	_Invalidate(NULL);
}

void SHostWnd::OnPrint(HDC dc, UINT uFlags)
{
    //刷新前重新布局，会自动检查布局脏标志
	UpdateLayout();
    
    if (m_bNeedAllRepaint)
    {
        m_rgnInvalidate->Clear();
        m_bNeedAllRepaint = FALSE;
        m_bNeedRepaint=TRUE;
    }

    CRect rcInvalid;
	CRect rcWnd = SWindow::GetWindowRect();
    if (m_bNeedRepaint)
    {
        m_bNeedRepaint = FALSE;

        SPainter painter;
        BeforePaint(m_memRT,painter);

        //m_rgnInvalidate有可能在RedrawRegion时被修改，必须生成一个临时的区域对象
        SAutoRefPtr<IRegion> pRgnUpdate=m_rgnInvalidate;
        m_rgnInvalidate=NULL;
        GETRENDERFACTORY->CreateRegion(&m_rgnInvalidate);

        if (!pRgnUpdate->IsEmpty())
        {
            pRgnUpdate->GetRgnBox(&rcInvalid);
			rcInvalid.IntersectRect(rcInvalid, rcWnd);
            m_memRT->PushClipRegion(pRgnUpdate,RGN_COPY);
        }else
        {
            rcInvalid= rcWnd;
            m_memRT->PushClipRect(&rcInvalid,RGN_COPY);
        }
        //清除残留的alpha值
        m_memRT->ClearRect(rcInvalid,0);

        BuildWndTreeZorder();
        RedrawRegion(m_memRT, pRgnUpdate);
        
        m_memRT->PopClip();
        
        AfterPaint(m_memRT,painter);
        
    }else
    {//缓存已经更新好了，只需要重新更新到窗口
        m_rgnInvalidate->GetRgnBox(&rcInvalid);
        m_rgnInvalidate->Clear();
    }
    
    if(uFlags != KConstDummyPaint) //由系统发的WM_PAINT或者WM_PRINT产生的重绘请求
    {
        rcInvalid = rcWnd;
    }
    
    //渲染非背景混合窗口,设置m_bRending=TRUE以保证只执行一次UpdateHost
    m_bRendering = TRUE;
    _UpdateNonBkgndBlendSwnd();
    SPOSITION pos = m_lstUpdatedRect.GetHeadPosition();
    while(pos)
    {
        rcInvalid = rcInvalid | m_lstUpdatedRect.GetNext(pos);
    }
    m_lstUpdatedRect.RemoveAll();
    m_bRendering = FALSE;

    UpdateHost(dc,rcInvalid);
}

void SHostWnd::OnPaint(HDC dc)
{
    PAINTSTRUCT ps;
    dc=::BeginPaint(m_hWnd, &ps);
    OnPrint(m_hostAttr.m_bTranslucent?NULL:dc, 0);
    ::EndPaint(m_hWnd, &ps);
}

BOOL SHostWnd::OnEraseBkgnd(HDC dc)
{
    return TRUE;
}


IToolTip * SHostWnd::CreateTooltip() const
{
	return GETTOOLTIPFACTORY->CreateToolTip(m_hWnd);
}

void SHostWnd::DestroyTooltip(IToolTip * pTooltip) const
{
	GETTOOLTIPFACTORY->DestroyToolTip(pTooltip);
}

int SHostWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	m_nAutoSizing++;
    GETRENDERFACTORY->CreateRenderTarget(&m_memRT,0,0);
    GETRENDERFACTORY->CreateRegion(&m_rgnInvalidate);
    m_pTipCtrl = CreateTooltip();
    if(m_pTipCtrl) GetMsgLoop()->AddMessageFilter(m_pTipCtrl);
    
	m_szAppSetted.cx = lpCreateStruct->cx;
	m_szAppSetted.cy = lpCreateStruct->cy;
    SWindow::SetContainer(this);

	if(!m_strXmlLayout.IsEmpty())
	{
		pugi::xml_document xmlDoc;
		LOADXML(xmlDoc,m_strXmlLayout);
		if(xmlDoc)
		{
			InitFromXml(xmlDoc.child(L"SOUI"));
		}else
		{
			SASSERT_FMTA(FALSE,"Load layout [%s] Failed",S_CT2A(m_strXmlLayout));
		}
	}
	m_nAutoSizing--;
    return 0;
}

void SHostWnd::OnDestroy()
{
    EventExit evt(this);
    FireEvent(evt);

    SWindow::SSendMessage(WM_DESTROY);

    if(m_pTipCtrl)
    {
        GetMsgLoop()->RemoveMessageFilter(m_pTipCtrl);
		DestroyTooltip(m_pTipCtrl);
        m_pTipCtrl = NULL;
    }
    if(m_dummyWnd)
    {
        m_dummyWnd->DestroyWindow();
		delete m_dummyWnd;
		m_dummyWnd = NULL;
    }

	m_memRT = NULL;
	m_rgnInvalidate = NULL;

    //exit app. (copy from wtl)
    if(m_hostAttr.m_byWndType == SHostWndAttr::WT_APPMAIN 
    || (m_hostAttr.m_byWndType == SHostWndAttr::WT_UNDEFINE && (SNativeWnd::GetStyle() & (WS_CHILD | WS_POPUP)) == 0 && (SNativeWnd::GetExStyle()&WS_EX_TOOLWINDOW) == 0))
        ::PostQuitMessage(1);
}

void SHostWnd::OnSize(UINT nType, CSize size)
{
    if(IsIconic()) return;

    if (size.cx==0 || size.cy==0)
        return;
    if(m_nAutoSizing==0)
	{
		m_szAppSetted = size;
	}
    m_bResizing = TRUE;
    m_memRT->Resize(size);
	OnRelayout(CRect(0, 0, size.cx, size.cy));
    _Redraw();
    m_bResizing = FALSE;
}

void SHostWnd::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_bTrackFlag)
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE;
        tme.dwHoverTime = 0;
        m_bTrackFlag = TrackMouseEvent(&tme);
    }
    OnMouseEvent(WM_MOUSEMOVE,nFlags,MAKELPARAM(point.x,point.y));
}

void SHostWnd::OnMouseLeave()
{
    m_bTrackFlag = FALSE;
    OnMouseEvent(WM_MOUSELEAVE,0,0);
}

BOOL SHostWnd::OnSetCursor(HWND hwnd, UINT nHitTest, UINT message)
{
    if(hwnd!=m_hWnd) return FALSE;
    if(nHitTest==HTCLIENT)
    {
        CPoint pt;
        GetCursorPos(&pt);
        ScreenToClient(&pt);
        return DoFrameEvent(WM_SETCURSOR,0,MAKELPARAM(pt.x,pt.y))!=0;
    }
    return DefWindowProc()!=0;
}

void SHostWnd::OnTimer(UINT_PTR idEvent)
{
	if(idEvent == kPulseTimer)
	{
		if (!IsIconic())
		{
			SwndContainerImpl::OnNextFrame();
		}
		return;
	}

    STimerID sTimerID((DWORD)idEvent);
    if(sTimerID.bSwndTimer)
    {
        SWindow *pSwnd=SWindowMgr::GetWindow((SWND)sTimerID.swnd);
        if(pSwnd)
        {
            pSwnd->SSendMessage(WM_TIMER,sTimerID.uTimerID,0);
        }
        else
        {
            //窗口已经删除，自动清除该窗口的定时器
            ::KillTimer(m_hWnd,idEvent);
        }
    }
    else
    {
        SetMsgHandled(FALSE);
    }
}


LRESULT SHostWnd::OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
        m_msgMouse.message = uMsg;
        m_msgMouse.wParam = wParam;
        m_msgMouse.lParam = lParam;
        if(SNativeWnd::GetStyle()&WS_CHILD) SNativeWnd::SetFocus();//子窗口情况下才自动获取焦点
        break;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        m_msgMouse.message = 0;
        break;
    case WM_MOUSEMOVE:
        m_msgMouse.message = 0;
        m_msgMouse.wParam = wParam;
        m_msgMouse.lParam   = lParam;
        break;
    case WM_MOUSELEAVE:
        m_msgMouse.message = uMsg;
        m_msgMouse.wParam = 0;
        m_msgMouse.lParam   = 0;
        break;
    default:
        break;
    }

    DoFrameEvent(uMsg,wParam,lParam);    //将鼠标消息转发到SWindow处理

    UpdateTooltip();

    return 0;
}

LRESULT SHostWnd::OnKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg==WM_SYSKEYDOWN || uMsg==WM_SYSKEYUP)
    {
        SWindow *pFocus=SWindowMgr::GetWindow(m_focusMgr.GetFocusedHwnd());
        if(!pFocus  || !(pFocus->OnGetDlgCode()&SC_WANTSYSKEY))
        {
            SetMsgHandled(FALSE);
            return 0;
        }
    }
    LRESULT lRet = DoFrameEvent(uMsg,wParam,lParam);
    SetMsgHandled(SWindow::IsMsgHandled());
    return lRet;
}

LRESULT SHostWnd::OnHostMsg( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return DoFrameEvent(uMsg,wParam,lParam);
}

BOOL SHostWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    ScreenToClient(&pt);
    return DoFrameEvent(WM_MOUSEWHEEL,MAKEWPARAM(nFlags,zDelta),MAKELPARAM(pt.x,pt.y))!=0;
}


void SHostWnd::OnActivate( UINT nState, BOOL bMinimized, HWND wndOther )
{
    if(nState!=WA_INACTIVE)
        ::SetFocus(m_hWnd);
    else
    {
        ::SetFocus(NULL);
        _RestoreClickState();
    }
}

BOOL SHostWnd::OnFireEvent(EventArgs &evt)
{
    return _HandleEvent(&evt);
}

CRect SHostWnd::GetContainerRect() const
{
	return SWindow::GetWindowRect();
}

HWND SHostWnd::GetHostHwnd()
{
    return m_hWnd;
}

IRenderTarget * SHostWnd::OnGetRenderTarget(const CRect & rc,GrtFlag gdcFlags)
{
	IRenderTarget *pRT = NULL;
	if (gdcFlags == GRT_NODRAW)
	{
		GETRENDERFACTORY->CreateRenderTarget(&pRT, 0, 0);
		pRT->OffsetViewportOrg(-rc.left, -rc.top);
		pRT->ClearRect(rc, 0);
	}
	else
	{
		pRT = m_memRT;
		pRT->PushClipRect(rc, RGN_COPY);
		pRT->AddRef();
	}
    return pRT;
}

void SHostWnd::OnReleaseRenderTarget(IRenderTarget * pRT,const CRect &rc,GrtFlag gdcFlags)
{
    if(gdcFlags != GRT_NODRAW)
    {
		pRT->PopClip();
        if(!m_bRendering)
        {
            HDC dc=GetDC();
            UpdateHost(dc,rc);
            ReleaseDC(dc);
        }else
        {
            m_lstUpdatedRect.AddTail(rc);
        }
    }
    pRT->Release();
}

void SHostWnd::UpdateHost(HDC dc, const CRect &rcInvalid )
{
    if(m_hostAttr.m_bTranslucent)
    {
        SASSERT(m_hostAttr.m_byAlpha>5);
        UpdateLayerFromRenderTarget(m_memRT,m_hostAttr.m_byAlpha,&rcInvalid);
    }
    else
    {
        HDC hdc=m_memRT->GetDC(0);
        ::BitBlt(dc,rcInvalid.left,rcInvalid.top,rcInvalid.Width(),rcInvalid.Height(),hdc,rcInvalid.left,rcInvalid.top,SRCCOPY);
        m_memRT->ReleaseDC(hdc);
    }
}

void SHostWnd::OnRedraw(const CRect &rc)
{
    if(!IsWindow()) return;
    
    m_rgnInvalidate->CombineRect(&rc,RGN_OR);
    
    m_bNeedRepaint = TRUE;

	_Invalidate(rc);
}

BOOL SHostWnd::OnReleaseSwndCapture()
{
    if(!SwndContainerImpl::OnReleaseSwndCapture()) return FALSE;
    if(::GetCapture() == m_hWnd)
        ::ReleaseCapture();
    CPoint pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);
    PostMessage(WM_MOUSEMOVE,0,MAKELPARAM(pt.x,pt.y));
    return TRUE;
}

SWND SHostWnd::OnSetSwndCapture(SWND swnd)
{
    SNativeWnd::SetCapture();
    return SwndContainerImpl::OnSetSwndCapture(swnd);
}

BOOL SHostWnd::IsTranslucent() const
{
    return m_hostAttr.m_bTranslucent;
}


BOOL SHostWnd::IsSendWheel2Hover() const
{
    return m_hostAttr.m_bSendWheel2Hover;
}

BOOL SHostWnd::UpdateWindow()
{
    if(m_bResizing) return FALSE;
    if(m_dummyWnd) 
		::UpdateWindow(m_dummyWnd->m_hWnd);
    else 
		::UpdateWindow(m_hWnd);
    return TRUE;
}

void SHostWnd::UpdateTooltip()
{
    if(!m_pTipCtrl) return;
    SWindow *pHover=SWindowMgr::GetWindow(m_hHover);
    if(m_msgMouse.message != 0 || !pHover )
    {
        m_pTipCtrl->ClearTip();
    }
    else
    {
        CPoint pt(GET_X_LPARAM(m_msgMouse.lParam), GET_Y_LPARAM(m_msgMouse.lParam));
        SwndToolTipInfo tipInfo;
        BOOL bOK=pHover->OnUpdateToolTip(pt,tipInfo);
        if(bOK)
        {
            TIPID id={tipInfo.swnd,tipInfo.dwCookie};
            m_pTipCtrl->UpdateTip(id,tipInfo.rcTarget,tipInfo.strTip,GetScale());
        }else
        {//hide tooltip
            m_pTipCtrl->ClearTip();
        }
    }
}

LRESULT SHostWnd::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
{
    if (bCalcValidRects && (SNativeWnd::GetStyle() & WS_CHILDWINDOW))
    { 
        //子窗口，相对于父窗口坐标
        LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;

        if (SWP_NOSIZE == (SWP_NOSIZE & pParam->lppos->flags))
            return 0;

        CRect rcWindow;
        SNativeWnd::GetWindowRect(rcWindow);
        POINT point = {rcWindow.left, rcWindow.top};
        ::ScreenToClient(::GetParent(this->m_hWnd),&point); 
        int w = rcWindow.Width();
        int h = rcWindow.Height();

        rcWindow.left   =  point.x;
        rcWindow.top    = point.y;
        rcWindow.right  = rcWindow.left+w;
        rcWindow.bottom = rcWindow.top+h;

        if (0 == (SWP_NOMOVE & pParam->lppos->flags))
        {
            rcWindow.left = pParam->lppos->x;
            rcWindow.top = pParam->lppos->y;
        }

        rcWindow.right = rcWindow.left + pParam->lppos->cx;
        rcWindow.bottom = rcWindow.top + pParam->lppos->cy;
        pParam->rgrc[0] = rcWindow;
     }
     else if(bCalcValidRects)
     {
        //top-level 相对于屏幕坐标 
        LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;

        if (SWP_NOSIZE == (SWP_NOSIZE & pParam->lppos->flags))
            return 0;

        CRect rcWindow;
        SNativeWnd::GetWindowRect(rcWindow);
        rcWindow = CRect(rcWindow.TopLeft(),CSize(pParam->lppos->cx,pParam->lppos->cy));
        if (0 == (SWP_NOMOVE & pParam->lppos->flags))
        {
            rcWindow.MoveToXY(pParam->lppos->x,pParam->lppos->y);
        }
        pParam->rgrc[0] = rcWindow;
    }else
    {
        SNativeWnd::GetWindowRect((LPRECT)lParam);
    }

    return 0;
}

void SHostWnd::OnGetMinMaxInfo(LPMINMAXINFO lpMMI)
{
    HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONULL);

    if (hMonitor)
    {
        MONITORINFO mi = {sizeof(MONITORINFO)};
        ::GetMonitorInfo(hMonitor, &mi);

        CRect rcWork = mi.rcWork, rcMonitor = mi.rcMonitor;
		CRect rcMaxInset = m_hostAttr.GetMaxInset(GetScale());
        lpMMI->ptMaxPosition.x = abs(rcWork.left - rcMonitor.left) - rcMaxInset.left;
        lpMMI->ptMaxPosition.y = abs(rcWork.top - rcMonitor.top) - rcMaxInset.top;
        lpMMI->ptMaxSize.x = abs(rcWork.Width())+ rcMaxInset.right + rcMaxInset.left;
        lpMMI->ptMaxSize.y = abs(rcWork.Height())  + rcMaxInset.bottom + rcMaxInset.top;
        lpMMI->ptMaxTrackSize.x = abs(rcWork.Width())  + rcMaxInset.right + rcMaxInset.left;
        lpMMI->ptMaxTrackSize.y = abs(rcWork.Height())  + rcMaxInset.bottom + rcMaxInset.top;
		CSize szMin = m_hostAttr.GetMinSize(GetScale());
        lpMMI->ptMinTrackSize = CPoint(szMin.cx, szMin.cy);
    }
}

BOOL SHostWnd::OnNcActivate(BOOL bActive)
{
    return TRUE;
}

UINT SHostWnd::OnWndNcHitTest(CPoint point)
{
    if (m_hostAttr.m_bResizable && !::IsZoomed(m_hWnd))
    {
        ScreenToClient(&point);
		CRect rcMargin = m_hostAttr.GetMargin(GetScale());
		CRect rcWnd = SWindow::GetWindowRect();
        if (point.x > rcWnd.right - rcMargin.right)
        {
            if (point.y > rcWnd.bottom - rcMargin.bottom)
            {
                return HTBOTTOMRIGHT;
            }
            else if (point.y < rcMargin.top)
            {
                return HTTOPRIGHT;
            }
            return HTRIGHT;
        }
        else if (point.x < rcMargin.left)
        {
            if (point.y > rcWnd.bottom - rcMargin.bottom)
            {
                return HTBOTTOMLEFT;
            }
            else if (point.y < rcMargin.top)
            {
                return HTTOPLEFT;
            }
            return HTLEFT;
        }
        else if (point.y > rcWnd.bottom - rcMargin.bottom)
        {
            return HTBOTTOM;
        }
        else if (point.y < rcMargin.top)
        {
            return HTTOP;
        }
    }
    return HTCLIENT;
}

void SHostWnd::OnSetFocus( HWND wndOld )
{
    DoFrameEvent(WM_SETFOCUS,0,0);
}

void SHostWnd::OnKillFocus( HWND wndFocus )
{
    DoFrameEvent(WM_KILLFOCUS,0,0);
}

void SHostWnd::UpdateLayerFromRenderTarget(IRenderTarget *pRT,BYTE byAlpha, LPCRECT prcDirty)
{
    SASSERT(IsTranslucent());
    CRect rc;
    SNativeWnd::GetWindowRect(&rc);
    CRect rcDirty = prcDirty? (*prcDirty): CRect(0,0,rc.Width(),rc.Height());
    BLENDFUNCTION bf= {AC_SRC_OVER,0,byAlpha,AC_SRC_ALPHA};        
    
    //注意：下面这几个参数不能直接在info参数里直接使用&rc.Size()，否则会被编译器优化掉，导致参数错误
    CPoint ptDst = rc.TopLeft();
    CSize  szDst = rc.Size();
    CPoint ptSrc;
    
    HDC hdc=pRT->GetDC(0);
    S_UPDATELAYEREDWINDOWINFO info={sizeof(info), NULL, &ptDst, &szDst,hdc, &ptSrc, 0, &bf, ULW_ALPHA, &rcDirty};
    SWndSurface::SUpdateLayeredWindowIndirect(m_hWnd,&info);
    pRT->ReleaseDC(hdc);
}

BOOL _BitBlt(IRenderTarget *pRTDst,IRenderTarget * pRTSrc,CRect rcDst,CPoint ptSrc)
{
    return S_OK == pRTDst->BitBlt(&rcDst,pRTSrc,ptSrc.x,ptSrc.y,SRCCOPY);
}


BOOL SHostWnd::AnimateHostWindow(DWORD dwTime,DWORD dwFlags)
{
    if(!IsTranslucent())
    {
        return ::AnimateWindow(m_hWnd,dwTime,dwFlags);
    }else
    {
        CRect rcWnd;//窗口矩形
        SNativeWnd::GetClientRect(&rcWnd);
        CRect rcShow(rcWnd);//动画过程中可见部分
        
        SAutoRefPtr<IRenderTarget> pRT;
        GETRENDERFACTORY->CreateRenderTarget(&pRT,rcShow.Width(),rcShow.Height());
        
        _Redraw();
        RedrawRegion(m_memRT,m_rgnInvalidate);

        int nSteps=dwTime/10;
        if(dwFlags & AW_HIDE)
        {
            if(dwFlags& AW_SLIDE)
            {
                LONG  x1 = rcShow.left;
                LONG  x2 = rcShow.left;
                LONG  y1 = rcShow.top;
                LONG  y2 = rcShow.top;
                LONG * x =&rcShow.left;
                LONG * y =&rcShow.top;

                if(dwFlags & AW_HOR_POSITIVE)
                {//left->right:move left
                    x1=rcShow.left,x2=rcShow.right;
                    x=&rcShow.left;
                }else if(dwFlags & AW_HOR_NEGATIVE)
                {//right->left:move right
                    x1=rcShow.right,x2=rcShow.left;
                    x=&rcShow.right;
                }
                if(dwFlags & AW_VER_POSITIVE)
                {//top->bottom
                    y1=rcShow.top,y2=rcShow.bottom;
                    y=&rcShow.top;
                }else if(dwFlags & AW_VER_NEGATIVE)
                {//bottom->top
                    y1=rcShow.bottom,y2=rcShow.top;
                    y=&rcShow.bottom;
                }
                LONG xStepLen=(x2-x1)/nSteps;
                LONG yStepLen=(y2-y1)/nSteps;

                for(int i=0;i<nSteps;i++)
                {
                    *x+=xStepLen;
                    *y+=yStepLen;
                    pRT->ClearRect(rcWnd,0);
                    CPoint ptAnchor;
                    if(dwFlags & AW_VER_NEGATIVE)
                        ptAnchor.y=rcWnd.bottom-rcShow.Height();
                    if(dwFlags & AW_HOR_NEGATIVE)
                        ptAnchor.x=rcWnd.right-rcShow.Width();
                    _BitBlt(pRT,m_memRT,rcShow,ptAnchor);
                    UpdateLayerFromRenderTarget(pRT,m_hostAttr.m_byAlpha);
                    Sleep(10);
                }
                ShowWindow(SW_HIDE);
                return TRUE;
            }else if(dwFlags&AW_CENTER)
            {
                int xStep=rcShow.Width()/(2*nSteps);
                int yStep=rcShow.Height()/(2*nSteps);
                for(int i=0;i<nSteps;i++)
                {
                    rcShow.DeflateRect(xStep,yStep);
                    pRT->ClearRect(rcWnd,0);
                    _BitBlt(pRT,m_memRT,rcShow,rcShow.TopLeft());
                    UpdateLayerFromRenderTarget(pRT,m_hostAttr.m_byAlpha);
                    Sleep(10);
                }
                ShowWindow(SW_HIDE);
                return TRUE;
            }else if(dwFlags&AW_BLEND)
            {
                BYTE byAlpha=255;
                for(int i=0;i<nSteps;i++)
                {
                    byAlpha-=255/nSteps;
                    UpdateLayerFromRenderTarget(m_memRT,byAlpha);
                    Sleep(10);
                }
                ShowWindow(SW_HIDE);
                return TRUE;
            }
            return FALSE;
        }else
        {
            pRT->ClearRect(rcWnd,0);
            UpdateLayerFromRenderTarget(pRT, m_hostAttr.m_byAlpha);
            SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE|SWP_SHOWWINDOW|((dwFlags&AW_ACTIVATE)?0:SWP_NOACTIVATE));
            
            if(dwFlags& AW_SLIDE)
            {
                LONG  x1 = rcShow.left;
                LONG  x2 = rcShow.left;
                LONG  y1 = rcShow.top;
                LONG  y2 = rcShow.top;
                LONG * x =&rcShow.left;
                LONG * y =&rcShow.top;
                
                if(dwFlags & AW_HOR_POSITIVE)
                {//left->right:move right
                    x1=rcShow.left,x2=rcShow.right;
                    rcShow.right=rcShow.left,x=&rcShow.right;
                }else if(dwFlags & AW_HOR_NEGATIVE)
                {//right->left:move left
                    x1=rcShow.right,x2=rcShow.left;
                    rcShow.left=rcShow.right,x=&rcShow.left;
                }
                if(dwFlags & AW_VER_POSITIVE)
                {//top->bottom
                    y1=rcShow.top,y2=rcShow.bottom;
                    rcShow.bottom=rcShow.top,y=&rcShow.bottom;
                }else if(dwFlags & AW_VER_NEGATIVE)
                {//bottom->top
                    y1=rcShow.bottom,y2=rcShow.top;
                    rcShow.top=rcShow.bottom,y=&rcShow.top;
                }
                LONG xStepLen=(x2-x1)/nSteps;
                LONG yStepLen=(y2-y1)/nSteps;
                
                for(int i=0;i<nSteps;i++)
                {
                    *x+=xStepLen;
                    *y+=yStepLen;
                    pRT->ClearRect(rcWnd,0);
                    CPoint ptAnchor;
                    if(dwFlags & AW_VER_POSITIVE)
                        ptAnchor.y=rcWnd.bottom-rcShow.Height();
                    if(dwFlags & AW_HOR_POSITIVE)
                        ptAnchor.x=rcWnd.right-rcShow.Width();
                     _BitBlt(pRT,m_memRT,rcShow,ptAnchor);
                    UpdateLayerFromRenderTarget(pRT, m_hostAttr.m_byAlpha);
                    Sleep(10);
                }
                UpdateLayerFromRenderTarget(m_memRT,m_hostAttr.m_byAlpha);
                
            
                return TRUE;
            }else if(dwFlags&AW_CENTER)
            {
                int xStep=rcShow.Width()/(2*nSteps);
                int yStep=rcShow.Height()/(2*nSteps);
                rcShow.left=rcShow.right=(rcShow.left+rcShow.right)/2;
                rcShow.top=rcShow.bottom=(rcShow.top+rcShow.bottom)/2;
                for(int i=0;i<nSteps;i++)
                {
                    rcShow.InflateRect(xStep,yStep);
                    pRT->ClearRect(rcWnd,0);
                    _BitBlt(pRT,m_memRT,rcShow,rcShow.TopLeft());
                    UpdateLayerFromRenderTarget(pRT,m_hostAttr.m_byAlpha);
                    Sleep(10);
                }
                UpdateLayerFromRenderTarget(m_memRT,m_hostAttr.m_byAlpha);
                return TRUE;
            }else if(dwFlags&AW_BLEND)
            {
                BYTE byAlpha=0;
                for(int i=0;i<nSteps;i++)
                {
                    byAlpha+=255/nSteps;
                    UpdateLayerFromRenderTarget(m_memRT,byAlpha);
                    Sleep(10);
                }
                UpdateLayerFromRenderTarget(m_memRT,m_hostAttr.m_byAlpha);
                return TRUE;
            }
        }
        return FALSE;
    }
}

BOOL SHostWnd::RegisterTimelineHandler( ITimelineHandler *pHandler )
{
	bool bEmpty1=m_timelineHandlerMgr.IsEmpty();
    BOOL bRet = SwndContainerImpl::RegisterTimelineHandler(pHandler);
	bool bEmpty2 = m_timelineHandlerMgr.IsEmpty();
	if(bEmpty1 && !bEmpty2)
	{
		SNativeWnd::SetTimer(kPulseTimer, kPulseInterval, NULL);
	}
    return bRet;
}

BOOL SHostWnd::UnregisterTimelineHandler( ITimelineHandler *pHandler )
{
	bool bEmpty1=m_timelineHandlerMgr.IsEmpty();
	BOOL bRet = SwndContainerImpl::UnregisterTimelineHandler(pHandler);
	bool bEmpty2 = m_timelineHandlerMgr.IsEmpty();
	if(!bEmpty1 && bEmpty2)
	{
		SNativeWnd::KillTimer(kPulseTimer);
	}
    return bRet;
}


const SStringW & SHostWnd::GetTranslatorContext() const
{
    return m_hostAttr.m_strTrCtx;
}

SMessageLoop * SHostWnd::GetMsgLoop()
{
    return SApplication::getSingletonPtr()->GetMsgLoop();
}

#ifndef DISABLE_SWNDSPY

LRESULT SHostWnd::OnSpyMsgSwndEnum(UINT uMsg, WPARAM wParam,LPARAM lParam )
{
    if(!m_hostAttr.m_bAllowSpy) return 0;
    SWND swndCur=(SWND)wParam;
    if(swndCur==0) swndCur=m_swnd;
    SWindow *pSwnd = SWindowMgr::GetWindow(swndCur);
    if(!pSwnd) return 0;
    SWindow *pRet = pSwnd->GetWindow((int)lParam);
    if(!pRet) return 0;
    return pRet->GetSwnd();
}

LRESULT SHostWnd::OnSpyMsgSwndSpy(UINT uMsg, WPARAM wParam,LPARAM lParam )
{
    if(!m_hostAttr.m_bAllowSpy) return 0;
    SWND swndCur=(SWND)wParam;
    if(swndCur==0) swndCur=m_swnd;
    SWindow *pSwnd = SWindowMgr::GetWindow(swndCur);
    if(!pSwnd) return 0;
    
    SWNDINFO *pSwndInfo=new SWNDINFO;
    COPYDATASTRUCT cds;
    cds.dwData = SPYMSG_SWNDINFO;
    cds.cbData = sizeof(SWNDINFO);
    cds.lpData = pSwndInfo;
    memset(pSwndInfo,0,sizeof(SWNDINFO));
    
    pSwndInfo->swnd = swndCur;
    pSwnd->GetWindowRect(&pSwndInfo->rcWnd);
    pSwnd->GetClientRect(&pSwndInfo->rcClient);
    pSwndInfo->bVisible=pSwnd->IsVisible(TRUE);
    pSwndInfo->nID = pSwnd->GetID();

    SStringW strTmp=pSwnd->GetName();
    if(strTmp.GetLength()<=SWND_MAX_NAME)
        wcscpy(pSwndInfo->szName,strTmp);
    else
        wcscpy(pSwndInfo->szName,L"##buf overflow!");
    
    strTmp = pSwnd->GetObjectClass();
    if(strTmp.GetLength()<=SWND_MAX_CLASS)
        wcscpy(pSwndInfo->szClassName,strTmp);
    else
        wcscpy(pSwndInfo->szClassName,L"##buf overflow!");
    
    wcscpy(pSwndInfo->szXmlStr,L"##unavailable!");
#ifdef _DEBUG
    strTmp=pSwnd->m_strXml;
    if(strTmp.GetLength()<=SWND_MAX_XML)
        wcscpy(pSwndInfo->szXmlStr,strTmp);
    else
        wcscpy(pSwndInfo->szXmlStr,L"##buf overflow!");
#endif//_DEBUG
    ::SendMessage(m_hSpyWnd,WM_COPYDATA,(WPARAM)m_hWnd,(LPARAM)&cds);
    delete pSwndInfo;
    return 1;
}

LRESULT SHostWnd::OnSpyMsgSetSpy( UINT uMsg,WPARAM wParam,LPARAM lParam )
{
    m_hSpyWnd = (HWND)lParam;
    if(!::IsWindow(m_hSpyWnd))
    {
        m_hSpyWnd = 0;
        return 0;
    }
    return 1;
}

LRESULT SHostWnd::OnSpyMsgHitTest( UINT uMsg,WPARAM wParam,LPARAM lParam )
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
    ScreenToClient(&pt);
    return SwndFromPoint(pt);
}
#endif//DISABLE_SWNDSPY

void SHostWnd::_UpdateNonBkgndBlendSwnd()
{
    SList<SWND> lstUpdateSwnd = m_lstUpdateSwnd;
    m_lstUpdateSwnd.RemoveAll();
    
    SPOSITION pos = lstUpdateSwnd.GetHeadPosition();
    while(pos)
    {
        SWindow *pWnd = SWindowMgr::getSingleton().GetWindow(lstUpdateSwnd.GetNext(pos));
        if(pWnd)
        {
            pWnd->_Update();
        }
    }
}


void SHostWnd::BeforePaint(IRenderTarget *pRT, SPainter &painter)
{
    pRT->SelectObject(SFontPool::getSingleton().GetFont(FF_DEFAULTFONT,GetScale()));
    pRT->SetTextColor(RGBA(0,0,0,255));
}

void SHostWnd::AfterPaint(IRenderTarget *pRT, SPainter &painter)
{
    pRT->SelectDefaultObject(OT_FONT);
}

void SHostWnd::UpdateLayout()
{
	if (!IsLayoutDirty()) 
		return;
	if ((m_szAppSetted.cx <=0 || m_szAppSetted.cy<=0) && GetLayoutParam()->IsWrapContent(Any))
	{
		int nWid = m_szAppSetted.cx<=0?SIZE_WRAP_CONTENT:m_szAppSetted.cx;
		int nHei = m_szAppSetted.cy<=0?SIZE_WRAP_CONTENT:m_szAppSetted.cy;
		CSize szRoot = GetDesiredSize(nWid, nHei);
		OnRelayout(CRect(CPoint(), szRoot));
	}
	else
	{
		SWindow::UpdateLayout();
	}
}

void SHostWnd::OnCaptureChanged( HWND wnd )
{
    if(wnd == m_hWnd) return;
    if(wnd!=NULL)
    {//如果当前响应了鼠标按下消息，在lost capture时也应该响应弹起消息
        TCHAR szClassName[30];
        ::GetClassName(wnd,szClassName,30);
        if(_tcscmp(szClassName,_T("CLIPBRDWNDCLASS"))==0)
            return;//在窗口内拖动时也可能产生capturechange消息。
    }
    _RestoreClickState();
}

IScriptModule * SHostWnd::GetScriptModule()
{
    return m_pScriptModule;
}

int SHostWnd::GetScale() const
{
	return m_nScale;
}

void SHostWnd::OnCavasInvalidate(SWND swnd)
{
	SASSERT(SWindowMgr::getSingleton().GetWindow(swnd));

	if (!m_lstUpdateSwnd.Find(swnd))
	{//防止重复加入
		if (m_lstUpdateSwnd.IsEmpty())
		{//请求刷新窗口
			_Invalidate(NULL);
		}
		m_lstUpdateSwnd.AddTail(swnd);
	}
}

LRESULT SHostWnd::OnScriptTimer( UINT uMsg,WPARAM wParam,LPARAM lParam )
{
    if(m_pScriptModule)
    {
        EventTimer evt(this);
		evt.uID = (UINT)wParam;
        m_pScriptModule->executeScriptedEventHandler((LPCSTR)lParam,&evt);
    }
    return 0;
}

UINT SHostWnd::setTimeout( LPCSTR pszScriptFunc,UINT uElapse )
{
    return SScriptTimer::getSingleton().SetTimer(m_hWnd,pszScriptFunc,uElapse,FALSE);
}

UINT SHostWnd::setInterval( LPCSTR pszScriptFunc,UINT uElapse )
{
    return SScriptTimer::getSingleton().SetTimer(m_hWnd,pszScriptFunc,uElapse,TRUE);
}

void SHostWnd::clearTimer( UINT uID )
{
    SScriptTimer::getSingleton().ClearTimer(uID);
}

void SHostWnd::RequestRelayout(SWND hSource,BOOL bSourceResizable)
{
	SWindow *pSource = SWindowMgr::GetWindow(hSource);
	SASSERT(pSource);

	if(m_layoutDirty != dirty_self)
		m_layoutDirty = (hSource == m_swnd || bSourceResizable ) ? dirty_self:dirty_child;

	CRect rcSour = pSource->GetWindowRect();
    while(rcSour.IsRectEmpty())
    {
        pSource = pSource->GetParent();
        if(!pSource) break;
        rcSour = pSource->GetWindowRect();
    }
    OnRedraw(rcSour);
}

//////////////////////////////////////////////////////////////////////////
BOOL SHostWnd::DestroyWindow()
{
    return SNativeWnd::DestroyWindow();
}

UINT_PTR SHostWnd::SetTimer(UINT_PTR nIDEvent,UINT nElapse)
{
    return SNativeWnd::SetTimer(nIDEvent,nElapse);
}

BOOL SHostWnd::KillTimer(UINT_PTR id)
{
    return SNativeWnd::KillTimer(id);
}

CRect SHostWnd::GetWindowRect() const
{
    CRect rc;
    SNativeWnd::GetWindowRect(&rc);
    return rc;
}

CRect SHostWnd::GetClientRect() const
{
    CRect rc;
    SNativeWnd::GetClientRect(&rc);
    return rc;
}

LRESULT SHostWnd::OnMenuExEvent(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    return _HandleEvent((EventArgs *)lParam);
}


void SHostWnd::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{//默认不处理该消息，同时防止系统处理该消息

}

LRESULT SHostWnd::OnGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef SOUI_ENABLE_ACC
	IAccessible * pAcc = NULL;
	DWORD dwObjectId = lParam;
	if (dwObjectId == OBJID_CLIENT) {
		pAcc = GetAccessible();
	} else{
		SWindow *pWnd = SWindowMgr::GetWindow(dwObjectId);
		if(pWnd) pAcc = pWnd->GetAccessible();//user swnd to identify a swindow.
	}

	if (pAcc != NULL) {
		return LresultFromObject(IID_IAccessible, wParam, pAcc);
	}
	else
#endif
	{
		SetMsgHandled(FALSE);
		return 0;
	}
}

void SHostWnd::_RestoreClickState()
{
    switch(m_msgMouse.message)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        SendMessage(WM_LBUTTONUP,m_msgMouse.wParam,m_msgMouse.lParam);
        break;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
        SendMessage(WM_RBUTTONUP,m_msgMouse.wParam,m_msgMouse.lParam);
        break;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
        SendMessage(WM_MBUTTONUP,m_msgMouse.wParam,m_msgMouse.lParam);
        break;
    default:
        break;
    }

}

void SHostWnd::_Invalidate(LPCRECT prc)
{
	if (!m_hostAttr.m_bTranslucent)
	{
		if(prc)
			SNativeWnd::InvalidateRect(prc, FALSE);
		else
			SNativeWnd::Invalidate(FALSE);
	}
	else if (m_dummyWnd)
	{
		m_dummyWnd->Invalidate(FALSE);
	}
}

HRESULT SHostWnd::OnLanguageChanged()
{
	SNativeWnd::SetWindowText(m_hostAttr.m_strTitle.GetText(FALSE));
	return 3;
}


void SHostWnd::OnScaleChanged(int scale)
{
	m_nScale = scale;
	m_layoutDirty = dirty_self;
	SWindow::InvalidateRect(NULL);
}


}//namespace SOUI

