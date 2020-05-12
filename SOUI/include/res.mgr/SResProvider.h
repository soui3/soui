//////////////////////////////////////////////////////////////////////////
//   File Name: sresprovider.h
// Description: Resource Provider
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <interface/SResProvider-i.h>
#include <helper/SResID.h>
#include <helper/obj-ref-impl.hpp>

SNSBEGIN

//定义3种系统资源类型
extern const  TCHAR KTypeBitmap[];
extern const  TCHAR KTypeCursor[];
extern const  TCHAR KTypeIcon[];  
extern const  TCHAR KTypeHtml[];

class SOUI_EXP SResLoadFromMemory
{
public:
	static IBitmap * LoadImage(LPVOID pBuf,size_t size);
	static IImgX   * LoadImgX(LPVOID pBuf,size_t size);
};

class SResProviderPE:public TObjRefImpl<IResProvider>
{
public:
	SResProviderPE();
	STDMETHOD_(BOOL,Init)(THIS_ WPARAM wParam,LPARAM lParam);
	STDMETHOD_(BOOL,HasResource)(THIS_ LPCTSTR pszType,LPCTSTR pszResName);
	STDMETHOD_(HICON,LoadIcon)(THIS_ LPCTSTR pszResName,int cx,int cy);
	STDMETHOD_(HBITMAP,LoadBitmap)(THIS_ LPCTSTR pszResName);
	STDMETHOD_(HCURSOR,LoadCursor)(THIS_ LPCTSTR pszResName);
	STDMETHOD_(IBitmap*, LoadImage)(THIS_ LPCTSTR pszType,LPCTSTR pszResName);
	STDMETHOD_(IImgX*, LoadImgX)(THIS_ LPCTSTR pszType,LPCTSTR pszResName);
	STDMETHOD_(size_t,GetRawBufferSize)(THIS_ LPCTSTR pszType,LPCTSTR pszResName);
	STDMETHOD_(BOOL,GetRawBuffer)(THIS_ LPCTSTR pszType,LPCTSTR pszResName,LPVOID pBuf,size_t size);
	STDMETHOD_(void,EnumResource)(THIS_ EnumResCallback funEnumCB,LPARAM lp);

protected:
	LPVOID GetRawBufferPtr(LPCTSTR strType,LPCTSTR pszResName);

	HRSRC MyFindResource(LPCTSTR strType, LPCTSTR pszResName );


	HINSTANCE m_hResInst;
};


class SOUI_EXP SResLoadFromFile
{
public:
	static HBITMAP LoadBitmap(LPCTSTR pszFileName);
	static HICON   LoadIcon(LPCTSTR pszFileName,int cx=0,int cy=0);
	static HCURSOR LoadCursor(LPCTSTR pszFileName);
	static IBitmap * LoadImage(LPCTSTR pszFileName);
	static IImgX   * LoadImgX(LPCTSTR pszFileName);
	static size_t GetRawBufferSize(LPCTSTR pszFileName);
	static BOOL GetRawBuffer(LPCTSTR pszFileName,LPVOID pBuf,size_t size);
};

class SResProviderFiles:public TObjRefImpl<IResProvider>
{
public:

	SResProviderFiles();

	STDMETHOD_(BOOL,Init)(THIS_ WPARAM wParam,LPARAM lParam);
	STDMETHOD_(BOOL,HasResource)(THIS_ LPCTSTR pszType,LPCTSTR pszResName);
	STDMETHOD_(HICON,LoadIcon)(THIS_ LPCTSTR pszResName,int cx,int cy);
	STDMETHOD_(HBITMAP,LoadBitmap)(THIS_ LPCTSTR pszResName);
	STDMETHOD_(HCURSOR,LoadCursor)(THIS_ LPCTSTR pszResName);
	STDMETHOD_(IBitmap*, LoadImage)(THIS_ LPCTSTR pszType,LPCTSTR pszResName);
	STDMETHOD_(IImgX*, LoadImgX)(THIS_ LPCTSTR pszType,LPCTSTR pszResName);
	STDMETHOD_(size_t,GetRawBufferSize)(THIS_ LPCTSTR pszType,LPCTSTR pszResName);
	STDMETHOD_(BOOL,GetRawBuffer)(THIS_ LPCTSTR pszType,LPCTSTR pszResName,LPVOID pBuf,size_t size);
	STDMETHOD_(void,EnumResource)(THIS_ EnumResCallback funEnumCB,LPARAM lp);
protected:
	SStringT GetRes( LPCTSTR strType,LPCTSTR pszResName );


	SStringT m_strPath;
	SMap<SResID,SStringT> m_mapFiles;
};

BOOL SOUI_EXP CreateResProvider(BUILTIN_RESTYPE resType,IObjRef **pObj);

SNSEND