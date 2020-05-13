#pragma once
#include <utilities.h>
#include <utilities-def.h>
#include <interface/sxml-i.h>
#include <helper/obj-ref-impl.hpp>
#include <pugixml/pugixml.hpp>

SNSBEGIN

class UTILITIES_API SXmlAttr : public TObjRefImpl<IXmlAttr>
{
	friend class SXmlNode;
private:
	pugi::xml_attribute _attr;

private:
	static IXmlAttr *toIXmlAttr(pugi::xml_attribute attr);
public:
	SXmlAttr(pugi::xml_attribute attr);
	explicit SXmlAttr(LPVOID pData);
public:
	STDMETHOD_(LPVOID,GetPrivPtr)(THIS) SCONST OVERRIDE;

	// Check if attribute is empty
	STDMETHOD_(bool,Empty)(THIS) SCONST OVERRIDE;

	// Get attribute name/value, or "" if attribute is empty
	STDMETHOD_(const wchar_t*,Name)(THIS) SCONST OVERRIDE;
	STDMETHOD_(const wchar_t*,Value)(THIS) SCONST OVERRIDE;

	STDMETHOD_(bool,set_userdata)(THIS_ int data) OVERRIDE;
	STDMETHOD_(int,get_userdata)(THIS) SCONST OVERRIDE;

	// Get next/previous attribute in the attribute list of the parent node
	STDMETHOD_(IXmlAttr*,Next)(THIS) OVERRIDE;
	STDMETHOD_(IXmlAttr*,Prev)(THIS) OVERRIDE;
};

class UTILITIES_API SXmlNode : public TObjRefImpl<IXmlNode>
{
	friend class SXmlDoc;
private:
	pugi::xml_node _node;

private:
	static IXmlNode * toIXmlNode(pugi::xml_node node);
public:
	SXmlNode(pugi::xml_node node);
	explicit SXmlNode(LPVOID pData);
public:
	STDMETHOD_(LPVOID,GetPrivPtr)(THIS) SCONST OVERRIDE;

	STDMETHOD_(bool,Empty)(THIS) SCONST OVERRIDE;

	STDMETHOD_(const wchar_t*,Name)(THIS) SCONST OVERRIDE;

	STDMETHOD_(const wchar_t*,Value)(THIS) SCONST OVERRIDE;

	STDMETHOD_(bool,set_userdata)(THIS_ int data) OVERRIDE;
	STDMETHOD_(int,get_userdata)(THIS) SCONST OVERRIDE;

	// Get attribute list
	STDMETHOD_(IXmlAttr*,Attribute)(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST OVERRIDE;
	STDMETHOD_(IXmlAttr*,FirstAttribute)(THIS) SCONST OVERRIDE;
	STDMETHOD_(IXmlAttr*,LastAttribute)(THIS) SCONST OVERRIDE;

	// Get children list
	STDMETHOD_(IXmlNode*,Child)(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST OVERRIDE;
	STDMETHOD_(IXmlNode*, FirstChild)(THIS) SCONST OVERRIDE;
	STDMETHOD_(IXmlNode*, LastChild)(THIS) SCONST OVERRIDE;

	// Get next/previous sibling in the children list of the parent node
	STDMETHOD_(IXmlNode*, NextSibling)(THIS) SCONST OVERRIDE;
	STDMETHOD_(IXmlNode*, PrevSibling)(THIS) SCONST OVERRIDE;
	STDMETHOD_(IXmlNode*, NextSibling2)(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST OVERRIDE;
	STDMETHOD_(IXmlNode*, PrevSibling2)(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST OVERRIDE;

};


class UTILITIES_API SXmlDoc : public TObjRefImpl<IXmlDoc>
{
public:
	pugi::xml_document *doc;
	SXmlDoc();
	~SXmlDoc();
public:
	STDMETHOD_(LPVOID,GetPrivPtr)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,Reset)(THIS)OVERRIDE;

	// Removes all nodes, then copies the entire contents of the specified document
	STDMETHOD_(void,Copy)(THIS_ const IXmlDoc* proto) OVERRIDE;

	// Load document from zero-terminated string. No encoding conversions are applied.
	STDMETHOD_(XmlStatus,LoadString)(THIS_ const wchar_t* contents, unsigned int options)OVERRIDE;

	// Load document from file
	STDMETHOD_(XmlStatus,LoadFileA)(THIS_ const char* path, unsigned int options, XmlEncoding encoding)OVERRIDE;

	STDMETHOD_(XmlStatus,LoadFileW)(THIS_ const wchar_t* path, unsigned int options , XmlEncoding encoding)OVERRIDE;

	// Load document from buffer. Copies/converts the buffer, so it may be deleted or changed after the function returns.
	STDMETHOD_(XmlStatus,LoadBuffer)(THIS_ const void* contents, size_t size, unsigned int options , XmlEncoding encoding)OVERRIDE;

	// Load document from buffer, using the buffer for in-place parsing (the buffer is modified and used for storage of document data).
	// You should ensure that buffer data will persist throughout the document's lifetime, and free the buffer memory manually once document is destroyed.
	STDMETHOD_(XmlStatus,LoadBufferInplace)(THIS_ void* contents, size_t size, unsigned int options , XmlEncoding encoding)OVERRIDE;

	// Load document from buffer, using the buffer for in-place parsing (the buffer is modified and used for storage of document data).
	// You should allocate the buffer with pugixml allocation function; document will free the buffer when it is no longer needed (you can't use it anymore).
	STDMETHOD_(XmlStatus,LoadBufferInplaceOwn)(THIS_ void* contents, size_t size, unsigned int options , XmlEncoding encoding )OVERRIDE;

	// Save XML document to writer (semantics is slightly different from xml_node::print, see documentation for details).
	STDMETHOD_(void,SaveBinary)(THIS_ FILE *f) SCONST OVERRIDE;

	// Save XML to file
	STDMETHOD_(bool,SaveFileA)(THIS_ const char* path, const wchar_t* indent , unsigned int flags, XmlEncoding encoding) SCONST OVERRIDE;
	STDMETHOD_(bool,SaveFileW)(THIS_ const wchar_t* path, const wchar_t* indent , unsigned int flags, XmlEncoding encoding) SCONST OVERRIDE;

	// Get document element
	STDMETHOD_(IXmlNode *,Root)(THIS) SCONST OVERRIDE;
};


SNSEND
