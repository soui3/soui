#include <string/sstringa.h>
#include <soui_mem_wrapper.h>
#include <stdio.h>
#ifndef _MBCS
#define _MBCS
#endif
#include <mbstring.h>

#define smin_tsr(a,b)            (((a) < (b)) ? (a) : (b))

SNSBEGIN

int char_traits::LoadString(HINSTANCE hInst, UINT uID, char* lpBuffer, int nBufferMax)
{
	return ::LoadStringA(hInst, uID, lpBuffer, nBufferMax);
}

int char_traits::Format(char** ppszDst, const char* pszFormat, va_list args)
{
	int len = _vscprintf(pszFormat, args); // _vscprintf doesn't count terminating '\0'
	if (len == 0) return 0;
	*ppszDst = (char*)soui_mem_wrapper::SouiMalloc(len + 1);
	vsprintf_s(*ppszDst, len + 1, pszFormat, args);
	return len;
}

char* char_traits::CharNext(char* psz)
{
#ifdef _MBCS
	return (char*)_mbsinc((unsigned char*)psz);
#else
	return psz + 1;
#endif
}

char char_traits::CharUpper(char ch)
{
	return (char)toupper(ch);
}

char char_traits::CharLower(char ch)
{
	return (char)tolower(ch);
}

int char_traits::IsSpace(char ch)
{
#ifdef _MBCS
	return _ismbcspace(ch);
#else
	return isspace(ch);
#endif
}

char* char_traits::StrLower(char* psz)
{
#ifdef _MBCS
	return (char*)_mbslwr((unsigned char*)psz);
#else
	return strlwr(psz);
#endif
}

char* char_traits::StrUpper(char* psz)
{
#ifdef _MBCS
	return (char*)_mbsupr((unsigned char*)psz);
#else
	return strupr(psz);
#endif
}

char* char_traits::StrStr(const char* psz, const char* psz2)
{
#ifdef _MBCS
	return (char*)_mbsstr((unsigned char*)psz, (unsigned char*)psz2);
#else
	return strstr(psz, psz2);
#endif
}

char* char_traits::StrRChr(const char* psz, char ch)
{
#ifdef _MBCS
	return (char*)_mbsrchr((unsigned char*)psz, ch);
#else
	return strrchr(psz, ch);
#endif
}

char* char_traits::StrChr(const char* psz, char ch)
{
#ifdef _MBCS
	return (char*)_mbschr((unsigned char*)psz, ch);
#else
	return strchr(psz, ch);
#endif
}

int char_traits::CompareNoCase(const char* psz1, const char* psz2)
{
#ifdef _MBCS
	return _mbsicmp((unsigned char*)psz1, (unsigned char*)psz2);
#else
	return stricmp(psz1, psz2);
#endif
}

int char_traits::Compare(const char* psz1, const char* psz2)
{
#ifdef _MBCS
	return _mbscmp((unsigned char*)psz1, (unsigned char*)psz2);
#else
	return strcmp(psz1, psz2);
#endif
}

size_t char_traits::StrLen(const char* psz)
{
	return psz ? strlen(psz) : 0;
}

//////////////////////////////////////////////////////////////////////////


void SStringA::ReleaseData(TStringData* pData)
{
	if (pData != TStringData::InitDataNil())
	{
		SASSERT(pData->nRefs != 0);
		pData->Release();
	}
}

TStringData* SStringA::AllocData(int nLength, TStringData* pOldData /*= NULL*/)
{
	SASSERT(nLength >= 0);
	SASSERT(nLength <= 0x7fffffff);    // max size (enough room for 1 extra)

	if (nLength == 0)
		return TStringData::InitDataNil();

	int nSize = sizeof(TStringData) + (nLength + 1) * sizeof(char);
	TStringData* pData;
	if (pOldData == NULL)
		pData = (TStringData*)soui_mem_wrapper::SouiMalloc(nSize);
	else
		pData = (TStringData*)soui_mem_wrapper::SouiRealloc(pOldData, nSize);
	if (pData == NULL)
		return NULL;

	pData->nRefs = 1;
	pData->nDataLength = nLength;
	pData->nAllocLength = nLength;

	char* pchData = (char*)pData->data();
	pchData[nLength] = '\0';

	return pData;
}

