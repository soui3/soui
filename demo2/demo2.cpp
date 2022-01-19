// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "MainDlg.h"
//从PE文件加载，注意从文件加载路径位置
#define RES_TYPE 0
//#define SYSRES_TYPE 0
// #define RES_TYPE 0   //PE
// #define RES_TYPE 1   //ZIP
// #define RES_TYPE 2   //7z
// #define RES_TYPE 2   //文件
//去掉多项支持，以免代码显得混乱
#if (RES_TYPE==1)
#include "resprovider-zip\zipresprovider-param.h"
#else 
#if (RES_TYPE==2)
#include "resprovider-7zip\zip7resprovider-param.h"
#endif
#endif
#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif

#include <interface/SFactory-i.h>

using namespace SOUI;

#define INIT_R_DATA
#include "res/resource.h"

class SOUIEngine
{
private:
	SComMgr m_ComMgr;
	SAutoRefPtr<IApplication> m_theApp;
	SAutoRefPtr<ISouiFactory> m_factory;
public:
	SOUIEngine():m_theApp(NULL){
		CreateSouiFactory(&m_factory);
	}

	BOOL Init(HINSTANCE hInstance){
		SAutoRefPtr<IRenderFactory> pRenderFactory;
		BOOL bLoaded = TRUE;
		do{
			//使用GDI渲染界面
			bLoaded = m_ComMgr.CreateRender_GDI((IObjRef * *)& pRenderFactory);
			SASSERT_FMT(bLoaded, _T("load interface [render] failed!"));
			if(!bLoaded) break;
			//设置图像解码引擎。默认为GDIP。基本主流图片都能解码。系统自带，无需其它库
			SAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory;
			bLoaded = m_ComMgr.CreateImgDecoder((IObjRef * *)& pImgDecoderFactory);
			SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("imgdecoder"));
			if(!bLoaded) break;

			pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
			m_factory->CreateApp(&m_theApp,pRenderFactory,hInstance,L"souihost",FALSE);
		}while(false);
		return bLoaded;
	};
	//加载系统资源
	BOOL LoadSystemRes()
	{
		BOOL bLoaded = TRUE;
		do{
			//从DLL加载系统资源
			{
				HMODULE hModSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
				if (hModSysResource)
				{
					SAutoRefPtr<IResProvider> sysResProvider;
					CreateResProvider(RES_PE, (IObjRef * *)& sysResProvider);
					sysResProvider->Init((WPARAM)hModSysResource, 0);
					m_theApp->LoadSystemNamedResource(sysResProvider);
					FreeLibrary(hModSysResource);
				}
				else
				{
					bLoaded = FALSE;
				}
				SASSERT(bLoaded);
				if(!bLoaded) break;
			}
		}while(false);
		return bLoaded;
	}
	//加载用户资源
	BOOL LoadUserRes()
	{
		SAutoRefPtr<IResProvider>   pResProvider;
		BOOL bLoaded = FALSE;
		do{
#ifdef _DEBUG		
			//选择了仅在Release版本打包资源则在DEBUG下始终使用文件加载
			{
				CreateResProvider(RES_FILE, (IObjRef * *)& pResProvider);
				bLoaded = pResProvider->Init((LPARAM)_T("uires"), 0);
				SASSERT(bLoaded);
				if(!bLoaded) break;
			}
#else
			{
				CreateResProvider(RES_PE, (IObjRef * *)& pResProvider);
				bLoaded = pResProvider->Init((WPARAM)m_theApp->GetModule(), 0);
				SASSERT(bLoaded);
				if(!bLoaded) break;
			}
#endif
			m_theApp->InitXmlNamedID((const LPCWSTR*)&R.name, (const int*)&R.id,sizeof(R.id)/sizeof(int));
			m_theApp->GetResProviderMgr()->AddResProvider(pResProvider);
		}while(false);
		return bLoaded;
	}
	//加载LUA支持
	BOOL LoadLUAModule()
	{
		BOOL bLoaded =FALSE;
		SAutoRefPtr<IScriptFactory> pScriptLuaFactory;
		bLoaded = m_ComMgr.CreateScrpit_Lua((IObjRef * *)& pScriptLuaFactory);
		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("scirpt_lua"));
		m_theApp->SetScriptFactory(pScriptLuaFactory);
		return bLoaded;
	}
	//加载多语言支持
	BOOL LoadMultiLangsModule()
	{
		BOOL bLoaded = FALSE;
		SAutoRefPtr<ITranslatorMgr> trans;
		bLoaded = m_ComMgr.CreateTranslator((IObjRef * *)& trans);
		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("translator"));
		if (trans)
		{//加载语言翻译包
			m_theApp->SetTranslator(trans);
			IXmlDoc *pXmlDoc=m_theApp->LoadXmlDocment( _T("translator:lang_cn"));
			if(pXmlDoc)
			{
				SAutoRefPtr<ITranslator> langCN;
				trans->CreateTranslator(&langCN);
				langCN->Load(pXmlDoc->Root()->Child(L"language",FALSE), 1);//1=LD_XML
				trans->InstallTranslator(langCN);
				pXmlDoc->Release();
			}
		}
		return bLoaded;
	}
	//注册用户自定义皮肤和控件
	void Regitercustom()
	{
	}

	~SOUIEngine()
	{
		m_theApp=NULL;
		m_factory=NULL;
	}

	IApplication* GetApp()
	{
		return m_theApp;
	}
};
//debug时方便调试设置当前目录以便从文件加载资源
void SetDefaultDir()
{
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));

	LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
#ifdef _DEBUG
	_tcscpy(lpInsertPos + 1, _T("..\\demo3"));
#else
	_tcscpy(lpInsertPos + 1, _T("\0"));
#endif
	SetCurrentDirectory(szCurrentDir);
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));
	SetDefaultDir();
	int nRet = 0;
	{
		SOUIEngine souiEngine;
		if (souiEngine.Init(hInstance))
		{
			//加载系统资源
			souiEngine.LoadSystemRes();
			//注册用户自定义皮肤/控件/布局/其它自定义
			souiEngine.Regitercustom();
			//加载用户资源
			souiEngine.LoadUserRes();
			CMainDlg dlgMain;
			dlgMain.Create(GetActiveWindow());
			dlgMain.SendMessage(WM_INITDIALOG);
			dlgMain.CenterWindow(dlgMain.m_hWnd);
			dlgMain.ShowWindow(SW_SHOWNORMAL);
			nRet = souiEngine.GetApp()->Run(dlgMain.m_hWnd);
		}
		else
		{
			MessageBox(NULL, _T("无法正常初使化SOUI"), _T("错误"), MB_OK | MB_ICONERROR);
		}
	}
	OleUninitialize();
	return nRet;
}
