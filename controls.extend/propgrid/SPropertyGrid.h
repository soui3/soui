#pragma once

#include <control/SListbox.h>
#include "SPropertyItemBase.h"
#include "propitem/SPropertyItem-Group.h"
namespace SOUI
{
#define EVT_PG_VALUECHANGED (EVT_EXTERNAL_BEGIN+100)
	//ADD
#define EVT_PG_ITEMCLICK (EVT_EXTERNAL_BEGIN+110)
#define EVT_PG_ITEMACTIVE (EVT_EXTERNAL_BEGIN+101)

	class EventPropGridValueChanged : public TplEventArgs<EventPropGridValueChanged>
	{
		SOUI_CLASS_NAME(EventPropGridValueChanged,L"on_propgrid_value_changed")
	public:
		EventPropGridValueChanged(SObject *pWnd):TplEventArgs<EventPropGridValueChanged>(pWnd){}
		enum{EventID=EVT_PG_VALUECHANGED};

		IPropertyItem * pItem;
	};

	class EventPropGridItemClick : public TplEventArgs<EventPropGridItemClick>
	{
		SOUI_CLASS_NAME(EventPropGridItemClick,L"on_propgrid_item_click")
	public:
		EventPropGridItemClick(SObject *pWnd):TplEventArgs<EventPropGridItemClick>(pWnd){}
		enum{EventID=EVT_PG_ITEMCLICK};

		IPropertyItem * pItem;
	};


	class EventPropGridItemActive : public TplEventArgs<EventPropGridItemActive>
	{
		SOUI_CLASS_NAME(EventPropGridItemActive,L"on_propgrid_item_active")
	public:
		EventPropGridItemActive(SObject *pWnd):TplEventArgs<EventPropGridItemActive>(pWnd){}
		enum{EventID=EVT_PG_ITEMACTIVE};

		IPropertyItem * pItem;
	};


#define IG_FIRST (SPropertyGroup*)0
#define IG_LAST  (SPropertyGroup*)1

	class SPropertyGrid : public SListBox
	{
		SOUI_CLASS_NAME(SPropertyGrid, L"propgrid")
	public:
		enum EXPSTATE
		{
			GROUP_EXPANDED,
			GROUP_COLLAPSED,
			ITEM_EXPANDED,
			ITEM_EXCOLLAPSED,
		};

		enum ORDERTYPE
		{
			OT_NULL,
			OT_GROUP,
			OT_NAME,
		};
		SPropertyGrid(void);
		~SPropertyGrid(void);

		int GetIndent();

		void OnItemExpanded(IPropertyItem *pItem);
		CRect GetItemRect(IPropertyItem *pItem) const;

		void SortInsert(IPropertyItem *pItem);

		BOOL InsertGroup(SPropertyGroup * pGroup,SPropertyGroup* pInertAfter=IG_LAST);

		void LoadFromXml(pugi::xml_node data);

		void RemoveAllItems();
		IPropertyItem * FindItemByName(const SStringT & strName) const;
		IPropertyItem * FindChildById(int nID) const;

		IPropertyItem * CreateItem(const SStringW& strName);
		pugi::xml_node GetInplaceItemStyle(LPCWSTR pszStyle);

		BOOL IsItemVisible(IPropertyItem *pItem) const;
		BOOL SetItemAttribute(IPropertyItem * pItem,const SStringW & attr,const SStringW & value);

		COLORREF GetGroupColor() const;

		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"indent",m_nIndent,TRUE)
			ATTR_INT(L"titleWidth",m_nTitleWidth,TRUE)
			ATTR_ENUM_BEGIN(L"orderType",ORDERTYPE,TRUE)
				ATTR_ENUM_VALUE(L"null",OT_NULL)
				ATTR_ENUM_VALUE(L"group",OT_GROUP)
				ATTR_ENUM_VALUE(L"name",OT_NAME)
			ATTR_ENUM_END(m_orderType)
			ATTR_SKIN(L"switchSkin",m_switchSkin,TRUE)
			ATTR_COLOR(L"colorGroup",m_crGroup,FALSE)
			ATTR_COLOR(L"colorItem",m_crItem,FALSE)
			ATTR_COLOR(L"colorItemText",m_crItemText,FALSE)
			ATTR_COLOR(L"colorItemSel",m_crItemSel,FALSE)
			ATTR_COLOR(L"colorItemBorder",m_crItemBorder,FALSE)
		SOUI_ATTRS_END()

	protected:
		enum ITEMPART
		{
			IP_NULL,
			IP_SWITCH,
			IP_TITLE,
			IP_VALUE,
		};
		ITEMPART HitTest(int iItem, const CPoint &pt);
		void SortItems(SList<IPropertyItem*> & lstItems);

		virtual BOOL CreateChildren(pugi::xml_node xmlNode);

		virtual void DrawItem(IRenderTarget *pRT, CRect &rc, int iItem);
		virtual UINT OnGetDlgCode(){return SC_WANTALLKEYS;}
		virtual BOOL OnSetCursor(const CPoint &pt);
		virtual BOOL OnScroll(BOOL bVertical,UINT uCode,int nPos);


		void OnLButtonDown(UINT nFlags,CPoint pt);
		void OnLButtonUp(UINT nFlags,CPoint pt);
		void OnMouseMove(UINT nFlags,CPoint pt);
		void OnLButtonDbClick(UINT nFlags, CPoint point);
		void OnSize(UINT nType, CSize size);
		void OnDestroy();

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_LBUTTONDBLCLK(OnLButtonDbClick)
			MSG_WM_SIZE(OnSize)
			MSG_WM_DESTROY(OnDestroy)
		SOUI_MSG_MAP_END()
	public:
		void OnInplaceActiveWndCreate(IPropertyItem *pItem,SWindow *pWnd,pugi::xml_node xmlInit);
		void OnInplaceActiveWndDestroy(IPropertyItem *pItem,SWindow *pWnd);
		void OnItemValueChanged(IPropertyItem *pItem);
		void OnItemInvalidate(IPropertyItem *pItem);
		void OnItemButtonClick(IPropertyItem *pItem);
	protected:
		SWindow *   m_pInplaceActiveWnd;    //属性内嵌的窗口

	protected:
		bool OnSelChanged(EventArgs *pEvt);
		bool OnCmdBtnClicked(EventArgs *pEvt);

		int ExpandChildren(const IPropertyItem *pItem,int iInsert);
		void CollapseChildren(const IPropertyItem *pItem,int idx);

		int IndexOfPropertyItem(const IPropertyItem *pItem) const;
		enum {CHILD_CMDBTN=1,CHILD_INPLACEWND=2};
		void UpdateChildrenPos(UINT childs=CHILD_CMDBTN|CHILD_INPLACEWND);
		BOOL IsInplaceItemStyle(const SStringW& strName);
	protected:
		int m_nIndent;          //缩进大小
		int m_nTitleWidth;    //属性名占用空间
		ORDERTYPE   m_orderType;
		SList<SPropertyGroup *> m_lstGroup; //根分类列表
		ISkinObj  *  m_switchSkin;
		SWindow   *  m_pCmdBtn; //有弹出按钮的表项使用的按钮

		CPoint      m_ptDrag;
		BOOL        m_bDraging;

		COLORREF m_crGroup;       //Group背景颜色
		COLORREF m_crItem;        //Item背景颜色
		COLORREF m_crItemText;    //Item文本颜色
		COLORREF m_crItemSel;     //Item选中时的背景色
		COLORREF m_crItemBorder;      //边框颜色

		pugi::xml_document m_inplaceItemProp;
	};

}
