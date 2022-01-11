/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SCalendar.h
 * @brief      日历时间控件
 * @version    v1.0      
 * @author     soui      
 * @date       2014-05-25
 * 
 * Describe    此类完成日历控件
 */

#pragma once
#include "core/SWnd.h"

namespace SOUI
{
    /**
     * @class      SCalendarCore
     * @brief      日历核心类
     * 
     * Describe    此类是日历的核心类 大部分函数都是静态函数
     */
    class SOUI_EXP SCalendarCore
    {
    public:
        /**
         * SCalendarCore::IsLeapYear
         * @brief    判断闰年
         * @param    WORD wYear -- 待判断的公历年份
		 * @param	 BOOL &bLeapYear -- 【输出参数】TRUE：闰年，FALSE：平年
         * @return   TRUE -- 成功  FALSE -- 失败
         *
         * Describe  判断是否是闰年
					 注意：有效范围是（1600年到6999年）
         */        
        static BOOL IsLeapYear(WORD wYear,BOOL &bLeapYear);

		/**
         * SCalendarCore::GetDaysNumInYear
         * @brief    计算日期在年内的序数（序数从0开始，即0为元旦）
         * @param    WORD wYear -- 公历年
		 * @param	 WORD wMonth -- 公历月
		 * @param	 WORD wDay -- 公历日
		 * @param	 WORD &wDays -- 【输出参数】年内序数
         * @return   TRUE -- 成功  FALSE -- 失败
         *
         * Describe  输入日期，计算该日期在这一年内的序数，序数从0开始
					 注意：有效范围是（START_YEAR年到END_YEAR-1年）
         */ 
		static BOOL GetDaysNumInYear(WORD wYear, WORD wMonth, WORD wDay,WORD &wDays);

		/**
         * SCalendarCore::GetDateFromDays
         * @brief    从年内序数计算月、日
         * @param    WORD wYear -- 公历年
		 * @param	 WORD wDays -- 年内序数
		 * @param	 WORD &wMonth -- 【输出参数】公历月
		 * @param	 WORD &wDay -- 【输出参数】公历日
         * @return   TRUE -- 成功  FALSE -- 失败
         *
         * Describe  输入年份以及年内序数，计算出月和日
					 注意：有效范围是（START_YEAR年到END_YEAR-1年
         */ 
		static BOOL GetDateFromDays(WORD wYear, WORD wDays, WORD &wMonth, WORD &wDay);

        /**
         * SCalendarCore::GetDayOfWeek
         * @brief    返回星期几
         * @param    WORD wYear -- 公历年
         * @param    WORD wMonth -- 公历月
         * @param    WORD wDay -- 公历日
         * @return   返回:0,1,2,3,4,5,6分别对应日、一、二、三、四、五、六，-1表示日期错误或超出范围
         *
         * Describe  输入年月日返回星期几 
         *           注意:有效范围是(START_YEAR年1月1日 --- END_YEAR-1年12月31日)
         */
		static short GetDayOfWeek(WORD wYear,WORD wMonth,WORD wDay);

        /**
         * SCalendarCore::GetDaysOfMonth
         * @brief    返回指定月份的天数
         * @param    WORD wYear -- 公历年
         * @param    WORD wMonth -- 公历月
         * @return   返回该月的天数，返回0表示输入年月有误
         *
         * Describe  输入年月返回本月的天数 
         *           注意:有效范围是(START_YEAR年1月 --- END_YEAR-1年12月)
         */
        static WORD GetDaysOfMonth(WORD wYear,WORD wMonth);


		/**
         * SCalendarCore::DateCheck
         * @brief    检验年、月、日的合法性
         * @param    WORD wYear  -- 公历年 
         * @param    WORD wMonth  -- 公历月 
         * @param    WORD wDay  -- 公历日 
         * @return	 FALSE-失败，TRUE-成功
         *
         * Describe  输入公历日期，检查日期的合法性
         *           注意:有效范围是(START_YEAR年1月1日---END_YEAR-1年12月31日)
         */
		static BOOL DateCheck(WORD wYear,WORD wMonth,WORD wDay);

        /**
         * SCalendarCore::FormatYear
         * @brief    格式化年份显示型式
         * @param    WORD iYear -- 年
         *
         * Describe  指定阴历年返回采用干支纪年法
         */        
        static SStringT FormatYear(WORD  iYear);

        /**
         * SCalendarCore::FormatMonth
         * @brief    格式化月份显示型式
         * @param    WORD iMonth -- 月
         */        
        static SStringT FormatMonth(WORD iMonth);
        
        /**
         * SCalendarCore::FormatDay
         * @brief    格式化日期显示型式
         * @param    WORD iDay -- 日
         */
        static SStringT FormatDay(WORD wiDay);

    };

	// 按钮 宏定义
#define HIT_NULL				-1				// 无
#define HIT_LEFT				-10			// 上一个月 按钮
#define HIT_RIGHT				-11			// 下一个月 按钮
#define HIT_YEAR				-12			//  年月  还没用到
#define HIT_YEAR_1			-13
#define HIT_YEAR_2			-14
#define HIT_YEAR_3			-15
#define HIT_TODAY				42				//  今天 

	//日历显示的状态
#define SHOW_MONTH		-101	//显示月份
#define SHOW_YEAR		-102	//显示年份
#define SHOW_YEAR_DECADE	-103	//显示近10年
#define SHOW_YEAR_CENTURY	-104	//显示近100年

