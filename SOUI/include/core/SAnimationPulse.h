#pragma once
#include <core/SSingleton2.h>
#include <core/SwndContainer-i.h>
#include <core/STimerlineHandlerMgr.h>

namespace SOUI
{
	class SAnimationPulse : public SSingleton2<SAnimationPulse>
	{
		SINGLETON2_TYPE(SINGLETON_ANIMATIONPULSE)
	public:
		enum {
			kPulseSpan = 10,
		};

		SAnimationPulse(void);
		~SAnimationPulse(void);

		bool RegisterTimelineHandler(ITimelineHandler *pHandler);

		bool UnregisterTimelineHandler(ITimelineHandler *pHandler);

	protected:
		static void CALLBACK OnTimer(HWND hWnd,UINT nMsg,UINT_PTR nTimerid,DWORD dwTime);
		STimerlineHandlerMgr		  m_timelineMgr;
		UINT_PTR					  m_timerId;
	};


}
