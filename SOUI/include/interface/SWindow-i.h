#pragma once

#include <interface/sobject-i.h>
#include <interface/sstring-i.h>
#include <interface/SAnimation-i.h>
#include <interface/SRender-i.h>

SNSBEGIN

typedef struct SWNDMSG
{
	UINT uMsg;
	WPARAM wParam;
	LPARAM lParam;
}SWNDMSG, *PSWNDMSG;

interface ISwndContainer;
interface IEvtArgs;

#undef INTERFACE
#define INTERFACE IWindow
DECLARE_INTERFACE_(IWindow,IObject)
{
	STDMETHOD_(SWND,GetSwnd)(THIS) SCONST PURE;

	STDMETHOD_(ILayout *,GetLayout)(THIS) PURE;

	STDMETHOD_(const ILayoutParam *, GetLayoutParam2)(THIS) SCONST PURE;

	STDMETHOD_(ILayoutParam *, GetLayoutParam)(THIS) PURE;

	STDMETHOD_(BOOL,SetLayoutParam)(THIS_ ILayoutParam * pLayoutParam) PURE;

	STDMETHOD_(BOOL,IsFloat)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,IsDisplay)(THIS) SCONST PURE;

	STDMETHOD_(void,SetWindowText)(THIS_ LPCTSTR lpszText) PURE;

	STDMETHOD_(void,SetToolTipText)(THIS_ LPCTSTR pszText) PURE;

	STDMETHOD_(BOOL,IsChecked)(THIS) SCONST PURE;
	STDMETHOD_(void,SetCheck)(THIS_ BOOL bCheck) PURE;

	STDMETHOD_(BOOL,IsDisabled)(THIS_ BOOL bCheckParent) SCONST PURE;
	STDMETHOD_(void,EnableWindow)(THIS_ BOOL bEnable,BOOL bUpdate) PURE;

	STDMETHOD_(BOOL,IsVisible)(THIS_ BOOL bCheckParent) SCONST PURE;
	STDMETHOD_(void,SetVisible)(THIS_ BOOL bVisible,BOOL bUpdate) PURE;

	STDMETHOD_(BOOL,IsMsgTransparent)(THIS) SCONST PURE;

	STDMETHOD_(ULONG_PTR,GetUserData)(THIS) SCONST PURE;
	STDMETHOD_(ULONG_PTR,SetUserData)(THIS_ ULONG_PTR uData) PURE;
	STDMETHOD_(void,GetWindowRect)(THIS_ LPRECT prect) SCONST PURE;
	STDMETHOD_(void,GetClientRect)(THIS_ LPRECT prect) SCONST PURE;
	STDMETHOD_(BOOL,IsContainPoint)(THIS_ POINT pt,BOOL bClientOnly) SCONST PURE;
	STDMETHOD_(void,DoColorize)(THIS_ COLORREF cr) PURE;
	STDMETHOD_(COLORREF,GetColorizeColor)(THIS) SCONST PURE;
	STDMETHOD_(BOOL,Destroy)(THIS) PURE;
	STDMETHOD_(void,BringWindowToTop)(THIS) PURE;
	STDMETHOD_(UINT,GetChildrenCount)(THIS) SCONST PURE;
	STDMETHOD_(LRESULT,SSendMessage)(THIS_ UINT uMsg, WPARAM wParam, LPARAM lParam,BOOL *pbMsgHandled) PURE;
	STDMETHOD_(void,SDispatchMessage)(THIS_ UINT uMsg, WPARAM wParam, LPARAM lParam) PURE;

	STDMETHOD_(void,SetFocus)(THIS) PURE;
	STDMETHOD_(void,KillFocus)(THIS) PURE;
	STDMETHOD_(BOOL,IsFocused)(THIS) SCONST PURE;

	STDMETHOD_(void,Invalidate)(THIS) PURE;
	STDMETHOD_(void,InvalidateRect)(THIS_ LPCRECT lprect) PURE;

	STDMETHOD_(void,LockUpdate)(THIS) PURE;
	STDMETHOD_(void,UnlockUpdate)(THIS) PURE;
	STDMETHOD_(BOOL,IsUpdateLocked)(THIS) SCONST PURE;
	STDMETHOD_(void,Update)(THIS) PURE;
	STDMETHOD_(void,Move)(THIS_ LPCRECT prect) PURE;
	STDMETHOD_(void,SetWindowRgn)(THIS_ IRegion *pRgn,BOOL bRedraw) PURE;
	STDMETHOD_(IRegion*,GetWindowRgn)(THIS) SCONST PURE;
	STDMETHOD_(void,SetWindowPath)(THIS_ IPath *pPath,BOOL bRedraw) PURE;

	STDMETHOD_(BOOL,SetTimer)(THIS_ char id,UINT uElapse) PURE;
	STDMETHOD_(void,KillTimer)(THIS_ char id) PURE;

	STDMETHOD_(SWND,GetCapture)(THIS) PURE;
	STDMETHOD_(SWND,SetCapture)(THIS) PURE;
	STDMETHOD_(BOOL,ReleaseCapture)(THIS) PURE;

	STDMETHOD_(void,SetAnimation)(THIS_ IAnimation * animation) PURE;

	STDMETHOD_(IAnimation *,GetAnimation)(THIS) PURE;

	STDMETHOD_(void,StartAnimation)(THIS_ IAnimation * animation) PURE;

	STDMETHOD_(void,ClearAnimation)(THIS) PURE;

	STDMETHOD_(void,SetAlpha)(THIS_ BYTE byAlpha) PURE;

	STDMETHOD_(BYTE,GetAlpha)(THIS) SCONST PURE;

	STDMETHOD_(int,GetScale)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,IsSiblingsAutoGroupped)(THIS) SCONST PURE;

	STDMETHOD_(void,RequestRelayout)(THIS) PURE;
	STDMETHOD_(void,UpdateLayout)(THIS) PURE;

	STDMETHOD_(UINT,OnGetDlgCode)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,IsFocusable)(THIS) SCONST PURE;

	STDMETHOD_(BOOL,IsClipClient)(THIS) SCONST PURE;
	STDMETHOD_(BOOL,IsLayoutDirty)(THIS) SCONST PURE;
	STDMETHOD_(void,UpdateChildrenPosition)(THIS) PURE;

	STDMETHOD_(IWindow *,GetIWindow)(THIS_ int uCode)  PURE;

	STDMETHOD_(IWindow *,GetIChild)(THIS_ int iChild)  PURE;

	STDMETHOD_(IWindow *,GetIParent)(THIS)  PURE;

	STDMETHOD_(IWindow *,GetIRoot)(THIS)  PURE;

	STDMETHOD_(const IWindow*,GetNextLayoutIChild2)(THIS_ const IWindow* pCurChild) SCONST PURE;
	STDMETHOD_(IWindow*,GetNextLayoutIChild)(THIS_ IWindow* pCurChild) PURE;

	STDMETHOD_(BOOL,IsIDescendant)(THIS_ const IWindow *pTest) SCONST PURE;

	
		/**
         * AdjustZOrder
         * @brief    调整窗口Z序
		 * @param    SWindow *pInsertAfter --  插入在这个窗口之后
         * @return   bool,  pInsertAfter与this非同级窗口返回失败
         *
         * Describe  pInsertAfter可以为NULL，或是与this同一级的兄弟窗口
         */
		STDMETHOD_(BOOL,AdjustIZOrder)(THIS_ IWindow *pInsertAfter) PURE;

		        /**
         * InsertChild
         * @brief    在窗口树中插入一个子窗口
         * @param    SWindow * pNewChild --  子窗口对象
         * @param    SWindow * pInsertAfter --  插入位置
         * @return   void 
         *
         * Describe  一般用于UI初始化的时候创建，插入的窗口不会自动进入布局流程
         */
        STDMETHOD_(void,InsertIChild)(THIS_ IWindow *pNewChild,IWindow *pInsertAfter) PURE;

        /**
         * RemoveChild
         * @brief    从窗口树中移除一个子窗口对象
         * @param    SWindow * pChild --  子窗口对象
         * @return   BOOL 
         *
         * Describe  子窗口不会自动释放
         */
        STDMETHOD_(BOOL,RemoveIChild)(THIS_ IWindow *pChild) PURE;
        
        /**
         * DestroyChild
         * @brief    销毁一个子窗口
         * @param    SWindow * pChild --  子窗口对象
         * @return   BOOL 
         *
         * Describe  先调用RemoveChild，再调用pChild->Release来释放子窗口对象
         */
        STDMETHOD_(BOOL,DestroyIChild)(THIS_ IWindow *pChild) PURE;
        
		STDMETHOD_(void,DestroyAllChildren)(THIS) PURE;

		STDMETHOD_(ISwndContainer*,GetContainer)(THIS) PURE;
		STDMETHOD_(void,SetContainer)(THIS_ ISwndContainer *pContainer) PURE;


        /**
        * GetChildrenLayoutRect
        * @brief    获得子窗口的布局空间
        * @return   CRect 
        *
        * Describe  通常是客户区，但是tab,group这样的控件不一样
        */
		STDMETHOD_(RECT,GetChildrenLayoutRect)(THIS) SCONST PURE;

        /**
        * GetDesiredSize
        * @brief    当没有指定窗口大小时，通过如皮肤计算窗口的期望大小
        * @param    int nParentWid -- 容器宽度，<0代表容器宽度依赖当前窗口宽度
        * @param    int nParentHei -- 容器高度，<0代表容器高度依赖当前窗口高度
        * @return   CSize 
        *
        * Describe  
        */
		STDMETHOD_(SIZE,GetDesiredSize)(THIS_ int nParentWid, int nParentHei) PURE;

		STDMETHOD_(BOOL,OnRelayout)(THIS_ RECT rcWnd) PURE;

		
		/**
        * Move
        * @brief    将窗口移动到指定位置
        * @param    int x --  left
        * @param    int y --  top
        * @param    int cx --  width
        * @param    int cy --  height
        * @return   void 
        *
        * Describe 
        * @see     Move(LPRECT prect)
        */
        STDMETHOD_(void,Move)(THIS_ int x,int y, int cx,int cy) PURE;
		

        /**
        * SetTimer2
        * @brief    利用函数定时器来模拟一个兼容窗口定时器
        * @param    UINT_PTR id --  定时器ID
        * @param    UINT uElapse --  延时(MS)
        * @return   BOOL 
        *
        * Describe  由于SetTimer只支持0-127的定时器ID，SetTimer2提供设置其它timerid
        *           能够使用SetTimer时尽量不用SetTimer2，在Kill时效率会比较低
        */
        STDMETHOD_(BOOL,SetTimer2)(THIS_ UINT_PTR id,UINT uElapse) PURE;

        /**
        * KillTimer2
        * @brief    删除一个SetTimer2设置的定时器
        * @param    UINT_PTR id --  SetTimer2设置的定时器ID
        * @return   void 
        *
        * Describe  需要枚举定时器列表
        */
        STDMETHOD_(void,KillTimer2)(THIS_ UINT_PTR id) PURE;

		STDMETHOD_(int,GetWindowText)(THIS_ TCHAR * pBuf,int nBufLen,BOOL bRawText) PURE;

		STDMETHOD_(void,SetEventMute)(THIS_ BOOL bMute) PURE;

		STDMETHOD_(DWORD,GetState)(THIS) SCONST PURE;
		STDMETHOD_(DWORD,ModifyState)(THIS_ DWORD dwStateAdd, DWORD dwStateRemove,BOOL bUpdate) PURE;

		        /**
        * GetCurMsg
        * @brief    获得当前正在处理的消息
        * @return   PSWNDMSG 
        *
        * Describe  
        */
        STDMETHOD_(PSWNDMSG,GetCurMsg)(THIS) PURE;


		STDMETHOD_(void,SetIOwner)(THIS_ IWindow *pOwner) PURE;
		STDMETHOD_(IWindow *,GetIOwner)(THIS) PURE;

		/**
        * CreateChildren
        * @brief    从XML创建子窗口
        * @param    LPCWSTR pszXml --  合法的utf16编码XML字符串
        * @return   BOOL 是否创建成功
        *
        * Describe  
        */
        STDMETHOD_(BOOL,CreateChildrenFromXml)(THIS_ LPCWSTR pszXml) PURE;


		/**
        * GetISelectedSiblingInGroup
        * @brief    获得在一个group中选中状态的窗口
        * @return   SWindow * 
        *
        * Describe  不是group中的窗口时返回NULL
        */
        STDMETHOD_(IWindow *,GetISelectedSiblingInGroup)(THIS) PURE;

        /**
        * GetSelectedChildInGroup
        * @brief    获取有选择状态的子窗口
        * @return   IWindow * -- 选中状态窗口
        * Describe  
        */    
        STDMETHOD_(IWindow *, GetISelectedChildInGroup)(THIS) PURE;

		STDMETHOD_(BOOL,FireEvent)(THIS_ IEvtArgs *evt) PURE;

		/**
        * FireCommand
        * @brief    激活窗口的EVT_CMD事件
        * @return   BOOL-- true:EVT_CMD事件被处理
        *
        * Describe  
        */
        STDMETHOD_(BOOL,FireCommand)(THIS) PURE;

        /**
        * FireCtxMenu
        * @brief    激活快捷菜单事件
        * @param    CPoint pt --  鼠标点击位置
        * @return   BOOL -- true:外部处理了快捷菜单事件
        *
        * Describe  
        */
        STDMETHOD_(BOOL,FireCtxMenu)(THIS_ POINT pt) PURE;
        

		STDMETHOD_(SWND,SwndFromPoint)(THIS_ POINT *pt,bool bIncludeMsgTransparent) PURE;

		//caret相关方法
		STDMETHOD_(BOOL,CreateCaret)(THIS_ HBITMAP pBmp,int nWid,int nHeight) PURE;
		STDMETHOD_(void,ShowCaret)(THIS_ BOOL bShow) PURE;   
		STDMETHOD_(void,SetCaretPos)(THIS_ int x,int y) PURE;

};

SNSEND