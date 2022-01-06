#include "souistd.h"
#include "control/SListView.h"
#include "helper/SListViewItemLocator.h"

namespace SOUI
{


    class SListViewDataSetObserver : public TObjRefImpl<ILvDataSetObserver>
    {
    public:
        SListViewDataSetObserver(SListView *pView):m_pOwner(pView)
        {
        }
        virtual void onChanged();
        virtual void onInvalidated();

		virtual void OnItemChanged(int iItem);
    protected:
        SListView * m_pOwner;
    };

    //////////////////////////////////////////////////////////////////////////
    void SListViewDataSetObserver::onChanged()
    {
        m_pOwner->onDataSetChanged();
    }

    void SListViewDataSetObserver::onInvalidated()
    {
        m_pOwner->onDataSetInvalidated();
    }

	void SListViewDataSetObserver::OnItemChanged(int iItem)
	{
		m_pOwner->onItemDataChanged(iItem);
	}



    //////////////////////////////////////////////////////////////////////////
    SListView::SListView()
        :m_iSelItem(-1)
        ,m_iFirstVisible(-1)
        ,m_pHoverItem(NULL)
        ,m_itemCapture(NULL)
        ,m_pSkinDivider(NULL)
        ,m_bWantTab(FALSE)
        ,m_bDataSetInvalidated(FALSE)
		,m_bPendingUpdate(false)
		,m_iPendingUpdateItem(-2)
		,m_bVertical(TRUE)
    {
        m_bFocusable = TRUE;
        m_observer.Attach(new SListViewDataSetObserver(this));
        m_dwUpdateInterval= 40;
        m_evtSet.addEvent(EVENTID(EventLVSelChanging));
        m_evtSet.addEvent(EVENTID(EventLVSelChanged));
    }

    SListView::~SListView()
    {
        m_observer=NULL;
        m_lvItemLocator=NULL;
    }

    BOOL SListView::SetAdapter(ILvAdapter * adapter)
    {
        if(!m_lvItemLocator)
        {
            SASSERT_FMT(FALSE,_T("error: A item locator is in need before setting adapter!!!"));
            return FALSE;
        }
		if(m_adapter == adapter)
		{
			SLOG_WARN("the new adapter is same to previous set adapter, same as notifyDatasetChanged");
			if(m_adapter)
			{
				onDataSetChanged();
			}
			return TRUE;
		}

        if(m_adapter)
        {
            m_adapter->unregisterDataSetObserver(m_observer);

            //free all itemPanels in recycle
            for(size_t i=0;i<m_itemRecycle.GetCount();i++)
            {
                SList<SItemPanel*> *lstItemPanels = m_itemRecycle.GetAt(i);
                SPOSITION pos = lstItemPanels->GetHeadPosition();
                while(pos)
                {
                    SItemPanel * pItemPanel = lstItemPanels->GetNext(pos);
                    pItemPanel->Destroy();
                }
                delete lstItemPanels;
            }
            m_itemRecycle.RemoveAll();

            //free all visible itemPanels
            SPOSITION pos=m_lstItems.GetHeadPosition();
            while(pos)
            {
                ItemInfo ii = m_lstItems.GetNext(pos);
                ii.pItem->Destroy();
            }
            m_lstItems.RemoveAll();
        }

        m_adapter = adapter;
        if(m_lvItemLocator)
            m_lvItemLocator->SetAdapter(adapter);
        if(m_adapter) 
        {
            m_adapter->InitByTemplate(m_xmlTemplate.root().first_child());
            m_adapter->registerDataSetObserver(m_observer);
            for(int i=0;i<m_adapter->getViewTypeCount();i++)
            {
                m_itemRecycle.Add(new SList<SItemPanel*>());
            }
            onDataSetChanged();
        }
        return TRUE;
    }

