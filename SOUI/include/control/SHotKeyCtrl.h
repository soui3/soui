/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SHotKeyCtrl.h
 * @brief      热键控件
 * @version    v1.0      
 * @author     soui      
 * @date       2014-07-02
 * 
 * Describe    热键控件
 */
#pragma once
#include "core/SWnd.h"
#include "core/SAccelerator.h"

namespace SOUI
{
  /**
   * @class      SHotKeyCtrl
   * @brief      热键控件
   * 
   * Describe    热键控件
   */
  class SOUI_EXP SHotKeyCtrl 
      : public SWindow
      , public SAccelerator
  {
      SOUI_CLASS_NAME(SHotKeyCtrl, L"hotkey")
	  enum{
		Mod_SC = Mod_Shift|Mod_Ctrl,
		Mod_SA = Mod_Shift|Mod_Alt,
		Mod_CA = Mod_Ctrl|Mod_Alt,
		Mod_SCA = Mod_Shift|Mod_Ctrl|Mod_Alt,
	  };
  public:
      /**
       * SHotKeyCtrl::SHotKeyCtrl
       * @brief    构造函数
       *
       * Describe  构造函数  
       */
      SHotKeyCtrl(void);
      /**
       * SHotKeyCtrl::~SHotKeyCtrl
       * @brief    析构函数
       *
       * Describe  析构函数  
       */
      virtual ~SHotKeyCtrl(void);

      /**
       * SHotKeyCtrl::SetRule
       * @brief    设置规则
       * @param    WORD wInvalidComp --
       * @param    WORD wModifier  --  
       *
       * Describe  设置规则  
       */
      void SetRule(WORD wInvalidComp, WORD wModifier);
      
      /**
       * SHotKeyCtrl::SetHotKey
       * @brief    设置热键
       * @param    WORD vKey --
       * @param    WORD wModifiers  --  
       *
       * Describe  设置热键  
       */
      void SetHotKey(WORD vKey, WORD wModifiers);
      
      /**
       * SHotKeyCtrl::GetHotKey
       * @brief    获取热键
       * @param    WORD &vKey --
       * @param    WORD wModifiers  --  
       *
       * Describe  获取热键  
       */
      void GetHotKey(WORD & vKey,WORD &wModifers);

	  
  public:
	  virtual SStringT GetWindowText(BOOL bRawText = FALSE);
  protected:
      /**
       * SHotKeyCtrl::OnGetDlgCode
       * @brief    获取窗口消息码
       *
       * Describe  消息响应函数  
       */  
	  virtual UINT WINAPI OnGetDlgCode () const
      {
          return (SC_WANTALLKEYS|SC_WANTSYSKEY) & (~SC_WANTTAB);
      }

      /**
       * SHotKeyCtrl::OnLButtonDown
       * @brief    左键按下事件
       * @param    UINT nFlags -- 标志
       * @param    CPoint point -- 鼠标坐标
       *
       * Describe  此函数是消息响应函数
       */
      void OnLButtonDown(UINT nFlags,CPoint pt);

      /**
       * SHotKeyCtrl::OnPaint
       * @brief    绘制消息
       * @param    IRenderTarget *pRT -- 渲染目标
       * 
       * Describe  此函数是消息响应函数
       */
      void OnPaint(IRenderTarget *pRT);

      /**
       * SHotKeyCtrl::OnSetFocus
       * @brief    设置焦点
       * 
       * Describe  此函数是消息响应函数
       */
      void OnSetFocus(SWND wndOld);

      /**
       * SHotKeyCtrl::OnKillFocus
       * @brief    失去焦点
       * 
       * Describe  此函数是消息响应函数
       */
      void OnKillFocus(SWND wndFocus);

      /**
       * SHotKeyCtrl::OnKeyDown
       * @brief    键盘按下事件
       * @param    UINT nChar -- 按键对应的码值 
       * @param    UINT nRepCnt -- 重复次数
       * @param    UINT nFlags -- 标志
       * 
       * Describe  此函数是消息响应函数
       */ 
      void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
      /**
       * SHotKeyCtrl::OnKeyUp
       * @brief    键盘抬起事件
       * @param    UINT nChar -- 按键对应的码值 
       * @param    UINT nRepCnt -- 重复次数
       * @param    UINT nFlags -- 标志
       * 
       * Describe  此函数是消息响应函数
       */ 
      void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

      /**
       * SHotKeyCtrl::OnSysKeyDown
       * @brief    系统按键按下事件
       * @param    UINT nChar -- 按键对应的码值 
       * @param    UINT nRepCnt -- 重复次数
       * @param    UINT nFlags -- 标志
       * 
       * Describe  此函数是消息响应函数
       */ 
      void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

      /**
       * SHotKeyCtrl::OnSysKeyUp
       * @brief    系统按键抬起事件
       * @param    UINT nChar -- 按键对应的码值 
       * @param    UINT nRepCnt -- 重复次数
       * @param    UINT nFlags -- 标志
       * 
       * Describe  此函数是消息响应函数
       */ 
      void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

      void UpdateModifier();

      void UpdateCaret(IRenderTarget *pRT);

	  UINT GetTextAlign();

	protected:
		HRESULT OnAttrInvalidComb(const SStringW & value, BOOL bLoading);
		HRESULT OnAttrInvalidModifier(const SStringW & value, BOOL bLoading);
		HRESULT OnAttrHotKey(const SStringW & value, BOOL bLoading);
      SOUI_ATTRS_BEGIN()
          ATTR_CUSTOM(L"invalidComb",OnAttrInvalidComb)
          ATTR_CUSTOM(L"invalidModifier",OnAttrInvalidModifier)
          ATTR_CUSTOM(L"hotKey",OnAttrHotKey)
      SOUI_ATTRS_END()

      SOUI_MSG_MAP_BEGIN()
          MSG_WM_PAINT_EX(OnPaint)
          MSG_WM_LBUTTONDOWN(OnLButtonDown)
          MSG_WM_SETFOCUS_EX(OnSetFocus)
          MSG_WM_KILLFOCUS_EX(OnKillFocus)
          MSG_WM_KEYDOWN(OnKeyDown)
          MSG_WM_KEYUP(OnKeyUp)
          MSG_WM_SYSKEYDOWN(OnSysKeyDown)
          MSG_WM_SYSKEYUP(OnSysKeyUp)
      SOUI_MSG_MAP_END()
      WORD     m_wInvalidComb;     /**< 无效的组合键 */
      WORD     m_wInvalidModifier; /**< 对无效组合键的替换方案,默认方案 */

      BOOL    m_bInSetting;        /**< 正在设置中 */
  };
}
//namespace SOUI
