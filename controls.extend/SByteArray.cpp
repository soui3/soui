#include "stdafx.h"
#include "SByteArray.h"
#include <ctype.h>
#include <algorithm>

namespace SOUI
{
	SByteArray::SByteArray()
	{
	}

	SByteArray::SByteArray(const SByteArray& C)
	{
		m_data = C.m_data;
	}

	SByteArray::SByteArray(BYTE* psrc, int len)
	{
		CopyTo(0, psrc, len);
	}

	SByteArray::SByteArray(const std::vector<BYTE>& C)
	{
		m_data = C;
	}

	SByteArray::~SByteArray()
	{
	}

	void SByteArray::Resize(int size)
	{
		int oldsize = m_data.size();
		m_data.resize(size);
		if (size > oldsize)
			memset(&m_data[oldsize], 0, size - oldsize);
	}

	void SByteArray::CopyTo(int index, const void* psrc, int len)
	{
		if (len <= 0)
			return;

		if ((index + len) > m_data.size())
			m_data.resize(index + len);

		memcpy(&m_data[index], psrc, len);
	}

	BYTE* SByteArray::GetData(int index)
	{
		if (index >= m_data.size())
			return NULL;
		return &m_data[index];
	}

	void SByteArray::Clear()
	{
		m_data.clear();
	}

	SByteArray& SByteArray::Insert(int pos, const SByteArray& C)
	{
		m_data.insert(m_data.begin() + pos, C.cbegin(), C.cend());
		return *this;
	}

	SByteArray& SByteArray::Insert(int pos, BYTE* psrc, int len)
	{
		m_data.insert(m_data.begin() + pos, psrc, psrc + len);
		return *this;
	}

	SByteArray& SByteArray::Insert(int pos, BYTE ch)
	{
		m_data.insert(m_data.begin() + pos, ch);
		return *this;
	}

	SByteArray& SByteArray::Append(const SByteArray& C)
	{
		m_data.insert(m_data.end(), C.cbegin(), C.cend());
		return *this;
	}

	SByteArray& SByteArray::Append(const BYTE* psrc, int len)
	{
		if (len > 0)
			CopyTo(m_data.size(), psrc, len);
		return *this;
	}

	SByteArray& SByteArray::Append(BYTE ch)
	{
		m_data.push_back(ch);
		return *this;
	}

	int SByteArray::Size() const
	{
		return m_data.size();
	}

	SByteArray& SByteArray::Reverse()
	{
		std::reverse(begin(), end());
		return *this;
	}

	SByteArray& SByteArray::Remove(int startPos, int len)
	{
		int eraselen = len;
		if ((startPos + eraselen) > m_data.size())
			eraselen = m_data.size() - startPos;

		if (startPos < m_data.size())
		{
			if (len < 0)
				m_data.erase(m_data.begin() + startPos, m_data.end());
			else
				m_data.erase(m_data.begin() + startPos, m_data.begin() + startPos + eraselen);
		}

		return *this;
	}

	SByteArray SByteArray::Mid(int pos, int len)
	{
		SByteArray retbin;
		int copylen = len;
		if (len == -1 || (pos + copylen) > m_data.size())
			copylen = m_data.size() - pos;

		if (pos < m_data.size())
		{
			retbin.Append(&m_data[pos], copylen);
		}

		return retbin;
	}

	SByteArray& SByteArray::Zero()
	{
		if (m_data.size() > 0)
			memset(GetData(), 0, m_data.size());
		return *this;
	}

	void SByteArray::SetAt(int index, BYTE ch)
	{
		if (m_data.size() <= index)
			m_data.resize(index + 1);

		m_data[index] = ch;
	}

	BYTE SByteArray::GetAt(int index)
	{
		if (index < m_data.size())
			return m_data[index];

		return 0;
	}

	BYTE& SByteArray::operator[] (int pos)
	{
		if (m_data.size() <= pos)
			m_data.resize(pos + 1);

		return m_data[pos];
	}

	SByteArray& SByteArray::operator=(const SByteArray& C)
	{
		m_data.assign(C.cbegin(), C.cend());
		return *this;
	}

	SByteArray& SByteArray::operator+=(BYTE ch)
	{
		m_data.push_back(ch);
		return *this;
	}

	bool SByteArray::operator==(const SByteArray& C)
	{
		if (Size() != C.Size())
			return false;

		for (int i = 0; i < Size(); i++)
		{
			if (m_data[i] != C.m_data[i])
				return false;
		}

		return true;
	}

	static bool isprintex(char ch)
	{
		if (ch >= 0x20 && ch <= 0x7E)
			return true;
		if (ch == 9 || ch == 0xA || ch == 0xD)
			return true;

		return false;
	}

	static bool isHexNumber(char ch)
	{
		if (('0' > ch || '9' < ch)
			&& ('A' > ch || 'F' < ch)
			&& ('a' > ch || 'f' < ch))
		{
			return false;
		}
		return true;
	}

	void SByteArray::FromHexString(std::string hexstr)
	{
		m_data.clear();
		int len = hexstr.length();
		BYTE bytmp = 0;
		int byteCount = 0;
		for (int i = 0; i < len; i++)
		{
			BYTE by1 = hexstr.at(i);
			if (!isHexNumber(by1))
				continue;

			if ((by1 - '0') > 9)
			{
				if ((by1 - 'a') > 5)
				{
					if ((by1 - 'A') >= 5)
						bytmp += (by1 + 0xC9) & 0x0F;
				}
				else
					bytmp += (by1 + 0xA9) & 0x0F;
			}
			else
				bytmp += (by1 + 0xD0) & 0x0F;

			byteCount++;
			if (byteCount == 2)
			{
				m_data.push_back(bytmp);
				bytmp = 0;
				byteCount = 0;
			}
			else
				bytmp *= 0x10;
		}
	}

	SByteArray SByteArray::ToHex(char chSpan) const
	{
		char hexChar[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
		SByteArray retstr;

		int length = m_data.size();
		for (int i = 0; i < length; i++)
		{
			if (chSpan && retstr.Size())
				retstr += chSpan;
			retstr.Append(hexChar[m_data[i] >> 4]);
			retstr.Append(hexChar[m_data[i] & 0xF]);
		}
		return retstr;
	}

	std::string SByteArray::ToString()
	{
		return std::string((const char*)&m_data[0], m_data.size());
	}

}