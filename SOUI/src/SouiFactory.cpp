#include <souistd.h>
#include <SouiFactory.h>

SNSBEGIN

SouiFactory::SouiFactory(void)
{
}

SouiFactory::~SouiFactory(void)
{
}

STDMETHODIMP_(HRESULT) SouiFactory::CreateApp(THIS_ IApplication **ppRet,HMODULE hInst,LPCTSTR pszHostClassName,BOOL bImeApp)
{
	* ppRet = new SApplication(NULL,hInst,pszHostClassName,SObjectDefaultRegister(),bImeApp);
	return S_OK;
}

STDMETHODIMP_(HRESULT) SouiFactory::CreateHostWnd(THIS_ IHostWnd **ppRet,LPCTSTR pszResID)
{
	*ppRet = new SHostWnd(pszResID);
	return S_OK;
}

STDMETHODIMP_(HRESULT) SouiFactory::CreateHostDialog(THIS_ IHostDialog **ppRet,LPCTSTR pszResID)
{
	*ppRet = new SHostDialog(pszResID);
	return S_OK;
}

STDMETHODIMP_(HRESULT) SouiFactory::CreateStringA(THIS_ IStringA **ppRet,LPCSTR pszSrc)
{
	*ppRet = new SStringA(pszSrc);
	return S_OK;
}

STDMETHODIMP_(HRESULT) SouiFactory::CreateStringW(THIS_ IStringW **ppRet,LPCWSTR pszSrc)
{
	*ppRet = new SStringW(pszSrc);
	return S_OK;
}

STDMETHODIMP_(HRESULT) SouiFactory::CreateXmlDoc(THIS_ IXmlDoc **ppRet)
{
	*ppRet = new SXmlDoc();
	return S_OK;
}

extern "C"
HRESULT CreateSouiFactory(ISouiFactory ** ppRet)
{
	*ppRet = new SouiFactory();
	return S_OK;
}


SNSEND