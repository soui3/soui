#pragma once

#include "SPropertyItem-Text.h"

namespace SOUI
{
    class SPropertyItemRect : public SPropertyItemText
    {
		friend class SPropertyGrid;
        SOUI_CLASS_NAME(SPropertyItemText,L"proprect")

			enum{
				CHILD_LEFT=1,
				CHILD_TOP,
				CHILD_RIGHT,
				CHILD_BOTTOM,
		};
    public:
		static LPCWSTR GetInplaceItemStyleName(){return NULL;}

		virtual PropType GetType() const {return PT_SIZE;}
        virtual void SetValue(const SStringT & strValue);
        
        virtual SStringT GetValue() const;
        virtual void OnValueChanged();
        virtual void OnChildValueChanged(IPropertyItem *pChild);
        
		RECT GetValue2() const{return m_rcValue;}
		virtual BOOL HasValue() const ;
		virtual void ClearValue() ;

        SOUI_ATTRS_BEGIN()
			ATTR_BOOL(L"digit",m_bDigit,FALSE)
            ATTR_CUSTOM(L"value",OnAttrValue)
            ATTR_CUSTOM(L"childrenTitles",OnAttrChildrenTitles)
        SOUI_ATTRS_END()

    protected:
        HRESULT OnAttrValue(const SStringW & strValue,BOOL bLoading);
        HRESULT OnAttrChildrenTitles(const SStringW & strValue,BOOL bLoading);
        
		BOOL	 m_bDigit;
        CRect    m_rcValue;
        SStringT m_strValue[4];
        BOOL    m_bChildChanged;
    protected:
        SPropertyItemRect(SPropertyGrid *pOwner);
    };
}
