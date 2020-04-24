#ifndef __TSTRINGA_H__
#define __TSTRINGA_H__

#pragma once

#include <windows.h>
#include <utilities-def.h>
#include "sstringdata.h"

namespace SOUI
{
	struct UTILITIES_API char_traits
	{
		static size_t StrLen(const char* psz);
		static int Compare(const char* psz1, const char* psz2);
		static int CompareNoCase(const char* psz1, const char* psz2);
		static char* StrChr(const char* psz, char ch);
		static char* StrRChr(const char* psz, char ch);
		static char* StrStr(const char* psz, const char* psz2);
		static char* StrUpper(char* psz);
		static char* StrLower(char* psz);
		static int IsSpace(char ch);
		static char CharLower(char ch);
		static char CharUpper(char ch);
		static char* CharNext(char* psz);
		static int Format(char** ppszDst, const char* pszFormat, va_list args);

		static int LoadString(HINSTANCE hInst,
			UINT uID,
			char* lpBuffer,
			int nBufferMax);
	};

    class UTILITIES_API SStringA
    {
    public:
		typedef const char * pctstr;
		typedef char _tchar;
		typedef char_traits _tchar_traits;
        // Constructors
        SStringA();
        SStringA(const SStringA& stringSrc);
        SStringA(char ch, int nLength = 1);
        SStringA(const char* psz, int nLength);
        SStringA(const char* psz);

        ~SStringA();

        // Attributes & Operations
        // as an array of characters
        int GetLength() const;
        bool IsEmpty() const;
        void Empty();    // free up the data

        char GetAt(int nIndex) const;
        char operator[](int nIndex) const;
        void SetAt(int nIndex, char ch);
        operator const char*() const;    // as a C string

		const char * c_str() const;

        // overloaded assignment
        SStringA& operator=(const SStringA& stringSrc);
        SStringA& operator=(const char* psz);
        const SStringA& operator=(char ch);

        // string concatenation
        const SStringA& operator+=(const char* psz);

        const SStringA& operator+=(char ch);

        const SStringA& operator+=(const SStringA& src);

        const SStringA& Append(char ch);

        const SStringA& Append(const char * psz);

        const SStringA& Append(const SStringA& src);

        // string comparison
        int Compare(const char* psz) const;
        int CompareNoCase(const char* psz) const;

        // simple sub-string extraction
        SStringA Mid(int nFirst) const;
        SStringA Mid(int nFirst, int nCount) const;
        SStringA Right(int nCount) const;
        SStringA Left(int nCount) const;

        //    string utilities
        SStringA& MakeUpper();
        SStringA& MakeLower();

        // remove continuous occcurrences of characters in passed string, starting from right
        SStringA & TrimRight(char chTarget = VK_SPACE);

        // remove continuous occurrences of chTarget starting from left
        SStringA & TrimLeft(char chTarget = VK_SPACE);

        SStringA & Trim(char ch = VK_SPACE);


        static bool IsBlankChar(const char &c);

        void TrimBlank();

		bool StartsWith(const SStringA& prefix, bool IgnoreCase = false) const;

		bool EndsWith(const SStringA& suffix, bool IgnoreCase = false) const;

        // insert character at zero-based index; concatenates if index is past end of string
        int Insert(int nIndex, char ch);
        // insert substring at zero-based index; concatenates if index is past end of string
        int Insert(int nIndex, const char* psz);
        int Delete(int nIndex, int nCount = 1);
        int Replace(char chOld, char chNew);
        int Replace(const char* pszOld, const char* pszNew);
        int Remove(char chRemove);

        // searching (return starting index, or -1 if not found)
        // look for a single character match
        int Find(char ch, int nStart = 0) const;
        int ReverseFind(char ch) const;

        // find a sub-string (like strstr)
        int Find(const char* pszSub, int nStart = 0) const;

        BOOL LoadString(UINT nID,HINSTANCE hInst);

        BOOL __cdecl Format(HINSTANCE hInst,UINT nFormatID, ...);

        void __cdecl AppendFormat(HINSTANCE hInst,UINT nFormatID, ...);

        // formatting (using sprintf style formatting)
        SStringA __cdecl Format(const char* pszFormat, ...);
        // Append formatted data using format string 'pszFormat'
        SStringA  __cdecl AppendFormat(const char* pszFormat, ...);

        // Access to string implementation buffer as "C" character array
        char* GetBuffer(int nMinBufLength);
        void ReleaseBuffer(int nNewLength = -1);
        char* GetBufferSetLength(int nNewLength);
        void SetLength(int nLength);
        void Preallocate(int nLength);
        void FreeExtra();

        // Use LockBuffer/UnlockBuffer to turn refcounting off
        char* LockBuffer();
        void UnlockBuffer();

        friend inline bool __stdcall operator==(const SStringA& s1, const SStringA& s2)
        {
            return s1.Compare(s2) == 0;
        }
        friend inline bool __stdcall operator==(const SStringA& s1, const char* s2)
        {
            return s1.Compare(s2) == 0;
        }
        friend inline bool __stdcall operator==(const char* s1, const SStringA& s2)
        {
            return s2.Compare(s1) == 0;
        }