	class SOUI_EXP SCalendar : public SWindow
	{
		SOUI_CLASS_NAME(SWindow,L"calendar")
	public:
		SCalendar(WORD iYeay, WORD iMonth, WORD iDay);
		SCalendar();
		~SCalendar();
		WORD GetYear();
		WORD GetMonth();
		WORD GetDay();
		void GetDate(WORD &iYear, WORD &iMonth, WORD &iDay);
		BOOL SetDate(WORD iYear, WORD iMonth, WORD iDay, int nBtnType = HIT_NULL, bool bNotify = false);

	protected:
		virtual HRESULT OnLanguageChanged();
	protected:
		void Init();
		void OnPaint(IRenderTarget *pRT);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnMouseMove(UINT nFlags, CPoint pt);
		void OnMouseLeave();
		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

		SOUI_ATTRS_BEGIN()
			ATTR_LAYOUTSIZE(L"yearHeight", m_nYearMonthHeight, TRUE)
			ATTR_LAYOUTSIZE(L"weekHeight", m_nWeekHeight, TRUE)
			ATTR_LAYOUTSIZE(L"todayHeight", m_nFooterHeight, TRUE)
			ATTR_COLOR(L"colorSelText", m_crSelText, TRUE)
			ATTR_COLOR(L"colorOtherText", m_crOtherDayText, TRUE)
			ATTR_COLOR(L"colorSelBg", m_crSelDayBack, TRUE)
			ATTR_COLOR(L"colorHoverText", m_crHoverText, TRUE)
			ATTR_SKIN(L"prevSkin", m_pSkinPrev, TRUE)
			ATTR_SKIN(L"nextSkin", m_pSkinNext, TRUE)
			ATTR_SKIN(L"daySkin", m_pSkinDay, TRUE)
			ATTR_SKIN(L"weekSkin", m_pSkinWeek,TRUE)
			ATTR_I18NSTRT(L"textSunday", m_strWeek[0], TRUE)
			ATTR_I18NSTRT(L"textMonday", m_strWeek[1], TRUE)
			ATTR_I18NSTRT(L"textTuesday", m_strWeek[2], TRUE)
			ATTR_I18NSTRT(L"textWednesday", m_strWeek[3], TRUE)
			ATTR_I18NSTRT(L"textThursday", m_strWeek[4], TRUE)
			ATTR_I18NSTRT(L"textFriday", m_strWeek[5], TRUE)
			ATTR_I18NSTRT(L"textSaturday", m_strWeek[6], TRUE)
		SOUI_ATTRS_END()

	protected:
		int OnCreate(LPVOID);
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_CREATE(OnCreate)
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_MOUSELEAVE(OnMouseLeave)
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
			SOUI_MSG_MAP_END()
	protected:
		// 定位 小于0  是 年月 3个按钮 暂时 没完成   0 - 41  天    42 today按钮
		int HitTest(const CPoint& pt);
		// 绘制日历头 中上部分
		void DrawYearMonth(IRenderTarget* pRT, const CRect& rect);
		void DrawWeek(IRenderTarget* pRT, const CRect& rect);
		void DrawDay(IRenderTarget* pRT, CRect& rcDay, int nItem);
		void DrawToday(IRenderTarget* pRT, CRect& rcDay);
		void GetItemRect(int nItem, CRect& rcItem);
		void SetLastMonth();
		void SetNextMonth();
		//扩展功能
		void SetLastYear();
		void SetNextYear();
		void SetLastYearDecade();
		void SetNextYearDecade();
		void SetLastYearCentury();
		void SetNextYearCentury();
		void SetYearMonth(int iYear, int iMonth);
		void OnPaintMonth(IRenderTarget *pRT);
		void OnPaintYearDecadeCentury(IRenderTarget *pRT);
		void DrawYearDecadeCentury(IRenderTarget *pRT, const CRect& rect, int nItem);
	public:
		void SetShowType(int showType);
		void SetYearDecadeCentury();

	protected:
		SLayoutSize			m_nYearMonthHeight;   //年月
		SLayoutSize			m_nWeekHeight;				//星期高度
		SLayoutSize			m_nFooterHeight;

		COLORREF				m_crSelText;
		COLORREF				m_crOtherDayText;			//其他 天 的 字体颜色 
		COLORREF				m_crSelDayBack;			// 选中 的 天 背颜色
		COLORREF				m_crHoverText;

		SAutoRefPtr<ISkinObj>	m_pSkinPrev;
		SAutoRefPtr<ISkinObj>	m_pSkinNext;
		SAutoRefPtr<ISkinObj>	m_pSkinDay;					// 天 皮肤 
		SAutoRefPtr<ISkinObj>	m_pSkinWeek;
		STrText					m_strWeek[7];/**< 表头文本 */

		struct wDayInfo
		{
			WORD			iDay;				// 日历 天 
			int					nType;			// -1 前一个月 0 当月 1 下一个月
		};
		wDayInfo				m_arrDays[42];
		CRect						m_rcDays;
		CRect						m_rcToday;

		int							m_nSelItem;
		int							m_nHoverItem;

		WORD					m_iYear;
		WORD					m_iMonth;
		SYSTEMTIME			m_Today;

		int					m_showType;			//日历显示状态
		int					m_showTypeLbdown;	//鼠标按下时，日历的显示状态
		struct wMonthOrYearInfo
		{
			WORD			iMonthOrYear;	//日历 月-年-年代-世纪
			int				nType;			//-1前一 0 当前 1后一
		};
		wMonthOrYearInfo m_arrMonthOrYear[12];
	};
}//end of namespace

