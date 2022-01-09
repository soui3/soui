//////////////////////////////////////////////////////////////////////////
//  Class Name: SHostWnd
//    Description: Real Container of SWindow
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <core/SWndContainerImpl.h>
#include <core/SNativeWnd.h>
#include <core/SDropTargetDispatcher.h>
#include <event/SEventcrack.h>
#include <interface/stooltip-i.h>
#include <interface/SHostMsgHandler-i.h>
#include <interface/shostwnd-i.h>
#include <core/SCaret.h>
#include <core/SHostMsgDef.h>
#include <layout/SLayoutsize.h>
#include <helper/SplitString.h>
#include <helper/SWndSpy.h>
#include <helper/SScriptTimer.h>
namespace SOUI
{
    class SHostWndAttr : public TObjRefImpl<SObject>, public ITrCtxProvider
    {
        SOUI_CLASS_NAME(SHostWndAttr, L"SHostWndAttr")
        enum {WT_UNDEFINE=0,WT_APPMAIN=1,WT_NORMAL=2};
		friend class SHostWnd;
		friend class SRootWindow;
    public:
		SHostWndAttr(void);

		~SHostWndAttr();
        
		void Init();
        
		virtual const SStringW & GetTrCtx() const;

		CRect GetMaxInset(int nScale) const;

		CRect GetMargin(int nScale) const;

		CSize GetMinSize(int nScale) const;

		void SetTranslucent(bool bTranslucent);

		void SetTrCtx(const SStringW & strTrCtx);

		void SetSendWheel2Hover(bool value);

        SOUI_ATTRS_BEGIN()
            ATTR_STRINGW(L"trCtx",m_strTrCtx,FALSE)
            ATTR_I18NSTRT(L"title",m_strTitle,FALSE)
			ATTR_LAYOUTSIZE4(L"maxInset",m_rcMaxInset,FALSE)
			ATTR_LAYOUTSIZE4(L"margin",m_rcMargin,FALSE)
			ATTR_LAYOUTSIZE2(L"minsize",m_szMin,FALSE)
            ATTR_DWORD(L"wndStyle",m_dwStyle,FALSE)
            ATTR_DWORD(L"wndStyleEx",m_dwExStyle,FALSE)
            ATTR_INT(L"resizable",m_bResizable,FALSE)
            ATTR_INT(L"translucent",m_bTranslucent,FALSE)
            ATTR_INT(L"sendWheel2Hover",m_bSendWheel2Hover,FALSE)
            ATTR_INT(L"appWnd",m_bAppWnd,FALSE)
            ATTR_INT(L"toolWindow",m_bToolWnd,FALSE)
            ATTR_ICON(L"smallIcon",m_hAppIconSmall,FALSE)
            ATTR_ICON(L"bigIcon",m_hAppIconBig,FALSE)
            ATTR_INT(L"allowSpy",m_bAllowSpy,FALSE)
            ATTR_ENUM_BEGIN(L"wndType",DWORD,FALSE)
                ATTR_ENUM_VALUE(L"undefine",WT_UNDEFINE)
                ATTR_ENUM_VALUE(L"appMain",WT_APPMAIN)
                ATTR_ENUM_VALUE(L"normal",WT_NORMAL)
            ATTR_ENUM_END(m_byWndType)
        SOUI_ATTRS_END()

	protected:
        SLayoutSize m_rcMargin[4];       //窗口拉伸的边缘检测大小
		SLayoutSize m_szMin[2];          //窗口最小值
		SLayoutSize m_rcMaxInset[4];     //窗口最大化时超出屏幕的边缘大小。经测试，WS_OVERLAPPED style的窗口该属性无效

        DWORD m_byWndType:8;         //主窗口标志,有该标志的窗口关闭时自动发送WM_QUIT
        DWORD m_bResizable:1;       //窗口大小可调节
        DWORD m_bAppWnd:1;          //APP窗口，在任务栏上显示按钮
        DWORD m_bToolWnd:1;         //设置WS_ES_TOOLWINDOW属性
        DWORD m_bTranslucent:1;     //窗口的半透明属性
        DWORD m_bAllowSpy:1;        //允许spy
        DWORD m_bSendWheel2Hover:1; //将滚轮消息发送到hover窗口

