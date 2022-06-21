#include "StdAfx.h"
#include "SPropertyGrid.h"
#include "propitem/SPropertyItem-Text.h"
#include "propitem/SPropertyItem-Option.h"
#include "propitem/SPropertyItem-Color.h"
#include "propitem/SPropertyItem-Size.h"
#include "propitem/SPropertyItem-Rect.h"

namespace SOUI
{
	//////////////////////////////////////////////////////////////////////////
	IPropertyItem * SPropertyGrid::CreateItem(const SStringW& strName)
	{
		if(strName.CompareNoCase(SPropertyItemText::GetClassName())==0)
			return new SPropertyItemText(this);
		if(strName.CompareNoCase(SPropertyItemOption::GetClassName())==0)
			return new SPropertyItemOption(this);
		if(strName.CompareNoCase(SPropertyItemColor::GetClassName())==0)
			return new SPropertyItemColor(this);
		if(strName.CompareNoCase(SPropertyItemSize::GetClassName())==0)
			return new SPropertyItemSize(this);
		if(strName.CompareNoCase(SPropertyItemRect::GetClassName())==0)
			return new SPropertyItemRect(this);
		if(strName.CompareNoCase(SPropertyGroup::GetClassName())==0)
			return new SPropertyGroup(this);
		return NULL;
	}

	BOOL SPropertyGrid::IsInplaceItemStyle(const SStringW& strName)
	{
		static const LPCWSTR kInplaceStyleNames[]={
			SPropertyItemText::GetInplaceItemStyleName(),
			SPropertyItemOption::GetInplaceItemStyleName(),
			SPropertyItemColor::GetInplaceItemStyleName(),
			SPropertyItemSize::GetInplaceItemStyleName(),
			SPropertyItemRect::GetInplaceItemStyleName(),
			SPropertyGroup::GetInplaceItemStyleName(),
		};
		for(int i=0;i<ARRAYSIZE(kInplaceStyleNames);i++)
		{
			if(kInplaceStyleNames[i] && strName.CompareNoCase(kInplaceStyleNames[i])==0)
				return TRUE;
		}
		return FALSE;
	}


	pugi::xml_node SPropertyGrid::GetInplaceItemStyle(LPCWSTR pszStyle)
	{
		return m_inplaceItemProp.root().child(pszStyle);
	}

    //////////////////////////////////////////////////////////////////////////
    SPropertyGrid::SPropertyGrid(void)
    :m_nTitleWidth(100)
    ,m_switchSkin(NULL)
    ,m_bDraging(FALSE)
    ,m_pInplaceActiveWnd(NULL)
	,m_crGroup(RGBA(233,236,250,255))
	,m_crItemBorder(m_crGroup)
	,m_crItem(CR_INVALID)
	,m_crItemText(RGBA(0,0,0,255))
	,m_crItemSel(RGBA(0,120,215,255))
	,m_orderType(OT_GROUP)
    {
		//注册事件   
		//1、item的值改变的时候响应
		//2、Sel改变的时候响应
        GetEventSet()->addEvent(EVENTID(EventPropGridValueChanged));
        GetEventSet()->addEvent(EVENTID(EventPropGridItemButtonClick));
        GetEventSet()->addEvent(EVENTID(EventPropGridItemActive));
		GetEventSet()->addEvent(EVENTID(EventPropGridItemInplaceInit));
		
        GetEventSet()->subscribeEvent(EventLBSelChanged::EventID,Subscriber(&SPropertyGrid::OnSelChanged,this));
    }

    SPropertyGrid::~SPropertyGrid(void)
    {
        SPOSITION pos = m_lstGroup.GetHeadPosition();
        while(pos)
        {
            SPropertyGroup * pGroup = m_lstGroup.GetNext(pos);
            pGroup->Release();        
        }
        m_lstGroup.RemoveAll();
    }

    void SPropertyGrid::OnItemExpanded( IPropertyItem *pItem)
    {
        int iInsert = IndexOfPropertyItem(pItem);
        if(pItem->IsExpand())
        {
            ExpandChildren(pItem,iInsert);
        }else
        {
            CollapseChildren(pItem,iInsert);
        }
    }