    void SListView::UpdateScrollBar()
    {
        CRect rcClient=SWindow::GetClientRect();
        CSize size = rcClient.Size();
        CSize szView;
		if(m_bVertical)
		{
			szView.cx = rcClient.Width();
			szView.cy = m_lvItemLocator?m_lvItemLocator->GetTotalHeight():0;

			//  关闭滚动条
			m_wBarVisible = SSB_NULL;

			if (size.cy<szView.cy )
			{
				//  需要纵向滚动条
				m_wBarVisible |= SSB_VERT;
				m_siVer.nMin  = 0;
				m_siVer.nMax  = szView.cy-1;
				m_siVer.nPage = size.cy;
				m_siVer.nPos = smin(m_siVer.nPos,m_siVer.nMax-(int)m_siVer.nPage);
			}
			else
			{
				//  不需要纵向滚动条
				m_siVer.nPage = size.cy;
				m_siVer.nMin  = 0;
				m_siVer.nMax  = size.cy-1;
				m_siVer.nPos  = 0;
			}

			SetScrollPos(TRUE, m_siVer.nPos, FALSE);
		}else
		{
			szView.cy = rcClient.Height();
			szView.cx = m_lvItemLocator?m_lvItemLocator->GetTotalHeight():0;

			//  关闭滚动条
			m_wBarVisible = SSB_NULL;

			if (size.cx<szView.cx )
			{
				//  需要纵向滚动条
				m_wBarVisible |= SSB_HORZ;
				m_siHoz.nMin  = 0;
				m_siHoz.nMax  = szView.cx-1;
				m_siHoz.nPage = size.cx;
				m_siHoz.nPos = smin(m_siHoz.nPos,m_siHoz.nMax-(int)m_siHoz.nPage);
			}
			else
			{
				//  不需要纵向滚动条
				m_siHoz.nPage = size.cx;
				m_siHoz.nMin  = 0;
				m_siHoz.nMax  = size.cx-1;
				m_siHoz.nPos  = 0;
			}

			SetScrollPos(FALSE, m_siHoz.nPos, FALSE);

		}

        //  重新计算客户区及非客户区
        SSendMessage(WM_NCCALCSIZE);

        InvalidateRect(NULL);
    }

    void SListView::onDataSetChanged()
    {
        if(!m_adapter) return;
		if(!IsVisible(TRUE))
		{
			m_bPendingUpdate = true;
			m_iPendingUpdateItem = -1;
			return;
		}
        if(m_lvItemLocator) m_lvItemLocator->OnDataSetChanged();
        if(m_iSelItem != -1 && m_iSelItem >= m_adapter->getCount())
            m_iSelItem = -1;
        UpdateScrollBar();
        UpdateVisibleItems();
    }

    void SListView::onDataSetInvalidated()
    {
        m_bDataSetInvalidated = TRUE;
        Invalidate();
    }


	void SListView::onItemDataChanged(int iItem)
	{
		if(!m_adapter) return;
		if(!IsVisible(TRUE))
		{
			m_bPendingUpdate = true;
			m_iPendingUpdateItem = m_iPendingUpdateItem==-2?iItem:-1;
			return;
		}

		if(iItem<m_iFirstVisible) return;
		if(iItem>=m_iFirstVisible + (int)m_lstItems.GetCount()) return;
		if(m_lvItemLocator->IsFixHeight())
			UpdateVisibleItem(iItem);
		else
			UpdateVisibleItems();
	}


    void SListView::OnPaint(IRenderTarget *pRT)
    {
        if(m_bDataSetInvalidated)
        {
            UpdateVisibleItems();
            m_bDataSetInvalidated = FALSE;
        }
        
        SPainter duiDC;
        BeforePaint(pRT,duiDC);


        int iFirst = m_iFirstVisible;
        if(iFirst!=-1)
        {
            CRect rcClient;
            GetClientRect(&rcClient);
            pRT->PushClipRect(&rcClient,RGN_AND);

            CRect rcClip;
			SAutoRefPtr<IRegion> rgnClip;
			pRT->GetClipRegion(&rgnClip);
            pRT->GetClipBox(&rcClip);

			float fMat[9];
			pRT->GetTransform(fMat);
			SMatrix mtx(fMat);

			SCROLLINFO &si = m_bVertical?m_siVer:m_siHoz;
            int nOffset = m_lvItemLocator->Item2Position(iFirst)-si.nPos;

            CRect rcItem(rcClient);
			if(m_bVertical)
				rcItem.bottom = rcItem.top + nOffset;
			else
				rcItem.right = rcItem.left + nOffset;

            SPOSITION pos= m_lstItems.GetHeadPosition();
            int i=0;
            for(;pos;i++)
            {
                ItemInfo ii = m_lstItems.GetNext(pos);
				if(m_bVertical)
				{
					rcItem.top=rcItem.bottom;
					rcItem.bottom = rcItem.top + m_lvItemLocator->GetItemHeight(iFirst+i);
				}else
				{
					rcItem.left = rcItem.right;
					rcItem.right = rcItem.left + m_lvItemLocator->GetItemHeight(iFirst+i);
				}
				if(SItemPanel::IsItemInClip(mtx,rcClip,rgnClip,rcItem))
				{
					ii.pItem->Draw(pRT, rcItem);
				}

				if(m_bVertical)
				{
					rcItem.top = rcItem.bottom;
					rcItem.bottom += m_lvItemLocator->GetDividerSize();
				}else
				{
					rcItem.left = rcItem.right;
					rcItem.right += m_lvItemLocator->GetDividerSize();
				}
                if(m_pSkinDivider && !rcItem.IsRectEmpty() && rgnClip->RectInRegion(&rcItem))
                {//绘制分隔线
                    m_pSkinDivider->DrawByIndex(pRT,rcItem,0);
                }
            }

            pRT->PopClip();
        }
        AfterPaint(pRT,duiDC);
    }

