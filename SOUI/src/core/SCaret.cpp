#include "souistd.h"
#include "core\SCaret.h"
#include "animation\SInterpolatorImpl.h"

namespace SOUI{

    SCaret::SCaret(ISwndContainer *pContainer)
		:m_pContainer(pContainer)
		,m_bDrawCaret(true),m_bVisible(FALSE),m_iFrame(0),m_byAlpha(0xFF)
		,m_crCaret(RGBA(0,0,0,255)), m_nAniFrames(20),m_nShowFrames(20),m_bAniCaret(FALSE)
    {
		m_AniInterpolator.Attach(CREATEINTERPOLATOR(SAccelerateInterpolator::GetClassName()));
	}


	BOOL SCaret::Init(HBITMAP hBmp, int nWid, int nHei)
	{
		m_bDrawCaret = true;
		SAutoRefPtr<IRenderTarget> pRT;
		GETRENDERFACTORY->CreateRenderTarget(&pRT, nWid, nHei);
		m_bmpCaret = (IBitmap*)pRT->GetCurrentObject(OT_BITMAP);
		if (hBmp)
		{
			//以拉伸方式创建一个插入符位图
			HDC hdc = pRT->GetDC(0);
			HDC hdc2 = CreateCompatibleDC(hdc);
			SelectObject(hdc2, hBmp);

			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);
			StretchBlt(hdc, 0, 0, nWid, nHei, hdc2, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			DeleteDC(hdc2);
			pRT->ReleaseDC(hdc);
		}
		else
		{
			//创建一个黑色插入符的位图
			CRect rc(0, 0, nWid, nHei);
			pRT->FillSolidRect(&rc, m_crCaret);
		}
		return TRUE;
	}

	void SCaret::Draw(IRenderTarget * pRT)
	{
		if (!m_bVisible)
			return;
		if (!m_bDrawCaret)
			return;
		if (!m_bmpCaret)
			return;
		CRect rcCaret = GetRect();
		pRT->DrawBitmap(rcCaret, m_bmpCaret, 0, 0,m_byAlpha);
	}

	void SCaret::OnNextFrame()
	{
		if (!m_bVisible)
			return;

		m_iFrame++;
		int nFrameCount=(m_nShowFrames + m_nAniFrames*2);
		if(m_iFrame%nFrameCount==0)
			m_iFrame=0;

		if (m_bAniCaret)
		{
			if(m_iFrame<m_nShowFrames)
				m_byAlpha = 255;//visible
			else if(m_iFrame<m_nShowFrames+m_nAniFrames)
			{//fadeout
				int iFrame = m_iFrame-m_nShowFrames;
				m_byAlpha = (BYTE)(255 * m_AniInterpolator->getInterpolation(1.0f - iFrame*1.0f / m_nAniFrames));
			}else
			{//fadein
				int iFrame = m_iFrame-m_nShowFrames-m_nAniFrames;
				m_byAlpha = (BYTE)(255 * m_AniInterpolator->getInterpolation( iFrame*1.0f / m_nAniFrames));
			}
			m_pContainer->OnRedraw(GetRect());
		}
		else
		{
			if(m_iFrame == 0)
			{
				m_bDrawCaret = true;
				m_byAlpha = 255;
				m_pContainer->OnRedraw(GetRect());
			}else if(m_iFrame == m_nShowFrames)
			{
				m_bDrawCaret = false;
				m_pContainer->OnRedraw(GetRect());
			}
		}
	}

	void SCaret::SetPosition(int x, int y)
	{
		m_ptCaret.x = x;
		m_ptCaret.y = y;
		m_iFrame = 0;
		if (m_bVisible)
		{
			m_bDrawCaret = TRUE;
		}
	}

	BOOL SCaret::SetVisible(BOOL bVisible)
	{
		if (bVisible == m_bVisible)
			return FALSE;
		m_bVisible = bVisible;

		m_iFrame = 0;
		if (m_bVisible)
		{
			m_bDrawCaret = TRUE;
		}
		if(bVisible)
			m_pContainer->RegisterTimelineHandler(this);
		else
			m_pContainer->UnregisterTimelineHandler(this);
		return TRUE;
	}

	BOOL SCaret::IsVisible() const
	{
		return m_bVisible;
	}

	RECT SCaret::GetRect() const
	{
		CSize szCaret;
		if (m_bmpCaret) szCaret = m_bmpCaret->Size();
		return	CRect(m_ptCaret, szCaret);
	}

}
