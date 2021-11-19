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
        SPropCombobox(IPropertyItem *pOwner):m_pOwner(pOwner)
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
            //int nCurSel=GetCurSel();
            //m_pOwner->SetValue(&nCurSel);
			int nCurSel=GetCurSel();
			SStringT strText = GetLBText(nCurSel);

			m_pOwner->SetString(strText);
        }

    protected:
        CAutoRefPtr<IPropertyItem> m_pOwner;

    };
    
	static const WCHAR * kOptionStyle = L"optionStyle";

	LPCWSTR SPropertyItemOption::GetInplaceItemStyleName()
	{
		return kOptionStyle;
	}

    void SPropertyItemOption::DrawItem( IRenderTarget *pRT,CRect rc )
    {
		rc.left += 5;
        pRT->DrawText(m_strDisplay,m_strDisplay.GetLength(),rc,DT_SINGLELINE|DT_VCENTER);
    }
    
    void SPropertyItemOption::OnInplaceActive(bool bActive)
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
			inplaceStyle.append_attribute(L"dropHeght").set_value(m_nDropHeight);
			inplaceStyle.append_attribute(L"dropDown").set_value(1);

			m_pOwner->OnInplaceActiveWndCreate(this,m_pCombobox,inplaceStyle);
			m_pCombobox->GetEventSet()->subscribeEvent(EventCBSelChange::EventID,Subscriber(&SPropertyItemOption::OnCBSelChange,this));

			UINT i = 0;
			if (m_bCanEmpty)
			{
				m_pCombobox->InsertItem(0, _T(""), 0, 0);
				i++;
			}
			
			SPOSITION nPos = m_options.GetStartPosition();

			SStringT str;
			int nCurSel;

			while (nPos)
			{
				SMap<SStringT, SStringT>::CPair *p = m_options.GetNext(nPos);
			    m_pCombobox->InsertItem(i, p->m_key, 0, i);

				str = p->m_value;
				if (str.CompareNoCase(m_strValue) == 0)
				{
					nCurSel = i;
					m_strDisplay = p->m_key;
				}
				
				i++;
			}

			m_pCombobox->SetCurSel(nCurSel);

			m_pCombobox->SetFocus();/////
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

    SStringT SPropertyItemOption::GetString() const
    {
        //if(m_nValue<0 || m_nValue>=(int)m_options.GetCount()) return _T("");
        //return m_options[m_nValue];
		return m_strValue;
    }

    void SPropertyItemOption::SetString( const SStringT & strValue )
    {
		if (strValue.CompareNoCase(m_strValue) == 0)
		{
			return;
		}

		if (strValue.IsEmpty())
		{
			m_strValue.Empty();
			m_strDisplay.Empty();
			OnValueChanged();
			return;
		}

		SMap<SStringT, SStringT>::CPair *p = m_options.Lookup(strValue);
		if (p)
		{
			if (m_strValue.CompareNoCase(p->m_value) != 0)
			{
				m_strValue = p->m_value;
				m_strDisplay = p->m_key;
			    OnValueChanged();
			}
		}
    }

	void SPropertyItemOption::SetStringOnly( const SStringT & strValue )
	{
		SPOSITION nPos = m_options.GetStartPosition();
		SStringT str;
		while (nPos)
		{
			SMap<SStringT, SStringT>::CPair *p = m_options.GetNext(nPos);

			str = p->m_value;
			if (str.CompareNoCase(strValue) == 0)
			{
				m_strDisplay = p->m_key;
				break;
			}
		}

		m_strValue = strValue;
	}

    HRESULT SPropertyItemOption::OnAttrOptions( const SStringW & strValue,BOOL bLoading )
    {
        SStringT strValueT = S_CW2T(strValue);

		// 可见:1|不可见:0
		// top|buttom|left|右:right

		SStringTList strList;
        SplitString(strValueT,_T('|'),strList);
		for (int i = 0; i < strList.GetCount(); i++)
		{
		    SStringT strItem = strList[i];

			if (-1 == strItem.Find(':'))
			{
				m_options[strItem] = strItem;
			}else
			{
				SStringTList strList1;
				SplitString(strItem,_T(':'),strList1);
				if (strList1.GetCount() == 2)
				{
					m_options[strList1[0]] = strList1[1];
				}else
				{
					m_options[strList1[0]] = _T("");
				}
			}
		}
        return S_FALSE;
    }

	bool SPropertyItemOption::OnCBSelChange(EventArgs *pEvt)
	{
		int nCurSel=m_pCombobox->GetCurSel();
		SStringT strText = m_pCombobox->GetLBText(nCurSel);

		SetString(strText);
		return true;
	}

}