    BOOL SListView::OnScroll(BOOL bVertical,UINT uCode,int nPos)
    {
		SCROLLINFO & si = bVertical?m_siVer:m_siHoz;
		int nOldPos = si.nPos;
        SPanel::OnScroll(bVertical, uCode, nPos);
        int nNewPos = si.nPos;
        if(nOldPos != nNewPos)
        {
            UpdateVisibleItems();

            //加速滚动时UI的刷新
            if (uCode==SB_THUMBTRACK)
                ScrollUpdate();

            return TRUE;
        }
        return FALSE;
    }


    void SListView::UpdateVisibleItems()
    {
        if(!m_adapter) return;
        int iOldFirstVisible = m_iFirstVisible;
        int iOldLastVisible = m_iFirstVisible + (int)m_lstItems.GetCount();
        int nOldTotalHeight = m_lvItemLocator->GetTotalHeight();

		SCROLLINFO & si = m_bVertical?m_siVer:m_siHoz;

        int iNewFirstVisible = m_lvItemLocator->Position2Item(si.nPos);
        int iNewLastVisible = iNewFirstVisible;
        int pos = m_lvItemLocator->Item2Position(iNewFirstVisible);
        int iHoverItem = m_pHoverItem?(int)m_pHoverItem->GetItemIndex():-1;
        m_pHoverItem = NULL;

        ItemInfo *pItemInfos = new ItemInfo[m_lstItems.GetCount()];
        SPOSITION spos = m_lstItems.GetHeadPosition();
        int i=0;
        while(spos)
        {
            pItemInfos[i++]=m_lstItems.GetNext(spos);
        }

        m_lstItems.RemoveAll();

        if(iNewFirstVisible!=-1)
        {
            while(pos < si.nPos + (int)si.nPage && iNewLastVisible < m_adapter->getCount())
            {
                DWORD dwState = WndState_Normal;
                if(iHoverItem == iNewLastVisible) dwState |= WndState_Hover;
                if(m_iSelItem == iNewLastVisible) dwState |= WndState_Check;

                ItemInfo ii={NULL,-1};
                ii.nType = m_adapter->getItemViewType(iNewLastVisible,dwState);
                if(iNewLastVisible>=iOldFirstVisible && iNewLastVisible < iOldLastVisible)
                {//use the old visible item
                    int iItem = iNewLastVisible-iOldFirstVisible;//(iNewLastVisible-iNewFirstVisible) + (iNewFirstVisible-iOldFirstVisible);
                    SASSERT(iItem>=0 && iItem <= (iOldLastVisible-iOldFirstVisible));
                    if(ii.nType == pItemInfos[iItem].nType)
                    {
                        ii = pItemInfos[iItem];
                        pItemInfos[iItem].pItem = NULL;//标记该行已经被重用
                    }
                }
				BOOL bNewItem=FALSE;
                if(!ii.pItem)
                {//create new visible item
                    SList<SItemPanel *> *lstRecycle = m_itemRecycle.GetAt(ii.nType);
                    if(lstRecycle->IsEmpty())
                    {//创建一个新的列表项
						bNewItem = TRUE;
                        ii.pItem = SItemPanel::Create(this,SXmlNode(),this);
                        ii.pItem->GetEventSet()->subscribeEvent(EventItemPanelClick::EventID,Subscriber(&SListView::OnItemClick,this));
                    }else
                    {
                        ii.pItem = lstRecycle->RemoveHead();
                    }
                    ii.pItem->SetItemIndex(iNewLastVisible);
                }
                ii.pItem->SetVisible(TRUE);
                CRect rcItem = GetClientRect();
                rcItem.MoveToXY(0,0);
                if(m_lvItemLocator->IsFixHeight())
                {
                    if(m_bVertical)
						rcItem.bottom=m_lvItemLocator->GetItemHeight(iNewLastVisible);
					else
						rcItem.right=m_lvItemLocator->GetItemHeight(iNewLastVisible);
                    ii.pItem->Move(rcItem);
                }

                //设置状态，同时暂时禁止应用响应statechanged事件。
                ii.pItem->GetEventSet()->setMutedState(true);
                ii.pItem->ModifyItemState(dwState,0);
                ii.pItem->GetEventSet()->setMutedState(false);
                if(dwState & WndState_Hover)
                    m_pHoverItem = ii.pItem;

                m_adapter->getView(iNewLastVisible,ii.pItem,m_xmlTemplate.root().first_child());
				if(bNewItem)
				{
					ii.pItem->SDispatchMessage(UM_SETSCALE, GetScale(), 0);
					ii.pItem->SDispatchMessage(UM_SETLANGUAGE,0,0);
					ii.pItem->DoColorize(GetColorizeColor());
				}
                if(!m_lvItemLocator->IsFixHeight())
                {
					if(m_bVertical)
					{
						rcItem.bottom=0;
						CSize szItem = m_adapter->getViewDesiredSize(iNewLastVisible,ii.pItem,rcItem.Width(),rcItem.Height());
						rcItem.bottom = rcItem.top + szItem.cy;
						ii.pItem->Move(rcItem);
						m_lvItemLocator->SetItemHeight(iNewLastVisible,szItem.cy);
					}else
					{
						rcItem.right=0;
						CSize szItem = m_adapter->getViewDesiredSize(iNewLastVisible,ii.pItem,rcItem.Width(),rcItem.Height());
						rcItem.right = rcItem.left + szItem.cx;
						ii.pItem->Move(rcItem);
						m_lvItemLocator->SetItemHeight(iNewLastVisible,szItem.cx);
					}
                }                
                ii.pItem->UpdateLayout();
                    
                m_lstItems.AddTail(ii);
				pos += (m_bVertical?rcItem.bottom:rcItem.right) + m_lvItemLocator->GetDividerSize();

                iNewLastVisible ++;
            }
        }

        //move old visible items which were not reused to recycle
        for(int i=0;i<(iOldLastVisible-iOldFirstVisible);i++)
        {
            ItemInfo ii = pItemInfos[i];
            if(!ii.pItem) continue;
            if(ii.pItem->GetState() & WndState_Hover)
            {
                ii.pItem->DoFrameEvent(WM_MOUSELEAVE,0,0);
            }
            ii.pItem->GetEventSet()->setMutedState(true);
            if(ii.pItem->GetState() & WndState_Check)
            {
                ii.pItem->ModifyItemState(0,WndState_Check);
                ii.pItem->GetFocusManager()->ClearFocus();
            }
            ii.pItem->SetVisible(FALSE);
            ii.pItem->GetEventSet()->setMutedState(false);
            m_itemRecycle[ii.nType]->AddTail(ii.pItem);    
        }
        delete [] pItemInfos;

        m_iFirstVisible = iNewFirstVisible;

        if(!m_lvItemLocator->IsFixHeight() && m_lvItemLocator->GetTotalHeight() != nOldTotalHeight)
        {//update scroll range
            UpdateScrollBar();
            UpdateVisibleItems();//根据新的滚动条状态重新记录显示列表项
        }
    }