        DWORD m_dwStyle;
        DWORD m_dwExStyle;

        SStringW m_strTrCtx;     //在语言翻译时作为context使用
        STrText  m_strTitle;
        HICON   m_hAppIconSmall;
        HICON   m_hAppIconBig;
    };

	enum	AniState{
		Ani_none=0,
		Ani_win=1,
		Ani_host=2,
		Ani_both=(Ani_win|Ani_host),
	};

	class SOUI_EXP SRootWindow: public SWindow
	{
		SOUI_CLASS_NAME(SRootWindow,L"root")
		friend class SHostWnd;
	public:
		SRootWindow(SHostWnd *pHostWnd);

	public:
		SHostWnd * GetHostWnd() const;
	public:
		STDMETHOD_(void,UpdateLayout)(THIS) OVERRIDE;
	protected:
		virtual void OnAnimationInvalidate(IAnimation *pAni,bool bErase);
		virtual void OnAnimationUpdate(IAnimation *pAni);
		virtual void OnAnimationStop(IAnimation *pAni);
	protected://Swindow 虚方法
		virtual void BeforePaint(IRenderTarget *pRT, SPainter &painter);
		virtual void AfterPaint(IRenderTarget *pRT, SPainter &painter);
		virtual BOOL IsLayeredWindow() const{return FALSE;}
		virtual HRESULT OnLanguageChanged(); 
		virtual void OnScaleChanged(int scale);
		virtual void RequestRelayout(SWND hSource ,BOOL bSourceResizable );
	public:
		SOUI_ATTRS_BEGIN()
			ATTR_ANIMATION(L"enterAnimation",m_aniEnter,FALSE)
			ATTR_ANIMATION(L"exitAnimation",m_aniExit,FALSE)
		SOUI_ATTRS_END()
	protected:
		SAutoRefPtr<IAnimation>		m_aniEnter,m_aniExit;
		SHostWnd*					m_pHostWnd;
	};

