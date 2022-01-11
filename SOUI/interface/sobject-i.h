/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       Sobject.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/01
* 
* Describe    the base class used in SOUI, which provides type identify of class in runtime
*             and implement attributes dispatcher described in XML. 
*/

#pragma once
#include <interface/obj-ref-i.h>
#include <interface/sstring-i.h>
#include <interface/sxml-i.h>

#pragma warning(disable:4275)
//////////////////////////////////////////////////////////////////////////
SNSBEGIN

// SObject Class Name Declaration
#define SOUI_CLASS_NAME_EX(baseCls, classname,clsType)  \
public:                                                 \
	typedef baseCls	__baseCls;                          \
	static LPCWSTR GetClassName()                       \
	{                                                   \
		return classname;                               \
	}                                                   \
	\
	static int GetClassType()                           \
	{                                                   \
		int ret = clsType;                              \
		if(ret == SOUI::Undef)                          \
			ret = __baseCls::GetClassType();            \
		return ret;                                     \
	}                                                   \
	\
	static LPCWSTR BaseClassName()                      \
	{                                                   \
		return __baseCls::GetClassName();               \
	}                                                   \
	\
	virtual LPCWSTR WINAPI GetObjectClass()  const      \
	{                                                   \
		return classname;                               \
	}                                                   \
	\
	virtual int WINAPI GetObjectType()  const           \
	{                                                   \
		int ret = clsType;                              \
		if(ret == SOUI::Undef)                          \
			ret = __baseCls::GetObjectType();           \
		return ret;                                     \
	}                                                   \
	\
	virtual BOOL WINAPI IsClass(LPCWSTR lpszName) const        \
	{                                                   \
		if(wcscmp(GetClassName(), lpszName)  == 0)      \
			return TRUE;                                \
		return baseCls::IsClass(lpszName);              \
	}                                                   \


#define SOUI_CLASS_NAME(baseCls,classname) \
	SOUI_CLASS_NAME_EX(baseCls,classname,0)

