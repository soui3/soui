#pragma once
#include <interface/obj-ref-i.h>
#include <interface/sstring-i.h>

SNSBEGIN

class SWindow;

#undef INTERFACE
#define INTERFACE IAttrStorage
DECLARE_INTERFACE_(IAttrStorage,IObjRef)
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

	STDMETHOD_(void,OnSetAttribute)(THIS_ const IStringW * strName, const IStringW * strValue,bool bHandled) PURE;
	STDMETHOD_(BOOL,OnGetAttribute)(THIS_ const IStringW * strName, IStringW *strValue) SCONST PURE;
};

#undef INTERFACE
#define INTERFACE IAttrStorageFactory
DECLARE_INTERFACE_(IAttrStorageFactory,IObjRef)
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

	STDMETHOD_(HRESULT,CreateAttrStorage)(THIS_ SWindow * owner,IAttrStorage** ppAttrStorage) SCONST PURE; 
};

SNSEND