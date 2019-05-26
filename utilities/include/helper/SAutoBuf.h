#pragma once

namespace SOUI{

	class UTILITIES_API SAutoBuf
	{
	public:
		SAutoBuf();

		SAutoBuf(size_t nElements);

		~SAutoBuf();

		operator char *() const;

		char* operator ->() const;

		const char & operator[] (int i) const;

		char & operator[] (int i);

		void Attach(char *pBuf,size_t size);

		//return buffer, must be freed using soui_mem_wrapper::SouiFree
		char *Detach();

		char* Allocate(size_t nElements);

		size_t size();

		void Free();
	private:
		char * m_pBuf;
		size_t m_nSize;
		bool   m_bExternalBuf;
	};

}
