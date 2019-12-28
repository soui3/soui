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
namespace SOUI
{

    struct IAcceleratorMgr;
    
    enum{
    ZORDER_MIN  = 0,
    ZORDER_MAX  = (UINT)-1,
    };

	enum GrtFlag;
    /**
    * @struct     ISwndContainer
    * @brief      SOUI窗口容器接口
    * 
    * Describe    
    */
    struct ISwndContainer : public ITimelineHandler, public ITimelineHandlersMgr
    {
        virtual BOOL RegisterDragDrop(SWND swnd,IDropTarget *pDropTarget)=0;

        virtual BOOL RevokeDragDrop(SWND swnd)=0;

        virtual BOOL OnFireEvent(EventArgs &evt)=0;

        virtual HWND GetHostHwnd()=0;

        virtual const SStringW & GetTranslatorContext() const=0;

        virtual BOOL IsTranslucent() const =0;
        
        virtual BOOL IsSendWheel2Hover() const =0;
        
        virtual CRect GetContainerRect() const =0;

        virtual IRenderTarget * OnGetRenderTarget(const CRect & rc,GrtFlag gdcFlags)=0;

        virtual void OnReleaseRenderTarget(IRenderTarget *pRT,const CRect &rc,GrtFlag gdcFlags)=0;

        virtual void OnRedraw(const CRect &rc)=0;

        virtual SWND OnGetSwndCapture()=0;

        virtual BOOL OnReleaseSwndCapture()=0;

        virtual SWND OnSetSwndCapture(SWND swnd)=0;

        virtual void OnSetSwndFocus(SWND swnd)=0;

        virtual SWND GetHover()=0;

        virtual SWND GetFocus()=0;

        virtual BOOL UpdateWindow()=0;

		virtual void UpdateTooltip() = 0;
        
        //将当前Frame的矩形坐标转换成最终Host的坐标
        virtual void FrameToHost(RECT & rc) const = 0;
                
        virtual IAcceleratorMgr* GetAcceleratorMgr()=0;

        virtual BOOL RegisterTrackMouseEvent(SWND swnd)=0;

        virtual BOOL UnregisterTrackMouseEvent(SWND swnd)=0;

        virtual SMessageLoop * GetMsgLoop() =0;

        //标记窗口树的zorder失效
        virtual void MarkWndTreeZorderDirty() = 0;

        //重建窗口树的zorder
        virtual void BuildWndTreeZorder() = 0;

        virtual IScriptModule * GetScriptModule() = 0;

		virtual int GetScale() const = 0;

		virtual ICaret* GetCaret() = 0;

		virtual void OnCavasInvalidate(SWND swnd) = 0;

		virtual void EnableIME(BOOL bEnable) = 0;
    };


}//namespace SOUI