	class SOUI_EXP SHostWnd: public TObjRefImpl<SObjectImpl<IHostWnd>>
    , public SwndContainerImpl
    , public SNativeWnd
	, protected IHostMsgHandler
{
    friend class SDummyWnd;
	friend class SRootWindow;
	SOUI_CLASS_NAME_EX(SHostWnd,L"SHostWnd",NativeWnd)
protected:    
    SDummyWnd*           m_dummyWnd;            /**<半透明窗口使用的一个响应WM_PAINT消息的窗口*/
    SHostWndAttr         m_hostAttr;            /**<host属性，对应XML中的SOUI结点 */
    SStringT             m_strXmlLayout;        /**<布局资源名称,如xml:main_wnd */

    BOOL                 m_bTrackFlag;          /**< 跟踪鼠标标志 */

    BOOL                    m_bNeedRepaint;     /**<缓存脏标志*/
    BOOL                    m_bNeedAllRepaint;  /**<缓存全部更新标志*/

    IToolTip *              m_pTipCtrl;         /**<tip接口*/

    SAutoRefPtr<IRegion>    m_rgnInvalidate;    /**<脏区域*/
    SAutoRefPtr<IRenderTarget> m_memRT;         /**<绘制缓存*/
    SAutoRefPtr<SStylePool> m_privateStylePool; /**<局部style pool*/
    SAutoRefPtr<SSkinPool>  m_privateSkinPool;  /**<局部skin pool*/
	SAutoRefPtr<STemplatePool>  m_privateTemplatePool;/**< 局部template pool */

    SList<SWND>             m_lstUpdateSwnd;    /**<等待刷新的非背景混合窗口列表*/
    SList<RECT>             m_lstUpdatedRect;   /**<更新的脏矩形列表*/
    BOOL                    m_bRendering;         /**<正在渲染过程中*/
    
    MSG                     m_msgMouse;         /**<上一次鼠标按下消息*/
    
    SAutoRefPtr<IScriptModule> m_pScriptModule; /**<脚本模块*/   

	int						m_nScale;			/**<缩放比例 */

	CSize					m_szAppSetted;		/**<应用层设置的窗口大小 */
	int						m_nAutoSizing;		/**<自动计算大小触发的WM_SIZE消息 */
	bool                    m_bResizing;        /**<执行WM_SIZE*/

	SAutoRefPtr<IAnimation> m_hostAnimation;
	DWORD m_AniState;
	BOOL  m_bFirstShow;
	DWORD					m_dwThreadID;
	SRootWindow*			m_pRoot;
public:
    SHostWnd(LPCTSTR pszResName = NULL);
    virtual ~SHostWnd();

public:
	enum {
		kPulseTimer = 4321,		//soui timer. don't use it in app
		kPulseInterval = 10,
	};

public:
	STDMETHOD_(void,SetLayoutId)(THIS_ LPCTSTR pszLayoutId) OVERRIDE
	{
		m_strXmlLayout = pszLayoutId;
	}
	STDMETHOD_(HWND,Create)(THIS_ HWND hWndParent,DWORD dwStyle,DWORD dwExStyle, int x = 0, int y = 0, int nWidth = 0, int nHeight = 0) OVERRIDE;

	STDMETHOD_(BOOL,Destroy)(THIS) OVERRIDE
	{
		return DestroyWindow();
	}
	STDMETHOD_(IWindow*,GetIRoot)(THIS) OVERRIDE
	{
		return m_pRoot;
	}
	STDMETHOD_(HWND,GetHwnd)(THIS) OVERRIDE
	{
		return m_hWnd;
	}

public:

	SWindow* FindChildByName(LPCWSTR strName , int nDeep =-1){
		return GetRoot()->FindChildByName(strName,nDeep);
	}

	SWindow* FindChildByName(LPCSTR strName , int nDeep =-1)
	{
		return GetRoot()->FindChildByName(strName,nDeep);
	}

	template<class T>
	T* FindChildByName2(LPCWSTR pszName, int nDeep =-1)
	{
		return GetRoot()->FindChildByName2<T>(pszName,nDeep);
	}

	template<class T>
	T* FindChildByName2(LPCSTR pszName, int nDeep =-1)
	{
		return GetRoot()->FindChildByName2<T>(pszName,nDeep);
	}

	SWindow* FindChildByID(int nID, int nDeep =-1)
	{
		return GetRoot()->FindChildByID(nID,nDeep);
	}

	template<class T>
	T* FindChildByID2(int nID, int nDeep =-1)
	{
		return GetRoot()->FindChildByID2<T>(nID,nDeep);
	}

	template<class T>
	T * FindChildByClass(int nDeep=-1) const
	{
		return GetRoot()->FindChildByClass<T>(nDeep);
	}

    SWindow * GetRoot() const {return m_pRoot;}

	SNativeWnd * GetNative(){return this;}

    HWND Create(HWND hWndParent,int x = 0, int y = 0, int nWidth = 0, int nHeight = 0);

	BOOL WINAPI InitFromXml(IXmlNode * pNode);
    
    BOOL AnimateHostWindow(DWORD dwTime,DWORD dwFlags);

    //实现3个供脚本使用的定时器函数
    UINT setTimeout(LPCSTR pszScriptFunc,UINT uElapse);
    UINT setInterval(LPCSTR pszScriptFunc,UINT uElapse);
    void clearTimer(UINT uID);

    //实现几个常用接口,防止派生类调用时产生歧义
    BOOL DestroyWindow();
    UINT_PTR SetTimer(UINT_PTR nIDEvent,UINT nElapse);
    BOOL KillTimer(UINT_PTR id);
    CRect GetWindowRect() const;
    CRect GetClientRect() const;

	SHostWndAttr & GetHostAttr() {
		return m_hostAttr;
	}
	IToolTip * GetToolTip() const {
		return m_pTipCtrl;
	}

	bool StartHostAnimation(IAnimation *pAni);
	bool StopHostAnimation();
	void UpdateAutoSizeCount(bool bInc);
protected:
	class SHostAnimationHandler : public ITimelineHandler
	{
	public:
		SHostWnd *				m_pHostWnd;
		CRect					m_rcInit;
	protected:
		STDMETHOD_(void,OnNextFrame)(THIS_) OVERRIDE;
	} m_hostAnimationHandler;

	virtual void OnHostAnimationStarted(IAnimation * pAni);
	virtual void OnHostAnimationStoped(IAnimation * pAni);

protected://辅助函数
    void _Redraw();
    void _UpdateNonBkgndBlendSwnd();
    void _RestoreClickState();
	void _Invalidate(LPCRECT prc);

protected:
    //////////////////////////////////////////////////////////////////////////
    // Message handler

    void OnPrint(HDC dc, UINT uFlags);

    void OnPaint(HDC dc);

    BOOL OnEraseBkgnd(HDC dc);

    int OnCreate(LPCREATESTRUCT lpCreateStruct);

    void OnDestroy();

    void OnSize(UINT nType, CSize size);

    void OnMouseMove(UINT nFlags, CPoint point);

    void OnMouseLeave();

    BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);

