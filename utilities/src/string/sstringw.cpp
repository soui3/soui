#include "string/sstringw.h"
#include <soui_mem_wrapper.h>
#include <stdio.h>

#define smin_tsr(a,b)            (((a) < (b)) ? (a) : (b))

namespace SOUI
{
	int wchar_traits::LoadString(HINSTANCE hInst, UINT uID, wchar_t* lpBuffer, int nBufferMax)
	{
		return ::LoadStringW(hInst, uID, lpBuffer, nBufferMax);
	}

	int wchar_traits::Format(wchar_t** ppszDst, const wchar_t* pszFormat, va_list args)
	{
		int len = _vscwprintf(pszFormat, args); // _vscprintf doesn't count terminating '\0'
		if (len == 0) return 0;
		*ppszDst = (wchar_t*)soui_mem_wrapper::SouiMalloc((len + 1) * sizeof(wchar_t));
		//vswprintf(*ppszDst, pszFormat, args);
		vswprintf_s(*ppszDst, len + 1, pszFormat, args);
		return len;
	}

	wchar_t* wchar_traits::CharNext(wchar_t* psz)
	{
		return psz + 1;
	}

	wchar_t wchar_traits::CharUpper(wchar_t ch)
	{
		return (wchar_t)towupper(ch);
	}

	wchar_t wchar_traits::CharLower(wchar_t ch)
	{
		return (wchar_t)towlower(ch);
	}

	int wchar_traits::IsSpace(wchar_t ch)
	{
		return iswspace(ch);
	}

	wchar_t* wchar_traits::StrLower(wchar_t* psz)
	{
		return _wcslwr(psz);
	}

	wchar_t* wchar_traits::StrUpper(wchar_t* psz)
	{
		return _wcsupr(psz);
	}

	wchar_t* wchar_traits::StrStr(const wchar_t* psz, const wchar_t* psz2)
	{
		return const_cast<wchar_t*>(wcsstr(psz, psz2));
	}

	wchar_t* wchar_traits::StrRChr(const wchar_t* psz, wchar_t ch)
	{
		return const_cast<wchar_t*>(wcsrchr(psz, ch));
	}

	wchar_t* wchar_traits::StrChr(const wchar_t* psz, wchar_t ch)
	{
		return const_cast<wchar_t*>(wcschr(psz, ch));
	}

	int wchar_traits::CompareNoCase(const wchar_t* psz1, const wchar_t* psz2)
	{
		return _wcsicmp(psz1, psz2);
	}

	int wchar_traits::Compare(const wchar_t* psz1, const wchar_t* psz2)
	{
		return wcscmp(psz1, psz2);
	}

	size_t wchar_traits::StrLen(const wchar_t* psz)
	{
		return psz ? wcslen(psz) : 0;
	}

	//////////////////////////////////////////////////////////////////////////


	void SStringW::ReleaseData(TStringData* pData)
	{
		if (pData != TStringData::InitDataNil())
		{
			SASSERT(pData->nRefs != 0);
			pData->Release();
		}
	}

	TStringData* SStringW::AllocData(int nLength, TStringData* pOldData /*= NULL*/)
	{
		SASSERT(nLength >= 0);
		SASSERT(nLength <= 0x7fffffff);    // max size (enough room for 1 extra)

		if (nLength == 0)
			return TStringData::InitDataNil();

		int nSize = sizeof(TStringData) + (nLength + 1) * sizeof(wchar_t);
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

		wchar_t* pchData = (wchar_t*)pData->data();
		pchData[nLength] = '\0';

		return pData;
	}

	void SStringW::Release()
	{
		TStringData* pData = GetData();
		if (pData != TStringData::InitDataNil())
		{
			SASSERT(pData->nRefs != 0);
			pData->Release();
			Init();
		}
	}

	bool SStringW::ReallocBuffer(int nNewLength)
	{
#define TSTRING_REALLOC
#ifdef TSTRING_REALLOC
		TStringData* pData = GetData();
		if (!pData->IsShared() && pData != TStringData::InitDataNil())
		{
			pData = AllocData(nNewLength, pData);
			if (pData != NULL)
			{
				m_pszData = (wchar_t*)pData->data();
				return true;
			}

			Init();
			return false;
		}
#endif
		TStringData* pOldData = GetData();
		wchar_t* psz = m_pszData;
		if (AllocBuffer(nNewLength))
		{
			int nLength = smin_tsr(pOldData->nDataLength, nNewLength) + 1;
			memcpy(m_pszData, psz, nLength * sizeof(wchar_t));
			ReleaseData(pOldData);
			return true;
		}
		return false;
	}

