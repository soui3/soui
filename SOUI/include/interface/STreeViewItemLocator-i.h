#pragma once

#include <interface/SAdapter-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE ITreeViewItemLocator
DECLARE_INTERFACE_(ITreeViewItemLocator,IObjRef)
{
	STDMETHOD_(void,SetAdapter)(THIS_ ITvAdapter *pAdapter) PURE;
	STDMETHOD_(void,OnBranchChanged)(THIS_ HTREEITEM hItem) PURE;
	STDMETHOD_(void,OnBranchExpandedChanged)(THIS_ HTREEITEM hItem,BOOL bExpandedOld,BOOL bExpandedNew) PURE;

	STDMETHOD_(int,GetTotalHeight)(THIS) SCONST PURE;
	STDMETHOD_(int,GetTotalWidth)(THIS) SCONST PURE;
	STDMETHOD_(int,Item2Position)(THIS_ HTREEITEM hItem) SCONST PURE;
	STDMETHOD_(HTREEITEM,Position2Item)(THIS_ int position) SCONST PURE;
	STDMETHOD_(int,GetScrollLineSize)(THIS) SCONST PURE;

	STDMETHOD_(void,SetItemWidth)(THIS_ HTREEITEM hItem,int nWidth) PURE;
	STDMETHOD_(int,GetItemWidth)(THIS_ HTREEITEM hItem) SCONST PURE;
	STDMETHOD_(void,SetItemHeight)(THIS_ HTREEITEM hItem,int nHeight) PURE;
	STDMETHOD_(int,GetItemHeight)(THIS_ HTREEITEM hItem) SCONST PURE;
	STDMETHOD_(int,GetItemIndent)(THIS_ HTREEITEM hItem) SCONST PURE;
};
    
SNSEND