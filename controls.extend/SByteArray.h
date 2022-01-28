#pragma once
#include <vector>
#include <string>

namespace SOUI
{
#ifndef BYTE
	typedef unsigned char BYTE;
#endif

	class SByteArray
	{
	public:
		SByteArray();
		SByteArray(const SByteArray& C);
		SByteArray(BYTE* psrc, int len);
		SByteArray(const std::vector<BYTE>& C);

		~SByteArray();

		void Resize(int size);
		void CopyTo(int index, void* psrc, int len);
		BYTE* GetData(int index = 0);
		void Clear();

		SByteArray& Insert(int pos, const SByteArray& C);
		SByteArray& Insert(int pos, BYTE* psrc, int len);
		SByteArray& Insert(int pos, BYTE ch);

		SByteArray& Append(const SByteArray& C);
		SByteArray& Append(BYTE* psrc, int len);
		SByteArray& Append(BYTE ch);
		int Size() const;

		SByteArray& Reverse();

		SByteArray& Remove(int startPos, int len);
		SByteArray   Mid(int pos, int len = -1);
		SByteArray& Zero();
		void         SetAt(int index, BYTE ch);
		BYTE         GetAt(int index);
		BYTE& operator[](int pos);
		SByteArray& operator =(const SByteArray& C);
		SByteArray& operator +=(BYTE ch);
		bool         operator ==(const SByteArray& C);
		void         FromHexString(std::string hexstr);
		SByteArray   ToHex(char chSpan = 0) const;
		std::string  ToString();

		std::vector<BYTE>::iterator begin() { return m_data.begin(); }
		std::vector<BYTE>::iterator end() { return m_data.end(); }
		std::vector<BYTE>::const_iterator cbegin() const { return m_data.cbegin(); }
		std::vector<BYTE>::const_iterator cend() const { return m_data.cend(); }

	protected:
		std::vector<BYTE> m_data;
	};

}

