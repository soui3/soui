/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       RealWndHandler-i.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/14
* 
* Describe     提供RealWnd的创建及销毁管理
*/
#pragma  once
#include <interface/obj-ref-i.h>

SNSBEGIN

interface IWindow;
/** 
    * @struct     IRealWndHandler
    * @brief     
    *
    * Describe   
    */
#undef INTERFACE
#define INTERFACE IRealWndHandler
DECLARE_INTERFACE_(IRealWndHandler,IObject)
{
	/**
	* SRealWnd::OnRealWndCreate
	* @brief    窗口创建
	* @param    SRealWnd *pRealWnd -- 窗口指针
	*
	* Describe  窗口创建
	*/    
	STDMETHOD_(HWND,OnRealWndCreate)(THIS_ IWindow *pRealWnd) PURE;

	/**
	* SRealWnd::OnRealWndDestroy
	* @brief    销毁窗口
	* @param    SRealWnd *pRealWnd -- 窗口指针
	*
	* Describe  销毁窗口
	*/
	STDMETHOD_(void,OnRealWndDestroy)(THIS_ IWindow *pRealWnd) PURE;

	/**
	* SRealWnd::OnRealWndInit
	* @brief    初始化窗口
	* @param    SRealWnd *pRealWnd -- 窗口指针
	* @return   BOOL -- FALSE:交由系统处理，TRUE:用户处理
	*
	* Describe  初始化窗口
	*/
	STDMETHOD_(BOOL,OnRealWndInit)(THIS_ IWindow *pRealWnd) PURE;

	/**
	* SRealWnd::OnRealWndSize
	* @brief    调整窗口大小
	* @param    SRealWnd *pRealWnd -- 窗口指针
	* @return   BOOL -- FALSE：交由SOUI处理; TRUE:用户管理窗口的移动
	*
	* Describe  调整窗口大小,该方法由OnRealWndPosition代替
	*/
	STDMETHOD_(BOOL,OnRealWndSize)(THIS_ IWindow *pRealWnd) PURE;

	/**
	* SRealWnd::OnRealWndPosition
	* @brief    调整窗口位置
	* @param    SRealWnd *pRealWnd -- 窗口指针
	* @return   BOOL -- FALSE：交由SOUI处理; TRUE:用户管理窗口的移动
	*
	* Describe  调整窗口大小
	*/
	STDMETHOD_(BOOL,OnRealWndPosition)(THIS_ IWindow *pRealWnd, RECT rcWnd) PURE;
};

SNSEND