	bool SStringW::AllocBuffer(int nLength)
	{
		TStringData* pData = AllocData(nLength);
		if (pData != NULL)
		{
			m_pszData = (wchar_t*)pData->data();
			return true;
		}
		return false;
	}

	bool SStringW::AllocBeforeWrite(int nLen)
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

	void SStringW::CopyBeforeWrite()
	{
		TStringData* pData = GetData();
		if (pData->IsShared())
		{
			Release();
			if (AllocBuffer(pData->nDataLength))
				memcpy(m_pszData, pData->data(), (pData->nDataLength + 1) * sizeof(wchar_t));
		}
		SASSERT(GetData()->nRefs <= 1);
	}

	void SStringW::ConcatInPlace(int nSrcLen, const wchar_t* pszSrcData)
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
				memcpy(m_pszData + nOldDataLength, pszSrcData, nSrcLen * sizeof(wchar_t));
		}
		else
		{
			// fast concatenation when buffer big enough
			memcpy(m_pszData + pData->nDataLength, pszSrcData, nSrcLen * sizeof(wchar_t));
			pData->nDataLength += nSrcLen;
			SASSERT(pData->nDataLength <= pData->nAllocLength);
			m_pszData[pData->nDataLength] = '\0';
		}
	}

	bool SStringW::ConcatCopy(int nSrc1Len, const wchar_t* pszSrc1Data, int nSrc2Len, const wchar_t* pszSrc2Data)
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
				memcpy(m_pszData, pszSrc1Data, nSrc1Len * sizeof(wchar_t));
				memcpy(m_pszData + nSrc1Len, pszSrc2Data, nSrc2Len * sizeof(wchar_t));
			}
		}
		return bRet;
	}

	void SStringW::AssignCopy(int nSrcLen, const wchar_t* pszSrcData)
	{
		if (AllocBeforeWrite(nSrcLen))
		{
			memcpy(m_pszData, pszSrcData, nSrcLen * sizeof(wchar_t));
			GetData()->nDataLength = nSrcLen;
			m_pszData[nSrcLen] = '\0';
		}
	}

	void SStringW::AllocCopy(SStringW& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const
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
				memcpy(dest.m_pszData, m_pszData + nCopyIndex, nCopyLen * sizeof(wchar_t));
		}
	}

	void SStringW::_AppendFormat(const wchar_t* pszFormat, va_list args)
	{
		if (pszFormat == NULL || *pszFormat == '\0')
			return;

		wchar_t* pszBuffer = NULL;
		int nLength = wchar_traits::Format(&pszBuffer, pszFormat, args);
		if (nLength > 0 && pszBuffer != NULL)
		{
			*this += SStringW(pszBuffer, nLength);
			soui_mem_wrapper::SouiFree(pszBuffer);
		}
	}

	BOOL SStringW::_Format(const wchar_t* pszFormat, va_list args)
	{
		if (pszFormat == NULL || *pszFormat == '\0')
		{
			Empty();
			return FALSE;
		}

		wchar_t* pszBuffer = NULL;
		int nLength = wchar_traits::Format(&pszBuffer, pszFormat, args);
		if (nLength > 0 && pszBuffer != NULL)
		{
			*this = SStringW(pszBuffer, nLength);
			soui_mem_wrapper::SouiFree(pszBuffer);
			return TRUE;
		}
		else
		{
			Empty();
			return FALSE;
		}
	}

	void SStringW::Init()
	{
		m_pszData = (wchar_t*)TStringData::InitPszNil();
	}

	TStringData* SStringW::GetData() const
	{
		SASSERT(m_pszData != NULL);
		return ((TStringData*)m_pszData) - 1;
	}

	int SStringW::SafeStrlen(const wchar_t* psz)
	{
		return (psz == NULL) ? 0 : (int)wchar_traits::StrLen(psz);
	}

	int SStringW::GetAllocLength() const
	{
		return GetData()->nAllocLength;
	}

	void SStringW::UnlockBuffer()
	{
		if (GetData() != TStringData::InitDataNil())
			GetData()->Unlock();
	}

	wchar_t* SStringW::LockBuffer()
	{
		wchar_t* psz = GetBuffer(0);
		if (psz != NULL)
			GetData()->Lock();
		return psz;
	}

	void SStringW::FreeExtra()
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

	void SStringW::Preallocate(int nLength)
	{
		int nOldLength = GetLength();
		GetBuffer(nLength);
		ReleaseBuffer(nOldLength);
	}

	void SStringW::SetLength(int nLength)
	{
		SASSERT(nLength >= 0);
		SASSERT(nLength <= GetData()->nAllocLength);

		if (nLength >= 0 && nLength < GetData()->nAllocLength)
		{
			GetData()->nDataLength = nLength;
			m_pszData[nLength] = 0;
		}
	}

	wchar_t* SStringW::GetBufferSetLength(int nNewLength)
	{
		SASSERT(nNewLength >= 0);

		if (GetBuffer(nNewLength) == NULL)
			return NULL;

		GetData()->nDataLength = nNewLength;
		m_pszData[nNewLength] = '\0';
		return m_pszData;
	}

	void SStringW::ReleaseBuffer(int nNewLength /*= -1*/)
	{
		CopyBeforeWrite();  // just in case GetBuffer was not called

		if (nNewLength == -1)
			nNewLength = SafeStrlen(m_pszData); // zero terminated

		TStringData* pData = GetData();
		SASSERT(nNewLength <= pData->nAllocLength);
		pData->nDataLength = nNewLength;
		m_pszData[nNewLength] = '\0';
	}

	wchar_t* SStringW::GetBuffer(int nMinBufLength)
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

	SStringW __cdecl SStringW::AppendFormat(const wchar_t* pszFormat, ...)
	{
		va_list argList;
		va_start(argList, pszFormat);
		_AppendFormat(pszFormat, argList);
		va_end(argList);
		return *this;
	}

	void __cdecl SStringW::AppendFormat(HINSTANCE hInst,UINT nFormatID, ...)
	{
		SStringW strFormat;
		if (!strFormat.LoadString(nFormatID, hInst))
			return;

		va_list argList;
		va_start(argList, nFormatID);
		_AppendFormat(strFormat, argList);
		va_end(argList);
	}

	SStringW __cdecl SStringW::Format(const wchar_t* pszFormat, ...)
	{
		va_list argList;
		va_start(argList, pszFormat);
		_Format(pszFormat, argList);
		va_end(argList);
		return *this;
	}

	BOOL __cdecl SStringW::Format(HINSTANCE hInst,UINT nFormatID, ...)
	{
		SStringW strFormat;
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

	BOOL SStringW::LoadString(UINT nID,HINSTANCE hInst)
	{
		SASSERT(hInst);
		wchar_t buf[1024 + 1];
		int nChar = wchar_traits::LoadString(hInst, nID, buf, 1024);
		if (nChar == 0) return FALSE;
		AssignCopy(nChar, buf);
		return TRUE;
	}

	int SStringW::Find(const wchar_t* pszSub, int nStart /*= 0*/) const
	{
		int nLength = GetData()->nDataLength;
		if (nStart > nLength)
			return -1;

		// find first matching substring
		wchar_t* psz = wchar_traits::StrStr(m_pszData + nStart, pszSub);

		// return -1 for not found, distance from beginning otherwise
		return (psz == NULL) ? -1 : (int)(psz - m_pszData);
	}

	int SStringW::Find(wchar_t ch, int nStart /*= 0*/) const
	{
		int nLength = GetData()->nDataLength;
		if (nStart >= nLength)
			return -1;

		// find first single character
		wchar_t* psz = wchar_traits::StrChr(m_pszData + nStart, ch);

		// return -1 if not found and index otherwise
		return (psz == NULL) ? -1 : (int)(psz - m_pszData);
	}

	int SStringW::ReverseFind(wchar_t ch) const
	{
		// find last single character
		wchar_t* psz = wchar_traits::StrRChr(m_pszData, ch);

		// return -1 if not found, distance from beginning otherwise
		return (psz == NULL) ? -1 : (int)(psz - m_pszData);
	}

	int SStringW::Remove(wchar_t chRemove)
	{
		CopyBeforeWrite();

		wchar_t* pstrSource = m_pszData;
		wchar_t* pstrDest = m_pszData;
		wchar_t* pstrEnd = m_pszData + GetData()->nDataLength;

		while (pstrSource < pstrEnd)
		{
			if (*pstrSource != chRemove)
			{
				*pstrDest = *pstrSource;
				pstrDest = wchar_traits::CharNext(pstrDest);
			}
			pstrSource = wchar_traits::CharNext(pstrSource);
		}
		*pstrDest = '\0';
		int nCount = (int)(pstrSource - pstrDest);
		GetData()->nDataLength -= nCount;

		return nCount;
	}

	int SStringW::Replace(const wchar_t* pszOld, const wchar_t* pszNew)
	{
		// can't have empty or NULL pszOld
		int nSourceLen = SafeStrlen(pszOld);
		if (nSourceLen == 0)
			return 0;
		int nReplacementLen = SafeStrlen(pszNew);

		// loop once to figure out the size of the result string
		int nCount = 0;
		wchar_t* pszStart = m_pszData;
		wchar_t* pszEnd = m_pszData + GetData()->nDataLength;
		wchar_t* pszTarget;
		while (pszStart < pszEnd)
		{
			while ((pszTarget = wchar_traits::StrStr(pszStart, pszOld)) != NULL)
			{
				nCount++;
				pszStart = pszTarget + nSourceLen;
			}
			pszStart += wchar_traits::StrLen(pszStart) + 1;
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
				while ((pszTarget = wchar_traits::StrStr(pszStart, pszOld)) != NULL)
				{
					int nBalance = nOldLength - ((int)(pszTarget - m_pszData) + nSourceLen);
					memmove(pszTarget + nReplacementLen, pszTarget + nSourceLen, nBalance * sizeof(wchar_t));
					memcpy(pszTarget, pszNew, nReplacementLen * sizeof(wchar_t));
					pszStart = pszTarget + nReplacementLen;
					pszStart[nBalance] = '\0';
					nOldLength += (nReplacementLen - nSourceLen);
				}
				pszStart += wchar_traits::StrLen(pszStart) + 1;
			}
			SASSERT(m_pszData[nNewLength] == '\0');
			GetData()->nDataLength = nNewLength;
		}
		return nCount;
	}

	int SStringW::Replace(wchar_t chOld, wchar_t chNew)
	{
		int nCount = 0;

		// short-circuit the nop case
		if (chOld != chNew)
		{
			CopyBeforeWrite();

			// otherwise modify each character that matches in the string
			wchar_t* psz = m_pszData;
			wchar_t* pszEnd = psz + GetData()->nDataLength;
			while (psz < pszEnd)
			{
				// replace instances of the specified character only
				if (*psz == chOld)
				{
					*psz = chNew;
					nCount++;
				}
				psz = wchar_traits::CharNext(psz);
			}
		}
		return nCount;
	}

	int SStringW::Delete(int nIndex, int nCount /*= 1*/)
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
			memmove(m_pszData + nIndex, m_pszData + nIndex + nCount, nBytesToCopy * sizeof(wchar_t));
			nLength -= nCount;
			GetData()->nDataLength = nLength;
		}

		return nLength;
	}

	int SStringW::Insert(int nIndex, const wchar_t* psz)
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
			memmove(m_pszData + nIndex + nInsertLength, m_pszData + nIndex, (nNewLength - nIndex - nInsertLength + 1) * sizeof(wchar_t));
			memcpy(m_pszData + nIndex, psz, nInsertLength * sizeof(wchar_t));
			GetData()->nDataLength = nNewLength;
		}
		return nNewLength;
	}

	int SStringW::Insert(int nIndex, wchar_t ch)
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
		memmove(m_pszData + nIndex + 1, m_pszData + nIndex, (nNewLength - nIndex) * sizeof(wchar_t));
		m_pszData[nIndex] = ch;
		GetData()->nDataLength = nNewLength;

		return nNewLength;
	}

	bool SStringW::EndsWith(const SStringW& suffix, bool IgnoreCase /*= false*/) const
	{
		if (GetLength() >= suffix.GetLength())
		{
			SStringW _src = Right(suffix.GetLength());
			if (IgnoreCase)
				return 0 == _src.CompareNoCase(suffix);
			else
				return 0 == _src.Compare(suffix);
		}
		return false;
	}

	bool SStringW::StartsWith(const SStringW& prefix, bool IgnoreCase /*= false*/) const
	{
		if (GetLength() >= prefix.GetLength())
		{
			SStringW _src = Left(prefix.GetLength());
			if (IgnoreCase)
				return 0 == _src.CompareNoCase(prefix);
			else
				return 0 == _src.Compare(prefix);
		}
		return false;
	}

	void SStringW::TrimBlank()
	{
		if (IsEmpty()) return;

		const wchar_t * pbuf = m_pszData;
		const wchar_t * p = pbuf;
		//look for start
		while (*p)
		{
			if (!IsBlankChar(*p)) break;
			p++;
		}
		const wchar_t * p1 = p;   //get start
		//look for end
		const wchar_t * p2 = pbuf + GetLength() - 1;
		while (p2 >= p1)
		{
			if (!IsBlankChar(*p2)) break;
			p2--;
		}
		if (p2 < p1)
			Empty();
		else
			(*this) = SStringW(p1, (int)(p2 - p1 + 1));
	}

	bool SStringW::IsBlankChar(const wchar_t &c)
	{
		const wchar_t szBlank[] = { 0x0a,0x0d,0x20,0x09 };
		for (int i = 0; i < ARRAYSIZE(szBlank); i++)
		{
			if (c == szBlank[i]) return true;
		}
		return false;
	}

	SStringW & SStringW::Trim(wchar_t ch /*= VK_SPACE*/)
	{
		TrimRight(ch);
		TrimLeft(ch);
		return *this;
	}

	SStringW & SStringW::TrimLeft(wchar_t chTarget /*= VK_SPACE*/)
	{
		CopyBeforeWrite();

		// find first non-matching character
		wchar_t* psz = m_pszData;

		while (chTarget == *psz)
			psz = wchar_traits::CharNext(psz);

		if (psz != m_pszData)
		{
			// fix up data and length
			TStringData* pData = GetData();
			int nDataLength = pData->nDataLength - (int)(psz - m_pszData);
			memmove(m_pszData, psz, (nDataLength + 1) * sizeof(wchar_t));
			pData->nDataLength = nDataLength;
		}
		return *this;
	}

	SStringW & SStringW::TrimRight(wchar_t chTarget /*= VK_SPACE*/)
	{
		CopyBeforeWrite();

		// find beginning of trailing matches
		// by starting at beginning (DBCS aware)
		wchar_t* psz = m_pszData;
		wchar_t* pszLast = NULL;

		while (*psz != '\0')
		{
			if (*psz == chTarget)
			{
				if (pszLast == NULL)
					pszLast = psz;
			}
			else
				pszLast = NULL;
			psz = wchar_traits::CharNext(psz);
		}

		if (pszLast != NULL)
		{
			// truncate at left-most matching character
			*pszLast = '\0';
			GetData()->nDataLength = (int)(pszLast - m_pszData);
		}
		return *this;
	}

	SStringW& SStringW::MakeLower()
	{
		CopyBeforeWrite();

		if (m_pszData != NULL)
			wchar_traits::StrLower(m_pszData);
		return *this;
	}

	SStringW& SStringW::MakeUpper()
	{
		CopyBeforeWrite();

		if (m_pszData != NULL)
			wchar_traits::StrUpper(m_pszData);
		return *this;
	}

	SOUI::SStringW SStringW::Left(int nCount) const
	{
		TStringData* pData = GetData();
		if (nCount < 0)
			nCount = 0;
		else if (nCount > pData->nDataLength)
			nCount = pData->nDataLength;

		SStringW dest;
		AllocCopy(dest, nCount, 0, 0);
		return dest;
	}

	SOUI::SStringW SStringW::Right(int nCount) const
	{
		TStringData* pData = GetData();
		if (nCount < 0)
			nCount = 0;
		else if (nCount > pData->nDataLength)
			nCount = pData->nDataLength;

		SStringW dest;
		AllocCopy(dest, nCount, pData->nDataLength - nCount, 0);
		return dest;
	}

	SOUI::SStringW SStringW::Mid(int nFirst, int nCount) const
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

		SStringW dest;
		AllocCopy(dest, nCount, nFirst, 0);
		return dest;
	}

	SOUI::SStringW SStringW::Mid(int nFirst) const
	{
		return Mid(nFirst, GetData()->nDataLength - nFirst);
	}

	int SStringW::CompareNoCase(const wchar_t* psz) const
	{
		return wchar_traits::CompareNoCase(m_pszData, psz);
	}

	int SStringW::Compare(const wchar_t* psz) const
	{
		return wchar_traits::Compare(m_pszData, psz);
	}

	const SStringW& SStringW::Append(const SStringW& src)
	{
		SStringW strCopy(src);
		ConcatInPlace(strCopy.GetData()->nDataLength, strCopy.m_pszData);
		return *this;
	}

	const SStringW& SStringW::Append(const wchar_t * psz)
	{
		SStringW strCopy(psz);
		ConcatInPlace(strCopy.GetData()->nDataLength, strCopy.m_pszData);
		return *this;
	}

	const SStringW& SStringW::Append(wchar_t ch)
	{
		ConcatInPlace(1, &ch);
		return *this;
	}

	const SStringW& SStringW::operator+=(const SStringW& src)
	{
		return Append(src);
	}

	const SStringW& SStringW::operator+=(wchar_t ch)
	{
		return Append(ch);
	}

	const SStringW& SStringW::operator+=(const wchar_t* psz)
	{
		return Append(psz);
	}

	const SStringW& SStringW::operator=(wchar_t ch)
	{
		AssignCopy(1, &ch);
		return *this;
	}

	SStringW& SStringW::operator=(const wchar_t* psz)
	{
		SStringW strCopy(psz);
		AssignCopy(strCopy.GetData()->nDataLength, strCopy.m_pszData);
		return *this;
	}

	SStringW& SStringW::operator=(const SStringW& stringSrc)
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

	const wchar_t * SStringW::c_str() const
	{
		return m_pszData;
	}

	SStringW::operator const wchar_t*() const // as a C string
	{
		return m_pszData;
	}

	void SStringW::SetAt(int nIndex, wchar_t ch)
	{
		SASSERT(nIndex >= 0);
		SASSERT(nIndex < GetData()->nDataLength);

		CopyBeforeWrite();
		m_pszData[nIndex] = ch;
	}

	wchar_t SStringW::operator[](int nIndex) const
	{
		// same as GetAt
		SASSERT(nIndex >= 0);
		SASSERT(nIndex < GetData()->nDataLength);
		return m_pszData[nIndex];
	}

	wchar_t SStringW::GetAt(int nIndex) const
	{
		SASSERT(nIndex >= 0);
		SASSERT(nIndex < GetData()->nDataLength);
		return m_pszData[nIndex];
	}

	void SStringW::Empty() // free up the data
	{
		TStringData* pData = GetData();
		if (pData->nDataLength == 0)
			return;

		if (pData->nRefs >= 0)
			Release();
		else
		{
			wchar_t sz[1] = { 0 };
			*this = sz;
		}

		SASSERT(GetData()->nDataLength == 0);
		SASSERT(GetData()->IsLocked() || GetData()->nAllocLength == 0);
	}

	bool SStringW::IsEmpty() const
	{
		return GetData()->nDataLength == 0;
	}

	int SStringW::GetLength() const
	{
		return GetData()->nDataLength;
	}

	void SStringW::Copy(const IStringW *pSrc)
	{
		AssignCopy(pSrc->GetLength(),pSrc->c_str());
	}

	SStringW::~SStringW()
	{
		//  free any attached data
		TStringData* pData = GetData();
		if (pData != TStringData::InitDataNil())
			pData->Release();
	}

	SStringW::SStringW(const wchar_t* psz)
	{
		Init();
		int nLength = SafeStrlen(psz);
		if (nLength != 0)
		{
			if (AllocBuffer(nLength))
				memcpy(m_pszData, psz, nLength * sizeof(wchar_t));
		}
	}

	SStringW::SStringW(const wchar_t* psz, int nLength)
	{
		Init();
		if (nLength < 0) nLength = SafeStrlen(psz);
		if (nLength != 0)
		{
			if (AllocBuffer(nLength))
				memcpy(m_pszData, psz, nLength * sizeof(wchar_t));
		}
	}

	SStringW::SStringW(wchar_t ch, int nLength /*= 1*/)
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

	SStringW::SStringW(const SStringW& stringSrc)
	{
		SASSERT(stringSrc.GetData()->nRefs != 0);
		if (stringSrc.GetData()->nRefs >= 0)
		{
			SASSERT(stringSrc.GetData() != TStringData::InitDataNil());
			m_pszData = stringSrc.m_pszData;
			GetData()->AddRef();
		}
		else
		{
			Init();
			*this = stringSrc.m_pszData;
		}
	}

	SStringW::SStringW()
	{
		Init();
	}
}