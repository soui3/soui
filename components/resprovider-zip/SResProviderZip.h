#pragma once

#include <interface/SResProvider-i.h>
#include <unknown/obj-ref-impl.hpp>
#include <string/tstring.h>
#include <string/strcpcvt.h>
#include <souicoll.h>
#define _COLL_NS SOUI
#include <helper/SResID.h>
#include <interface/SRender-i.h>

#include "ZipArchive.h"
#include "zipresprovider-param.h"

SNSBEGIN

class SResProviderZip : public TObjRefImpl<IResProvider>
{
public:
	SResProviderZip();
	~SResProviderZip(void);
	
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
    BOOL _Init(LPCTSTR pszZipFile ,LPCSTR pszPsw);
    BOOL _Init(LPBYTE pBytes, DWORD dwByteCounts, LPCSTR pszPsw);    
    BOOL _Init(HINSTANCE hInst,LPCTSTR pszResName,LPCTSTR pszType ,LPCSTR pszPsw);
	BOOL _LoadSkin();
	SStringT _GetFilePath(LPCTSTR pszResName,LPCTSTR pszType);
	
	SMap<SResID,SStringT> m_mapFiles;
    SAutoRefPtr<IRenderFactory> m_renderFactory;
	CZipArchive m_zipFile;
	SStringT m_childDir;
};

namespace RESPROVIDER_ZIP
{
    SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef ** ppObj);
}

SNSEND