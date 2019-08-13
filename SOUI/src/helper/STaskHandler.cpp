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
		m_runningLock(),
		m_itemsSem(),
		m_items(),
		m_hasRunningItem(false),
		m_runningItem(NULL, 0),
		m_nextTaskID(0)
	{
		m_dwThreadID = GetCurrentThreadId();
		Create(_T("task_handler_wnd"), WS_POPUP, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL);
	}

	STaskHandler::~STaskHandler()
	{
		stop();
	}

	void STaskHandler::start(const char * pszName, Priority priority)
	{
		SetTimer(100, 10, NULL);
		m_isRunning = true;
	}

	void STaskHandler::stop()
	{
		int taskNum = getTaskCount();

		m_itemsSem.notify();
		KillTimer(100);
		m_isRunning = false;
	}

	bool STaskHandler::isRunning()
	{
		return m_isRunning;
	}

	long STaskHandler::postTask(const IRunnable *runnable, bool waitUntilDone, int priority)
	{
		if (!m_isRunning)
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

		SSemaphore semaphore;
		TaskItem item(pCloneRunnable, priority);

		if (waitUntilDone)
		{
			item.semaphore = &semaphore;
		}

		m_taskListLock.Enter();

		item.taskID = m_nextTaskID;
		m_nextTaskID = (m_nextTaskID + 1) & ((std::numeric_limits<long>::max)());
		std::list<TaskItem>::reverse_iterator it = m_items.rbegin();
		while (it != m_items.rend())
		{
			if (it->nPriority >= priority)
			{
				m_items.insert(it.base(), item);
				break;
			}
			it++;
		}
		if (it == m_items.rend())
			m_items.push_front(item);

		m_taskListLock.Leave();
		m_itemsSem.notify();

		if (waitUntilDone)
		{
			int ret = semaphore.wait();

			if (ret == RETURN_ERROR)
			{
			}
		}

		return item.taskID;
	}

	void STaskHandler::OnTimer(UINT_PTR timerId)
	{
		while (true)
		{
			{
				SAutoLock autoLock(m_taskListLock);
				SAutoLock autoRunningLock(m_runningLock);
				SAutoLock autoRuningInfoLock(m_runningInfoLock);
				m_hasRunningItem = false;
				m_runingItemInfo.clear();
				m_runningItem = TaskItem(NULL, 0);
				if (!m_items.empty())
				{
					m_hasRunningItem = true;
					m_runningItem = m_items.front();
					m_runingItemInfo = m_runningItem.getRunnableInfo();
					m_items.pop_front();
				}
			}

			{
				SAutoLock autoRunningLock(m_runningLock);
				if (m_hasRunningItem)
				{
					TaskItem item = m_runningItem;
					item.runnable->run();
					if (item.semaphore)
					{
						//通知一个task执行完毕
						item.semaphore->notify();
					}
					SAutoLock autoRuningInfoLock(m_runningInfoLock);
					m_hasRunningItem = false;
					m_runingItemInfo.clear();
					m_runningItem = TaskItem(NULL, 0);
				}
				else
				{
					break;
				}

			}
		}// end of while
	}

	bool STaskHandler::getName(char * pszBuf, int nBufLen)
	{
		SAutoLock autoLock(m_taskListLock);
		if (m_strName.length() >= (size_t)nBufLen)
			return false;
		strcpy_s(pszBuf, nBufLen, m_strName.c_str());
		return true;
	}

	void STaskHandler::cancelTasksForObject(void *object)
	{
		if (object == NULL)
		{
			return;
		}

		{
			SAutoLock autoLock(m_taskListLock);
			std::list<TaskItem>::iterator iter = m_items.begin();

			while (iter != m_items.end())
			{
				TaskItem &item = *iter;

				if (item.runnable->getObject() == object)
				{
					iter = m_items.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}
		{
			SAutoLock autoLock(m_runningLock);
			SAutoLock autoLockRunningInfo(m_runningInfoLock);
			if (m_hasRunningItem)
			{//make sure the running item is not belong to the to be canceled object.
				m_hasRunningItem = false;
				m_runingItemInfo.clear();
				m_runningItem = TaskItem(NULL, 0);
			}
		}
	}

	bool STaskHandler::cancelTask(long taskId)
	{
		SAutoLock autoLock(m_taskListLock);
		std::list<TaskItem>::iterator itemIt = m_items.begin();

		while (itemIt != m_items.end())
		{
			if (itemIt->taskID == taskId)
			{
				itemIt = m_items.erase(itemIt);
				return true;
			}
			else
			{
				++itemIt;
			}
		}
		return false;
	}

	int STaskHandler::getTaskCount() const
	{
		SAutoLock autoLock(m_taskListLock);

		return (int)m_items.size();
	}

	bool STaskHandler::getRunningTaskInfo(char *buf, int bufLen)
	{
		SAutoLock autoLock(m_runningInfoLock);
		if (!m_hasRunningItem)
			return false;
		strcat_s(buf, bufLen - 1, m_runingItemInfo.c_str());
		return true;
	}

}
