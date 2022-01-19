/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SHostDialog.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI的Dialog模块
*/

#pragma once

#include "shostwnd.h"
#include "smsgloop.h"

namespace SOUI
{
	template<class T>
	class THostWndProxy : public T, public SHostWnd
	{
	public:
		THostWndProxy(LPCTSTR pszResId):SHostWnd(pszResId){}

	public:
		STDMETHOD_(long,AddRef) (THIS) {return SHostWnd::AddRef();}
		STDMETHOD_(long,Release) (THIS) {return SHostWnd::Release();}
		STDMETHOD_(void,OnFinalRelease) (THIS) {SHostWnd::OnFinalRelease();}
		STDMETHOD_(int,GetID)(THIS) SCONST {return SHostWnd::GetID();}
		STDMETHOD_(void,SetID)(THIS_ int nID) {return SHostWnd::SetID(nID);}
		STDMETHOD_(LPCWSTR,GetName)(THIS) SCONST {return SHostWnd::GetName();}
		STDMETHOD_(void,SetName)(THIS_ LPCWSTR pszName) {return SHostWnd::SetName(pszName);}
		STDMETHOD_(BOOL,InitFromXml)(THIS_ IXmlNode * xmlNode ) {return SHostWnd::InitFromXml(xmlNode);}
		STDMETHOD_(void,OnInitFinished)(THIS_ IXmlNode* xmlNode) {return SHostWnd::OnInitFinished(xmlNode);}
		STDMETHOD_(HRESULT,SetAttributeA)(THIS_ const IStringA * strAttribName, const IStringA *  strValue, BOOL bLoading) {
			return SHostWnd::SetAttributeA(strAttribName,strValue,bLoading);
		}
		STDMETHOD_(HRESULT,SetAttributeW)(THIS_ const IStringW *  strAttribName, const IStringW *  strValue, BOOL bLoading)
		{
			return SHostWnd::SetAttributeW(strAttribName,strValue,bLoading);
		}
		STDMETHOD_(HRESULT,SetAttribute)(THIS_ LPCSTR pszAttr, LPCSTR pszValue, BOOL bLoading)
		{
			return SHostWnd::SetAttribute(pszAttr,pszValue,bLoading);
		}
		STDMETHOD_(HRESULT,SetAttributeW)(THIS_ LPCWSTR pszAttr, LPCWSTR pszValue, BOOL bLoading)
		{
			return SHostWnd::SetAttributeW(pszAttr,pszValue,bLoading);
		}
		STDMETHOD_(BOOL,GetAttribute)(THIS_ const IStringW * strAttr, IStringW * pValue) SCONST
		{
			return SHostWnd::GetAttribute(strAttr,pValue);
		}
		STDMETHOD_(HRESULT,AfterAttribute)(THIS_ const IStringW * strAttribName,const IStringW * strValue, BOOL bLoading, HRESULT hr)
		{
			return SHostWnd::AfterAttribute(strAttribName,strValue,bLoading,hr);
		}

		STDMETHOD_(HWND,CreateWindow)(THIS_ LPCTSTR lpWindowName, DWORD dwStyle,DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent,LPVOID lpParam ) OVERRIDE
		{return SHostWnd::CreateWindow(lpWindowName,dwStyle,dwExStyle,x,y,nWidth,nHeight,hWndParent,lpParam);}

		STDMETHOD_(HWND,GetHwnd)(THIS) OVERRIDE
		{return SHostWnd::GetHwnd();}