void SStringA::Release()
{
	TStringData* pData = GetData();
	if (pData != TStringData::InitDataNil())
	{
		SASSERT(pData->nRefs != 0);
		pData->Release();
		Init();
	}
}

bool SStringA::ReallocBuffer(int nNewLength)
{
#define TSTRING_REALLOC
#ifdef TSTRING_REALLOC
	TStringData* pData = GetData();
	if (!pData->IsShared() && pData != TStringData::InitDataNil())
	{
		pData = AllocData(nNewLength, pData);
		if (pData != NULL)
		{
			m_pszData = (char*)pData->data();
			return true;
		}

		Init();
		return false;
	}
#endif
	TStringData* pOldData = GetData();
	char* psz = m_pszData;
	if (AllocBuffer(nNewLength))
	{
		int nLength = smin_tsr(pOldData->nDataLength, nNewLength) + 1;
		memcpy(m_pszData, psz, nLength * sizeof(char));
		ReleaseData(pOldData);
		return true;
	}
	return false;
}

bool SStringA::AllocBuffer(int nLength)
{
	TStringData* pData = AllocData(nLength);
	if (pData != NULL)
	{
		m_pszData = (char*)pData->data();
		return true;
	}
	return false;
}

bool SStringA::AllocBeforeWrite(int nLen)
{
	bool bRet = true;
	TStringData* pData = GetData();
	if (pData->IsShared() || nLen > pData->nAllocLength)
	{
		Release();
		bRet = AllocBuffer(nLen);
	}
	SASSERT(GetData()->nRefs <= 1);
	return bRet;
}

void SStringA::CopyBeforeWrite()
{
	TStringData* pData = GetData();
	if (pData->IsShared())
	{
		Release();
		if (AllocBuffer(pData->nDataLength))
			memcpy(m_pszData, pData->data(), (pData->nDataLength + 1) * sizeof(char));
	}
	SASSERT(GetData()->nRefs <= 1);
}

void SStringA::ConcatInPlace(int nSrcLen, const char* pszSrcData)
{
	//  -- the main routine for += operators

	// concatenating an empty string is a no-op!
	if (nSrcLen == 0)
		return;

	// if the buffer is too small, or we have a width mis-match, just
	//   allocate a new buffer (slow but sure)
	TStringData* pData = GetData();
	if (pData->IsShared() || pData->nDataLength + nSrcLen > pData->nAllocLength)
	{
		// we have to grow the buffer
		int nOldDataLength = pData->nDataLength;
		int nNewLength = nOldDataLength + nSrcLen;
		if (ReallocBuffer(nNewLength))
			memcpy(m_pszData + nOldDataLength, pszSrcData, nSrcLen * sizeof(char));
	}
	else
	{
		// fast concatenation when buffer big enough
		memcpy(m_pszData + pData->nDataLength, pszSrcData, nSrcLen * sizeof(char));
		pData->nDataLength += nSrcLen;
		SASSERT(pData->nDataLength <= pData->nAllocLength);
		m_pszData[pData->nDataLength] = '\0';
	}
}

bool SStringA::ConcatCopy(int nSrc1Len, const char* pszSrc1Data, int nSrc2Len, const char* pszSrc2Data)
{
	// -- master concatenation routine
	// Concatenate two sources
	// -- assume that 'this' is a new TStringT object

	bool bRet = true;
	int nNewLength = nSrc1Len + nSrc2Len;
	if (nNewLength != 0)
	{
		bRet = ReallocBuffer(nNewLength);
		if (bRet)
		{
			memcpy(m_pszData, pszSrc1Data, nSrc1Len * sizeof(char));
			memcpy(m_pszData + nSrc1Len, pszSrc2Data, nSrc2Len * sizeof(char));
		}
	}
	return bRet;
}