        friend inline bool __stdcall operator!=(const SStringA& s1, const SStringA& s2)
        {
            return s1.Compare(s2) != 0;
        }
        friend inline bool __stdcall operator!=(const SStringA& s1, const char* s2)
        {
            return s1.Compare(s2) != 0;
        }
        friend inline bool __stdcall operator!=(const char* s1, const SStringA& s2)
        {
            return s2.Compare(s1) != 0;
        }

        friend inline bool __stdcall operator<(const SStringA& s1, const SStringA& s2)
        {
            return s1.Compare(s2) < 0;
        }
        friend inline bool __stdcall operator<(const SStringA& s1, const char* s2)
        {
            return s1.Compare(s2) < 0;
        }
        friend inline bool __stdcall operator<(const char* s1, const SStringA& s2)
        {
            return s2.Compare(s1) > 0;
        }

        friend inline bool __stdcall operator>(const SStringA& s1, const SStringA& s2)
        {
            return s1.Compare(s2) > 0;
        }
        friend inline bool __stdcall operator>(const SStringA& s1, const char* s2)
        {
            return s1.Compare(s2) > 0;
        }
        friend inline bool __stdcall operator>(const char* s1, const SStringA& s2)
        {
            return s2.Compare(s1) < 0;
        }

        friend inline bool __stdcall operator<=(const SStringA& s1, const SStringA& s2)
        {
            return s1.Compare(s2) <= 0;
        }
        friend inline bool __stdcall operator<=(const SStringA& s1, const char* s2)
        {
            return s1.Compare(s2) <= 0;
        }
        friend inline bool __stdcall operator<=(const char* s1, const SStringA& s2)
        {
            return s2.Compare(s1) >= 0;
        }

        friend inline bool __stdcall operator>=(const SStringA& s1, const SStringA& s2)
        {
            return s1.Compare(s2) >= 0;
        }
        friend inline bool __stdcall operator>=(const SStringA& s1, const char* s2)
        {
            return s1.Compare(s2) >= 0;
        }
        friend inline bool __stdcall operator>=(const char* s1, const SStringA& s2)
        {
            return s2.Compare(s1) <= 0;
        }

        friend inline SStringA __stdcall operator+(const SStringA& string1, const SStringA& string2)
        {
            SStringA s;
            s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pszData, string2.GetData()->nDataLength, string2.m_pszData);
            return s;
        }
        friend inline SStringA __stdcall operator+(const SStringA& string, const char* psz)
        {
            SASSERT(psz != NULL);
            SStringA s;
            s.ConcatCopy(string.GetData()->nDataLength, string.m_pszData, SStringA::SafeStrlen(psz), psz);
            return s;
        }
        friend inline SStringA __stdcall operator+(const char* psz, const SStringA& string)
        {
            SASSERT(psz != NULL);
            SStringA s;
            s.ConcatCopy(SStringA::SafeStrlen(psz), psz, string.GetData()->nDataLength, string.m_pszData);
            return s;
        }
        friend inline SStringA __stdcall operator+(const SStringA& string1, char ch)
        {
            SStringA s;
            s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pszData, 1, &ch);
            return s;
        }
        friend inline SStringA __stdcall operator+(char ch, const SStringA& string)
        {
            SStringA s;
            s.ConcatCopy(1, &ch, string.GetData()->nDataLength, string.m_pszData);
            return s;
        }

        // Implementation
    public:
        int GetAllocLength() const;

        static int SafeStrlen(const char* psz);

    protected:
        // implementation helpers
        TStringData* GetData() const;
        void Init();

        BOOL _Format(const char* pszFormat, va_list args);

        void _AppendFormat(const char* pszFormat, va_list args);

        // Assignment operators
        //  All assign a new value to the string
        //      (a) first see if the buffer is big enough
        //      (b) if enough room, copy on top of old buffer, set size and type
        //      (c) otherwise free old string data, and create a new one
        //
        //  All routines return the new string (but as a 'const TStringT&' so that
        //      assigning it again will cause a copy, eg: s1 = s2 = "hi there".
        //
        void AllocCopy(SStringA& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;

        void AssignCopy(int nSrcLen, const char* pszSrcData);

        bool ConcatCopy(int nSrc1Len, const char* pszSrc1Data, int nSrc2Len, const char* pszSrc2Data);
        void ConcatInPlace(int nSrcLen, const char* pszSrcData);
        void CopyBeforeWrite();
        bool AllocBeforeWrite(int nLen);

        bool AllocBuffer(int nLength);

        bool ReallocBuffer(int nNewLength);
        void Release();

        // always allocate one extra character for '\0' termination
        // assumes [optimistically] that data length will equal allocation length
        static TStringData* AllocData(int nLength, TStringData* pOldData = NULL);
        static void ReleaseData(TStringData* pData);

    protected:
        char* m_pszData;   // pointer to ref counted string data
    };

}//end of namespace


#endif    //    __TSTRINGA_H__
