#pragma once

#include "../SPropertyItemBase.h"
#include <souicoll.h>

namespace SOUI
{
    class SPropertyItemOption : public SPropertyItemBase
    {
		friend class SPropertyGrid;
        SOUI_CLASS_NAME(SPropertyItemOption,L"propoption")
    public:
		static LPCWSTR GetInplaceItemStyleName();
        
        virtual void SetValue(const SStringT & strValue);

        virtual SStringT GetValue() const;

		void SetValue2(int nValue);

        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"options",OnAttrOptions)
			ATTR_CUSTOM(L"value",OnAttrValue)
            ATTR_INT(L"dropHeight",m_nDropHeight,FALSE)
        SOUI_ATTRS_END()

    protected:
		virtual void DrawItem(IRenderTarget *pRT,CRect rc);
        virtual void OnInplaceActive(BOOL bActive);
    protected:
        HRESULT OnAttrOptions(const SStringW & strValue,BOOL bLoading);
		HRESULT OnAttrValue(const SStringW & strValue,BOOL bLoading);


        int      m_nDropHeight;
        int      m_nValue;

        SArray<SStringT>    m_options;//value options   
		SMap<SStringT, SStringT> m_value2text;//value 2 text map.

        SComboBox  * m_pCombobox;
        
    public:
		bool OnCBSelChange(EventArgs *pEvt);
    protected:
        SPropertyItemOption(SPropertyGrid *pOwner):SPropertyItemBase(pOwner),m_pCombobox(NULL),m_nValue(-1),m_nDropHeight(200)
        {
        }
    };
}
