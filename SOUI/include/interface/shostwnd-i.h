#pragma once
#include <interface/obj-ref-i.h>

SNSBEGIN
#undef INTERFACE
#define INTERFACE IHostWnd
DECLARE_INTERFACE(IHostWnd)
{
	STDMETHOD_(void,SetLayoutId)(THIS_ LPCTSTR pszLayoutId) PURE;
	STDMETHOD_(HWND,Create)(THIS_ HWND hWndParent,DWORD dwStyle,DWORD dwExStyle, int x = 0, int y = 0, int nWidth = 0, int nHeight = 0) PURE;
	STDMETHOD_(BOOL,Destroy)(THIS) PURE;
};

#undef INTERFACE
#define INTERFACE IHostDialog
DECLARE_INTERFACE(IHostDialog)
{
	STDMETHOD_(IHostWnd*, GetHostWnd)(THIS) PURE;
	STDMETHOD_(INT_PTR,DoModal)(THIS_ HWND hParent=NULL) PURE;
	STDMETHOD_(void,EndDialog)(THIS_ INT_PTR nResult) PURE;
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

	STDMETHOD_(IHostWnd *,InitHostWnd)(THIS_ LPCTSTR pszLayout) PURE;
	STDMETHOD_(void,UninitHostWnd)(THIS_ IHostWnd *pHostWnd) PURE;

	STDMETHOD_(IHostDialog *,InitHostDialog)(THIS_ LPCTSTR pszLayout) PURE;
	STDMETHOD_(void,UninitHostDialog)(THIS_ IHostDialog * pHostDlg) PURE;
};

typedef IHostFactory * IHostFactoryPtr;

SNSEND
