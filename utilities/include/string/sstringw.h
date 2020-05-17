#ifndef __TSTRINGW_H__
#define __TSTRINGW_H__

#pragma once

#include <windows.h>
#include <utilities-def.h>
#include <string/sstringdata.h>
#include <interface/sstring-i.h>
#include <helper/obj-ref-impl.hpp>

SNSBEGIN

struct UTILITIES_API wchar_traits
{
	static size_t StrLen(const wchar_t* psz);
	static int Compare(const wchar_t* psz1, const wchar_t* psz2);
	static int CompareNoCase(const wchar_t* psz1, const wchar_t* psz2);
	static wchar_t* StrChr(const wchar_t* psz, wchar_t ch);
	static wchar_t* StrRChr(const wchar_t* psz, wchar_t ch);
	static wchar_t* StrStr(const wchar_t* psz, const wchar_t* psz2);
	static wchar_t* StrUpper(wchar_t* psz);
	static wchar_t* StrLower(wchar_t* psz);
	static int IsSpace(wchar_t ch);
	static wchar_t CharLower(wchar_t ch);
	static wchar_t CharUpper(wchar_t ch);
	static wchar_t* CharNext(wchar_t* psz);
	static int Format(wchar_t** ppszDst, const wchar_t* pszFormat, va_list args);
	static int LoadString(HINSTANCE hInst,
		UINT uID,
		wchar_t* lpBuffer,
		int nBufferMax);
};

class UTILITIES_API SStringW : public TObjRefImpl<IStringW>
{
public:
	typedef const wchar_t * pctstr;
	typedef wchar_t _tchar;
	typedef wchar_traits _tchar_traits;
	// Constructors
	SStringW();
	SStringW(const IStringW * stringSrc);
	SStringW(const SStringW& stringSrc);
	SStringW(wchar_t ch, int nLength = 1);
	SStringW(const wchar_t* psz, int nLength);
	SStringW(const wchar_t* psz);

	~SStringW();

	STDMETHOD_(int, GetLength)(THIS) SCONST ;
	STDMETHOD_(BOOL, IsEmpty)(THIS) SCONST ;
	STDMETHOD_(void, Empty)(THIS) ;

	STDMETHOD_(wchar_t, GetAt)(THIS_ int nIndex) SCONST ;
	STDMETHOD_(void, SetAt)(THIS_ int nIndex, wchar_t ch);
	STDMETHOD_(const wchar_t *, c_str)(THIS) SCONST ;

	// string comparison
	STDMETHOD_(int, Compare)(THIS_ const wchar_t* psz) SCONST ;
	STDMETHOD_(int, CompareNoCase)(THIS_ const wchar_t* psz) SCONST ;

	STDMETHOD_(void, TrimBlank)(THIS) ;
	STDMETHOD_(int, InsertChar)(THIS_ int nIndex, wchar_t ch) ;
	STDMETHOD_(int, Insert)(THIS_ int nIndex, const wchar_t* psz);
	STDMETHOD_(int, Delete)(THIS_ int nIndex, int nCount= 1);
	STDMETHOD_(int, ReplaceChar)(THIS_ wchar_t chOld, wchar_t chNew);
	STDMETHOD_(int, Replace)(THIS_ const wchar_t* pszOld, const wchar_t* pszNew);
	STDMETHOD_(int, Remove)(THIS_ wchar_t chRemove);

	STDMETHOD_(int, FindChar)(THIS_ wchar_t ch, int nStart=0) SCONST ;
	STDMETHOD_(int, ReverseFind)(THIS_ wchar_t ch) SCONST ;

	// find a sub-string (like strstr)
	STDMETHOD_(int, Find)(THIS_ const wchar_t* pszSub, int nStart=0) SCONST ;
	// Access to string implementation buffer as "C" character array
	STDMETHOD_(wchar_t*, GetBuffer)(THIS_ int nMinBufLength);
	STDMETHOD_(void ,ReleaseBuffer)(THIS_ int nNewLength=-1);
	STDMETHOD_(wchar_t* ,GetBufferSetLength)(THIS_ int nNewLength);
	STDMETHOD_(void ,SetLength)(THIS_ int nLength);
	STDMETHOD_(void ,Copy)(THIS_ const IStringW * src);
	STDMETHOD_(void ,Assign)(THIS_ LPCWSTR src);
	STDMETHOD_(void ,Assign2)(THIS_ LPCWSTR src,int nLen);
	STDMETHOD_(LPVOID,GetPrivData)(THIS) SCONST;


