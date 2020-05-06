#pragma once
#include <utilities-def.h>


#undef INTERFACE
#define INTERFACE IStringA
DECLARE_INTERFACE(IStringA)
{
	STDMETHOD_(int, GetLength)(THIS) SCONST PURE;
	STDMETHOD_(bool, IsEmpty)(THIS) SCONST PURE;
	STDMETHOD_(void, Empty)(THIS) PURE;

	STDMETHOD_(char, GetAt)(THIS_ int nIndex) SCONST PURE;
	STDMETHOD_(void, SetAt)(THIS_ int nIndex, char ch)PURE;
	STDMETHOD_(const char *, c_str)(THIS) SCONST PURE;

	// string comparison
	STDMETHOD_(int, Compare)(THIS_ const char* psz) SCONST PURE;
	STDMETHOD_(int, CompareNoCase)(THIS_ const char* psz) SCONST PURE;

	STDMETHOD_(void, TrimBlank)(THIS) PURE;
	STDMETHOD_(int, Insert)(THIS_ int nIndex, char ch) PURE;
	STDMETHOD_(int, Insert)(THIS_ int nIndex, const char* psz)PURE;
	STDMETHOD_(int, Delete)(THIS_ int nIndex, int nCount)PURE;
	STDMETHOD_(int, Replace)(THIS_ char chOld, char chNew)PURE;
	STDMETHOD_(int, Replace)(THIS_ const char* pszOld, const char* pszNew)PURE;
	STDMETHOD_(int, Remove)(THIS_ char chRemove)PURE;

	STDMETHOD_(int, Find)(THIS_ char ch, int nStart) SCONST PURE;
	STDMETHOD_(int, ReverseFind)(THIS_ char ch) SCONST PURE;

	// find a sub-string (like strstr)
	STDMETHOD_(int, Find)(THIS_ const char* pszSub, int nStart) SCONST PURE;
	// Access to string implementation buffer as "C" character array
	STDMETHOD_(char*, GetBuffer)(THIS_ int nMinBufLength)PURE;
	STDMETHOD_(void ,ReleaseBuffer)(THIS_ int nNewLength)PURE;
	STDMETHOD_(char* ,GetBufferSetLength)(THIS_ int nNewLength)PURE;
	STDMETHOD_(void ,SetLength)(THIS_ int nLength)PURE;
	STDMETHOD_(void ,Copy)(THIS_ const IStringA * src) PURE;
};


#undef INTERFACE
#define INTERFACE IStringW
DECLARE_INTERFACE(IStringW)
{
	STDMETHOD_(int, GetLength)(THIS) SCONST PURE;
	STDMETHOD_(bool, IsEmpty)(THIS) SCONST PURE;
	STDMETHOD_(void, Empty)(THIS) PURE;

	STDMETHOD_(wchar_t, GetAt)(THIS_ int nIndex) SCONST PURE;
	STDMETHOD_(void, SetAt)(THIS_ int nIndex, wchar_t ch)PURE;
	STDMETHOD_(const wchar_t *, c_str)(THIS) SCONST PURE;

	// string comparison
	STDMETHOD_(int, Compare)(THIS_ const wchar_t* psz) SCONST PURE;
	STDMETHOD_(int, CompareNoCase)(THIS_ const wchar_t* psz) SCONST PURE;

	STDMETHOD_(void, TrimBlank)(THIS) PURE;
	STDMETHOD_(int, Insert)(THIS_ int nIndex, wchar_t ch) PURE;
	STDMETHOD_(int, Insert)(THIS_ int nIndex, const wchar_t* psz)PURE;
	STDMETHOD_(int, Delete)(THIS_ int nIndex, int nCount)PURE;
	STDMETHOD_(int, Replace)(THIS_ wchar_t chOld, wchar_t chNew)PURE;
	STDMETHOD_(int, Replace)(THIS_ const wchar_t* pszOld, const wchar_t* pszNew)PURE;
	STDMETHOD_(int, Remove)(THIS_ wchar_t chRemove)PURE;

	STDMETHOD_(int, Find)(THIS_ wchar_t ch, int nStart) SCONST PURE;
	STDMETHOD_(int, ReverseFind)(THIS_ wchar_t ch) SCONST PURE;

	// find a sub-string (like strstr)
	STDMETHOD_(int, Find)(THIS_ const wchar_t* pszSub, int nStart) SCONST PURE;
	// Access to string implementation buffer as "C" character array
	STDMETHOD_(wchar_t*, GetBuffer)(THIS_ int nMinBufLength)PURE;
	STDMETHOD_(void ,ReleaseBuffer)(THIS_ int nNewLength)PURE;
	STDMETHOD_(wchar_t* ,GetBufferSetLength)(THIS_ int nNewLength)PURE;
	STDMETHOD_(void ,SetLength)(THIS_ int nLength) PURE;
	STDMETHOD_(void ,Copy)(THIS_ const IStringW * src) PURE;
};