#pragma once
#include <interface/obj-ref-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE IHostDialog
DECLARE_INTERFACE(IHostDialog)
{
	STDMETHOD_(INT_PTR,DoModal)(THIS_ HWND hParent/*=NULL*/) PURE;
	STDMETHOD_(void,EndDialog)(THIS_ INT_PTR nResult) PURE;
};

#undef INTERFACE
#define INTERFACE IHostWnd
DECLARE_INTERFACE_(IHostWnd,IObjRef)
{
	STDMETHOD_(void,SetLayoutId)(THIS_ LPCTSTR pszLayoutId) PURE;
	STDMETHOD_(HWND,Create)(THIS_ HWND hWndParent,DWORD dwStyle,DWORD dwExStyle, int x /*= 0*/, int y /*= 0*/, int nWidth /*= 0*/, int nHeight /*= 0*/) PURE;
	STDMETHOD_(BOOL,Destroy)(THIS) PURE;
	STDMETHOD_(HRESULT,GetHostDialog)(THIS_ IHostDialog **ppDialog) PURE;
};



#undef INTERFACE
#define INTERFACE IHostFactory
DECLARE_INTERFACE_(IHostFactory,IObjRef)
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

	STDMETHOD_(HRESULT,NewHostWnd)(THIS_ IHostWnd ** ppHostWnd) PURE;

	STDMETHOD_(HRESULT,NewHostDialog)(THIS_ IHostDialog ** ppHostDialog) PURE;
};

typedef IHostFactory * IHostFactoryPtr;

SNSEND
