#pragma once

#include "core/Swnd.h"
#include "core/SItemPanel.h"
#include "interface/SAdapter-i.h"
#include "interface/STreeViewItemLocator-i.h"

namespace SOUI
{
    //////////////////////////////////////////////////////////////////////////
    class SOUI_EXP STreeViewItemLocator : public TObjRefImpl<ITreeViewItemLocator>
    {
    public:
        STreeViewItemLocator(int nIndent=10);

        ~STreeViewItemLocator();

	public:
		STDMETHOD_(void,SetAdapter)(THIS_ ITvAdapter *pAdapter) OVERRIDE;
		STDMETHOD_(void,OnBranchChanged)(THIS_ HTREEITEM hItem) OVERRIDE;
		STDMETHOD_(void,OnBranchExpandedChanged)(THIS_ HTREEITEM hItem,BOOL bExpandedOld,BOOL bExpandedNew) OVERRIDE;

		STDMETHOD_(int,GetTotalHeight)(THIS) SCONST OVERRIDE;
		STDMETHOD_(int,GetTotalWidth)(THIS) SCONST OVERRIDE;
		STDMETHOD_(int,Item2Position)(THIS_ HTREEITEM hItem) SCONST OVERRIDE;
		STDMETHOD_(HTREEITEM,Position2Item)(THIS_ int position) SCONST OVERRIDE;
		STDMETHOD_(int,GetScrollLineSize)(THIS) SCONST OVERRIDE;

		STDMETHOD_(void,SetItemWidth)(THIS_ HTREEITEM hItem,int nWidth) OVERRIDE;
		STDMETHOD_(int,GetItemWidth)(THIS_ HTREEITEM hItem) SCONST OVERRIDE;
		STDMETHOD_(void,SetItemHeight)(THIS_ HTREEITEM hItem,int nHeight) OVERRIDE;
		STDMETHOD_(int,GetItemHeight)(THIS_ HTREEITEM hItem) SCONST OVERRIDE;
		STDMETHOD_(int,GetItemIndent)(THIS_ HTREEITEM hItem) SCONST OVERRIDE;

    protected:
        BOOL IsItemExpanded(HTREEITEM hItem) const;

        //更新hItem所在的父窗口中分枝宽度数据
        //hItem:显示宽度发生变化的节点，可以是节点本身宽度变化，也可能是子节点宽度发生了变化
        //nOldWidth：原显示宽度
        //nNewWidth: 新显示宽度
        void _UpdateBranchWidth(HTREEITEM hItem,int nOldWidth,int nNewWidth);

        int _GetBranchWidth(HTREEITEM hBranch) const;

        void _SetBranchWidth(HTREEITEM hBranch,int nWidth);

        void _SetItemWidth(HTREEITEM hItem,int nWidth);


        int _GetBranchHeight(HTREEITEM hItem) const;

        void _SetBranchHeight(HTREEITEM hItem ,int nHeight);

        void _UpdateBranchHeight(HTREEITEM hItem,int nDiff);

        //向后更新兄弟结点的偏移量
        void _UpdateSiblingsOffset(HTREEITEM hItem);

        int _GetItemOffset(HTREEITEM hItem) const;

        void _SetItemOffset(HTREEITEM hItem, int nOffset);

        void _SetItemHeight(HTREEITEM hItem,int nHeight);


        int _GetItemVisibleHeight(HTREEITEM hItem) const;

        int _GetItemVisibleWidth(HTREEITEM hItem) const;

        HTREEITEM _Position2Item(int position,HTREEITEM hParent,int nParentPosition) const;

        BOOL _IsItemVisible(HTREEITEM hItem) const;

        void _InitBranch(HTREEITEM hItem);


        SAutoRefPtr<ITvAdapter> m_adapter;
        int                     m_nLineHeight;
        int                     m_nIndent;
        CSize                   m_szDef;
    };


	class SOUI_EXP STreeView : public SPanel, protected IItemContainer
	{
		SOUI_CLASS_NAME(STreeView, L"treeview")
		friend class STreeViewDataSetObserver;
	public:
		STreeView();
		~STreeView();
			
		BOOL SetAdapter(ITvAdapter * adapter);
		ITvAdapter * GetAdapter () { return m_adapter; }
		void SetItemLocator(ITreeViewItemLocator *pItemLocator)
		{
		    m_tvItemLocator = pItemLocator;
		}
		
		ITreeViewItemLocator * GetItemLocator() const
		{
		    return m_tvItemLocator;
		}
		
        
        void SetSel(HTREEITEM hItem,BOOL bNotify=FALSE);
        HTREEITEM  GetSel()const{return m_hSelected;}
        SItemPanel * HitTest(CPoint & pt);

