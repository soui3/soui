#include <utilities.h>
#include <xml/SXmlDoc.h>
#include <pugixml/pugixml.hpp>

SNSBEGIN

class SXmlDoc : public TObjRefImpl<IXmlDoc>
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
	STDMETHOD_(XmlStatus,LoadString)(THIS_ const char_t* contents, unsigned int options)OVERRIDE;

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
	STDMETHOD_(bool,SaveFileA)(THIS_ const char* path, const char_t* indent , unsigned int flags, XmlEncoding encoding) SCONST OVERRIDE;
	STDMETHOD_(bool,SaveFileW)(THIS_ const wchar_t* path, const char_t* indent , unsigned int flags, XmlEncoding encoding) SCONST OVERRIDE;

	// Get document element
	STDMETHOD_(IXmlNode *,Root)(THIS) SCONST OVERRIDE;
};


STDMETHODIMP_(IXmlNode *) SXmlDoc::Root(THIS) SCONST
{
	pugi::xml_node root = doc->document_element();
	if(root)
		return new pugi::xml_node(&root);
	else
		return NULL;
}

STDMETHODIMP_(bool) SXmlDoc::SaveFileW(THIS_ const wchar_t* path, const char_t* indent , unsigned int flags, XmlEncoding encoding) SCONST
{
	return doc->save_file(path,indent,flags,(pugi::xml_encoding)encoding);
}

STDMETHODIMP_(bool) SXmlDoc::SaveFileA(THIS_ const char* path, const char_t* indent , unsigned int flags, XmlEncoding encoding) SCONST
{
	return doc->save_file(path,indent,flags,(pugi::xml_encoding)encoding);
}

STDMETHODIMP_(void) SXmlDoc::SaveBinary(THIS_ FILE *f) SCONST
{
	doc->save_bin(f);
}

STDMETHODIMP_(XmlStatus) SXmlDoc::LoadBufferInplaceOwn(THIS_ void* contents, size_t size, unsigned int options , XmlEncoding encoding)
{
	pugi::xml_parse_result res = doc->load_buffer_inplace_own(contents,size,options,(pugi::xml_encoding)encoding);
	return (XmlStatus)res.status;
}

STDMETHODIMP_(XmlStatus) SXmlDoc::LoadBufferInplace(THIS_ void* contents, size_t size, unsigned int options , XmlEncoding encoding)
{
	pugi::xml_parse_result res = doc->load_buffer_inplace(contents,size,options,(pugi::xml_encoding)encoding);
	return (XmlStatus)res.status;
}

STDMETHODIMP_(XmlStatus) SXmlDoc::LoadBuffer(THIS_ const void* contents, size_t size, unsigned int options , XmlEncoding encoding)
{
	pugi::xml_parse_result res = doc->load_buffer(contents,size,options,(pugi::xml_encoding)encoding);
	return (XmlStatus)res.status;
}

STDMETHODIMP_(XmlStatus) SXmlDoc::LoadFileW(THIS_ const wchar_t* path, unsigned int options , XmlEncoding encoding)
{
	pugi::xml_parse_result res = doc->load_file(path,options,(pugi::xml_encoding)encoding);
	return (XmlStatus)res.status;
}

STDMETHODIMP_(XmlStatus) SXmlDoc::LoadFileA(THIS_ const char* path, unsigned int options, XmlEncoding encoding)
{
	pugi::xml_parse_result res = doc->load_file(path,options,(pugi::xml_encoding)encoding);
	return (XmlStatus)res.status;
}

STDMETHODIMP_(XmlStatus) SXmlDoc::LoadString(THIS_ const char_t* contents, unsigned int options)
{
	pugi::xml_parse_result res = doc->load_string(contents,options);
	return (XmlStatus)res.status;
}

STDMETHODIMP_(void) SXmlDoc::Copy(THIS_ const IXmlDoc* proto)
{
	doc->reset(*(pugi::xml_document*)proto->GetPrivPtr());
}

STDMETHODIMP_(void) SXmlDoc::Reset(THIS)
{
	doc->reset();
}

STDMETHODIMP_(LPVOID) SXmlDoc::GetPrivPtr(THIS) SCONST
{
	return doc;
}

SXmlDoc::~SXmlDoc()
{
	delete doc;
}

SXmlDoc::SXmlDoc()
{
	doc = new pugi::xml_document;
}

SNSEND

IXmlDocPtr CreateXmlDoc()
{
	return new SOUI::SXmlDoc();
}
