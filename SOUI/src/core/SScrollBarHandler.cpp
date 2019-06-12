#include <souistd.h>
#include <core/SScrollBarHandler.h>

namespace SOUI
{
	SScrollBarHandler::SScrollBarHandler(IScrollBarHost *pCB, bool bVert)
		:m_bVert(bVert)
		,m_pCB(pCB)
		, m_iFrame(0)
		, m_fadeMode(FADE_STOP)
		, m_iHitPart(-1)
		, m_iClickPart(-1)
	{
		SASSERT(m_pCB);
	}


	void SScrollBarHandler::SetVertical(bool bVert)
	{
		m_bVert = bVert;
	}

	void SScrollBarHandler::OnNextFrame()
	{
		SASSERT(m_fadeMode != FADE_STOP);
		if (m_iFrame>=0 && m_iFrame < m_pCB->GetScrollFadeFrames())
		{
			m_iFrame += GetFadeStep();
			m_pCB->UpdateScrollBar(m_bVert, -1);
		}
		else
		{
			m_fadeMode = FADE_STOP;
			GetContainer()->UnregisterTimelineHandler(this);
		}
	}

	CRect SScrollBarHandler::GetPartRect(int iPart) const
	{
		SASSERT(m_pCB->GetScrollBarSkin(m_bVert));
		const SCROLLINFO * pSi = m_pCB->GetScrollBarInfo(m_bVert);
		int nTrackPos=pSi->nTrackPos;
		int nMax=pSi->nMax;
		if(nMax<pSi->nMin+(int)pSi->nPage-1) nMax=pSi->nMin+pSi->nPage-1;

		if(nTrackPos==-1)
			nTrackPos=pSi->nPos;
		CRect rcAll = m_pCB->GetScrollBarRect(m_bVert);
		int nLength=(IsVertical()?rcAll.Height():rcAll.Width());

		int nArrowHei=m_pCB->GetScrollBarArrowSize(m_bVert);
		int nInterHei=nLength-2*nArrowHei;
		if(nInterHei<0)
			nInterHei=0;
		int    nSlideHei=pSi->nPage*nInterHei/(nMax-pSi->nMin+1);
		if(nMax==(int)(pSi->nMin+pSi->nPage-1))
			nSlideHei=nInterHei;
		if(nSlideHei<THUMB_MINSIZE)
			nSlideHei=THUMB_MINSIZE;
		if(nInterHei<THUMB_MINSIZE)
			nSlideHei=0;
		int nEmptyHei=nInterHei-nSlideHei;
		if(nInterHei==0)
			nArrowHei=nLength/2;
		CRect rcRet(0,0,rcAll.Width(),nArrowHei);
		if (iPart == kSbRail)
		{
			rcRet = CRect(0,0, rcAll.Width(), nLength);
			rcRet.DeflateRect(0,nArrowHei);
			goto end;
		}
		if(iPart==SB_LINEUP) goto end;
		rcRet.top=rcRet.bottom;
		if((pSi->nMax-pSi->nMin-pSi->nPage+1)==0)
			rcRet.bottom+=nEmptyHei/2;
		else
			rcRet.bottom+=nEmptyHei*nTrackPos/(pSi->nMax-pSi->nMin-pSi->nPage+1);
		if(iPart==SB_PAGEUP) goto end;
		rcRet.top=rcRet.bottom;
		rcRet.bottom+=nSlideHei;
		if(iPart==SB_THUMBTRACK) goto end;
		rcRet.top=rcRet.bottom;
		rcRet.bottom=nLength-nArrowHei;
		if(iPart==SB_PAGEDOWN) goto end;
		rcRet.top=rcRet.bottom;
		rcRet.bottom=nLength;
		if(iPart==SB_LINEDOWN) goto end;
end:
		if(!IsVertical())
		{
			rcRet.left=rcRet.top;
			rcRet.right=rcRet.bottom;
			rcRet.top=0;
			rcRet.bottom=rcAll.Height();
		}
		rcRet.OffsetRect(rcAll.TopLeft());
		return rcRet;
	}

	bool SScrollBarHandler::IsVertical() const
	{
		return m_bVert;
	}

	ISwndContainer * SScrollBarHandler::GetContainer()
	{
		return m_pCB->GetScrollBarContainer();
	}

	const IInterpolator * SScrollBarHandler::GetInterpolator() const
	{
		return m_pCB->GetScrollInterpolator();;
	}

