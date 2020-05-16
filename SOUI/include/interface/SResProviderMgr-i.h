#pragma once
#include <interface/SResProvider-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE IResProviderMgr
DECLARE_INTERFACE(IResProviderMgr)
{
	STDMETHOD_(void,AddResProvider)(THIS_ IResProvider * pResProvider,LPCTSTR pszUidef = _T("uidef:xml_init")) PURE;

	STDMETHOD_(void,RemoveResProvider)(THIS_ IResProvider * pResProvider) PURE;
	
	STDMETHOD_(void,RemoveAll)(THIS) PURE;

	STDMETHOD_(IResProvider *,GetHeadResProvider)(THIS) SCONST PURE;
	STDMETHOD_(IResProvider *,GetTailResProvider)(THIS) SCONST PURE;

	//////////////////////////////////////////////////////////////////////////
	STDMETHOD_(BOOL,HasResource)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) PURE;

	STDMETHOD_(HICON,LoadIcon)(THIS_ LPCTSTR pszResName,int cx=0,int cy=0,BOOL bFromFile = FALSE) PURE;

	STDMETHOD_(HCURSOR,LoadCursor)(THIS_ LPCTSTR pszResName,BOOL bFromFile = FALSE) PURE;

	STDMETHOD_(HBITMAP,LoadBitmap)(THIS_ LPCTSTR pszResName,BOOL bFromFile = FALSE) PURE;

	STDMETHOD_(IBitmap *,LoadImage)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) PURE;

	STDMETHOD_(IImgX *,LoadImgX)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) PURE;

	STDMETHOD_(size_t,GetRawBufferSize)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) PURE;

	STDMETHOD_(BOOL,GetRawBuffer)(THIS_ LPCTSTR pszType,LPCTSTR pszResName,LPVOID pBuf,size_t size) PURE;
};

SNSEND