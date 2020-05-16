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

namespace SOUI
{

	class SOUI_EXP SRootWindow: public SWindow
	{
		SOUI_CLASS_NAME(SRootWindow,L"root")

	public:
		SRootWindow();

	public:
		SOUI_ATTRS_BEGIN()
			ATTR_ANIMATION(L"enterAnimation",m_aniEnter,FALSE)
			ATTR_ANIMATION(L"exitAnimation",m_aniExit,FALSE)
		SOUI_ATTRS_END()
	protected:
		SAutoRefPtr<IAnimation>		m_aniEnter,m_aniExit;
	};

    class SOUI_EXP SwndContainerImpl : public ISwndContainer
                                     , public SRootWindow
									 , public ITimelineHandler
    {
        SOUI_CLASS_NAME(SwndContainerImpl,L"SwndContainerImpl")
    public:
        SwndContainerImpl();
        
        IDropTarget * GetDropTarget(){return &m_dropTarget;}

        SFocusManager * GetFocusManager() {return &m_focusMgr;}
    protected:
        //ISwndContainer
        virtual BOOL RegisterDragDrop(SWND swnd,IDropTarget *pDropTarget);

        virtual BOOL RevokeDragDrop(SWND swnd);

        virtual LRESULT DoFrameEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

        virtual BOOL OnReleaseSwndCapture();

        virtual SWND OnSetSwndCapture(SWND swnd);
        virtual void OnSetSwndFocus(SWND swnd);
        virtual void FrameToHost(RECT & rc) const{}
        virtual SWND OnGetSwndCapture();

        virtual SWND GetFocus() const;

        virtual SWND GetHover() const;

        virtual IAcceleratorMgr* GetAcceleratorMgr(){return &m_focusMgr;}

        STDMETHOD_(BOOL,RegisterTimelineHandler)(THIS_ ITimelineHandler *pHandler) OVERRIDE;
		STDMETHOD_(BOOL,UnregisterTimelineHandler)(THIS_ ITimelineHandler *pHandler) OVERRIDE;

        virtual BOOL RegisterTrackMouseEvent(SWND swnd);

        virtual BOOL UnregisterTrackMouseEvent(SWND swnd);

        //标记窗口树的zorder失效
        virtual void MarkWndTreeZorderDirty();

        //重建窗口树的zorder
        virtual void BuildWndTreeZorder();

		virtual ICaret* GetCaret();

		virtual void OnCavasInvalidate(SWND swnd) {}

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

}//namespace SOUI

