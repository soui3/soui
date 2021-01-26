#include "souistd.h"
#include "core/SMsgLoop.h"

namespace SOUI
{
	template<class T>
	BOOL RemoveElementFromArray(SArray<T> &arr, T ele)
	{
		for(size_t i=0;i<arr.GetCount();i++)
		{
			if(arr[i] == ele)
			{
				arr.RemoveAt(i);
				return TRUE;
			}
		}
		return FALSE;
	}

	SMessageLoop::SMessageLoop() :m_bRunning(FALSE),m_tid(0)
	{

	}



    void SMessageLoop::OnMsg(LPMSG pMsg)
    {
        if(!PreTranslateMessage(pMsg))
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);
        }
    }

	void SMessageLoop::Quit()
	{
		if (!m_bRunning) return;
		m_bQuit = TRUE;
	}

    int SMessageLoop::Run()
    {
        BOOL bDoIdle = TRUE;
        int nIdleCount = 0;
        BOOL bRet;
        
        m_bRunning = TRUE;
		m_tid = GetCurrentThreadId();
		m_bQuit = FALSE;
        for(;;)
        {
			{
				m_cs.Enter();
				SList<IRunnable *> runnable;
				runnable.Copy(m_runnables);
				m_runnables.RemoveAll();
				m_cs.Leave();
				SPOSITION pos = runnable.GetHeadPosition();
				while(pos)
				{
					IRunnable *pRunnable = runnable.GetNext(pos);
					pRunnable->run();
					pRunnable->destroy();
				}
                runnable.RemoveAll();
			}
            while(bDoIdle && !::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE))
            {
                if(!OnIdle(nIdleCount++))
                    bDoIdle = FALSE;
				if (m_bQuit) goto exit_loop;
            }

            bRet = ::GetMessage(&m_msg, NULL, 0, 0);

            if(bRet == -1)
            {
				SLOGFMTD(_T("::GetMessage returned -1 (error)"));
                continue;   // error, don't process
            }
            else if(!bRet)
            {
				SLOGFMTD(_T("SMessageLoop::Run - exiting,code = %d"),(int)m_msg.wParam);
                break;   // WM_QUIT, exit message loop
            }
            
            OnMsg(&m_msg);

            if(IsIdleMessage(&m_msg))
            {
                bDoIdle = TRUE;
                nIdleCount = 0;
            }
			if (m_bQuit) break;
        }

	exit_loop:
		{
			SAutoLock lock(m_cs);
			SPOSITION pos = m_runnables.GetHeadPosition();
			while(pos)
			{
				IRunnable *pRunnable = m_runnables.GetNext(pos);
				pRunnable->destroy();
			}
			m_runnables.RemoveAll();
		}
        m_bRunning = FALSE;
        return (int)m_msg.wParam;
    }

    BOOL SMessageLoop::OnIdle( int /*nIdleCount*/ )
    {
        for(size_t i = 0; i < m_aIdleHandler.GetCount(); i++)
        {
            IIdleHandler* pIdleHandler = m_aIdleHandler[i];
            if(pIdleHandler != NULL)
                pIdleHandler->OnIdle();
        }
        return FALSE;   // don't continue
    }

    BOOL SMessageLoop::PreTranslateMessage( MSG* pMsg )
    {
        // loop backwards
        for(int i = (int)m_aMsgFilter.GetCount() - 1; i >= 0; i--)
        {
            IMessageFilter* pMessageFilter = m_aMsgFilter[i];
            if(pMessageFilter != NULL && pMessageFilter->PreTranslateMessage(pMsg))
                return TRUE;
        }
        return FALSE;   // not translated
    }

    BOOL SMessageLoop::IsIdleMessage( MSG* pMsg )
    {
        // These messages should NOT cause idle processing
        switch(pMsg->message)
        {
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
        case WM_PAINT:
        case WM_SYSTIMER:	// WM_SYSTIMER (caret blink)
            return FALSE;
        }

        return TRUE;
    }

    BOOL SMessageLoop::RemoveIdleHandler( IIdleHandler* pIdleHandler )
    {
        return RemoveElementFromArray(m_aIdleHandler,pIdleHandler);
    }

    BOOL SMessageLoop::AddIdleHandler( IIdleHandler* pIdleHandler )
    {
        m_aIdleHandler.Add(pIdleHandler);
		return TRUE;
    }

    BOOL SMessageLoop::RemoveMessageFilter( IMessageFilter* pMessageFilter )
    {
        return RemoveElementFromArray(m_aMsgFilter,pMessageFilter);
    }

    BOOL SMessageLoop::AddMessageFilter( IMessageFilter* pMessageFilter )
    {
        m_aMsgFilter.Add(pMessageFilter);
		return TRUE;
    }

	BOOL SMessageLoop::postTask(const IRunnable & runable)
	{
		SAutoLock lock(m_cs);
		if(m_tid == 0)
			return FALSE;
		m_runnables.AddTail(runable.clone());
		if(m_runnables.GetCount()==1)
		{
			PostThreadMessage(m_tid,WM_NULL,0,0);
		}
		return TRUE;
	}


}//end of namespace SOUI
