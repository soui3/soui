#include <core-def.h>

#ifdef DLL_CORE
#ifdef UTILITIES_EXPORTS
#define UTILITIES_API __declspec(dllexport)
#else
#define UTILITIES_API __declspec(dllimport)
#endif
#else
#define UTILITIES_API
#endif


#ifndef SASSERT
#include <assert.h>
#define SASSERT(x) assert(x)
#endif

#include <ObjBase.h>
#include <com-def.h>

#ifdef LIB_SOUI_COM
#define SOUI_COM_API
#define SOUI_COM_C
#else
#define SOUI_COM_API __declspec(dllexport)
#define SOUI_COM_C  EXTERN_C
#endif//LIB_SOUI_COM

#include <sdef.h>