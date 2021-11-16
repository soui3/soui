#include "stdafx.h"
#include "SPropertyItem-Color.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"
#include <commdlg.h>

const int KColorWidth   = 30;
const int KTransGridSize    =5;
namespace SOUI
{
    void SPropertyItemColor::DrawItem( IRenderTarget *pRT,CRect rc )
    {
        CRect rcColor = rc;
        rcColor.right = rcColor.left + KColorWidth;
        rcColor.DeflateRect(2,2);
        
        //画一个代表透明的网格背景
        pRT->PushClipRect(&rcColor);
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
        
        pRT->FillSolidRect(&rcColor,m_crValue);
        pRT->DrawRectangle(&rcColor);
        CRect rcValue = rc;
        rcValue.left += KColorWidth;
        SStringT strValue = GetString();
        pRT->DrawText(strValue,strValue.GetLength(),&rcValue,DT_SINGLELINE|DT_VCENTER);
    }
    
    void SPropertyItemColor::OnInplaceActive(bool bActive)
    {
        SPropertyItemText::OnInplaceActive(bActive);
        if(bActive)
        {
            LRESULT lr=m_pEdit->SSendMessage(EM_SETEVENTMASK,0,ENM_CHANGE);
        }
    }

    bool SPropertyItemColor::OnReNotify(EventArgs *pEvt)
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
			m_pOwner->InvalidateRect(rcColor);
			}
        }
        return true;
    }
    

    void SPropertyItemColor::SetString( const SStringT & strValue )
    {

		COLORREF crTmp;
		crTmp = CR_INVALID;
		ParseValue(strValue,crTmp);
		if (m_crValue != crTmp)
		{
			m_crValue = crTmp;
			OnValueChanged();
		}
    }

	void SPropertyItemColor::SetStringOnly( const SStringT & strValue )
	{

		COLORREF crTmp;
		crTmp = CR_INVALID;
		ParseValue(strValue,crTmp);
		m_crValue = crTmp;
	}

    void SPropertyItemColor::OnButtonClick()
    {
		CColourPopup *pCrPopup = new CColourPopup(GetOwner()->GetContainer()->GetHostHwnd(),this);
		CPoint pt;
		GetCursorPos(&pt);
		pt.x += 10;
		pCrPopup->Create(pt,m_crValue,_T("默认"),_T("更多"));

    }


    bool SPropertyItemColor::ParseValue(const SStringT & strValue, COLORREF & value)
	{
		int r,g,b,a=255;
		int nSeg=0;
		SStringW strValueL = strValue;
		strValueL.MakeLower();
		if(strValueL.Left(1)==L"#")
		{
			nSeg = swscanf(strValueL,L"#%02x%02x%02x%02x",&r,&g,&b,&a);
		}else if(strValueL.Left(4).CompareNoCase(L"rgba")==0)
		{
			nSeg = swscanf(strValueL,L"rgba(%d,%d,%d,%d)",&r,&g,&b,&a);                
		}else if(strValueL.Left(3).CompareNoCase(L"rgb")==0)
		{
			nSeg = swscanf(strValueL,L"rgb(%d,%d,%d)",&r,&g,&b); 
		}
		if(nSeg!=3 && nSeg != 4)
		{
			value = CR_INVALID;
			return false;
		}else
		{
			value = RGBA(r,g,b,a);
			return true;
		}
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

	SMessageLoop * SPropertyItemColor::GetMsgLoop()
	{
		return GetOwner()->GetContainer()->GetMsgLoop();
	}

}
