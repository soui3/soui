#pragma once

#include <ObjBase.h>
#include <com-def.h>

#ifdef LIB_SOUI_COM
#define SOUI_COM_API
#define SOUI_COM_C
#else
#define SOUI_COM_API __declspec(dllexport)
#define SOUI_COM_C  EXTERN_C
#endif//SOUI_COM_DLL


#undef INTERFACE
#define INTERFACE IObjRef
DECLARE_INTERFACE(IObjRef)
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
};
