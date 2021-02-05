#include "stdafx.h"
#include "SGifPlayer.h"
#include "sskingif.h"
#include "SSkinAPNG.h"

namespace SOUI
{

SGifPlayer::SGifPlayer() 
	: m_iCurFrame(0)
	, m_nNextInterval(0)
	, m_bEnableScale(TRUE)
	, m_nScale(100)
	, m_bLoop(TRUE)
	, m_bTile(TRUE)
{

}

SGifPlayer::~SGifPlayer()
{
}


void SGifPlayer::OnPaint( IRenderTarget *pRT )
{	
	__super::OnPaint(pRT);
	if (m_aniSkin)
	{		
		m_aniSkin->DrawByIndex(pRT, GetWindowRect(),m_iCurFrame);
	}
}

void SGifPlayer::OnShowWindow( BOOL bShow, UINT nStatus )
{
	__super::OnShowWindow(bShow,nStatus);
	if(!bShow)
	{
        GetContainer()->UnregisterTimelineHandler(this);
	}else if(m_aniSkin && m_aniSkin->GetStates()>1)
	{
        GetContainer()->RegisterTimelineHandler(this);
        if(m_aniSkin->GetFrameDelay()==0)
            m_nNextInterval = 90;
        else
            m_nNextInterval = m_aniSkin->GetFrameDelay()*10;
	}
}

void SGifPlayer::OnNextFrame()
{
    m_nNextInterval -= 10;
    if(m_nNextInterval <= 0 && m_aniSkin)
    {
        int nStates=m_aniSkin->GetStates();
        m_iCurFrame++;
		if (!m_bLoop && m_iCurFrame >= nStates)
    	{
			GetContainer()->UnregisterTimelineHandler(this);
			EventGifPlayOver evt(this);
			FireEvent(evt);
			return;
    	}
        m_iCurFrame%=nStates;
        Invalidate();

        if(m_aniSkin->GetFrameDelay()==0)
            m_nNextInterval = 90;
        else
            m_nNextInterval =m_aniSkin->GetFrameDelay()*10;	
    }
}

HRESULT SGifPlayer::OnAttrSkin( const SStringW & strValue, BOOL bLoading )
{
	ISkinObj *pSkin = SSkinPoolMgr::getSingleton().GetSkin(strValue,GetScale());
	if(!pSkin) return E_FAIL;
	if(!pSkin->IsClass(SSkinAni::GetClassName())) return S_FALSE;
	m_aniSkin=static_cast<SSkinAni*>(pSkin);
    if(!bLoading)
    {
        m_iCurFrame = 0;
        if(m_aniSkin->GetFrameDelay()==0)
            m_nNextInterval = 90;
        else
            m_nNextInterval =m_aniSkin->GetFrameDelay()*10;	
    }
	return bLoading?S_OK:S_FALSE;
}

SIZE SGifPlayer::GetDesiredSize(int wid,int hei )
{
	CSize sz;
	if(m_aniSkin) sz=m_aniSkin->GetSkinSize();
	return sz;
}

BOOL SGifPlayer::PlayGifFile( LPCTSTR pszFileName )
{
    return _PlayFile(pszFileName,TRUE);
}

BOOL SGifPlayer::PlayAPNGFile( LPCTSTR pszFileName )
{
    return _PlayFile(pszFileName,FALSE);
}

BOOL SGifPlayer::_PlayFile( LPCTSTR pszFileName, BOOL bGif )
{
	SSkinAni *pGifSkin = (SSkinAni*)SApplication::getSingleton().CreateSkinByName(bGif?SSkinGif::GetClassName():SSkinAPNG::GetClassName());
	if(!pGifSkin) return FALSE;
	if(0==pGifSkin->LoadFromFile(pszFileName))
	{
		pGifSkin->Release();
		return FALSE;
	}

	GetContainer()->UnregisterTimelineHandler(this);

	m_aniSkin = pGifSkin;
	pGifSkin->Release();

	m_iCurFrame = 0;
	TCHAR buff[16] = {0};
	m_aniSkin->SetAttribute(_T("enableScale"), _itot(m_bEnableScale, buff, 10));
	m_aniSkin->SetAttribute(_T("scale"), _itot(m_nScale, buff, 10));
	m_aniSkin->SetAttribute(_T("tile"), _itot(m_bTile, buff, 10));
	
	if(GetLayoutParam()->IsWrapContent(Any))
	{
		RequestRelayout();
	}
	if(IsVisible(TRUE))
	{
		GetContainer()->RegisterTimelineHandler(this);
	}
	return TRUE;
}
	
void SGifPlayer::OnDestroy()
{
    GetContainer()->UnregisterTimelineHandler(this);
    __super::OnDestroy();
}
}