void SStringA::AssignCopy(int nSrcLen, const char* pszSrcData)
{
	if (AllocBeforeWrite(nSrcLen))
	{
		memcpy(m_pszData, pszSrcData, nSrcLen * sizeof(char));
		GetData()->nDataLength = nSrcLen;
		m_pszData[nSrcLen] = '\0';
	}
}

void SStringA::AllocCopy(SStringA& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const
{
	// will clone the data attached to this string
	// allocating 'nExtraLen' characters
	// Places results in uninitialized string 'dest'
	// Will copy the part or all of original data to start of new string

	int nNewLen = nCopyLen + nExtraLen;
	if (nNewLen == 0)
	{
		dest.Init();
	}
	else
	{
		if (dest.ReallocBuffer(nNewLen))
			memcpy(dest.m_pszData, m_pszData + nCopyIndex, nCopyLen * sizeof(char));
	}
}

void SStringA::_AppendFormat(const char* pszFormat, va_list args)
{
	if (pszFormat == NULL || *pszFormat == '\0')
		return;

	char* pszBuffer = NULL;
	int nLength = char_traits::Format(&pszBuffer, pszFormat, args);
	if (nLength > 0 && pszBuffer != NULL)
	{
		*this += SStringA(pszBuffer, nLength);
		soui_mem_wrapper::SouiFree(pszBuffer);
	}
}

BOOL SStringA::_Format(const char* pszFormat, va_list args)
{
	if (pszFormat == NULL || *pszFormat == '\0')
	{
		Empty();
		return FALSE;
	}

	char* pszBuffer = NULL;
	int nLength = char_traits::Format(&pszBuffer, pszFormat, args);
	if (nLength > 0 && pszBuffer != NULL)
	{
		*this = SStringA(pszBuffer, nLength);
		soui_mem_wrapper::SouiFree(pszBuffer);
		return TRUE;
	}
	else
	{
		Empty();
		return FALSE;
	}
}

void SStringA::Init()
{
	m_pszData = (char*)TStringData::InitPszNil();
}

TStringData* SStringA::GetData() const
{
	SASSERT(m_pszData != NULL);
	return ((TStringData*)m_pszData) - 1;
}

int SStringA::SafeStrlen(const char* psz)
{
	return (psz == NULL) ? 0 : (int)char_traits::StrLen(psz);
}

int SStringA::GetAllocLength() const
{
	return GetData()->nAllocLength;
}

void SStringA::UnlockBuffer()
{
	if (GetData() != TStringData::InitDataNil())
		GetData()->Unlock();
}

char* SStringA::LockBuffer()
{
	char* psz = GetBuffer(0);
	if (psz != NULL)
		GetData()->Lock();
	return psz;
}

void SStringA::FreeExtra()
{
	TStringData* pData = GetData();
	SASSERT(pData->nDataLength <= pData->nAllocLength);
	if (pData->nDataLength < pData->nAllocLength)
	{
		if (ReallocBuffer(pData->nDataLength))
			SASSERT(m_pszData[GetData()->nDataLength] == '\0');
	}
	SASSERT(GetData() != NULL);
}

void SStringA::Preallocate(int nLength)
{
	int nOldLength = GetLength();
	GetBuffer(nLength);
	ReleaseBuffer(nOldLength);
}

void SStringA::SetLength(int nLength)
{
	SASSERT(nLength >= 0);
	SASSERT(nLength <= GetData()->nAllocLength);

	if (nLength >= 0 && nLength < GetData()->nAllocLength)
	{
		GetData()->nDataLength = nLength;
		m_pszData[nLength] = 0;
	}
}

void SStringA::Copy(const IStringA *pSrc)
{
	AssignCopy(pSrc->GetLength(),pSrc->c_str());
}

void SStringA::Assign(LPCSTR src)
{
	AssignCopy(strlen(src),src);
}

void SStringA::Assign2(LPCSTR src,int nLen)
{
	AssignCopy(nLen,src);
}

LPVOID SStringA::GetPrivData() SCONST
{
	return GetData();
}

char* SStringA::GetBufferSetLength(int nNewLength)
{
	SASSERT(nNewLength >= 0);

	if (GetBuffer(nNewLength) == NULL)
		return NULL;

	GetData()->nDataLength = nNewLength;
	m_pszData[nNewLength] = '\0';
	return m_pszData;
}

void SStringA::ReleaseBuffer(int nNewLength /*= -1*/)
{
	CopyBeforeWrite();  // just in case GetBuffer was not called

	if (nNewLength == -1)
		nNewLength = SafeStrlen(m_pszData); // zero terminated

	TStringData* pData = GetData();
	SASSERT(nNewLength <= pData->nAllocLength);
	pData->nDataLength = nNewLength;
	m_pszData[nNewLength] = '\0';
}

char* SStringA::GetBuffer(int nMinBufLength)
{
	SASSERT(nMinBufLength >= 0);

	TStringData* pData = GetData();
	if (pData->IsShared() || nMinBufLength > pData->nAllocLength)
	{
		// we have to grow the buffer
		int nOldLen = pData->nDataLength;
		if (nMinBufLength < nOldLen)
			nMinBufLength = nOldLen;
		if (!ReallocBuffer(nMinBufLength))
			return NULL;
	}
	SASSERT(GetData()->nRefs <= 1);

	// return a pointer to the character storage for this string
	SASSERT(m_pszData != NULL);
	return m_pszData;
}

SStringA __cdecl SStringA::AppendFormat(const char* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	_AppendFormat(pszFormat, argList);
	va_end(argList);
	return *this;
}

void __cdecl SStringA::AppendFormat(HINSTANCE hInst,UINT nFormatID, ...)
{
	SStringA strFormat;
	if (!strFormat.LoadString(nFormatID, hInst))
		return;

	va_list argList;
	va_start(argList, nFormatID);
	_AppendFormat(strFormat, argList);
	va_end(argList);
}

SStringA __cdecl SStringA::Format(const char* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	_Format(pszFormat, argList);
	va_end(argList);
	return *this;
}

BOOL __cdecl SStringA::Format(HINSTANCE hInst,UINT nFormatID, ...)
{
	SStringA strFormat;
	if (!strFormat.LoadString(nFormatID, hInst))
	{
		Empty();
		return FALSE;
	}

	va_list argList;
	va_start(argList, nFormatID);
	BOOL bRet = _Format(strFormat, argList);
	va_end(argList);
	return bRet;
}

BOOL SStringA::LoadString(UINT nID,HINSTANCE hInst)
{
	SASSERT(hInst);
	char buf[1024 + 1];
	int nChar = char_traits::LoadString(hInst, nID, buf, 1024);
	if (nChar == 0) return FALSE;
	AssignCopy(nChar, buf);
	return TRUE;
}

int SStringA::Find(const char* pszSub, int nStart /*= 0*/) const
{
	int nLength = GetData()->nDataLength;
	if (nStart > nLength)
		return -1;

	// find first matching substring
	char* psz = char_traits::StrStr(m_pszData + nStart, pszSub);

	// return -1 for not found, distance from beginning otherwise
	return (psz == NULL) ? -1 : (int)(psz - m_pszData);
}

int SStringA::Find(char ch, int nStart /*= 0*/) const
{
	int nLength = GetData()->nDataLength;
	if (nStart >= nLength)
		return -1;

	// find first single character
	char* psz = char_traits::StrChr(m_pszData + nStart, ch);

	// return -1 if not found and index otherwise
	return (psz == NULL) ? -1 : (int)(psz - m_pszData);
}

int SStringA::ReverseFind(char ch) const
{
	// find last single character
	char* psz = char_traits::StrRChr(m_pszData, ch);

	// return -1 if not found, distance from beginning otherwise
	return (psz == NULL) ? -1 : (int)(psz - m_pszData);
}

int SStringA::Remove(char chRemove)
{
	CopyBeforeWrite();

	char* pstrSource = m_pszData;
	char* pstrDest = m_pszData;
	char* pstrEnd = m_pszData + GetData()->nDataLength;

	while (pstrSource < pstrEnd)
	{
		if (*pstrSource != chRemove)
		{
			*pstrDest = *pstrSource;
			pstrDest = char_traits::CharNext(pstrDest);
		}
		pstrSource = char_traits::CharNext(pstrSource);
	}
	*pstrDest = '\0';
	int nCount = (int)(pstrSource - pstrDest);
	GetData()->nDataLength -= nCount;

	return nCount;
}

int SStringA::Replace(const char* pszOld, const char* pszNew)
{
	// can't have empty or NULL pszOld
	int nSourceLen = SafeStrlen(pszOld);
	if (nSourceLen == 0)
		return 0;
	int nReplacementLen = SafeStrlen(pszNew);

	// loop once to figure out the size of the result string
	int nCount = 0;
	char* pszStart = m_pszData;
	char* pszEnd = m_pszData + GetData()->nDataLength;
	char* pszTarget;
	while (pszStart < pszEnd)
	{
		while ((pszTarget = char_traits::StrStr(pszStart, pszOld)) != NULL)
		{
			nCount++;
			pszStart = pszTarget + nSourceLen;
		}
		pszStart += char_traits::StrLen(pszStart) + 1;
	}

	// if any changes were made, make them
	if (nCount > 0)
	{
		CopyBeforeWrite();

		// if the buffer is too small, just
		//   allocate a new buffer (slow but sure)
		TStringData* pOldData = GetData();
		int nOldLength = pOldData->nDataLength;
		int nNewLength = nOldLength + (nReplacementLen - nSourceLen) * nCount;
		if (pOldData->nAllocLength < nNewLength || pOldData->IsShared())
			if (!ReallocBuffer(nNewLength))
				return -1;

		// else, we just do it in-place
		pszStart = m_pszData;
		pszEnd = m_pszData + GetData()->nDataLength;

		// loop again to actually do the work
		while (pszStart < pszEnd)
		{
			while ((pszTarget = char_traits::StrStr(pszStart, pszOld)) != NULL)
			{
				int nBalance = nOldLength - ((int)(pszTarget - m_pszData) + nSourceLen);
				memmove(pszTarget + nReplacementLen, pszTarget + nSourceLen, nBalance * sizeof(char));
				memcpy(pszTarget, pszNew, nReplacementLen * sizeof(char));
				pszStart = pszTarget + nReplacementLen;
				pszStart[nBalance] = '\0';
				nOldLength += (nReplacementLen - nSourceLen);
			}
			pszStart += char_traits::StrLen(pszStart) + 1;
		}
		SASSERT(m_pszData[nNewLength] == '\0');
		GetData()->nDataLength = nNewLength;
	}
	return nCount;
}

int SStringA::Replace(char chOld, char chNew)
{
	int nCount = 0;

	// short-circuit the nop case
	if (chOld != chNew)
	{
		CopyBeforeWrite();

		// otherwise modify each character that matches in the string
		char* psz = m_pszData;
		char* pszEnd = psz + GetData()->nDataLength;
		while (psz < pszEnd)
		{
			// replace instances of the specified character only
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz = char_traits::CharNext(psz);
		}
	}
	return nCount;
}

int SStringA::Delete(int nIndex, int nCount /*= 1*/)
{
	if (nIndex < 0)
		nIndex = 0;
	int nLength = GetData()->nDataLength;
	if (nCount > 0 && nIndex < nLength)
	{
		if((nIndex + nCount) > nLength)
			nCount = nLength - nIndex;

		CopyBeforeWrite();

		int nBytesToCopy = nLength - (nIndex + nCount) + 1;
		memmove(m_pszData + nIndex, m_pszData + nIndex + nCount, nBytesToCopy * sizeof(char));
		nLength -= nCount;
		GetData()->nDataLength = nLength;
	}

	return nLength;
}

int SStringA::Insert(int nIndex, const char* psz)
{
	if (nIndex < 0)
		nIndex = 0;

	int nInsertLength = SafeStrlen(psz);
	int nNewLength = GetData()->nDataLength;
	if (nInsertLength > 0)
	{
		CopyBeforeWrite();

		if (nIndex > nNewLength)
			nIndex = nNewLength;
		nNewLength += nInsertLength;

		TStringData* pData = GetData();
		if (pData->nAllocLength < nNewLength)
			if (!ReallocBuffer(nNewLength))
				return -1;

		// move existing bytes down
		memmove(m_pszData + nIndex + nInsertLength, m_pszData + nIndex, (nNewLength - nIndex - nInsertLength + 1) * sizeof(char));
		memcpy(m_pszData + nIndex, psz, nInsertLength * sizeof(char));
		GetData()->nDataLength = nNewLength;
	}
	return nNewLength;
}

int SStringA::Insert(int nIndex, char ch)
{
	CopyBeforeWrite();

	if (nIndex < 0)
		nIndex = 0;

	TStringData* pData = GetData();
	int nNewLength = pData->nDataLength;
	if (nIndex > nNewLength)
		nIndex = nNewLength;
	nNewLength++;

	if (pData->nAllocLength < nNewLength)
		if (!ReallocBuffer(nNewLength))
			return -1;

	// move existing bytes down
	memmove(m_pszData + nIndex + 1, m_pszData + nIndex, (nNewLength - nIndex) * sizeof(char));
	m_pszData[nIndex] = ch;
	GetData()->nDataLength = nNewLength;

	return nNewLength;
}

bool SStringA::EndsWith(const SStringA& suffix, bool IgnoreCase /*= false*/) const
{
	if (GetLength() >= suffix.GetLength())
	{
		SStringA _src = Right(suffix.GetLength());
		if (IgnoreCase)
			return 0 == _src.CompareNoCase(suffix);
		else
			return 0 == _src.Compare(suffix);
	}
	return false;
}

bool SStringA::StartsWith(const SStringA& prefix, bool IgnoreCase /*= false*/) const
{
	if (GetLength() >= prefix.GetLength())
	{
		SStringA _src = Left(prefix.GetLength());
		if (IgnoreCase)
			return 0 == _src.CompareNoCase(prefix);
		else
			return 0 == _src.Compare(prefix);
	}
	return false;
}

void SStringA::TrimBlank()
{
	if (IsEmpty()) return;

	const char * pbuf = m_pszData;
	const char * p = pbuf;
	//look for start
	while (*p)
	{
		if (!IsBlankChar(*p)) break;
		p++;
	}
	const char * p1 = p;   //get start
	//look for end
	const char * p2 = pbuf + GetLength() - 1;
	while (p2 >= p1)
	{
		if (!IsBlankChar(*p2)) break;
		p2--;
	}
	if (p2 < p1)
		Empty();
	else
		(*this) = SStringA(p1, (int)(p2 - p1 + 1));
}

bool SStringA::IsBlankChar(const char &c)
{
	const char szBlank[] = { 0x0a,0x0d,0x20,0x09 };
	for (int i = 0; i < ARRAYSIZE(szBlank); i++)
	{
		if (c == szBlank[i]) return true;
	}
	return false;
}

SStringA & SStringA::Trim(char ch /*= VK_SPACE*/)
{
	TrimRight(ch);
	TrimLeft(ch);
	return *this;
}

SStringA & SStringA::TrimLeft(char chTarget /*= VK_SPACE*/)
{
	CopyBeforeWrite();

	// find first non-matching character
	char* psz = m_pszData;

	while (chTarget == *psz)
		psz = char_traits::CharNext(psz);

	if (psz != m_pszData)
	{
		// fix up data and length
		TStringData* pData = GetData();
		int nDataLength = pData->nDataLength - (int)(psz - m_pszData);
		memmove(m_pszData, psz, (nDataLength + 1) * sizeof(char));
		pData->nDataLength = nDataLength;
	}
	return *this;
}

SStringA & SStringA::TrimRight(char chTarget /*= VK_SPACE*/)
{
	CopyBeforeWrite();

	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)
	char* psz = m_pszData;
	char* pszLast = NULL;

	while (*psz != '\0')
	{
		if (*psz == chTarget)
		{
			if (pszLast == NULL)
				pszLast = psz;
		}
		else
			pszLast = NULL;
		psz = char_traits::CharNext(psz);
	}

	if (pszLast != NULL)
	{
		// truncate at left-most matching character
		*pszLast = '\0';
		GetData()->nDataLength = (int)(pszLast - m_pszData);
	}
	return *this;
}

