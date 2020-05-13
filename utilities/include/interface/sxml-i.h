#pragma once

#include <interface/obj-ref-i.h>
#include <stdio.h>

SNSBEGIN

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

	STDMETHOD_(LPVOID,GetPrivPtr)(THIS) SCONST PURE;

	// Check if attribute is empty
	STDMETHOD_(bool,Empty)(THIS) SCONST PURE;

	// Get attribute name/value, or "" if attribute is empty
	STDMETHOD_(const wchar_t*,Name)(THIS) SCONST PURE;
	STDMETHOD_(const wchar_t*,Value)(THIS) SCONST PURE;

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

	STDMETHOD_(LPVOID,GetPrivPtr)(THIS) SCONST PURE;

	STDMETHOD_(bool,Empty)(THIS) SCONST PURE;

	STDMETHOD_(const wchar_t*,Name)(THIS) SCONST PURE;

	STDMETHOD_(const wchar_t*,Value)(THIS) SCONST PURE;

	STDMETHOD_(bool,set_userdata)(THIS_ int data) PURE;
	STDMETHOD_(int,get_userdata)(THIS) SCONST PURE;

	// Get attribute list
	STDMETHOD_(IXmlAttr*,Attribute)(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST PURE;
	STDMETHOD_(IXmlAttr*,FirstAttribute)(THIS) SCONST PURE;
	STDMETHOD_(IXmlAttr*,LastAttribute)(THIS) SCONST PURE;

	// Get children list
	STDMETHOD_(IXmlNode*,Child)(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST PURE;
	STDMETHOD_(IXmlNode*, FirstChild)(THIS) SCONST PURE;
	STDMETHOD_(IXmlNode*, LastChild)(THIS) SCONST PURE;

	// Get next/previous sibling in the children list of the parent node
	STDMETHOD_(IXmlNode*, NextSibling)(THIS) SCONST PURE;
	STDMETHOD_(IXmlNode*, PrevSibling)(THIS) SCONST PURE;
	STDMETHOD_(IXmlNode*, NextSibling2)(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST PURE;
	STDMETHOD_(IXmlNode*, PrevSibling2)(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST PURE;
};

typedef enum _XmlStatus
{
	xml_ok = 0,				// No error

	xml_file_not_found,		// File was not found during load_file()
	xml_io_error,			// Error reading from file/stream
	xml_out_of_memory,		// Could not allocate memory
	xml_internal_error,		// Internal error occurred

	xml_unrecognized_tag,	// Parser could not determine tag type

	xml_bad_pi,				// Parsing error occurred while parsing document declaration/processing instruction
	xml_bad_comment,			// Parsing error occurred while parsing comment
	xml_bad_cdata,			// Parsing error occurred while parsing CDATA section
	xml_bad_doctype,			// Parsing error occurred while parsing document type declaration
	xml_bad_pcdata,			// Parsing error occurred while parsing PCDATA section
	xml_bad_start_element,	// Parsing error occurred while parsing start element tag
	xml_bad_attribute,		// Parsing error occurred while parsing element attribute
	xml_bad_end_element,		// Parsing error occurred while parsing end element tag
	xml_end_element_mismatch,// There was a mismatch of start-end tags (closing tag had incorrect name, some tag was not closed or there was an excessive closing tag)

	xml_append_invalid_root,	// Unable to append nodes since root type is not node_element or node_document (exclusive to xml_node::append_buffer)

	xml_no_document_element	// Parsing resulted in a document without element nodes
}XmlStatus;

typedef enum _XmlEncoding
{
	enc_auto,		// Auto-detect input encoding using BOM or < / <? detection; use UTF8 if BOM is not found
	enc_utf8,		// UTF8 encoding
	enc_utf16_le,	// Little-endian UTF16
	enc_utf16_be,	// Big-endian UTF16
	enc_utf16,		// UTF16 with native endianness
	enc_utf32_le,	// Little-endian UTF32
	enc_utf32_be,	// Big-endian UTF32
	enc_utf32,		// UTF32 with native endianness
	enc_wchar,		// The same encoding wchar_t has (either UTF16 or UTF32)
	enc_latin1,
	enc_bin,		// bin xml
}XmlEncoding;

#undef INTERFACE
#define INTERFACE IXmlDoc
DECLARE_INTERFACE_(IXmlDoc,IObjRef)
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


	STDMETHOD_(LPVOID,GetPrivPtr)(THIS) SCONST PURE;

	STDMETHOD_(void,Reset)(THIS) PURE;

	// Removes all nodes, then copies the entire contents of the specified document
	STDMETHOD_(void,Copy)(THIS_ const IXmlDoc* proto) PURE;

	// Load document from zero-terminated string. No encoding conversions are applied.
	STDMETHOD_(XmlStatus,LoadString)(THIS_ const wchar_t* contents, unsigned int options) PURE;

	// Load document from file
	STDMETHOD_(XmlStatus,LoadFileA)(THIS_ const char* path, unsigned int options, XmlEncoding encoding) PURE;
	STDMETHOD_(XmlStatus,LoadFileW)(THIS_ const wchar_t* path, unsigned int options , XmlEncoding encoding) PURE;

	// Load document from buffer. Copies/converts the buffer, so it may be deleted or changed after the function returns.
	STDMETHOD_(XmlStatus,LoadBuffer)(THIS_ const void* contents, size_t size, unsigned int options , XmlEncoding encoding) PURE;

	// Load document from buffer, using the buffer for in-place parsing (the buffer is modified and used for storage of document data).
	// You should ensure that buffer data will persist throughout the document's lifetime, and free the buffer memory manually once document is destroyed.
	STDMETHOD_(XmlStatus,LoadBufferInplace)(THIS_ void* contents, size_t size, unsigned int options , XmlEncoding encoding) PURE;

	// Load document from buffer, using the buffer for in-place parsing (the buffer is modified and used for storage of document data).
	// You should allocate the buffer with pugixml allocation function; document will free the buffer when it is no longer needed (you can't use it anymore).
	STDMETHOD_(XmlStatus,LoadBufferInplaceOwn)(THIS_ void* contents, size_t size, unsigned int options , XmlEncoding encoding ) PURE;

	// Save XML document to writer (semantics is slightly different from xml_node::print, see documentation for details).
	STDMETHOD_(void,SaveBinary)(THIS_ FILE *f) SCONST PURE;

	// Save XML to file
	STDMETHOD_(bool,SaveFileA)(THIS_ const char* path, const wchar_t* indent , unsigned int flags, XmlEncoding encoding) SCONST PURE;
	STDMETHOD_(bool,SaveFileW)(THIS_ const wchar_t* path, const wchar_t* indent , unsigned int flags, XmlEncoding encoding) SCONST PURE;

	// Get document element
	STDMETHOD_(IXmlNode *,Root)(THIS) SCONST PURE;
};

SNSEND

#ifdef __cplusplus
typedef SOUI::IXmlDoc * IXmlDocPtr;
#else
typedef IXmlDoc * IXmlDocPtr;
#endif

