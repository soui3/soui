#include "StdAfx.h"
#include "SPropertyItem-Text.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"

namespace SOUI
{
    class SPropEdit: public SEdit
                   , public IPropInplaceWnd
    {
    public:
        SPropEdit(IPropertyItem *pOwner):m_pOwner(pOwner)
        {
            SASSERT(m_pOwner);
        }
        
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
        {
            if(nChar==VK_RETURN)
            {
                GetParent()->SetFocus();
            }else
            {
                SEdit::OnKeyDown(nChar,nRepCnt,nFlags);            
            }
        }
        
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_KEYDOWN(OnKeyDown)
        SOUI_MSG_MAP_END()
        
        virtual IPropertyItem* GetOwner(){return m_pOwner;}
        
        virtual void UpdateData()
        {
            SStringT strValue=GetWindowText();
            m_pOwner->SetValue(strValue);
        }

    protected:
        SAutoRefPtr<IPropertyItem> m_pOwner;

    };
    
	SPropertyItemText::SPropertyItemText(SPropertyGrid *pOwner) :SPropertyItemBase(pOwner),m_pEdit(NULL),m_hasButton(FALSE)
	{
	}

	static const WCHAR * kEditStyle = L"editStyle";

	LPCWSTR SPropertyItemText::GetInplaceItemStyleName()
	{
		return kEditStyle;
	}

    void SPropertyItemText::DrawItem( IRenderTarget *pRT,CRect rc )
    {
        SStringT strValue = GetValue();
		rc.left += 5;
        pRT->DrawText(strValue,strValue.GetLength(),rc,DT_SINGLELINE|DT_VCENTER);
    }
    
    void SPropertyItemText::OnInplaceActive(BOOL bActive)
    {
        __super::OnInplaceActive(bActive);
        if(bActive)
        {
            SASSERT(!m_pEdit);
            m_pEdit = new TplPropEmbedWnd<SPropEdit>(this);
			pugi::xml_node inplaceStyle=m_pOwner->GetInplaceItemStyle(kEditStyle);			
            pugi::xml_document xmlDoc;
			if(!inplaceStyle)
			{
				inplaceStyle=xmlDoc.root();
				inplaceStyle.append_attribute(L"colorBkgnd").set_value(L"#FFFFFF");
				inplaceStyle.append_attribute(L"colorText").set_value(L"#000000");
				inplaceStyle.append_attribute(L"autoWordSel").set_value(L"1");
			}
            m_pOwner->OnInplaceActiveWndCreate(this,m_pEdit,inplaceStyle);
            m_pEdit->SetWindowText(GetValue());
			m_pEdit->SetFocus();
        }else
        {
            if(m_pEdit)
            {
                m_pOwner->OnInplaceActiveWndDestroy(this,m_pEdit);
                m_pEdit->Release();
                m_pEdit = NULL;
            }
        }
    }


    void SPropertyItemText::SetValue( const SStringT & strValue )
    {
		//如果值没有改变，就不发送通知
		if (m_strValue != strValue)
		{
			m_strValue = strValue;
			OnValueChanged();
		}
		
    }

    BOOL SPropertyItemText::HasButton() const 
	{
		return m_hasButton;
	}


	BOOL SPropertyItemText::HasValue() const
	{
		return !m_strValue.IsEmpty();
	}

	void SPropertyItemText::ClearValue()
	{
		m_strValue.Empty();
		OnValueChanged();
	}

}
