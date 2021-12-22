#pragma once
#include <interface/STaskLoop-i.h>
#include <helper/obj-ref-impl.hpp>
#if _MSC_VER>=1700
#include <functional>
#endif

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

SNSBEGIN

class SRunnable : public TObjRefImpl<IRunnable>
{
public:
	SRunnable(void * pObj = NULL) :_pObj(pObj) {

	}
	virtual ~SRunnable() {

	}
	STDMETHOD_(void *,getObject)(THIS)
	{
		return _pObj;
	}
protected:
	void * _pObj;
};

#define IMPL_GETCLASSINFO \
	public: \
	STDMETHOD_(const char *,getClassInfo)(THIS) SCONST \
	{\
		return __PRETTY_FUNCTION__;\
	}

#if _MSC_VER>=1700

	class StdRunnable : public SRunnable
	{
		IMPL_GETCLASSINFO
	public:
		StdRunnable(const std::function<void(void)> & fun):_func(fun)
		{
		}

		virtual IRunnable * clone() const {
			return new StdRunnable(_func);
		}

		virtual void run()
		{
			_func();
		}
	protected:
		std::function<void(void)> _func;
	};
#endif

template <typename TClass, typename Func>
class SFunctor0 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	SFunctor0(TClass *obj, Func func) :
	SRunnable(obj),
		_func(func)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new SFunctor0((TClass*)_pObj, _func);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(((TClass*)getObject())->*_func)();
	}

private:
	Func _func;
};

template <typename TClass, typename Func, typename T1>
class SFunctor1 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	SFunctor1(TClass *obj, Func func, T1 param1) :
	SRunnable(obj),
		_func(func),
		_param1(param1)
	{
	}
	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new SFunctor1((TClass*)_pObj, _func,_param1);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(((TClass*)getObject())->*_func)(_param1);
	}

private:
	Func _func;
	T1 _param1;
};

template <typename TClass, typename Func, typename T1, typename T2>
class SFunctor2 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	SFunctor2(TClass *obj, Func func, T1 param1, T2 param2) :
	SRunnable(obj),
		_func(func),
		_param1(param1),
		_param2(param2)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new SFunctor2((TClass*)_pObj, _func, _param1, _param2);
	}


	STDMETHOD_(void,run)(THIS)
	{
		(((TClass*)getObject())->*_func)(_param1, _param2);
	}

private:
	Func _func;
	T1 _param1;
	T2 _param2;
};

template <typename TClass, typename Func, typename T1, typename T2, typename T3>
class SFunctor3 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	SFunctor3(TClass *obj, Func func, T1 param1, T2 param2, T3 param3) :
	SRunnable(obj),
		_func(func),
		_param1(param1),
		_param2(param2),
		_param3(param3)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new SFunctor3((TClass*)_pObj, _func, _param1, _param2, _param3);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(((TClass*)getObject())->*_func)(_param1, _param2, _param3);
	}

private:
	Func _func;
	T1 _param1;
	T2 _param2;
	T3 _param3;
};

template <typename TClass, typename Func, typename T1, typename T2, typename T3, typename T4>
class SFunctor4 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	SFunctor4(TClass *obj, Func func, T1 param1, T2 param2, T3 param3, T4 param4) :
	SRunnable(obj),
		_func(func),
		_param1(param1),
		_param2(param2),
		_param3(param3),
		_param4(param4)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new SFunctor4((TClass*)_pObj, _func, _param1, _param2, _param3, _param4);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(((TClass*)getObject())->*_func)(_param1, _param2, _param3, _param4);
	}
private:
	Func _func;
	T1 _param1;
	T2 _param2;
	T3 _param3;
	T4 _param4;
};

template <typename TClass, typename Func, typename T1, typename T2, typename T3, typename T4, typename T5>
class SFunctor5 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	SFunctor5(TClass *obj, Func func, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5) :
	SRunnable(obj),
		_func(func),
		_param1(param1),
		_param2(param2),
		_param3(param3),
		_param4(param4),
		_param5(param5)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new SFunctor5((TClass*)_pObj, _func, _param1, _param2, _param3, _param4, _param5);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(((TClass*)getObject())->*_func)(_param1, _param2, _param3, _param4, _param5);
	}

	private:
		Func _func;
		T1 _param1;
		T2 _param2;
		T3 _param3;
		T4 _param4;
		T5 _param5;
	};