SStringA& SStringA::MakeLower()
{
	CopyBeforeWrite();

	if (m_pszData != NULL)
		char_traits::StrLower(m_pszData);
	return *this;
}

SStringA& SStringA::MakeUpper()
{
	CopyBeforeWrite();

	if (m_pszData != NULL)
		char_traits::StrUpper(m_pszData);
	return *this;
}

SOUI::SStringA SStringA::Left(int nCount) const
{
	TStringData* pData = GetData();
	if (nCount < 0)
		nCount = 0;
	else if (nCount > pData->nDataLength)
		nCount = pData->nDataLength;

	SStringA dest;
	AllocCopy(dest, nCount, 0, 0);
	return dest;
}

SOUI::SStringA SStringA::Right(int nCount) const
{
	TStringData* pData = GetData();
	if (nCount < 0)
		nCount = 0;
	else if (nCount > pData->nDataLength)
		nCount = pData->nDataLength;

	SStringA dest;
	AllocCopy(dest, nCount, pData->nDataLength - nCount, 0);
	return dest;
}

SOUI::SStringA SStringA::Mid(int nFirst, int nCount) const
{
	// out-of-bounds requests return sensible things
	if (nFirst < 0)
		nFirst = 0;
	if (nCount < 0)
		nCount = 0;

	TStringData* pData = GetData();
	if (nFirst + nCount > pData->nDataLength)
		nCount = pData->nDataLength - nFirst;
	if (nFirst > pData->nDataLength)
		nCount = 0;

	SStringA dest;
	AllocCopy(dest, nCount, nFirst, 0);
	return dest;
}

