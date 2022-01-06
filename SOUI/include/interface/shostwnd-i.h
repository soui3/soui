#pragma once
#include <interface/sobject-i.h>
SNSBEGIN

interface IWindow;

#undef INTERFACE
#define INTERFACE IHostWnd
DECLARE_INTERFACE_(IHostWnd,IObject)
{
	STDMETHOD_(void,SetLayoutId)(THIS_ LPCTSTR pszLayoutId) PURE;
	STDMETHOD_(HWND,Create)(THIS_ HWND hWndParent,DWORD dwStyle,DWORD dwExStyle, int x /*= 0*/, int y /*= 0*/, int nWidth /*= 0*/, int nHeight /*= 0*/) PURE;
	STDMETHOD_(BOOL,Destroy)(THIS) PURE;
	STDMETHOD_(IWindow*,GetIRoot)(THIS) PURE;
	STDMETHOD_(HWND,GetHwnd)(THIS) PURE;
};

#undef INTERFACE
#define INTERFACE IHostDialog
DECLARE_INTERFACE_(IHostDialog, IObjRef)
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

	STDMETHOD_(INT_PTR,DoModal)(THIS_ HWND hParent/*=NULL*/) PURE;
	STDMETHOD_(void,EndDialog)(THIS_ INT_PTR nResult) PURE;
	STDMETHOD_(IHostWnd*,GetHostWnd)(THIS) PURE;
};

SNSEND
