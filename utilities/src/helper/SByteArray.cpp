#include <Windows.h>
#include <soui_mem_wrapper.h>
#include <helper/SByteArray.h>

namespace SOUI{


	SByteArray::SByteArray(size_t nElements) : m_pBuf(0)
	{
		Allocate(nElements);
	}

	SByteArray::SByteArray() : m_pBuf(NULL),m_nSize(0),m_bExternalBuf(false)
	{

	}

	SByteArray::~SByteArray()
	{
		Free();
	}

	void SByteArray::Free()
	{
		if(!m_bExternalBuf)
			soui_mem_wrapper::SouiFree(m_pBuf);
		m_pBuf=NULL;
		m_nSize=0;
		m_bExternalBuf=false;
	}

	size_t SByteArray::size()
	{
		return m_nSize;
	}

	char* SByteArray::Allocate(size_t nBytes)
	{
		SASSERT(nElements <= SIZE_MAX-1);
		SASSERT(m_pBuf == NULL);
		m_pBuf = static_cast<char*>(soui_mem_wrapper::SouiMalloc(nBytes+1));
		m_nSize=nBytes;
		memset(m_pBuf,0,nBytes+1);
		m_bExternalBuf=false;
		return m_pBuf;
	}

	void SByteArray::Attach(char *pBuf,size_t size)
	{
		Free();
		m_pBuf=pBuf;
		m_nSize=size;
		m_bExternalBuf=true;
	}

	char * SByteArray::Detach()
	{
		char *pRet = m_pBuf;
		m_pBuf = NULL;
		m_nSize = 0;
		m_bExternalBuf=false;
		return pRet;
	}

	char & SByteArray::operator[](int i)
	{
		SASSERT(m_pBuf != NULL);
		return m_pBuf[i];
	}

	const char & SByteArray::operator[](int i) const
	{
		SASSERT(m_pBuf != NULL);
		return m_pBuf[i];
	}

	char* SByteArray::operator->() const
	{
		SASSERT(m_pBuf != NULL);
		return m_pBuf;
	}

	SByteArray::operator char *() const
	{
		return m_pBuf;
	}

}