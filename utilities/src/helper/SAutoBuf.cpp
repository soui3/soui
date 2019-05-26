#include <Windows.h>
#include <soui_mem_wrapper.h>
#include <helper/SAutoBuf.h>

namespace SOUI{


	SAutoBuf::SAutoBuf(size_t nElements) : m_pBuf(0)
	{
		Allocate(nElements);
	}

	SAutoBuf::SAutoBuf() : m_pBuf(NULL),m_nSize(0),m_bExternalBuf(false)
	{

	}

	SAutoBuf::~SAutoBuf()
	{
		Free();
	}

	void SAutoBuf::Free()
	{
		if(!m_bExternalBuf)
			soui_mem_wrapper::SouiFree(m_pBuf);
		m_pBuf=NULL;
		m_nSize=0;
		m_bExternalBuf=false;
	}

	size_t SAutoBuf::size()
	{
		return m_nSize;
	}

	char* SAutoBuf::Allocate(size_t nBytes)
	{
		SASSERT(nBytes <= SIZE_MAX-1);
		SASSERT(m_pBuf == NULL);
		m_pBuf = static_cast<char*>(soui_mem_wrapper::SouiMalloc(nBytes+1));
		m_nSize=nBytes;
		memset(m_pBuf,0,nBytes+1);
		m_bExternalBuf=false;
		return m_pBuf;
	}

	void SAutoBuf::Attach(char *pBuf,size_t size)
	{
		Free();
		m_pBuf=pBuf;
		m_nSize=size;
		m_bExternalBuf=true;
	}

	char * SAutoBuf::Detach()
	{
		char *pRet = m_pBuf;
		m_pBuf = NULL;
		m_nSize = 0;
		m_bExternalBuf=false;
		return pRet;
	}

	char & SAutoBuf::operator[](int i)
	{
		SASSERT(m_pBuf != NULL);
		return m_pBuf[i];
	}

	const char & SAutoBuf::operator[](int i) const
	{
		SASSERT(m_pBuf != NULL);
		return m_pBuf[i];
	}

	char* SAutoBuf::operator->() const
	{
		SASSERT(m_pBuf != NULL);
		return m_pBuf;
	}

	SAutoBuf::operator char *() const
	{
		return m_pBuf;
	}

}