/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SResProvider-i.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    
*/

#ifndef _SRESPROVIDERBASE_
#define _SRESPROVIDERBASE_
#pragma once

#include <interface/obj-ref-i.h>
#include <interface/SRender-i.h>
#include <string/tstring.h>

SNSBEGIN
#define UIRES_INDEX    _T("uires.idx")        //文件夹资源的文件映射表索引表文件名

enum BUILTIN_RESTYPE
{
	RES_PE=0,
	RES_FILE,
};

typedef BOOL (CALLBACK * EnumResCallback)(LPCTSTR pszType,LPCTSTR pszName,LPARAM lp);

/**
* @struct     IResProvider
* @brief      ResProvider对象
* 
* Describe  实现各种资源的加载
*/
#undef INTERFACE
#define INTERFACE IResProvider
DECLARE_INTERFACE_(IResProvider,IObjRef)
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
	* Init
	* @brief    资源初始化函数
	* @param    WPARAM wParam --  param 1 
	* @param    LPARAM lParam --  param 2
	* @return   BOOL -- true:succeed
	*
	* Describe  every Resprovider must implement this interface.
	*/
	STDMETHOD_(BOOL,Init)(THIS_ WPARAM wParam,LPARAM lParam) PURE;

	/**
	* HasResource
	* @brief    查询一个资源是否存在
	* @param    LPCTSTR strType --  资源类型
	* @param    LPCTSTR pszResName --  资源名称
	* @return   BOOL -- true存在，false不存在
	* Describe  
	*/    
	STDMETHOD_(BOOL,HasResource)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) PURE;

	/**
	* LoadIcon
	* @brief    从资源中加载ICON
	* @param    LPCTSTR pszResName --  ICON名称
	* @param    int cx --  ICON宽度
	* @param    int cy --  ICON高度
	* @return   HICON -- 成功返回ICON的句柄，失败返回0
	* Describe  
	*/    
	STDMETHOD_(HICON,LoadIcon)(THIS_ LPCTSTR pszResName,int cx,int cy) PURE;

	/**
	* LoadBitmap
	* @brief    从资源中加载HBITMAP
	* @param    LPCTSTR pszResName --  BITMAP名称
	* @return   HBITMAP -- 成功返回BITMAP的句柄，失败返回0
	* Describe  
	*/    
	STDMETHOD_(HBITMAP,LoadBitmap)(THIS_ LPCTSTR pszResName) PURE;

	/**
	* LoadCursor
	* @brief    从资源中加载光标
	* @param    LPCTSTR pszResName --  光标名
	* @return   HCURSOR -- 成功返回光标的句柄，失败返回0
	* Describe  支持动画光标
	*/    
	STDMETHOD_(HCURSOR,LoadCursor)(THIS_ LPCTSTR pszResName) PURE;

	/**
	* LoadImage
	* @brief    从资源加载一个IBitmap对象
	* @param    LPCTSTR strType --  图片类型
	* @param    LPCTSTR pszResName --  图片名
	* @return   IBitmap * -- 成功返回一个IBitmap对象，失败返回0
	* Describe  如果没有定义strType，则根据name使用FindImageType自动查找匹配的类型
	*/    
	STDMETHOD_(IBitmap*, LoadImage)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) PURE;

	/**
	* LoadImgX
	* @brief    从资源中创建一个IImgX对象
	* @param    LPCTSTR strType --  图片类型
	* @param    LPCTSTR pszResName --  图片名
	* @return   IImgX   * -- 成功返回一个IImgX对象，失败返回0
	* Describe  
	*/    
	STDMETHOD_(IImgX*, LoadImgX)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) PURE;

	/**
	* GetRawBufferSize
	* @brief    获得资源数据大小
	* @param    LPCTSTR strType --  资源类型
	* @param    LPCTSTR pszResName --  资源名
	* @return   size_t -- 资源大小（byte)，失败返回0
	* Describe  
	*/    
	STDMETHOD_(size_t,GetRawBufferSize)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) PURE;

	/**
	* GetRawBuffer
	* @brief    获得资源内存块
	* @param    LPCTSTR strType --  资源类型
	* @param    LPCTSTR pszResName --  资源名
	* @param    LPVOID pBuf --  输出内存块
	* @param    size_t size --  内存大小
	* @return   BOOL -- true成功
	* Describe  应该先用GetRawBufferSize查询资源大小再分配足够空间
	*/    
	STDMETHOD_(BOOL,GetRawBuffer)(THIS_ LPCTSTR pszType,LPCTSTR pszResName,LPVOID pBuf,size_t size) PURE;


	/**
	* EnumResource
	* @brief    枚举资源
	* @param    EnumResCallback funEnumCB --  枚举使用的回调函数
	* @return   void
	* Describe  
	*/
	STDMETHOD_(void,EnumResource)(THIS_ EnumResCallback funEnumCB,LPARAM lp) PURE;
};

SNSEND

#endif//_SRESPROVIDERBASE_
