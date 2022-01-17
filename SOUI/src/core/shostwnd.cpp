#include "souistd.h"

#include "SApp.h"
#include "core/SHostWnd.h"
#include "helper/SAutoBuf.h"
#include "helper/SColor.h"
#include "helper/SplitString.h"
#include "helper/STime.h"
#include "../updatelayeredwindow/SUpdateLayeredWindow.h"
#include <helper/SHostMgr.h>
#include <Imm.h>
#pragma comment(lib,"imm32.lib")

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

		void OnPaint(HDC dc)
		{
			PAINTSTRUCT ps;
			::BeginPaint(m_hWnd, &ps);
			::EndPaint(m_hWnd, &ps);
			m_pOwner->OnPrint(NULL,KConstDummyPaint);
		}

		void OnDestroy()
		{
			m_pOwner->m_dummyWnd = NULL;
		}

		virtual void OnFinalMessage(HWND hWnd)
		{
			delete this;
		}

		BEGIN_MSG_MAP_EX(SDummyWnd)
			MSG_WM_PAINT(OnPaint)
			MSG_WM_DESTROY(OnDestroy)
		END_MSG_MAP()
	private:
		SHostWnd *m_pOwner;
	};



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

void SHostWndAttr::SetTranslucent(bool bTranslucent)
{
	m_bTranslucent = bTranslucent;
}

void SHostWndAttr::SetTrCtx(const SStringW & strTrCtx)
{
	m_strTrCtx = strTrCtx;
}

void SHostWndAttr::SetSendWheel2Hover(bool value)
{
	m_bSendWheel2Hover = value;
}


//////////////////////////////////////////////////////////////////////////
// SRootWindow
//////////////////////////////////////////////////////////////////////////

SRootWindow::SRootWindow(SHostWnd *pHostWnd) :m_pHostWnd(pHostWnd)
{
	GetEventSet()->addEvent(EVENTID(EventInit));
	GetEventSet()->addEvent(EVENTID(EventExit));
}

void SRootWindow::OnAnimationInvalidate(IAnimation *pAni,bool bErase)
{
	if(bErase)
	{
		CRect rcWnd = GetClientRect();
		m_pHostWnd->m_memRT->ClearRect(rcWnd,0);
	}
	SWindow::OnAnimationInvalidate(pAni,bErase);
}

void SRootWindow::OnAnimationUpdate(IAnimation *pAni)
{
	Update();
}

void SRootWindow::OnAnimationStop(IAnimation *pAni)
{
	SWindow::OnAnimationStop(pAni);
	if(pAni == m_aniEnter || pAni==m_aniExit)
	{
		m_pHostWnd->m_AniState &=~Ani_win;
	}
	if(pAni == m_aniExit && m_pHostWnd->m_AniState==Ani_none)
	{
		m_pHostWnd->GetNative()->DestroyWindow();
	}
}



void SRootWindow::BeforePaint(IRenderTarget *pRT, SPainter &painter)
{
	int iState = SState2Index::GetDefIndex(GetState(),true);
	SwndStyle & style = SWindow::GetStyle();
	IFontPtr pFont = style.GetTextFont(iState);
	if(pFont) 
		pRT->SelectObject(pFont,(IRenderObj**)&painter.oldFont);
	else
		pRT->SelectObject(SFontPool::getSingleton().GetFont(FF_DEFAULTFONT,GetScale()),NULL);

	COLORREF crTxt =style.GetTextColor(iState);
	if(crTxt != CR_INVALID)
		painter.oldTextColor = pRT->SetTextColor(crTxt);
	else
		pRT->SetTextColor(RGBA(0,0,0,255));
}

void SRootWindow::AfterPaint(IRenderTarget *pRT, SPainter &painter)
{
	pRT->SelectDefaultObject(OT_FONT,NULL);
}

