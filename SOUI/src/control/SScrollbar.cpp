// Sscrollbar.h : implementation file
//    模块:滚动条控件
//////////////////////////////////////////////////////////////////////////
#include "souistd.h"
#include "control/Sscrollbar.h"

namespace SOUI
{
/////////////////////////////////////////////////////////////////////////////
// SScrollBar
SScrollBar::SScrollBar()
    : m_pSkin(GETBUILTINSKIN(SKIN_SYS_SCROLLBAR))
    , m_uAllowSize((UINT)-1)
    , m_sbHander(this)
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
    return m_sbHander.IsVertical();
}

int SScrollBar::SetPos(int nPos)
{
    if(nPos>(m_si.nMax-(int)m_si.nPage+1)) nPos=(m_si.nMax-m_si.nPage+1);
    if(nPos<m_si.nMin) nPos=m_si.nMin;
    if(nPos!=m_si.nPos)
    {
        if(m_si.nTrackPos==-1)
        {
            CRect rcOldThumb=m_sbHander.GetPartRect(SB_THUMBTRACK);
            m_si.nTrackPos=nPos;
            CRect rcNewThumb=m_sbHander.GetPartRect(SB_THUMBTRACK);
            CRect rcUnion;
            rcUnion.UnionRect(&rcOldThumb,&rcNewThumb);
			if (IsVisible())
			{
				IRenderTarget *pRT = GetRenderTarget(&rcUnion, OLEDC_PAINTBKGND);
				m_sbHander.OnDraw(pRT, SScrollBarHandler::kSbRail);
				m_sbHander.OnDraw(pRT,SB_THUMBTRACK);
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
	m_sbHander.OnDraw(pRT,SB_LINEUP);
	m_sbHander.OnDraw(pRT, SScrollBarHandler::kSbRail);
	m_sbHander.OnDraw(pRT,SB_THUMBTRACK);
	m_sbHander.OnDraw(pRT,SB_LINEDOWN);
}

void SScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_sbHander.OnMouseUp(point);
	ReleaseCapture();
}

void SScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCapture();
	m_sbHander.OnMouseDown(point);
}


void SScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	m_sbHander.OnMouseMove(point);
}

void SScrollBar::OnTimer(char nIDEvent)
{
	m_sbHander.OnTimer(nIDEvent);
}

void SScrollBar::OnMouseHover(UINT nFlags, CPoint ptPos)
{
	m_sbHander.OnMouseHover(ptPos);
}

void SScrollBar::OnMouseLeave()
{
	m_sbHander.OnMouseLeave();
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

void SScrollBar::OnScrollThumbTrackPos(bool bVert, int nPos)
{
	CRect rcOldThumb = m_sbHander.GetPartRect(SB_THUMBTRACK);
	m_si.nTrackPos = nPos;
	CRect rcThumb = m_sbHander.GetPartRect(SB_THUMBTRACK);
	CRect rcUnion;
	rcUnion.UnionRect(rcOldThumb, rcThumb);
	IRenderTarget *pRT = GetRenderTarget(&rcUnion, OLEDC_PAINTBKGND);
	m_sbHander.OnDraw(pRT, SScrollBarHandler::kSbRail);
	m_sbHander.OnDraw(pRT, SB_THUMBTRACK);
	ReleaseRenderTarget(pRT);
	NotifySbCode(SB_THUMBTRACK, m_si.nTrackPos);
}

void SScrollBar::OnScrollCommand(bool bVert, int iCmd)
{
	int nOldPos = m_si.nPos;
	switch (iCmd)
	{
	case SB_LINEUP: m_si.nPos--; break;
	case SB_PAGEUP: m_si.nPos -= m_si.nPage; break;
	case SB_PAGEDOWN:m_si.nPos += m_si.nPage; break;
	case SB_LINEDOWN: m_si.nPos++; break;
	case SB_THUMBPOSITION: 
		if (m_si.nTrackPos != -1)
		{
			m_si.nPos = m_si.nTrackPos;
			m_si.nTrackPos = -1;
		}
		break;
	}
	if (m_si.nPos < m_si.nMin) m_si.nPos = m_si.nMin;
	if (m_si.nPos > m_si.nMax - m_si.nPage + 1) m_si.nPos = m_si.nMax - m_si.nPage + 1;
	if (nOldPos != m_si.nPos)
	{
		if (iCmd != SB_THUMBPOSITION)
		{
			CRect rcRail = m_sbHander.GetPartRect(SScrollBarHandler::kSbRail);
			IRenderTarget *pRT = GetRenderTarget(&rcRail, OLEDC_PAINTBKGND);
			m_sbHander.OnDraw(pRT, SScrollBarHandler::kSbRail);
			m_sbHander.OnDraw(pRT, SB_THUMBTRACK);
			ReleaseRenderTarget(pRT);
		}
		NotifySbCode(iCmd, m_si.nPos);
	}
}

void SScrollBar::OnScrollSetTimer(bool bVert, char id, UINT uElapse)
{
	SetTimer(id, uElapse);
}

void SScrollBar::OnScrollKillTimer(bool bVert, char id)
{
	KillTimer(id);
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
	CRect rc=m_sbHander.GetPartRect(iPart);
	IRenderTarget *pRT=GetRenderTarget(&rc,OLEDC_PAINTBKGND);
	m_sbHander.OnDraw(pRT,iPart);
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