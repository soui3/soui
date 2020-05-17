#pragma once
#include <utilities-def.h>
#include <interface/obj-ref-i.h>
SNSBEGIN

#undef INTERFACE
#define INTERFACE IStringA
DECLARE_INTERFACE_(IStringA,IObjRef)
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

	STDMETHOD_(int, GetLength)(THIS) SCONST PURE;
	STDMETHOD_(BOOL, IsEmpty)(THIS) SCONST PURE;
	STDMETHOD_(void, Empty)(THIS) PURE;

	STDMETHOD_(char, GetAt)(THIS_ int nIndex) SCONST PURE;
	STDMETHOD_(void, SetAt)(THIS_ int nIndex, char ch)PURE;
	STDMETHOD_(const char *, c_str)(THIS) SCONST PURE;

	// string comparison
	STDMETHOD_(int, Compare)(THIS_ const char* psz) SCONST PURE;
	STDMETHOD_(int, CompareNoCase)(THIS_ const char* psz) SCONST PURE;

	STDMETHOD_(void, TrimBlank)(THIS) PURE;
	STDMETHOD_(int, InsertChar)(THIS_ int nIndex, char ch) PURE;
	STDMETHOD_(int, Insert)(THIS_ int nIndex, const char* psz)PURE;
	STDMETHOD_(int, Delete)(THIS_ int nIndex, int nCount)PURE;
	STDMETHOD_(int, ReplaceChar)(THIS_ char chOld, char chNew)PURE;
	STDMETHOD_(int, Replace)(THIS_ const char* pszOld, const char* pszNew)PURE;
	STDMETHOD_(int, Remove)(THIS_ char chRemove)PURE;

	STDMETHOD_(int, FindChar)(THIS_ char ch, int nStart) SCONST PURE;
	STDMETHOD_(int, ReverseFind)(THIS_ char ch) SCONST PURE;

	// find a sub-string (like strstr)
	STDMETHOD_(int, Find)(THIS_ const char* pszSub, int nStart) SCONST PURE;
	// Access to string implementation buffer as "C" character array
	STDMETHOD_(char*, GetBuffer)(THIS_ int nMinBufLength)PURE;
	STDMETHOD_(void ,ReleaseBuffer)(THIS_ int nNewLength)PURE;
	STDMETHOD_(char* ,GetBufferSetLength)(THIS_ int nNewLength)PURE;
	STDMETHOD_(void ,SetLength)(THIS_ int nLength)PURE;
	STDMETHOD_(void ,Copy)(THIS_ const IStringA * src) PURE;
	STDMETHOD_(void ,Assign)(THIS_ LPCSTR src) PURE;
	STDMETHOD_(void ,Assign2)(THIS_ LPCSTR src,int nLen) PURE;
	STDMETHOD_(LPVOID,GetPrivData)(THIS) SCONST PURE;
};


#undef INTERFACE
#define INTERFACE IStringW
DECLARE_INTERFACE_(IStringW,IObjRef)
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

	STDMETHOD_(int, GetLength)(THIS) SCONST PURE;
	STDMETHOD_(BOOL, IsEmpty)(THIS) SCONST PURE;
	STDMETHOD_(void, Empty)(THIS) PURE;

	STDMETHOD_(wchar_t, GetAt)(THIS_ int nIndex) SCONST PURE;
	STDMETHOD_(void, SetAt)(THIS_ int nIndex, wchar_t ch)PURE;
	STDMETHOD_(const wchar_t *, c_str)(THIS) SCONST PURE;

	// string comparison
	STDMETHOD_(int, Compare)(THIS_ const wchar_t* psz) SCONST PURE;
	STDMETHOD_(int, CompareNoCase)(THIS_ const wchar_t* psz) SCONST PURE;

	STDMETHOD_(void, TrimBlank)(THIS) PURE;
	STDMETHOD_(int, InsertChar)(THIS_ int nIndex, wchar_t ch) PURE;
	STDMETHOD_(int, Insert)(THIS_ int nIndex, const wchar_t* psz)PURE;
	STDMETHOD_(int, Delete)(THIS_ int nIndex, int nCount)PURE;
	STDMETHOD_(int, ReplaceChar)(THIS_ wchar_t chOld, wchar_t chNew)PURE;
	STDMETHOD_(int, Replace)(THIS_ const wchar_t* pszOld, const wchar_t* pszNew)PURE;
	STDMETHOD_(int, Remove)(THIS_ wchar_t chRemove)PURE;

	STDMETHOD_(int, FindChar)(THIS_ wchar_t ch, int nStart) SCONST PURE;
	STDMETHOD_(int, ReverseFind)(THIS_ wchar_t ch) SCONST PURE;

	// find a sub-string (like strstr)
	STDMETHOD_(int, Find)(THIS_ const wchar_t* pszSub, int nStart) SCONST PURE;
	// Access to string implementation buffer as "C" character array
	STDMETHOD_(wchar_t*, GetBuffer)(THIS_ int nMinBufLength)PURE;
	STDMETHOD_(void ,ReleaseBuffer)(THIS_ int nNewLength)PURE;
	STDMETHOD_(wchar_t* ,GetBufferSetLength)(THIS_ int nNewLength)PURE;
	STDMETHOD_(void ,SetLength)(THIS_ int nLength) PURE;
	STDMETHOD_(void ,Copy)(THIS_ const IStringW * src) PURE;
	STDMETHOD_(void ,Assign)(THIS_ LPCWSTR src) PURE;
	STDMETHOD_(void ,Assign2)(THIS_ LPCWSTR src,int nLen) PURE;
	STDMETHOD_(LPVOID,GetPrivData)(THIS) SCONST PURE;
};

#ifdef _UNICODE
#define IStringT IStringW
#else
#define IStringT IStringA
#endif

SNSEND