SOUI::SStringA SStringA::Mid(int nFirst) const
{
	return Mid(nFirst, GetData()->nDataLength - nFirst);
}

int SStringA::CompareNoCase(const char* psz) const
{
	return char_traits::CompareNoCase(m_pszData, psz);
}

int SStringA::Compare(const char* psz) const
{
	return char_traits::Compare(m_pszData, psz);
}

const SStringA& SStringA::Append(const SStringA& src)
{
	SStringA strCopy(src);
	ConcatInPlace(strCopy.GetData()->nDataLength, strCopy.m_pszData);
	return *this;
}

const SStringA& SStringA::Append(const char * psz)
{
	SStringA strCopy(psz);
	ConcatInPlace(strCopy.GetData()->nDataLength, strCopy.m_pszData);
	return *this;
}

const SStringA& SStringA::Append(char ch)
{
	ConcatInPlace(1, &ch);
	return *this;
}

const SStringA& SStringA::operator+=(const SStringA& src)
{
	return Append(src);
}

const SStringA& SStringA::operator+=(char ch)
{
	return Append(ch);
}

const SStringA& SStringA::operator+=(const char* psz)
{
	return Append(psz);
}

const SStringA& SStringA::operator=(char ch)
{
	AssignCopy(1, &ch);
	return *this;
}

