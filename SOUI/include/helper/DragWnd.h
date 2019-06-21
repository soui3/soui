#pragma once

#include "core/SNativeWnd.h"

namespace SOUI
{

class SDragWnd : public SNativeWnd
{
public:
    SDragWnd(void);
    ~SDragWnd(void);

    static BOOL BeginDrag(HBITMAP hBmp,POINT ptHot ,COLORREF crKey, BYTE byAlpha,DWORD dwFlags);
    static void DragMove(POINT pt);
    static void EndDrag();
protected:

    void OnPaint(HDC dc);

    BEGIN_MSG_MAP_EX(SDragWnd)
        MSG_WM_PAINT(OnPaint)
    END_MSG_MAP()

    CPoint m_ptHot;
    HBITMAP m_bmp;

    static SDragWnd * s_pCurDragWnd;
};

}//end of namespace
