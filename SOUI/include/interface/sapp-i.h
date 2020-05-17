#pragma once
#include <interface/obj-ref-i.h>
#include <interface/SResProvider-i.h>
#include <interface/STranslator-i.h>
#include <interface/stooltip-i.h>
#include <interface/slog-i.h>
#include <interface/SAttrStorage-i.h>
#include <interface/smsgloop-i.h>

SNSBEGIN


#undef INTERFACE
#define INTERFACE IApplication
DECLARE_INTERFACE_(IApplication,IObjRef)
{
	//!添加引用
	/*!
	*/
	STDMETHOD_(long,AddRef) (THIS) PURE;

	//!释放引用
	/*!
	*/
	STDMETHOD_(long,Release) (THIS) PURE;

	//!释放对象
	/*!
	*/
	STDMETHOD_(void,OnFinalRelease) (THIS) PURE;

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

	STDMETHOD_(IMessageLoop*,GetMsgLoop)(THIS_ DWORD dwThreadID = ::GetCurrentThreadId()) SCONST PURE;

};

SNSEND