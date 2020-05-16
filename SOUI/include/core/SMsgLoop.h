#pragma once
#include <interface/smsgloop-i.h>
#include <souicoll.h>

SNSBEGIN

class SOUI_EXP SMessageLoop : public IMessageLoop
{
public:
	SMessageLoop():m_bRunning(FALSE)
	{

	}
	virtual ~SMessageLoop(){}

public:
	// Message filter operations
	STDMETHOD_(BOOL,AddMessageFilter)(THIS_ IMessageFilter* pMessageFilter) OVERRIDE;

	STDMETHOD_(BOOL,RemoveMessageFilter)(THIS_ IMessageFilter* pMessageFilter) OVERRIDE;

	// Idle handler operations
	STDMETHOD_(BOOL,AddIdleHandler)(THIS_ IIdleHandler* pIdleHandler) OVERRIDE;

	STDMETHOD_(BOOL,RemoveIdleHandler)(THIS_ IIdleHandler* pIdleHandler) OVERRIDE;

	// Override to change message filtering
	STDMETHOD_(BOOL,PreTranslateMessage)(THIS_ MSG* pMsg) OVERRIDE;

	// override to change idle processing
	STDMETHOD_(BOOL,OnIdle)(THIS_ int /*nIdleCount*/) OVERRIDE;

	STDMETHOD_(int,Run)(THIS) OVERRIDE;

	STDMETHOD_(BOOL,IsRunning)(THIS) const OVERRIDE;

	STDMETHOD_(void,OnMsg)(THIS_ LPMSG pMsg) OVERRIDE;

	STDMETHOD_(void,Quit)(THIS) OVERRIDE;
public:
	static BOOL IsIdleMessage( MSG* pMsg );
protected:
	BOOL m_bRunning;
	BOOL m_bQuit;

	SArray<IMessageFilter*> m_aMsgFilter;
	SArray<IIdleHandler*> m_aIdleHandler;
	MSG m_msg;
};


SNSEND