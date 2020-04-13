#pragma once
#include <interface/SHostMsgHandler-i.h>
#include <core/SSingleton2.h>

namespace SOUI
{
	class SOUI_EXP SHostMgr: public SSingleton2<SHostMgr>
	{
		SINGLETON2_TYPE(SINGLETON_HOSTMGR)
		friend class SHostWnd;
	public:
		SHostMgr(void);
		~SHostMgr(void);

		void DispatchMessage(bool bRelayout,UINT uMsg, WPARAM wp=0,LPARAM lp=0);

	protected:
		bool AddHostMsgHandler(IHostMsgHandler * pHost);

		bool RemoveHostMsgHandler(IHostMsgHandler *pHost);

	protected:
		SMap<IHostMsgHandler*,bool>	m_mapHostMsgHandler;
		SCriticalSection			m_cs;
	};
}
