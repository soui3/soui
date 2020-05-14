#include <xml/SXml.h>
#include <pugixml/pugixml.hpp>
#include <string/sstringw.h>
SNSBEGIN


SXmlAttr::SXmlAttr(LPVOID pData):_attr((pugi::xml_attribute_struct*)pData)
{
}

SXmlAttr::SXmlAttr(const IXmlAttr * src):_attr((pugi::xml_attribute_struct*)src->GetPrivPtr())
{
}

SXmlAttr::SXmlAttr(pugi::xml_attribute attr):_attr(attr)
{

}

SXmlAttr::SXmlAttr(const SXmlAttr& src):_attr(src._attr)
{

}

IXmlAttr * SXmlAttr::toIXmlAttr(pugi::xml_attribute attr)
{
	if(attr)
	{
		return new SXmlAttr(attr.internal_object());
	}else
	{
		return NULL;
	}
}

LPVOID SXmlAttr::GetPrivPtr(THIS) SCONST
{
	return _attr.internal_object();
}

bool SXmlAttr::Empty(THIS) SCONST
{
	return _attr.empty();
}

const wchar_t * SXmlAttr::Name(THIS) SCONST
{
	return _attr.name();
}

const wchar_t * SXmlAttr::Value(THIS) SCONST
{
	return _attr.value();
}

bool SXmlAttr::set_userdata(THIS_ int data)
{
	return _attr.set_userdata(data);
}

int SXmlAttr::get_userdata(THIS) SCONST
{
	return _attr.get_userdata();
}

IXmlAttr * SXmlAttr::Next(THIS)
{
	pugi::xml_attribute attr = _attr.next_attribute();
	return toIXmlAttr(attr);
}

IXmlAttr * SXmlAttr::Prev(THIS)
{
	pugi::xml_attribute attr = _attr.previous_attribute();
	return toIXmlAttr(attr);
}

SXmlAttr::operator bool() const
{
	return !_attr.empty();
}

bool SXmlAttr::empty() const
{
	return _attr.empty();
}

const wchar_t* SXmlAttr::name() const
{
	return _attr.name();
}

const wchar_t* SXmlAttr::value() const
{
	return _attr.value();
}

const wchar_t* SXmlAttr::as_string(const wchar_t* def /*= L""*/) const
{
	return _attr.as_string(def);
}

int SXmlAttr::as_int(int def /*= 0*/) const
{
	return _attr.as_int(def);
}

unsigned int SXmlAttr::as_uint(unsigned int def /*= 0*/) const
{
	return _attr.as_uint(def);
}

double SXmlAttr::as_double(double def /*= 0*/) const
{
	return _attr.as_double(def);
}

float SXmlAttr::as_float(float def /*= 0*/) const
{
	return _attr.as_float(def);
}

bool SXmlAttr::as_bool(bool def /*= false*/) const
{
	return _attr.as_bool(def);
}

bool SXmlAttr::set_name(const wchar_t* rhs)
{
	return _attr.set_name(rhs);
}

bool SXmlAttr::set_value(const wchar_t* rhs)
{
	return _attr.set_value(rhs);
}

bool SXmlAttr::set_value(int rhs)
{
	return _attr.set_value(rhs);
}

bool SXmlAttr::set_value(unsigned int rhs)
{
	return _attr.set_value(rhs);
}

bool SXmlAttr::set_value(long rhs)
{
	return _attr.set_value(rhs);
}

bool SXmlAttr::set_value(unsigned long rhs)
{
	return _attr.set_value(rhs);
}

bool SXmlAttr::set_value(double rhs)
{
	return _attr.set_value(rhs);
}

bool SXmlAttr::set_value(double rhs, int precision)
{
	return _attr.set_value(rhs,precision);
}

bool SXmlAttr::set_value(float rhs)
{
	return _attr.set_value(rhs);
}

bool SXmlAttr::set_value(float rhs, int precision)
{
	return _attr.set_value(rhs,precision);
}

bool SXmlAttr::set_value(bool rhs)
{
	return _attr.set_value(rhs);
}

