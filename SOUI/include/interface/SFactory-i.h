#pragma once

#include <interface/obj-ref-i.h>
#include <interface/sapp-i.h>
#include <interface/shostwnd-i.h>
#include <interface/sstring-i.h>
#include <interface/sxml-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE ISouiFactory
DECLARE_INTERFACE_(ISouiFactory,IObjRef)
{
	STDMETHOD_(HRESULT,CreateApp)(THIS_ IApplication **ppRet,IRenderFactory *pRenderFac,HMODULE hInst,LPCTSTR pszHostClassName,BOOL bImeApp) PURE;
	STDMETHOD_(HRESULT,CreateHostWnd)(THIS_ IHostWnd **ppRet,LPCTSTR pszResID) PURE;
	STDMETHOD_(HRESULT,CreateHostDialog)(THIS_ IHostDialog **ppRet,LPCTSTR pszResID) PURE;
	STDMETHOD_(HRESULT,CreateStringA)(THIS_ IStringA **ppRet,LPCSTR pszSrc) PURE;
	STDMETHOD_(HRESULT,CreateStringW)(THIS_ IStringW **ppRet,LPCWSTR pszStr) PURE;
	STDMETHOD_(HRESULT,CreateXmlDoc)(THIS_ IXmlDoc **ppRet) PURE;
};

extern "C"
{
	HRESULT SOUI_EXP CreateSouiFactory(ISouiFactory ** ppRet);
}

SNSEND