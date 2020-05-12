// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TRANSLATOR_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TRANSLATOR_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <souicoll.h>
#include <helper/obj-ref-impl.hpp>
#include <string/tstring.h>
#include <string/strcpcvt.h>
#include <pugixml/pugixml.hpp>
#include <interface/STranslator-i.h>

SNSBEGIN

class SStrMap;
class SStrMapEntry;

enum LANGDATA{
	LD_UNKNOWN=0,
	LD_XML,
	LD_COMPILEDFILE,
	LD_COMPILEDDATA,
};

struct TrFontInfo{
	BYTE lfCharset;
	SStringT strFaceName;
};


class STranslator : public TObjRefImpl<ITranslator>
{
	friend class STranslatorMgr;

public:
	STranslator();
	~STranslator();

	STDMETHOD_(BOOL,Load)(THIS_ LPVOID pData,UINT uType);
	STDMETHOD_(void, GetName)(THIS_ wchar_t szBuf[TR_MAX_NAME_LEN]);
	STDMETHOD_( bool, NameEqual)(THIS_ LPCWSTR pszName);
	STDMETHOD_(GUID,guid)(THIS);
	STDMETHOD_(int,tr)(THIS_ const IStringW * strSrc,const IStringW * strCtx,wchar_t *pszOut,int nLen) SCONST;
	STDMETHOD_(void,getFontInfo)(THIS_ IStringW * strFont) SCONST;
protected:
	BOOL LoadFromXml(pugi::xml_node xmlLang);

	wchar_t	 m_szLangName[TR_MAX_NAME_LEN];
	GUID     m_guid;
	SArray<SStrMapEntry*> * m_arrEntry;
	SStringW m_strFontInfo;
};

class STranslatorMgr : public TObjRefImpl<ITranslatorMgr>
{
public:
	STranslatorMgr(void);
	~STranslatorMgr(void);

	STDMETHOD_(void ,SetLanguage)(THIS_ const IStringW * strLang);
	STDMETHOD_(void,GetLanguage)(THIS_ wchar_t szOut[TR_MAX_NAME_LEN]) SCONST;
	STDMETHOD_(BOOL,CreateTranslator)(THIS_ ITranslator ** ppTranslator);
	STDMETHOD_(BOOL,InstallTranslator)(THIS_ ITranslator * ppTranslator);
	STDMETHOD_(BOOL,UninstallTranslator)(THIS_ REFGUID id);
	STDMETHOD_(int,tr)(THIS_ const IStringW * strSrc,const IStringW * strCtx,wchar_t *pszOut,int nLen) SCONST;

protected:

	wchar_t	 m_szLangName[TR_MAX_NAME_LEN];

	SList<ITranslator*> *m_lstLang;

};

namespace TRANSLATOR
{
	SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **ppTrans);
}

SNSEND