#include <souistd.h>
#include <core\SAnimatorPulse.h>

namespace SOUI
{
	SAnimatorPulse::SAnimatorPulse(void):m_timerId(-1)
	{
	}

	SAnimatorPulse::~SAnimatorPulse(void)
	{
	}

	bool SAnimatorPulse::RegisterTimelineHandler(ITimelineHandler *pHandler)
	{
		bool bRet = m_timelineMgr.RegisterTimelineHandler(pHandler);
		if(!bRet) return false;
		if(m_timerId == -1)
		{
			m_timerId = ::SetTimer(NULL,0,kPulseSpan,SAnimatorPulse::OnTimer);
		}
		return true;
	}

	bool SAnimatorPulse::UnregisterTimelineHandler(ITimelineHandler *pHandler)
	{
		bool bRet = m_timelineMgr.UnregisterTimelineHandler(pHandler);
		if(bRet && m_timelineMgr.IsEmpty())
		{
			::KillTimer(NULL,m_timerId);
			m_timerId = -1;
		}
		return bRet;
	}

	void SAnimatorPulse::OnTimer(HWND hWnd,UINT nMsg,UINT_PTR nTimerid,DWORD dwTime)
	{
		SAnimatorPulse * thiz = SAnimatorPulse::getSingletonPtr();
		thiz->m_timelineMgr.OnNextFrame();
	}

}