	// simple sub-string extraction
	SStringW Mid(int nFirst) const;
	SStringW Mid(int nFirst, int nCount) const;
	SStringW Right(int nCount) const;
	SStringW Left(int nCount) const;

	//    string utilities
	SStringW& MakeUpper();
	SStringW& MakeLower();

	// remove continuous occcurrences of characters in passed string, starting from right
	SStringW & TrimRight(wchar_t chTarget = VK_SPACE);

	// remove continuous occurrences of chTarget starting from left
	SStringW & TrimLeft(wchar_t chTarget = VK_SPACE);

	SStringW & Trim(wchar_t ch = VK_SPACE);


	static bool IsBlankChar(const wchar_t &c);

	bool StartsWith(const SStringW& prefix, bool IgnoreCase = false) const;

	bool EndsWith(const SStringW& suffix, bool IgnoreCase = false) const;

	wchar_t operator[](int nIndex) const;
	operator const wchar_t*() const;    // as a C string
	// overloaded assignment
	SStringW& operator=(const SStringW& stringSrc);
	SStringW& operator=(const wchar_t* psz);
	const SStringW& operator=(wchar_t ch);

	// string concatenation
	const SStringW& operator+=(const wchar_t* psz);

	const SStringW& operator+=(wchar_t ch);

	const SStringW& operator+=(const SStringW& src);

	const SStringW& Append(wchar_t ch);

	const SStringW& Append(const wchar_t * psz);

	const SStringW& Append(const SStringW& src);

	BOOL LoadString(UINT nID,HINSTANCE hInst);

	BOOL __cdecl Format(HINSTANCE hInst,UINT nFormatID, ...);

	void __cdecl AppendFormat(HINSTANCE hInst,UINT nFormatID, ...);

	// formatting (using sprintf style formatting)
	SStringW __cdecl Format(const wchar_t* pszFormat, ...);
	// Append formatted data using format string 'pszFormat'
	SStringW  __cdecl AppendFormat(const wchar_t* pszFormat, ...);

	friend inline bool __stdcall operator==(const SStringW& s1, const SStringW& s2)
	{
		return s1.Compare(s2) == 0;
	}
	friend inline bool __stdcall operator==(const SStringW& s1, const wchar_t* s2)
	{
		return s1.Compare(s2) == 0;
	}
	friend inline bool __stdcall operator==(const wchar_t* s1, const SStringW& s2)
	{
		return s2.Compare(s1) == 0;
	}

	friend inline bool __stdcall operator!=(const SStringW& s1, const SStringW& s2)
	{
		return s1.Compare(s2) != 0;
	}
	friend inline bool __stdcall operator!=(const SStringW& s1, const wchar_t* s2)
	{
		return s1.Compare(s2) != 0;
	}
	friend inline bool __stdcall operator!=(const wchar_t* s1, const SStringW& s2)
	{
		return s2.Compare(s1) != 0;
	}

	friend inline bool __stdcall operator<(const SStringW& s1, const SStringW& s2)
	{
		return s1.Compare(s2) < 0;
	}
	friend inline bool __stdcall operator<(const SStringW& s1, const wchar_t* s2)
	{
		return s1.Compare(s2) < 0;
	}
	friend inline bool __stdcall operator<(const wchar_t* s1, const SStringW& s2)
	{
		return s2.Compare(s1) > 0;
	}

	friend inline bool __stdcall operator>(const SStringW& s1, const SStringW& s2)
	{
		return s1.Compare(s2) > 0;
	}
	friend inline bool __stdcall operator>(const SStringW& s1, const wchar_t* s2)
	{
		return s1.Compare(s2) > 0;
	}
	friend inline bool __stdcall operator>(const wchar_t* s1, const SStringW& s2)
	{
		return s2.Compare(s1) < 0;
	}

