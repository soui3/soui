/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SwndContainer-i.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    DUI窗口容器接口
*/

#pragma once

#include <event/SEvents.h>
#include <core/SWnd.h>
#include <core/SMsgLoop.h>
#include <interface/SScriptModule-i.h>
#include <interface/scaret-i.h>
#include <interface/STimelineHandler-i.h>
#include <interface/SAccelerator-i.h>

SNSBEGIN

enum{
	ZORDER_MIN  = 0,
	ZORDER_MAX  = (UINT)-1,
};

enum GrtFlag;

//todo: move to other place.
inline int RectWidth(LPCRECT rc) {return rc->right-rc->left;}
inline int RectHeight(LPCRECT rc) {return rc->bottom-rc->top;}

/**
* @struct     ISwndContainer
* @brief      SOUI窗口容器接口
* 
* Describe    
*/

#undef INTERFACE
#define INTERFACE ISwndContainer
DECLARE_INTERFACE_(ISwndContainer,ITimelineHandlersMgr)
{
	STDMETHOD_(BOOL,RegisterDragDrop)(THIS_ SWND swnd,IDropTarget *pDropTarget) PURE;

	STDMETHOD_(BOOL,RevokeDragDrop)(THIS_ SWND swnd) PURE;

	STDMETHOD_(BOOL,OnFireEvent)(THIS_ IEvtArgs *evt) PURE;

	STDMETHOD_(HWND,GetHostHwnd)(THIS) PURE;

	STDMETHOD_(const SStringW &,GetTranslatorContext)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,IsTranslucent)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,IsSendWheel2Hover)(THIS) SCONST PURE;

	STDMETHOD_(CRect,GetContainerRect)(THIS) SCONST PURE;

	STDMETHOD_(IRenderTarget *,OnGetRenderTarget)(THIS_ LPCRECT rc,GrtFlag gdcFlags) PURE;

	STDMETHOD_(void,OnReleaseRenderTarget)(THIS_ IRenderTarget *pRT,LPCRECT rc,GrtFlag gdcFlags) PURE;

	STDMETHOD_(void,OnRedraw)(THIS_ LPCRECT rc) PURE;

	STDMETHOD_(SWND,OnGetSwndCapture)(THIS) PURE;

	STDMETHOD_(BOOL,OnReleaseSwndCapture)(THIS) PURE;

	STDMETHOD_(SWND,OnSetSwndCapture)(THIS_ SWND swnd) PURE;

	STDMETHOD_(void,OnSetSwndFocus)(THIS_ SWND swnd) PURE;

	STDMETHOD_(SWND,GetHover)(THIS) SCONST PURE;

	STDMETHOD_(SWND,GetFocus)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,UpdateWindow)(THIS) PURE;

	STDMETHOD_(void,UpdateTooltip)(THIS) PURE;

	//将当前Frame的矩形坐标转换成最终Host的坐标
	STDMETHOD_(void,FrameToHost)(THIS_ RECT & rc) SCONST PURE;

	STDMETHOD_(IAcceleratorMgr*,GetAcceleratorMgr)(THIS) PURE;

	STDMETHOD_(BOOL,RegisterTrackMouseEvent)(THIS_ SWND swnd) PURE;

	STDMETHOD_(BOOL,UnregisterTrackMouseEvent)(THIS_ SWND swnd) PURE;

	STDMETHOD_(IMessageLoop *,GetMsgLoop)(THIS) PURE;

	//标记窗口树的zorder失效
	STDMETHOD_(void,MarkWndTreeZorderDirty)(THIS) PURE;

	//重建窗口树的zorder
	STDMETHOD_(void,BuildWndTreeZorder)(THIS) PURE;

	STDMETHOD_(IScriptModule *,GetScriptModule)(THIS) PURE;

	STDMETHOD_(int,GetScale)(THIS) SCONST PURE;

	STDMETHOD_(ICaret*,GetCaret)(THIS) PURE;

	STDMETHOD_(void,OnCavasInvalidate)(THIS_ SWND swnd) PURE;

	STDMETHOD_(void,EnableIME)(THIS_ BOOL bEnable) PURE;
};

SNSEND
