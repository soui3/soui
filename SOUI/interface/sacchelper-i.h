#pragma once
#include <utilities-def.h>

SNSBEGIN

interface IWindow;

#undef INTERFACE
#define INTERFACE IObjRef
DECLARE_INTERFACE_IID_(IAccHelper,IUnknown,"9FAF3067-6723-40DA-A56B-CDCB11B8902B")
{
	STDMETHOD_(void,SetOwner)(THIS_ IWindow *pOwner) PURE;
	STDMETHOD_(IWindow *,GetOwner)(THIS) SCONST PURE;
};

SNSEND