#include <include\souistd.h>
#include <helper\SHostMgr.h>

namespace SOUI
{
	SHostMgr::SHostMgr(void)
	{
	}

	SHostMgr::~SHostMgr(void)
	{
	}


	bool SHostMgr::AddHostMsgHandler(IHostMsgHandler * pHost)
	{
		SAutoLock autoLock(m_cs);
		if(m_mapHostMsgHandler.Lookup(pHost))
			return false;
		m_mapHostMsgHandler[pHost]=true;
		return true;
	}

	bool SHostMgr::RemoveHostMsgHandler(IHostMsgHandler * pHost)
	{
		SAutoLock autoLock(m_cs);
		return m_mapHostMsgHandler.RemoveKey(pHost);
	}

	void SHostMgr::DispatchMessage(bool bRelayout,UINT uMsg,WPARAM wp,LPARAM lp)
	{
		SAutoLock autoLock(m_cs);
		SPOSITION pos = m_mapHostMsgHandler.GetStartPosition();
		while(pos)
		{
			IHostMsgHandler *pHost = m_mapHostMsgHandler.GetNextKey(pos);
			pHost->OnHostMsg(bRelayout,uMsg,wp,lp);
		}
	}

}