    void OnTimer(UINT_PTR idEvent);

    LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT OnKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT OnActivateApp(UINT uMsg, WPARAM wParam, LPARAM lParam);

    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

    void OnActivate(UINT nState, BOOL bMinimized, HWND wndOther);
 
    LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam);

    void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);

    BOOL OnNcActivate(BOOL bActive);
    
    UINT OnWndNcHitTest(CPoint point);

    void OnSetFocus(HWND wndOld);
    void OnKillFocus(HWND wndFocus);
        
    void UpdateHost(HDC dc,LPCRECT rc,BYTE byAlpha=255);
    void UpdateLayerFromRenderTarget(IRenderTarget *pRT,BYTE byAlpha, LPCRECT prcDirty=NULL);

    void OnCaptureChanged(HWND wnd);

    LRESULT OnScriptTimer(UINT uMsg,WPARAM wParam,LPARAM lParam);
    
    LRESULT OnMenuExEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

	void OnWindowPosChanging(LPWINDOWPOS lpWndPos);
	void OnWindowPosChanged(LPWINDOWPOS lpWndPos);
        
	LRESULT OnGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnSysCommand(UINT nID, CPoint lParam);

#ifndef DISABLE_SWNDSPY
protected:
    LRESULT OnSpyMsgSetSpy(UINT uMsg,WPARAM wParam,LPARAM lParam);

    LRESULT OnSpyMsgSwndEnum(UINT uMsg,WPARAM wParam,LPARAM lParam);

    LRESULT OnSpyMsgSwndSpy(UINT uMsg,WPARAM wParam,LPARAM lParam);
    
    LRESULT OnSpyMsgHitTest(UINT uMsg,WPARAM wParam,LPARAM lParam);

    HWND    m_hSpyWnd;
#endif
public:
	virtual BOOL ShowWindow(int nCmdShow);

protected:// IContainer

    STDMETHOD_(BOOL,OnFireEvent)(IEvtArgs *evt);

    STDMETHOD_(CRect,GetContainerRect)() const;

    STDMETHOD_(HWND,GetHostHwnd)();

    STDMETHOD_(const SStringW &,GetTranslatorContext)() const;

    STDMETHOD_(IRenderTarget *,OnGetRenderTarget)(LPCRECT rc,GrtFlag gdcFlags);

    STDMETHOD_(void,OnReleaseRenderTarget)(IRenderTarget * pRT,LPCRECT rc,GrtFlag gdcFlags);

    STDMETHOD_(void,OnRedraw)(LPCRECT rc);

    STDMETHOD_(BOOL,OnReleaseSwndCapture)();

    STDMETHOD_(SWND,OnSetSwndCapture)(SWND swnd);

    STDMETHOD_(BOOL,IsTranslucent)() const;
    
    STDMETHOD_(BOOL,IsSendWheel2Hover)() const;

    STDMETHOD_(BOOL,UpdateWindow)();

	STDMETHOD_(void,UpdateTooltip)();

	STDMETHOD_(BOOL,RegisterTimelineHandler)(THIS_ ITimelineHandler *pHandler) OVERRIDE;
	STDMETHOD_(BOOL,UnregisterTimelineHandler)(THIS_ ITimelineHandler *pHandler) OVERRIDE;

    STDMETHOD_(IMessageLoop *,GetMsgLoop)();

    STDMETHOD_(IScriptModule *, GetScriptModule)();

	STDMETHOD_(int,GetScale)() const;

	STDMETHOD_(void,OnCavasInvalidate)(SWND swnd);

	STDMETHOD_(void,EnableIME)(BOOL bEnable);