	void SListView::UpdateVisibleItem(int iItem)
	{
		SASSERT(m_lvItemLocator->IsFixHeight());
		SItemPanel * pItem = GetItemPanel(iItem);
		SASSERT(pItem);
		m_adapter->getView(iItem,pItem,m_xmlTemplate.root().first_child());
	}


    void SListView::OnSize(UINT nType, CSize size)
    {
        __super::OnSize(nType,size);
        UpdateScrollBar();

        //update item window
        CRect rcClient=GetClientRect();
        SPOSITION pos = m_lstItems.GetHeadPosition();
        while(pos)
        {
            ItemInfo ii = m_lstItems.GetNext(pos);
            int idx = (int)ii.pItem->GetItemIndex();
            int nHei = m_lvItemLocator->GetItemHeight(idx);
			CRect rcItem;
			if(m_bVertical)
				rcItem.right=rcClient.Width(),rcItem.bottom = nHei;
			else
				rcItem.right = nHei, rcItem.bottom = rcClient.Height();
            ii.pItem->Move(rcItem);
        }

        UpdateVisibleItems();
    }

    void SListView::OnDestroy()
    {
		if(m_adapter)
		{
			m_adapter->unregisterDataSetObserver(m_observer);
		}

        //destroy all itempanel
        SPOSITION pos = m_lstItems.GetHeadPosition();
        while(pos)
        {
            ItemInfo ii = m_lstItems.GetNext(pos);
            ii.pItem->Release();
        }
        m_lstItems.RemoveAll();

        for(int i=0;i<(int)m_itemRecycle.GetCount();i++)
        {
            SList<SItemPanel*> *pLstTypeItems = m_itemRecycle[i];
            SPOSITION pos = pLstTypeItems->GetHeadPosition();
            while(pos)
            {
                SItemPanel *pItem = pLstTypeItems->GetNext(pos);
                pItem->Release();
            }
            delete pLstTypeItems;
        }
        m_itemRecycle.RemoveAll();
        __super::OnDestroy();
    }


