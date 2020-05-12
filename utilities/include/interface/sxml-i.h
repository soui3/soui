#pragma once

#include <interface/obj-ref-i.h>

SNSBEGIN

typedef wchar_t char_t;

#undef INTERFACE
#define INTERFACE IXmlAttr
DECLARE_INTERFACE_(IXmlAttr,IObjRef)
{
	//!添加引用
	/*!
	*/
	STDMETHOD_(long,AddRef) (THIS) PURE;

	//!释放引用
	/*!
	*/
	STDMETHOD_(long,Release) (THIS) PURE;

	//!释放对象
	/*!
	*/
	STDMETHOD_(void,OnFinalRelease) (THIS) PURE;

	STDMETHOD_(void,Assign)(THIS_ const IXmlAttr * pAttr) PURE;
	STDMETHOD_(LPVOID,Data)(THIS) SCONST PURE;

	// Check if attribute is empty
	STDMETHOD_(bool,Empty)(THIS) SCONST PURE;

	// Get attribute name/value, or "" if attribute is empty
	STDMETHOD_(const char_t*,Name)(THIS) SCONST PURE;
	STDMETHOD_(const char_t*,Value)(THIS) SCONST PURE;

	STDMETHOD_(bool,set_userdata)(THIS_ int data) PURE;
	STDMETHOD_(int,get_userdata)(THIS) SCONST PURE;

	// Get next/previous attribute in the attribute list of the parent node
	STDMETHOD_(bool,Next)(THIS) PURE;
	STDMETHOD_(bool, Prev)(THIS) PURE;
};

#undef INTERFACE
#define INTERFACE IXmlNode
DECLARE_INTERFACE_(IXmlNode,IObjRef)
{
	//!添加引用
	/*!
	*/
	STDMETHOD_(long,AddRef) (THIS) PURE;

	//!释放引用
	/*!
	*/
	STDMETHOD_(long,Release) (THIS) PURE;

	//!释放对象
	/*!
	*/
	STDMETHOD_(void,OnFinalRelease) (THIS) PURE;

	STDMETHOD_(void,Assign)(THIS_ const IXmlNode * pNode) PURE;
	STDMETHOD_(LPVOID,Data)(THIS) SCONST PURE;

	STDMETHOD_(bool,Empty)(THIS) SCONST PURE;

	STDMETHOD_(const char_t*,Name)(THIS) SCONST PURE;

	STDMETHOD_(const char_t*,Value)(THIS) SCONST PURE;

	STDMETHOD_(bool,set_userdata)(THIS_ int data) PURE;
	STDMETHOD_(int,get_userdata)(THIS) SCONST PURE;

	// Get attribute list
	STDMETHOD_(bool,Attribute)(THIS_ const char_t* name,bool bCaseSensitive,IXmlAttr* pAttr) SCONST PURE;
	STDMETHOD_(bool,FirstAttribute)(THIS_ IXmlAttr *pAttr) SCONST PURE;
	STDMETHOD_(bool,LastAttribute)(THIS_ IXmlAttr *pAttr) SCONST PURE;

	// Get children list
	STDMETHOD_(bool,Child)(THIS_ const char_t* name,bool bCaseSensitive,IXmlNode* pChild) SCONST PURE;
	STDMETHOD_(bool, FirstChild)(THIS_ IXmlNode* pChild) SCONST PURE;
	STDMETHOD_(bool, LastChild)(THIS_ IXmlNode* pChild) SCONST PURE;

	// Get next/previous sibling in the children list of the parent node
	STDMETHOD_(bool, NextSibling)(THIS_ IXmlNode* pSib) SCONST PURE;
	STDMETHOD_(bool, PrevSibling)(THIS_ IXmlNode* pSib) SCONST PURE;
	STDMETHOD_(bool, NextSibling2)(THIS_ const char_t* name,bool bCaseSensitive, IXmlNode* pSib) SCONST PURE;
	STDMETHOD_(bool, PrevSibling2)(THIS_ const char_t* name,bool bCaseSensitive, IXmlNode* pSib) SCONST PURE;
};

#undef INTERFACE
#define INTERFACE IXmlDoc
DECLARE_INTERFACE(IXmlDoc)
{

};

SNSEND