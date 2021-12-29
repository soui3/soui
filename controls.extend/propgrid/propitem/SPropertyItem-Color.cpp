#include "stdafx.h"
#include "SPropertyItem-Color.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"
#include <commdlg.h>
#include <res.mgr/SNamedValue.h>

const int KColorWidth   = 30;
const int KTransGridSize    =5;
namespace SOUI
{
	SPropertyItemColor::SPropertyItemColor(SPropertyGrid *pOwner) :SPropertyItemText(pOwner),m_crValue(CR_INVALID)
	{
		m_strFormat = _T("#%02x%02x%02x%02x");
	}

    void SPropertyItemColor::DrawItem( IRenderTarget *pRT,CRect rc )
    {
        CRect rcColor = rc;
        rcColor.right = rcColor.left + KColorWidth;
        rcColor.DeflateRect(2,2);
        
        //画一个代表透明的网格背景
        pRT->PushClipRect(&rcColor,RGN_AND);
        bool bDrawY=true;
        for(int y=rcColor.top;y<rcColor.bottom;y+=KTransGridSize)
        {
            bool bDraw= bDrawY;
            for(int x=rcColor.left;x<rcColor.right;x+=KTransGridSize)
            {
                if(bDraw) pRT->FillSolidRect(CRect(CPoint(x,y),CSize(KTransGridSize,KTransGridSize)),RGBA(0xcc,0xcc,0xcc,0xff));
                bDraw=!bDraw;
            }
            bDrawY=!bDrawY;
        }
        pRT->PopClip();
        
		if(m_crValue!=CR_INVALID)
		{
			pRT->FillSolidRect(&rcColor,m_crValue);
			pRT->DrawRectangle(&rcColor);
			CRect rcValue = rc;
			rcValue.left += KColorWidth;
			SStringT strValue = GetValue();
			pRT->DrawText(strValue,strValue.GetLength(),&rcValue,DT_SINGLELINE|DT_VCENTER);
		}else
		{
			SAutoRefPtr<IPen> pen,oldPen;
			pRT->CreatePen(PS_SOLID,RGBA(255,0,0,255),1,&pen);
			pRT->SelectObject(pen,(IRenderObj**)&oldPen);
			{
				CPoint pts[2]={rcColor.TopLeft(),rcColor.BottomRight()};
				pRT->DrawLines(pts,2);
			}
			{
				CPoint pts[2]={CPoint(rcColor.left,rcColor.bottom),CPoint(rcColor.right,rcColor.top)};
				pRT->DrawLines(pts,2);
			}
			pRT->SelectObject(oldPen,NULL);
		}
    }
    
    void SPropertyItemColor::OnInplaceActive(BOOL bActive)
    {
        SPropertyItemText::OnInplaceActive(bActive);
        if(bActive)
        {
            LRESULT lr=m_pEdit->SSendMessage(EM_SETEVENTMASK,0,ENM_CHANGE);
        }
    }

    bool SPropertyItemColor::OnReNotify(IEvtArgs *pEvt)
    {
        EventRENotify *pReEvt = (EventRENotify*)pEvt;
        if(pReEvt->iNotify == EN_CHANGE)
        {
			SStringT strValue=m_pEdit->GetWindowText();
			int r,g,b,a;
			int nGet=_stscanf(strValue,m_strFormat,&r,&g,&b,&a);
			if(nGet==4)
			{
			m_crValue = RGBA(r,g,b,a);
			CRect rcColor;
			m_pEdit->GetWindowRect(&rcColor);
			rcColor.right=rcColor.left;
			rcColor.left -= KColorWidth;
			m_pOwner->InvalidateRect(&rcColor);
			}
        }
        return true;
    }
    

    void SPropertyItemColor::SetValue( const SStringT & strValue )
    {

		COLORREF crTmp;
		crTmp = CR_INVALID;
		SColorParser::ParseValue(strValue,crTmp);
		if (m_crValue != crTmp)
		{
			m_crValue = crTmp;
			OnValueChanged();
		}
    }

    BOOL SPropertyItemColor::OnButtonClick()
    {
		CColourPopup *pCrPopup = new CColourPopup(GetOwner()->GetContainer()->GetHostHwnd(),this);
		CPoint pt;
		GetCursorPos(&pt);
		pt.x += 10;
		pCrPopup->Create(pt,m_crValue,_T("默认"),_T("更多"));
		return TRUE;
    }

    void SPropertyItemColor::AdjustInplaceActiveWndRect( CRect & rc )
    {
        __super::AdjustInplaceActiveWndRect(rc);
        rc.left += KColorWidth;
    }

	void SPropertyItemColor::OnColorChanged( COLORREF cr )
	{
	}

	void SPropertyItemColor::OnColorEnd( BOOL bCancel,COLORREF cr )
	{
		if(!bCancel)
		{
			COLORREF crTmp = cr|0xff000000;

		  if (m_crValue == crTmp)
		  {
			  return;
		  }else
		  {
			  m_crValue = crTmp;
		  }
		  OnValueChanged();
		  CRect rc=GetOwner()->GetItemRect(this);
		  GetOwner()->InvalidateRect(&rc);
		}
	}

	IMessageLoop * SPropertyItemColor::GetMsgLoop()
	{
		return GetOwner()->GetContainer()->GetMsgLoop();
	}

	SStringT SPropertyItemColor::GetValue() const
	{
		if (m_crValue == CR_INVALID)
		{
			return _T("");
		}

		SStringT str;
		int r,g,b,a;
		r = GetRValue(m_crValue);
		g = GetGValue(m_crValue);
		b = GetBValue(m_crValue);
		a = GetAValue(m_crValue);
		str.Format(m_strFormat,r,g,b,a);
		return str;
	}

	BOOL SPropertyItemColor::HasValue() const
	{
		return m_crValue != CR_INVALID;
	}

	void SPropertyItemColor::ClearValue()
	{
		m_crValue = CR_INVALID;
		OnValueChanged();
	}

}