    int SPropertyGrid::IndexOfPropertyItem( const IPropertyItem *pItem ) const
    {
        for(int i=0;i<GetCount();i++)
        {
            if(pItem == (IPropertyItem *)GetItemData(i)) return i;
        }
        return -1;
    }
    
    CRect SPropertyGrid::GetItemRect( IPropertyItem *pItem ) const
    {
        int idx = IndexOfPropertyItem(pItem);
        SASSERT(idx != -1);
        int iTopIdx = GetTopIndex();
        
		int nItemHei = m_itemHeight.toPixelSize(GetScale());
        int nPageItems = (m_rcClient.Height()+nItemHei-1)/nItemHei+1;
        if(iTopIdx + nPageItems > GetCount()) nPageItems = GetCount() - iTopIdx;
        CRect rcItem;
        if(idx >= iTopIdx && idx <= iTopIdx+nPageItems)
        {
            rcItem = CRect(0,0,m_rcClient.Width(),nItemHei);
            rcItem.OffsetRect(0,nItemHei*idx-m_ptOrigin.y);
            rcItem.OffsetRect(m_rcClient.TopLeft());
        }
        return rcItem;
    }

    void SPropertyGrid::SortInsert( IPropertyItem *pItem )
    {
        int iInsert = -1;
        for(int i=0;i<GetCount();i++)
        {
            IPropertyItem *p = (IPropertyItem *)GetItemData(i);
            if(pItem->GetName2()<p->GetName2()) 
            {
                iInsert = i;
                break;
            }
        }
        InsertString(iInsert,NULL,-1,(LPARAM)pItem);
    }

    BOOL SPropertyGrid::InsertGroup( SPropertyGroup * pGroup,SPropertyGroup* pInertAfter/*=IG_LAST*/ )
    {
        SPOSITION pos = m_lstGroup.Find(pGroup);
        if(pos) return FALSE;
        if(pInertAfter == IG_FIRST)
        {
            m_lstGroup.InsertBefore(0,pGroup);
        }else if(pInertAfter == IG_LAST)
        {
            m_lstGroup.InsertAfter(0,pGroup);
        }else
        {
            pos = m_lstGroup.Find(pInertAfter);
            if(!pos) return FALSE;
            m_lstGroup.InsertAfter(pos,pGroup);
        }
        pGroup->AddRef();

        switch(m_orderType)
        {
        case OT_GROUP:
            {
                int iInserted = InsertString(-1,NULL,-1,(LPARAM)pGroup);
                if(pGroup->IsExpand())
                {
                    ExpandChildren(pGroup,iInserted);
                }
            }
            break;
        case OT_NAME:
            {
                IPropertyItem *pChild=pGroup->GetItem(IPropertyItem::GPI_FIRSTCHILD);
                while(pChild)
                {
                    SortInsert(pChild);
                    pChild = pChild->GetItem(IPropertyItem::GPI_NEXTSIBLING);
                }
                //展开子项s
                pChild=pGroup->GetItem(IPropertyItem::GPI_FIRSTCHILD);
                while(pChild)
                {
                    if(pChild->ChildrenCount() && pChild->IsExpand())
                    {
                        int iInsert = IndexOfPropertyItem(pChild);
                        ExpandChildren(pChild,iInsert);
                    }
                    pChild = pChild->GetItem(IPropertyItem::GPI_NEXTSIBLING);
                }

            }
            break;
        }
        return TRUE;
    }

    int SPropertyGrid::ExpandChildren( const IPropertyItem *pItem,int iInsert )
    {
        SASSERT(pItem->IsExpand());
        SASSERT(iInsert != -1);
        int nRet =0;
        SList<IPropertyItem*> lstChilds;
        IPropertyItem *pChild = pItem->GetItem(IPropertyItem::GPI_FIRSTCHILD);
        while(pChild)
        {
            lstChilds.AddTail(pChild);
            pChild = pChild->GetItem(IPropertyItem::GPI_NEXTSIBLING);
        }

        if(m_orderType == OT_NAME)
        {
            SortItems(lstChilds);
        }

        SPOSITION pos = lstChilds.GetHeadPosition();
        while(pos)
        {
            IPropertyItem *pChild = lstChilds.GetNext(pos);
            InsertString(++iInsert,NULL,-1,(LPARAM)pChild);
            nRet ++;
            if(pChild->ChildrenCount() && pChild->IsExpand())
            {
                int nAdded = ExpandChildren(pChild,iInsert);
                nRet += nAdded;
                iInsert += nAdded;
            }
        }

        return nRet;
    }

