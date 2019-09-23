#pragma once

namespace SOUI
{

class SOUI_EXP SMemDC
{
public:
    SMemDC();
    SMemDC(HDC hdc, const CRect & rc);
    SMemDC(HDC hdc, HBITMAP hBmp);
    ~SMemDC(void);

    HBITMAP SelectBitmap(HBITMAP hBmp);

    BOOL    HasBitmap()
    {
        return m_bHasBitmap;
    }

    void SetBitmapOwner(BOOL bOwner)
    {
        m_bBmpOwner=bOwner;
    }
    BOOL IsBitmapOwner(){return m_bBmpOwner;}

    void DeleteDC();

    BOOL InitDC(HDC hdc,const CRect &rc);

    operator HDC() const
    {
        return m_hDC;
    }

protected:
    HDC     m_hDC;
    
    HBITMAP        m_hOldBmp;
    BOOL        m_bBmpOwner;
    BOOL        m_bHasBitmap;
};

}//namespace SOUI