	friend inline bool __stdcall operator<=(const SStringW& s1, const SStringW& s2)
	{
		return s1.Compare(s2) <= 0;
	}
	friend inline bool __stdcall operator<=(const SStringW& s1, const wchar_t* s2)
	{
		return s1.Compare(s2) <= 0;
	}
	friend inline bool __stdcall operator<=(const wchar_t* s1, const SStringW& s2)
	{
		return s2.Compare(s1) >= 0;
	}

	friend inline bool __stdcall operator>=(const SStringW& s1, const SStringW& s2)
	{
		return s1.Compare(s2) >= 0;
	}
	friend inline bool __stdcall operator>=(const SStringW& s1, const wchar_t* s2)
	{
		return s1.Compare(s2) >= 0;
	}
	friend inline bool __stdcall operator>=(const wchar_t* s1, const SStringW& s2)
	{
		return s2.Compare(s1) <= 0;
	}

	friend inline SStringW __stdcall operator+(const SStringW& string1, const SStringW& string2)
	{
		SStringW s;
		s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pszData, string2.GetData()->nDataLength, string2.m_pszData);
		return s;
	}
	friend inline SStringW __stdcall operator+(const SStringW& string, const wchar_t* psz)
	{
		SASSERT(psz != NULL);
		SStringW s;
		s.ConcatCopy(string.GetData()->nDataLength, string.m_pszData, SStringW::SafeStrlen(psz), psz);
		return s;
	}
	friend inline SStringW __stdcall operator+(const wchar_t* psz, const SStringW& string)
	{
		SASSERT(psz != NULL);
		SStringW s;
		s.ConcatCopy(SStringW::SafeStrlen(psz), psz, string.GetData()->nDataLength, string.m_pszData);
		return s;
	}
	friend inline SStringW __stdcall operator+(const SStringW& string1, wchar_t ch)
	{
		SStringW s;
		s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pszData, 1, &ch);
		return s;
	}
	friend inline SStringW __stdcall operator+(wchar_t ch, const SStringW& string)
	{
		SStringW s;
		s.ConcatCopy(1, &ch, string.GetData()->nDataLength, string.m_pszData);
		return s;
	}

	// Implementation
protected:
	int GetAllocLength() const;

	static int SafeStrlen(const wchar_t* psz);

	void Preallocate(int nLength);
	void FreeExtra();

protected:
	// implementation helpers
	TStringData* GetData() const;
	void Init();
	void InitFromIString(const IStringW *src);

	BOOL _Format(const wchar_t* pszFormat, va_list args);

	void _AppendFormat(const wchar_t* pszFormat, va_list args);

	// Assignment operators
	//  All assign a new value to the string
	//      (a) first see if the buffer is big enough
	//      (b) if enough room, copy on top of old buffer, set size and type
	//      (c) otherwise free old string data, and create a new one
	//
	//  All routines return the new string (but as a 'const TStringT&' so that
	//      assigning it again will cause a copy, eg: s1 = s2 = "hi there".
	//
	void AllocCopy(SStringW& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;

	void AssignCopy(int nSrcLen, const wchar_t* pszSrcData);

	bool ConcatCopy(int nSrc1Len, const wchar_t* pszSrc1Data, int nSrc2Len, const wchar_t* pszSrc2Data);
	void ConcatInPlace(int nSrcLen, const wchar_t* pszSrcData);
	void CopyBeforeWrite();
	bool AllocBeforeWrite(int nLen);

	bool AllocBuffer(int nLength);

	bool ReallocBuffer(int nNewLength);
	void _ReleaseData();

	// always allocate one extra character for '\0' termination
	// assumes [optimistically] that data length will equal allocation length
	static TStringData* AllocData(int nLength, TStringData* pOldData = NULL);
	static void ReleaseData(TStringData* pData);

protected:
	wchar_t* m_pszData;   // pointer to ref counted string data
};

SNSEND

#endif    //    __TSTRINGW_H__
