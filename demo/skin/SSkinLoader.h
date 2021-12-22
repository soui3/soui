#pragma once
#include "pugixml\pugixml.hpp"
#include "res.mgr\SSkinPool.h"
#include "helper\SplitString.h"
#include "resprovider-zip\zipresprovider-param.h"
#include "resprovider-zip\SResProviderZip.h"
namespace SOUI
{
#define RT_SKIN _T("SkinXml")
	class SSkinLoader :public SSingleton<SSkinLoader>
	{
		SAutoRefPtr<IResProvider> m_pResProvider; 
		SAutoRefPtr<SSkinPool>  m_privateSkinPool;
		SApplication *m_theApp;
// 		BOOL CreateResProvider_ZIP(IObjRef **ppObj)
// 		{
// 			return RESPROVIDER_ZIP::SCreateInstance(ppObj);
// 		}
	public:
		SSkinLoader(SApplication*);
		~SSkinLoader();
		void LoadSkinFormZip(SStringT respath, const TCHAR *strXmlSkin=_T("LoadSkinXml"));
		void LoadSkin(SStringT respath,const TCHAR *strXmlSkin = _T("LoadSkinXml"));
	};
}//END SOUI