SStringA& SStringA::operator=(const char* psz)
{
	SStringA strCopy(psz);
	AssignCopy(strCopy.GetData()->nDataLength, strCopy.m_pszData);
	return *this;
}

SStringA& SStringA::operator=(const SStringA& stringSrc)
{
	if (m_pszData != stringSrc.m_pszData)
	{
		TStringData* pData = GetData();
		if ((pData->IsLocked() && pData != TStringData::InitDataNil()) || stringSrc.GetData()->IsLocked())
		{
			// actual copy necessary since one of the strings is locked
			AssignCopy(stringSrc.GetData()->nDataLength, stringSrc.m_pszData);
		}
		else
		{
			// can just copy references around
			Release();
			SASSERT(stringSrc.GetData() != TStringData::InitDataNil());
			m_pszData = stringSrc.m_pszData;
			GetData()->AddRef();
		}
	}
	return *this;
}

const char * SStringA::c_str() const
{
	return m_pszData;
}

SStringA::operator const char*() const // as a C string
{
	return m_pszData;
}

void SStringA::SetAt(int nIndex, char ch)
{
	SASSERT(nIndex >= 0);
	SASSERT(nIndex < GetData()->nDataLength);

	CopyBeforeWrite();
	m_pszData[nIndex] = ch;
}

