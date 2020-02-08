#include "souistd.h"
#include "control/SHotKeyCtrl.h"
#include <CommCtrl.h>

namespace SOUI
{

SHotKeyCtrl::SHotKeyCtrl(void)
{
    m_wInvalidModifier=0;
    m_wInvalidComb=HKCOMB_NONE;
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
	
	GetContainer()->GetCaret()->SetVisible(true);
    
    __super::OnSetFocus(wndOld);
    
}

void SHotKeyCtrl::OnKillFocus(SWND wndFocus)
{
    GetContainer()->GetCaret()->SetVisible(false);
    __super::OnKillFocus(wndFocus);
}

void SHotKeyCtrl::UpdateModifier()
{
    BOOL bAlt=GetKeyState(VK_MENU)&0x8000;
    BOOL bCtrl=GetKeyState(VK_CONTROL)&0x8000;
    BOOL bShift=GetKeyState(VK_SHIFT)&0x8000;

    WORD wCombKey=0;
    if(!bAlt && !bCtrl && !bShift) wCombKey=HKCOMB_NONE,m_wModifier=0;
    else if(bAlt && !bCtrl && !bShift) wCombKey=HKCOMB_A,m_wModifier=MOD_ALT;
    else if(!bAlt && bCtrl && !bShift) wCombKey=HKCOMB_C,m_wModifier=MOD_CONTROL;
    else if(!bAlt && !bCtrl && bShift) wCombKey=HKCOMB_S,m_wModifier=MOD_SHIFT;
    else if(bAlt && bCtrl && !bShift) wCombKey=HKCOMB_CA,m_wModifier=MOD_ALT|MOD_CONTROL;
    else if(bAlt && !bCtrl && bShift) wCombKey=HKCOMB_SA,m_wModifier=MOD_SHIFT|MOD_ALT;
    else if(!bAlt && bCtrl && bShift) wCombKey=HKCOMB_SC,m_wModifier=MOD_SHIFT|MOD_CONTROL;
    else wCombKey=HKCOMB_SCA,m_wModifier=MOD_ALT|MOD_SHIFT|MOD_CONTROL;
    if(wCombKey&m_wInvalidComb)
        m_wModifier=m_wInvalidModifier;

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
	WORD  wComb = HIWORD(dwKey);
	switch(wComb)
	{
	case MOD_ALT:
		m_wInvalidComb = HKCOMB_A;
		break;
	case MOD_CONTROL:
		m_wInvalidComb = HKCOMB_C;
		break;
	case MOD_SHIFT:
		m_wInvalidComb = HKCOMB_S;
		break;
	case MOD_CONTROL|MOD_ALT:
		m_wInvalidComb = HKCOMB_CA;
		break;
	case MOD_SHIFT|MOD_ALT:
		m_wInvalidComb = HKCOMB_SA;
		break;
	case MOD_SHIFT|MOD_CONTROL:
		m_wInvalidComb = HKCOMB_SC;
		break;
	case MOD_SHIFT|MOD_CONTROL|MOD_ALT:
		m_wInvalidComb = HKCOMB_SCA;
		break;
	default:
		m_wInvalidComb = HKCOMB_NONE;
		break;
	}
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
