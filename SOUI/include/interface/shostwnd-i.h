#pragma once
#include <interface/SNativeWnd-i.h>
SNSBEGIN

interface IWindow;

#undef INTERFACE
#define INTERFACE IHostWnd
DECLARE_INTERFACE_(IHostWnd,INativeWnd)
{
	STDMETHOD_(HWND,CreateEx)(THIS_ HWND hWndParent,DWORD dwStyle,DWORD dwExStyle, int x, int y , int nWidth, int nHeight) PURE;
	STDMETHOD_(HWND,Create)(THIS_ HWND hWndParent,int x /*= 0*/, int y /*= 0*/, int nWidth /*= 0*/, int nHeight /*= 0*/) PURE;
	STDMETHOD_(void,SetLayoutId)(THIS_ LPCTSTR pszLayoutId) PURE;
	STDMETHOD_(IWindow*,GetIRoot)(THIS) PURE;
};

#undef INTERFACE
#define INTERFACE IHostDialog
DECLARE_INTERFACE_(IHostDialog,IHostWnd)
{
	STDMETHOD_(INT_PTR,DoModal)(THIS_ HWND hParent/*=NULL*/) PURE;
	STDMETHOD_(void,EndDialog)(THIS_ INT_PTR nResult) PURE;
};

SNSEND
