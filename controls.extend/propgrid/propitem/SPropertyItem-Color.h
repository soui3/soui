#pragma once

#include "SPropertyItem-Text.h"
#include "../colorpicker/SColorPicker.h"
#include "../colorpicker/ColourPopup.h"

namespace SOUI
{
    class SPropertyItemColor : public SPropertyItemText,public IColorPicker
    {
		friend class SPropertyGrid;
        friend class SPropColorEdit;
        SOUI_CLASS_NAME(SPropertyItemColor,L"propcolor")
    public:
		static LPCWSTR GetInplaceItemStyleName(){return NULL;}

        virtual BOOL HasButton() const {return TRUE;}
        virtual void DrawItem(IRenderTarget *pRT,CRect rc);
        virtual void AdjustInplaceActiveWndRect(CRect & rc);
        
        virtual void SetString(const SStringT & strValue);


		//add
		virtual void SetStringOnly(const SStringT & strValue);
		bool ParseValue(const SStringT & strValue, COLORREF & value);

        
        virtual SStringT GetString() const {
			if (m_crValue == CR_INVALID)
			{
				return _T("");
			}

            SStringT str;
            int r,g,b,a;
            r = GetRValue(m_crValue);
            g = GetGValue(m_crValue);
            b = GetBValue(m_crValue);
            a = GetAValue(m_crValue);
            str.Format(m_strFormat,r,g,b,a);
            return str;
        }
        
        SOUI_ATTRS_BEGIN()
            ATTR_STRINGT(L"format",m_strFormat,TRUE)
            ATTR_COLOR(L"value",m_crValue,TRUE)
        SOUI_ATTRS_END()

    protected:
        virtual void OnInplaceActive(bool bActive);
        virtual void OnButtonClick();
    protected:
        SStringT m_strFormat;
        COLORREF    m_crValue;
        
    protected:
        bool OnReNotify(EventArgs *pEvt);

        SPropertyItemColor(SPropertyGrid *pOwner):SPropertyItemText(pOwner)
        {
            //m_strFormat = _T("RGBA(%d,%d,%d,%d)");
			m_strFormat = _T("#%02x%02x%02x%02x");
        }

	protected://IColorPicker
		virtual void OnColorChanged(COLORREF cr);
		virtual void OnColorEnd(BOOL bCancel,COLORREF cr);
		virtual SMessageLoop * GetMsgLoop();
    };
}