    //////////////////////////////////////////////////////////////////////////
    void SListView::OnItemRequestRelayout(SItemPanel *pItem)
    {//do nothing
    }

    BOOL SListView::IsItemRedrawDelay() const
    {
        return TRUE;
    }

    BOOL SListView::OnItemGetRect(const SItemPanel *pItem,CRect &rcItem) const
    {
        int iPosition = (int)pItem->GetItemIndex();
		if(iPosition<0 || iPosition >= m_adapter->getCount())
			return FALSE;
		if(m_bVertical)
		{
			int nOffset = m_lvItemLocator->Item2Position(iPosition)-m_siVer.nPos;
			rcItem = GetClientRect();
			rcItem.top += nOffset;
			rcItem.bottom = rcItem.top + m_lvItemLocator->GetItemHeight(iPosition);
		}else
		{
			int nOffset = m_lvItemLocator->Item2Position(iPosition)-m_siHoz.nPos;
			rcItem = GetClientRect();
			rcItem.left += nOffset;
			rcItem.right = rcItem.left + m_lvItemLocator->GetItemHeight(iPosition);
		}
        return TRUE;
    }

    void SListView::OnItemSetCapture(SItemPanel *pItem,BOOL bCapture)
    {
        if(bCapture)
        {
            GetContainer()->OnSetSwndCapture(m_swnd);
            m_itemCapture=pItem;
        }else
        {
            GetContainer()->OnReleaseSwndCapture();
            m_itemCapture=NULL;
        }
    }

    void SListView::RedrawItem(SItemPanel *pItem)
    {
        pItem->InvalidateRect(NULL);
    }

    SItemPanel * SListView::HitTest(CPoint & pt)
    {
        SPOSITION pos = m_lstItems.GetHeadPosition();
        while(pos)
        {
            ItemInfo ii = m_lstItems.GetNext(pos);
            CRect rcItem = ii.pItem->GetItemRect();
            if(rcItem.PtInRect(pt)) 
            {
                pt-=rcItem.TopLeft();
                return ii.pItem;
            }
        }
        return NULL;
    }

