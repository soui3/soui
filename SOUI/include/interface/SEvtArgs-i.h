#pragma once

#include <interface/sobject-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE IEvtArgs
DECLARE_INTERFACE_(IEvtArgs,IObject)
{
	STDMETHOD_(IObject*,Sender)(THIS) PURE;
	STDMETHOD_(int,IdFrom)(THIS) SCONST PURE;
	STDMETHOD_(void,SetIdFrom)(THIS_ int id) PURE;
	STDMETHOD_(LPCWSTR,NameFrom) (THIS) SCONST PURE;
	STDMETHOD_(void,SetNameFrom) (THIS_ LPCWSTR name) PURE;
	STDMETHOD_(BOOL,IsBubbleUp) (THIS) SCONST PURE;
	STDMETHOD_(void,SetBubbleUp) (THIS_ BOOL bBubbleUp) PURE;
	STDMETHOD_(UINT,HandleCount)(THIS) SCONST PURE;
	STDMETHOD_(void,IncreaseHandleCount)(THIS) PURE;
	STDMETHOD_(LPCVOID,Data)(THIS) PURE;
};

SNSEND