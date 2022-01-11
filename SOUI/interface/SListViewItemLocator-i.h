#pragma once

#include <interface/SAdapter-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE IListViewItemLocator
DECLARE_INTERFACE_(IListViewItemLocator,IObjRef)
{
	STDMETHOD_(void,SetAdapter)(THIS_ ILvAdapter *pAdapter) PURE;
	STDMETHOD_(void,OnDataSetChanged)(THIS) PURE;
	STDMETHOD_(BOOL,IsFixHeight)(THIS) SCONST PURE;
	STDMETHOD_(int,GetItemHeight)(THIS_ int iItem) SCONST PURE;
	STDMETHOD_(void,SetItemHeight)(THIS_ int iItem,int nHeight) PURE;
	STDMETHOD_(int,GetTotalHeight)(THIS) PURE;
	STDMETHOD_(int,Item2Position)(THIS_ int iItem) PURE;
	STDMETHOD_(int,Position2Item)(THIS_ int position) PURE;
	STDMETHOD_(int,GetScrollLineSize)(THIS) SCONST PURE;
	STDMETHOD_(int,GetDividerSize)(THIS) SCONST PURE;
	STDMETHOD_(void,SetScale)(THIS_ int nScale) PURE;
};

SNSEND