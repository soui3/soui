#include <souistd.h>
#include <core\SanimationPulse.h>

namespace SOUI
{
	SAnimationPulse::SAnimationPulse(void):m_timerId(-1)
	{
	}

	SAnimationPulse::~SAnimationPulse(void)
	{
	}

	bool SAnimationPulse::RegisterTimelineHandler(ITimelineHandler *pHandler)
	{
		bool bRet = m_timelineMgr.RegisterTimelineHandler(pHandler);
		if(!bRet) return false;
		if(m_timerId == -1)
		{
			m_timerId = ::SetTimer(NULL,0,kPulseSpan,SAnimationPulse::OnTimer);
		}
		return true;
	}

	bool SAnimationPulse::UnregisterTimelineHandler(ITimelineHandler *pHandler)
	{
		bool bRet = m_timelineMgr.UnregisterTimelineHandler(pHandler);
		if(bRet && m_timelineMgr.IsEmpty())
		{
			::KillTimer(NULL,m_timerId);
			m_timerId = -1;
		}
		return bRet;
	}

	void SAnimationPulse::OnTimer(HWND hWnd,UINT nMsg,UINT_PTR nTimerid,DWORD dwTime)
	{
		SAnimationPulse * thiz = SAnimationPulse::getSingletonPtr();
		thiz->m_timelineMgr.OnNextFrame();
	}

}
