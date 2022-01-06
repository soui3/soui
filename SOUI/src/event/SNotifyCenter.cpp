#include "souistd.h"
#include "event/SNotifyCenter.h"

namespace SOUI{

template<> SNotifyCenter * SSingleton<SNotifyCenter>::ms_Singleton = 0;

//////////////////////////////////////////////////////////////////////////
class SNotifyReceiver:public SNativeWnd
{
public:
	enum{
		UM_RUNONUISYNC = (WM_USER+1000),
		TIMERID_ASYNC = 100,
	};

	SNotifyReceiver(INotifyCallback * pCallback) :m_pCallback(pCallback)
	{
		SASSERT(m_pCallback);
	}

	~SNotifyReceiver()
	{

	}

	LRESULT OnRunOnUISync(UINT uMsg,WPARAM wParam,LPARAM lParam);

	void OnTimer(UINT_PTR uID);

	BEGIN_MSG_MAP_EX(SNotifyReceiver)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER_EX(UM_RUNONUISYNC, OnRunOnUISync)
	END_MSG_MAP()

protected:
	INotifyCallback * m_pCallback;
};


LRESULT SNotifyReceiver::OnRunOnUISync(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
#ifdef ENABLE_RUNONUI
	std::function<void(void)>* f = (std::function<void(void)>*)lParam;
	(*f)();
#endif//_MSC_VER >= 1700	//VS2012
	return 0;
}

void SNotifyReceiver::OnTimer(UINT_PTR uID)
{
	if(uID==TIMERID_ASYNC)
	{
		KillTimer(uID);
		m_pCallback->OnFireEvts();
	}
}


//////////////////////////////////////////////////////////////////////////
SNotifyCenter::SNotifyCenter(int nInterval)
:m_pReceiver(NULL)
,m_bRunning(FALSE)
,m_nInterval(nInterval)
{
	m_dwMainTrdID = GetCurrentThreadId();
	m_pReceiver = new SNotifyReceiver(this);
	m_pReceiver->Create(_T("NotifyReceiver"),WS_POPUP,0,0,0,0,0,HWND_MESSAGE,0);
	SASSERT(m_pReceiver->IsWindow());
}

SNotifyCenter::~SNotifyCenter(void)
{
	m_pReceiver->DestroyWindow();
	delete m_pReceiver;
	m_pReceiver = NULL;

	SPOSITION pos = m_ayncEvent.GetTailPosition();
	while (pos)
	{
		IEvtArgs *e = m_ayncEvent.GetNext(pos);
		e->Release();
	}
}

void SNotifyCenter::FireEventSync( IEvtArgs *e )
{
	SASSERT(m_dwMainTrdID == GetCurrentThreadId());
	OnFireEvent(e);
}

//把事件抛到事件队列，不检查事件是否注册，执行事件时再检查。
void SNotifyCenter::FireEventAsync( IEvtArgs *e )
{
	SAutoLock lock(m_cs);
	e->AddRef();
	if(!m_bRunning)
	{
		m_pReceiver->SetTimer(SNotifyReceiver::TIMERID_ASYNC,m_nInterval,NULL);
		m_bRunning = TRUE;
	}
	m_ayncEvent.AddTail(e);
}


void SNotifyCenter::OnFireEvent( IEvtArgs *e )
{
	if(!GetEventObject(e->GetID())) return;//确保事件是已经注册过的已经事件。

	FireEvent(e);
	if(!e->IsBubbleUp()) return ;

	SPOSITION pos = m_evtHandlerMap.GetTailPosition();
	while(pos)
	{
		IEvtSlot * pSlot = m_evtHandlerMap.GetPrev(pos);
		pSlot->Run(e);
		if(!e->IsBubbleUp()) break;
	}
}


void SNotifyCenter::OnFireEvts()
{
	SList<IEvtArgs *> evts;
#ifdef ENABLE_RUNONUI
	SList< std::function<void(void)> *> cbs;
#endif
	{
		SAutoLock lock(m_cs);
		evts = m_ayncEvent;
		m_ayncEvent.RemoveAll();
#ifdef ENABLE_RUNONUI
		cbs = m_asyncFuns;
		m_asyncFuns.RemoveAll();
#endif
		m_bRunning = FALSE;
	}
	{
		SPOSITION pos = evts.GetHeadPosition();
		while (pos)
		{
			IEvtArgs *e = evts.GetNext(pos);
			OnFireEvent(e);
			e->Release();
		}
	}
#ifdef ENABLE_RUNONUI
	{
		SPOSITION pos = cbs.GetHeadPosition();
		while (pos)
		{
			std::function<void(void)>* f = cbs.GetNext(pos);
			(*f)();
			delete f;
		}
	}
#endif
}


bool SNotifyCenter::RegisterEventMap( const IEvtSlot &slot )
{
	for(SPOSITION pos = m_evtHandlerMap.GetHeadPosition();pos;)
	{
		IEvtSlot * pSlot = m_evtHandlerMap.GetNext(pos);
		if(pSlot->Equal(&slot)) return false;
	}
	m_evtHandlerMap.AddTail(slot.Clone());
	return true;
}

bool SNotifyCenter::UnregisterEventMap( const IEvtSlot &slot )
{
	for(SPOSITION pos = m_evtHandlerMap.GetHeadPosition();pos;)
	{
		SPOSITION posPrev = pos;
		IEvtSlot * pSlot = m_evtHandlerMap.GetNext(pos);
		if(pSlot->Equal(&slot))
		{
			m_evtHandlerMap.RemoveAt(posPrev);
			pSlot->Release();
			return true;
		}
	}
	return false;
}

#ifdef ENABLE_RUNONUI
void SNotifyCenter::RunOnUISync(std::function<void(void)> fn)
{
	m_pReceiver->SendMessage(SNotifyReceiver::UM_RUNONUISYNC, 0, (LPARAM)&fn);
}

void SNotifyCenter::RunOnUIAsync(std::function<void(void)> fn)
{
	SAutoLock lock(m_cs);
	if(!m_bRunning)
	{
		m_pReceiver->SetTimer(SNotifyReceiver::TIMERID_ASYNC,m_nInterval,NULL);
		m_bRunning = TRUE;
	}
	std::function<void(void)>* f = new std::function<void()>(std::move(fn));
	m_asyncFuns.AddTail(f);
}
#endif


}