template <typename TClass, typename Func, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
class SFunctor6 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	SFunctor6(TClass *obj, Func func, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6) :
	SRunnable(obj),
		_func(func),
		_param1(param1),
		_param2(param2),
		_param3(param3),
		_param4(param4),
		_param5(param5),
		_param6(param6)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new SFunctor6((TClass*)_pObj, _func,_param1, _param2, _param3, _param4, _param5, _param6);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(((TClass*)getObject())->*_func)(_param1, _param2, _param3, _param4, _param5, _param6);
	}


private:
	Func _func;
	T1 _param1;
	T2 _param2;
	T3 _param3;
	T4 _param4;
	T5 _param5;
	T6 _param6;
};

template <typename TClass, typename Func, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
class SFunctor7 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	SFunctor7(TClass *obj, Func func, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7) :
	SRunnable(obj),
		_func(func),
		_param1(param1),
		_param2(param2),
		_param3(param3),
		_param4(param4),
		_param5(param5),
		_param6(param6),
		_param7(param7)
	{
	}
	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new SFunctor7((TClass*)_pObj, _func, _param1, _param2, _param3, _param4, _param5, _param6, _param7);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(((TClass*)getObject())->*_func)(_param1, _param2, _param3, _param4, _param5, _param6, _param7);
	}

	private:
		Func _func;
		T1 _param1;
		T2 _param2;
		T3 _param3;
		T4 _param4;
		T5 _param5;
		T6 _param6;
		T7 _param7;
	};

template <typename TClass, typename Func, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
class SFunctor8 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	SFunctor8(TClass *obj, Func func, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8) :
	SRunnable(obj),
		_func(func),
		_param1(param1),
		_param2(param2),
		_param3(param3),
		_param4(param4),
		_param5(param5),
		_param6(param6),
		_param7(param7),
		_param8(param8)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new SFunctor8((TClass*)_pObj, _func, _param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8);
	}


	STDMETHOD_(void,run)(THIS)
	{
		(((TClass*)getObject())->*_func)(_param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8);
	}
	private:
		Func _func;
		T1 _param1;
		T2 _param2;
		T3 _param3;
		T4 _param4;
		T5 _param5;
		T6 _param6;
		T7 _param7;
		T8 _param8;
	};


template <typename TClass, typename Func, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
class SFunctor9 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	SFunctor9(TClass *obj, Func func, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8, T9 param9) :
	SRunnable(obj),
		_func(func),
		_param1(param1),
		_param2(param2),
		_param3(param3),
		_param4(param4),
		_param5(param5),
		_param6(param6),
		_param7(param7),
		_param8(param8),
		_param9(param9)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new SFunctor9((TClass*)_pObj, _func, _param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8, _param9);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(((TClass*)getObject())->*_func)(_param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8, _param9);
	}

	private:
		Func _func;
		T1 _param1;
		T2 _param2;
		T3 _param3;
		T4 _param4;
		T5 _param5;
		T6 _param6;
		T7 _param7;
		T8 _param8;
		T9 _param9;
	};


template <typename Func>
class StaticSFunctor0 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	StaticSFunctor0(Func func) :
	_func(func)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new StaticSFunctor0(_func);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(*_func)();
	}
private:
	Func _func;
};

template <typename Func, typename T1>
class StaticSFunctor1 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	StaticSFunctor1(Func func, T1 param1) :
	_func(func),
		_param1(param1)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new StaticSFunctor1(_func,_param1);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(*_func)(_param1);
	}

private:
	Func _func;
	T1 _param1;
};

template <typename Func, typename T1, typename T2>
class StaticSFunctor2 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	StaticSFunctor2(Func func, T1 param1, T2 param2) :
	_func(func),
		_param1(param1),
		_param2(param2)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new StaticSFunctor2(_func, _param1, _param2);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(*_func)(_param1, _param2);
	}
private:
	Func _func;
	T1 _param1;
	T2 _param2;
};

template <typename Func, typename T1, typename T2, typename T3>
class StaticSFunctor3 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	StaticSFunctor3(Func func, T1 param1, T2 param2, T3 param3) :
	_func(func),
		_param1(param1),
		_param2(param2),
		_param3(param3)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new StaticSFunctor3(_func, _param1, _param2, _param3);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(*_func)(_param1, _param2, _param3);
	}
private:
	Func _func;
	T1 _param1;
	T2 _param2;
	T3 _param3;
};

