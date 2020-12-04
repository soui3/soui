#pragma once

#include <interface/SEvtArgs-i.h>

#ifdef __cplusplus

#include <core/sobjType.h>
#include <sobject/Sobject.hpp>
#include <helper/obj-ref-impl.hpp>

SNSBEGIN

class SEvtArgs : public TObjRefImpl< SObjectImpl<IEvtArgs> >
{
public:
	UINT handled; 
	BOOL bubbleUp; 
	int  idFrom; 
	LPCWSTR nameFrom;
	SAutoRefPtr<IObject> sender; 

	STDMETHOD_(IObject*,Sender)(THIS){return sender;}
	STDMETHOD_(int,IdFrom) (THIS) SCONST{ return idFrom;}
	STDMETHOD_(void,SetIdFrom)(THIS_ int id) {idFrom = id;}
	STDMETHOD_(LPCWSTR,NameFrom) (THIS) SCONST{return nameFrom;}
	STDMETHOD_(void,SetNameFrom) (THIS_ LPCWSTR name) {nameFrom = name;}
	STDMETHOD_(BOOL,IsBubbleUp) (THIS) SCONST{return bubbleUp;}
	STDMETHOD_(void,SetBubbleUp) (THIS_ BOOL bSet) {bubbleUp = bSet;}
	STDMETHOD_(UINT,HandleCount)(THIS) SCONST {return handled;}
	STDMETHOD_(void,IncreaseHandleCount)(THIS) {handled++;}
	STDMETHOD_(LPCVOID,Data)(THIS) {return NULL;}

public:
	SEvtArgs(IObject *pSender)
		: handled(0)
		, sender(pSender)
		, bubbleUp(true)
	{
		if(NULL!=pSender) {
			idFrom = pSender->GetID();
			nameFrom = pSender->GetName();
		} else {
			idFrom = 0;
			nameFrom = NULL;
		}
	}
};

SNSEND

#endif