    void SPropertyGrid::CollapseChildren( const IPropertyItem *pItem,int idx)
    {
        int nChilds = pItem->ChildrenCount();
        for(int i=0;i<nChilds;i++)
        {
            IPropertyItem *pChild = (IPropertyItem *)GetItemData(idx+1);
            if(pChild->ChildrenCount() && pChild->IsExpand())
            {
                CollapseChildren(pChild,idx+1);
            }
            DeleteString(idx+1);
        }
    }

    void SPropertyGrid::DrawItem( IRenderTarget *pRT, CRect &rc, int iItem )
    {
		IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);

		CRect rcSwitch = rc;
		CRect rcTitle = rc;
		rcSwitch.right = rcSwitch.left +rcSwitch.Height();
		rcTitle.left = rcSwitch.right;
		rcTitle.right = rcTitle.left + m_nTitleWidth;
		pRT->FillSolidRect(rcSwitch,m_crGroup);
		pRT->FillSolidRect(rcTitle,iItem == SListBox::GetCurSel()? m_crItemSel:(pItem->GetType()==PT_GROUP?m_crGroup:m_crItem));
		int iLevel = pItem->GetLevel();
		if(iLevel>1) rcSwitch.OffsetRect(rcSwitch.Width()*(iLevel-1),0);
		if(pItem->ChildrenCount() && m_switchSkin)
		{
			int iState = pItem->IsExpand()?GROUP_EXPANDED:GROUP_COLLAPSED;
			if(pItem->GetType()!=PT_GROUP) iState += 2;
			CRect rcDraw = rcSwitch;
			rcDraw.DeflateRect((rcSwitch.Size()-m_switchSkin->GetSkinSize())/2);
			m_switchSkin->DrawByIndex(pRT,rcDraw,iState);
		}

		CRect rcText = rcTitle;
		rcText.left = rcSwitch.right;

		SStringT strTitle = pItem->GetTitle();
		pRT->DrawText(strTitle,strTitle.GetLength(),rcText,DT_SINGLELINE|DT_VCENTER);
		CRect rcItem = rc;
		rcItem.left= rcTitle.right+1;
		if(pItem->HasButton()) rcItem.right -= rcItem.Height();


		pItem->DrawItem(pRT,rcItem); //绘制Item

