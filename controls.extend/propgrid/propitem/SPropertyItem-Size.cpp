#include "StdAfx.h"
#include "SPropertyItem-Size.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"

#include <helper/SplitString.h>

#define CHILD_WIDTH     1
#define CHILD_HEIGHT    2

namespace SOUI
{
    SPropertyItemSize::SPropertyItemSize( SPropertyGrid *pOwner ) :SPropertyItemText(pOwner),m_bChildChanged(FALSE)
    {
		IPropertyItem *pWidth = pOwner->CreateItem(SPropertyItemText::GetClassName());
        pWidth->SetID(CHILD_WIDTH);
        pWidth->SetTitle(TR(L"width",GetOwner()->GetContainer()->GetTranslatorContext()));
        InsertChild(pWidth);
        pWidth->Release();
        IPropertyItem *pHeight = pOwner->CreateItem(SPropertyItemText::GetClassName());
        pHeight->SetID(CHILD_HEIGHT);
        pHeight->SetTitle(TR(L"height",GetOwner()->GetContainer()->GetTranslatorContext()));
        InsertChild(pHeight);
        pHeight->Release();
        m_szValue.cx=m_szValue.cy=0;
    }


    void SPropertyItemSize::SetValue( const SStringT & strValue )
    {
        SIZE sz;
        if(_stscanf(strValue,_T("%d,%d"),&sz.cx,&sz.cy)==2)
        {
			//如果值有变化，就发送通知
			if (sz.cy != m_szValue.cy || sz.cy != m_szValue.cy)
			{

				m_szValue = sz;
				OnValueChanged();
			}
        }
    }

	SStringT SPropertyItemSize::GetValue() const
	{
		return SStringT().Format(_T("%d,%d"),m_szValue.cx,m_szValue.cy);
	}


    void SPropertyItemSize::OnChildValueChanged( IPropertyItem *pChild )
    {
        if(pChild->GetID() == CHILD_WIDTH)
        {
            m_szValue.cx=_ttoi(pChild->GetValue());
        }else if(pChild->GetID()==CHILD_HEIGHT)
        {
            m_szValue.cy=_ttoi(pChild->GetValue());
        }
        m_bChildChanged=TRUE;
        OnValueChanged();
        m_bChildChanged=FALSE;
    }


    void SPropertyItemSize::OnValueChanged()
    {
        if(!m_bChildChanged)
        {
            IPropertyItem *pWid = GetItem(IPropertyItem::GPI_FIRSTCHILD);
            SASSERT(pWid && pWid->GetID()==CHILD_WIDTH);
            SStringT str;
            str.Format(_T("%d"),m_szValue.cx);
            pWid->SetValue(str);
            IPropertyItem *pHei = GetItem(IPropertyItem::GPI_LASTCHILD);
            SASSERT(pHei && pHei->GetID()==CHILD_HEIGHT);
            str.Format(_T("%d"),m_szValue.cy);
            pHei->SetValue(str);
        }
        __super::OnValueChanged();
    }

    HRESULT SPropertyItemSize::OnAttrValue( const SStringW & strValue,BOOL bLoading )
    {
        SetValue(S_CW2T(strValue));
        return S_OK;
    }

    HRESULT SPropertyItemSize::OnAttrChildrenTitles( const SStringW & strValue,BOOL bLoading )
    {
        SArray<SStringW> strNames;
        SplitString(TR(strValue,GetOwner()->GetContainer()->GetTranslatorContext()),L'|',strNames);
        if(strNames.GetCount()==2)
        {
            GetItem(GPI_FIRSTCHILD)->SetTitle(S_CW2T(strNames[0]));
            GetItem(GPI_LASTCHILD)->SetTitle(S_CW2T(strNames[1]));
        }
        return S_OK;
    }

	BOOL SPropertyItemSize::HasValue() const
	{
		return m_szValue.cx || m_szValue.cy;
	}

	void SPropertyItemSize::ClearValue()
	{
		m_szValue.cx = m_szValue.cy =0;
		OnValueChanged();
	}

}