void SRootWindow::UpdateLayout()
{
	if (!IsLayoutDirty()) 
		return;
	SIZE szAppSet = m_pHostWnd->m_szAppSetted;
	if ((szAppSet.cx <=0 || szAppSet.cy<=0) && GetLayoutParam()->IsWrapContent(Any))
	{
		int nWid = szAppSet.cx;
		if(nWid<=0)
		{
			nWid = GetLayoutParam()->IsSpecifiedSize(Horz)?GetLayoutParam()->GetSpecifiedSize(Horz).toPixelSize(GetScale()):SIZE_WRAP_CONTENT;
		}
		int nHei = szAppSet.cy;
		if(nHei<=0)
		{
			nHei = GetLayoutParam()->IsSpecifiedSize(Vert)?GetLayoutParam()->GetSpecifiedSize(Vert).toPixelSize(GetScale()):SIZE_WRAP_CONTENT;
		}
		CSize szRoot = GetDesiredSize(nWid, nHei);
		OnRelayout(CRect(CPoint(), szRoot));
	}
	else
	{
		SWindow::UpdateLayout();
	}
}

HRESULT SRootWindow::OnLanguageChanged()
{
	m_pHostWnd->SetWindowText(m_pHostWnd->m_hostAttr.m_strTitle.GetText(FALSE));
	return 3;
}


void SRootWindow::OnScaleChanged(int scale)
{
	m_layoutDirty = dirty_self;
	SWindow::OnScaleChanged(scale);
	SWindow::InvalidateRect(NULL);
}


void SRootWindow::RequestRelayout(SWND hSource,BOOL bSourceResizable)
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
	m_pHostWnd->OnRedraw(rcSour);
}

