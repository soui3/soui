#pragma once

#include <interface/Sskinobj-i.h>
#include <unknown/obj-ref-impl.hpp>
#include <souicoll.h>
#include <core/Swnd.h>

SNSBEGIN
//////////////////////////////////////////////////////////////////////////
class SOUI_EXP SState2Index{
public:
	SState2Index() {};
	SState2Index(const SState2Index &src);

	int GetIndex(DWORD dwState, bool checkAsPushdown) const;

	BOOL Init(IXmlNode *pNode);

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

	STDMETHOD_(LPCWSTR,GetName)(THIS) SCONST OVERRIDE;
	STDMETHOD_(void,DrawByState)(THIS_ IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha) SCONST OVERRIDE;
	STDMETHOD_(void,DrawByState)(THIS_ IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState) SCONST OVERRIDE;
	STDMETHOD_(void,DrawByIndex)(THIS_ IRenderTarget *pRT, LPCRECT rcDraw, int iState,BYTE byAlpha) SCONST OVERRIDE;
	STDMETHOD_(void,DrawByIndex)(THIS_ IRenderTarget *pRT, LPCRECT rcDraw, int iState) SCONST OVERRIDE;
	STDMETHOD_(SIZE,GetSkinSize)(THIS) SCONST OVERRIDE;
	STDMETHOD_(int,GetStates)(THIS) SCONST OVERRIDE;
	STDMETHOD_(BYTE,GetAlpha)(THIS) SCONST OVERRIDE;
	STDMETHOD_(void,SetAlpha)(THIS_ BYTE byAlpha) OVERRIDE;
	STDMETHOD_(int,GetScale)(THIS) SCONST OVERRIDE;
	STDMETHOD_(ISkinObj *,Scale)(THIS_ int nScale) OVERRIDE;
	STDMETHOD_(void,OnColorize)(THIS_ COLORREF cr) OVERRIDE;

	int State2Index(DWORD dwState) const;

public:
	SOUI_ATTRS_BEGIN()
		ATTR_INT(L"alpha",m_byAlpha,TRUE)   //皮肤透明度
		ATTR_BOOL(L"enableColorize",m_bEnableColorize,TRUE)
		ATTR_BOOL(L"checkAsPushdown",m_checkAsPushdown,TRUE)
		ATTR_INT(L"scale",m_nScale,FALSE)
		ATTR_BOOL(L"enableScale",m_bEnableScale,TRUE)
		ATTR_STRINGW(L"name",m_strName,FALSE)
	SOUI_ATTRS_END()

protected:
	STDMETHOD_(void,OnInitFinished)(THIS_ IXmlNode * pNode) OVERRIDE;

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

SNSEND