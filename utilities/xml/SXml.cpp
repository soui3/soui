#include <xml/SXml.h>
#include <pugixml/pugixml.hpp>

SNSBEGIN


SXmlAttr::SXmlAttr(LPVOID pData):_attr((pugi::xml_attribute_struct*)pData)
{
}

SXmlAttr::SXmlAttr(pugi::xml_attribute attr):_attr(attr)
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

//////////////////////////////////////////////////////////////////////////

SXmlNode::SXmlNode(LPVOID pData):_node((pugi::xml_node_struct*)pData)
{

}

SXmlNode::SXmlNode(pugi::xml_node node):_node(node)
{

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

//////////////////////////////////////////////////////////////////////////

SXmlDoc::SXmlDoc()
{
	doc = new pugi::xml_document;
}

SXmlDoc::~SXmlDoc()
{
	delete doc;
}

IXmlNode * SXmlDoc::Root(THIS) SCONST
{
	pugi::xml_node root = doc->document_element();
	return SXmlNode::toIXmlNode(root);
}

bool SXmlDoc::SaveFileW(THIS_ const wchar_t* path, const wchar_t* indent , unsigned int flags, XmlEncoding encoding) SCONST
{
	return doc->save_file(path,indent,flags,(pugi::xml_encoding)encoding);
}

bool SXmlDoc::SaveFileA(THIS_ const char* path, const wchar_t* indent , unsigned int flags, XmlEncoding encoding) SCONST
{
	return doc->save_file(path,indent,flags,(pugi::xml_encoding)encoding);
}

void SXmlDoc::SaveBinary(THIS_ FILE *f) SCONST
{
	doc->save_bin(f);
}

XmlStatus SXmlDoc::LoadBufferInplaceOwn(THIS_ void* contents, size_t size, unsigned int options , XmlEncoding encoding)
{
	pugi::xml_parse_result res = doc->load_buffer_inplace_own(contents,size,options,(pugi::xml_encoding)encoding);
	return (XmlStatus)res.status;
}

XmlStatus SXmlDoc::LoadBufferInplace(THIS_ void* contents, size_t size, unsigned int options , XmlEncoding encoding)
{
	pugi::xml_parse_result res = doc->load_buffer_inplace(contents,size,options,(pugi::xml_encoding)encoding);
	return (XmlStatus)res.status;
}

XmlStatus SXmlDoc::LoadBuffer(THIS_ const void* contents, size_t size, unsigned int options , XmlEncoding encoding)
{
	pugi::xml_parse_result res = doc->load_buffer(contents,size,options,(pugi::xml_encoding)encoding);
	return (XmlStatus)res.status;
}

XmlStatus SXmlDoc::LoadFileW(THIS_ const wchar_t* path, unsigned int options , XmlEncoding encoding)
{
	pugi::xml_parse_result res = doc->load_file(path,options,(pugi::xml_encoding)encoding);
	return (XmlStatus)res.status;
}

XmlStatus SXmlDoc::LoadFileA(THIS_ const char* path, unsigned int options, XmlEncoding encoding)
{
	pugi::xml_parse_result res = doc->load_file(path,options,(pugi::xml_encoding)encoding);
	return (XmlStatus)res.status;
}

XmlStatus SXmlDoc::LoadString(THIS_ const wchar_t* contents, unsigned int options)
{
	pugi::xml_parse_result res = doc->load_string(contents,options);
	return (XmlStatus)res.status;
}

void SXmlDoc::Copy(THIS_ const IXmlDoc* proto)
{
	doc->reset(*(pugi::xml_document*)proto->GetPrivPtr());
}

void SXmlDoc::Reset(THIS)
{
	doc->reset();
}

LPVOID SXmlDoc::GetPrivPtr(THIS) SCONST
{
	return doc;
}




SNSEND
