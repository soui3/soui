#pragma once

#include "SPropertyItem-Text.h"

namespace SOUI
{
    class SPropertyItemSize : public SPropertyItemText
    {
		friend class SPropertyGrid;
        SOUI_CLASS_NAME(SPropertyItemSize,L"propsize")
    public:
		static LPCWSTR GetInplaceItemStyleName(){return NULL;}

        virtual void SetValue(const SStringT & strValue);
        
        virtual SStringT GetValue() const;
        virtual void OnValueChanged();
        virtual void OnChildValueChanged(IPropertyItem *pChild);
        
		SIZE GetValue2() const{return m_szValue;}

        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"value",OnAttrValue)
            ATTR_CUSTOM(L"childrenTitles",OnAttrChildrenTitles)
        SOUI_ATTRS_END()

    protected:
        HRESULT OnAttrValue(const SStringW & strValue,BOOL bLoading);
        HRESULT OnAttrChildrenTitles(const SStringW & strValue,BOOL bLoading);
        
        SIZE    m_szValue;
        
        BOOL    m_bChildChanged;
    protected:
        SPropertyItemSize(SPropertyGrid *pOwner);
    };
}
