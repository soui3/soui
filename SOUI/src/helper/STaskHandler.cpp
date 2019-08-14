#include <souistd.h>
#include <helper/STaskHandler.h>
#include <algorithm>
#include <cassert>
#include <deque>
#include <limits>

namespace SOUI
{

	STaskHandler::STaskHandler() :
		m_taskListLock(),
		m_items(),
		m_nextTaskID(0)
	{
		m_dwThreadID = GetCurrentThreadId();
	}

	STaskHandler::~STaskHandler()
	{
		stop();
	}

	void STaskHandler::start(const char * pszName, Priority priority)
	{
		SAutoLock autoLock(m_taskListLock);
		if (isRunning())
			return;
		TCHAR szBuf[101];
#ifdef _UNICODE
		int nLen =MultiByteToWideChar(CP_ACP, 0, pszName, strlen(pszName), szBuf, 100);
		szBuf[nLen] = 0;
#else
		strcpy_s(szBuf, 100, pszName);
#endif
		Create(szBuf, WS_POPUP, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL);
	}

	void STaskHandler::stop()
	{
		SAutoLock autoLock(m_taskListLock);
		if (IsWindow())
			DestroyWindow();
	}

	bool STaskHandler::isRunning()
	{
		return !!IsWindow();
	}

	long STaskHandler::postTask(const IRunnable *runnable, bool waitUntilDone, int priority)
	{
		if (!isRunning())
		{
			return -1;
		}
		IRunnable *pCloneRunnable = runnable->clone();
		if (GetCurrentThreadId() == m_dwThreadID && waitUntilDone)
		{
			pCloneRunnable->run();
			delete pCloneRunnable;
			return -1;
		}
		SAutoLock autoLock(m_taskListLock);
		if (waitUntilDone)
		{
			SendMessage(UM_RUN_TASK, ++m_nextTaskID, (LPARAM)pCloneRunnable);
			return -1;
		}
		else
		{
			PostMessage(UM_RUN_TASK, ++m_nextTaskID, (LPARAM)pCloneRunnable);
			m_items[m_nextTaskID] = pCloneRunnable;
			return m_nextTaskID;
		}
	}

	LRESULT STaskHandler::OnRunTask(UINT uMsg, WPARAM wp, LPARAM lp)
	{
		SAutoLock autoLock(m_taskListLock);
		if (m_items.Lookup((long)wp) != NULL)
		{
			IRunnable *pRunnable = (IRunnable *)lp;
			pRunnable->run();
			delete pRunnable;
			m_items.RemoveKey((long)wp);
		}
		return 0;
	}

	bool STaskHandler::getName(char * pszBuf, int nBufLen)
	{
		if (!isRunning())
			return false;
		GetWindowTextA(m_hWnd,pszBuf,nBufLen);
		return true;
	}

	void STaskHandler::cancelTasksForObject(void *object)
	{
		if (object == NULL)
		{
			return;
		}
		SAutoLock autoLock(m_taskListLock);
		SPOSITION pos = m_items.GetStartPosition();
		while (pos)
		{
			SPOSITION posPrev = pos;
			IRunnable * p = m_items.GetNextValue(pos);
			if (p->getObject() == object)
			{
				m_items.RemoveAtPos(posPrev);
				delete p;
			}
		}
	}

	bool STaskHandler::cancelTask(long taskId)
	{
		SAutoLock autoLock(m_taskListLock);
		SPOSITION pos = m_items.GetStartPosition();
		while (pos)
		{
			SPOSITION posPrev = pos;
			long key=0;
			IRunnable * p=NULL;
			m_items.GetNextAssoc(pos,key,p);
			if (key == taskId)
			{
				m_items.RemoveAtPos(posPrev);
				delete p;
				return true;
			}
		}
		return false;
	}

	int STaskHandler::getTaskCount() const
	{
		SAutoLock autoLock(m_taskListLock);
		return (int)m_items.GetCount();
	}

	bool STaskHandler::getRunningTaskInfo(char *buf, int bufLen)
	{
		return false;
	}

}
