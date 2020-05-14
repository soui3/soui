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
	SXmlAttr(const SXmlAttr& src);
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

public:
	operator bool() const;

	// Check if attribute is empty
	bool empty() const;

	// Get attribute name/value, or "" if attribute is empty
	const wchar_t* name() const;
	const wchar_t* value() const;

	// Get attribute value, or the default value if attribute is empty
	const wchar_t* as_string(const wchar_t* def = L"") const;

	// Get attribute value as a number, or the default value if conversion did not succeed or attribute is empty
	int as_int(int def = 0) const;
	unsigned int as_uint(unsigned int def = 0) const;
	double as_double(double def = 0) const;
	float as_float(float def = 0) const;

	// Get attribute value as bool (returns true if first character is in '1tTyY' set), or the default value if attribute is empty
	bool as_bool(bool def = false) const;

	// Set attribute name/value (returns false if attribute is empty or there is not enough memory)
	bool set_name(const wchar_t* rhs);
	bool set_value(const wchar_t* rhs);

	// Set attribute value with type conversion (numbers are converted to strings, boolean is converted to "true"/"false")
	bool set_value(int rhs);
	bool set_value(unsigned int rhs);
	bool set_value(long rhs);
	bool set_value(unsigned long rhs);
	bool set_value(double rhs);
	bool set_value(double rhs, int precision);
	bool set_value(float rhs);
	bool set_value(float rhs, int precision);
	bool set_value(bool rhs);

public:
	// Get next/previous attribute in the attribute list of the parent node
	SXmlAttr next_attribute() const;
	SXmlAttr previous_attribute() const;
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
	SXmlNode(const SXmlNode& src);
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

public:
	operator bool() const;

	// Check if node is empty.
	bool empty() const;

	// Get node type
	XmlNodeType type() const;

	// Get node name, or "" if node is empty or it has no name
	const wchar_t* name() const;

	// Get node value, or "" if node is empty or it has no value
	// Note: For <node>text</node> node.value() does not return "text"! Use child_value() or text() methods to access text inside nodes.
	const wchar_t* value() const;

	// Get attribute list
	SXmlAttr first_attribute() const;
	SXmlAttr last_attribute() const;

	// Get children list
	SXmlNode first_child() const;
	SXmlNode last_child() const;

	// Get next/previous sibling in the children list of the parent node
	SXmlNode next_sibling() const;
	SXmlNode previous_sibling() const;

	// Get parent node
	SXmlNode parent() const;

	// Get root of DOM tree this node belongs to
	SXmlNode root() const;

	// Get child, attribute or next/previous sibling with the specified name
	SXmlNode child(const wchar_t* name,bool bCaseSensitive=false) const;
	SXmlAttr attribute(const wchar_t* name,bool bCaseSensitive=false) const;
	SXmlNode next_sibling(const wchar_t* name,bool bCaseSensitive=false) const;
	SXmlNode previous_sibling(const wchar_t* name,bool bCaseSensitive=false) const;

	// Get attribute, starting the search from a hint (and updating hint so that searching for a sequence of attributes is fast)
	SXmlAttr attribute(const wchar_t* name, SXmlAttr& hint,bool bCaseSensitive=false) const;

	// Get child value of current node; that is, value of the first child node of type PCDATA/CDATA
	const wchar_t* child_value() const;

	// Get child value of child with specified name. Equivalent to child(name).child_value().
	const wchar_t* child_value(const wchar_t* name,bool bCaseSensitive=false) const;

	// Set node name/value (returns false if node is empty, there is not enough memory, or node can not have name/value)
	bool set_name(const wchar_t* rhs);
	bool set_value(const wchar_t* rhs);

	// Add attribute with specified name. Returns added attribute, or empty attribute on errors.
	SXmlAttr append_attribute(const wchar_t* name);
	SXmlAttr prepend_attribute(const wchar_t* name);
	SXmlAttr insert_attribute_after(const wchar_t* name, const SXmlAttr& attr);
	SXmlAttr insert_attribute_before(const wchar_t* name, const SXmlAttr& attr);

	// Add a copy of the specified attribute. Returns added attribute, or empty attribute on errors.
	SXmlAttr append_copy(const SXmlAttr& proto);
	SXmlAttr prepend_copy(const SXmlAttr& proto);
	SXmlAttr insert_copy_after(const SXmlAttr& proto, const SXmlAttr& attr);
	SXmlAttr insert_copy_before(const SXmlAttr& proto, const SXmlAttr& attr);

	// Add child node with specified type. Returns added node, or empty node on errors.
	SXmlNode append_child(XmlNodeType type = node_element);
	SXmlNode prepend_child(XmlNodeType type = node_element);
	SXmlNode insert_child_after(XmlNodeType type, const SXmlNode& node);
	SXmlNode insert_child_before(XmlNodeType type, const SXmlNode& node);

	// Add child element with specified name. Returns added node, or empty node on errors.
	SXmlNode append_child(const wchar_t* name);
	SXmlNode prepend_child(const wchar_t* name);
	SXmlNode insert_child_after(const wchar_t* name, const SXmlNode& node);
	SXmlNode insert_child_before(const wchar_t* name, const SXmlNode& node);

	// Add a copy of the specified node as a child. Returns added node, or empty node on errors.
	SXmlNode append_copy(const SXmlNode& proto);
	SXmlNode prepend_copy(const SXmlNode& proto);
	SXmlNode insert_copy_after(const SXmlNode& proto, const SXmlNode& node);
	SXmlNode insert_copy_before(const SXmlNode& proto, const SXmlNode& node);

	// Move the specified node to become a child of this node. Returns moved node, or empty node on errors.
	SXmlNode append_move(const SXmlNode& moved);
	SXmlNode prepend_move(const SXmlNode& moved);
	SXmlNode insert_move_after(const SXmlNode& moved, const SXmlNode& node);
	SXmlNode insert_move_before(const SXmlNode& moved, const SXmlNode& node);

	// Remove specified attribute
	bool remove_attribute(const SXmlAttr& a);
	bool remove_attribute(const wchar_t* name);

	// Remove all attributes
	bool remove_attributes();

	// Remove specified child
	bool remove_child(const SXmlNode& n);
	bool remove_child(const wchar_t* name);

	// Remove all children
	bool remove_children();
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
