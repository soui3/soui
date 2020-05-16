#include "souistd.h"
#include "SApp.h"
#include "core/SNativeWnd.h"
#include "core/SWindowMgr.h"

#include "res.mgr/sfontpool.h"
#include "res.mgr/SUiDef.h"

#include "helper/STimerEx.h"
#include "helper/SScriptTimer.h"
#include "helper/SAutoBuf.h"
#include "helper/SToolTip.h"
#include "helper/SAppDir.h"
#include "helper/SwndFinder.h"
#include "helper/SHostMgr.h"

#include "control/Smessagebox.h"
#include "updatelayeredwindow/SUpdateLayeredWindow.h"
#include "helper/splitstring.h"
#include "res.mgr/SObjDefAttr.h"

#include "core/SSkin.h"
#include "control/souictrls.h"
#include "layout/SouiLayout.h"
#include "layout/SLinearLayout.h"
#include "layout/SGridLayout.h"
#include "animation/SInterpolatorImpl.h"
#include "core/SWndAccessible.h"

#include "msaa/SAccProxyWindow.h"
#include "msaa/SAccProxyCmnCtrl.h"

#include "animation/SAnimation.h"
#include "animation/SAnimationSet.h"
#include "animation/ScaleAnimation.h"
#include "animation/SRotateAnimation.h"
#include "animation/SAlphaAnimation.h"
#include "animation/STranslateAnimation.h"
#include "valueAnimator/SValueAnimator.h"

namespace SOUI
{

class SNullTranslator : public TObjRefImpl<ITranslatorMgr>
{
public:
	STDMETHOD_(void ,SetLanguage)(THIS_ const IStringW * strLang)
	{
	}
	STDMETHOD_(void,GetLanguage)(THIS_ wchar_t szOut[TR_MAX_NAME_LEN]) SCONST
	{
		szOut[0] = 0;
	}
	STDMETHOD_(BOOL,CreateTranslator)(THIS_ ITranslator ** ppTranslator)
	{
		return FALSE;
	}
	STDMETHOD_(BOOL,InstallTranslator)(THIS_ ITranslator * ppTranslator)
	{
		return FALSE;
	}
	STDMETHOD_(BOOL,UninstallTranslator)(THIS_ REFGUID id)
	{
		return FALSE;
	}
	STDMETHOD_(int,tr)(THIS_ const IStringW * strSrc,const IStringW * strCtx,wchar_t *pszOut,int nLen) SCONST
	{
		return 0;
	}
};

class SDefToolTipFactory : public TObjRefImpl<IToolTipFactory>
{
public:
    /*virtual */IToolTip * CreateToolTip(HWND hHost)
    {
        STipCtrl *pTipCtrl = new STipCtrl;
        if(!pTipCtrl->Create())
        {
            delete pTipCtrl;
            return NULL;
        }
        return pTipCtrl;
    }

    /*virtual */void DestroyToolTip(IToolTip *pToolTip)
    {
        if(pToolTip)
        {
            STipCtrl *pTipCtrl= (STipCtrl *)pToolTip;
            pTipCtrl->DestroyWindow();
        }
    }
};

class SDefMsgLoopFactory : public TObjRefImpl<IMsgLoopFactory>
{
public:
    virtual SMessageLoop * CreateMsgLoop()
    {
        return new SMessageLoop;
    }

