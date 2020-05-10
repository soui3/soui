#pragma once
#include <interface/sobject-i.h>

SNSBEGIN

#undef INTERFACE
#define INTERFACE ICaret
DECLARE_INTERFACE_(ICaret,IObject)
{
	STDMETHOD_(BOOL,Init)(THIS_ HBITMAP hBmp, int nWid, int nHei) PURE;

	STDMETHOD_(void,SetPosition)(THIS_ int x, int y) PURE;

	STDMETHOD_(BOOL,SetVisible)(THIS_ BOOL bVisible,SWND owner) PURE;

	STDMETHOD_(BOOL,IsVisible)(THIS) SCONST PURE;

	STDMETHOD_(void,Draw)(THIS_ IRenderTarget *pRT) PURE;

	STDMETHOD_(RECT,GetRect)(THIS) SCONST PURE;
};

SNSEND