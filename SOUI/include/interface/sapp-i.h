#pragma once
#include <interface/obj-ref-i.h>
#include <interface/SResProvider-i.h>
#include <interface/STranslator-i.h>
#include <interface/stooltip-i.h>
#include <interface/slog-i.h>
#include <interface/SAttrStorage-i.h>
#include <interface/smsgloop-i.h>
#include <interface/shostwnd-i.h>
#include <interface/SResProviderMgr-i.h>
#include <interface/SRealWndHandler-i.h>
#include <interface/SRender-i.h>
SNSBEGIN


#undef INTERFACE
#define INTERFACE IApplication
DECLARE_INTERFACE_(IApplication,IObjRef)
{
	STDMETHOD_(HMODULE,GetModule)(THIS) SCONST PURE;
	STDMETHOD_(UINT,LoadSystemNamedResource)(THIS_ IResProvider *pResProvider) PURE;
	STDMETHOD_(ITranslatorMgr *,GetTranslator)(THIS) PURE;
	STDMETHOD_(void,SetTranslator)(THIS_ ITranslatorMgr * pTrans) PURE;
    STDMETHOD_(IToolTipFactory *, GetToolTipFactory)(THIS) PURE;
    STDMETHOD_(void,SetToolTipFactory)(THIS_ IToolTipFactory* pToolTipFac) PURE; 

    STDMETHOD_(BOOL,SetMsgLoopFactory)(THIS_ IMsgLoopFactory *pMsgLoopFac) PURE;
    STDMETHOD_(IMsgLoopFactory *,GetMsgLoopFactory)(THIS) PURE;
    
    STDMETHOD_(void,SetLogManager)(THIS_ ILog4zManager * pLogMgr) PURE;
    STDMETHOD_(ILog4zManager *,GetLogManager)(THIS) PURE;
    
	STDMETHOD_(void,SetAttrStorageFactory)(THIS_ IAttrStorageFactory * pAttrStorageFactory) PURE;
	STDMETHOD_(IAttrStorageFactory*,GetAttrStorageFactory)(THIS) PURE;

    STDMETHOD_(int,Run)(THIS_ HWND hMainWnd) PURE;
	STDMETHOD_(HWND,GetMainWnd)(THIS) PURE;

	STDMETHOD_(BOOL,AddMsgLoop)(THIS_ IMessageLoop* pMsgLoop) PURE;
	STDMETHOD_(BOOL,RemoveMsgLoop)(THIS) PURE;

	STDMETHOD_(IMessageLoop*,GetMsgLoop)(THIS_ DWORD dwThreadID) SCONST PURE;

	STDMETHOD_(IResProviderMgr*,GetResProviderMgr)(THIS) PURE;

	/**
     * GetRealWndHander
     * @brief    获得RealWndHander
     * @return   IRealWndHandler * -- RealWndHander
     * Describe  
     */    	    
    STDMETHOD_(IRealWndHandler *, GetRealWndHander)(THIS) PURE;

    /**
     * SetRealWndHandler
     * @brief    设置RealWnd处理接口
     * @param    IRealWndHandler * pRealHandler --  RealWnd处理接口
     * @return   void
     * Describe  
     */    
    STDMETHOD_(void,SetRealWndHandler)(THIS_ IRealWndHandler *pRealHandler) PURE;

	/**
     * GetRenderFactory
     * @brief    获得当前的渲染模块
     * @return   IRenderFactory * 渲染模块指针
     *
     * Describe  
     */
    STDMETHOD_(IRenderFactory *, GetRenderFactory)(THIS) PURE;
};

typedef IApplication * IApplicationPtr;

SNSEND