    virtual void DestoryMsgLoop(SMessageLoop * pMsgLoop)
    {
        delete pMsgLoop;
    }
};

void SObjectDefaultRegister::RegisterWindows(SObjectFactoryMgr *objFactory) const
{
	objFactory->TplRegisterFactory<SWindow>();
	objFactory->TplRegisterFactory<SPanel>();
	objFactory->TplRegisterFactory<SStatic>();
	objFactory->TplRegisterFactory<SButton>();
	objFactory->TplRegisterFactory<SImageWnd>();
	objFactory->TplRegisterFactory<SProgress>();
	objFactory->TplRegisterFactory<SImageButton>();
	objFactory->TplRegisterFactory<SLine>();
	objFactory->TplRegisterFactory<SCheckBox>();
	objFactory->TplRegisterFactory<SIconWnd>();
	objFactory->TplRegisterFactory<SRadioBox>();
	objFactory->TplRegisterFactory<SRadioGroup>();
	objFactory->TplRegisterFactory<SLink>();
	objFactory->TplRegisterFactory<SGroup>();
	objFactory->TplRegisterFactory<SAnimateImgWnd>();
	objFactory->TplRegisterFactory<SScrollView>();
	objFactory->TplRegisterFactory<SRealWnd>();
	objFactory->TplRegisterFactory<SToggle>();
	objFactory->TplRegisterFactory<SCaption>();
	objFactory->TplRegisterFactory<STabCtrl>();
	objFactory->TplRegisterFactory<STabPage>();
	objFactory->TplRegisterFactory<SActiveX>();
	objFactory->TplRegisterFactory<SFlashCtrl>();
	objFactory->TplRegisterFactory<SSplitPane>();
	objFactory->TplRegisterFactory<SSplitWnd>();
	objFactory->TplRegisterFactory<SSplitWnd_Col>();
	objFactory->TplRegisterFactory<SSplitWnd_Row>();
	objFactory->TplRegisterFactory<SSliderBar>();
	objFactory->TplRegisterFactory<STreeCtrl>();
	objFactory->TplRegisterFactory<SScrollBar>();
	objFactory->TplRegisterFactory<SHeaderCtrl>();
	objFactory->TplRegisterFactory<SListCtrl>();
	objFactory->TplRegisterFactory<SListBox>();
	objFactory->TplRegisterFactory<SRichEdit>();
	objFactory->TplRegisterFactory<SEdit>();
	objFactory->TplRegisterFactory<SHotKeyCtrl>();
	objFactory->TplRegisterFactory<SComboBox>();
	objFactory->TplRegisterFactory<SSpinButtonCtrl>();
	objFactory->TplRegisterFactory<SListView>();
	objFactory->TplRegisterFactory<SComboView>();
	objFactory->TplRegisterFactory<SMCListView>();
	objFactory->TplRegisterFactory<STileView>();
	objFactory->TplRegisterFactory<STreeView>();
	objFactory->TplRegisterFactory<SMenuBar>();
	objFactory->TplRegisterFactory<SCalendar>();
	objFactory->TplRegisterFactory<SDateTimePicker>();
}

void SObjectDefaultRegister::RegisterSkins(SObjectFactoryMgr *objFactory)  const
{
	objFactory->TplRegisterFactory<SSkinImgList>();
	objFactory->TplRegisterFactory<SSkinImgCenter>();
	objFactory->TplRegisterFactory<SSkinImgFrame>();
	objFactory->TplRegisterFactory<SSkinImgFrame2>();
	objFactory->TplRegisterFactory<SSkinButton>();
	objFactory->TplRegisterFactory<SSkinGradation>();
	objFactory->TplRegisterFactory<SSkinScrollbar>();
	objFactory->TplRegisterFactory<SSkinColorRect>();
	objFactory->TplRegisterFactory<SSkinShape>();
	objFactory->TplRegisterFactory<SSKinGroup>();
}

void SObjectDefaultRegister::RegisterLayouts(SObjectFactoryMgr *objFactory)  const
{
	objFactory->TplRegisterFactory<SouiLayout>();
	objFactory->TplRegisterFactory<SLinearLayout>();
	objFactory->TplRegisterFactory<SHBox>();
	objFactory->TplRegisterFactory<SVBox>();
	objFactory->TplRegisterFactory<SGridLayout>();
}

void SObjectDefaultRegister::RegisterInterpolator(SObjectFactoryMgr *objFactory)  const
{
	objFactory->TplRegisterFactory<SLinearInterpolator>();
	objFactory->TplRegisterFactory<SAccelerateInterpolator>();
	objFactory->TplRegisterFactory<SDecelerateInterpolator>();
	objFactory->TplRegisterFactory<SAccelerateDecelerateInterpolator>();
	objFactory->TplRegisterFactory<SAnticipateInterpolator>();
	objFactory->TplRegisterFactory<SAnticipateOvershootInterpolator>();
	objFactory->TplRegisterFactory<SBounceInterpolator>();
	objFactory->TplRegisterFactory<SCycleInterpolator>();
	objFactory->TplRegisterFactory<SOvershootInterpolator>();
	objFactory->TplRegisterFactory<SPathInterpolator>();
}

void SObjectDefaultRegister::RegisterAnimation(SObjectFactoryMgr * objFactory) const
{
	objFactory->TplRegisterFactory<SAnimation>();
	objFactory->TplRegisterFactory<SAnimationSet>();
	objFactory->TplRegisterFactory<SAlphaAnimation>();
	objFactory->TplRegisterFactory<SScaleAnimation>();
	objFactory->TplRegisterFactory<SRotateAnimation>();
	objFactory->TplRegisterFactory<STranslateAnimation>();
}
void SObjectDefaultRegister::RegisterValueAnimator(SObjectFactoryMgr * objFactory) const
{
	objFactory->TplRegisterFactory<SIntAnimator>();
	objFactory->TplRegisterFactory<SFloatAnimator>();
	objFactory->TplRegisterFactory<SColorAnimator>();
}
//////////////////////////////////////////////////////////////////////////
// SApplication

template<> SApplication* SSingleton<SApplication>::ms_Singleton = 0;

SApplication::SApplication(IRenderFactory *pRendFactory,HINSTANCE hInst,LPCTSTR pszHostClassName, const ISystemObjectRegister & sysObjRegister,BOOL bImeApp)
    :m_hInst(hInst)
    ,m_RenderFactory(pRendFactory)
    ,m_hMainWnd(NULL)
{
    SWndSurface::Init();
	memset(m_pSingletons, 0, sizeof(m_pSingletons));
    _CreateSingletons(hInst,pszHostClassName,bImeApp);

    m_translator.Attach(new SNullTranslator);
    m_tooltipFactory.Attach(new SDefToolTipFactory);
    m_msgLoopFactory.Attach(new SDefMsgLoopFactory);
    
    SAppDir appDir(hInst);
    m_strAppDir = appDir.AppDir();
    
    m_pMsgLoop = GetMsgLoopFactory()->CreateMsgLoop();
	AddMsgLoop(m_pMsgLoop);
	sysObjRegister.RegisterLayouts(this);
	sysObjRegister.RegisterSkins(this);
	sysObjRegister.RegisterWindows(this);
	sysObjRegister.RegisterInterpolator(this);
	sysObjRegister.RegisterAnimation(this);
	sysObjRegister.RegisterValueAnimator(this);
}

SApplication::~SApplication(void)
{
	RemoveMsgLoop();
    GetMsgLoopFactory()->DestoryMsgLoop(m_pMsgLoop);
    
	SResProviderMgr::RemoveAll();
    _DestroySingletons();
}

void SApplication::_CreateSingletons(HINSTANCE hInst,LPCTSTR pszHostClassName,BOOL bImeApp)
{
	m_pSingletons[SUiDef::GetType()] = new SUiDef();
	m_pSingletons[SWindowMgr::GetType()] = new SWindowMgr();
	m_pSingletons[STimer2::GetType()] = new STimer2();
	m_pSingletons[SScriptTimer::GetType()] = new SScriptTimer();
	m_pSingletons[SFontPool::GetType()] = new SFontPool(m_RenderFactory);
	m_pSingletons[SSkinPoolMgr::GetType()] =  new SSkinPoolMgr();
	m_pSingletons[SStylePoolMgr::GetType()] =  new SStylePoolMgr();
	m_pSingletons[STemplatePoolMgr::GetType()] = new STemplatePoolMgr();
	m_pSingletons[SWindowFinder::GetType()] = new SWindowFinder();
	m_pSingletons[STextServiceHelper::GetType()] = new STextServiceHelper();
	m_pSingletons[SRicheditMenuDef::GetType()] = new SRicheditMenuDef();
	m_pSingletons[SNativeWndHelper::GetType()] =   new SNativeWndHelper(hInst, pszHostClassName, bImeApp);
	m_pSingletons[SHostMgr::GetType()] =   new SHostMgr();
}

#define DELETE_SINGLETON(x) \
 delete (x*)m_pSingletons[x::GetType()];\
 m_pSingletons[x::GetType()] = NULL;

void SApplication::_DestroySingletons()
{
	DELETE_SINGLETON(SHostMgr);
	DELETE_SINGLETON(SNativeWndHelper);
	DELETE_SINGLETON(SRicheditMenuDef);
	DELETE_SINGLETON(STextServiceHelper);
	DELETE_SINGLETON(SWindowFinder);
	DELETE_SINGLETON(SStylePoolMgr);
	DELETE_SINGLETON(STemplatePoolMgr);
	DELETE_SINGLETON(SSkinPoolMgr);
	DELETE_SINGLETON(SFontPool);
	DELETE_SINGLETON(SScriptTimer);
	DELETE_SINGLETON(STimer2);
	DELETE_SINGLETON(SWindowMgr);
	DELETE_SINGLETON(SUiDef);
}


IAccProxy * SApplication::CreateAccProxy(SWindow* pWnd) const
{
#ifdef SOUI_ENABLE_ACC
	if(pWnd->IsClass(SProgress::GetClassName()))
	{
		return new SAccProxyProgress(pWnd);
	}else if(pWnd->IsClass(SSliderBar::GetClassName()))
	{
		return new SAccProxySlideBar(pWnd);
	}else if(pWnd->IsClass(SButton::GetClassName()) || pWnd->IsClass(SImageButton::GetClassName()))
	{
		return new SAccProxyButton(pWnd);
	}else if(pWnd->IsClass(SComboBox::GetClassName()))
	{
		return new SAccProxyCombobox(pWnd);
	}else if(pWnd->IsClass(SRichEdit::GetClassName()))
	{
		return new SAccProxyEdit(pWnd);
	}else if(pWnd->IsClass(SCheckBox::GetClassName()))
	{
		return new SAccProxyCheckButton(pWnd);
	}else if(pWnd->IsClass(SRadioBox::GetClassName()))
	{
		return new SAccProxyRadioButton(pWnd);
	}

	return new SAccProxyWindow(pWnd);
#else
	return NULL;
#endif//SOUI_ENABLE_ACC
}

IAccessible * SApplication::CreateAccessible(SWindow *pWnd) const
{
#ifdef SOUI_ENABLE_ACC
	return new SAccessible(pWnd);
#else
	return NULL;
#endif//SOUI_ENABLE_ACC
}


void * SApplication::GetInnerSingleton(int nType)
{
	if (nType < 0 || nType >= SINGLETON_COUNT)
		return NULL;
	return m_pSingletons[nType];
}

BOOL SApplication::_LoadXmlDocment( LPCTSTR pszXmlName ,LPCTSTR pszType ,SXmlDoc & xmlDoc,IResProvider *pResProvider/* = NULL*/)
{
    if(!pResProvider) 
    {
        if(IsFileType(pszType))
        {
            bool bLoad = xmlDoc.load_file(pszXmlName,xml_parse_default,enc_auto);
			if(!bLoad)
			{
				XmlParseResult res;
				xmlDoc.GetParseResult(&res);
				SASSERT_FMTW(bLoad,L"parse xml error! xmlName=%s,desc=%s,offset=%d",pszXmlName,SXmlDoc::GetErrDesc(res.status),res.offset);
			}
            return bLoad;
        }else
        {
            pResProvider = GetMatchResProvider(pszType,pszXmlName);
        }
    }
    if(!pResProvider) return FALSE;
    
    size_t dwSize=pResProvider->GetRawBufferSize(pszType,pszXmlName);
    if(dwSize==0) return FALSE;

    SAutoBuf strXml;
    strXml.Allocate(dwSize);
    pResProvider->GetRawBuffer(pszType,pszXmlName,strXml,dwSize);

    bool  bLoad = xmlDoc.load_buffer(strXml,strXml.size(),xml_parse_default,enc_auto);
	if(!bLoad)
	{
		XmlParseResult result;
		xmlDoc.GetParseResult(&result);
		SASSERT_FMTW(bLoad,L"parse xml error! xmlName=%s,desc=%s,offset=%d",pszXmlName,S_CA2W(SXmlDoc::GetErrDesc(result.status)),result.offset);
	}
    return bLoad;
}


BOOL SApplication::LoadXmlDocment(SXmlDoc & xmlDoc, const SStringT & strResId)
{
    SStringTList strLst;
    if(2!=ParseResID(strResId,strLst)) return FALSE;
    return _LoadXmlDocment(strLst[1],strLst[0],xmlDoc);
}

IAnimation * SApplication::LoadAnimation(const SStringT &strResId)
{
	SXmlDoc xml;
	if (!LoadXmlDocment(xml, strResId))
		return NULL;
	IAnimation *pRet = CreateAnimationByName(xml.root().first_child().name());
	if (!pRet)
		return NULL;
	pRet->InitFromXml(&xml.root().first_child());
	return pRet;
}

IValueAnimator * SApplication::LoadValueAnimator(const SStringT & strResId)
{
	SXmlDoc xml;
	if (!LoadXmlDocment(xml, strResId))
		return NULL;
	IValueAnimator *pRet = CreateValueAnimatorByName(xml.root().first_child().name());
	if (!pRet)
		return NULL;
	pRet->InitFromXml(&xml.root().first_child());
	return pRet;
}

UINT SApplication::LoadSystemNamedResource( IResProvider *pResProvider )
{
    UINT uRet=0;
    AddResProvider(pResProvider,NULL);
    //load system skins
    {
        SXmlDoc xmlDoc;
        if(_LoadXmlDocment(_T("SYS_XML_SKIN"),_T("XML"),xmlDoc,pResProvider))
        {
            SSkinPool * p= SSkinPoolMgr::getSingletonPtr()->GetBuiltinSkinPool();
            p->LoadSkins(xmlDoc.root().child(L"skin"));
        }else
        {
            uRet |= 0x01;
        }
    }
    //load edit context menu
    {
        SXmlDoc xmlDoc;
        if(_LoadXmlDocment(_T("SYS_XML_EDITMENU"),_T("XML"),xmlDoc,pResProvider))
        {
            SRicheditMenuDef::getSingleton().SetMenuXml(xmlDoc.root().child(L"editmenu"));
        }else
        {
            uRet |= 0x02;
        }
    }
    //load messagebox template
    {
        SXmlDoc xmlDoc;
        if(!_LoadXmlDocment(_T("SYS_XML_MSGBOX"),_T("XML"),xmlDoc,pResProvider)
        || !SetMsgTemplate(xmlDoc.root().child(L"SOUI")))
        {
            uRet |= 0x04;
        }
    }
    RemoveResProvider(pResProvider);
    return uRet;
}

int SApplication::Run( HWND hMainWnd )
{
    m_hMainWnd = hMainWnd;
    int nRet = m_pMsgLoop->Run();
	if(::IsWindow(m_hMainWnd))
	{
		DestroyWindow(m_hMainWnd);
	}
	return nRet;
}

HMODULE SApplication::GetModule() const
{
	return m_hInst;
}

void SApplication::SetTranslator(ITranslatorMgr * pTrans)
{
	m_translator = pTrans;
}

ITranslatorMgr * SApplication::GetTranslator()
{
	return m_translator;
}

void SApplication::SetScriptFactory(IScriptFactory *pScriptFactory)
{
	m_pScriptFactory = pScriptFactory;
}


HRESULT SApplication::CreateScriptModule( IScriptModule **ppScriptModule )
{
    if(!m_pScriptFactory) return E_FAIL;
    return m_pScriptFactory->CreateScriptModule(ppScriptModule);
}

IRenderFactory * SApplication::GetRenderFactory()
{
	return m_RenderFactory;
}

void SApplication::SetRealWndHandler( IRealWndHandler *pRealHandler )
{
    m_pRealWndHandler = pRealHandler;
}

IRealWndHandler * SApplication::GetRealWndHander()
{
    return m_pRealWndHandler;
}

IToolTipFactory * SApplication::GetToolTipFactory()
{
    return m_tooltipFactory;
}

void SApplication::SetToolTipFactory( IToolTipFactory* pToolTipFac )
{
    m_tooltipFactory = pToolTipFac;
}

HWND SApplication::GetMainWnd()
{
    return m_hMainWnd;
}

BOOL SApplication::SetMsgLoopFactory(IMsgLoopFactory *pMsgLoopFac)
{
	if(m_msgLoopMap.GetCount()>0)
		return FALSE;
    m_msgLoopFactory->DestoryMsgLoop(m_pMsgLoop);
    m_msgLoopFactory = pMsgLoopFac;
    m_pMsgLoop = m_msgLoopFactory->CreateMsgLoop();
    return TRUE;
}

IMsgLoopFactory * SApplication::GetMsgLoopFactory()
{
    return m_msgLoopFactory;
}

void SApplication::InitXmlNamedID(const SNamedID::NAMEDVALUE *pNamedValue,int nCount,BOOL bSorted)
{
    m_namedID.Init2(pNamedValue,nCount,bSorted);
}

int SApplication::Str2ID(const SStringW & str)
{
    return m_namedID.String2Value(str);
}


SStringW SApplication::tr(const SStringW & strSrc,const SStringW & strCtx) const
{
	int nRet = m_translator->tr(&strSrc,&strCtx,NULL,0);
	if(nRet == 0) return strSrc;
	SStringW strRet;
	wchar_t *pBuf = strRet.GetBufferSetLength(nRet-1);
	m_translator->tr(&strSrc,&strCtx,pBuf,nRet);
	strRet.ReleaseBuffer();
	return strRet;
}

SWindow * SApplication::CreateWindowByName(LPCWSTR pszWndClass) const
{//支持使用类似button.ok这样的控件名来创建控件，对于这种格式自动应用button.ok为class属性.
	SStringW strClsName = pszWndClass;
	int nPos = strClsName.ReverseFind(L'.');
	if (nPos != -1) strClsName = strClsName.Left(nPos);
	SWindow *pRet = (SWindow*)CreateObject(SObjectInfo(strClsName, Window));
	if (pRet && nPos != -1)
	{
		pRet->SetAttribute(L"class", pszWndClass, TRUE);
	}
	return pRet;
}

ISkinObj * SApplication::CreateSkinByName(LPCWSTR pszSkinClass) const
{
	return (ISkinObj*)CreateObject(SObjectInfo(pszSkinClass, Skin));
}

IInterpolator * SApplication::CreateInterpolatorByName(LPCWSTR pszName) const
{
	return (IInterpolator*)CreateObject(SObjectInfo(pszName, Interpolator));
}

IAnimation * SApplication::CreateAnimationByName(LPCWSTR pszName) const
{
	return (IAnimation*)CreateObject(SObjectInfo(pszName, Animation));
}

IValueAnimator * SApplication::CreateValueAnimatorByName(LPCWSTR pszName) const
{
	return (IValueAnimator*)CreateObject(SObjectInfo(pszName, ValueAnimator));
}

void SApplication::SetLogManager(ILog4zManager * pLogMgr)
{
    m_logManager = pLogMgr;
}

ILog4zManager * SApplication::GetLogManager()
{
    return m_logManager;
}

SStringT SApplication::GetAppDir() const
{
	return m_strAppDir;
}

void SApplication::SetAppDir(const SStringT & strAppDir)
{
	m_strAppDir = strAppDir;
}

IAttrStorageFactory * SApplication::GetAttrStorageFactory()
{
	return m_pAttrStroageFactory;
}

void SApplication::SetAttrStorageFactory(IAttrStorageFactory * pAttrStorageFactory)
{
	m_pAttrStroageFactory = pAttrStorageFactory;
}


bool SApplication::AddMsgLoop(IMessageLoop* pMsgLoop)
{
	SAutoLock autoLock(m_cs);
	SASSERT(pMsgLoop != NULL);
	DWORD dwThreadID= ::GetCurrentThreadId();
	if(m_msgLoopMap.Lookup(dwThreadID) != NULL)
		return false;// in map yet

	m_msgLoopMap[dwThreadID]= pMsgLoop;
	return true;
}

bool SApplication::RemoveMsgLoop()
{
	SAutoLock autoLock(m_cs);
	return m_msgLoopMap.RemoveKey(::GetCurrentThreadId());
}

IMessageLoop* SApplication::GetMsgLoop(DWORD dwThreadID /*= ::GetCurrentThreadId()*/) const
{
	SAutoLock autoLock(m_cs);
	const SMap<DWORD,IMessageLoop*>::CPair * p =  m_msgLoopMap.Lookup(dwThreadID);
	if(!p) return NULL;
	return p->m_value;
}


}//namespace SOUI