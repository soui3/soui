#include "StdAfx.h"
#include "SColorPicker.h"

namespace SOUI
{

SColorPicker::SColorPicker(void):m_crDef(RGBA(0,0,0,255)),m_crCur(RGBA(0,0,0,255))
{
    GetEventSet()->addEvent(EVENTID(EventColorChange));
}

SColorPicker::~SColorPicker(void)
{
}

void SColorPicker::OnPaint( IRenderTarget *pRT)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	pRT->FillSolidRect(&rcClient,m_crCur);
	CAutoRefPtr<IPen> pen,oldPen;
	pRT->CreatePen(PS_DOT,RGBA(0xcc,0xcc,0xcc,0xff),1,&pen);
	pRT->SelectObject(pen,(IRenderObj**)&oldPen);
	pRT->DrawRectangle(&rcClient);
	pRT->SelectObject(oldPen);
}

void SColorPicker::OnLButtonUp( UINT nFlags,CPoint pt )
{
	__super::OnLButtonUp(nFlags,pt);
	CColourPopup *pCrPopup = new CColourPopup(GetContainer()->GetHostHwnd(),this);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	pt.x=rcWnd.left,pt.y=rcWnd.bottom;
	::ClientToScreen(GetContainer()->GetHostHwnd(),&pt);
	pCrPopup->SetDefColor(m_crDef);
	pCrPopup->Create(pt,m_crCur,_T("默认"),_T("更多"));
}

void SColorPicker::OnColorChanged( COLORREF cr )
{
	m_crCur=cr|0xff000000;
	Invalidate();
}

void SColorPicker::OnColorEnd( BOOL bCancel,COLORREF cr )
{
	if(bCancel) m_crCur=m_crDef;
	else m_crCur=cr|0xff000000;
	ModifyState(0,WndState_PushDown,TRUE);
    EventColorChange evt(this,m_crCur);
    FireEvent(evt);
}

SMessageLoop * SColorPicker::GetMsgLoop()
{
    return GetContainer()->GetMsgLoop();
}

}//end of namespace