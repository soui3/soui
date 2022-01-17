#pragma once

#include <interface/obj-ref-i.h>
#include <interface/STaskLoop-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE IMessageFilter
DECLARE_INTERFACE(IMessageFilter)
{
	STDMETHOD_(BOOL,PreTranslateMessage)(THIS_ MSG* pMsg) PURE;
};

#undef INTERFACE
#define INTERFACE IIdleHandler
DECLARE_INTERFACE(IIdleHandler)
{
	STDMETHOD_(BOOL,OnIdle)(THIS) PURE;
};


#undef INTERFACE
#define INTERFACE IMessageLoop
DECLARE_INTERFACE_(IMessageLoop,IObjRef)
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

	// Message filter operations
	STDMETHOD_(BOOL,AddMessageFilter)(THIS_ IMessageFilter* pMessageFilter) PURE;

	STDMETHOD_(BOOL,RemoveMessageFilter)(THIS_ IMessageFilter* pMessageFilter) PURE;

	// Idle handler operations
	STDMETHOD_(BOOL,AddIdleHandler)(THIS_ IIdleHandler* pIdleHandler) PURE;

	STDMETHOD_(BOOL,RemoveIdleHandler)(THIS_ IIdleHandler* pIdleHandler) PURE;

	// Override to change message filtering
	STDMETHOD_(BOOL,PreTranslateMessage)(THIS_ MSG* pMsg) PURE;

	// override to change idle processing
	STDMETHOD_(BOOL,OnIdle)(THIS_ int /*nIdleCount*/) PURE;

	STDMETHOD_(int,Run)(THIS) PURE;

	STDMETHOD_(BOOL,IsRunning)(THIS) const PURE;

	STDMETHOD_(void,OnMsg)(THIS_ LPMSG pMsg) PURE;

	STDMETHOD_(void,Quit)(THIS) PURE;

	STDMETHOD_(BOOL,PostTask)(THIS_ IRunnable * runable) PURE;

	STDMETHOD_(int ,RemoveTasksForObject)(THIS_ void *pObj) PURE;

};


#undef INTERFACE
#define INTERFACE IMsgLoopFactory
DECLARE_INTERFACE_(IMsgLoopFactory,IObjRef)
{
	STDMETHOD_(HRESULT,CreateMsgLoop)(THIS_ IMessageLoop ** ppMsgLoop) PURE;
};

SNSEND