SXmlAttr SXmlAttr::next_attribute() const
{
	return _attr.next_attribute();
}

SXmlAttr SXmlAttr::previous_attribute() const
{
	return _attr.previous_attribute();
}

//////////////////////////////////////////////////////////////////////////

SXmlNode::SXmlNode(LPVOID pData):_node((pugi::xml_node_struct*)pData)
{

}

SXmlNode::SXmlNode(const IXmlNode * src):_node((pugi::xml_node_struct*)src->GetPrivPtr())
{

}

SXmlNode::SXmlNode(pugi::xml_node node):_node(node)
{

}

SXmlNode::SXmlNode(const SXmlNode& src):_node(src._node)
{
	
}


IStringW* SXmlNode::ToString(THIS) SCONST
{
	pugi::xml_writer_buff writer;
	_node.print(writer, L"\t", pugi::format_default, pugi::encoding_utf16);
	return new SStringW(writer.buffer(), writer.size());
}

LPVOID SXmlNode::GetPrivPtr(THIS) SCONST
{
	return _node.internal_object();
}

bool SXmlNode::Empty(THIS) SCONST
{
	return _node.empty();
}

const wchar_t * SXmlNode::Name(THIS) SCONST
{
	return _node.name();
}

const wchar_t * SXmlNode::Value(THIS) SCONST
{
	return _node.value();
}

const wchar_t * SXmlNode::Text(THIS) SCONST
{
	return _node.text().get();
}

bool SXmlNode::set_userdata(THIS_ int data)
{
	return _node.set_userdata(data);
}

int SXmlNode::get_userdata(THIS) SCONST
{
	return _node.get_userdata();
}

