#pragma once

SNSBEGIN

#undef INTERFACE
#define INTERFACE IHostMsgHandler
DECLARE_INTERFACE(IHostMsgHandler)
{
	STDMETHOD_(void,OnHostMsg)(THIS_ BOOL bRelayout,UINT uMsg,WPARAM wp,LPARAM lp) PURE;
};

SNSEND