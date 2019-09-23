#include "souistd.h"
#include "helper/SMemDC.h"
namespace SOUI
{

SMemDC::SMemDC()
    :m_bBmpOwner(FALSE)
    ,m_bHasBitmap(FALSE)
    ,m_hOldBmp(NULL)
    ,m_hDC(NULL)
{
}

SMemDC::SMemDC( HDC hdc, const CRect & rc )
    :m_bBmpOwner(FALSE)
    ,m_bHasBitmap(FALSE)
    ,m_hOldBmp(NULL)
{
    InitDC(hdc,rc);
}

SMemDC::SMemDC( HDC hdc,HBITMAP hBmp)
    :m_bBmpOwner(FALSE)
    ,m_bHasBitmap(TRUE)
{
    m_hDC = CreateCompatibleDC(hdc);
    SASSERT(m_hDC != NULL);
    if(hBmp)
        m_hOldBmp=(HBITMAP)::SelectObject(m_hDC,hBmp);
    else
        m_hOldBmp=NULL;
    ::SetViewportOrgEx(m_hDC,0,0,NULL);
}

SMemDC::~SMemDC(void)
{
    DeleteDC();
}

HBITMAP SMemDC::SelectBitmap( HBITMAP hBmp )
{
    SASSERT(m_hDC);
    if(hBmp)
    {
        m_hOldBmp=(HBITMAP)::SelectObject(m_hDC,hBmp);
        m_bBmpOwner=FALSE;
        m_bHasBitmap=TRUE;
        ::SetViewportOrgEx(m_hDC,0,0,NULL);
        return m_hOldBmp;
    }
    else if(m_bHasBitmap)
    {
        HBITMAP hBmp=(HBITMAP)::SelectObject(m_hDC,m_hOldBmp);
        m_hOldBmp=NULL;
        m_bBmpOwner=FALSE;
        m_bHasBitmap=FALSE;
        return hBmp;
    }
    else
    {
        return NULL;
    }
}

void SMemDC::DeleteDC()
{
    if(m_hDC && m_hOldBmp)
    {
        HBITMAP hBmp=(HBITMAP)::SelectObject(m_hDC,m_hOldBmp);
        if(m_bBmpOwner) DeleteObject(hBmp);
    }
    if(m_hDC) ::DeleteDC(m_hDC);
    m_bHasBitmap=FALSE;
    m_bBmpOwner=FALSE;
}

BOOL SMemDC::InitDC( HDC hdc,const CRect &rc )
{
    if(m_hDC) return FALSE;
    m_hDC=::CreateCompatibleDC(hdc);
    if(!m_hDC) return FALSE;
    HBITMAP hBmp=CreateCompatibleBitmap(hdc,rc.Width(),rc.Height());
    if(!hBmp)
    {
        ::DeleteDC(m_hDC);
        return FALSE;
    }
    m_hOldBmp=(HBITMAP)SelectObject(m_hDC,hBmp);
    ::SetViewportOrgEx(m_hDC,-rc.left, -rc.top,NULL);
    m_bHasBitmap=TRUE;
    m_bBmpOwner=TRUE;
    return TRUE;
}

}//namespace SOUI
