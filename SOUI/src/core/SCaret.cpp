#include "souistd.h"
#include "core\SCaret.h"

namespace SOUI{

    SCaret::SCaret():m_bDrawCaret(false),m_bVisible(FALSE), m_nWaitFrame(0)
    {
 
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
			pRT->FillSolidRect(&rc, RGBA(0, 0, 0, 0xFF));
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
		pRT->DrawBitmap(rcCaret, m_bmpCaret, 0, 0);
	}

	BOOL SCaret::NextFrame()
	{
		if (!m_bVisible)
			return FALSE;

		m_nWaitFrame++;
		if (m_nWaitFrame%20 == 0)
		{
			m_bDrawCaret = !m_bDrawCaret;
			m_nWaitFrame = 0;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void SCaret::SetPosition(int x, int y)
	{
		m_ptCaret.x = x;
		m_ptCaret.y = y;
		m_nWaitFrame = 0;
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

		m_nWaitFrame = 0;
		if (m_bVisible)
		{
			m_bDrawCaret = TRUE;
		}
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