		SAutoRefPtr<IPen> pen,oldPen;
		BOOL bAntiAlias=pRT->SetAntiAlias(FALSE);
		pRT->CreatePen(PS_SOLID,m_crItemBorder,1,&pen);
		pRT->SelectObject(pen,(IRenderObj**)&oldPen);
		CPoint pts[2]={CPoint(rc.left+rc.Height(),rc.bottom-1),CPoint(rc.right,rc.bottom-1)};
		pRT->DrawLines(pts,2);
		CPoint pts2[2]={CPoint(rcTitle.right,rcTitle.top),rcTitle.BottomRight()};
		pRT->DrawLines(pts2,2);
		pRT->SelectObject(oldPen);
		pRT->SetAntiAlias(bAntiAlias);
    }

    void SPropertyGrid::OnLButtonDbClick( UINT nFlags, CPoint point )
    {
        SListBox::OnLButtonDbClick(nFlags,point);
        int iItem = SListBox::HitTest(point);
        if(iItem != -1)
        {
            IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);
            if(pItem->ChildrenCount())
            {
                pItem->Expand(!pItem->IsExpand());
            }else if(pItem->GetType()!=PT_GROUP && !pItem->IsInplaceActive()) 
            {
                pItem->OnInplaceActive(TRUE);
            }
        }
    }

    BOOL SPropertyGrid::CreateChildren( pugi::xml_node xmlNode )
    {
        pugi::xml_node xmlCmdBtn = xmlNode.child(L"cmdbtnstyle");
        SASSERT(xmlCmdBtn);
        m_pCmdBtn = SApplication::getSingleton().CreateWindowByName(SWindow::GetClassName());
        InsertChild(m_pCmdBtn);
        m_pCmdBtn->InitFromXml(xmlCmdBtn);
        m_pCmdBtn->SetVisible(FALSE);
        m_pCmdBtn->GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SPropertyGrid::OnCmdBtnClicked,this));
        
		pugi::xml_node xmlProp = xmlNode.first_child();
		while(xmlProp)
		{
			if(IsInplaceItemStyle(xmlProp.name()))
			{
				m_inplaceItemProp.root().append_copy(xmlProp);
			}
			xmlProp = xmlProp.next_sibling();
		}
        pugi::xml_node xmlGroups = xmlNode.child(L"groups");
		LoadFromXml(xmlGroups);
        return TRUE;
    }

    void SPropertyGrid::OnSize( UINT nType, CSize size )
    {
        __super::OnSize(nType,size);
        UpdateChildrenPos();
    }

    SPropertyGrid::ITEMPART SPropertyGrid::HitTest(int iItem,const CPoint &pt )
    {
        if(iItem==-1) return IP_NULL;
        IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);
        CRect rcItem=GetItemRect(pItem);
        int iLevel = pItem->GetLevel();
        CRect rcSwitch = rcItem;
        rcSwitch.right = rcSwitch.left+ rcSwitch.Height();
        if(iLevel>1) rcSwitch.OffsetRect(rcSwitch.Width()*(iLevel-1),0);
        if(pt.x<rcSwitch.left) return IP_NULL;
        if(pt.x<rcSwitch.right && pItem->ChildrenCount()) return IP_SWITCH;
        CRect rcName = rcItem;
        rcName.right = rcItem.left + m_nTitleWidth + rcItem.Height();
        if(pt.x<rcName.right) return IP_TITLE;
        return IP_VALUE;
    }

    void SPropertyGrid::OnLButtonDown( UINT nFlags,CPoint pt )
    {
        CRect rcClient;
        GetClientRect(&rcClient);
		int nItemHei = m_itemHeight.toPixelSize(GetScale());
        if(pt.x-rcClient.left>=nItemHei+m_nTitleWidth-1
        && pt.x-rcClient.left<=nItemHei+m_nTitleWidth+1)
        {
            SWindow::OnLButtonDown(nFlags,pt);
            m_ptDrag = pt;
            m_bDraging = TRUE;
        }else
        {
            SListBox::OnLButtonDown(nFlags,pt);
            CPoint pt2(pt);
            int iItem = SListBox::HitTest(pt2);
            if(iItem!=-1)
            {
                ITEMPART ip = HitTest(iItem,pt);
				if (ip == IP_SWITCH)
				{
					IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);
					if (pItem->ChildrenCount())
					{
						pItem->Expand(!pItem->IsExpand());
					}
                }else if (ip == IP_VALUE)
                {
                    IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);

					if(!pItem->IsReadOnly()){
						EventPropGridItemActive evt(this);
						evt.pItem = pItem;
						FireEvent(evt);

						pItem->OnInplaceActive(TRUE);
					}

                }else if (ip == IP_TITLE)
				{
					IPropertyItem *pItem = (IPropertyItem*)GetItemData(iItem);
					if(!pItem->IsReadOnly()){
						EventPropGridItemActive evt(this);
						evt.pItem = pItem;
						FireEvent(evt);
					}
				}
            }
        }

    }

    void SPropertyGrid::SortItems( SList<IPropertyItem*> & lstItems )
    {
        SList<IPropertyItem*> lstCpy;
        SPOSITION pos = lstItems.GetHeadPosition();
        while(pos)
        {
            IPropertyItem *pItem = lstItems.GetNext(pos);
            SPOSITION pos2 = lstCpy.GetHeadPosition();
            while(pos2)
            {
                IPropertyItem* pItem2=lstCpy.GetAt(pos2);
                if(pItem->GetName()<pItem2->GetName())
                {
                    lstCpy.InsertBefore(pos2,pItem);
                    break;
                }
                lstCpy.GetNext(pos2);
            }
            if(!pos2)
            {
                lstCpy.InsertAfter(NULL,pItem);
            }
        }
        lstItems.RemoveAll();
        pos = lstCpy.GetHeadPosition();
        while(pos)
        {
            IPropertyItem *pItem = lstItems.GetNext(pos);
            lstItems.InsertAfter(NULL,pItem);
        }
    }

    BOOL SPropertyGrid::OnSetCursor( const CPoint &pt )
    {
        CRect rcClient;
        GetClientRect(&rcClient);
		int nItemHei = m_itemHeight.toPixelSize(GetScale());
        if(m_bDraging 
        ||  (pt.x-rcClient.left>=nItemHei+m_nTitleWidth-1
            && pt.x-rcClient.left<=nItemHei+m_nTitleWidth+1))
        {
            SetCursor(SApplication::getSingleton().LoadCursor(MAKEINTRESOURCE(IDC_SIZEWE)));
        }else
        {
            CPoint pt2(pt);
            int iItem = SListBox::HitTest(pt2);
            if(iItem<0) return FALSE;
            ITEMPART ip = HitTest(iItem,pt);
            if(ip==IP_SWITCH)
                SetCursor(SApplication::getSingleton().LoadCursor(MAKEINTRESOURCE(IDC_HAND)));
            else
                return FALSE;            
        }
        return TRUE;
    }

    void SPropertyGrid::OnLButtonUp( UINT nFlags,CPoint pt )
    {
        if(m_bDraging)
        {
            m_bDraging = FALSE;
            SWindow::OnLButtonUp(nFlags,pt);
        }else
        {
            SListBox::OnLButtonUp(nFlags,pt);
        }
    }

    void SPropertyGrid::OnMouseMove( UINT nFlags,CPoint pt )
    {
        SListBox::OnMouseMove(nFlags,pt);
        if(m_bDraging)
        {
            m_nTitleWidth += pt.x-m_ptDrag.x;
            m_ptDrag = pt;
            Invalidate();
        }
    }

	void SPropertyGrid::OnDestroy()
	{
		RemoveAllItems();
		__super::OnDestroy();
	}

    bool SPropertyGrid::OnSelChanged( EventArgs *pEvt )
    {
        UpdateChildrenPos(CHILD_CMDBTN);
        return true;
    }

    bool SPropertyGrid::OnCmdBtnClicked( EventArgs *pEvt )
    {
        int nCurSel = GetCurSel();
        SASSERT(nCurSel!=-1);
        IPropertyItem *pItem =(IPropertyItem*)GetItemData(nCurSel);
        SASSERT(pItem->HasButton());
		if(!pItem->IsReadOnly())
		{
			OnItemButtonClick(pItem);
		}
        return true;
    }
    
    BOOL SPropertyGrid::OnScroll( BOOL bVertical,UINT uCode,int nPos )
    {
        BOOL bRet = SListBox::OnScroll(bVertical,uCode,nPos);
        UpdateChildrenPos();
        return bRet;
    }

    void SPropertyGrid::OnInplaceActiveWndCreate( IPropertyItem *pItem,SWindow *pWnd ,pugi::xml_node xmlInit)
    {
		SASSERT(m_pInplaceActiveWnd == NULL);
		InsertChild(pWnd);
		pWnd->InitFromXml(xmlInit);

		CRect rcValue = GetInplaceWndPos(pItem);
		pWnd->Move(rcValue);
		m_pInplaceActiveWnd = pWnd;

		EventPropGridItemInplaceInit evt(this);
		evt.pItem = pItem;
		evt.pInplaceWnd = pWnd;
		FireEvent(evt);
    }

    void SPropertyGrid::OnInplaceActiveWndDestroy( IPropertyItem *pItem,SWindow *pWnd )
    {
        SASSERT(m_pInplaceActiveWnd == pWnd);
		pWnd->SSendMessage(WM_DESTROY);
		RemoveChild(pWnd);
        m_pInplaceActiveWnd = NULL;
    }

    void SPropertyGrid::UpdateChildrenPos(UINT childs)
    {
        int nCurSel = GetCurSel();
        if(nCurSel==-1) return;
        
        IPropertyItem * pItem = (IPropertyItem*)GetItemData(nCurSel);
        
        if(m_pInplaceActiveWnd && childs&CHILD_INPLACEWND)
        {
			CRect rcValue = GetInplaceWndPos(pItem);
            m_pInplaceActiveWnd->Move(rcValue);
        }
        
        if(childs & CHILD_CMDBTN)
        {
            if(pItem->HasButton())
            {
                CRect rcBtn = GetCmdButtonPos(pItem);
                rcBtn.left = rcBtn.right-rcBtn.Height();
                m_pCmdBtn->Move(&rcBtn);
                m_pCmdBtn->SetVisible(TRUE,TRUE);
            }else
			{
				m_pCmdBtn->SetVisible(FALSE,TRUE);
			}
        }
    }

    void SPropertyGrid::OnItemValueChanged( IPropertyItem *pItem )
    {
        EventPropGridValueChanged evt(this);
        evt.pItem = pItem;
        FireEvent(evt);
    }


	void SPropertyGrid::RemoveAllItems()
	{
		SPOSITION pos = m_lstGroup.GetHeadPosition();
		while(pos)
		{
			SPropertyGroup *pGroup = m_lstGroup.GetNext(pos);
			pGroup->Release();
		}
		m_lstGroup.RemoveAll();
		__super::DeleteAll();
		InvalidateRect(GetWindowRect());
	}

	IPropertyItem * SPropertyGrid::FindItemByName(const SStringW & strName) const
	{
		SPOSITION pos = m_lstGroup.GetHeadPosition();
		while(pos)
		{
			SPropertyGroup *pGroup = m_lstGroup.GetNext(pos);
			if(pGroup->GetName2() == strName)
				return pGroup;
			
			IPropertyItem *pChild = pGroup->FindChildByName(strName);
			if(pChild)
				return pChild;
		}
		return NULL;
	}

	IPropertyItem * SPropertyGrid::FindChildById(int nID) const
	{
		SPOSITION pos = m_lstGroup.GetHeadPosition();
		while(pos)
		{
			SPropertyGroup *pGroup = m_lstGroup.GetNext(pos);
			if(pGroup->GetID() == nID)
				return pGroup;

			IPropertyItem *pChild = pGroup->FindChildById(nID);
			if(pChild)
				return pChild;
		}
		return NULL;
	}

	void SPropertyGrid::OnItemButtonClick(IPropertyItem *pItem)
	{
		if(!pItem->OnButtonClick())
		{
			EventPropGridItemButtonClick evt(this);
			evt.pItem = pItem;
			FireEvent(evt);
		}
	}

	void SPropertyGrid::LoadFromXml(pugi::xml_node data)
	{
		if(data)
		{
			pugi::xml_node xmlChild = data.child(SPropertyGroup::GetClassName());
			while(xmlChild)
			{
				SPropertyGroup *pGroup = (SPropertyGroup *)CreateItem(SPropertyGroup::GetClassName());
				pGroup->InitFromXml(xmlChild);
				InsertGroup(pGroup);
				pGroup->Release();
				xmlChild = xmlChild.next_sibling(SPropertyGroup::GetClassName());
			}
			InvalidateRect(GetWindowRect());
		}
	}

	BOOL SPropertyGrid::IsItemVisible(IPropertyItem *pItem) const
	{
		for(int i=0;i<GetCount();i++)
		{
			if(GetItemData(i) == (LPARAM)pItem)
				return TRUE;
		}
		return FALSE;
	}

	BOOL SPropertyGrid::SetItemAttribute(IPropertyItem * pItem,const SStringW & attr,const SStringW & value)
	{
		HRESULT hRet = pItem->SetAttribute(attr,value,FALSE);
		if(hRet == S_OK)
		{//update view.
			OnItemInvalidate(pItem);
		}
		return SUCCEEDED(hRet);
	}

	COLORREF SPropertyGrid::GetGroupColor() const
	{
		return m_crGroup;
	}

	void SPropertyGrid::OnItemInvalidate(IPropertyItem *pItem)
	{
		for(int i=0;i<GetCount();i++)
		{
			IPropertyItem *p = (IPropertyItem *)GetItemData(i);
			if(p==pItem)
			{
				RedrawItem(i);
				break;
			}
		}
	}

	void SPropertyGrid::EnumProp(FunEnumProp funEnum,void* opaque)
	{
		SPOSITION pos = m_lstGroup.GetHeadPosition();
		while(pos)
		{
			SPropertyGroup *pGroup = m_lstGroup.GetNext(pos);
			if(!funEnum(pGroup,opaque))
				return;
			IPropertyItem *pItem = pGroup->GetItem(IPropertyItem::GPI_FIRSTCHILD);
			while(pItem)
			{
				if(!funEnum(pItem,opaque))
					return;	
				pItem = pItem->GetItem(IPropertyItem::GPI_NEXTSIBLING);
			}
		}
	}

	CRect SPropertyGrid::GetInplaceWndPos(IPropertyItem *pItem) const
	{
		CRect rcItem = GetItemRect(pItem);
		rcItem.left += rcItem.Height()+m_nTitleWidth;

		CRect rcValue = rcItem;
		rcValue.DeflateRect(0,0,1,1);
		if(pItem->HasButton())
		{
			rcValue.right -= rcValue.Height();
		}
		pItem->AdjustInplaceActiveWndRect(rcValue);
		return rcValue;
	}

	CRect SPropertyGrid::GetCmdButtonPos(IPropertyItem *pItem) const
	{
		if(pItem->HasButton())
		{
			CRect rcItem = GetItemRect(pItem);
			rcItem.left += rcItem.Height()+m_nTitleWidth;

			CRect rcValue = rcItem;
			rcValue.DeflateRect(0,0,1,1);
			rcValue.left = rcValue.right-rcValue.Height();
			return rcValue;

		}else
		{
			return CRect();
		}

	}

	SPropertyGrid::ORDERTYPE SPropertyGrid::GetOrderType() const
	{
		return m_orderType;
	}

	void SPropertyGrid::SetOrderType(SPropertyGrid::ORDERTYPE type)
	{
		if(type == m_orderType)
			return;
		int nCurSel = GetCurSel();
		IPropertyItem *pSelItem = NULL;
		BOOL bActive = FALSE;
		if(nCurSel != -1)
		{
			pSelItem = (IPropertyItem*)GetItemData(nCurSel);
			bActive = pSelItem->IsInplaceActive();
		}
		if(bActive)
		{
			pSelItem->OnInplaceActive(FALSE);
		}
		m_orderType = type;
		DeleteAll();
		SList<SPropertyGroup*> groups;
		m_lstGroup.Swap(groups);
		SPOSITION pos = groups.GetHeadPosition();
		while(pos)
		{
			SPropertyGroup *p = groups.GetNext(pos);
			InsertGroup(p);
			p->Release();
		}
		if(pSelItem)
		{
			int iItem = IndexOfPropertyItem(pSelItem);
			if(iItem!=-1)
			{
				EnsureVisible(iItem);
				SetCurSel(iItem);
				if(bActive)
				{
					pSelItem->OnInplaceActive(TRUE);
				}
			}
		}
	}

	LRESULT SPropertyGrid::OnAttrOrderType(const SStringW &strValue,BOOL bLoading)
	{
		ORDERTYPE orderType;
		if(strValue.CompareNoCase(L"name")==0)
			orderType = OT_NAME;
		else if(strValue.CompareNoCase(L"group")==0)
			orderType = OT_GROUP;
		else
			return E_INVALIDARG;
		SetOrderType(orderType);
		return bLoading?S_OK:S_FALSE;
	}

}

