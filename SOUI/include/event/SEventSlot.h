#pragma once

#include <interface/SEvtArgs-i.h>
#include <helper/obj-ref-impl.hpp>

#if _MSC_VER >= 1700	//VS2012
#include <functional>
#endif

SNSBEGIN

enum _SLOTTYPE{SLOT_FUN,SLOT_STDFUNCTOR,SLOT_MEMBER,SLOT_USER};


/*!
\brief
    Slot functor class that calls back via a free function pointer.
*/
typedef BOOL (*FunCallback)(IEvtArgs *,void *Ctx);
class FreeFunctionSlot : public TObjRefImpl<IEvtSlot>
{
public:
	//! Slot function type.

	FreeFunctionSlot(FunCallback func,void * ctx) : d_function(func),d_ctx(ctx)
	{}

	STDMETHOD_(BOOL,Run)(THIS_ IEvtArgs *pArg) OVERRIDE
	{
		return d_function(pArg,d_ctx);
	}
	STDMETHOD_(IEvtSlot*, Clone)(THIS) SCONST OVERRIDE
	{
		return new FreeFunctionSlot(d_function,d_ctx);
	}
	STDMETHOD_(BOOL,Equal)(THIS_ const IEvtSlot * sour) SCONST OVERRIDE
	{
		if(sour->GetSlotType()!=SLOT_FUN) return FALSE;
		const FreeFunctionSlot *psour=static_cast<const FreeFunctionSlot*>(sour);
		SASSERT(psour);
		return psour->d_function==d_function && psour->d_ctx == d_ctx;

	}
	STDMETHOD_(UINT,GetSlotType)(THIS) SCONST OVERRIDE
	{
		return SLOT_FUN;
	}


private:
	FunCallback d_function;
	void *		d_ctx;
};

#if _MSC_VER >= 1700	//VS2012
typedef std::function<BOOL (IEvtArgs*)> StdFunCallback;
class StdFunctionSlot : public TObjRefImpl<IEvtSlot>
{
public:
	//! Slot function type.

	StdFunctionSlot(const StdFunCallback & fun) :d_function(fun)
	{}

	STDMETHOD_(BOOL,Run)(THIS_ IEvtArgs *pArg) OVERRIDE
	{
		return d_function(pArg);
	}
	STDMETHOD_(IEvtSlot*, Clone)(THIS) SCONST OVERRIDE
	{
		return new StdFunctionSlot(d_function);
	}
	STDMETHOD_(BOOL,Equal)(THIS_ const IEvtSlot * sour) SCONST OVERRIDE
	{
		SASSERT(psour);
		if(sour->GetSlotType()!=SLOT_STDFUNCTOR) return FALSE;
		const StdFunctionSlot *psour=static_cast<const StdFunctionSlot*>(sour);
		return psour->d_function==d_function;

	}
	STDMETHOD_(UINT,GetSlotType)(THIS) SCONST OVERRIDE
	{
		return SLOT_STDFUNCTOR;
	}

private:
	StdFunCallback d_function;
};
#endif

/*!
\brief
    Slot template class that creates a functor that calls back via a class
    member function.
*/

template<typename T, typename A>
class MemberFunctionSlot : public TObjRefImpl<IEvtSlot>
{
public:
	//! Member function slot type.
	typedef BOOL(T::*MemberFunctionType)(A *);

	MemberFunctionSlot(MemberFunctionType func, T* obj)
	:d_function(func)
	,d_object(obj)
	{}

	STDMETHOD_(BOOL,Run)(THIS_ IEvtArgs *pArg) OVERRIDE
	{
		return (d_object->*d_function)(static_cast<A*>(pArg));
	}
	STDMETHOD_(IEvtSlot*, Clone)(THIS) SCONST OVERRIDE
	{
		return new MemberFunctionSlot(d_function,d_object);
	}
	STDMETHOD_(BOOL,Equal)(THIS_ const IEvtSlot * sour) SCONST OVERRIDE
	{
		SASSERT(sour);
		if(sour->GetSlotType()!= GetSlotType()) return FALSE;
		const MemberFunctionSlot *psour=static_cast<const MemberFunctionSlot*>(sour);
		SASSERT(psour);
		return psour->d_function==d_function && psour->d_object==d_object;

	}
	STDMETHOD_(UINT,GetSlotType)(THIS) SCONST OVERRIDE
	{
		return SLOT_MEMBER;
	}


private:
	MemberFunctionType d_function;
	T* d_object;
};

template<typename T, typename A = IEvtArgs>
class MemberFunctionSlot;


template <class T, class A>
MemberFunctionSlot<T,A> Subscriber(BOOL (T::* pFn)(A *), T* pObject)
{
    return MemberFunctionSlot<T,A>(pFn, pObject);
}

inline FreeFunctionSlot Subscriber(FunCallback  pFn,void * ctx)
{
    return FreeFunctionSlot(pFn,ctx); 
}

SNSEND