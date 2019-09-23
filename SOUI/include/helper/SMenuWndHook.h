/*########################################################################
    Filename:     MenuWndHook.h
    ----------------------------------------------------
    Remarks:    ...
    ----------------------------------------------------
    Author:        成真
    Email:        anyou@sina.com
                anyou@msn.com
    Created:    7/4/2003 17:37
  ########################################################################*/

#pragma once

namespace SOUI
{

/*########################################################################
              ------------------------------------------------
                             CMenuWndHook 类
              ------------------------------------------------
  ########################################################################*/
class SOUI_EXP SMenuWndHook
{
// 建构 ---------------------------------------------------------
public:
    SMenuWndHook(HWND hWnd);
    ~SMenuWndHook();
    static void InstallHook(HINSTANCE hInst,LPCWSTR pszSkinName=NULL);
    static void UnInstallHook();

// 消息 ----------------------------------------------------------
public:
    static LRESULT CALLBACK CoolMenuProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK WindowHook (int code, WPARAM wParam, LPARAM lParam);

    int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnNcCalcsize(BOOL bValidCalc,NCCALCSIZE_PARAMS* lpncsp);
    void OnNcDestroy();
    void OnPrint(HDC dc);
    void OnNcPaint();
    void OnWindowPosChanging(WINDOWPOS* pWindowPos);
    void OnWindowPosChanged();

// 操作 ----------------------------------------------------------
public:
    static SMenuWndHook* AddWndHook(HWND hwnd);
    static SMenuWndHook* GetWndHook(HWND hwnd);

// 数据 -----------------------------------------------------------
protected:
    HWND m_hWnd;

    static SMap<HWND, SMenuWndHook*> m_WndMenuMap;
    static HHOOK m_hMenuHook;
    static SStringW    m_strSkinName;
};

}//namespace SOUI