		STDMETHOD_(BOOL,SubclassWindow)(THIS_ HWND hWnd) OVERRIDE
		{return SHostWnd::SubclassWindow(hWnd);}
		STDMETHOD_(HWND,UnsubclassWindow)(THIS_ BOOL bForce = FALSE) OVERRIDE
		{return SHostWnd::UnsubclassWindow(bForce);}
		STDMETHOD_(const MSG *, GetCurrentMessage)(THIS) SCONST OVERRIDE
		{return SHostWnd::GetCurrentMessage();}
		STDMETHOD_(int,GetDlgCtrlID)(THIS) SCONST OVERRIDE
		{return SHostWnd::GetDlgCtrlID();}
		STDMETHOD_(DWORD,GetStyle)(THIS) SCONST OVERRIDE
		{return SHostWnd::GetStyle();}
		STDMETHOD_(DWORD,GetExStyle)(THIS) SCONST OVERRIDE
		{return SHostWnd::GetExStyle();}
		STDMETHOD_(LONG_PTR,GetWindowLongPtr)(THIS_ int nIndex) SCONST OVERRIDE
		{return SHostWnd::GetWindowLongPtr(nIndex);}
		STDMETHOD_(LONG_PTR,SetWindowLongPtr)(THIS_ int nIndex, LONG_PTR dwNewLong) OVERRIDE
		{return SHostWnd::SetWindowLongPtr(nIndex,dwNewLong);}
		STDMETHOD_(HWND,GetParent)(THIS) OVERRIDE
		{return SHostWnd::GetParent();}
		STDMETHOD_(HWND,SetParent)(THIS_ HWND hWndNewParent) OVERRIDE
		{return SHostWnd::SetParent(hWndNewParent);}
		STDMETHOD_(BOOL,IsWindowEnabled)(THIS) SCONST OVERRIDE
		{return SHostWnd::IsWindowEnabled();}
		STDMETHOD_(BOOL,ModifyStyle)(THIS_ DWORD dwRemove, DWORD dwAdd, UINT nFlags=0) OVERRIDE
		{return SHostWnd::ModifyStyle(dwRemove,dwAdd,nFlags);}
		STDMETHOD_(BOOL,ModifyStyleEx)(THIS_ DWORD dwRemove, DWORD dwAdd, UINT nFlags=0) OVERRIDE
		{return SHostWnd::ModifyStyleEx(dwRemove,dwAdd,nFlags);}
		STDMETHOD_(BOOL,SetWindowPos)(THIS_ HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) OVERRIDE
		{return SHostWnd::SetWindowPos(hWndInsertAfter,x,y,cx,cy,nFlags);}
		STDMETHOD_(BOOL,CenterWindow)(THIS_ HWND hWndCenter = NULL) OVERRIDE
		{return SHostWnd::CenterWindow(hWndCenter);}
		STDMETHOD_(BOOL,DestroyWindow)(THIS) OVERRIDE
		{return SHostWnd::DestroyWindow();}
		STDMETHOD_(BOOL,IsWindow)(THIS) OVERRIDE
		{return SHostWnd::IsWindow();}
		STDMETHOD_(BOOL,Invalidate)(THIS_ BOOL bErase = TRUE) OVERRIDE
		{return SHostWnd::Invalidate(bErase);}
		STDMETHOD_(BOOL,InvalidateRect)(THIS_ LPCRECT lpRect, BOOL bErase = TRUE) OVERRIDE
		{return SHostWnd::InvalidateRect(lpRect,bErase);}
		STDMETHOD_(BOOL,GetWindowRect)(THIS_ LPRECT lpRect) SCONST OVERRIDE
		{return SNativeWnd::GetWindowRect(lpRect);}
		STDMETHOD_(BOOL,GetClientRect)(THIS_ LPRECT lpRect) SCONST OVERRIDE
		{return SNativeWnd::GetClientRect(lpRect);}
		STDMETHOD_(BOOL,ClientToScreen)(THIS_ LPPOINT lpPoint) SCONST OVERRIDE
		{return SHostWnd::ClientToScreen(lpPoint);}
		STDMETHOD_(BOOL,ClientToScreen2)(THIS_ LPRECT lpRect) SCONST OVERRIDE
		{return SHostWnd::ClientToScreen2(lpRect);}
		STDMETHOD_(BOOL,ScreenToClient)(THIS_ LPPOINT lpPoint) SCONST OVERRIDE
		{return SHostWnd::ScreenToClient(lpPoint);}
		STDMETHOD_(BOOL,ScreenToClient2)(THIS_ LPRECT lpRect) SCONST OVERRIDE
		{return SHostWnd::ScreenToClient2(lpRect);}
		STDMETHOD_(int,MapWindowPoints)(THIS_ HWND hWndTo, LPPOINT lpPoint, UINT nCount) SCONST OVERRIDE
		{return SHostWnd::MapWindowPoints(hWndTo,lpPoint,nCount);}
		STDMETHOD_(int,MapWindowRect)(THIS_ HWND hWndTo, LPRECT lpRect) SCONST OVERRIDE
		{return SHostWnd::MapWindowRect(hWndTo,lpRect);}
		STDMETHOD_(UINT_PTR,SetTimer)(THIS_ UINT_PTR nIDEvent, UINT nElapse, void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD) = NULL) OVERRIDE
		{return SHostWnd::SetTimer(nIDEvent,nElapse,lpfnTimer);}
		STDMETHOD_(BOOL,KillTimer)(THIS_ UINT_PTR nIDEvent) OVERRIDE
		{return SHostWnd::KillTimer(nIDEvent);}
		STDMETHOD_(HDC,GetDC)(THIS) OVERRIDE
		{return SHostWnd::GetDC();}
		STDMETHOD_(HDC,GetWindowDC)(THIS) OVERRIDE
		{return SHostWnd::GetWindowDC();}
		STDMETHOD_(int,ReleaseDC)(THIS_ HDC hDC) OVERRIDE
		{return SHostWnd::ReleaseDC(hDC);}
		STDMETHOD_(BOOL,CreateCaret)(THIS_ HBITMAP hBitmap) OVERRIDE
		{return SHostWnd::CreateCaret(hBitmap);}
		STDMETHOD_(BOOL,HideCaret)(THIS) OVERRIDE
		{return SHostWnd::HideCaret();}
		STDMETHOD_(BOOL,ShowCaret)(THIS) OVERRIDE
		{return SHostWnd::ShowCaret();}
		STDMETHOD_(HWND,SetCapture)(THIS) OVERRIDE
		{return SHostWnd::SetCapture();}
		STDMETHOD_(HWND,SetFocus)(THIS) OVERRIDE
		{return SHostWnd::SetFocus();}
		STDMETHOD_(LRESULT,SendMessage)(THIS_ UINT message, WPARAM wParam =0, LPARAM lParam =0) OVERRIDE
		{return SHostWnd::SendMessage(message,wParam,lParam);}
		STDMETHOD_(BOOL,PostMessage)(THIS_ UINT message, WPARAM wParam =0, LPARAM lParam =0) OVERRIDE
		{return SHostWnd::PostMessage(message,wParam,lParam);}
		STDMETHOD_(BOOL,SendNotifyMessage)(THIS_ UINT message, WPARAM wParam =0, LPARAM lParam =0) OVERRIDE
		{return SHostWnd::SendNotifyMessage(message,wParam,lParam);}
		STDMETHOD_(BOOL,SetWindowText)(THIS_ LPCTSTR lpszString) OVERRIDE
		{return SHostWnd::SetWindowText(lpszString);}
		STDMETHOD_(int,GetWindowText)(THIS_ LPTSTR lpszStringBuf, int nMaxCount) SCONST OVERRIDE
		{return SHostWnd::GetWindowText(lpszStringBuf,nMaxCount);}
		STDMETHOD_(BOOL,IsIconic)(THIS) SCONST OVERRIDE
		{return SHostWnd::IsIconic();}
		STDMETHOD_(BOOL,IsZoomed)(THIS) SCONST OVERRIDE
		{return SHostWnd::IsZoomed();}
		STDMETHOD_(BOOL,IsWindowVisible)(THIS) SCONST OVERRIDE
		{return SHostWnd::IsWindowVisible();}
		STDMETHOD_(BOOL,MoveWindow)(THIS_ int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE) OVERRIDE
		{return SHostWnd::MoveWindow(x,y,nWidth,nHeight,bRepaint);}
		STDMETHOD_(BOOL,MoveWindow2)(THIS_ LPCRECT lpRect, BOOL bRepaint = TRUE) OVERRIDE
		{return SHostWnd::MoveWindow2(lpRect,bRepaint);}
		STDMETHOD_(BOOL,ShowWindow)(THIS_ int nCmdShow) OVERRIDE
		{return SHostWnd::ShowWindow(nCmdShow);}
		STDMETHOD_(int,SetWindowRgn)(THIS_ HRGN hRgn,BOOL bRedraw=TRUE) OVERRIDE
		{return SHostWnd::SetWindowRgn(hRgn,bRedraw);}
		STDMETHOD_(BOOL,SetLayeredWindowAttributes)(THIS_ COLORREF crKey,BYTE byAlpha,DWORD dwFlags) OVERRIDE
		{return SHostWnd::SetLayeredWindowAttributes(crKey,byAlpha,dwFlags);}
		STDMETHOD_(BOOL,UpdateLayeredWindow)(THIS_ HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc,COLORREF crKey, BLENDFUNCTION *pblend,DWORD dwFlags)
		{return SHostWnd::UpdateLayeredWindow(hdcDst,pptDst,psize,hdcSrc,pptSrc,crKey,pblend,dwFlags);}