char SStringA::operator[](int nIndex) const
{
	// same as GetAt
	SASSERT(nIndex >= 0);
	SASSERT(nIndex < GetData()->nDataLength);
	return m_pszData[nIndex];
}

char SStringA::GetAt(int nIndex) const
{
	SASSERT(nIndex >= 0);
	SASSERT(nIndex < GetData()->nDataLength);
	return m_pszData[nIndex];
}

void SStringA::Empty() // free up the data
{
	TStringData* pData = GetData();
	if (pData->nDataLength == 0)
		return;

	if (pData->nRefs >= 0)
		Release();
	else
	{
		char sz[1] = { 0 };
		*this = sz;
	}

	SASSERT(GetData()->nDataLength == 0);
	SASSERT(GetData()->IsLocked() || GetData()->nAllocLength == 0);
}

bool SStringA::IsEmpty() const
{
	return GetData()->nDataLength == 0;
}

int SStringA::GetLength() const
{
	return GetData()->nDataLength;
}

SStringA::~SStringA()
{
	//  free any attached data
	TStringData* pData = GetData();
	if (pData != TStringData::InitDataNil())
		pData->Release();
}

SStringA::SStringA(const char* psz)
{
	Init();
	int nLength = SafeStrlen(psz);
	if (nLength != 0)
	{
		if (AllocBuffer(nLength))
			memcpy(m_pszData, psz, nLength * sizeof(char));
	}
}

