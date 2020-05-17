/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       sapp.h
 * @brief      
 * @version    v3.0      
 * @author     soui      
 * @date       2020-05-17
 * 
 * Describe    SOUI应用程序入口 
 */

#pragma once
#include <core/ssingleton.h>
#include <interface/sapp-i.h>
#include <interface/SRender-i.h>
#include <interface/SScriptModule-i.h>
#include <interface/STranslator-i.h>
#include <interface/STooltip-i.h>
#include <interface/SLog-i.h>
#include <interface/SAttrStorage-i.h>
#include <interface/SInterpolator-i.h>
#include <interface/SAnimation-i.h>
#include <interface/SValueAnimator-i.h>
#include <control/SRealWndHandler-i.h>

#include <res.mgr/SResProviderMgr.h>
#include <res.mgr/SNamedValue.h>

#include <core/smsgloop.h>
#include <core/SObjectFactory.h>
#include <OleAcc.h>
#include <helper/obj-ref-impl.hpp>

#define GETRESPROVIDER      SOUI::SApplication::getSingletonPtr()
#define GETRENDERFACTORY    SOUI::SApplication::getSingleton().GetRenderFactory()
#define GETREALWNDHANDLER   SOUI::SApplication::getSingleton().GetRealWndHander()
#define GETTOOLTIPFACTORY   SOUI::SApplication::getSingleton().GetToolTipFactory()

#define LOADXML(p1,p2)   SOUI::SApplication::getSingleton().LoadXmlDocment(p1,p2)
#define LOADIMAGE(p1,p2)    SOUI::SApplication::getSingleton().LoadImage(p1,p2)
#define LOADIMAGE2(p1)      SOUI::SApplication::getSingleton().LoadImage2(p1)
#define LOADICON(p1,p2)     SOUI::SApplication::getSingleton().LoadIcon(p1,p2,p2)
#define LOADICON2(p1)       SOUI::SApplication::getSingleton().LoadIcon2(p1)
#define TR(p1,p2)           SOUI::SApplication::getSingleton().tr(p1,p2)
#define STR2ID(p1)          SOUI::SApplication::getSingleton().Str2ID(p1)

#define GETCOLOR(x)         SOUI::SApplication::getSingleton().GetColor(x)
#define GETSTRING(x)        SOUI::SApplication::getSingleton().GetString(x)
#define GETLAYOUTSIZE(x)    SOUI::SApplication::getSingleton().GetLayoutSize(x)

#define CREATEINTERPOLATOR(x)  SOUI::SApplication::getSingleton().CreateInterpolatorByName(x)

