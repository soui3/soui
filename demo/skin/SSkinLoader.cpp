#include "stdafx.h"
#include "SSkinLoader.h"
#include "res.mgr\SResProvider.h"

template<>
SSkinLoader * SSingleton<SSkinLoader>::ms_Singleton = NULL;

SSkinLoader::SSkinLoader(SApplication* theApp):m_pResProvider(NULL),m_theApp(theApp)
{
	m_privateSkinPool = new SSkinPool();
	GETSKINPOOLMGR->PushSkinPool(m_privateSkinPool);
}


SSkinLoader::~SSkinLoader()
{	
}

void SOUI::SSkinLoader::LoadSkinFormZip(SStringT respath, const TCHAR *strXmlSkin/*=_T("SkinXml:LoadSkinXml")*/)
{
	if (m_pResProvider == NULL)
	{
		if(CreateResProvider_ZIP((IObjRef**)&m_pResProvider))
			m_theApp->AddResProvider(m_pResProvider,NULL);
	}
	SASSERT(m_pResProvider);
	ZIPRES_PARAM param;
	param.ZipFile(m_theApp->GetRenderFactory(), respath, "www.bukengnikengshui.com");
	if (!m_pResProvider->Init((WPARAM)&param, 0))
	{
		SASSERT(0);
	}
	m_privateSkinPool->RemoveAll();

	pugi::xml_document xmlDoc;
	if (LOADXML(xmlDoc, strXmlSkin))
	{
		m_privateSkinPool->LoadSkins(xmlDoc.child(L"skin"));
	}
}

void SOUI::SSkinLoader::LoadSkin(SStringT respath,const TCHAR *strXmlSkin /*= _T("SkinXml:LoadSkinXml")*/)
{
	if (m_pResProvider == NULL)
	{
		if(CreateResProvider(RES_FILE, (IObjRef**)&m_pResProvider))
			m_theApp->AddResProvider(m_pResProvider, NULL);
	}
	SASSERT(m_pResProvider);
	if (!m_pResProvider->Init((WPARAM)respath.GetBuffer(0), NULL))
	{
		SASSERT(0);
	}
	m_privateSkinPool->RemoveAll();
	pugi::xml_document xmlDoc;
	if (LOADXML(xmlDoc,strXmlSkin))
	{
		m_privateSkinPool->LoadSkins(xmlDoc.child(L"skin"));
	}
}
