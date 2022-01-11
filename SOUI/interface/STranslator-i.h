/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       STranslator-i.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    
*/

#pragma once

#include <interface/obj-ref-i.h>
#include <interface/sstring-i.h>

SNSBEGIN

#define TR_MAX_NAME_LEN	64

/** 
* @struct     ITranslator
* @brief      语言翻译接口
*
* Describe
*/
#undef INTERFACE
#define INTERFACE ITranslator
DECLARE_INTERFACE_(ITranslator,IObjRef)
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
	* Load
	* @brief    从资源中加载语言翻译数据
	* @param    LPVOID pData --  资源指针，具体含义由接口的实现来解释
	* @param    UINT uType --  资源类型，具体含义由接口的实现来解释
	* @return   BOOL true-加载成功, false-加载失败
	*
	* Describe  
	*/
	STDMETHOD_(BOOL,Load)(THIS_ LPVOID pData,UINT uType) PURE;
	/**
	* name
	* @brief    获取翻译资源的name
	* @return   SOUI::SStringW 翻译资源的name
	*
	* Describe  
	*/
	STDMETHOD_(void, GetName)(THIS_ wchar_t szBuf[TR_MAX_NAME_LEN]) PURE;

	STDMETHOD_( bool, NameEqual)(THIS_ LPCWSTR pszName) PURE;
	/**
	* guid
	* @brief    获取翻译资源的ID
	* @return   GUID 翻译资源的ID
	*
	* Describe  
	*/
	STDMETHOD_(GUID,guid)(THIS)PURE;
	/**
	* tr
	* @brief    执行翻译的接口
	* @param    const SStringW & strSrc --  原字符串
	* @param    const SStringW & strCtx --  翻译上下文
	* @param    SStringW & strRet --  翻译后的字符串
	* @return   int - buflen; 0 - no translation; -1 - buf size not enough;  
	*
	* Describe  
	*/
	STDMETHOD_(int,tr)(THIS_ const IStringW * strSrc,const IStringW * strCtx,wchar_t *pszOut,int nLen) SCONST PURE;

	STDMETHOD_(void,getFontInfo)(THIS_ IStringW * strFont) SCONST PURE;
};


/** 
* @struct     ITranslatorMgr
* @brief      语言翻译接口管理器
*
* Describe
*/
#undef INTERFACE
#define INTERFACE ITranslatorMgr
DECLARE_INTERFACE_(ITranslatorMgr,IObjRef)
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
	* SetLanguage
	* @brief    设置翻译模块当前接受的语言
	* @param [in] const SStringW & strLang --  翻译语言
	*
	* Describe 自动清除语言和目标语言不同的模块
	*/
	STDMETHOD_(void ,SetLanguage)(THIS_ const IStringW * strLang) PURE;

	/**
	* GetLanguage
	* @brief    获取翻译模块当前接受的语言
	* @return SStringW  --  翻译语言
	*
	* Describe 
	*/
	STDMETHOD_(void,GetLanguage)(THIS_ wchar_t szOut[TR_MAX_NAME_LEN]) SCONST PURE;

	/**
	* CreateTranslator
	* @brief    创建一个语言翻译对象
	* @param [out] ITranslator * * ppTranslator --  接收语言翻译对象的指针
	* @return   BOOL true-成功，false-失败
	*
	* Describe  
	*/
	STDMETHOD_(BOOL,CreateTranslator)(THIS_ ITranslator ** ppTranslator) PURE;
	/**
	* InstallTranslator
	* @brief    向管理器中安装一个语言翻译对象
	* @param    ITranslator * ppTranslator -- 语言翻译对象
	* @return   BOOL true-成功，false-失败
	*
	* Describe  
	*/

	STDMETHOD_(BOOL,InstallTranslator)(THIS_ ITranslator * ppTranslator) PURE;
	/**
	* UninstallTranslator
	* @brief    从管理器中卸载一个语言翻译对象
	* @param    REFGUID id --  语言翻译对象的ID
	* @return   BOOL true-成功，false-失败
	*
	* Describe  
	*/
	STDMETHOD_(BOOL,UninstallTranslator)(THIS_ REFGUID id) PURE;

	/**
	* tr
	* @brief    翻译字符串
	* @param    const SStringW & strSrc --  原字符串
	* @param    const SStringW & strCtx --  翻译上下文
	* @return   SOUI::SStringW 翻译后的字符串
	*
	* Describe  调用ITranslator的tr接口执行具体翻译过程
	*/
	STDMETHOD_(int,tr)(THIS_ const IStringW * strSrc,const IStringW * strCtx,wchar_t *pszOut,int nLen) SCONST PURE;
};

SNSEND