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
		STDMETHOD_(BOOL,IsClass)(THIS_ LPCWSTR lpszName) SCONST {return SHostWnd::IsClass(lpszName);}
		STDMETHOD_(LPCWSTR,GetObjectClass)(THIS) SCONST {return SHostWnd::GetObjectClass();}
		STDMETHOD_(int,GetObjectType)(THIS)  SCONST {return SHostWnd::GetObjectType();}
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

		STDMETHOD_(void,SetLayoutId)(THIS_ LPCTSTR pszLayoutId) OVERRIDE
		{
			SHostWnd::SetLayoutId(pszLayoutId);
		}
		STDMETHOD_(HWND,Create)(THIS_ HWND hWndParent,DWORD dwStyle,DWORD dwExStyle, int x = 0, int y = 0, int nWidth = 0, int nHeight = 0) OVERRIDE
		{
			return SHostWnd::Create(hWndParent,dwExStyle,dwExStyle,x,y,nWidth,nHeight);
		}

		STDMETHOD_(BOOL,Destroy)(THIS) OVERRIDE
		{
			return SHostWnd::Destroy();
		}
		STDMETHOD_(IWindow*,GetIRoot)(THIS) OVERRIDE
		{
			return SHostWnd::GetIRoot();
		}

		STDMETHOD_(HWND,GetHwnd)(THIS) OVERRIDE
		{
			return SHostWnd::GetHwnd();
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