namespace SOUI
{
struct IAccProxy;


interface SOUI_EXP ISystemObjectRegister
{
	virtual void RegisterLayouts(SObjectFactoryMgr *objFactory) const {}
	virtual void RegisterSkins(SObjectFactoryMgr *objFactory)  const {}
	virtual void RegisterWindows(SObjectFactoryMgr *objFactory)  const {}
	virtual void RegisterInterpolator(SObjectFactoryMgr *objFactory) const {}
	virtual void RegisterAnimation(SObjectFactoryMgr *objFactory) const {}
	virtual void RegisterValueAnimator(SObjectFactoryMgr *objFactory) const {}
};

class SOUI_EXP SObjectDefaultRegister : public ISystemObjectRegister
{
public:
	void RegisterWindows(SObjectFactoryMgr *objFactory) const;
	void RegisterSkins(SObjectFactoryMgr *objFactory) const;
	void RegisterLayouts(SObjectFactoryMgr *objFactory) const;
	void RegisterInterpolator(SObjectFactoryMgr *objFactory) const;
	void RegisterAnimation(SObjectFactoryMgr *objFactory) const;
	void RegisterValueAnimator(SObjectFactoryMgr *objFactory) const;
};


/** 
 * @class     SApplication
 * @brief     SOUI Application
 *
 * Describe   SOUI Application
 */
class SOUI_EXP SApplication :public SSingleton<SApplication>
							,public TObjRefImpl<IApplication>
                            ,public SResProviderMgr
	                        ,public SObjectFactoryMgr
{
public:
    /**
     * SApplication
     * @brief    构造函数
     * @param    IRenderFactory * pRendFactory --  渲染模块
     * @param    HINSTANCE hInst --  应用程序句柄
     * @param    LPCTSTR pszHostClassName --  使用SOUI创建窗口时默认的窗口类名
	 * @param    ISystemObjectRegister *pSysObjRegister -- 系统控件注册器
     *
     * Describe  
     */
    SApplication(IRenderFactory *pRendFactory,HINSTANCE hInst,LPCTSTR pszHostClassName = _T("SOUIHOST"), const ISystemObjectRegister & sysObjRegister = SObjectDefaultRegister(), BOOL bImeApp = FALSE);

    ~SApplication(void);

public:
	STDMETHOD_(HMODULE,GetModule)(THIS) SCONST OVERRIDE;
	STDMETHOD_(UINT,LoadSystemNamedResource)(THIS_ IResProvider *pResProvider) OVERRIDE;
	STDMETHOD_(ITranslatorMgr *,GetTranslator)(THIS) OVERRIDE;
	STDMETHOD_(void,SetTranslator)(THIS_ ITranslatorMgr * pTrans) OVERRIDE;
	STDMETHOD_(IToolTipFactory *, GetToolTipFactory)(THIS) OVERRIDE;
	STDMETHOD_(void,SetToolTipFactory)(THIS_ IToolTipFactory* pToolTipFac) OVERRIDE; 

	STDMETHOD_(BOOL,SetMsgLoopFactory)(THIS_ IMsgLoopFactory *pMsgLoopFac) OVERRIDE;
	STDMETHOD_(IMsgLoopFactory *,GetMsgLoopFactory)(THIS) OVERRIDE;

	STDMETHOD_(void,SetLogManager)(THIS_ ILog4zManager * pLogMgr) OVERRIDE;
	STDMETHOD_(ILog4zManager *,GetLogManager)(THIS) OVERRIDE;

	STDMETHOD_(void,SetAttrStorageFactory)(THIS_ IAttrStorageFactory * pAttrStorageFactory) OVERRIDE;
	STDMETHOD_(IAttrStorageFactory*,GetAttrStorageFactory)(THIS) OVERRIDE;

	STDMETHOD_(int,Run)(THIS_ HWND hMainWnd) OVERRIDE;
	STDMETHOD_(HWND,GetMainWnd)(THIS) OVERRIDE;

	STDMETHOD_(BOOL,AddMsgLoop)(THIS_ IMessageLoop* pMsgLoop) OVERRIDE;
	STDMETHOD_(BOOL,RemoveMsgLoop)(THIS) OVERRIDE;

	STDMETHOD_(IMessageLoop*,GetMsgLoop)(THIS_ DWORD dwThreadID = ::GetCurrentThreadId()) SCONST OVERRIDE;
    
public:
    /**
     * Init
     * @brief    从数组里初始化命名ID列表
     * @param    SNamedID::NAMEDVALUE *pValue --  数组
     * @param    int nCount --  数组长度
     * @param    BOOL bSorted -- 数组关键字从小到大已经有序
     *
     * Describe  SNamedID::NAMEDVALUE应该是由uiresbuilder自动生成的列表数据，不要手动修改
     */
    void InitXmlNamedID(const SNamedID::NAMEDVALUE *pNamedValue,int nCount,BOOL bSorted);
    
    
    /**
     * LoadXmlDocment
     * @brief    从资源中加载一个XML Document。
     * @param [out] SXmlDoc & xmlDoc --  输出的xml_document对象
     * @param    const SStringT & strResId --  XML文件在资源中的type:name
     * @return   BOOL true-加载成功, false-加载失败
     *
     * Describe  
     */
    BOOL LoadXmlDocment(SXmlDoc & xmlDoc,const SStringT & strResId);

	IAnimation * LoadAnimation(const SStringT &strResId);

	IValueAnimator *LoadValueAnimator(const SStringT &strResId);
    /**
     * GetRenderFactory
     * @brief    获得当前的渲染模块
     * @return   IRenderFactory * 渲染模块指针
     *
     * Describe  
     */
    IRenderFactory * GetRenderFactory();
    
    /**
     * GetScriptModule
     * @brief    创建脚本模块对象
     * @param [out] IScriptModule **ppScriptModule -- 脚本模块对象
     * @return   HRESULT -- S_OK 创建成功
     *
     * Describe  
     */
    HRESULT CreateScriptModule(IScriptModule **ppScriptModule);

    /**
     * SetScriptModule
     * @brief    设置SOUI中使用的脚本模块类厂
     * @param    IScriptFactory *pScriptModule --  脚本模块类厂
     * @return   void 
     *
     * Describe  
     */
    void SetScriptFactory(IScriptFactory *pScriptModule);
    
        

    
    /**
     * GetRealWndHander
     * @brief    获得RealWndHander
     * @return   IRealWndHandler * -- RealWndHander
     * Describe  
     */    
    IRealWndHandler * GetRealWndHander();

    /**
     * SetRealWndHandler
     * @brief    设置RealWnd处理接口
     * @param    IRealWndHandler * pRealHandler --  RealWnd处理接口
     * @return   void
     * Describe  
     */    
    void SetRealWndHandler(IRealWndHandler *pRealHandler);

    void SetAppDir(const SStringT & strAppDir);

    SStringT GetAppDir()const;
    
    
    template<class T>
    bool RegisterWindowClass()
    {
		if (T::GetClassType() != Window) return false;
        return TplRegisterFactory<T>();
    }
    
    template<class T>
    bool UnregisterWindowClass()
    {
		if (T::GetClassType() != Window) return false;
        return TplUnregisterFactory<T>();
    }
    template<class T>
    bool RegisterSkinClass()
    {
		if (T::GetClassType() != Skin) return false;
		return TplRegisterFactory<T>();
	}
    
    template<class T>
    bool UnregisterSkinClass()
    {
		if (T::GetClassType() != Skin) return false;
		return TplUnregisterFactory<T>();
	}
    
    int Str2ID(const SStringW & str);

	SStringW tr(const SStringW & strSrc,const SStringW & strCtx) const;
	virtual SWindow * CreateWindowByName(LPCWSTR pszWndClass) const;
	virtual ISkinObj * CreateSkinByName(LPCWSTR pszSkinClass) const;
	virtual IInterpolator * CreateInterpolatorByName(LPCWSTR pszName) const;
	virtual IAnimation * CreateAnimationByName(LPCWSTR pszName) const;
	virtual IValueAnimator *CreateValueAnimatorByName(LPCWSTR pszName) const;

	virtual IAccProxy * CreateAccProxy(SWindow* pWnd) const;
	virtual IAccessible * CreateAccessible(SWindow *pWnd) const;

	void * GetInnerSingleton(int nType);

protected:
	virtual void RegisterSystemObjects(){}

    void _CreateSingletons(HINSTANCE hInst, LPCTSTR pszHostClassName, BOOL bImeApp);
    void _DestroySingletons();
    BOOL _LoadXmlDocment(LPCTSTR pszXmlName ,LPCTSTR pszType ,SXmlDoc & xmlDoc,IResProvider *pResProvider = NULL);
    
    SAutoRefPtr<IRealWndHandler>    m_pRealWndHandler;
    SAutoRefPtr<IScriptFactory>     m_pScriptFactory;
    SAutoRefPtr<IRenderFactory>     m_RenderFactory;
    SAutoRefPtr<ITranslatorMgr>     m_translator;
    SAutoRefPtr<IToolTipFactory>    m_tooltipFactory;
    SAutoRefPtr<IMsgLoopFactory>    m_msgLoopFactory;
    SAutoRefPtr<ILog4zManager>      m_logManager;   //log manager
	SAutoRefPtr<IAttrStorageFactory> m_pAttrStroageFactory;

    SNamedID                        m_namedID;
    
    SStringT    m_strAppDir;
    HMODULE		m_hInst;
    HWND        m_hMainWnd;

	mutable SCriticalSection	m_cs;
	SMap<DWORD,IMessageLoop * > m_msgLoopMap;
	IMessageLoop *		 m_pMsgLoop;
	//一组单例指针
	void * m_pSingletons[SINGLETON_COUNT];
};

}//namespace SOUI