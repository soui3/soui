#pragma once

#ifdef _DEBUG
#include <crtdbg.h>
#   define SASSERT_FMTW(expr, format, ...) \
	(void) ((!!(expr)) || \
	(1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, format, __VA_ARGS__)) || \
	(_CrtDbgBreak(), 0))

#   define SASSERT_FMTA(expr, format, ...) \
	(void) ((!!(expr)) || \
	(1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, format, __VA_ARGS__)) || \
	(_CrtDbgBreak(), 0))
#else
#   define SASSERT_FMTW(expr, format, ...) \
	if(!(expr)) SLOGFMTW(format,__VA_ARGS__);

#   define SASSERT_FMTA(expr, format, ...) \
	if(!(expr)) SLOGFMTW(format,__VA_ARGS__);
#endif

#ifdef _UNICODE 
#   define SASSERT_FMT    SASSERT_FMTW
#else
#   define SASSERT_FMT    SASSERT_FMTA
#endif//_UNICODE