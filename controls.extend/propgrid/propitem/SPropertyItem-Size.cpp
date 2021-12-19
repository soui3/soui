#include "StdAfx.h"
#include "SPropertyItem-Size.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"

#include <helper/SplitString.h>

namespace SOUI
{
    SPropertyItemSize::SPropertyItemSize( SPropertyGrid *pOwner ) :SPropertyItemText(pOwner),m_bChildChanged(FALSE),m_bDigit(TRUE)
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
		if(m_bDigit)
		{
			SIZE sz;
			if(_stscanf(strValue,_T("%d,%d"),&sz.cx,&sz.cy)==2)
			{
				//如果值有变化，就发送通知
				if (sz.cy != m_szValue.cy || sz.cy != m_szValue.cy)
				{

					m_szValue = sz;
					m_strValue[0].Format(_T("%d"),sz.cx);
					m_strValue[1].Format(_T("%d"),sz.cy);
					OnValueChanged();
				}
			}
		}else
		{
			SStringTList lstValue;
			if(2==SplitString(strValue,_T(','),lstValue))
			{
				m_strValue[0]=lstValue[0];
				m_strValue[1]=lstValue[1];
				OnValueChanged();
			}
		}
    }

	SStringT SPropertyItemSize::GetValue() const
	{
		if(HasValue())
			return SStringT().Format(_T("%s,%s"),m_strValue[0],m_strValue[1]);
		else
			return SStringT();
	}


    void SPropertyItemSize::OnChildValueChanged( IPropertyItem *pChild )
    {
        if(pChild->GetID() == CHILD_WIDTH)
        {
			if(m_bDigit)
			{
				m_szValue.cx=_ttoi(pChild->GetValue());
				m_strValue[0].Format(_T("%d"),m_szValue.cx);
			}else
			{
				m_strValue[0]=pChild->GetValue();
			}
        }else if(pChild->GetID()==CHILD_HEIGHT)
        {
			if(m_bDigit)
			{
				m_szValue.cy=_ttoi(pChild->GetValue());
				m_strValue[0].Format(_T("%d"),m_szValue.cy);
			}else
			{
				m_strValue[1]=pChild->GetValue();
			}
        }
        m_bChildChanged=TRUE;
        OnValueChanged();
        m_bChildChanged=FALSE;
    }


    void SPropertyItemSize::OnValueChanged()
    {
        if(!m_bChildChanged)
        {
            IPropertyItem *pWid = GetChildById(CHILD_WIDTH);
            pWid->SetValue(m_strValue[0]);
            IPropertyItem *pHei = GetChildById(CHILD_HEIGHT);
            pHei->SetValue(m_strValue[1]);
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
		if(m_bDigit)
			return m_szValue.cx || m_szValue.cy;
		else
			return !(m_strValue[0].IsEmpty() && m_strValue[1].IsEmpty());
	}

	void SPropertyItemSize::ClearValue()
	{
		m_szValue.cx = m_szValue.cy =0;
		m_strValue[0]=m_strValue[1]=SStringT();
		OnValueChanged();
	}

}
