#include "StdAfx.h"
#include "SSkinPNGX.h"
#include <helper/SplitString.h>
#include <interface/imgdecoder-i.h>
#include <interface/render-i.h>


namespace SOUI
{

HRESULT SSkinPNGX::OnAttrDelay(const SStringW &strValue,BOOL bLoading)
{
	SStringWList strDelays;
	int nSegs = (int)SplitString(strValue,L',',strDelays);
	m_nDelays.RemoveAll();
	for(int i=0;i<nSegs;i++)
	{
		int nDelay=0,nRepeat=1;
		SStringW strSub = strDelays[i];
		int nReaded = swscanf(strSub,L"%d[%d]",&nDelay,&nRepeat);
		for(int j=0;j<nRepeat;j++)
			m_nDelays.Add(nDelay);
	}
	return S_FALSE;
}

int SSkinPNGX::GetStates() const
{
	return (int)m_nDelays.GetCount();
}

SIZE SSkinPNGX::GetSkinSize() const
{
	CSize szSkin;
	if(m_pngx)
	{
		szSkin.cx = m_pngx->Width();
		szSkin.cy = m_pngx->Height();

		if(m_bVert)
			szSkin.cy/=(int)m_nDelays.GetCount();
		else
			szSkin.cx/=(int)m_nDelays.GetCount();
	}
	return szSkin;
}

long SSkinPNGX::GetFrameDelay(int iFrame/*=-1*/) const
{
	if(iFrame == -1)
		iFrame = m_iFrame;
	SASSERT(iFrame>=0 && iFrame < GetStates());
	return m_nDelays[iFrame];
}

void SSkinPNGX::_DrawByIndex2(IRenderTarget *pRT, LPCRECT rcDraw, int dwState,BYTE byAlpha/*=0xFF*/) const
{
	if(m_pngx)
	{
		CRect rcSrc(CPoint(0,0),GetSkinSize());
		if(m_bVert)
			rcSrc.OffsetRect(0,rcSrc.Height()*dwState);
		else
			rcSrc.OffsetRect(rcSrc.Width()*dwState,0);
		pRT->DrawBitmapEx(rcDraw,m_pngx,rcSrc,EM_STRETCH,byAlpha);
	}
}

}//end of namespace SOUI