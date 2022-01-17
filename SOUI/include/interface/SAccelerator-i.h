#pragma once
#include <utilities-def.h>

SNSBEGIN

enum{
	Mod_None = 0,
	Mod_Alt  = 1, 
	Mod_Ctrl = 2,
	Mod_Shift= 4,
};

#undef INTERFACE
#define INTERFACE IAccelerator
DECLARE_INTERFACE(IAccelerator)
{
	/**
	* GetModifier
	* @brief    获得加速键的修饰位
	* @return   WORD -- 加速键的修饰键
	* Describe  
	*/    
	STDMETHOD_(WORD,GetModifier)(THIS) SCONST PURE;

	/**
	* GetKey
	* @brief    获得加速键的主键
	* @return   WORD -- 加速键的主键
	* Describe  
	*/    
	STDMETHOD_(WORD,GetKey)(THIS) SCONST PURE;

	/**
	* GetAcc
	* @brief    获得加速键
	* @return   DWORD -- 加速键
	* Describe  
	*/    
	STDMETHOD_(DWORD,GetAcc)(THIS) SCONST PURE;
};

/**
* @struct     IAcceleratorTarget
* @brief      加速键按下的处理接口
* 
* Describe 想要注册键盘加速键的类需要实现本接口
*/
#undef INTERFACE
#define INTERFACE IAcceleratorTarget
DECLARE_INTERFACE(IAcceleratorTarget)
{
	/**
	* OnAcceleratorPressed
	* @brief    
	* @param    const CAccelerator & accelerator --  按下的加速键
	* @return   bool -- 加速键被处理返回true
	* Describe  
	*/    
	STDMETHOD_(BOOL,OnAcceleratorPressed)(THIS_ const IAccelerator* acc) PURE;
};

/**
* @struct     IAcceleratorMgr
* @brief      加速键管理接口
* 
* Describe
*/
#undef INTERFACE
#define INTERFACE IAcceleratorMgr
DECLARE_INTERFACE(IAcceleratorMgr)
{
	// Register a keyboard accelerator for the specified target. If multiple
	// targets are registered for an accelerator, a target registered later has
	// higher priority.
	// Note that we are currently limited to accelerators that are either:
	// - a key combination including Ctrl or Alt
	// - the escape key
	// - the enter key
	// - any F key (F1, F2, F3 ...)
	// - any browser specific keys (as available on special keyboards)
	STDMETHOD_(void,RegisterAccelerator)(THIS_ const IAccelerator* pAcc,
		IAcceleratorTarget* target) PURE;

	// Unregister the specified keyboard accelerator for the specified target.
	STDMETHOD_(void,UnregisterAccelerator)(THIS_ const IAccelerator* pAcc,
		IAcceleratorTarget* target) PURE;

	// Unregister all keyboard accelerator for the specified target.
	STDMETHOD_(void,UnregisterAccelerators)(THIS_ IAcceleratorTarget* target) PURE;
};

SNSEND