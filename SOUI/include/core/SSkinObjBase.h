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

		int GetIndex(DWORD dwState, bool checkAsPushdown) const;

		BOOL Init(pugi::xml_node xmlNode);

		static int GetDefIndex(DWORD dwState,bool checkAsPushdown=false);

		static DWORD String2State(const SStringW & strState);
	protected:
		SMap<DWORD,int> m_mapOfStates;
	};

    class SOUI_EXP SSkinObjBase : public TObjRefImpl<SObjectImpl<ISkinObj>>
    {
		SOUI_CLASS_NAME_EX(SSkinObjBase, L"skinObjBase", Skin)
    public:
		SSkinObjBase();

        /**
        * GetAlpha
        * @brief    获得skin对象包含透明度
        * @return   BYTE -- 透明度
        * Describe  [0-255]
        */    
        BYTE GetAlpha() const;

		virtual int State2Index(DWORD dwState) const;
        /**
        * SetAlpha
        * @brief    设定skin对象包含透明度
        * @param    BYTE byAlpha-- 透明度
        * @return   void
        * Describe  
        */    
        virtual void SetAlpha(BYTE byAlpha);

        virtual void DrawByState(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha) const;
        virtual void DrawByState(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState) const;

		virtual void DrawByIndex(IRenderTarget *pRT, LPCRECT rcDraw, int iState, BYTE byAlpha) const;
		virtual void DrawByIndex(IRenderTarget *pRT, LPCRECT rcDraw, int iState) const;

        virtual SIZE GetSkinSize() const;

        virtual int GetStates() const;

		virtual int GetScale() const;

		virtual ISkinObj * Scale(int nScale);

		virtual LPCWSTR WINAPI GetName() const;

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"alpha",m_byAlpha,TRUE)   //皮肤透明度
			ATTR_BOOL(L"enableColorize",m_bEnableColorize,TRUE)
			ATTR_BOOL(L"checkAsPushdown",m_checkAsPushdown,TRUE)
			ATTR_INT(L"scale",m_nScale,FALSE)
			ATTR_BOOL(L"enableScale",m_bEnableScale,TRUE)
			ATTR_STRINGW(L"name",m_strName,FALSE)
        SOUI_ATTRS_END()

    protected:
		virtual void WINAPI OnInitFinished(pugi::xml_node xmlNode);

		virtual void _Scale(ISkinObj *pObj, int nScale);

		virtual void _DrawByState(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha) const;

		virtual void _DrawByIndex(IRenderTarget *pRT, LPCRECT rcDraw, int iState, BYTE byAlpha)  const = 0;

        BYTE		m_byAlpha;
        COLORREF    m_crColorize;
        bool        m_bEnableColorize;
		int			m_nScale;
		bool		m_bEnableScale;
		bool		m_checkAsPushdown;

		SStringW	m_strName;

		SState2Index	m_state2Index;

    };

}//namespace SOUI