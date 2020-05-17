#pragma once

#include <interface/obj-ref-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE IPathEffect
DECLARE_INTERFACE_(IPathEffect,IObjRef)
{
	//!添加引用
	/*!
	*/
	STDMETHOD_(long,AddRef) (THIS) PURE;

	//!释放引用
	/*!
	*/
	STDMETHOD_(long,Release) (THIS) PURE;

	//!释放对象
	/*!
	*/
	STDMETHOD_(void,OnFinalRelease) (THIS) PURE;
	STDMETHOD_(void *,GetRealPathEffect)(THIS) PURE;
};

#undef INTERFACE
#define INTERFACE ICornerPathEffect
DECLARE_INTERFACE_IID_(ICornerPathEffect,IPathEffect,"3C4A07C2-2617-432a-ADFC-8921D8B2E2BD")
{
	STDMETHOD_(void,Init)(THIS_ float radius) PURE;
};

#undef INTERFACE
#define INTERFACE IDashPathEffect
DECLARE_INTERFACE_IID_(IDashPathEffect,IPathEffect,"5FA8CF4C-8C83-4eac-82C0-EE160E04D78C")
{
	STDMETHOD_(void,Init)(THIS_ float intervals[], int count, float phase) PURE;
};

#undef INTERFACE
#define INTERFACE IDashPathEffect
DECLARE_INTERFACE_IID_(IDiscretePathEffect,IPathEffect,"51D32163-594E-4fbe-B2A5-8A07DEDDBEAB")
{
	STDMETHOD_(void,Init)(THIS_ float segmentLength, float deviation) PURE;
};

SNSEND