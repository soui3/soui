/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SwndContainerImpl.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI窗口容器的实现
*/

#pragma  once

#include <core/SDropTargetDispatcher.h>
#include <core/SFocusManager.h>
#include <core/SCaret.h>
#include <core/STimerlineHandlerMgr.h>

SNSBEGIN

    class SOUI_EXP SwndContainerImpl : public ISwndContainer
									 , public ITimelineHandler
    {
    public:
        SwndContainerImpl();
        
        IDropTarget * GetDropTarget(){return &m_dropTarget;}

        SFocusManager * GetFocusManager() {return &m_focusMgr;}

		void SetRoot(SWindow *pRoot);
		virtual LRESULT DoFrameEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);
    protected:
        //ISwndContainer
        STDMETHOD_(BOOL,RegisterDragDrop)(THIS_ SWND swnd,IDropTarget *pDropTarget) OVERRIDE;

        STDMETHOD_(BOOL,RevokeDragDrop)(THIS_ SWND swnd) OVERRIDE;


        STDMETHOD_(BOOL,OnReleaseSwndCapture)(THIS) OVERRIDE;

        STDMETHOD_(SWND,OnSetSwndCapture)(THIS_ SWND swnd) OVERRIDE;
        STDMETHOD_(void,OnSetSwndFocus)(THIS_ SWND swnd) OVERRIDE;
        STDMETHOD_(void,FrameToHost)(THIS_ RECT & rc) SCONST OVERRIDE{}
        STDMETHOD_(SWND,OnGetSwndCapture)(THIS) OVERRIDE;

		STDMETHOD_(SWND,GetFocus)(THIS) SCONST OVERRIDE;

        STDMETHOD_(SWND,GetHover)(THIS) SCONST OVERRIDE;

        STDMETHOD_(IAcceleratorMgr*,GetAcceleratorMgr)(THIS){return &m_focusMgr;}

        STDMETHOD_(BOOL,RegisterTimelineHandler)(THIS_ ITimelineHandler *pHandler) OVERRIDE;
		STDMETHOD_(BOOL,UnregisterTimelineHandler)(THIS_ ITimelineHandler *pHandler) OVERRIDE;

		STDMETHOD_(BOOL,RegisterTrackMouseEvent)(THIS_ SWND swnd) OVERRIDE;

		STDMETHOD_(BOOL,UnregisterTrackMouseEvent)(THIS_ SWND swnd) OVERRIDE;

        //标记窗口树的zorder失效
        STDMETHOD_(void,MarkWndTreeZorderDirty)(THIS) OVERRIDE;

        //重建窗口树的zorder
        STDMETHOD_(void,BuildWndTreeZorder)(THIS) OVERRIDE;

		STDMETHOD_(ICaret*,GetCaret)(THIS) OVERRIDE;

		STDMETHOD_(void,OnCavasInvalidate)(SWND swnd) OVERRIDE{}

    public://ITimelineHandler
		STDMETHOD_(void,OnNextFrame)(THIS_) OVERRIDE;
    protected:

        void OnFrameMouseMove(UINT uFlag,CPoint pt);

        void OnFrameMouseLeave();

        BOOL OnFrameSetCursor(const CPoint &pt);

        void OnFrameMouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

        void OnFrameMouseWheel(UINT uMsg,WPARAM wParam,LPARAM lParam);

        LRESULT OnFrameKeyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

        void OnFrameKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

        void OnActivate(UINT nState);

        void OnActivateApp(BOOL bActive, DWORD dwThreadID);

	protected:
        void _BuildWndTreeZorder(SWindow *pWnd,UINT &iOrder);
    protected:
		SWindow * m_pRoot;
        SWND m_hCapture;
        SWND m_hHover;
        BOOL    m_bNcHover;

        SFocusManager m_focusMgr;

        SDropTargetDispatcher m_dropTarget;

        BOOL        m_bZorderDirty;

        SList<SWND>                 m_lstTrackMouseEvtWnd;
		SAutoRefPtr<ICaret>			m_caret;

		STimerlineHandlerMgr	    m_timelineHandlerMgr;
    };

SNSEND