template <typename Func, typename T1, typename T2, typename T3, typename T4>
class StaticSFunctor4 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	StaticSFunctor4(Func func, T1 param1, T2 param2, T3 param3, T4 param4) :
	_func(func),
		_param1(param1),
		_param2(param2),
		_param3(param3),
		_param4(param4)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new StaticSFunctor4(_func, _param1, _param2, _param3,_param4);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(*_func)(_param1, _param2, _param3, _param4);
	}
private:
	Func _func;
	T1 _param1;
	T2 _param2;
	T3 _param3;
	T4 _param4;
};

template <typename Func, typename T1, typename T2, typename T3, typename T4, typename T5>
class StaticSFunctor5 : public SRunnable
{
	IMPL_GETCLASSINFO
public:
	StaticSFunctor5(Func func, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5) :
	_func(func),
		_param1(param1),
		_param2(param2),
		_param3(param3),
		_param4(param4),
		_param5(param5)
	{
	}

	STDMETHOD_(IRunnable *,clone)(THIS) SCONST {
		return new StaticSFunctor5(_func, _param1, _param2, _param3, _param4,_param5);
	}

	STDMETHOD_(void,run)(THIS)
	{
		(*_func)(_param1, _param2, _param3, _param4, _param5);
	}

private:
	Func _func;
	T1 _param1;
	T2 _param2;
	T3 _param3;
	T4 _param4;
	T5 _param5;
};

	class SMessageLoop;
class STaskHelper {
public:
	template<typename TClass,typename Fun>
	static long post(ITaskLoop *pTaskLoop,TClass * pObj, Fun fun,bool waitUntilDone,int nPriority=0)
	{
		SFunctor0<TClass, Fun> runnable(pObj, fun);
		return pTaskLoop->postTask(&runnable, waitUntilDone,nPriority);
	}

	template<typename TClass, typename Fun,typename P1>
	static long post(ITaskLoop *pTaskLoop, TClass * pObj, Fun fun, P1 p1, bool waitUntilDone,int nPriority=0)
	{
		SFunctor1<TClass,Fun,P1> runnable(pObj, fun,p1);
		return pTaskLoop->postTask(&runnable, waitUntilDone,nPriority);
	}

	template<typename TClass, typename Fun, typename P1, typename P2>
	static long post(ITaskLoop *pTaskLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,bool waitUntilDone,int nPriority=0)
	{
		SFunctor2<TClass, Fun, P1,P2> runnable(pObj, fun, p1,p2);
		return pTaskLoop->postTask(&runnable, waitUntilDone,nPriority);
	}

	template<typename TClass, typename Fun, typename P1, typename P2, typename P3>
	static long post(ITaskLoop *pTaskLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,bool waitUntilDone,int nPriority=0)
	{
		SFunctor3<TClass, Fun, P1,P2,P3> runnable(pObj, fun, p1,p2,p3);
		return pTaskLoop->postTask(&runnable, waitUntilDone,nPriority);
	}

	template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4>
	static long post(ITaskLoop *pTaskLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4,bool waitUntilDone,int nPriority=0)
	{
		SFunctor4<TClass, Fun, P1,P2,P3,P4> runnable(pObj, fun, p1,p2,p3,p4);
		return pTaskLoop->postTask(&runnable, waitUntilDone,nPriority);
	}

	template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4, typename P5>
	static long post(ITaskLoop *pTaskLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4, P5 p5,bool waitUntilDone,int nPriority=0)
	{
		SFunctor5<TClass, Fun, P1,P2,P3,P4,P5> runnable(pObj, fun, p1,p2,p3,p4,p5);
		return pTaskLoop->postTask(&runnable, waitUntilDone,nPriority);
	}

	template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
	static long post(ITaskLoop *pTaskLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4, P5 p5,P6 p6,bool waitUntilDone,int nPriority=0)
	{
		SFunctor6<TClass, Fun, P1,P2,P3,P4,P5,P6> runnable(pObj, fun, p1,p2,p3,p4,p5,p6);
		return pTaskLoop->postTask(&runnable, waitUntilDone,nPriority);
	}

	template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
	static long post(ITaskLoop *pTaskLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4, P5 p5,P6 p6,P7 p7,bool waitUntilDone,int nPriority=0)
	{
		SFunctor7<TClass, Fun, P1,P2,P3,P4,P5,P6,P7> runnable(pObj, fun, p1,p2,p3,p4,p5,p6,p7);
		return pTaskLoop->postTask(&runnable, waitUntilDone,nPriority);
	}