IXmlAttr * SXmlNode::Attribute(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST
{
	pugi::xml_attribute attr = _node.attribute(name,bCaseSensitive);
	return SXmlAttr::toIXmlAttr(attr);
}

IXmlAttr * SXmlNode::FirstAttribute(THIS) SCONST
{
	pugi::xml_attribute attr = _node.first_attribute();
	return SXmlAttr::toIXmlAttr(attr);
}

IXmlAttr * SXmlNode::LastAttribute(THIS) SCONST
{
	pugi::xml_attribute attr = _node.last_attribute();
	return SXmlAttr::toIXmlAttr(attr);
}

IXmlNode *SXmlNode::Child(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST
{
	pugi::xml_node node = _node.child(name,bCaseSensitive);
	return toIXmlNode(node);
}

IXmlNode *SXmlNode::FirstChild(THIS) SCONST
{
	pugi::xml_node node = _node.first_child();
	return toIXmlNode(node);
}

IXmlNode *SXmlNode::LastChild(THIS) SCONST
{
	pugi::xml_node node = _node.last_child();
	return toIXmlNode(node);
}

IXmlNode *SXmlNode::NextSibling(THIS) SCONST
{
	pugi::xml_node node = _node.next_sibling();
	return toIXmlNode(node);
}

IXmlNode *SXmlNode::PrevSibling(THIS) SCONST
{
	pugi::xml_node node = _node.previous_sibling();
	return toIXmlNode(node);
}

IXmlNode *SXmlNode::NextSibling2(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST
{
	pugi::xml_node node = _node.next_sibling(name,bCaseSensitive);
	return toIXmlNode(node);
}

IXmlNode *SXmlNode::PrevSibling2(THIS_ const wchar_t* name,bool bCaseSensitive) SCONST
{
	pugi::xml_node node = _node.previous_sibling(name,bCaseSensitive);
	return toIXmlNode(node);
}

IXmlNode * SXmlNode::toIXmlNode(pugi::xml_node node)
{
	if(node)
	{
		return new SXmlNode(node.internal_object());
	}else
	{
		return NULL;
	}
}

SXmlNode::operator bool() const
{
	return !_node.empty();
}

bool SXmlNode::empty() const
{
	return _node.empty();
}

XmlNodeType SXmlNode::type() const
{
	return (XmlNodeType)_node.type();
}

const wchar_t* SXmlNode::name() const
{
	return _node.name();
}

const wchar_t* SXmlNode::value() const
{
	return _node.value();
}

SXmlAttr SXmlNode::first_attribute() const
{
	return _node.first_attribute();
}

SXmlAttr SXmlNode::last_attribute() const
{
	return _node.last_attribute();
}

SXmlNode SXmlNode::first_child() const
{
	return _node.first_child();
}

SXmlNode SXmlNode::last_child() const
{
	return _node.last_child();
}

SXmlNode SXmlNode::next_sibling() const
{
	return _node.next_sibling();
}

SXmlNode SXmlNode::next_sibling(const wchar_t* name,bool bCaseSensitive/*=false*/) const
{
	return _node.next_sibling(name,bCaseSensitive);
}

SXmlNode SXmlNode::previous_sibling() const
{
	return _node.previous_sibling();
}

SXmlNode SXmlNode::previous_sibling(const wchar_t* name,bool bCaseSensitive/*=false*/) const
{
	return _node.previous_sibling(name,bCaseSensitive);
}

SXmlNode SXmlNode::parent() const
{
	return _node.parent();
}

SXmlNode SXmlNode::root() const
{
	return _node.root();
}

SXmlNode SXmlNode::child(const wchar_t* name,bool bCaseSensitive/*=false*/) const
{
	return _node.child(name,bCaseSensitive);
}

SXmlAttr SXmlNode::attribute(const wchar_t* name,bool bCaseSensitive/*=false*/) const
{
	return _node.attribute(name,bCaseSensitive);
}

SXmlAttr SXmlNode::attribute(const wchar_t* name, SXmlAttr& hint,bool bCaseSensitive/*=false*/) const
{
	return _node.attribute(name,hint._attr,bCaseSensitive);
}

const wchar_t* SXmlNode::child_value() const
{
	return _node.child_value();
}

const wchar_t* SXmlNode::child_value(const wchar_t* name,bool bCaseSensitive/*=false*/) const
{
	return _node.child_value(name,bCaseSensitive);
}

bool SXmlNode::set_name(const wchar_t* rhs)
{
	return _node.set_name(rhs);
}

bool SXmlNode::set_value(const wchar_t* rhs)
{
	return _node.set_value(rhs);
}

SXmlAttr SXmlNode::append_attribute(const wchar_t* name)
{
	return _node.append_attribute(name);
}

SXmlAttr SXmlNode::prepend_attribute(const wchar_t* name)
{
	return _node.prepend_attribute(name);
}

SXmlAttr SXmlNode::insert_attribute_after(const wchar_t* name, const SXmlAttr& attr)
{
	return _node.insert_attribute_after(name,attr._attr);
}

SXmlAttr SXmlNode::insert_attribute_before(const wchar_t* name, const SXmlAttr& attr)
{
	return _node.insert_attribute_before(name,attr._attr);
}

SXmlAttr SXmlNode::append_copy(const SXmlAttr& proto)
{
	return _node.append_copy(proto._attr);
}

SXmlNode SXmlNode::append_copy(const SXmlNode& proto)
{
	return _node.append_copy(proto._node);
}

SXmlAttr SXmlNode::prepend_copy(const SXmlAttr& proto)
{
	return _node.prepend_copy(proto._attr);
}

SXmlNode SXmlNode::prepend_copy(const SXmlNode& proto)
{
	return _node.prepend_copy(proto._node);
}

SXmlAttr SXmlNode::insert_copy_after(const SXmlAttr& proto, const SXmlAttr& attr)
{
	return _node.insert_copy_after(proto._attr,attr._attr);
}

SXmlNode SXmlNode::insert_copy_after(const SXmlNode& proto, const SXmlNode& node)
{
	return _node.insert_copy_after(proto._node,node._node);
}

SXmlAttr SXmlNode::insert_copy_before(const SXmlAttr& proto, const SXmlAttr& attr)
{
	return _node.insert_copy_before(proto._attr,attr._attr);
}

SXmlNode SXmlNode::insert_copy_before(const SXmlNode& proto, const SXmlNode& node)
{
	return _node.insert_move_before(proto._node,node._node);
}

SXmlNode SXmlNode::append_child(XmlNodeType type /*= node_element*/)
{
	return _node.append_child((pugi::xml_node_type)type);
}

SXmlNode SXmlNode::append_child(const wchar_t* name)
{
	return _node.append_child(name);
}

SXmlNode SXmlNode::prepend_child(XmlNodeType type /*= node_element*/)
{
	return _node.prepend_child((pugi::xml_node_type)type);
}

SXmlNode SXmlNode::prepend_child(const wchar_t* name)
{
	return _node.prepend_child(name);
}

SXmlNode SXmlNode::insert_child_after(XmlNodeType type, const SXmlNode& node)
{
	return _node.insert_child_after((pugi::xml_node_type)type,node._node);
}

SXmlNode SXmlNode::insert_child_after(const wchar_t* name, const SXmlNode& node)
{
	return _node.insert_child_after(name,node._node);
}

SXmlNode SXmlNode::insert_child_before(XmlNodeType type, const SXmlNode& node)
{
	return _node.insert_child_before((pugi::xml_node_type)type,node._node);
}

SXmlNode SXmlNode::insert_child_before(const wchar_t* name, const SXmlNode& node)
{
	return _node.insert_child_before(name,node._node);
}

SXmlNode SXmlNode::append_move(const SXmlNode& moved)
{
	return _node.append_move(moved._node);
}

SXmlNode SXmlNode::prepend_move(const SXmlNode& moved)
{
	return _node.prepend_move(moved._node);
}

SXmlNode SXmlNode::insert_move_after(const SXmlNode& moved, const SXmlNode& node)
{
	return _node.insert_move_after(moved._node,node._node);
}

SXmlNode SXmlNode::insert_move_before(const SXmlNode& moved, const SXmlNode& node)
{
	return _node.insert_move_before(moved._node,node._node);
}

bool SXmlNode::remove_attribute(const SXmlAttr& a)
{
	return _node.remove_attribute(a._attr);
}

bool SXmlNode::remove_attribute(const wchar_t* name)
{
	return _node.remove_attribute(name);
}

bool SXmlNode::remove_attributes()
{
	return _node.remove_attributes();
}

bool SXmlNode::remove_child(const SXmlNode& n)
{
	return _node.remove_child(n._node);
}

bool SXmlNode::remove_child(const wchar_t* name)
{
	return _node.remove_child(name);
}

bool SXmlNode::remove_children()
{
	return _node.remove_children();
}


//////////////////////////////////////////////////////////////////////////

SXmlDoc::SXmlDoc()
{
	_doc = new pugi::xml_document;
}

SXmlDoc::~SXmlDoc()
{
	delete _doc;
}

IXmlNode * SXmlDoc::Root(THIS) SCONST
{
	return SXmlNode::toIXmlNode(*_doc);
}

bool SXmlDoc::SaveFileW(THIS_ const wchar_t* path, const wchar_t* indent , unsigned int flags, XmlEncoding encoding) SCONST
{
	return _doc->save_file(path,indent,flags,(pugi::xml_encoding)encoding);
}

bool SXmlDoc::SaveFileA(THIS_ const char* path, const wchar_t* indent , unsigned int flags, XmlEncoding encoding) SCONST
{
	return _doc->save_file(path,indent,flags,(pugi::xml_encoding)encoding);
}

void SXmlDoc::SaveBinary(THIS_ FILE *f) SCONST
{
	_doc->save_bin(f);
}

bool SXmlDoc::LoadBufferInplaceOwn(THIS_ void* contents, size_t size, unsigned int options , XmlEncoding encoding)
{
	_result = _doc->load_buffer_inplace_own(contents,size,options,(pugi::xml_encoding)encoding);
	return _result;
}

bool SXmlDoc::LoadBufferInplace(THIS_ void* contents, size_t size, unsigned int options , XmlEncoding encoding)
{
	_result = _doc->load_buffer_inplace(contents,size,options,(pugi::xml_encoding)encoding);
	return _result;
}

bool SXmlDoc::LoadBuffer(THIS_ const void* contents, size_t size, unsigned int options , XmlEncoding encoding)
{
	_result = _doc->load_buffer(contents,size,options,(pugi::xml_encoding)encoding);
	return _result;
}

bool SXmlDoc::LoadFileW(THIS_ const wchar_t* path, unsigned int options , XmlEncoding encoding)
{
	_result = _doc->load_file(path,options,(pugi::xml_encoding)encoding);
	return _result;
}

bool SXmlDoc::LoadFileA(THIS_ const char* path, unsigned int options, XmlEncoding encoding)
{
	_result = _doc->load_file(path,options,(pugi::xml_encoding)encoding);
	return _result;
}

bool SXmlDoc::LoadString(THIS_ const wchar_t* contents, unsigned int options)
{
	_result = _doc->load_string(contents,options);
	return _result;
}

void SXmlDoc::GetParseResult(THIS_ XmlParseResult *pResult) SCONST
{
	SASSERT(pResult);
	pResult->status = (XmlStatus)_result.status;
	pResult->offset = _result.offset;
	pResult->encoding = (XmlEncoding)_result.encoding;
}

void SXmlDoc::Copy(THIS_ const IXmlDoc* proto)
{
	_doc->reset(*(pugi::xml_document*)proto->GetPrivPtr());
}

void SXmlDoc::Reset(THIS)
{
	_doc->reset();
}

LPVOID SXmlDoc::GetPrivPtr(THIS) SCONST
{
	return _doc;
}

bool SXmlDoc::load_string(const wchar_t* contents, unsigned int options /*= xml_parse_default*/)
{
	_result = _doc->load_string(contents,options);
	return _result;
}

bool SXmlDoc::load_file(const char* path, unsigned int options /*= xml_parse_default*/, XmlEncoding encoding /*= enc_auto*/)
{
	_result = _doc->load_file(path,options,(pugi::xml_encoding)encoding);
	return _result;
}

bool SXmlDoc::load_file(const wchar_t* path, unsigned int options /*= xml_parse_default*/, XmlEncoding encoding /*= enc_auto*/)
{
	_result = _doc->load_file(path,options,(pugi::xml_encoding)encoding);
	return _result;
}

bool SXmlDoc::load_buffer(const void* contents, size_t size, unsigned int options /*= xml_parse_default*/, XmlEncoding encoding /*= enc_auto*/)
{
	_result = _doc->load_buffer(contents,size,options,(pugi::xml_encoding)encoding);
	return _result;
}

bool SXmlDoc::load_buffer_inplace(void* contents, size_t size, unsigned int options /*= xml_parse_default*/, XmlEncoding encoding /*= enc_auto*/)
{
	_result = _doc->load_buffer_inplace(contents,size,options,(pugi::xml_encoding)encoding);
	return _result;
}

bool SXmlDoc::load_buffer_inplace_own(void* contents, size_t size, unsigned int options /*= xml_parse_default*/, XmlEncoding encoding /*= enc_auto*/)
{
	_result = _doc->load_buffer_inplace_own(contents,size,options,(pugi::xml_encoding)encoding);
	return _result;
}

bool SXmlDoc::save_file(const char* path, const wchar_t* indent /*= L"\t"*/, unsigned int flags /*= xml_parse_default*/, XmlEncoding encoding /*= enc_auto*/) const
{
	return _doc->save_file(path,indent,flags,(pugi::xml_encoding)encoding);
}

bool SXmlDoc::save_file(const wchar_t* path, const wchar_t* indent /*= L"\t"*/, unsigned int flags /*= xml_parse_default*/, XmlEncoding encoding /*= enc_auto*/) const
{
	return _doc->save_file(path,indent,flags,(pugi::xml_encoding)encoding);
}

SXmlNode SXmlDoc::root() const
{
	return SXmlNode(_doc);
}

SNSEND
