#pragma once

#include <interface/STaskLoop-i.h>
#include <helper/SSharedPtr.hpp>
#include <unknown/obj-ref-impl.hpp>
#include <helper/SFunctor.hpp>
#include <core/SNativeWnd.h>
#include <helper/SSemaphore.h>
#include <souicoll.h>
namespace SOUI
{
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
		void start(const char * pszName, Priority priority);

		/**
		* Stop task mgr synchronized.
		*/
		void stop();


		/**
		* Remove tasks for a sepcific object from task mgr pening task list
		* @param object the specific object wants pending functors to be removed
		*/
		void cancelTasksForObject(void *object);

		/**
		* Cancel tasks for a specific task ID list
		* @param taskList the task ID list to be canceled
		* @return the removed task list.
		*/
		bool cancelTask(long taskId);


		/**
		* get the total task number in the task mgr queue.
		* @return total task number in task mgr queue
		*/
		int getTaskCount() const;

		/**
		* get the run loop status.
		* @return the running status
		*/
		bool isRunning();

		long postTask(const IRunnable *runnable, bool waitUntilDone, int priority);

		bool getName(char * pszBuf, int nBufLen);

		bool getRunningTaskInfo(char *buf, int bufLen);

	private:
		LRESULT OnRunTask(UINT uMsg, WPARAM wp, LPARAM lp);
		BEGIN_MSG_MAP_EX(STaskHandler)
			MESSAGE_HANDLER_EX(UM_RUN_TASK,OnRunTask)
			CHAIN_MSG_MAP(SNativeWnd)
		END_MSG_MAP()

		mutable SCriticalSection m_taskListLock;
		SMap<long,IRunnable *> m_items;

		long m_nextTaskID;
		DWORD   m_dwThreadID;
	};
}
