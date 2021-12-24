#pragma once
#include "interface/slayout-i.h"
#include "SouiLayoutParamStruct.h"

namespace SOUI{



	class SouiLayoutParam: public TObjRefImpl<SObjectImpl<ILayoutParam>>
						 , protected SouiLayoutParamStruct
	{
		SOUI_CLASS_NAME(SouiLayoutParam,L"SouiLayoutParam")

		friend class SouiLayout;
	public:
		SouiLayoutParam();

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
		bool IsOffsetRequired(ORIENTATION orientation) const;
        int  GetExtraSize(ORIENTATION orientation,int nScale) const;
	protected:
		HRESULT OnAttrWidth(const SStringW & strValue,BOOL bLoading);

		HRESULT OnAttrHeight(const SStringW & strValue,BOOL bLoading);

		HRESULT OnAttrSize(const SStringW & strValue,BOOL bLoading);

		HRESULT OnAttrPos(const SStringW & strValue,BOOL bLoading);

		HRESULT OnAttrOffset(const SStringW & strValue,BOOL bLoading);

		SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"width",OnAttrWidth)
            ATTR_CUSTOM(L"height",OnAttrHeight)
			ATTR_CUSTOM(L"pos",OnAttrPos)
			ATTR_CUSTOM(L"size",OnAttrSize)
			ATTR_CUSTOM(L"offset",OnAttrOffset)
        SOUI_ATTRS_BREAK()

    protected:
        //将字符串描述的坐标转换成POSITION_ITEM
        BOOL StrPos2ItemPos(const SStringW &strPos,POS_INFO & posItem);

        //解析在pos中定义的前两个位置
        BOOL ParsePosition12(const SStringW & pos1, const SStringW &pos2);

        //解析在pos中定义的后两个位置
        BOOL ParsePosition34(const SStringW & pos3, const SStringW &pos4);


	};

	class SOUI_EXP SouiLayout: public TObjRefImpl<SObjectImpl<ILayout>>
	{
		SOUI_CLASS_NAME_EX(SouiLayout,L"SouiLayout",Layout)
	public:
		SouiLayout(void);
		~SouiLayout(void);

		STDMETHOD_(BOOL, IsParamAcceptable)(THIS_ const ILayoutParam *pLayoutParam) SCONST OVERRIDE;
		STDMETHOD_(void,LayoutChildren)(THIS_ IWindow * pParent) OVERRIDE;
		STDMETHOD_(ILayoutParam *, CreateLayoutParam)(THIS) SCONST OVERRIDE;
		STDMETHOD_(SIZE, MeasureChildren)(THIS_ const IWindow * pParent,int nWidth,int nHeight) SCONST OVERRIDE;
    protected:
        struct WndPos{
            IWindow *pWnd;
            CRect    rc;
			bool     bWaitOffsetX;
			bool	 bWaitOffsetY;
        };

        void CalcPositionEx(SList<WndPos> *pListChildren,int nWidth,int nHeight) const;
        int CalcPostion(SList<WndPos> *pListChildren,int nWidth,int nHeight) const;

		int PositionItem2Value(SList<WndPos> *pLstChilds,SPOSITION position,const POS_INFO &pos , int nMax,BOOL bX,int nScale) const;
        
        int CalcChildLeft(SWindow *pWindow,SouiLayoutParam *pParam);
        int CalcChildRight(SWindow *pWindow,SouiLayoutParam *pParam);
        int CalcChildTop(SWindow *pWindow,SouiLayoutParam *pParam);
        int CalcChildBottom(SWindow *pWindow,SouiLayoutParam *pParam);


        BOOL IsWaitingPos( int nPos ) const;
		SWindow * GetRefSibling(SWindow *pCurWnd,int uCode);
        CRect GetWindowLayoutRect(SWindow *pWindow);
    };


}