SStringA::SStringA(const char* psz, int nLength)
{
	Init();
	if (nLength < 0) nLength = SafeStrlen(psz);
	if (nLength != 0)
	{
		if (AllocBuffer(nLength))
			memcpy(m_pszData, psz, nLength * sizeof(char));
	}
}

SStringA::SStringA(char ch, int nLength /*= 1*/)
{
	Init();
	if (nLength >= 1)
	{
		if (AllocBuffer(nLength))
		{
			for (int i = 0; i < nLength; i++)
				m_pszData[i] = ch;
		}
	}
}


void SStringA::InitFromIString(const IStringA *stringSrc)
{
	TStringData *pData = (TStringData*)stringSrc->GetPrivData();
	SASSERT(pData->nRefs != 0);
	if (pData->nRefs >= 0)
	{
		SASSERT(pData != TStringData::InitDataNil());
		m_pszData = (char*)pData->data();
		GetData()->AddRef();
	}
	else
	{
		Init();
		m_pszData = (char*)pData->data();
	}
}

SStringA::SStringA(const IStringA * stringSrc)
{
	InitFromIString(stringSrc);
}

SStringA::SStringA(const SStringA& stringSrc)
{
	InitFromIString(&stringSrc);
}

SStringA::SStringA()
{
	Init();
}

SNSEND
