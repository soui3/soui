#pragma once

#include <unknown/obj-ref-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE IAccProxy
DECLARE_INTERFACE_(IAccProxy,IObjRef)
{
	//!添加引用
	/*!
	*/
	STDMETHOD_(long,AddRef) (THIS) PURE;

	//!释放引用
	/*!
	*/
	STDMETHOD_(long,Release) (THIS) PURE;

	//!释放对象
	/*!
	*/
	STDMETHOD_(void,OnFinalRelease) (THIS) PURE;
	STDMETHOD(get_accName)(THIS_ BSTR *pszName) PURE;
	STDMETHOD(get_accValue)(THIS_ BSTR *pszValue) PURE;
	STDMETHOD(get_accDescription)(THIS_ BSTR *pszDescription) PURE;
	STDMETHOD(get_accRole)(THIS_ VARIANT *pvarRole) PURE;
	STDMETHOD(get_accState)(THIS_ VARIANT *pvarState) PURE;
	STDMETHOD(get_accHelp)(THIS_ BSTR *pszHelp) PURE;
	STDMETHOD(get_accHelpTopic)(THIS_ BSTR *pszHelpFile, long *pidTopic) PURE;
	STDMETHOD(get_accKeyboardShortcut)(THIS_ BSTR *pszKeyboardShortcut) PURE;
	STDMETHOD(get_accDefaultAction)(THIS_ BSTR *pszDefaultAction) PURE;
	STDMETHOD(accSelect)(THIS_ long flagsSelect) PURE;
	STDMETHOD(accDoDefaultAction)(THIS) PURE;
	STDMETHOD(put_accName)(THIS_ BSTR szName) PURE;
	STDMETHOD(put_accValue)(THIS_ BSTR szValue) PURE;
};

SNSEND