#include "StdAfx.h"
#include "SSkinAPNG.h"
#include <helper/SplitString.h>
#include <interface/SImgDecoder-i.h>
#include <interface/SRender-i.h>


namespace SOUI
{

    HRESULT SSkinAPNG::OnAttrSrc( const SStringW &strValue,BOOL bLoading )
    {
        SStringTList strLst;
        size_t nSegs=ParseResID(S_CW2T(strValue),strLst);
        LPBYTE pBuf=NULL;
        size_t szBuf=0;

        if(nSegs == 2)
        {
            szBuf=GETRESPROVIDER->GetRawBufferSize(strLst[0],strLst[1]);
            if(szBuf)
            {
                pBuf=new BYTE[szBuf];
                GETRESPROVIDER->GetRawBuffer(strLst[0],strLst[1],pBuf,szBuf);
            }
        }else
        {//自动从APNG资源类型里查找资源
            szBuf=GETRESPROVIDER->GetRawBufferSize(_T("apng"),strLst[0]);
            if(szBuf)
            {
                pBuf=new BYTE[szBuf];
                GETRESPROVIDER->GetRawBuffer(_T("apng"),strLst[0],pBuf,szBuf);
            }
        }
        if(pBuf)
        {
            LoadFromMemory(pBuf,szBuf);
            delete []pBuf;
        }
        return S_OK;
    }



int SSkinAPNG::LoadFromFile( LPCTSTR pszFileName )
{
    CAutoRefPtr<IImgX> imgX;
    GETRENDERFACTORY->GetImgDecoderFactory()->CreateImgX(&imgX);
    imgX->LoadFromFile(pszFileName);
    return _InitImgFrame(imgX);
}

int SSkinAPNG::LoadFromMemory( LPVOID pBuf,size_t dwSize )
{
    CAutoRefPtr<IImgX> imgX;
    GETRENDERFACTORY->GetImgDecoderFactory()->CreateImgX(&imgX);
    imgX->LoadFromMemory(pBuf,dwSize);
    return _InitImgFrame(imgX);
}

int SSkinAPNG::_InitImgFrame( IImgX *pImgX )
{
    if(m_pFrames) delete []m_pFrames;
    m_pFrames = NULL;
    m_nFrames =0;
    m_iFrame = 0;
    if(!pImgX) return 0;

    m_nFrames = pImgX->GetFrameCount();
    m_pFrames = new SAniFrame[m_nFrames];
    for(int i=0;i<m_nFrames;i++)
    {
        GETRENDERFACTORY->CreateBitmap(&m_pFrames[i].pBmp);
        m_pFrames[i].pBmp->Init(pImgX->GetFrame(i));
        m_pFrames[i].nDelay=pImgX->GetFrame(i)->GetDelay();
    }
    return m_nFrames;
}

void SSkinAPNG::_DrawByIndex2(IRenderTarget *pRT, LPCRECT rcDraw, int dwState,BYTE byAlpha/*=0xFF*/) const
{
	CRect rcSrc(CPoint(0,0),GetSkinSize());
	if(m_rcMargin.IsRectNull())
		pRT->DrawBitmapEx(rcDraw,m_pFrames[m_iFrame].pBmp,rcSrc,GetExpandCode(),byAlpha);
	else
		pRT->DrawBitmap9Patch(rcDraw,m_pFrames[m_iFrame].pBmp,rcSrc,m_rcMargin,GetExpandCode(),byAlpha);
}

long SSkinAPNG::GetFrameDelay(int iFrame/*=-1*/) const
{
	if(iFrame==-1) iFrame=m_iFrame;
	long nRet=-1;
	if(m_nFrames>1 && iFrame>=0 && iFrame<m_nFrames)
	{
		nRet=m_pFrames[iFrame].nDelay;
	}
	return nRet;
}

SIZE SSkinAPNG::GetSkinSize() const
{
	SIZE sz={0};
	if(m_nFrames>0 && m_pFrames)
	{
		sz=m_pFrames[0].pBmp->Size();
	}
	return sz;
}

IBitmap * SSkinAPNG::GetFrameImage(int iFrame/*=-1*/)
{
	if(iFrame==-1) iFrame=m_iFrame;
	long nRet=-1;
	if(m_nFrames>1 && iFrame>=0 && iFrame<m_nFrames)
	{
		return m_pFrames[iFrame].pBmp;
	}else
	{
		return NULL;
	}
}

}//end of namespace SOUI