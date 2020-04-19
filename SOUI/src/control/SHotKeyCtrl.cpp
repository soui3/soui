#include "souistd.h"
#include "control/SHotKeyCtrl.h"
#include <CommCtrl.h>

namespace SOUI
{

SHotKeyCtrl::SHotKeyCtrl(void)
{
    m_wInvalidModifier=Mod_None;
    m_wInvalidComb=Mod_None;
    m_bInSetting=FALSE;
    m_bFocusable=TRUE;
	m_evtSet.addEvent(EVENTID(EventSetHotKey));
}

SHotKeyCtrl::~SHotKeyCtrl(void)
{
}

void SHotKeyCtrl::OnLButtonDown( UINT nFlags,CPoint pt )
{
    __super::OnLButtonDown(nFlags,pt);
    GetContainer()->OnSetSwndFocus(m_swnd);
}

void SHotKeyCtrl::OnPaint( IRenderTarget * pRT )
{
    SPainter painter;
    BeforePaint(pRT,painter);
    CRect rcClient;
    GetTextRect(&rcClient);
    SStringT str=GetWindowText();
	UINT uAlign = GetTextAlign();
    pRT->DrawText(str,str.GetLength(),&rcClient, uAlign);
    AfterPaint(pRT,painter);
}

void SHotKeyCtrl::UpdateCaret(IRenderTarget *pRT)
{
    SStringT str=GetWindowText();
    SIZE szTxt;
    pRT->MeasureText(str,str.GetLength(),&szTxt);
    
    CRect rcClient;
    GetTextRect(&rcClient);

	UINT uAlign = GetTextAlign();
	int x = rcClient.left + szTxt.cx;
	int y = rcClient.top + (rcClient.Height() - szTxt.cy) / 2;
	if (uAlign & DT_CENTER)
		x += (rcClient.Width() - szTxt.cx) / 2;
	else if (uAlign & DT_RIGHT)
		x = rcClient.right;

	SetCaretPos(x, y);
}

UINT SHotKeyCtrl::GetTextAlign()
{
	UINT uAlign = SWindow::GetTextAlign();
	uAlign &= ~DT_BOTTOM;
	uAlign |= DT_VCENTER | DT_SINGLELINE;
	return uAlign;
}

void SHotKeyCtrl::OnSetFocus(SWND wndOld)
{
    IRenderTarget *pRT=GetRenderTarget(NULL,GRT_NODRAW);
	BeforePaintEx(pRT);
    SIZE szTxt;
    pRT->MeasureText(_T("A"),1,&szTxt);
    
    CreateCaret(NULL,1,szTxt.cy);

    UpdateCaret(pRT);

	ReleaseRenderTarget(pRT);
	
	GetContainer()->EnableIME(FALSE);
	ShowCaret(TRUE);
    
    __super::OnSetFocus(wndOld);
    
}

void SHotKeyCtrl::OnKillFocus(SWND wndFocus)
{
	ShowCaret(FALSE);
	GetContainer()->EnableIME(TRUE);
    __super::OnKillFocus(wndFocus);
}

void SHotKeyCtrl::UpdateModifier()
{
    BOOL bAlt=GetKeyState(VK_MENU)&0x8000;
    BOOL bCtrl=GetKeyState(VK_CONTROL)&0x8000;
    BOOL bShift=GetKeyState(VK_SHIFT)&0x8000;

    WORD wCombKey=0;
    if(!bAlt && !bCtrl && !bShift) wCombKey=Mod_None;
    else if(bAlt && !bCtrl && !bShift) wCombKey=Mod_Alt;
    else if(!bAlt && bCtrl && !bShift) wCombKey=Mod_Ctrl;
    else if(!bAlt && !bCtrl && bShift) wCombKey=Mod_Shift;
    else if(bAlt && bCtrl && !bShift) wCombKey=Mod_CA;
    else if(bAlt && !bCtrl && bShift) wCombKey=Mod_SA;
    else if(!bAlt && bCtrl && bShift) wCombKey=Mod_SC;
    else wCombKey=Mod_SCA;
    if(wCombKey==m_wInvalidComb)
        m_wModifier=m_wInvalidModifier;
	else
		m_wModifier = wCombKey;

	EventSetHotKey ev(this);
	ev.wModifiers = m_wModifier;
	ev.vKey = m_wVK;
	FireEvent(ev);
}

void SHotKeyCtrl::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    if(!m_bInSetting)
    {
        m_bInSetting=TRUE;
        m_wVK=0;
        m_wModifier=m_wInvalidModifier;
    }
    SStringT strKey=GetKeyName(nChar);
    if(!strKey.IsEmpty())
    {
        m_wVK=nChar;
    }
    UpdateModifier();
	IRenderTarget *pRT = GetRenderTarget(NULL, GRT_NODRAW);
	BeforePaintEx(pRT);
    UpdateCaret(pRT);
	ReleaseRenderTarget(pRT);
    Invalidate();
}