    LRESULT SListView::OnMouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam)
    {
        SetMsgHandled(FALSE);
        if(!m_adapter)
        {
            return 0;
        }

        LRESULT lRet=0;
        CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));


        if(m_itemCapture)
        {
            CRect rcItem=m_itemCapture->GetItemRect();
            pt.Offset(-rcItem.TopLeft());
            lRet = m_itemCapture->DoFrameEvent(uMsg,wParam,MAKELPARAM(pt.x,pt.y));
        }
        else
        {
            if(uMsg==WM_LBUTTONDOWN || uMsg== WM_RBUTTONDOWN || uMsg==WM_MBUTTONDOWN)
            {//交给panel处理
                __super::ProcessSwndMessage(uMsg,wParam,lParam,lRet);
            }

            SItemPanel * pHover=HitTest(pt);
            if(pHover!=m_pHoverItem)
            {
                SItemPanel * nOldHover=m_pHoverItem;
                m_pHoverItem=pHover;
                if(nOldHover)
                {
                    nOldHover->DoFrameEvent(WM_MOUSELEAVE,0,0);
                    RedrawItem(nOldHover);
                }
                if(m_pHoverItem)
                {
                    m_pHoverItem->DoFrameEvent(WM_MOUSEHOVER,wParam,MAKELPARAM(pt.x,pt.y));
                    RedrawItem(m_pHoverItem);
                }
            }
            if(m_pHoverItem)
            {
                m_pHoverItem->DoFrameEvent(uMsg,wParam,MAKELPARAM(pt.x,pt.y));
            }
        }
        
        if(uMsg==WM_LBUTTONUP || uMsg== WM_RBUTTONUP || uMsg==WM_MBUTTONUP)
        {//交给panel处理
            __super::ProcessSwndMessage(uMsg,wParam,lParam,lRet);
        }
        SetMsgHandled(TRUE);
        return 0;
    }

    LRESULT SListView::OnKeyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam)
    {
        LRESULT lRet=0;
        SItemPanel *pItem = GetItemPanel(m_iSelItem);
        if(pItem)
        {
            lRet=pItem->DoFrameEvent(uMsg,wParam,lParam);
            SetMsgHandled(pItem->IsMsgHandled());
        }else
        {
            SetMsgHandled(FALSE);
        }
        return lRet;
    }

    void SListView::OnMouseLeave()
    {
		__super::OnMouseLeave();
        if(m_pHoverItem)
        {
            m_pHoverItem->DoFrameEvent(WM_MOUSELEAVE,0,0);
            m_pHoverItem = NULL;
        }

    }

    void SListView::OnKeyDown( TCHAR nChar, UINT nRepCnt, UINT nFlags )
    {
        if(!m_adapter)
        {
            SetMsgHandled(FALSE);
            return;
        }
        
        if(m_iSelItem!=-1 && m_bWantTab)
        {
            SItemPanel *pItem = GetItemPanel(m_iSelItem);
            if(pItem)
            {
                pItem->DoFrameEvent(WM_KEYDOWN,nChar, MAKELONG(nFlags, nRepCnt));
                if(pItem->IsMsgHandled()) return;
            }
        }
        
        int  nNewSelItem = -1;
        SWindow *pOwner = GetOwner();
        if (pOwner && (nChar == VK_ESCAPE || nChar == VK_RETURN))
        {
            pOwner->SSendMessage(WM_KEYDOWN, nChar, MAKELONG(nFlags, nRepCnt));
            return;
        }

        if (nChar == VK_DOWN && m_iSelItem < m_adapter->getCount() - 1)
            nNewSelItem = m_iSelItem+1;
        else if (nChar == VK_UP && m_iSelItem > 0)
            nNewSelItem = m_iSelItem-1;
        else
        {
            switch(nChar)
            {
            case VK_PRIOR:
                OnScroll(TRUE,SB_PAGEUP,0);
                break;
            case VK_NEXT:
                OnScroll(TRUE,SB_PAGEDOWN,0);
                break;
            case VK_HOME:
                OnScroll(TRUE,SB_TOP,0);
                break;
            case VK_END:
                OnScroll(TRUE,SB_BOTTOM,0);
                break;
            }
            if(nChar == VK_PRIOR || nChar == VK_HOME)
            {
                if(!m_lstItems.IsEmpty())
                {
                    nNewSelItem = (int)m_lstItems.GetHead().pItem->GetItemIndex();
                }
            }else if(nChar == VK_NEXT || nChar == VK_END)
            {
                if(!m_lstItems.IsEmpty())
                {
                    nNewSelItem = (int)m_lstItems.GetTail().pItem->GetItemIndex();
                }
            }
        }

        if(nNewSelItem!=-1)
        {
            EnsureVisible(nNewSelItem);
            SetSel(nNewSelItem);
        }
    }

    void SListView::EnsureVisible( int iItem )
    {
        if(iItem<0 || iItem>=m_adapter->getCount()) return;

        int iFirstVisible= m_iFirstVisible;
        int iLastVisible = m_iFirstVisible + (int)m_lstItems.GetCount();

        if(iItem>=iFirstVisible && iItem<iLastVisible)
        {
            if(iItem == iFirstVisible)
            {
                int pos = m_lvItemLocator->Item2Position(iItem);
                OnScroll(m_bVertical,SB_THUMBPOSITION,pos);
            }else if(iItem == iLastVisible-1)
            {
                if(iItem == m_adapter->getCount()-1)
                    OnScroll(m_bVertical,SB_BOTTOM,0);
                else
                {
					int pos = m_lvItemLocator->Item2Position(iItem+1) - m_bVertical?m_siVer.nPage:m_siHoz.nPage;
                    OnScroll(m_bVertical,SB_THUMBPOSITION,pos);
                }
            }

            return;
        }


        if(iItem < iFirstVisible)
        {//scroll up
            int pos = m_lvItemLocator->Item2Position(iItem);
            OnScroll(m_bVertical,SB_THUMBPOSITION,pos);
        }else // if(iItem >= iLastVisible)
        {//scroll down
            if(iItem == m_adapter->getCount()-1)
            {
                OnScroll(m_bVertical,SB_BOTTOM,0);
            }else
            {
				int pos = m_lvItemLocator->Item2Position(iItem+1)-m_bVertical?m_siVer.nPage:m_siHoz.nPage;
                OnScroll(m_bVertical,SB_THUMBPOSITION,pos);
            }
        }
    }

    BOOL SListView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
    {
        SItemPanel *pSelItem = GetItemPanel(m_iSelItem);
        if(pSelItem)
        {
            CRect rcItem = pSelItem->GetItemRect();
            CPoint pt2=pt-rcItem.TopLeft();
            if(pSelItem->DoFrameEvent(WM_MOUSEWHEEL,MAKEWPARAM(nFlags,zDelta),MAKELPARAM(pt2.x,pt2.y)))
                return TRUE;
        }
        return __super::OnMouseWheel(nFlags, zDelta, pt);
    }

    int SListView::GetScrollLineSize(BOOL bVertical)
    {
        return m_lvItemLocator->GetScrollLineSize();
    }

    SItemPanel * SListView::GetItemPanel(int iItem)
    {
        if(iItem<0 || iItem>=m_adapter->getCount()) 
            return NULL; 
        SPOSITION pos = m_lstItems.GetHeadPosition();
        while(pos)
        {
            ItemInfo ii = m_lstItems.GetNext(pos);
            if((int)ii.pItem->GetItemIndex() == iItem)
                return ii.pItem;
        }
        return NULL;
    }


    BOOL SListView::CreateChildren(SXmlNode xmlNode)
    {
        SXmlNode xmlTemplate = xmlNode.child(L"template");
        if(xmlTemplate)
        {
            m_xmlTemplate.root().append_copy(xmlTemplate);
			LPCWSTR kItemSize = m_bVertical?L"itemHeight":L"itemWidth";
			SLayoutSize nItemHei = GETLAYOUTSIZE(xmlTemplate.attribute(kItemSize).value());
            if(nItemHei.fSize>0.0f)
            {//指定了itemHeight属性时创建一个固定行高的定位器
                IListViewItemLocator * pItemLocator = new  SListViewItemLocatorFix(nItemHei,m_nDividerSize);
                SetItemLocator(pItemLocator);
                pItemLocator->Release();
            }else
            {//创建一个行高可变的行定位器，从defHeight/defWidth属性中获取默认行高
				LPCWSTR kDefSize = m_bVertical?L"defHeight":L"defWidth";
				IListViewItemLocator * pItemLocator = new  SListViewItemLocatorFlex(GETLAYOUTSIZE(xmlTemplate.attribute(kDefSize).as_string(L"30dp")),m_nDividerSize);
                SetItemLocator(pItemLocator);
                pItemLocator->Release();
            }
        }
        return TRUE;
    }

    void SListView::SetItemLocator(IListViewItemLocator *pItemLocator)
    {
        m_lvItemLocator = pItemLocator;
        if(m_lvItemLocator) 
		{
			m_lvItemLocator->SetAdapter(GetAdapter());
			m_lvItemLocator->SetScale(GetScale());
		}
        onDataSetChanged();
    }

    BOOL SListView::UpdateToolTip(CPoint pt, SwndToolTipInfo & tipInfo)
    {
        if(!m_pHoverItem)
            return __super::UpdateToolTip(pt,tipInfo);
        return m_pHoverItem->UpdateToolTip(pt,tipInfo);
    }

    void SListView::SetSel(int iItem,BOOL bNotify)
    {
        if(!m_adapter) return;

        if(iItem>=m_adapter->getCount())
            return;

        if(iItem<0) iItem = -1;

        int nOldSel = m_iSelItem;
        int nNewSel = iItem;

        m_iSelItem = nNewSel;
        if(bNotify)
        {
            EventLVSelChanging evt(this);
			evt.bCancel = FALSE;
            evt.iOldSel = nOldSel;
            evt.iNewSel = nNewSel;
            FireEvent(evt);
            if(evt.bCancel) 
            {//Cancel SetSel and restore selection state
                m_iSelItem = nOldSel;
                return;
            }
        }

        if(nOldSel == nNewSel)
            return;

        m_iSelItem = nOldSel;
        SItemPanel *pItem = GetItemPanel(nOldSel);
        if(pItem)
        {
            pItem->GetFocusManager()->ClearFocus();
            pItem->ModifyItemState(0,WndState_Check);
            RedrawItem(pItem);
        }
        m_iSelItem = nNewSel;
        pItem = GetItemPanel(nNewSel);
        if(pItem)
        {
            pItem->ModifyItemState(WndState_Check,0);
            RedrawItem(pItem);
        }
        
        if(bNotify)
        {
            EventLVSelChanged evt(this);
            evt.iOldSel = nOldSel;
            evt.iNewSel = nNewSel;
            FireEvent(evt);
        }

    }

    UINT SListView::OnGetDlgCode() const
    {
        if(m_bWantTab) return SC_WANTALLKEYS;
        else return SC_WANTARROWS|SC_WANTSYSKEY;
    }

    void SListView::OnKillFocus(SWND wndFocus)
    {
        __super::OnKillFocus(wndFocus);
        
        if(m_iSelItem==-1) return;
        
        SItemPanel *pSelPanel = GetItemPanel(m_iSelItem);
        if(pSelPanel) pSelPanel->GetFocusManager()->StoreFocusedView();
    }

    void SListView::OnSetFocus(SWND wndOld)
    {
        __super::OnSetFocus(wndOld);
        if(m_iSelItem==-1) return;

        SItemPanel *pSelPanel = GetItemPanel(m_iSelItem);
        if(pSelPanel) pSelPanel->GetFocusManager()->RestoreFocusedView();
    }

    BOOL SListView::OnSetCursor(const CPoint &pt)
    {
        BOOL bRet=FALSE;
        if(m_itemCapture)
        {
            CRect rcItem=m_itemCapture->GetItemRect();
            bRet=m_itemCapture->DoFrameEvent(WM_SETCURSOR,0,MAKELPARAM(pt.x-rcItem.left,pt.y-rcItem.top))!=0;
        }
        else if(m_pHoverItem)
        {
            CRect rcItem=m_pHoverItem->GetItemRect();
            bRet=m_pHoverItem->DoFrameEvent(WM_SETCURSOR,0,MAKELPARAM(pt.x-rcItem.left,pt.y-rcItem.top))!=0;
        }
        if(!bRet)
        {
            bRet=__super::OnSetCursor(pt);
        }
        return bRet;

    }

    BOOL SListView::OnItemClick(IEvtArgs *pEvt)
    {
        SItemPanel *pItemPanel = sobj_cast<SItemPanel>(pEvt->Sender());
        SASSERT(pItemPanel);
        int iItem = (int)pItemPanel->GetItemIndex();
        if(iItem != m_iSelItem)
        {
            SetSel(iItem,TRUE);
        }
        return TRUE;

    }

	void SListView::OnColorize( COLORREF cr )
	{
		__super::OnColorize(cr);
		DispatchMessage2Items(UM_SETCOLORIZE,cr,0);
	}

	void SListView::OnScaleChanged(int nScale)
	{
		__super::OnScaleChanged(nScale);
		if(m_lvItemLocator) m_lvItemLocator->SetScale(nScale);
		DispatchMessage2Items(UM_SETSCALE,nScale,0);
	}

	HRESULT SListView::OnLanguageChanged()
	{
		HRESULT hret =__super::OnLanguageChanged();
		DispatchMessage2Items(UM_SETLANGUAGE,0,0);
		return hret;
	}

	void SListView::DispatchMessage2Items(UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		SPOSITION pos = m_lstItems.GetHeadPosition();
		while (pos)
		{
			ItemInfo ii = m_lstItems.GetNext(pos);
			ii.pItem->SDispatchMessage(uMsg, wParam, lParam);
		}
		for(UINT i=0;i<m_itemRecycle.GetCount();i++)
		{
			SList<SItemPanel*> *pLstTypeItems = m_itemRecycle[i];
			SPOSITION pos = pLstTypeItems->GetHeadPosition();
			while(pos)
			{
				SItemPanel *pItem = pLstTypeItems->GetNext(pos);
				pItem->SDispatchMessage(uMsg, wParam, lParam);
			}
		}
	}

	void SListView::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		__super::OnShowWindow(bShow,nStatus);
		if(IsVisible(TRUE) && m_bPendingUpdate)
		{
			if(m_iPendingUpdateItem == -1)
				onDataSetChanged();
			else
				onItemDataChanged(m_iPendingUpdateItem);
			m_bPendingUpdate = false;
			m_iPendingUpdateItem = -2;
		}
	}

	void SListView::OnRebuildFont()
	{
		__super::OnRebuildFont();
		DispatchMessage2Items(UM_UPDATEFONT,0,0);
	}

}