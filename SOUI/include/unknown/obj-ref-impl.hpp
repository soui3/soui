//IObjRef的实现类
#pragma  once
#include <sdef.h>

#ifndef SASSERT
#include <assert.h>
#define SASSERT(x) assert(x);
#endif

SNSBEGIN

template<class T>
class TObjRefImpl :  public T
{
public:
	TObjRefImpl():m_cRef(1)
	{
	}

	virtual ~TObjRefImpl(){
	}

	virtual long WINAPI AddRef () override{
		return InterlockedIncrement(&m_cRef);
	}

	virtual long WINAPI Release () override
	{
		long lRet = InterlockedDecrement(&m_cRef);
		if(lRet==0)
		{
			OnFinalRelease();
		}
		return lRet;
	}

	virtual void WINAPI OnFinalRelease () override
    {
        delete this;
    }
protected:
	volatile LONG m_cRef;
};


//SAutoRefPtr provides the basis for all other smart pointers
template <class T>
class SAutoRefPtr
{
public:
	SAutoRefPtr() throw()
	{
		p = NULL;
	}
	SAutoRefPtr(int nNull) throw()
	{
		(void)nNull;
		p = NULL;
	}
	SAutoRefPtr(T* lp) throw()
	{
		p = lp;
		if (p != NULL)
		{
			p->AddRef();
		}
	}

	SAutoRefPtr(const SAutoRefPtr & src) throw()
	{
		p=src.p;
		if(p)
		{
			p->AddRef();
		}
	}

	~SAutoRefPtr() throw()
	{
		if (p)
		{
			p->Release();
		}
	}

	T* operator->() const throw()
	{
		return p;
	}

	operator T*() const throw()
	{
		return p;
	}
	T& operator*() const
	{
		return *p;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&() throw()
	{
	    SASSERT(p==NULL);
		return &p;
	}
	bool operator!() const throw()
	{
		return (p == NULL);
	}
	bool operator<(T* pT) const throw()
	{
		return p < pT;
	}
	bool operator!=(T* pT) const
	{
		return !operator==(pT);
	}
	bool operator==(T* pT) const throw()
	{
		return p == pT;
	}

	T* operator=(T* lp) throw()
	{
		if(*this!=lp)
		{
			if(p)
			{
				p->Release();
			}
			p=lp;
			if(p)
			{
				p->AddRef();
			}
		}
		return *this;
	}

	T* operator=(const SAutoRefPtr<T>& lp) throw()
	{
		if(*this!=lp)
		{
			if(p)
			{
				p->Release();
			}
			p=lp;
			if(p)
			{
				p->AddRef();
			}
		}
		return *this;	
	}

	// Release the interface and set to NULL
	void Release() throw()
	{
		T* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release();
		}
	}

	// Attach to an existing interface (does not AddRef)
	void Attach(T* p2) throw()
	{
		if (p)
		{
			p->Release();
		}
		p = p2;
	}
	// Detach the interface (does not Release)
	T* Detach() throw()
	{
		T* pt = p;
		p = NULL;
		return pt;
	}
	HRESULT CopyTo(T** ppT) throw()
	{
		if (ppT == NULL)
			return E_POINTER;
		*ppT = p;
		if (p)
		{
			p->AddRef();
		}
		return S_OK;
	}

protected:
	T* p;
};



#define CAutoRefPtr SAutoRefPtr	//for compatible

SNSEND