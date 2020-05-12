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
	STDMETHOD_(IXmlAttr*,Next)(THIS) PURE;
	STDMETHOD_(IXmlAttr*,Prev)(THIS) PURE;
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
	STDMETHOD_(IXmlAttr*,Attribute)(THIS_ const char_t* name,bool bCaseSensitive) SCONST PURE;
	STDMETHOD_(IXmlAttr*,FirstAttribute)(THIS) SCONST PURE;
	STDMETHOD_(IXmlAttr*,LastAttribute)(THIS) SCONST PURE;

	// Get children list
	STDMETHOD_(IXmlNode*,Child)(THIS_ const char_t* name,bool bCaseSensitive) SCONST PURE;
	STDMETHOD_(IXmlNode*, FirstChild)(THIS) SCONST PURE;
	STDMETHOD_(IXmlNode*, LastChild)(THIS) SCONST PURE;

	// Get next/previous sibling in the children list of the parent node
	STDMETHOD_(IXmlNode*, NextSibling)(THIS) SCONST PURE;
	STDMETHOD_(IXmlNode*, PrevSibling)(THIS) SCONST PURE;
	STDMETHOD_(IXmlNode*, NextSibling2)(THIS_ const char_t* name,bool bCaseSensitive) SCONST PURE;
	STDMETHOD_(IXmlNode*, PrevSibling2)(THIS_ const char_t* name,bool bCaseSensitive) SCONST PURE;
};

#undef INTERFACE
#define INTERFACE IXmlDoc
DECLARE_INTERFACE(IXmlDoc)
{

};

SNSEND