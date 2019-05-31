#pragma once

#include <interface/Sskinobj-i.h>
#include <unknown/obj-ref-impl.hpp>
#include <souicoll.h>
#include <core/Swnd.h>
namespace SOUI
{
	//////////////////////////////////////////////////////////////////////////
	class SOUI_EXP SState2Index{
	public:
		SState2Index() {};
		SState2Index(const SState2Index &src);

		int GetIndex(DWORD dwState) const;

		static int GetIndex2(DWORD dwState);

		BOOL Init(pugi::xml_node xmlNode);
	protected:
		SMap<DWORD,int> m_mapOfStates;
	};

    class SOUI_EXP SSkinObjBase : public TObjRefImpl<SObjectImpl<ISkinObj>>
    {
		SOUI_CLASS_NAME_EX(SSkinObjBase, L"skinObjBase", Skin)
    public:
        SSkinObjBase():m_byAlpha(0xFF),m_bEnableColorize(TRUE),m_crColorize(0),m_nScale(100){}

        /**
        * GetAlpha
        * @brief    获得skin对象包含透明度
        * @return   BYTE -- 透明度
        * Describe  [0-255]
        */    
        BYTE GetAlpha() const;

		int State2Index(DWORD dwState) const;
        /**
        * SetAlpha
        * @brief    设定skin对象包含透明度
        * @param    BYTE byAlpha-- 透明度
        * @return   void
        * Describe  
        */    
        virtual void SetAlpha(BYTE byAlpha);

        virtual void Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha);
        virtual void Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState);

		virtual void Draw2(IRenderTarget *pRT, LPCRECT rcDraw, int iState, BYTE byAlpha);
		virtual void Draw2(IRenderTarget *pRT, LPCRECT rcDraw, int iState);

        virtual SIZE GetSkinSize();

        virtual BOOL IgnoreState();

        virtual int GetStates();

		virtual int GetScale() const;

		virtual ISkinObj * Scale(int nScale);

		virtual LPCWSTR GetName() const;

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"alpha",m_byAlpha,TRUE)   //皮肤透明度
            ATTR_INT(L"enableColorize",m_bEnableColorize,TRUE)
			ATTR_INT(L"scale",m_nScale,FALSE)
			ATTR_STRINGW(L"name",m_strName,FALSE)
        SOUI_ATTRS_END()

    protected:
		virtual void OnInitFinished(pugi::xml_node xmlNode);

		virtual void _Scale(ISkinObj *pObj, int nScale);

        /**
        * _Draw
        * @brief    Draw函数的实现
        * @param    IRenderTarget * pRT --  渲染目标
        * @param    LPCRECT rcDraw --  渲染位置
        * @param    DWORD dwState --  渲染状态
        * @param    BYTE byAlpha --  透明度
        * @return   void
        * Describe  每一个skin需要实现一个_Draw方法
        */    
        virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha)=0;

        BYTE		m_byAlpha;
        COLORREF    m_crColorize;
        BOOL        m_bEnableColorize;
		int			m_nScale;

		SStringW	m_strName;

		SState2Index	m_state2Index;

    };

}//namespace SOUI