SHostWnd * SRootWindow::GetHostWnd() const
{
	return m_pHostWnd;
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
, m_dwThreadID(0)
, m_AniState(0)
, m_pRoot(new SRootWindow(this))
{
	SwndContainerImpl::SetRoot(m_pRoot);
    m_msgMouse.message = 0;
    m_privateStylePool.Attach(new SStylePool);
    m_privateSkinPool.Attach(new SSkinPool);
	m_privateTemplatePool.Attach(new STemplatePool);
    m_pRoot->SetContainer(this);
	m_hostAnimationHandler.m_pHostWnd=this;
}

SHostWnd::~SHostWnd()
{
    GETSTYLEPOOLMGR->PopStylePool(m_privateStylePool);
    GETSKINPOOLMGR->PopSkinPool(m_privateSkinPool);
	GETTEMPLATEPOOLMR->PopTemplatePool(m_privateTemplatePool);
	delete m_pRoot;
}

HWND SHostWnd::Create(HWND hWndParent,DWORD dwStyle,DWORD dwExStyle, int x, int y, int nWidth, int nHeight)
{
    if (NULL != m_hWnd)
        return m_hWnd;
	UpdateAutoSizeCount(true);
    HWND hWnd = SNativeWnd::Create(_T("HOSTWND"),dwStyle,dwExStyle, x,y,nWidth,nHeight,hWndParent,NULL);
	UpdateAutoSizeCount(false);
	if(!hWnd) return NULL;

    if(nWidth==0 || nHeight==0) CenterWindow(hWndParent);
    return hWnd;
}

HWND SHostWnd::Create(HWND hWndParent,int x,int y,int nWidth,int nHeight)
{
    return Create(hWndParent, WS_CLIPCHILDREN | WS_TABSTOP | WS_OVERLAPPED,0,x,y,nWidth,nHeight);
}

BOOL SHostWnd::onRootResize( IEvtArgs *e )
{
	if (!m_bResizing)
	{
		EventSwndSize *e2 = sobj_cast<EventSwndSize>(e);
		UpdateAutoSizeCount(true);
		CRect rcWnd(CPoint(),e2->szWnd);
		rcWnd.InflateRect(GetRoot()->GetStyle().GetMargin());
		SetWindowPos(NULL, 0, 0, rcWnd.Width(), rcWnd.Height(), SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );
		UpdateAutoSizeCount(false);
	}
	return TRUE;
}

BOOL SHostWnd::InitFromXml(IXmlNode * pNode)
{
	SASSERT(pNode);
    if(pNode->Empty())
    {
        SASSERT_FMTA(FALSE,"Null XML node");
        return FALSE;
    }
    if(!SNativeWnd::IsWindow()) 
		return FALSE;
	if(m_AniState != Ani_none)
	{
		if(m_AniState&Ani_host)
			StopHostAnimation();
		if(m_AniState&Ani_win)
			GetRoot()->GetAnimation()->cancel();
		SASSERT(m_AniState==Ani_none);
	}

    //free old script module
    if(m_pScriptModule)
    {
        EventExit evt(GetRoot());
        GetRoot()->FireEvent(evt);
        m_pScriptModule = NULL;
    }

    //为了能够重入，先销毁原有的SOUI窗口
    GetRoot()->SSendMessage(WM_DESTROY);   
	m_bFirstShow = TRUE;
   //create new script module
    SApplication::getSingleton().CreateScriptModule(&m_pScriptModule);
    
	m_hostAttr.Init();
	m_hostAttr.InitFromXml(pNode);
    if(m_privateStylePool->GetCount())
    {
        m_privateStylePool->RemoveAll();
        GETSTYLEPOOLMGR->PopStylePool(m_privateStylePool);
    }
	SXmlNode xmlNode(pNode);

	SXmlNode xmlStyle = xmlNode.child(L"style");
	xmlStyle.set_userdata(1);
    m_privateStylePool->Init(xmlStyle);
    if(m_privateStylePool->GetCount())
    {
        GETSTYLEPOOLMGR->PushStylePool(m_privateStylePool);
    }
    if(m_privateSkinPool->GetCount())
    {
        m_privateSkinPool->RemoveAll();
        GETSKINPOOLMGR->PopSkinPool(m_privateSkinPool);
    }
	SXmlNode xmlSkin = xmlNode.child(L"skin");
	xmlSkin.set_userdata(1);
    m_privateSkinPool->LoadSkins(xmlSkin);//从xmlNode加加载私有skin
    if(m_privateSkinPool->GetCount())
    {
        GETSKINPOOLMGR->PushSkinPool(m_privateSkinPool);
    }

	if(m_privateTemplatePool->GetCount())
	{
		m_privateTemplatePool->RemoveAll();
		GETTEMPLATEPOOLMR->PopTemplatePool(m_privateTemplatePool);
	}
	SXmlNode xmlTemplate = xmlNode.child(L"template");
	xmlTemplate.set_userdata(1);
	m_privateTemplatePool->Init(xmlTemplate);
	if(m_privateTemplatePool->GetCount())
	{
		GETTEMPLATEPOOLMR->PushTemplatePool(m_privateTemplatePool);
	}
    //加载脚本数据
    SXmlNode xmlScript = xmlNode.child(L"script");
	xmlScript.set_userdata(1);
    if(m_pScriptModule && xmlScript)
    {
        SXmlAttr attrSrc = xmlScript.attribute(L"src");
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
    
    if(m_hostAttr.m_hAppIconSmall)
    {
        SendMessage(WM_SETICON,FALSE,(LPARAM)m_hostAttr.m_hAppIconSmall);
    }
    if(m_hostAttr.m_hAppIconBig)
    {
        SendMessage(WM_SETICON,TRUE,(LPARAM)m_hostAttr.m_hAppIconBig);
    }

	//compatible with 2.x width and height properties in soui element.
	SXmlAttr attrWid = xmlNode.attribute(L"width");
	SXmlAttr attrHei = xmlNode.attribute(L"height");
	SXmlNode xmlRoot = xmlNode.child(L"root");
	xmlRoot.set_userdata(1);
	if (attrWid && !xmlRoot.attribute(attrWid.name()))
		xmlRoot.append_copy(attrWid);
	if (attrHei && !xmlRoot.attribute(attrHei.name()))
		xmlRoot.append_copy(attrHei);

    GetRoot()->InitFromXml(&xmlRoot);

    if(m_hostAttr.m_bTranslucent)
	{
		if(!m_dummyWnd)
		{
			SetWindowLongPtr(GWL_EXSTYLE, GetWindowLongPtr(GWL_EXSTYLE) | WS_EX_LAYERED);
			m_dummyWnd = new SDummyWnd(this);
			HMONITOR hMonitor = MonitorFromWindow(m_hWnd,MONITOR_DEFAULTTONEAREST);
			MONITORINFO info = { sizeof(MONITORINFO) };
			GetMonitorInfo(hMonitor,&info);
			SStringT dummyTitle = SStringT().Format(_T("%s_dummy"),strTitle.c_str());
			m_dummyWnd->Create(dummyTitle,WS_POPUP,WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,info.rcWork.left,info.rcWork.top,1,1,m_hWnd,NULL);
			m_dummyWnd->SetWindowLongPtr(GWL_EXSTYLE,m_dummyWnd->GetWindowLongPtr(GWL_EXSTYLE) | WS_EX_LAYERED);
			::SetLayeredWindowAttributes(m_dummyWnd->m_hWnd,0,0,LWA_ALPHA);
			m_dummyWnd->ShowWindow(SW_SHOWNOACTIVATE);
		}
	}else if(dwExStyle & WS_EX_LAYERED || GetRoot()->GetAlpha()!=0xFF)
	{
		if(m_dummyWnd)
		{
			m_dummyWnd->DestroyWindow();//m_dummyWnd will be set to null in SDummyWnd::OnDestroy
		}
		if(!(dwExStyle & WS_EX_LAYERED)) ModifyStyleEx(0,WS_EX_LAYERED);
		::SetLayeredWindowAttributes(m_hWnd,0,GetRoot()->GetAlpha(),LWA_ALPHA);
	}

    BuildWndTreeZorder();

	int nWidth = m_szAppSetted.cx;
	int nHeight = m_szAppSetted.cy;
	ILayoutParam *pLayoutParam = GetRoot()->GetLayoutParam();
	if(nWidth==0 && pLayoutParam->IsSpecifiedSize(Horz))
	{
		nWidth = pLayoutParam->GetSpecifiedSize(Horz).toPixelSize(GetScale());
	}
	if(nHeight == 0 && pLayoutParam->IsSpecifiedSize(Vert))
	{
		nHeight = pLayoutParam->GetSpecifiedSize(Vert).toPixelSize(GetScale());
	}

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

		CSize szRoot = GetRoot()->GetDesiredSize(nWidth, nHeight);
		if(nWidth == SIZE_WRAP_CONTENT) 
			nWidth = szRoot.cx;
		if(nHeight == SIZE_WRAP_CONTENT)
			nHeight = szRoot.cy;
		GetRoot()->GetEventSet()->subscribeEvent(EventSwndSize::EventID, Subscriber(&SHostWnd::onRootResize, this));
	}

	if(nWidth != m_szAppSetted.cx || nHeight != m_szAppSetted.cy)
	{
		UpdateAutoSizeCount(true);
		SetWindowPos(NULL,0,0,nWidth,nHeight,SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
		UpdateAutoSizeCount(false);
	}

    CRect rcClient;
    SNativeWnd::GetClientRect(&rcClient);
    GetRoot()->OnRelayout(rcClient);

    //设置重绘标记
    m_bNeedAllRepaint = TRUE;
    m_bNeedRepaint = TRUE;
    m_rgnInvalidate->Clear();
    
    EventInit evt(GetRoot());
    GetRoot()->FireEvent(evt);

	//handle user xml node
	SXmlNode xmlChild = xmlNode.first_child();
	while(xmlChild)
	{
		if(xmlChild.get_userdata()!=1)
		{
			OnUserXmlNode(xmlChild);
		}else
		{
			xmlChild.set_userdata(0);
		}
		xmlChild = xmlChild.next_sibling();
	}
    return TRUE;
}

void SHostWnd::_Redraw()
{
    m_bNeedAllRepaint = TRUE;
    m_rgnInvalidate->Clear();
	_Invalidate(NULL);
}

void SHostWnd::OnPrint(HDC dc, UINT uFlags)
{
	if(!IsWindowVisible())
		return;
	SMatrix mtx = GetRoot()->_GetMatrixEx();
    //刷新前重新布局，会自动检查布局脏标志
	GetRoot()->UpdateLayout();
    
    if (m_bNeedAllRepaint)
    {
        m_rgnInvalidate->Clear();
        m_bNeedAllRepaint = FALSE;
        m_bNeedRepaint=TRUE;
    }

    CRect rcInvalid;
	CRect rcWnd = GetRoot()->GetWindowRect();
    if (m_bNeedRepaint)
    {
        m_bNeedRepaint = FALSE;

        SPainter painter;
        GetRoot()->BeforePaint(m_memRT,painter);

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
        GetRoot()->RedrawRegion(m_memRT, pRgnUpdate);
        
        m_memRT->PopClip();
        
        GetRoot()->AfterPaint(m_memRT,painter);
        
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

BOOL SHostWnd::OnLoadLayoutFromResourceID(const SStringT &resId)
{
	if(resId.IsEmpty())
		return FALSE;
	SXmlDoc xmlDoc;
	if(LOADXML(xmlDoc,resId))
	{
		return InitFromXml(&xmlDoc.root().child(L"SOUI"));
	}else
	{
		SASSERT_FMT(FALSE,_T("Load layout [%s] Failed"),m_strXmlLayout.c_str());
		return FALSE;
	}
}

int SHostWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	m_dwThreadID = GetCurrentThreadId();
	SHostMgr::getSingletonPtr()->AddHostMsgHandler(this);
	UpdateAutoSizeCount(true);
	m_memRT=NULL;
    GETRENDERFACTORY->CreateRenderTarget(&m_memRT,0,0);
	m_rgnInvalidate=NULL;
    GETRENDERFACTORY->CreateRegion(&m_rgnInvalidate);    
	m_szAppSetted.cx = lpCreateStruct->cx;
	m_szAppSetted.cy = lpCreateStruct->cy;
	OnLoadLayoutFromResourceID(m_strXmlLayout);

	m_pTipCtrl = CreateTooltip();
	if(m_pTipCtrl) GetMsgLoop()->AddMessageFilter(m_pTipCtrl);
	UpdateAutoSizeCount(false);
    return 0;
}

void SHostWnd::OnDestroy()
{
    EventExit evt(GetRoot());
    GetRoot()->FireEvent(evt);

    GetRoot()->SSendMessage(WM_DESTROY);

    if(m_pTipCtrl)
    {
        GetMsgLoop()->RemoveMessageFilter(m_pTipCtrl);
		DestroyTooltip(m_pTipCtrl);
        m_pTipCtrl = NULL;
    }

	m_memRT = NULL;
	m_rgnInvalidate = NULL;
	m_nScale = 100;//restore to 100
	SHostMgr::getSingletonPtr()->RemoveHostMsgHandler(this);
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
	BOOL bDirty = GetRoot()->IsLayoutDirty();
	GetRoot()->OnRelayout(CRect(0, 0, size.cx, size.cy));
	_Redraw();
	if(m_nAutoSizing && bDirty)
		GetRoot()->m_layoutDirty = SWindow::dirty_self;

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
    SetMsgHandled(GetRoot()->IsMsgHandled());
    return lRet;
}

LRESULT SHostWnd::OnActivateApp( UINT uMsg, WPARAM wParam, LPARAM lParam )
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

BOOL SHostWnd::OnFireEvent(IEvtArgs* evt)
{
    return _HandleEvent(evt);
}

CRect SHostWnd::GetContainerRect() const
{
	return GetRoot()->GetWindowRect();
}

HWND SHostWnd::GetHostHwnd()
{
    return m_hWnd;
}

IRenderTarget * SHostWnd::OnGetRenderTarget(LPCRECT rc,GrtFlag gdcFlags)
{
	IRenderTarget *pRT = NULL;
	if (gdcFlags == GRT_NODRAW)
	{
		GETRENDERFACTORY->CreateRenderTarget(&pRT, 0, 0);
		pRT->OffsetViewportOrg(-rc->left, -rc->top,NULL);
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

void SHostWnd::OnReleaseRenderTarget(IRenderTarget * pRT,LPCRECT rc,GrtFlag gdcFlags)
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
            m_lstUpdatedRect.AddTail(*rc);
        }
    }
    pRT->Release();
}

void SHostWnd::UpdateHost(HDC dc, LPCRECT rcInvalid, BYTE byAlpha)
{
    byAlpha=(BYTE)((int)byAlpha*GetRoot()->GetAlpha()/255);
    if(m_hostAttr.m_bTranslucent)
    {
        UpdateLayerFromRenderTarget(m_memRT,byAlpha,rcInvalid);
    }
    else
    {
        HDC hdc=m_memRT->GetDC(0);
        ::BitBlt(dc,rcInvalid->left,rcInvalid->top,RectWidth(rcInvalid),RectHeight(rcInvalid),hdc,rcInvalid->left,rcInvalid->top,SRCCOPY);
        m_memRT->ReleaseDC(hdc);
    }
}

void SHostWnd::OnRedraw(LPCRECT rc)
{
    if(!IsWindow()) return;
    
    m_rgnInvalidate->CombineRect(rc,RGN_OR);
    
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
        BOOL bOK=pHover->UpdateToolTip(pt,tipInfo);
        if(bOK)
        {
            TIPID id={tipInfo.swnd,tipInfo.dwCookie};
            m_pTipCtrl->UpdateTip(&id,tipInfo.rcTarget,tipInfo.strTip,GetScale());
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
		CRect rcWnd = GetRoot()->GetWindowRect();
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
        GetRoot()->RedrawRegion(m_memRT,m_rgnInvalidate);

        int nSteps=dwTime/10;
		const BYTE byAlpha =GetRoot()->GetAlpha() ;
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
                    UpdateLayerFromRenderTarget(pRT, byAlpha);
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
                    UpdateLayerFromRenderTarget(pRT,byAlpha);
                    Sleep(10);
                }
                ShowWindow(SW_HIDE);
                return TRUE;
            }else if(dwFlags&AW_BLEND)
            {
                BYTE byAlpha2= byAlpha;
                for(int i=0;i<nSteps;i++)
                {
                    byAlpha2-=255/nSteps;
                    UpdateLayerFromRenderTarget(m_memRT,byAlpha2);
                    Sleep(10);
                }
                ShowWindow(SW_HIDE);
                return TRUE;
            }
            return FALSE;
        }else
        {
            pRT->ClearRect(rcWnd,0);
            UpdateLayerFromRenderTarget(pRT, byAlpha);
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
                    UpdateLayerFromRenderTarget(pRT, byAlpha);
                    Sleep(10);
                }
                UpdateLayerFromRenderTarget(m_memRT,byAlpha);
                
            
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
                    UpdateLayerFromRenderTarget(pRT,byAlpha);
                    Sleep(10);
                }
                UpdateLayerFromRenderTarget(m_memRT,byAlpha);
                return TRUE;
            }else if(dwFlags&AW_BLEND)
            {
                BYTE byAlpha2=0;
                for(int i=0;i<nSteps;i++)
                {
                    byAlpha2+=byAlpha/nSteps;
                    UpdateLayerFromRenderTarget(m_memRT, byAlpha2);
                    Sleep(10);
                }
                UpdateLayerFromRenderTarget(m_memRT,byAlpha);
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

IMessageLoop * SHostWnd::GetMsgLoop()
{
    return SApplication::getSingletonPtr()->GetMsgLoop(m_dwThreadID);
}

#ifndef DISABLE_SWNDSPY

LRESULT SHostWnd::OnSpyMsgSwndEnum(UINT uMsg, WPARAM wParam,LPARAM lParam )
{
    if(!m_hostAttr.m_bAllowSpy) return 0;
    SWND swndCur=(SWND)wParam;
    if(swndCur==0) swndCur=GetRoot()->m_swnd;
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
    if(swndCur==0) swndCur=GetRoot()->m_swnd;
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
    return GetRoot()->SwndFromPoint(pt);
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
        EventTimer evt(GetRoot());
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

//////////////////////////////////////////////////////////////////////////
BOOL SHostWnd::DestroyWindow()
{
	if(m_AniState!=Ani_none)
	{
		if(m_AniState &Ani_host)
			StopHostAnimation();
		if(m_AniState & Ani_win)
			GetRoot()->ClearAnimation();
		SASSERT(m_AniState==Ani_none);
	}
	if(m_pRoot->m_aniExit && !IsIconic())
	{
		GetRoot()->StartAnimation(m_pRoot->m_aniExit);
		m_AniState |= Ani_win;
		return TRUE;
	}else
	{
		return SNativeWnd::DestroyWindow();
	}
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
    SNativeWnd::GetClientRect(&rc);
    return rc;
}

CRect SHostWnd::GetClientRect() const
{
    CRect rc;
    GetRoot()->GetClientRect(&rc);
    return rc;
}

LRESULT SHostWnd::OnMenuExEvent(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    return _HandleEvent((IEvtArgs *)lParam);
}


void SHostWnd::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{//默认不处理该消息，同时防止系统处理该消息
	if(lpWndPos->flags&SWP_SHOWWINDOW && m_bFirstShow)
	{
		m_bFirstShow = FALSE;
		OnHostShowWindow(TRUE,0);
	}
}

void SHostWnd::OnWindowPosChanged(LPWINDOWPOS lpWndPos)
{
	if(!(lpWndPos->flags& SWP_NOMOVE) && m_dummyWnd)
	{
		HMONITOR hMonitor = MonitorFromWindow(m_hWnd,MONITOR_DEFAULTTONEAREST);
		MONITORINFO info = { sizeof(MONITORINFO) };
		if (GetMonitorInfo(hMonitor,&info))
		{
			m_dummyWnd->SetWindowPos(NULL, info.rcWork.left, info.rcWork.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
		}
	}
	SetMsgHandled(FALSE);
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

bool SHostWnd::StartHostAnimation(IAnimation *pAni)
{
	SASSERT(pAni);
	if(!IsWindow())
		return false;
	StopHostAnimation();

	m_AniState |=Ani_host;
	m_hostAnimation = pAni;
	m_hostAnimation->startNow();

	CRect rcWnd; GetNative()->GetWindowRect(&rcWnd);
	CRect rcParent;
	HWND hParent = SNativeWnd::GetParent();
	if(hParent)
	{
		::GetClientRect(hParent,&rcParent);
	}else
	{
		HMONITOR hMonitor = MonitorFromWindow(m_hWnd,MONITOR_DEFAULTTONEAREST);
		MONITORINFO info = { sizeof(MONITORINFO) };
		GetMonitorInfo(hMonitor,&info);
		rcParent = info.rcWork;
	}
	m_hostAnimation->initialize(rcWnd.Width(),rcWnd.Height(),rcParent.Width(),rcParent.Height());
	m_hostAnimationHandler.m_rcInit = rcWnd;
	RegisterTimelineHandler(&m_hostAnimationHandler);
	return true;
}

bool SHostWnd::StopHostAnimation()
{
	if(!m_hostAnimation)
		return false;
	m_hostAnimation->cancel();
	UnregisterTimelineHandler(&m_hostAnimationHandler);
	return true;
}

void SHostWnd::OnUserXmlNode(SXmlNode xmlUser)
{
	SLOG_DEBUG("unhandled xml node:"<<xmlUser.name());
}

void SHostWnd::UpdateAutoSizeCount(bool bInc)
{
	if(bInc)
		m_nAutoSizing++;
	else
		m_nAutoSizing--;
}

void SHostWnd::OnHostMsg(bool bRelayout,UINT uMsg,WPARAM wp,LPARAM lp)
{
	GetRoot()->SDispatchMessage(uMsg,wp,lp);
	if(bRelayout)
	{
		GetRoot()->RequestRelayout();
	}
}

void SHostWnd::EnableIME(BOOL bEnable)
{
	if(bEnable)
	{
		HIMC hImc = ImmGetContext(m_hWnd);
		if(!hImc)
		{
			hImc = ImmCreateContext();
			ImmAssociateContext(m_hWnd,hImc);
		}
	}else
	{
		HIMC hImc = ImmGetContext(m_hWnd);
		ImmAssociateContext(m_hWnd,NULL);
		if(hImc)
		{
			ImmDestroyContext(hImc);
		}
	}
}

void SHostWnd::OnUpdateCursor()
{
	CPoint pt;
	GetCursorPos(&pt);
	UINT ht = OnWndNcHitTest(pt);
	PostMessage(WM_SETCURSOR,(WPARAM)m_hWnd,MAKELPARAM(ht,WM_MOUSEMOVE));
}

BOOL SHostWnd::ShowWindow(int nCmdShow)
{
	UpdateAutoSizeCount(true);
	BOOL bRet = SNativeWnd::ShowWindow(nCmdShow);
	UpdateAutoSizeCount(false);
	return bRet;
}

void SHostWnd::OnHostShowWindow(BOOL bShow, UINT nStatus)
{
	DefWindowProc();
	if(bShow && m_pRoot->m_aniEnter)
	{
		if(m_pRoot->m_aniEnter)
		{
			GetRoot()->StartAnimation(m_pRoot->m_aniEnter);
			m_AniState |= Ani_win;
		}
		OnNextFrame();
	}
}


void SHostWnd::OnHostAnimationStarted(IAnimation * pAni)
{
	
}

void SHostWnd::OnHostAnimationStoped(IAnimation * pAni)
{
	m_AniState&=~Ani_host;
}

void SHostWnd::OnSysCommand(UINT nID, CPoint lParam)
{
	if(nID==SC_RESTORE)
	{
		UpdateAutoSizeCount(true);
		DefWindowProc();
		UpdateAutoSizeCount(false);
	}else
	{
		DefWindowProc();
	}
}


//////////////////////////////////////////////////////////////////
//  SHostWnd::SHostAnimationHandler
void SHostWnd::SHostAnimationHandler::OnNextFrame()
{
	if(!m_pHostWnd->m_hostAnimation)
		return;
	IAnimation *pAni = m_pHostWnd->m_hostAnimation;
	uint64_t tm = pAni->getStartTime();
	if (tm == -1)
	{
		m_pHostWnd->OnHostAnimationStarted(pAni);
	}
	STransformation xform;
	bool bMore = m_pHostWnd->m_hostAnimation->getTransformation(STime::GetCurrentTimeMs(),xform);
	SMatrix mtx = xform.getMatrix();
	mtx.preTranslate(-m_rcInit.left,-m_rcInit.top);
	mtx.postTranslate(m_rcInit.left,m_rcInit.top);
	if(mtx.rectStaysRect())
	{
		SRect rc = SRect::IMake(m_rcInit);
		mtx.mapRect(&rc);
		CRect rc2 = rc.toRect();
		::SetWindowPos(m_pHostWnd->m_hWnd,NULL,rc2.left,rc2.top,rc2.Width(),rc2.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
	}
	if(xform.hasAlpha())
	{//change alpha.
		if(m_pHostWnd->IsTranslucent())
		{
			CRect rcWnd = m_pHostWnd->GetRoot()->GetWindowRect();
			m_pHostWnd->UpdateHost(0,rcWnd,xform.getAlpha());
		}else if(m_pHostWnd->GetWindowLongPtr(GWL_EXSTYLE) & WS_EX_LAYERED)
		{
			::SetLayeredWindowAttributes(m_pHostWnd->m_hWnd,0,(BYTE)((int)m_pHostWnd->GetRoot()->GetAlpha()*xform.getAlpha()/255),LWA_ALPHA);
		}
	}
	if(!bMore)
	{
		m_pHostWnd->UnregisterTimelineHandler(this);
		m_pHostWnd->OnHostAnimationStoped(pAni);
	}
}
 
HRESULT CreateHostWnd(LPCTSTR pszResID,IHostWnd ** ppRet)
{
	(*ppRet) = new SHostWnd(pszResID);
	return S_OK;
}


}//namespace SOUI

