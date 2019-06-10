// Sscrollbar.h : implementation file
//    模块:滚动条控件
//////////////////////////////////////////////////////////////////////////
#include "souistd.h"
#include "control/Sscrollbar.h"

#define TIMERID_NOTIFY1    1
#define TIMERID_DELAY1    2

namespace SOUI
{


/////////////////////////////////////////////////////////////////////////////
// SScrollBar
SScrollBar::SScrollBar()
    : m_pSkin(GETBUILTINSKIN(SKIN_SYS_SCROLLBAR))
    , m_bDraging(FALSE)
    , m_iClicked(-1)
	, m_iHitPrev(-1)
    , m_uAllowSize((UINT)-1)
    , m_sbPainter(this)
{
    memset(&m_si,0,sizeof(SCROLLINFO));
    m_si.nTrackPos=-1;
    m_evtSet.addEvent(EVENTID(EventScroll));
}

SScrollBar::~SScrollBar()
{
}

BOOL SScrollBar::IsVertical() const
{
    return m_sbPainter.IsVertical();
}

int SScrollBar::SetPos(int nPos)
{
    if(nPos>(m_si.nMax-(int)m_si.nPage+1)) nPos=(m_si.nMax-m_si.nPage+1);
    if(nPos<m_si.nMin) nPos=m_si.nMin;
    if(nPos!=m_si.nPos)
    {
        if(m_si.nTrackPos==-1)
        {
            CRect rcOldThumb=m_sbPainter.GetPartRect(SB_THUMBTRACK);
            m_si.nTrackPos=nPos;
            CRect rcNewThumb=m_sbPainter.GetPartRect(SB_THUMBTRACK);
            CRect rcUnion;
            rcUnion.UnionRect(&rcOldThumb,&rcNewThumb);
			if (IsVisible())
			{
				IRenderTarget *pRT = GetRenderTarget(&rcUnion, OLEDC_PAINTBKGND);
				m_sbPainter.OnDraw(pRT,SB_PAGEUP);
				m_sbPainter.OnDraw(pRT,SB_PAGEDOWN);
				m_sbPainter.OnDraw(pRT,SB_THUMBTRACK);
				ReleaseRenderTarget(pRT);
			}         
            m_si.nTrackPos=-1;
        }
        m_si.nPos=nPos;
    }
    return m_si.nPos;
}

int SScrollBar::GetPos()
{
    return m_si.nPos;
}

int SScrollBar::GetMax()
{
	return m_si.nMax;
}

int SScrollBar::GetMin()
{
	return m_si.nMin;
}

void SScrollBar::OnInitFinished(pugi::xml_node xmlNode)
{
    __super::OnInitFinished(xmlNode);
    SASSERT(m_pSkin);
    if(m_uAllowSize==-1)
    {
        m_uAllowSize=m_pSkin->GetSkinSize().cx/9;
    }
}

void SScrollBar::OnPaint(IRenderTarget * pRT)
{
    if(!m_pSkin) return;

	m_sbPainter.OnDraw(pRT,SB_LINEUP);
	m_sbPainter.OnDraw(pRT,SB_PAGEUP);
	m_sbPainter.OnDraw(pRT,SB_THUMBTRACK);
	m_sbPainter.OnDraw(pRT,SB_PAGEDOWN);
	m_sbPainter.OnDraw(pRT,SB_LINEDOWN);
}

void SScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();
	m_sbPainter.OnMouseUp(point);
    if(m_bDraging)
    {
        m_bDraging=FALSE;
        m_si.nPos=m_si.nTrackPos;
        m_si.nTrackPos=-1;
        OnMouseMove(nFlags,point);
        NotifySbCode(SB_THUMBPOSITION,m_si.nPos);
    }
    else if(m_iClicked!=-1)
    {
        m_iClicked=-1;
    }
}

void SScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCapture();
	m_sbPainter.OnMouseDown(point);
	int iHit = m_sbPainter.GetHitPart();
    if(iHit==SB_THUMBTRACK)
    {
        m_bDraging=TRUE;
        m_ptDrag=point;
        m_si.nTrackPos=m_si.nPos;
    }
    else
    {
        m_iClicked=iHit;

		if(iHit == SB_LINEUP || iHit == SB_LINEDOWN
			|| iHit == SB_PAGEUP || iHit == SB_PAGEDOWN)
		{
			NotifySbCode(iHit,m_si.nPos);
		}
    }
}


void SScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	m_sbPainter.OnMouseMove(point);
    if(m_bDraging)
    {
        CRect rcWnd = GetWindowRect();
        int nInterHei=(IsVertical()?rcWnd.Height():rcWnd.Width())-2*m_uAllowSize;
        int    nSlideHei=m_si.nPage*nInterHei/(m_si.nMax-m_si.nMin+1);
        if(nSlideHei<THUMB_MINSIZE) nSlideHei=THUMB_MINSIZE;
        if(nInterHei<THUMB_MINSIZE) nSlideHei=0;
        int nEmptyHei=nInterHei-nSlideHei;
        int nDragLen=IsVertical()?(point.y-m_ptDrag.y):(point.x-m_ptDrag.x);
        int nSlide=(nEmptyHei==0)?0:(nDragLen*(int)(m_si.nMax-m_si.nMin-m_si.nPage+1)/nEmptyHei);
        int nNewTrackPos=m_si.nPos+nSlide;
        if(nNewTrackPos<m_si.nMin)
        {
            nNewTrackPos=m_si.nMin;
        }
        else if(nNewTrackPos>(int)(m_si.nMax-m_si.nMin-m_si.nPage+1))
        {
            nNewTrackPos=m_si.nMax-m_si.nMin-m_si.nPage+1;
        }
        if(nNewTrackPos!=m_si.nTrackPos)
        {
            CRect rcOldThumb=GetPartRect(SB_THUMBTRACK);
            m_si.nTrackPos=nNewTrackPos;
            CRect rcThumb=GetPartRect(SB_THUMBTRACK);
            CRect rcUnion;
            rcUnion.UnionRect(rcOldThumb,rcThumb);
            IRenderTarget *pRT=GetRenderTarget(&rcUnion,OLEDC_PAINTBKGND);

            m_pSkin->DrawByState(pRT,rcUnion,MAKESBSTATE(SB_PAGEUP,SBST_NORMAL,m_bVertical));
            m_pSkin->DrawByState(pRT,rcThumb,MAKESBSTATE(SB_THUMBTRACK,SBST_HOVER,m_bVertical));

            ReleaseRenderTarget(pRT);
            NotifySbCode(SB_THUMBTRACK,m_si.nTrackPos);
        }
    }
    else
    {
        UINT uHit=HitTest(point);
        if(uHit!=m_iHitPrev)
        {
            if(m_iHitPrev!=-1)
            {
                CRect rc=GetPartRect(m_iHitPrev);
                IRenderTarget *pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND);
                m_pSkin->DrawByState(pRT,rc,MAKESBSTATE(m_iHitPrev,SBST_NORMAL,m_bVertical));
                ReleaseRenderTarget(pRT);
            }
            if(uHit!=-1)
            {
                CRect rc=GetPartRect(uHit);
                IRenderTarget *pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND);
                m_pSkin->DrawByState(pRT,rc,MAKESBSTATE(uHit,SBST_HOVER,m_bVertical));
                ReleaseRenderTarget(pRT);
            }
            m_iHitPrev=uHit;
        }
    }
}

