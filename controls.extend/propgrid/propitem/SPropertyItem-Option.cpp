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
			SStringT strValue;
			if(m_bDropdown)
			{
				int nCurSel=GetCurSel();
				if(nCurSel != -1)
				{
					strValue = GetLBText(nCurSel);
				}
			}else
			{
				strValue = m_pEdit->GetWindowText();
			}
			m_pOwner->SetValue(strValue);
        }

    protected:
        SAutoRefPtr<SPropertyItemOption> m_pOwner;

    };
    

	SPropertyItemOption::SPropertyItemOption(SPropertyGrid *pOwner) :SPropertyItemBase(pOwner),m_pCombobox(NULL),m_nDropHeight(200),m_bDropDown(TRUE)
	{

	}

	static const WCHAR * kOptionStyle = L"optionStyle";
	static const WCHAR * kEditStyle = L"editStyle";

	LPCWSTR SPropertyItemOption::GetInplaceItemStyleName()
	{
		return kOptionStyle;
	}

    void SPropertyItemOption::DrawItem( IRenderTarget *pRT,CRect rc )
    {
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
			SXmlNode inplaceStyle = m_pOwner->GetInplaceItemStyle(kOptionStyle);
			SXmlDoc xmlDoc;
			if(!inplaceStyle)
			{
				wchar_t szXml[]=L"<combobox dropDown=\"1\" colorBkgnd=\"#ffffff\" drawFocusRect=\"0\">\
					<liststyle colorBorder=\"#000000\" margin-x=\"1\" margin-y=\"1\" colorText=\"#000000\" colorSelText=\"#FFFFFF\" colorItemBkgnd=\"#FFFFFF\" colorItemSelBkgnd=\"#000088\"/>\
					</combobox>";
				xmlDoc.load_buffer(szXml,sizeof(szXml));
			}else
			{
				xmlDoc.root().append_copy(inplaceStyle);
			}
			inplaceStyle=xmlDoc.root().first_child();
			if(!inplaceStyle.child(kEditStyle))
			{
				SXmlNode editStyle = m_pOwner->GetInplaceItemStyle(kEditStyle);
				inplaceStyle.append_copy(editStyle);
			}
			m_pOwner->OnInplaceActiveWndCreate(this,m_pCombobox,inplaceStyle);
			m_pCombobox->SetAttribute(L"dropHeight",SStringW().Format(L"%d",m_nDropHeight),TRUE);
			m_pCombobox->SetDropdown(m_bDropDown);

			m_pCombobox->GetEventSet()->subscribeEvent(EventCBSelChange::EventID,Subscriber(&SPropertyItemOption::OnCBSelChange,this));

			for(UINT i=0;i<m_options.GetCount();i++)
			{
				m_pCombobox->InsertItem(i, m_options[i], 0, i);
			}
			if(!m_strValue.IsEmpty())
			{
				SStringT strOption = Value2Option(m_strValue);
				int iFind = m_pCombobox->FindString(strOption);
				m_pCombobox->SetCurSel(iFind);
				if(iFind == -1)
				{
					m_pCombobox->SetWindowText(strOption);
				}
			}
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
        return m_strValue;
    }

    void SPropertyItemOption::SetValue( const SStringT & strValue )
    {
		m_strValue = strValue;
		OnValueChanged();
    }


    HRESULT SPropertyItemOption::OnAttrOptions( const SStringW & strValue,BOOL bLoading )
    {
        SStringT strValueT = S_CW2T(strValue);

		// format: option1:value1|option2:value2
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
			if(!m_option2value.Lookup(strText))
			{
				m_options.Add(strText);
				m_option2value[strText]=strValue;
			}
		}
        return S_FALSE;
    }

	bool SPropertyItemOption::OnCBSelChange(IEvtArgs *pEvt)
	{
		int iSel = m_pCombobox->GetCurSel();
		if(iSel != -1)
		{
			SetValue(m_pCombobox->GetLBText(iSel));
		}
		return true;
	}


	HRESULT SPropertyItemOption::OnAttrValue(const SStringW & strValue,BOOL bLoading)
	{
		m_strValue = S_CW2T(strValue);
		return bLoading?S_FALSE:S_OK;
	}

	BOOL SPropertyItemOption::HasValue() const
	{
		return !m_strValue.IsEmpty();
	}

	void SPropertyItemOption::ClearValue()
	{
		m_strValue.Empty();
		OnValueChanged();
	}

	SStringT SPropertyItemOption::Option2Value(const SStringT& option) const
	{
		const SMap<SStringT, SStringT>::CPair* p = m_option2value.Lookup(option);
		if (!p) return option;
		return p->m_value;
	}

	SOUI::SStringT SPropertyItemOption::Value2Option(const SStringT& value) const
	{
		SPOSITION pos = m_option2value.GetStartPosition();
		while(pos)
		{
			SStringT k,v;
			m_option2value.GetNextAssoc(pos,k,v);
			if(v==value)
				return k;
		}
		return value;
	}

}