void SHotKeyCtrl::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    if(!m_bInSetting) return;

	IRenderTarget *pRT = GetRenderTarget(NULL, GRT_NODRAW);
	BeforePaintEx(pRT);

    if(nChar == m_wVK)
    {
        m_bInSetting=FALSE;
    }
    else if(m_wVK==0 && (GetKeyState(VK_SHIFT)&0x8000)==0 && (GetKeyState(VK_MENU)&0x8000)==0 && (GetKeyState(VK_CONTROL)&0x8000)==0)
    {
        m_bInSetting=FALSE;
        UpdateModifier();
        UpdateCaret(pRT);
        Invalidate();
    }
    else if(nChar==VK_SHIFT || nChar==VK_MENU || nChar== VK_CONTROL)
    {
        UpdateModifier();
        UpdateCaret(pRT);
        Invalidate();
    }
	ReleaseRenderTarget(pRT);
}

void SHotKeyCtrl::OnSysKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    if(GetKeyState(VK_MENU)&0x8000) OnKeyDown(nChar,nRepCnt,nFlags);
    else SetMsgHandled(FALSE);
}

void SHotKeyCtrl::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if(nChar==VK_MENU || GetKeyState(VK_MENU)&0x8000) OnKeyUp(nChar,nRepCnt,nFlags);
    else SetMsgHandled(FALSE);
}

void SHotKeyCtrl::SetRule( WORD wInvalidComp,WORD wModifier )
{
    m_wInvalidComb=wInvalidComp;
    m_wInvalidModifier=wModifier;
}

void SHotKeyCtrl::SetHotKey( WORD vKey,WORD wModifiers )
{
    m_wVK=vKey;
    m_wModifier=wModifiers;
	IRenderTarget *pRT = GetRenderTarget(NULL, GRT_NODRAW);
	BeforePaintEx(pRT);
    UpdateCaret(pRT);
	ReleaseRenderTarget(pRT);
    Invalidate();
}

void SHotKeyCtrl::GetHotKey( WORD & vKey,WORD &wModifers )
{
    vKey=m_wVK;
    wModifers=m_wModifier;
}

SStringT SHotKeyCtrl::GetWindowText(BOOL bRawText)
{
	return FormatHotkey();
}

HRESULT SHotKeyCtrl::OnAttrInvalidComb(const SStringW & value, BOOL bLoading)
{
	DWORD dwKey = TranslateAccelKey(S_CW2T(value));
	m_wInvalidComb = HIWORD(dwKey);
	return bLoading?S_OK:S_FALSE;
}

HRESULT SHotKeyCtrl::OnAttrInvalidModifier(const SStringW & value, BOOL bLoading)
{
	DWORD dwKey = TranslateAccelKey(S_CW2T(value));
	m_wInvalidModifier = HIWORD(dwKey);
	return S_FALSE;
}

HRESULT SHotKeyCtrl::OnAttrHotKey(const SStringW & value, BOOL bLoading)
{
	DWORD dwKey = TranslateAccelKey(S_CW2T(value));
	m_wModifier = HIWORD(dwKey);
	m_wVK = LOWORD(dwKey);
	return bLoading?S_OK:S_FALSE;
}

}//namespace SOUI
