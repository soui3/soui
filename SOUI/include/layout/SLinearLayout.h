#pragma once

#include "interface/slayout-i.h"
#include "SLinearLayoutParamStruct.h"

namespace SOUI
{

	class SLinearLayoutParam : public SObjectImpl<TObjRefImpl<ILayoutParam>>
							 , protected SLinearLayoutParamStruct
    {
        SOUI_CLASS_NAME(SLinearLayoutParam,L"LinearLayoutParam")

		friend class SLinearLayout;
    public:
		SLinearLayoutParam();

		STDMETHOD_(void,Clear)(THIS) OVERRIDE;
		STDMETHOD_(BOOL, IsMatchParent)(THIS_ ORIENTATION orientation) SCONST OVERRIDE;
		STDMETHOD_(BOOL, IsWrapContent)(THIS_ ORIENTATION orientation) SCONST OVERRIDE;
		STDMETHOD_(BOOL, IsSpecifiedSize)(THIS_ ORIENTATION orientation) SCONST OVERRIDE;
		STDMETHOD_(SLayoutSize, GetSpecifiedSize)(THIS_ ORIENTATION orientation) SCONST OVERRIDE;
		STDMETHOD_(void,SetMatchParent)(THIS_ ORIENTATION orientation) OVERRIDE;
		STDMETHOD_(void,SetWrapContent)(THIS_ ORIENTATION orientation) OVERRIDE;
		STDMETHOD_(void,SetSpecifiedSize)(THIS_ ORIENTATION orientation, const SLayoutSize& layoutSize) OVERRIDE;
		STDMETHOD_(void*, GetRawData)(THIS) OVERRIDE;
		STDMETHOD_(ILayoutParam *, Clone)(THIS) SCONST OVERRIDE;

	public:
        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"width",OnAttrWidth)
            ATTR_CUSTOM(L"height",OnAttrHeight)
            ATTR_CUSTOM(L"size",OnAttrSize)
            ATTR_FLOAT(L"weight",weight,FALSE)
            ATTR_ENUM_BEGIN(L"layout_gravity",Gravity,FALSE)
                ATTR_ENUM_VALUE(L"left",G_Left)
                ATTR_ENUM_VALUE(L"top",G_Top)
                ATTR_ENUM_VALUE(L"center",G_Center)
                ATTR_ENUM_VALUE(L"right",G_Right)
                ATTR_ENUM_VALUE(L"bottom",G_Bottom)
            ATTR_ENUM_END(gravity)
			ATTR_CUSTOM(L"extend",OnAttrExtend)
			ATTR_LAYOUTSIZE(L"extend_left",extend_left,FALSE)
			ATTR_LAYOUTSIZE(L"extend_top",extend_top,FALSE)
			ATTR_LAYOUTSIZE(L"extend_right",extend_right,FALSE)
			ATTR_LAYOUTSIZE(L"extend_bottom",extend_bottom,FALSE)
        SOUI_ATTRS_BREAK()


	protected:
        HRESULT OnAttrSize(const SStringW & strValue,BOOL bLoading);
		HRESULT OnAttrWidth(const SStringW & strValue,BOOL bLoading);
		HRESULT OnAttrHeight(const SStringW & strValue,BOOL bLoading);
		HRESULT OnAttrExtend(const SStringW & strValue,BOOL bLoading);

    };

    class SOUI_EXP SLinearLayout : public SObjectImpl<TObjRefImpl<ILayout>>
    {
		SOUI_CLASS_NAME_EX(SLinearLayout,L"linearLayout",Layout)
    public:
        SLinearLayout(void);
        ~SLinearLayout(void);

		STDMETHOD_(BOOL, IsParamAcceptable)(THIS_ ILayoutParam *pLayoutParam) SCONST OVERRIDE;
		STDMETHOD_(void,LayoutChildren)(THIS_ SWindow * pParent) OVERRIDE;
		STDMETHOD_(ILayoutParam *, CreateLayoutParam)(THIS) SCONST OVERRIDE;
		STDMETHOD_(SIZE, MeasureChildren)(THIS_ const SWindow * pParent,int nWidth,int nHeight) SCONST OVERRIDE;

        
        SOUI_ATTRS_BEGIN()
            ATTR_ENUM_BEGIN(L"orientation",ORIENTATION,FALSE)
                ATTR_ENUM_VALUE(L"horizontal",Horz)
                ATTR_ENUM_VALUE(L"vertical",Vert)
            ATTR_ENUM_END(m_orientation)
            ATTR_ENUM_BEGIN(L"gravity",Gravity,FALSE)
                ATTR_ENUM_VALUE(L"left",G_Left)
                ATTR_ENUM_VALUE(L"top",G_Top)
                ATTR_ENUM_VALUE(L"center",G_Center)
                ATTR_ENUM_VALUE(L"right",G_Right)
                ATTR_ENUM_VALUE(L"bottom",G_Bottom)
            ATTR_ENUM_END(m_gravity)
			ATTR_LAYOUTSIZE(L"interval",m_interval,FALSE)
        SOUI_ATTRS_BREAK()


	protected:
		ORIENTATION m_orientation;
        Gravity     m_gravity;
		SLayoutSize m_interval;
    };

	class SVBox : public SLinearLayout
	{
		SOUI_CLASS_NAME(SVBox,L"vbox")

	public:
		SVBox(){m_orientation = Vert;}
	};

	class SHBox : public SLinearLayout
	{
		SOUI_CLASS_NAME(SHBox,L"hbox")

	public:
		SHBox(){m_orientation = Horz;}
	};
}

