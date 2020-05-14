#pragma once

#include <interface/obj-ref-i.h>
#include <interface/sstring-i.h>
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

typedef enum _XmlNodeType
{
	node_null,			// Empty (null) node handle
	node_document,		// A document tree's absolute root
	node_element,		// Element tag, i.e. '<node/>'
	node_pcdata,		// Plain character data, i.e. 'text'
	node_cdata,			// Character data, i.e. '<![CDATA[text]]>'
	node_comment,		// Comment tag, i.e. '<!-- text -->'
	node_pi,			// Processing instruction, i.e. '<?name?>'
	node_declaration,	// Document declaration, i.e. '<?xml version="1.0"?>'
	node_doctype		// Document type declaration, i.e. '<!DOCTYPE doc>'
}XmlNodeType;

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

	STDMETHOD_(IStringW*,ToString)(THIS) SCONST PURE;

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


typedef enum _XmlParseOpt
{
	// Minimal parsing mode (equivalent to turning all other flags off).
	// Only elements and PCDATA sections are added to the DOM tree, no text conversions are performed.
	xml_parse_minimal = 0x0000,

	// This flag determines if processing instructions (node_pi) are added to the DOM tree. This flag is off by default.
	xml_parse_pi = 0x0001,

	// This flag determines if comments (node_comment) are added to the DOM tree. This flag is off by default.
	xml_parse_comments = 0x0002,

	// This flag determines if CDATA sections (node_cdata) are added to the DOM tree. This flag is on by default.
	xml_parse_cdata = 0x0004,

	// This flag determines if plain character data (node_pcdata) that consist only of whitespace are added to the DOM tree.
	// This flag is off by default; turning it on usually results in slower parsing and more memory consumption.
	xml_parse_ws_pcdata = 0x0008,

	// This flag determines if character and entity references are expanded during parsing. This flag is on by default.
	xml_parse_escapes = 0x0010,

	// This flag determines if EOL characters are normalized (converted to #xA) during parsing. This flag is on by default.
	xml_parse_eol = 0x0020,

	// This flag determines if attribute values are normalized using CDATA normalization rules during parsing. This flag is on by default.
	xml_parse_wconv_attribute = 0x0040,

	// This flag determines if attribute values are normalized using NMTOKENS normalization rules during parsing. This flag is off by default.
	xml_parse_wnorm_attribute = 0x0080,

	// This flag determines if document declaration (node_declaration) is added to the DOM tree. This flag is off by default.
	xml_parse_declaration = 0x0100,

	// This flag determines if document type declaration (node_doctype) is added to the DOM tree. This flag is off by default.
	xml_parse_doctype = 0x0200,

	// This flag determines if plain character data (node_pcdata) that is the only child of the parent node and that consists only
	// of whitespace is added to the DOM tree.
	// This flag is off by default; turning it on may result in slower parsing and more memory consumption.
	xml_parse_ws_pcdata_single = 0x0400,

	// This flag determines if leading and trailing whitespace is to be removed from plain character data. This flag is off by default.
	xml_parse_trim_pcdata = 0x0800,

	// This flag determines if plain character data that does not have a parent node is added to the DOM tree, and if an empty document
	// is a valid document. This flag is off by default.
	xml_parse_fragment = 0x1000,

	// This flag determines if plain character data is be stored in the parent element's value. This significantly changes the structure of
	// the document; this flag is only recommended for parsing documents with many PCDATA nodes in memory-constrained environments.
	// This flag is off by default.
	xml_parse_embed_pcdata = 0x2000,

	// The default parsing mode.
	// Elements, PCDATA and CDATA sections are added to the DOM tree, character/reference entities are expanded,
	// End-of-Line characters are normalized, attribute values are normalized using CDATA normalization rules.
	xml_parse_default = xml_parse_cdata | xml_parse_escapes | xml_parse_wconv_attribute | xml_parse_eol,

	// The full parsing mode.
	// Nodes of all types are added to the DOM tree, character/reference entities are expanded,
	// End-of-Line characters are normalized, attribute values are normalized using CDATA normalization rules.
	xml_parse_full = xml_parse_default | xml_parse_pi | xml_parse_comments | xml_parse_declaration | xml_parse_doctype,
}XmlParseOpt;

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