	int SScrollBarHandler::HitTest(CPoint pt) const
	{
		static const int parts[] =
		{
			SB_LINEUP,
			SB_PAGEUP,
			SB_THUMBTRACK,
			SB_PAGEDOWN,
			SB_LINEDOWN,
		};
		for (int i = 0; i < ARRAYSIZE(parts); i++)
		{
			CRect rc = GetPartRect(parts[i]);
			if (rc.PtInRect(pt))
				return parts[i];
		}
		return -1;
	}

	void SScrollBarHandler::OnDraw(IRenderTarget *pRT, int iPart) const
	{
		CRect rcPart = GetPartRect(iPart);
		DWORD dwState = GetPartState(iPart);
		if (iPart == kSbRail) iPart = SB_PAGEUP;
		m_pCB->GetScrollBarSkin(IsVertical())->DrawByState(pRT,rcPart,MAKESBSTATE(iPart,dwState,IsVertical()),GetAlpha());
	}

	void SScrollBarHandler::OnTimer(char id)
	{
		if (id == IScrollBarHost::Timer_Wait)
		{
			m_pCB->OnScrollKillTimer(m_bVert, IScrollBarHost::Timer_Wait);
			m_pCB->OnScrollSetTimer(m_bVert, IScrollBarHost::Timer_Go, IScrollBarHost::kTime_Go);
		}
		else if (id == IScrollBarHost::Timer_Go)
		{
			SASSERT(m_iClickPart != -1);
			SASSERT(m_iClickPart != SB_THUMBTRACK);
			if (m_iClickPart == m_iHitPart)
			{
				if (m_iClickPart == SB_PAGEUP || m_iClickPart == SB_PAGEDOWN)
				{
					int iHitPart = HitTest(m_ptCursor);
					if (iHitPart == SB_THUMBTRACK)
						return;
				}
				m_pCB->OnScrollCommand(m_bVert, m_iClickPart);
			}
		}
	}

	void SScrollBarHandler::OnMouseHover(CPoint pt)
	{
		if (m_iClickPart == SB_THUMBTRACK)
			return;

		if (m_iClickPart == -1)
		{
			if (GetInterpolator())
			{
				m_iFrame = 0;
				m_fadeMode = FADEIN;//to show
				GetContainer()->RegisterTimelineHandler(this);
			}
		}
		else
		{
			m_iHitPart = HitTest(pt);//update hit part.
			m_pCB->UpdateScrollBar(m_bVert, m_iClickPart);
		}
	}

	void SScrollBarHandler::OnMouseLeave()
	{
		if (m_iClickPart == SB_THUMBTRACK)
			return;

		int iOldHit = m_iHitPart;
		m_iHitPart = -1;

		if (m_iClickPart == -1)
		{
			if (!GetInterpolator())
			{
				if (iOldHit != -1) m_pCB->UpdateScrollBar(m_bVert, iOldHit);
			}
			else
			{
				m_iFrame = m_pCB->GetScrollFadeFrames() -1;
				m_fadeMode = FADEOUT;//to hide
				GetContainer()->RegisterTimelineHandler(this);
			}
		}
		else
		{
			m_pCB->UpdateScrollBar(m_bVert, m_iClickPart);
		}
	}