		STDMETHOD_(void,SetLayoutId)(THIS_ LPCTSTR pszLayoutId) OVERRIDE
		{
			SHostWnd::SetLayoutId(pszLayoutId);
		}

		STDMETHOD_(IWindow*,GetIRoot)(THIS) OVERRIDE
		{
			return SHostWnd::GetIRoot();
		}
	};

    class SOUI_EXP SHostDialog : public THostWndProxy<IHostDialog>
    {
		SOUI_CLASS_NAME(SHostWnd,L"hostDialog")
    public:
        SHostDialog(LPCTSTR pszXmlName = NULL);
        ~SHostDialog(void);
        
	public:
		STDMETHOD_(INT_PTR,DoModal)(THIS_ HWND hParent=NULL) OVERRIDE;
		STDMETHOD_(void,EndDialog)(THIS_ INT_PTR nResult) OVERRIDE;
    protected:
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        void OnOK();
        void OnCancel();
        STDMETHOD_(IMessageLoop *,GetMsgLoop)(){return m_MsgLoop;}
        
        EVENT_MAP_BEGIN()
            EVENT_ID_COMMAND(IDOK,OnOK)
            EVENT_ID_COMMAND(IDCANCEL,OnCancel)
        EVENT_MAP_END()

        BEGIN_MSG_MAP_EX(SHostDialog)
            MSG_WM_CLOSE(OnCancel)
            MSG_WM_KEYDOWN(OnKeyDown)
            CHAIN_MSG_MAP(SHostWnd)
            REFLECT_NOTIFICATIONS_EX()
        END_MSG_MAP()

        INT_PTR m_nRetCode;
        
        SAutoRefPtr<IMessageLoop> m_MsgLoop;
    };

}