protected:
	virtual IToolTip * CreateTooltip() const;
	virtual void DestroyTooltip(IToolTip * pTooltip) const;

protected:
	virtual BOOL OnLoadLayoutFromResourceID(const SStringT &resId);
	virtual void OnUserXmlNode(SXmlNode xmlUser);
protected:
	STDMETHOD_(void,OnHostMsg)(THIS_ bool bRelayout,UINT uMsg,WPARAM wp,LPARAM lp) OVERRIDE;
public:
	virtual BOOL onRootResize(IEvtArgs *e);
	
public://事件处理接口
    virtual BOOL _HandleEvent(IEvtArgs *pEvt){return FALSE;}


	void OnHostShowWindow(BOOL bShow, UINT nStatus);
    BEGIN_MSG_MAP_EX(SHostWnd)
		MSG_WM_SHOWWINDOW(OnHostShowWindow)
        MSG_WM_SIZE(OnSize)
        MSG_WM_PRINT(OnPrint)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSEWHEEL(OnMouseWheel)
        MSG_WM_ACTIVATE(OnActivate)
        MSG_WM_SETFOCUS(OnSetFocus)
        MSG_WM_KILLFOCUS(OnKillFocus)
        MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseEvent)
        MESSAGE_RANGE_HANDLER_EX(WM_KEYFIRST, WM_KEYLAST, OnKeyEvent)
        MESSAGE_RANGE_HANDLER_EX(WM_IME_STARTCOMPOSITION,WM_IME_KEYLAST,OnKeyEvent)
        MESSAGE_HANDLER_EX(WM_IME_CHAR, OnKeyEvent)
		MESSAGE_HANDLER_EX(WM_IME_REQUEST, OnKeyEvent)
        MESSAGE_HANDLER_EX(WM_ACTIVATEAPP,OnActivateApp)
        MSG_WM_SETCURSOR(OnSetCursor)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_NCACTIVATE(OnNcActivate)
        MSG_WM_NCCALCSIZE(OnNcCalcSize)
        MSG_WM_NCHITTEST(OnWndNcHitTest)
        MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
        MSG_WM_CAPTURECHANGED(OnCaptureChanged)
        MESSAGE_HANDLER_EX(UM_SCRIPTTIMER,OnScriptTimer)
        MESSAGE_HANDLER_EX(UM_MENUEVENT,OnMenuExEvent)
		MSG_WM_WINDOWPOSCHANGING(OnWindowPosChanging)
		MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
		MESSAGE_HANDLER_EX(WM_GETOBJECT,OnGetObject)
		MSG_WM_SYSCOMMAND(OnSysCommand)
    #ifndef DISABLE_SWNDSPY
        MESSAGE_HANDLER_EX(SPYMSG_SETSPY, OnSpyMsgSetSpy)
        MESSAGE_HANDLER_EX(SPYMSG_SWNDENUM, OnSpyMsgSwndEnum)
        MESSAGE_HANDLER_EX(SPYMSG_SWNDINFO, OnSpyMsgSwndSpy)
        MESSAGE_HANDLER_EX(SPYMSG_HITTEST, OnSpyMsgHitTest)
    #endif
        REFLECT_NOTIFY_CODE(NM_CUSTOMDRAW)
    END_MSG_MAP()
};

EXTERN_C 
HRESULT SOUI_EXP CreateHostWnd(LPCTSTR pszResID,IHostWnd ** ppRet);
}//namespace SOUI
