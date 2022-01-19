#pragma once
#include <interface/obj-ref-i.h>

SNSBEGIN

#undef CreateWindow

#undef INTERFACE
#define INTERFACE INativeWnd
DECLARE_INTERFACE_(INativeWnd,IObject)
{
	STDMETHOD_(HWND,CreateWindow)(THIS_ LPCTSTR lpWindowName, DWORD dwStyle,DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent,LPVOID lpParam ) PURE;

	STDMETHOD_(HWND,GetHwnd)(THIS) PURE;

	STDMETHOD_(BOOL,SubclassWindow)(THIS_ HWND hWnd) PURE;

	STDMETHOD_(HWND,UnsubclassWindow)(THIS_ BOOL bForce /*= FALSE*/) PURE;

	STDMETHOD_(const MSG *, GetCurrentMessage)(THIS) SCONST PURE;

	STDMETHOD_(int,GetDlgCtrlID)(THIS) SCONST PURE;

	STDMETHOD_(DWORD,GetStyle)(THIS) SCONST PURE;

	STDMETHOD_(DWORD,GetExStyle)(THIS) SCONST PURE;

	STDMETHOD_(LONG_PTR,GetWindowLongPtr)(THIS_ int nIndex) SCONST PURE;

	STDMETHOD_(LONG_PTR,SetWindowLongPtr)(THIS_ int nIndex, LONG_PTR dwNewLong) PURE;

	STDMETHOD_(HWND,GetParent)(THIS) PURE;

	STDMETHOD_(HWND,SetParent)(THIS_ HWND hWndNewParent) PURE;

	STDMETHOD_(BOOL,IsWindowEnabled)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,ModifyStyle)(THIS_ DWORD dwRemove, DWORD dwAdd, UINT nFlags/*=0*/) PURE;

	STDMETHOD_(BOOL,ModifyStyleEx)(THIS_ DWORD dwRemove, DWORD dwAdd, UINT nFlags/*=0*/) PURE;

	STDMETHOD_(BOOL,SetWindowPos)(THIS_ HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) PURE;

	STDMETHOD_(BOOL,CenterWindow)(THIS_ HWND hWndCenter /*= NULL*/) PURE;

	STDMETHOD_(BOOL,DestroyWindow)(THIS) PURE;

	STDMETHOD_(BOOL,IsWindow)(THIS) PURE;

	STDMETHOD_(BOOL,Invalidate)(THIS_ BOOL bErase /*= TRUE*/) PURE;

	STDMETHOD_(BOOL,InvalidateRect)(THIS_ LPCRECT lpRect, BOOL bErase/* = TRUE*/) PURE;

	STDMETHOD_(BOOL,GetWindowRect)(THIS_ LPRECT lpRect) SCONST PURE;

	STDMETHOD_(BOOL,GetClientRect)(THIS_ LPRECT lpRect) SCONST PURE;

	STDMETHOD_(BOOL,ClientToScreen)(THIS_ LPPOINT lpPoint) SCONST PURE;

	STDMETHOD_(BOOL,ClientToScreen2)(THIS_ LPRECT lpRect) SCONST PURE;

	STDMETHOD_(BOOL,ScreenToClient)(THIS_ LPPOINT lpPoint) SCONST PURE;

	STDMETHOD_(BOOL,ScreenToClient2)(THIS_ LPRECT lpRect) SCONST PURE;

	STDMETHOD_(int,MapWindowPoints)(THIS_ HWND hWndTo, LPPOINT lpPoint, UINT nCount) SCONST PURE;

	STDMETHOD_(int,MapWindowRect)(THIS_ HWND hWndTo, LPRECT lpRect) SCONST PURE;

	STDMETHOD_(UINT_PTR,SetTimer)(THIS_ UINT_PTR nIDEvent, UINT nElapse, void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD) /*= NULL*/) PURE;

	STDMETHOD_(BOOL,KillTimer)(THIS_ UINT_PTR nIDEvent) PURE;

	STDMETHOD_(HDC,GetDC)(THIS) PURE;

	STDMETHOD_(HDC,GetWindowDC)(THIS) PURE;

	STDMETHOD_(int,ReleaseDC)(THIS_ HDC hDC) PURE;

	STDMETHOD_(BOOL,CreateCaret)(THIS_ HBITMAP hBitmap) PURE;

	STDMETHOD_(BOOL,HideCaret)(THIS) PURE;

	STDMETHOD_(BOOL,ShowCaret)(THIS) PURE;

	STDMETHOD_(HWND,SetCapture)(THIS) PURE;

	STDMETHOD_(HWND,SetFocus)(THIS) PURE;

	STDMETHOD_(LRESULT,SendMessage)(THIS_ UINT message, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/) PURE;

	STDMETHOD_(BOOL,PostMessage)(THIS_ UINT message, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/) PURE;

	STDMETHOD_(BOOL,SendNotifyMessage)(THIS_ UINT message, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/) PURE;

	STDMETHOD_(BOOL,SetWindowText)(THIS_ LPCTSTR lpszString) PURE;

	STDMETHOD_(int,GetWindowText)(THIS_ LPTSTR lpszStringBuf, int nMaxCount) SCONST PURE;

	STDMETHOD_(BOOL,IsIconic)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,IsZoomed)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,IsWindowVisible)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,MoveWindow)(THIS_ int x, int y, int nWidth, int nHeight, BOOL bRepaint /*= TRUE*/) PURE;

	STDMETHOD_(BOOL,MoveWindow2)(THIS_ LPCRECT lpRect, BOOL bRepaint /*= TRUE*/) PURE;

	STDMETHOD_(BOOL,ShowWindow)(THIS_ int nCmdShow) PURE;

	STDMETHOD_(int,SetWindowRgn)(THIS_ HRGN hRgn,BOOL bRedraw/*=TRUE*/) PURE;

	STDMETHOD_(BOOL,SetLayeredWindowAttributes)(THIS_ COLORREF crKey,BYTE bAlpha,DWORD dwFlags) PURE;

	STDMETHOD_(BOOL,UpdateLayeredWindow)(THIS_ HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc,COLORREF crKey, BLENDFUNCTION *pblend,DWORD dwFlags) PURE;
};

SNSEND