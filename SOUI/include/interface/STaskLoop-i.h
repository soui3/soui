#pragma once
#include <interface/obj-ref-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE IRunnable
DECLARE_INTERFACE_(IRunnable,IObjRef)
{
	STDMETHOD_(IRunnable*,clone)(THIS) SCONST PURE;
	STDMETHOD_(void,run)(THIS) PURE;
	STDMETHOD_(void *,getObject)(THIS) PURE;
	STDMETHOD_(const char *,getClassInfo)(THIS) SCONST PURE;
};

#undef INTERFACE
#define INTERFACE ITaskLoop
DECLARE_INTERFACE_(ITaskLoop,IObjRef)
{
	enum Priority
	{
		High = 1,
		Normal = 0,
		Low = -1,
	};
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

	STDMETHOD_(bool,getName)(THIS_ char *pszBuf, int nBufLen) PURE;

	/**
	* Start a thread to run.
	* @param priority the thread priority
	*/
	STDMETHOD_(void,start)(THIS_ const char * pszName,Priority priority) PURE;

	/**
	* Stop thread synchronized.
	*/
	STDMETHOD_(void,stop)(THIS) PURE;

	/**
	* postTask post or send a tasks to this task manager.
	* @param runnable the to be run task object.
	* @param waitUntilDone, true for send and false for post.
	* @param priority, the task priority.
	* @return the task id, can be used by cancelTask.
	*/
	STDMETHOD_(long,postTask)(THIS_ const IRunnable *runnable, bool waitUntilDone, int priority) PURE;

	/**
	* Remove tasks for a specific object from task loop pending task list
	* @param object the specific object wants pending functors to be removed
	*/
	STDMETHOD_(void,cancelTasksForObject)(THIS_ void *object) PURE;

	/**
	* Cancel tasks for a specific task ID list
	* @param taskList the task ID list to be canceled
	* @return the removed task list.
	*/
	STDMETHOD_(bool,cancelTask)(THIS_ long taskId) PURE;

	/**
	* get the total task number in the task loop queue.
	* @return total task number in task loop queue
	*/
	STDMETHOD_(int,getTaskCount)(THIS) SCONST PURE;

	/**
	* get the run loop status.
	* @return the running status
	*/
	STDMETHOD_(bool,isRunning)(THIS) PURE;

	/**
	* get the running task info.
	* @param buf, to receive task info buf.
	* @param bufLen, buffer length
	* @return false - no task is running; true - succeed.
	*/
	STDMETHOD_(bool,getRunningTaskInfo)(THIS_ char *buf, int bufLen) PURE;
};

SNSEND