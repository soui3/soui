#pragma once
#include <interface/sobject-i.h>
SNSBEGIN

#undef INTERFACE
#define INTERFACE IObject
DECLARE_INTERFACE_(IInterpolator,IObject)
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
	* @param    pugi::xml_node --  XML结节
	* @return   BOOL -- 成功返回TRUE
	* Describe  
	*/    
	STDMETHOD_(BOOL,InitFromXml)(THIS_ IXmlNode * xmlNode ) PURE;

	/**
	* OnInitFinished
	* @brief    属性初始化完成处理接口
	* @param    pugi::xml_node xmlNode --  属性节点
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

	/**
	* getInterpolation
	* @brief    计算出插值
	* @param    float input --  输入值[0,1]
	* @return   float -- 输出插值结果
	* Describe  
	*/  
	STDMETHOD_(float,getInterpolation)(THIS_ float input) SCONST PURE;
};
SNSEND