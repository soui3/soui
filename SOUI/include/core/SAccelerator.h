/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       Accelerator.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI加速键管理模块
*/

#pragma once

#include <interface/SAccelerator-i.h>

namespace SOUI
{

    /**
    * @class      CAccelerator
    * @brief      加速键映射
    * 
    * Describe
    */
	class SOUI_EXP SAccelerator : public IAccelerator
    {
    public:

        /**
         * CAccelerator
         * @brief    构造函数
         * @param    DWORD dwAccel --  加速键值
         * Describe  
         */    
        SAccelerator(DWORD dwAccel);

        /**
         * CAccelerator
         * @brief    构造函数
         * @param    UINT vKey --  键值
         * @param    bool bCtrl --  与Ctrl组合
         * @param    bool bAlt --  与Alt组合
         * @param    bool bShift --  与Shilft组合
         * Describe  
         */    
        SAccelerator(UINT vKey=0,bool bCtrl=false,bool bAlt=false,bool bShift=false);

        /**
         * ~CAccelerator
         * @brief    析构函数
         * Describe  
         */    
        ~SAccelerator(void);


        /**
         * FormatHotkey
         * @brief    获得当前加速键的字符串形式
         * @return   SOUI::SStringT -- 加速键的字符串形式
         * Describe  
         */    
        SStringT FormatHotkey();

		STDMETHOD_(WORD,GetModifier)(THIS) SCONST OVERRIDE;

    
		STDMETHOD_(WORD,GetKey)(THIS) SCONST OVERRIDE;

		STDMETHOD_(DWORD,GetAcc)(THIS) SCONST OVERRIDE;

		static WORD VkFromString(LPCTSTR pszKey);
		/**
		* GetKeyName
		* @brief    将键盘值转换为对应的字符串
		* @param    WORD vk --  键盘值
		* @return   SOUI::SStringT -- 键名
		* Describe
		*/
		static SStringT GetKeyName(WORD vk);

		static SStringT FormatAccelKey(DWORD dwAccel);
        /**
         * TranslateAccelKey
         * @brief    解析与字符串对应的加速键值
         * @param    LPCTSTR pszKeyName --  定义加速键的字符串
         * @return   DWORD -- 加速键值
         * Describe  
         */    
        static DWORD TranslateAccelKey(LPCTSTR pszKeyName);
    protected:
        WORD     m_wModifier;
        WORD    m_wVK;
    };



}//end of namespace SOUI
