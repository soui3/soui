#include <souistd.h>
#include <core/SScrollBarPainter.h>

namespace SOUI
{
	SScrollBarPainter::SScrollBarPainter(IScrollPainterCallback *pCB) :m_bVert(false),m_pCB(pCB)
		, m_nSpeed(30)
		, m_iFrame(0)
		, m_fadeMode(FADE_STOP)
		, m_iHitPart(-1)
		, m_iClickPart(-1)
	{
		SASSERT(m_pCB);
	}


	void SScrollBarPainter::SetVertical(bool bVert)
	{
		m_bVert = bVert;
	}

	void SScrollBarPainter::OnNextFrame()
	{
		SASSERT(m_fadeMode != FADE_STOP);
		if (m_iFrame>=0 && m_iFrame < m_nSpeed)
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

	CRect SScrollBarPainter::GetPartRect(int iPart) const
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

	bool SScrollBarPainter::IsVertical() const
	{
		return m_bVert;
	}

	ISwndContainer * SScrollBarPainter::GetContainer()
	{
		return m_pCB->GetScrollBarContainer();
	}

	int SScrollBarPainter::HitTest(CPoint pt) const
	{
		CRect rc = m_pCB->GetScrollBarRect(m_bVert);
		return 0;
	}

	void SScrollBarPainter::OnDraw(IRenderTarget *pRT, int iPart) const
	{
		CRect rcPart = GetPartRect(iPart);
		DWORD dwState = GetPartState(iPart);
		m_pCB->GetScrollBarSkin(IsVertical())->DrawByState(pRT,rcPart,MAKESBSTATE(iPart,dwState,IsVertical()),GetAlpha());
	}

	void SScrollBarPainter::OnMouseLeave()
	{
		int iOldHit = m_iHitPart;
		m_iHitPart = -1;
		if (!m_interpolator)
		{
			if(iOldHit!=-1) m_pCB->UpdateScrollBar(m_bVert, iOldHit);
		}
		else
		{
			m_fadeMode = FADEOUT;//to hide
			GetContainer()->RegisterTimelineHandler(this);
		}
	}

	void SScrollBarPainter::OnMouseMove(CPoint pt)
	{
		if (m_iClickPart==-1)
		{
			int iOldHit = m_iHitPart;
			m_iHitPart = HitTest(pt);
			if (iOldHit != m_iHitPart)
			{
				if(iOldHit!=-1)
					m_pCB->UpdateScrollBar(m_bVert, iOldHit);
				if(m_iHitPart!=-1)
					m_pCB->UpdateScrollBar(m_bVert, m_iHitPart);
			}
		}
	}

	void SScrollBarPainter::OnMouseUp(CPoint pt)
	{
		int iClickPart = m_iClickPart;
		m_iClickPart = -1;
		if(iClickPart != -1)
		{
			m_pCB->UpdateScrollBar(m_bVert, iClickPart);
		}
		if (m_iHitPart != -1 && m_iHitPart!= iClickPart)
		{
			m_pCB->UpdateScrollBar(m_bVert, m_iHitPart);
		}
	}

	void SScrollBarPainter::OnMouseDown(CPoint pt)
	{
		m_iClickPart = HitTest(pt);

		m_iFrame = m_nSpeed;//stop animate
		m_pCB->UpdateScrollBar(m_bVert, m_iHitPart);
	}

	void SScrollBarPainter::OnMouseHover(CPoint pt)
	{
		m_iHitPart = HitTest(pt);

		if (!m_interpolator)
		{
			m_pCB->UpdateScrollBar(m_bVert,m_iHitPart);
		}
		else
		{
			m_iFrame = 0;
			m_fadeMode = FADEIN;//to show
			GetContainer()->RegisterTimelineHandler(this);
		}
	}

	BYTE SScrollBarPainter::GetAlpha() const
	{
		if(!m_interpolator)
			return 0xFF;
		float fProg = m_interpolator->getInterpolation(m_iFrame*1.0f/m_nSpeed);
		return (BYTE)(fProg * 0xFF);
	}

	int SScrollBarPainter::GetFadeStep() const
	{
		switch(m_fadeMode)
		{
		case FADEIN: return 1;
		case FADEOUT:return -1;
		case FADE_STOP:
		default:return 0;
		}
	}

	int SScrollBarPainter::GetHitPart() const
	{
		return m_iHitPart;
	}

	int SScrollBarPainter::GetClickPart() const
	{
		return m_iClickPart;
	}

	DWORD SScrollBarPainter::GetPartState(int iPart) const
	{
		DWORD dwState = SBST_NORMAL;
		if(iPart == m_iHitPart)
			dwState = SBST_HOVER;
		else if(iPart == m_iClickPart)
			dwState = SBST_PUSHDOWN;
		if(!m_pCB->IsScrollBarEnable(m_bVert))
			dwState = SBST_INACTIVE;
		return dwState;
	}

}