/**
* @class      SObject
* @brief      SOUI系统中的对象基类
* 
* Describe    提供类RTTI机制，实现从XML节点中给派生类对象设置属性
*/
#undef INTERFACE
#define INTERFACE IObject
DECLARE_INTERFACE_(IObject,IObjRef)
{
#ifdef __cplusplus
	/**
	* GetClassName
	* @brief    获得对象类型名称
	* @return   LPCWSTR -- 类名
	* Describe  静态函数
	*/    
	static LPCWSTR GetClassName(){return L"object";}

	/**
	* GetClassType
	* @brief    获得对象类型
	* @return   int -- 类型
	* Describe  静态函数
	*/
	static int     GetClassType() { return 0; }
#endif

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

	/**
	* IsClass
	* @brief    判断this是不是属于指定的类型
	* @param    LPCWSTR lpszName --  测试类型名
	* @return   BOOL -- true是测试类型
	* Describe  
	*/    
	STDMETHOD_(BOOL,IsClass)(THIS_ LPCWSTR lpszName) SCONST PURE;

	/**
	* GetObjectClass
	* @brief    获得类型名
	* @return   LPCWSTR -- 类型名
	* Describe  这是一个虚函数，注意与GetClassName的区别。
	*/    
	STDMETHOD_(LPCWSTR,GetObjectClass)(THIS) SCONST PURE;

	/**
	* GetObjectType
	* @brief    获得对象类型
	* @return   int -- 对象类型
	* Describe  这是一个虚函数，注意与GetClassType的区别。
	*/    
	STDMETHOD_(int,GetObjectType)(THIS)  SCONST PURE;

	/**
	* GetID
	* @brief    获取对象ID
	* @return   int -- 对象ID
	* Describe  
	*/    
	STDMETHOD_(int,GetID)(THIS) SCONST PURE;
	STDMETHOD_(void,SetID)(THIS_ int nID) PURE;

	/**
	* GetName
	* @brief    获取对象Name
	* @return   LPCWSTR -- 对象Name
	* Describe  
	*/    
	STDMETHOD_(LPCWSTR,GetName)(THIS) SCONST PURE;
	STDMETHOD_(void,SetName)(THIS_ LPCWSTR pszName) PURE;

	/**
	* InitFromXml
	* @brief    从XML结节初始化SObject对象
	* @param    SXmlNode --  XML结节
	* @return   BOOL -- 成功返回TRUE
	* Describe  
	*/    
	STDMETHOD_(BOOL,InitFromXml)(THIS_ IXmlNode * xmlNode ) PURE;

	/**
	* OnInitFinished
	* @brief    属性初始化完成处理接口
	* @param    SXmlNode xmlNode --  属性节点
	* @return   void
	* Describe  
	*/    
	STDMETHOD_(void,OnInitFinished)(THIS_ IXmlNode* xmlNode) PURE;

	/**
	* SetAttributeA
	* @brief    设置一个对象属性
	* @param    const IStringA * strAttribName --  属性名
	* @param    const IStringA * strValue --  属性值
	* @param    BOOL bLoading --  对象创建时由系统调用标志
	* @return   HRESULT -- 处理处理结果
	* Describe  
	*/    
	STDMETHOD_(HRESULT,SetAttributeA)(THIS_ const IStringA * strAttribName, const IStringA *  strValue, BOOL bLoading) PURE;

	/**
	* SetAttributeW
	* @brief    设置一个对象属性
	* @param    const IStringA *strAttribName --  属性名
	* @param    const IStringA *strValue --  属性值
	* @param    BOOL bLoading --  对象创建时由系统调用标志
	* @return   HRESULT -- 处理处理结果
	* Describe  
	*/    
	STDMETHOD_(HRESULT,SetAttributeW)(THIS_ const IStringW *  strAttribName, const IStringW *  strValue, BOOL bLoading) PURE;

	/**
	* SetAttribute
	* @brief    设置一个对象属性
	* @param    LPCSTR pszAttr --  属性名
	* @param    LPCSTR pszValue --  属性值
	* @param    BOOL bLoading --  对象创建时由系统调用标志
	* @return   HRESULT -- 处理处理结果
	* Describe  
	*/    
	STDMETHOD_(HRESULT,SetAttribute)(THIS_ LPCSTR pszAttr, LPCSTR pszValue, BOOL bLoading) PURE;

	/**
	* SetAttribute
	* @brief    设置一个对象属性
	* @param    LPCWSTR pszAttr --  属性名
	* @param    LPCWSTR pszValue --  属性值
	* @param    BOOL bLoading --  对象创建时由系统调用标志
	* @return   HRESULT -- 处理处理结果
	* Describe  
	*/    
	STDMETHOD_(HRESULT,SetAttributeW)(THIS_ LPCWSTR pszAttr, LPCWSTR pszValue, BOOL bLoading) PURE;

	/**
	* GetAttribute
	* @brief    通过属性名查询属性值
	* @param    const SStringW & strAttr --  属性名
	* @param    IStringW * pValue -- 属性值
	* @return   BOOL, TRUE:获取成功，FALSE:获取失败，属性不存在
	* Describe  默认返回空
	*/    
	STDMETHOD_(BOOL,GetAttribute)(THIS_ const IStringW * strAttr, IStringW * pValue) SCONST PURE;

	/**
	* OnAttribute
	* @brief    属性处理后调用的方法
	* @param    const SStringW & strAttribName --  属性名
	* @param    const SStringW & strValue --  属性名
	* @param    BOOL bLoading --  对象创建时由系统调用标志
	* @param    HRESULT hr --  属性处理结果
	* Describe  不做处理，直接返回
	*/    
	STDMETHOD_(HRESULT,AfterAttribute)(THIS_ const IStringW * strAttribName,const IStringW * strValue, BOOL bLoading, HRESULT hr) PURE;
};

#ifdef __cplusplus
/**
* sobj_cast
* @brief    SOUI Object 的类型安全的类型转换接口
* @param    SObject * pObj --  源对象
* @return   T * -- 转换后的对象
* Describe  如果源对象不是待转换对象类型，返回NULL
*/    
template<class T>
T * sobj_cast(const IObject *pObj)
{
	if(!pObj)
		return NULL;

	if(pObj->IsClass(T::GetClassName()))
		return (T*)pObj;
	else
		return NULL;
}

#endif

SNSEND
