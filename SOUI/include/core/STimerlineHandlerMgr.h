#pragma once

#include <interface/STimelineHandler-i.h>
#include <souicoll.h>

namespace SOUI
{

	class STimerlineHandlerMgr : public ITimelineHandler
	{
	public:
		STimerlineHandlerMgr(void);
		~STimerlineHandlerMgr(void);

		bool RegisterTimelineHandler(ITimelineHandler *pHandler);

		bool UnregisterTimelineHandler(ITimelineHandler *pHandler);

		bool IsEmpty() const;
	public:
		virtual void OnNextFrame() override;

	protected:
		typedef SMap<ITimelineHandler *,bool> TLMAP;
		TLMAP m_mapHandlers;
	};

}