		void EnsureVisible(HTREEITEM hItem);

    protected:
        bool OnItemClick(IEvtArgs *pEvt);
        bool OnItemDblClick(IEvtArgs *pEvt);

		void onBranchChanged(HTREEITEM hBranch);
		void onBranchInvalidated(HTREEITEM hBranch,bool bInvalidParents, bool bInvalidChildren);
		void onBranchExpandedChanged(HTREEITEM hBranch,BOOL bExpandedOld,BOOL bExpandedNew);

    protected:
		void OnPaint(IRenderTarget * pRT);
		void OnSize(UINT nType, CSize size);
		void OnDestroy();
 		void OnKeyDown(TCHAR nChar, UINT nRepCnt, UINT nFlags);//实现键盘选择
		LRESULT OnKeyEvent( UINT uMsg,WPARAM wParam,LPARAM lParam );
		
		void OnMouseLeave();
		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		LRESULT OnMouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

		void OnKillFocus(SWND wndFocus);
		void OnSetFocus(SWND wndOld);
		

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_SIZE(OnSize)
			MSG_WM_DESTROY(OnDestroy)
			MSG_WM_KEYDOWN(OnKeyDown)
			MSG_WM_KILLFOCUS_EX(OnKillFocus)
			MSG_WM_SETFOCUS_EX(OnSetFocus)
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
			MSG_WM_MOUSELEAVE(OnMouseLeave)
			MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,WM_MOUSELAST,OnMouseEvent)
 			MESSAGE_RANGE_HANDLER_EX(WM_KEYFIRST,WM_KEYLAST,OnKeyEvent)
 			MESSAGE_RANGE_HANDLER_EX(WM_IME_STARTCOMPOSITION,WM_IME_KEYLAST,OnKeyEvent)
			MESSAGE_HANDLER_EX(WM_IME_CHAR, OnKeyEvent)
			MESSAGE_HANDLER_EX(WM_IME_REQUEST,OnKeyEvent)
		SOUI_MSG_MAP_END()
		
    protected:
        HRESULT OnAttrIndent(const SStringW & strValue,BOOL bLoading);
        
        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"indent",OnAttrIndent)
            ATTR_INT(L"wantTab", m_bWantTab,FALSE)
        SOUI_ATTRS_END()
	protected:
		virtual UINT WINAPI OnGetDlgCode () const;
        virtual BOOL OnSetCursor(const CPoint &pt);
        virtual BOOL UpdateToolTip(CPoint pt, SwndToolTipInfo & tipInfo);
        virtual void OnColorize(COLORREF cr);
		virtual void OnScaleChanged(int nScale);
		virtual HRESULT OnLanguageChanged();
		virtual void OnRebuildFont();

		virtual BOOL OnScroll(BOOL bVertical,UINT uCode,int nPos);
		virtual int  GetScrollLineSize(BOOL bVertical);
		virtual BOOL CreateChildren(SXmlNode xmlNode);
	
		virtual void OnItemSetCapture(SItemPanel *pItem,BOOL bCapture); //设置or释放鼠标捕获
		virtual BOOL OnItemGetRect(const SItemPanel *pItem,CRect &rcItem) const;    //获得表项的显示位置
		virtual BOOL IsItemRedrawDelay() const;                               //指示表项的更新方式
		virtual void OnItemRequestRelayout(SItemPanel *pItem);          //列表项请求重新布局
	protected:

		void UpdateScrollBar();
		void UpdateVisibleItems();


		void RedrawItem(SItemPanel *pItem);
		SItemPanel * GetItemPanel(HTREEITEM hItem);

		void DispatchMessage2Items(UINT uMsg,WPARAM wParam,LPARAM lParam);
	protected:
		SAutoRefPtr<ITvAdapter>		m_adapter;
		SAutoRefPtr<ITvDataSetObserver>   m_observer;
		SAutoRefPtr<ITreeViewItemLocator> m_tvItemLocator;
        
        struct ItemInfo
        {
            SItemPanel* pItem;
            int         nType;
        };
        SArray<SList<SItemPanel*> *>    m_itemRecycle;//item回收站,每一种样式在回收站中保持一个列表，以便重复利用
		SList<ItemInfo> m_visible_items; //可见元素
		
		typedef SMap<HTREEITEM,ItemInfo> VISIBLEITEMSMAP;
		VISIBLEITEMSMAP * m_pVisibleMap;
		
		SXmlDoc m_xmlTemplate;

		SItemPanel * m_itemCapture;
		SItemPanel * m_pHoverItem;
        
        HTREEITEM    m_hSelected;               /**< 当前选择项 */ 
        
        BOOL            m_bWantTab;             /**< want tab */ 
    };
}