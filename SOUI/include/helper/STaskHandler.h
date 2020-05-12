#pragma once

#include <interface/STaskLoop-i.h>
//#include <helper/SSharedPtr.hpp>
#include <helper/obj-ref-impl.hpp>
#include <helper/SFunctor.hpp>
#include <core/SNativeWnd.h>
#include <helper/SSemaphore.h>
#include <souicoll.h>

SNSBEGIN

class SOUI_EXP STaskHandler : public TObjRefImpl<ITaskLoop>, protected SNativeWnd
{
	enum{UM_RUN_TASK=(WM_USER+100) };
public:
	/**
	* Constructor.
	*/
	STaskHandler();

	/**
	* Destructor.
	*/
	virtual ~STaskHandler();

	/**
	* Start task mgr thread.
	*/
	STDMETHOD_(void,start)(THIS_ const char * pszName, Priority priority) OVERRIDE;

	/**
	* Stop task mgr synchronized.
	*/
	STDMETHOD_(void,stop)(THIS) OVERRIDE;


	/**
	* Remove tasks for a sepcific object from task mgr pening task list
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
	* get the total task number in the task mgr queue.
	* @return total task number in task mgr queue
	*/
	STDMETHOD_(int,getTaskCount)(THIS) SCONST OVERRIDE;

	/**
	* get the run loop status.
	* @return the running status
	*/
	STDMETHOD_(bool,isRunning)(THIS) OVERRIDE;

	STDMETHOD_(long,postTask)(THIS_ const IRunnable *runnable, bool waitUntilDone, int priority) OVERRIDE;

	STDMETHOD_(bool,getName)(THIS_ char *pszBuf, int nBufLen) OVERRIDE;

	STDMETHOD_(bool,getRunningTaskInfo)(THIS_ char *buf, int bufLen) OVERRIDE;

private:
	LRESULT OnRunTask(UINT uMsg, WPARAM wp, LPARAM lp);
	BEGIN_MSG_MAP_EX(STaskHandler)
		MESSAGE_HANDLER_EX(UM_RUN_TASK,OnRunTask)
		CHAIN_MSG_MAP(SNativeWnd)
	END_MSG_MAP()

	mutable SCriticalSection m_taskListLock;
	SMap<long,IRunnable* > m_items;

	long m_nextTaskID;
	DWORD   m_dwThreadID;
};
SNSEND