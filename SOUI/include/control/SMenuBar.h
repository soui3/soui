#pragma once

#include <helper/SMenu.h>

namespace SOUI
{
	class SMenuItem;

	class SMenuBar :
		public SWindow
	{
		SOUI_CLASS_NAME(SMenuBar, L"menubar")
			friend class SMenuItem;
	public:
		SMenuBar();
		~SMenuBar();

		BOOL Insert(LPCTSTR pszTitle, LPCTSTR pszResName, int iPos = -1);
		BOOL Insert(pugi::xml_node xmlNode, int iPos = -1);

		SMenu* GetMenu(DWORD dwPos);

		int HitTest(CPoint pt);
	protected:
		SMenuItem* GetMenuItem(DWORD dwPos);
		virtual BOOL CreateChildren(pugi::xml_node xmlNode);

		static LRESULT CALLBACK MenuSwitch(int code, WPARAM wParam, LPARAM lParam);

		SArray<SMenuItem*> m_lstMenuItem;
		HWND m_hWnd;
		pugi::xml_document  m_xmlStyle;
		BOOL m_bIsShow;
		SMenuItem* m_pNowMenu;
		int m_iNowMenu;
		CPoint m_ptMouse;

		static HHOOK m_hMsgHook;
		static SMenuBar* m_pMenuBar;
	};

}