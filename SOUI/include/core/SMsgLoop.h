#pragma once

#include <interface/STaskLoop-i.h>
#include <helper/SCriticalSection.h>

#ifndef WM_SYSTIMER
#define WM_SYSTIMER 0x0118   //(caret blink)
#endif//WM_SYSTIMER

namespace SOUI
{
    struct IMessageFilter
    {
        virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CIdleHandler - Interface for idle processing

    struct IIdleHandler
    {
        virtual BOOL OnIdle() = 0;
    };

    class SOUI_EXP SMessageLoop
    {
    public:
        SMessageLoop();
        virtual ~SMessageLoop(){}
        
        SArray<IMessageFilter*> m_aMsgFilter;
        SArray<IIdleHandler*> m_aIdleHandler;
        MSG m_msg;
                
        // Message filter operations
        BOOL AddMessageFilter(IMessageFilter* pMessageFilter);

        BOOL RemoveMessageFilter(IMessageFilter* pMessageFilter);

        // Idle handler operations
        BOOL AddIdleHandler(IIdleHandler* pIdleHandler);

        BOOL RemoveIdleHandler(IIdleHandler* pIdleHandler);

        static BOOL IsIdleMessage(MSG* pMsg);

        // Overrideables
        // Override to change message filtering
        virtual BOOL PreTranslateMessage(MSG* pMsg);

        // override to change idle processing
        virtual BOOL OnIdle(int /*nIdleCount*/);
        
        virtual int Run();
        
        virtual BOOL IsRunning() const{return m_bRunning;}
        
        virtual void OnMsg(LPMSG pMsg);
        
		virtual void Quit();

		virtual BOOL PostTask(const IRunnable & runable);
		virtual int  RemoveTasksForObject(void *pObj);
    protected:
        BOOL m_bRunning;
		BOOL m_bQuit;
		SCriticalSection m_cs;
		SList<IRunnable*>	m_runnables;
		SCriticalSection m_csRunningQueue;
		SList<IRunnable*>	m_runningQueue;
		DWORD  m_tid;
    };


}//end of namespace SOUI
