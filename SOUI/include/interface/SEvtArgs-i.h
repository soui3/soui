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

/*!
\brief
    Defines abstract interface which will be used when constructing various
    functor objects that bind slots to signals (or in CEGUI terms, handlers to
    events).
*/
#undef INTERFACE
#define INTERFACE IEvtArgs
DECLARE_INTERFACE_(IEvtSlot,IObjRef)
{
    STDMETHOD_(BOOL,Run)(THIS_ IEvtArgs *pArg) PURE;
    STDMETHOD_(IEvtSlot*, Clone)(THIS) SCONST PURE;
    STDMETHOD_(BOOL,Equal)(THIS_ const IEvtSlot * sour) SCONST PURE;
    STDMETHOD_(UINT,GetSlotType)(THIS) SCONST PURE;
};

SNSEND