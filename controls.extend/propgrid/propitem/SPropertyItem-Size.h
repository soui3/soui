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

        virtual void SetString(const SStringT & strValue);
		//add
		virtual void SetStringOnly( const SStringT & strValue );
        
        virtual SStringT GetString() const {
            SStringT strValue;
            strValue.Format(_T("%d,%d"),m_szValue.cx,m_szValue.cy);
            return strValue;
        }
        virtual void OnValueChanged();
        virtual void OnChildValueChanged(IPropertyItem *pChild);
        
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
