#pragma once

namespace SOUI
{
	class STabCtrlHeaderBinder
	{
	public:
		STabCtrlHeaderBinder(STabCtrl *phost=NULL);
		~STabCtrlHeaderBinder();
		void BindHost(STabCtrl *);
		STabCtrlHeaderBinder(const SOUI::STabCtrlHeaderBinder &){}
		STabCtrlHeaderBinder &operator =(const SOUI::STabCtrlHeaderBinder &){return *this;}
		void Bind(SWindow *pWnd, int idx, DWORD evtcode = EVT_CMD);
	private:
		CAutoRefPtr<STabCtrl> m_pTabHost;
	protected:
		SMap<SWindow*, int> m_lstPages;
		bool EvtHander(EventArgs *e);
		bool TabSelChanged(EventTabSelChanged *evt_sel);
	};
}