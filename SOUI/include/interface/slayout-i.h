/*
SOUI窗口布局接口
*/
#pragma once

#include <interface/sobject-i.h>

SNSBEGIN

class SWindow;
class SLayoutSize;


enum ORIENTATION{
	Horz,Vert,Any,Both,
};

enum{
	SIZE_UNDEF = -3,
	SIZE_WRAP_CONTENT=-1,
	SIZE_MATCH_PARENT=-2,
	SIZE_SPEC PURE,
};

#undef INTERFACE
#define INTERFACE ILayoutParam
DECLARE_INTERFACE_(ILayoutParam,IObject)
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
	STDMETHOD_(LPCWSTR,GetObjectClass)(THIS_) SCONST PURE;

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

	/**
	* GetName
	* @brief    获取对象Name
	* @return   LPCWSTR -- 对象Name
	* Describe  
	*/    
	STDMETHOD_(LPCWSTR,GetName)(THIS) SCONST PURE;

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
	* @param    LPCWSTR pszAttr --  属性名
	* @param    LPCWSTR pszValue --  属性值
	* @param    BOOL bLoading --  对象创建时由系统调用标志
	* @return   HRESULT -- 处理处理结果
	* Describe  
	*/    
	STDMETHOD_(HRESULT,SetAttribute)(THIS_ LPCWSTR pszAttr, LPCWSTR pszValue, BOOL bLoading) PURE;

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

	/**
	* DefAttributeProc
	* @brief    默认属性处理函数
	* @param    const SStringW & strAttribName --  属性名
	* @param	 const SStringW & strValue --属性值
	* @param    BOOL bLoading -- 从XML初始化标志
	* @return   HRESULT -- S_OK:刷新UI， S_FALSE:成功但不刷新UI，其它：失败
	* Describe  在SetAttribute中没有处理一个属性时转到本方法处理。
	*/  
	STDMETHOD_(HRESULT,DefAttributeProc)(THIS_ const IStringW * strAttribName,const IStringW * strValue, BOOL bLoading) PURE;

	STDMETHOD_(void,Clear)(THIS) PURE;
	STDMETHOD_(BOOL, IsMatchParent)(THIS_ ORIENTATION orientation) SCONST PURE;
	STDMETHOD_(BOOL, IsWrapContent)(THIS_ ORIENTATION orientation) SCONST PURE;
	STDMETHOD_(BOOL, IsSpecifiedSize)(THIS_ ORIENTATION orientation) SCONST PURE;
	STDMETHOD_(SLayoutSize, GetSpecifiedSize)(THIS_ ORIENTATION orientation) SCONST PURE;
	STDMETHOD_(void,SetMatchParent)(THIS_ ORIENTATION orientation) PURE;
	STDMETHOD_(void,SetWrapContent)(THIS_ ORIENTATION orientation) PURE;
	STDMETHOD_(void,SetSpecifiedSize)(THIS_ ORIENTATION orientation, const SLayoutSize& layoutSize) PURE;
	STDMETHOD_(void*, GetRawData)(THIS) PURE;
	STDMETHOD_(ILayoutParam *, Clone)(THIS) SCONST PURE;
};

#undef INTERFACE
#define INTERFACE ILayout
DECLARE_INTERFACE_(ILayout,IObject)
{
	STDMETHOD_(BOOL, IsParamAcceptable)(THIS_ ILayoutParam *pLayoutParam) SCONST PURE;
	STDMETHOD_(void,LayoutChildren)(THIS_ SWindow * pParent) PURE;
	STDMETHOD_(ILayoutParam *, CreateLayoutParam)(THIS) SCONST PURE;
	STDMETHOD_(SIZE, MeasureChildren)(THIS_ const SWindow * pParent,int nWidth,int nHeight) SCONST PURE;
};

SNSEND