	void SScrollBarHandler::OnMouseMove(CPoint pt)
	{
		m_ptCursor = pt;
		int iNewHit = HitTest(pt);
		if (m_iClickPart==-1)
		{
			if (iNewHit != m_iHitPart)
			{
				int iOldHit = m_iHitPart;
				m_iHitPart = iNewHit;
				if(iOldHit!=-1)
					m_pCB->UpdateScrollBar(m_bVert, iOldHit);
				if(m_iHitPart!=-1)
					m_pCB->UpdateScrollBar(m_bVert, m_iHitPart);
			}
		}
		else if (m_iClickPart == SB_THUMBTRACK)
		{//draging.
			CRect rcWnd = m_pCB->GetScrollBarRect(m_bVert);
			int nInterHei = (IsVertical() ? rcWnd.Height() : rcWnd.Width()) - 2 * m_pCB->GetScrollBarArrowSize(m_bVert);
			const SCROLLINFO * m_si = m_pCB->GetScrollBarInfo(m_bVert);
			int    nSlideHei = m_si->nPage*nInterHei / (m_si->nMax - m_si->nMin + 1);
			if (nSlideHei<THUMB_MINSIZE) nSlideHei = THUMB_MINSIZE;
			if (nInterHei<THUMB_MINSIZE) nSlideHei = 0;
			int nEmptyHei = nInterHei - nSlideHei;
			int nDragLen = IsVertical() ? (pt.y - m_ptClick.y) : (pt.x - m_ptClick.x);
			int nSlide = (nEmptyHei == 0) ? 0 : (nDragLen*(int)(m_si->nMax - m_si->nMin - m_si->nPage + 1) / nEmptyHei);
			int nNewTrackPos = m_si->nPos + nSlide;
			if (nNewTrackPos<m_si->nMin)
			{
				nNewTrackPos = m_si->nMin;
			}
			else if (nNewTrackPos>(int)(m_si->nMax - m_si->nMin - m_si->nPage + 1))
			{
				nNewTrackPos = m_si->nMax - m_si->nMin - m_si->nPage + 1;
			}
			if (nNewTrackPos != m_si->nTrackPos)
			{
				m_pCB->OnScrollThumbTrackPos(m_bVert,nNewTrackPos);
			}
			m_iHitPart = iNewHit;
		}
		else if (m_iHitPart != iNewHit)
		{
			m_iHitPart = iNewHit;
			if (m_iHitPart == m_iClickPart)
			{
				m_pCB->OnScrollSetTimer(m_bVert,IScrollBarHost::Timer_Go, IScrollBarHost::kTime_Go);
			}
			else
			{
				m_pCB->OnScrollKillTimer(m_bVert,IScrollBarHost::Timer_Go);
			}
			m_pCB->UpdateScrollBar(m_bVert, m_iClickPart);
		}
	}

	void SScrollBarHandler::OnMouseUp(CPoint pt)
	{
		int iClickPart = m_iClickPart;
		SASSERT(iClickPart != -1);
		m_iClickPart = -1;
		m_pCB->UpdateScrollBar(m_bVert, iClickPart);
		if (m_iHitPart != -1 && m_iHitPart!= iClickPart)
		{
			m_pCB->UpdateScrollBar(m_bVert, m_iHitPart);
		}
		m_pCB->OnScrollKillTimer(m_bVert, IScrollBarHost::Timer_Wait);
		m_pCB->OnScrollKillTimer(m_bVert, IScrollBarHost::Timer_Go);

		m_pCB->OnScrollCommand(m_bVert, iClickPart== SB_THUMBTRACK? SB_THUMBPOSITION:iClickPart);

		if (iClickPart != -1 && m_iHitPart==-1)
		{
			OnMouseLeave();
		}
	}

	void SScrollBarHandler::OnMouseDown(CPoint pt)
	{
		m_iClickPart = HitTest(pt);
		m_ptClick = pt;
		if (m_fadeMode != FADE_STOP)
		{
			m_iFrame = m_pCB->GetScrollFadeFrames();//stop animate
			m_pCB->UpdateScrollBar(m_bVert, -1);
		}
		else
		{
			m_pCB->UpdateScrollBar(m_bVert, m_iHitPart);
		}
		switch (m_iClickPart)
		{
		case SB_LINEUP:
		case SB_LINEDOWN:
		case SB_PAGEUP:
		case SB_PAGEDOWN:
			m_pCB->OnScrollSetTimer(m_bVert, IScrollBarHost::Timer_Wait, IScrollBarHost::kTime_Wait);
			break;
		}
	}


	BYTE SScrollBarHandler::GetAlpha() const
	{
		if(!GetInterpolator())
			return 0xFF;
		float fProg = GetInterpolator()->getInterpolation(m_iFrame*1.0f/ m_pCB->GetScrollFadeFrames());
		return (BYTE)(fProg * 0xFF);
	}

	int SScrollBarHandler::GetFadeStep() const
	{
		switch(m_fadeMode)
		{
		case FADEIN: return 1;
		case FADEOUT:return -1;
		case FADE_STOP:
		default:return 0;
		}
	}

	int SScrollBarHandler::GetHitPart() const
	{
		return m_iHitPart;
	}

	int SScrollBarHandler::GetClickPart() const
	{
		return m_iClickPart;
	}

	DWORD SScrollBarHandler::GetPartState(int iPart) const
	{
		if (iPart == kSbRail)
			return SBST_NORMAL;
		if (!m_pCB->IsScrollBarEnable(m_bVert))
			return SBST_INACTIVE;

		DWORD dwState = SBST_NORMAL;
		if(iPart == m_iClickPart && 
			(m_iClickPart == m_iHitPart || m_iClickPart == SB_THUMBTRACK))
			dwState = SBST_PUSHDOWN;
		else if(iPart == m_iHitPart)
			dwState = SBST_HOVER;
		return dwState;
	}

}
