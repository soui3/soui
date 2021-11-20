#pragma once

#include "../SPropertyItemBase.h"

namespace SOUI
{
    class SPropertyItemText : public SPropertyItemBase
    {
		friend class SPropertyGrid;
        SOUI_CLASS_NAME(SPropertyItemText,L"proptext")
    public:
		static LPCWSTR GetInplaceItemStyleName();
        virtual void DrawItem(IRenderTarget *pRT,CRect rc);
        
        virtual void SetValue(const SStringT & strValue);
        virtual SStringT GetValue() const {return m_strValue;}
		virtual BOOL HasButton() const;
       
        SOUI_ATTRS_BEGIN()
            ATTR_STRINGT(L"value",m_strValue,TRUE)
			ATTR_BOOL(L"hasButton",m_hasButton,TRUE)
        SOUI_ATTRS_END()

    protected:
        virtual void OnInplaceActive(BOOL bActive);
    protected:
        SStringT m_strValue;
        BOOL	m_hasButton;
        SEdit  * m_pEdit;
    protected:
        SPropertyItemText(SPropertyGrid *pOwner):SPropertyItemBase(pOwner),m_pEdit(NULL),m_hasButton(FALSE)
        {
        }
    };
}
