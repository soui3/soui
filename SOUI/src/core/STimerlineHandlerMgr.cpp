#include <souistd.h>
#include <core\STimerlineHandlerMgr.h>

namespace SOUI
{
	STimerlineHandlerMgr::STimerlineHandlerMgr(void)
	{
	}

	STimerlineHandlerMgr::~STimerlineHandlerMgr(void)
	{
	}

	bool STimerlineHandlerMgr::RegisterTimelineHandler(ITimelineHandler *pHandler)
	{
		if(m_mapHandlers.Lookup(pHandler))
			return false;
		m_mapHandlers[pHandler] = false;
		return true;
	}

	bool STimerlineHandlerMgr::UnregisterTimelineHandler(ITimelineHandler *pHandler)
	{
		return m_mapHandlers.RemoveKey(pHandler);
	}

	void STimerlineHandlerMgr::OnNextFrame()
	{
		ITimelineHandler ** pHandlers = new ITimelineHandler *[m_mapHandlers.GetCount()];
		//collect all handlers.
		SPOSITION pos = m_mapHandlers.GetStartPosition();
		int idx = 0;
		while(pos)
		{
			ITimelineHandler *p = m_mapHandlers.GetNextKey(pos);
			pHandlers[idx++] = p;
		}

		//run handles.
		for(int i=0;i<idx;i++)
		{
			TLMAP::CPair *p = m_mapHandlers.Lookup(pHandlers[i]);
			if(!p) continue;
			//make sure the handler is valid before run.
			pHandlers[i]->OnNextFrame();
		}
		delete [] pHandlers;
	}

	bool STimerlineHandlerMgr::IsEmpty() const
	{
		return m_mapHandlers.IsEmpty();
	}

}
