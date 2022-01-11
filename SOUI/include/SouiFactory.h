#pragma once
#include <interface/SFactory-i.h>
#include <helper/obj-ref-impl.hpp>

SNSBEGIN

class SouiFactory : public TObjRefImpl<ISouiFactory>
{
public:
	SouiFactory(void);
	~SouiFactory(void);

public:
	STDMETHOD_(HRESULT,CreateApp)(THIS_ IApplication **ppRet,HMODULE hInst,LPCTSTR pszHostClassName=_T("SOUIHOST"),BOOL bImeApp=FALSE) OVERRIDE;
	STDMETHOD_(HRESULT,CreateHostWnd)(THIS_ IHostWnd **ppRet,LPCTSTR pszResID=NULL) OVERRIDE;
	STDMETHOD_(HRESULT,CreateHostDialog)(THIS_ IHostDialog **ppRet,LPCTSTR pszResID=NULL) OVERRIDE;
	STDMETHOD_(HRESULT,CreateStringA)(THIS_ IStringA **ppRet,LPCSTR pszSrc) OVERRIDE;
	STDMETHOD_(HRESULT,CreateStringW)(THIS_ IStringW **ppRet,LPCWSTR pszSrc) OVERRIDE;
	STDMETHOD_(HRESULT,CreateXmlDoc)(THIS_ IXmlDoc **ppRet) OVERRIDE;
};

extern "C"
{
	HRESULT SOUI_EXP CreateSouiFactory(ISouiFactory ** ppRet);
}
SNSEND