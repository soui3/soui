#pragma once

#include <interface/obj-ref-i.h>
#include <interface/smsgloop-i.h>

SNSBEGIN

/** 
* @struct    TIPID
* @brief     标识一个tooltip的ID
*
* Describe
*/
struct TIPID
{
	DWORD dwHi; //ID1, 用来保存一个SWND。
	DWORD dwLow;//ID2, 用来保存附加数据
};

/** 
* @struct    IToolTip
* @brief     tooltip对象接口
*
* Describe
*/

#undef INTERFACE
#define INTERFACE IToolTip
DECLARE_INTERFACE_(IToolTip,IMessageFilter)
{
	STDMETHOD_(BOOL,PreTranslateMessage)(THIS_ MSG* pMsg) PURE;

	/**
	* UpdateTip
	* @brief    更新当前的tooltip
	* @param    const TIPID & id --  tooltip的ID
	* @param    CRect rc --  tooltip的感应区
	* @param    LPCTSTR pszTip --  tooltip字符串
	* @param    int nScale -- 当前的缩放比例
	* @return   void 
	*
	* Describe  
	*/
	STDMETHOD_(void,UpdateTip)(THIS_ const TIPID *id, RECT rc,LPCTSTR pszTip, int nScale) PURE;

	/**
	* ClearTip
	* @brief    清除当前的tooltip
	* @return   void 
	*
	* Describe  
	*/
	STDMETHOD_(void,ClearTip)(THIS) PURE;

	/**
	* RelayEvent
	* @brief    鼠标消息的中继
	* @param    const MSG * pMsg --  鼠标消息
	* @return   void 
	*
	* Describe  
	*/
	STDMETHOD_(void,RelayEvent)(THIS_ const MSG *pMsg) PURE;
};

/** 
* @struct    IToolTipFactory
* @brief     tooltip类厂
*
* Describe
*/
#undef INTERFACE
#define INTERFACE IToolTipFactory
DECLARE_INTERFACE_(IToolTipFactory,IObjRef)
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
	* CreateToolTip
	* @brief    创建tooltip对象
	* @param    HWND hHost --  soui host
	* @return   IToolTip * 创建的tooltip对象
	*
	* Describe  不要将hHost做为tooltip的Owner，否则可能导致程序运行异常。
	*/
	STDMETHOD_(IToolTip *,CreateToolTip)(THIS_ HWND hHost) PURE;

	/**
	* DestroyToolTip
	* @brief    销毁tooltip对象
	* @param    IToolTip * pToolTip --  待销毁tooltip
	* @return   void 
	*
	* Describe  
	*/
	STDMETHOD_(void,DestroyToolTip)(THIS_ IToolTip *pToolTip) PURE;
};

SNSEND