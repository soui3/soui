/**
* Copyright (C) 2014-2050
* All rights reserved.
*
* @file       Sobject.hpp
* @brief
* @version    v1.0
* @author     SOUI group
* @date       2014/08/01
*
* Describe    the base class used in SOUI, which provides type identify of class in runtime
*             and implement attributes dispatcher described in XML.
*/

#pragma once
#include <interface/sobject-i.h>
#include <string/strcpcvt.h>
#include <pugixml/pugixml.hpp>

SNSBEGIN


template<class T>
class SObjectImpl : public T
{
public:
	static void MarkAttributeHandled(pugi::xml_attribute xmlAttr, bool bHandled)
	{
		xmlAttr.set_userdata(bHandled?1:0);
	}

	static bool IsAttributeHandled(pugi::xml_attribute xmlAttr)
	{
		return xmlAttr.get_userdata()==1?true:false;
	}


	STDMETHOD_(HRESULT,DefAttributeProc)(THIS_ const IStringW * strAttribName,const IStringW * strValue, BOOL bLoading) OVERRIDE
	{
		UNREFERENCED_PARAMETER(strAttribName);
		UNREFERENCED_PARAMETER(strValue);
		UNREFERENCED_PARAMETER(bLoading);
		return E_FAIL;
	}

	STDMETHOD_(BOOL,InitFromXml)(THIS_ IXmlNode * pXmlNode ) OVERRIDE
	{
		pugi::xml_node xmlNode((pugi::xml_node_struct *)pXmlNode->GetPrivPtr());

		if (!xmlNode) return FALSE;
#if defined(_DEBUG) && defined(PUGIXML_WCHAR_MODE)
		{
			pugi::xml_writer_buff writer;
			xmlNode.print(writer, L"\t", pugi::format_default, pugi::encoding_utf16);
			m_strXml = SStringW(writer.buffer(), writer.size());
		}
#endif

		//设置当前对象的属性

		for (pugi::xml_attribute attr = xmlNode.first_attribute(); attr; attr = attr.next_attribute())
		{
			if (IsAttributeHandled(attr)) continue;   //忽略已经被预处理的属性
			SetAttribute(attr.name(), attr.value(), TRUE);
		}
		//调用初始化完成接口
		OnInitFinished(pXmlNode);
		return TRUE;
	}

	STDMETHOD_(LPCWSTR,GetName)(THIS) SCONST OVERRIDE
	{
		return NULL;
	}

	STDMETHOD_(int,GetID)(THIS) SCONST OVERRIDE
	{
		return 0;
	}

	STDMETHOD_(HRESULT,AfterAttribute)(THIS_ const IStringW * strAttribName,const IStringW * strValue, BOOL bLoading, HRESULT hr) OVERRIDE
	{
		UNREFERENCED_PARAMETER(strAttribName);
		UNREFERENCED_PARAMETER(strValue);
		UNREFERENCED_PARAMETER(bLoading);
		return hr;
	}

	virtual HRESULT SetAttribute(const SStringW &strAttr,const SStringW & strValue,BOOL bLoading)
	{
		return DefAttributeProc(&strAttr, &strValue, bLoading);
	}

	STDMETHOD_(HRESULT,SetAttribute)(THIS_ LPCWSTR  pszAttr, LPCWSTR  pszValue, BOOL bLoading) OVERRIDE
	{
		return SetAttribute(SStringW(pszAttr),SStringW(pszValue),bLoading);
	}

	STDMETHOD_(HRESULT,SetAttributeW)(THIS_ const IStringW *  strAttr, const IStringW *  strValue, BOOL bLoading) OVERRIDE
	{
		return SetAttribute(SStringW(strAttr),SStringW(strValue),bLoading);
	}

	STDMETHOD_(HRESULT,SetAttributeA)(THIS_ const IStringA * strAttribName, const IStringA *  strValue, BOOL bLoading) OVERRIDE
	{
		SStringA strNameA(strAttribName);
		SStringA strValueA(strValue);
		SStringW strNameW=S_CA2W(strNameA);
		SStringW strValueW=S_CA2W(strValueA);
		return SetAttribute(strNameW, strValueW, bLoading);
	}

	STDMETHOD_(LPCWSTR,GetObjectClass)(THIS_) SCONST OVERRIDE
	{
		return NULL;
	}

	STDMETHOD_(int,GetObjectType)(THIS)  SCONST OVERRIDE
	{
		return 0;
	}


	STDMETHOD_(BOOL,IsClass)(THIS_ LPCWSTR lpszName) SCONST OVERRIDE
	{
		UNREFERENCED_PARAMETER(lpszName);
		return FALSE;
	}

	STDMETHOD_(BOOL,GetAttribute)(THIS_ const IStringW * strAttr, IStringW * pValue) SCONST OVERRIDE
	{
		UNREFERENCED_PARAMETER(strAttr);
		UNREFERENCED_PARAMETER(pValue);
		return FALSE;
	}

	/**
	* OnInitFinished
	* @brief    属性初始化完成处理接口
	* @param    pugi::xml_node xmlNode --  属性节点
	* @return   void
	* Describe
	*/ 
	STDMETHOD_(void,OnInitFinished)(THIS_ IXmlNode * xmlNode) OVERRIDE
	{
		UNREFERENCED_PARAMETER(xmlNode);
	}

#ifdef    _DEBUG
public:
	SStringW m_strXml;  //<** XML字符串，用于在调试时观察对象*/
#endif//_DEBUG
};


typedef SObjectImpl<IObject> SObject;

SNSEND