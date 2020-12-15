#pragma once
#include "interface/SListViewItemLocator-i.h"
#include "control/STree.h"

namespace SOUI
{
    class SOUI_EXP SListViewItemLocatorFix : public TObjRefImpl<IListViewItemLocator>
    {
    public:
        SListViewItemLocatorFix(SLayoutSize nItemHei,SLayoutSize nDividerSize=SLayoutSize());

	public:
		STDMETHOD_(void,SetAdapter)(THIS_ ILvAdapter *pAdapter) OVERRIDE;
		STDMETHOD_(void,OnDataSetChanged)(THIS) OVERRIDE {}
		STDMETHOD_(BOOL,IsFixHeight)(THIS) SCONST OVERRIDE;
		STDMETHOD_(int,GetItemHeight)(THIS_ int iItem) SCONST OVERRIDE;
		STDMETHOD_(void,SetItemHeight)(THIS_ int iItem,int nHeight) OVERRIDE;
		STDMETHOD_(int,GetTotalHeight)(THIS) OVERRIDE;
		STDMETHOD_(int,Item2Position)(THIS_ int iItem) OVERRIDE;
		STDMETHOD_(int,Position2Item)(THIS_ int position) OVERRIDE;
		STDMETHOD_(int,GetScrollLineSize)(THIS) SCONST OVERRIDE;
		STDMETHOD_(int,GetDividerSize)(THIS) SCONST OVERRIDE;
		STDMETHOD_(void,SetScale)(THIS_ int nScale) OVERRIDE;
    protected:
		int GetFixItemHeight() const;

        SLayoutSize m_nItemHeight;
        SLayoutSize m_nDividerSize;
		int	m_nScale;
        SAutoRefPtr<ILvAdapter> m_adapter;
    };

    class SOUI_EXP SListViewItemLocatorFlex : public TObjRefImpl<IListViewItemLocator>
    {

    public:

        SListViewItemLocatorFlex(SLayoutSize nItemHei,SLayoutSize nDividerSize=SLayoutSize());
        ~SListViewItemLocatorFlex();


		STDMETHOD_(void,SetAdapter)(THIS_ ILvAdapter *pAdapter) OVERRIDE;
		STDMETHOD_(void,OnDataSetChanged)(THIS) OVERRIDE;
		STDMETHOD_(BOOL,IsFixHeight)(THIS) SCONST OVERRIDE;
		STDMETHOD_(int,GetItemHeight)(THIS_ int iItem) SCONST OVERRIDE;
		STDMETHOD_(void,SetItemHeight)(THIS_ int iItem,int nHeight) OVERRIDE;
		STDMETHOD_(int,GetTotalHeight)(THIS) OVERRIDE;
		STDMETHOD_(int,Item2Position)(THIS_ int iItem) OVERRIDE;
		STDMETHOD_(int,Position2Item)(THIS_ int position) OVERRIDE;
		STDMETHOD_(int,GetScrollLineSize)(THIS) SCONST OVERRIDE;
		STDMETHOD_(int,GetDividerSize)(THIS) SCONST OVERRIDE;
		STDMETHOD_(void,SetScale)(THIS_ int nScale) OVERRIDE;

    protected:
        void InitIndex(HSTREEITEM hParent,int nItems,int nSubBranchSize);
		int GetFixItemHeight() const;
        int GetIndexDeep() const;
        void Clear();
        int Branch2Offset(HSTREEITEM hBranch) const;
        int Branch2Index(HSTREEITEM hBranch) const;
        HSTREEITEM Offset2Branch(HSTREEITEM hParent,int nOffset);

        SLayoutSize m_nItemHeight;  //默认表项高度
        SLayoutSize m_nDividerSize;
		int m_nScale;
        struct BranchInfo
        {
            int nBranchHei; //分枝高度
            int nBranchSize;//分枝中包含的节点数量
        };

        CSTree<BranchInfo>    m_itemPosIndex;//记录分枝高度
        class SegmentInfo
        {
        public:
            SegmentInfo(int nItems,HSTREEITEM hBranch):hItem(hBranch){
                this->nItems = nItems;
                pItemHeight = new int[nItems];
                memset(pItemHeight,0xff,nItems*sizeof(int));
            }
            ~SegmentInfo()
            {
                if(pItemHeight) delete[] pItemHeight;
            }

            HSTREEITEM hItem;
            int        nItems;
            int*       pItemHeight;//段中每一个表项的高度
        };

        SArray<SegmentInfo*>     m_segments;
        SAutoRefPtr<ILvAdapter>   m_adapter;
    };

}