#include "StdAfx.h"
#include "SPropertyItem-Option.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"
#include <helper/SplitString.h>

namespace SOUI
{
    class SPropCombobox: public SComboBox
                   , public IPropInplaceWnd
    {
    public:
        SPropCombobox(SPropertyItemOption *pOwner):m_pOwner(pOwner)
        {
            SASSERT(m_pOwner);
        }
        
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
        {
            if(nChar == VK_RETURN)
            {
                GetParent()->SetFocus();
            }else
            {
                SComboBox::OnKeyDown(nChar,nRepCnt,nFlags); 
            }
        }
        
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_KEYDOWN(OnKeyDown)
        SOUI_MSG_MAP_END()
        
        virtual IPropertyItem* GetOwner(){return m_pOwner;}
        
        virtual void UpdateData()
        {
			int nCurSel=GetCurSel();
			m_pOwner->SetValue2(nCurSel);
        }

    protected:
        SAutoRefPtr<SPropertyItemOption> m_pOwner;

    };
    
	static const WCHAR * kOptionStyle = L"optionStyle";

	LPCWSTR SPropertyItemOption::GetInplaceItemStyleName()
	{
		return kOptionStyle;
	}

    void SPropertyItemOption::DrawItem( IRenderTarget *pRT,CRect rc )
    {
		rc.left += 5;
		SStringT strValue = GetValue();
        pRT->DrawText(strValue,strValue.GetLength(),rc,DT_SINGLELINE|DT_VCENTER);
    }
    
    void SPropertyItemOption::OnInplaceActive(BOOL bActive)
    {
        __super::OnInplaceActive(bActive);
        if(bActive)
        {
            SASSERT(!m_pCombobox);
            m_pCombobox = new TplPropEmbedWnd<SPropCombobox>(this);
			pugi::xml_node inplaceStyle = m_pOwner->GetInplaceItemStyle(kOptionStyle);
			pugi::xml_document xmlDoc;
			if(!inplaceStyle)
			{
				wchar_t szXml[]=L"<combobox dropDown=\"1\" colorBkgnd=\"#ffffff\" drawFocusRect=\"0\">\
					<liststyle colorBorder=\"#000000\" margin-x=\"1\" margin-y=\"1\" colorText=\"#000000\" colorSelText=\"#FFFFFF\" colorItemBkgnd=\"#FFFFFF\" colorItemSelBkgnd=\"#000088\"/>\
					</combobox>";
				xmlDoc.load_buffer(szXml,sizeof(szXml));
				inplaceStyle=xmlDoc.first_child();
			}
			m_pOwner->OnInplaceActiveWndCreate(this,m_pCombobox,inplaceStyle);
			m_pCombobox->SetAttribute(L"dropHeight",SStringW().Format(L"%d",m_nDropHeight),TRUE);
			m_pCombobox->SetAttribute(L"dropDown",L"1",TRUE);

			m_pCombobox->GetEventSet()->subscribeEvent(EventCBSelChange::EventID,Subscriber(&SPropertyItemOption::OnCBSelChange,this));

			for(UINT i=0;i<m_options.GetCount();i++)
			{
				m_pCombobox->InsertItem(i, m_options[i], 0, i);
			}
			m_pCombobox->SetCurSel(m_nValue);

			m_pCombobox->SetFocus();
        }else
        {
            if(m_pCombobox)
            {
                m_pOwner->OnInplaceActiveWndDestroy(this,m_pCombobox);
                m_pCombobox->Release();
                m_pCombobox = NULL;
            }
        }
    }

    SStringT SPropertyItemOption::GetValue() const
    {
        if(m_nValue<0 || m_nValue>=(int)m_options.GetCount()) return _T("");
        return m_options[m_nValue];
    }

    void SPropertyItemOption::SetValue( const SStringT & strValue )
    {
		int nValue = -1;
		SPOSITION pos = m_value2text.GetStartPosition();
		SMap<SStringT,SStringT>::CPair * p = m_value2text.Lookup(strValue);
		if(p){
			for(UINT i=0;i<m_options.GetCount();i++)
			{
				if(m_options[i]==p->m_value)
				{
					nValue = i;
					break;
				}
			}
		}
		SetValue2(nValue);
    }


    HRESULT SPropertyItemOption::OnAttrOptions( const SStringW & strValue,BOOL bLoading )
    {
        SStringT strValueT = S_CW2T(strValue);

		// format: text1:value1|text2:value2
		// etc. 可见:1|不可见:0

		SStringTList strList;
        SplitString(strValueT,_T('|'),strList);
		for (UINT i = 0; i < strList.GetCount(); i++)
		{
		    SStringT strItem = strList[i];
			SStringTList lstTextValue;
			SplitString(strItem,_T(':'),lstTextValue);
			SStringT strText = lstTextValue[0];
			SStringT strValue = lstTextValue.GetCount()>1? lstTextValue[1] : strText;
			if(!m_value2text.Lookup(strValue))
			{
				m_options.Add(strText);
				m_value2text[strValue]=strText;
			}
		}
        return S_FALSE;
    }

	bool SPropertyItemOption::OnCBSelChange(EventArgs *pEvt)
	{
		m_nValue=m_pCombobox->GetCurSel();
		OnValueChanged();
		return true;
	}

	void SPropertyItemOption::SetValue2(int nValue)
	{
		if(m_nValue != nValue)
		{
			if(nValue>=-1 && nValue<(int)m_options.GetCount())
			{
				m_nValue = nValue;
				OnValueChanged();
			}
		}
	}

	HRESULT SPropertyItemOption::OnAttrValue(const SStringW & strValue,BOOL bLoading)
	{
		int nValue = _wtoi(strValue);
		if(!bLoading)
		{
			SetValue2(nValue);
			return S_OK;
		}else
		{
			m_nValue = nValue;
			if(m_nValue<0) m_nValue = -1;
			return S_FALSE;
		}
	}

}