	template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
	static long post(ITaskLoop *pTaskLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4, P5 p5,P6 p6,P7 p7,P8 p8,bool waitUntilDone,int nPriority=0)
	{
		SFunctor8<TClass, Fun, P1,P2,P3,P4,P5,P6,P7,P8> runnable(pObj, fun, p1,p2,p3,p4,p5,p6,p7,p8);
		return pTaskLoop->postTask(&runnable, waitUntilDone,nPriority);
	}

	template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
	static long post(ITaskLoop *pTaskLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4, P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,bool waitUntilDone,int nPriority=0)
	{
		SFunctor9<TClass, Fun, P1,P2,P3,P4,P5,P6,P7,P8,P9> runnable(pObj, fun, p1,p2,p3,p4,p5,p6,p7,p8,p9);
		return pTaskLoop->postTask(&runnable, waitUntilDone,nPriority);
	}

		//////////////////////////////////////////////////////////////////////////
		template<typename TClass,typename Fun>
		static void post(SMessageLoop *pMsgLoop,TClass * pObj, Fun fun)
		{
			SFunctor0<TClass, Fun> runnable(pObj, fun);
			pMsgLoop->PostTask(runnable);
		}

		template<typename TClass, typename Fun,typename P1>
		static void post(SMessageLoop *pMsgLoop, TClass * pObj, Fun fun, P1 p1)
		{
			SFunctor1<TClass,Fun,P1> runnable(pObj, fun,p1);
			pMsgLoop->PostTask(runnable);
		}

		template<typename TClass, typename Fun, typename P1, typename P2>
		static void post(SMessageLoop *pMsgLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 )
		{
			SFunctor2<TClass, Fun, P1,P2> runnable(pObj, fun, p1,p2);
			pMsgLoop->PostTask(runnable);
		}

		template<typename TClass, typename Fun, typename P1, typename P2, typename P3>
		static void post(SMessageLoop *pMsgLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3)
		{
			SFunctor3<TClass, Fun, P1,P2,P3> runnable(pObj, fun, p1,p2,p3);
			pMsgLoop->PostTask(runnable);
		}

		template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4>
		static void post(SMessageLoop *pMsgLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4)
		{
			SFunctor4<TClass, Fun, P1,P2,P3,P4> runnable(pObj, fun, p1,p2,p3,p4);
			pMsgLoop->PostTask(runnable);
		}

		template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4, typename P5>
		static void post(SMessageLoop *pMsgLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4, P5 p5)
		{
			SFunctor5<TClass, Fun, P1,P2,P3,P4,P5> runnable(pObj, fun, p1,p2,p3,p4,p5);
			pMsgLoop->PostTask(runnable);
		}

		template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		static void post(SMessageLoop *pMsgLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4, P5 p5,P6 p6)
		{
			SFunctor6<TClass, Fun, P1,P2,P3,P4,P5,P6> runnable(pObj, fun, p1,p2,p3,p4,p5,p6);
			pMsgLoop->PostTask(runnable);
		}

		template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		static void post(SMessageLoop *pMsgLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4, P5 p5,P6 p6,P7 p7)
		{
			SFunctor7<TClass, Fun, P1,P2,P3,P4,P5,P6,P7> runnable(pObj, fun, p1,p2,p3,p4,p5,p6,p7);
			pMsgLoop->PostTask(runnable);
		}

		template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		static void post(SMessageLoop *pMsgLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4, P5 p5,P6 p6,P7 p7,P8 p8)
		{
			SFunctor8<TClass, Fun, P1,P2,P3,P4,P5,P6,P7,P8> runnable(pObj, fun, p1,p2,p3,p4,p5,p6,p7,p8);
			pMsgLoop->PostTask(runnable);
		}

		template<typename TClass, typename Fun, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		static void post(SMessageLoop *pMsgLoop, TClass * pObj, Fun fun, P1 p1, P2 p2 ,P3 p3,P4 p4, P5 p5,P6 p6,P7 p7,P8 p8,P9 p9)
		{
			SFunctor9<TClass, Fun, P1,P2,P3,P4,P5,P6,P7,P8,P9> runnable(pObj, fun, p1,p2,p3,p4,p5,p6,p7,p8,p9);
			pMsgLoop->PostTask(runnable);
		}
	};
}