void SScrollBar::OnTimer(char nIDEvent)
{
    if(nIDEvent==TIMERID_NOTIFY1)
    {
        SASSERT(m_iClicked!=-1 && m_iClicked!=SB_THUMBTRACK);

        switch(m_iClicked)
        {
        case SB_LINEUP:
            if(m_si.nPos==m_si.nMin)
            {
                KillTimer(TIMERID_NOTIFY1);
                break;
            }
            break;
        case SB_LINEDOWN:
            if(m_si.nPos==m_si.nMax)
            {
                KillTimer(TIMERID_NOTIFY1);
                break;
            }
            break;
        case SB_PAGEUP:
        case SB_PAGEDOWN:
        {
            CPoint pt;
            ::GetCursorPos(&pt);
            ::ScreenToClient(GetContainer()->GetHostHwnd(),&pt);
            CRect rc=GetPartRect(SB_THUMBTRACK);
            if(rc.PtInRect(pt))
            {
                KillTimer(TIMERID_NOTIFY1);
                break;
            }
        }
        break;
        default:
            SASSERT(FALSE);
            break;
        }
    }
}

void SScrollBar::OnMouseLeave()
{
    if(!m_bDraging && m_iHitPrev!=-1)
    {
		OnMouseMove(0,CPoint(-1,-1));
    }
}


LRESULT SScrollBar::OnSetScrollInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL bRedraw=wParam!=0;
    LPSCROLLINFO lpScrollInfo=(LPSCROLLINFO)lParam;
    if(lpScrollInfo->fMask&SIF_PAGE) m_si.nPage=lpScrollInfo->nPage;
    if(lpScrollInfo->fMask&SIF_POS) m_si.nPos=lpScrollInfo->nPos;
    if(lpScrollInfo->fMask&SIF_RANGE)
    {
        m_si.nMin=lpScrollInfo->nMin;
        m_si.nMax=lpScrollInfo->nMax;
    }
    if(m_si.nPos>(m_si.nMax-(int)m_si.nPage+1)) m_si.nPos=(m_si.nMax-m_si.nPage+1);
    if(m_si.nPos<m_si.nMin) m_si.nPos=m_si.nMin;
    if(bRedraw)    Invalidate();

    return TRUE;
}

LRESULT SScrollBar::OnGetScrollInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LPSCROLLINFO lpScrollInfo=(LPSCROLLINFO)lParam;
    int nMask=lpScrollInfo->fMask;
    if(nMask&SIF_PAGE) lpScrollInfo->nPage=m_si.nPage;
    if(nMask&SIF_POS) lpScrollInfo->nPos=m_si.nPos;
    if(nMask&SIF_TRACKPOS) lpScrollInfo->nTrackPos=m_si.nTrackPos;
    if(nMask&SIF_RANGE)
    {
        lpScrollInfo->nMin=m_si.nMin;
        lpScrollInfo->nMax=m_si.nMax;
    }
    return TRUE;
}

void SScrollBar::NotifySbCode(int nCode,int nPos)
{
    EventScroll evt(this);
    evt.nSbCode=nCode;
    evt.nPos=nPos;
    evt.bVertical=IsVertical();
    FireEvent(evt);
}

CRect SScrollBar::GetScrollBarRect(bool bVert) const
{
	return GetClientRect();
}

ISkinObj* SScrollBar::GetScrollBarSkin(bool bVert) const
{
	return m_pSkin;
}

const SCROLLINFO * SScrollBar::GetScrollBarInfo(bool bVert) const
{
	return &m_si;
}

int SScrollBar::GetScrollBarArrowSize(bool bVert) const
{
	return m_uAllowSize;
}

void SScrollBar::UpdateScrollBar(bool bVert, int iPart)
{
	CRect rc=m_sbPainter.GetPartRect(iPart);
	IRenderTarget *pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND);
	m_sbPainter.OnDraw(pRT,iPart);
	ReleaseRenderTarget(pRT);

}

ISwndContainer * SScrollBar::GetScrollBarContainer()
{
	return GetContainer();
}

bool SScrollBar::IsScrollBarEnable(bool bVert) const
{
	return !IsDisabled(TRUE);
}

}//namespace SOUI