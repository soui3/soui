#pragma once

#include <interface/STaskLoop-i.h>
#include <windows.h>
#include "thread.h"
#include <list>
#include <helper/obj-ref-impl.hpp>
#include <helper/SFunctor.hpp>

SNSBEGIN

class STaskLoop : public TObjRefImpl<ITaskLoop>
{
public:
	/**
	* Constructor.
	*/
	STaskLoop();

	/**
	* Destructor.
	*/
	virtual ~STaskLoop();

	template<typename TClass,typename Fun>
	void _start(TClass *obj, Fun fun, Priority priority)
	{
		SFunctor0<TClass,Fun>  runnable(this, &STaskLoop::runLoopProc);
		m_thread.start(&runnable, m_strName,  (Thread::ThreadPriority)priority);
	}

	STDMETHOD_(bool,getName)(THIS_ char *pszBuf, int nBufLen) OVERRIDE;

	/**
	* Start a thread to run.
	* @param priority the thread priority
	*/
	STDMETHOD_(void,start)(THIS_ const char * pszName,Priority priority) OVERRIDE;

	/**
	* Stop thread synchronized.
	*/
	STDMETHOD_(void,stop)(THIS) OVERRIDE;

	/**
	* postTask post or send a tasks to this task manager.
	* @param runnable the to be run task object.
	* @param waitUntilDone, true for send and false for post.
	* @param priority, the task priority.
	* @return the task id, can be used by cancelTask.
	*/
	STDMETHOD_(long,postTask)(THIS_ const IRunnable *runnable, bool waitUntilDone, int priority) OVERRIDE;

	/**
	* Remove tasks for a specific object from task loop pending task list
	* @param object the specific object wants pending functors to be removed
	*/
	STDMETHOD_(void,cancelTasksForObject)(THIS_ void *object) OVERRIDE;

	/**
	* Cancel tasks for a specific task ID list
	* @param taskList the task ID list to be canceled
	* @return the removed task list.
	*/
	STDMETHOD_(bool,cancelTask)(THIS_ long taskId) OVERRIDE;

	/**
	* get the total task number in the task loop queue.
	* @return total task number in task loop queue
	*/
	STDMETHOD_(int,getTaskCount)(THIS) SCONST OVERRIDE;

	/**
	* get the run loop status.
	* @return the running status
	*/
	STDMETHOD_(bool,isRunning)(THIS) OVERRIDE;

	/**
	* get the running task info.
	* @param buf, to receive task info buf.
	* @param bufLen, buffer length
	* @return false - no task is running; true - succeed.
	*/
	STDMETHOD_(bool,getRunningTaskInfo)(THIS_ char *buf, int bufLen) OVERRIDE;

private:
	class TaskItem
	{
	public:
		TaskItem(IRunnable *runnable_, int nPriority_) 
			: taskID(0)
			, runnable(runnable_)
			, nPriority(nPriority_)
			, semaphore(NULL) 
		{}

		const char *getRunnableInfo()
		{
			return runnable->getClassInfo();
		}

		long taskID;
		SAutoRefPtr<IRunnable> runnable;
		SSemaphore *semaphore;
		int  nPriority;
	};




	void runLoopProc();

	mutable SCriticalSection m_taskListLock;
	SCriticalSection m_runningLock;
	std::string m_strName;
	Thread m_thread;
	SSemaphore m_itemsSem;
	std::list<TaskItem> m_items;

	SCriticalSection m_runningInfoLock;
	bool m_hasRunningItem;
	std::string m_runingItemInfo;
	TaskItem m_runningItem;
	long m_nextTaskID;

};

namespace TASKLOOP
{
	SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **ppTaskLoop);
}
SNSEND
