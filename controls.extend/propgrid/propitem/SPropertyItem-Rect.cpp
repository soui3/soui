#include "StdAfx.h"
#include "SPropertyItem-Rect.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"

#include <helper/SplitString.h>

namespace SOUI
{
    SPropertyItemRect::SPropertyItemRect( SPropertyGrid *pOwner ) :SPropertyItemText(pOwner),m_bChildChanged(FALSE),m_bDigit(TRUE)
    {
		IPropertyItem *pLeft = pOwner->CreateItem(SPropertyItemText::GetClassName());
        pLeft->SetID(CHILD_LEFT);
        pLeft->SetTitle(S_CW2T(TR(L"left",GetOwner()->GetContainer()->GetTranslatorContext())));
        InsertChild(pLeft);
        pLeft->Release();

        IPropertyItem *pTop = pOwner->CreateItem(SPropertyItemText::GetClassName());
        pTop->SetID(CHILD_TOP);
        pTop->SetTitle(S_CW2T(TR(L"top",GetOwner()->GetContainer()->GetTranslatorContext())));
        InsertChild(pTop);
        pTop->Release();

		IPropertyItem *pRight = pOwner->CreateItem(SPropertyItemText::GetClassName());
		pRight->SetID(CHILD_RIGHT);
		pRight->SetTitle(S_CW2T(TR(L"right",GetOwner()->GetContainer()->GetTranslatorContext())));
		InsertChild(pRight);
		pRight->Release();

		IPropertyItem *pBottom = pOwner->CreateItem(SPropertyItemText::GetClassName());
		pBottom->SetID(CHILD_BOTTOM);
		pBottom->SetTitle(S_CW2T(TR(L"bottom",GetOwner()->GetContainer()->GetTranslatorContext())));
		InsertChild(pBottom);
		pBottom->Release();
    }


    void SPropertyItemRect::SetValue( const SStringT & strValue )
    {
		if(m_bDigit)
		{
        CRect rc;
        if(_stscanf(strValue,_T("%d,%d,%d,%d"),&rc.left,&rc.top,&rc.right,&rc.bottom)==4)
        {
			//如果值有变化，就发送通知
			if (rc != m_rcValue)
			{

				m_rcValue = rc;
				m_strValue[0].Format(_T("%d"),rc.left);
				m_strValue[1].Format(_T("%d"),rc.top);
				m_strValue[2].Format(_T("%d"),rc.right);
				m_strValue[3].Format(_T("%d"),rc.bottom);
				OnValueChanged();
			}
        }
		}else
		{
			SStringTList lstValue;
			if(4==SplitString(strValue,_T(','),lstValue))
			{
				m_strValue[0]=lstValue[0];
				m_strValue[1]=lstValue[1];
				m_strValue[2]=lstValue[2];
				m_strValue[3]=lstValue[3];
			}
		}
    }

	SStringT SPropertyItemRect::GetValue() const
	{
		if(HasValue())
			return SStringT().Format(_T("%s,%s,%s,%s"),m_strValue[0],m_strValue[1],m_strValue[2],m_strValue[3]);
		else
			return SStringT();
	}


    void SPropertyItemRect::OnChildValueChanged( IPropertyItem *pChild )
    {
		SStringT strValue = pChild->GetValue();
		if(m_bDigit)
		{
			int nValue = _ttoi(strValue);
			switch(pChild->GetID())
			{
			case CHILD_LEFT:m_rcValue.left=nValue;break;
			case CHILD_TOP:m_rcValue.top=nValue;break;
			case CHILD_RIGHT:m_rcValue.right=nValue;break;
			case CHILD_BOTTOM:m_rcValue.bottom=nValue;break;
			}
			m_strValue[pChild->GetID()-1].Format(_T("%d"),nValue);
		}else
		{
			m_strValue[pChild->GetID()-1]=strValue;
		}

        m_bChildChanged=TRUE;
        OnValueChanged();
        m_bChildChanged=FALSE;
    }


    void SPropertyItemRect::OnValueChanged()
    {
        if(!m_bChildChanged)
        {
            IPropertyItem *pLeft = GetChildById(CHILD_LEFT);
            pLeft->SetValue(m_strValue[0]);

            IPropertyItem *pTop = GetChildById(CHILD_TOP);
			pTop->SetValue(m_strValue[1]);

			IPropertyItem *pRight = GetChildById(CHILD_RIGHT);
			pRight->SetValue(m_strValue[2]);

			IPropertyItem *pBottom = GetChildById(CHILD_BOTTOM);
			pBottom->SetValue(m_strValue[3]);

		}
        __super::OnValueChanged();
    }

    HRESULT SPropertyItemRect::OnAttrValue( const SStringW & strValue,BOOL bLoading )
    {
        SetValue(S_CW2T(strValue));
        return S_OK;
    }

    HRESULT SPropertyItemRect::OnAttrChildrenTitles( const SStringW & strValue,BOOL bLoading )
    {
        SArray<SStringW> strNames;
        SplitString(TR(strValue,GetOwner()->GetContainer()->GetTranslatorContext()),L'|',strNames);
        if(strNames.GetCount()==4)
        {
            GetChildById(CHILD_LEFT)->SetTitle(S_CW2T(strNames[0]));
			GetChildById(CHILD_TOP)->SetTitle(S_CW2T(strNames[1]));
			GetChildById(CHILD_RIGHT)->SetTitle(S_CW2T(strNames[2]));
			GetChildById(CHILD_BOTTOM)->SetTitle(S_CW2T(strNames[3]));
        }
        return S_OK;
    }

	BOOL SPropertyItemRect::HasValue() const
	{
		if(m_bDigit)
			return !m_rcValue.IsRectNull();
		else
			return !(m_strValue[0].IsEmpty()&&m_strValue[1].IsEmpty()&&m_strValue[2].IsEmpty()&&m_strValue[3].IsEmpty());
	}

	void SPropertyItemRect::ClearValue()
	{
		m_rcValue = CRect();
		m_strValue[0]=m_strValue[1]=m_strValue[2]=m_strValue[3]=SStringT();
		OnValueChanged();
	}

}
