/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 *
 * @file       SCalendar.cpp
 * @brief      SCalendarCore以及SCalendar类源文件
 * @version    v1.0
 * @author     soui
 * @date       2014-05-25
 *
 * Describe  时间控件相关函数实现
 */
#include "souistd.h"
#include "control/SCalendar.h"
#include "helper/STime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SCalendarCore

namespace SOUI{

extern WORD gLunarMonthDay[];
extern BYTE gLunarMonth[];
extern BYTE gLunarHolDay[];

#define REFTYPE_SUJIU 1		//数九
#define REFTYPE_MEIYU 2		//梅雨
#define REFTYPE_SANFU 3		//三伏

#define TITLE_HEIGHT 20.f
#define FOOTER_HEIGHT 20.f

//两个子控件的名字
#define NAME_BTN_TODAY    "btn_today"
#define NAME_LABEL_TODAY "label_today"

WORD const DayOrdinal[13]={0,31,59,90,120,151,181,212,243,273,304,334,365};
WORD const DayOrdinalLeap[13]={0,31,60,91,121,152,182,213,244,274,305,335,366};

BOOL SCalendarCore::IsLeapYear(WORD wYear,BOOL &bLeapYear)
{
	if (wYear<1600||wYear>=7000)//压缩算法规定了的年份区间（提取器只能导出此区间的数据，Lunar.dll支持-6000到20000（不含20000）之间的年份）
	{
		return FALSE;
	}
	if (wYear%4==0&&wYear%100!=0||wYear%400==0)//判断闰年的条件
	{
		bLeapYear=TRUE;
	}else
	{
		bLeapYear=FALSE;
	}
	return TRUE;
}

BOOL SCalendarCore::GetDaysNumInYear(WORD wYear, WORD wMonth, WORD wDay,WORD &wDays)
{
	//从日期算出距离元旦的天数
	if (DateCheck(wYear,wMonth,wDay)==0)//对输入的日期进行检查
	{
		return FALSE;
	}
	BOOL bLeapYear=FALSE;
	if (!IsLeapYear(wYear,bLeapYear))
	{
		return FALSE;
	}
	if (bLeapYear==TRUE)
	{
		wDays=DayOrdinalLeap[wMonth-1]+wDay-1;//元旦为序数0，因此减1
	}else
	{
		wDays=DayOrdinal[wMonth-1]+wDay-1;
	}
	return TRUE;
}

BOOL SCalendarCore::GetDateFromDays(WORD wYear, WORD wDays, WORD &wMonth, WORD &wDay)
{
	//从距离元旦的天数算出日期
	if (wDays<1)
	{
		return FALSE;
	}
	BOOL bLeapYear=FALSE;
	if (!IsLeapYear(wYear,bLeapYear))
	{
		return FALSE;
	}
	if (bLeapYear==TRUE)
	{
		if (wDays>366)//超出了该年的总天数
		{
			return FALSE;
		}
	}else
	{
		if (wDays>365)
		{
			return FALSE;
		}
	}
	wMonth=0;
	int i=0;
	for (i=0;i<12;i++)
	{
		if (bLeapYear==TRUE)
		{
			if (wDays>=DayOrdinalLeap[i]&&wDays<DayOrdinalLeap[i+1])//找出月份
			{
				wMonth=i+1;
				wDay=wDays-DayOrdinalLeap[i];//计算出“日”
				break;
			}
		}else
		{
			if (wDays>=DayOrdinal[i]&&wDays<DayOrdinal[i+1])
			{
				wMonth=i+1;
				wDay=wDays-DayOrdinal[i];
				break;
			}
		}
	}
	return TRUE;
}

BOOL SCalendarCore::DateCheck(WORD wYear,WORD wMonth,WORD wDay)
{
	if (wMonth > 12 || wMonth < 1)
		return FALSE;

	if (wDay < 1 || wDay > 31)
		return FALSE;

	if (wMonth == 4 || wMonth == 6 || wMonth == 9 || wMonth == 11)
	{
		if (wDay > 30)
		{
			return FALSE;
		}
	}
	else if (wMonth == 2)
	{
		BOOL bLeapYear = FALSE;
		IsLeapYear(wYear, bLeapYear);
		if (bLeapYear == FALSE)
		{
			if (wDay > 28)
			{
				return FALSE;
			}
		}else
		{
			if (wDay > 29)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


short SCalendarCore::GetDayOfWeek(WORD wYear,WORD wMonth,WORD wDay)
{
	WORD uDayOrd=0;
	if (GetDaysNumInYear(wYear,wMonth,wDay,uDayOrd)==0)
	{
		return -1;
	}
	unsigned int DayofWeek=0;
	uDayOrd++;//一年中的第几天，因为GetDaysNumInYear所得到的是索引，因此要加一
	wYear--;
	DayofWeek=(wYear+wYear/4-wYear/100+wYear/400+uDayOrd)%7;//这个只是算星期的通用公式
	return DayofWeek;
}

WORD SCalendarCore::GetDaysOfMonth(WORD wYear,WORD wMonth)
{
	if (wMonth==12)
	{
		return 31;//这里为了简便，判断12月就直接返回
	}
	WORD days1=0,days2=0;
	WORD ret=0;
	ret=GetDaysNumInYear(wYear,wMonth,1,days1);//本月1日在年内的序数
	if (ret==0)
	{
		return ret; 
	}
	wMonth++;
	ret=GetDaysNumInYear(wYear,wMonth,1,days2);//下个月1日的年内序数
	if (ret==0)
	{
		return ret; 
	}
	ret=days2-days1;//下个月1日的序数减本月1日的序数
	return ret;
}

SStringT SCalendarCore::FormatYear(WORD  iYear)
{
    return SStringT().Format(_T("%d"),iYear);
}

SStringT SCalendarCore::FormatMonth(WORD iMonth)
{
    TCHAR szText[][5]={_T("一"),_T("二"),_T("三"),_T("四"),_T("五"),_T("六"),_T("七"),_T("八"),_T("九"),_T("十"),_T("十一"),_T("十二")};
    SASSERT(iMonth<=12 && iMonth>=1);
    return szText[iMonth-1];
}

SStringT SCalendarCore::FormatDay(WORD iDay)
{
    return SStringT().Format(_T("%d"),iDay);
}

SCalendar::SCalendar(WORD iYeay, WORD iMonth, WORD iDay)
{
	Init();
	if (!SetDate(iYeay, iMonth, iDay))
	{
		//STime tm = STime::GetCurrentTime();
		SetDate(m_Today.wYear, m_Today.wMonth, m_Today.wDay);
	}
}

SCalendar::SCalendar()
{
	Init();
	STime tm = STime::GetCurrentTime();
	SetDate(tm.GetYear(), tm.GetMonth(), tm.GetDay());
}

SCalendar::~SCalendar()
{

}

HRESULT SCalendar::OnLanguageChanged()
{
	__super::OnLanguageChanged();
	for (UINT i = 0; i < 7; i++)
	{
		m_strWeek[i].TranslateText();
	}
	return S_OK;
}

void SCalendar::Init()
{
	m_pSkinPrev = GETBUILTINSKIN(SKIN_SYS_BTN_PREV);
	m_pSkinNext = GETBUILTINSKIN(SKIN_SYS_BTN_NEXT);

	m_bFocusable = TRUE;
	GetLocalTime(&m_Today);

	m_evtSet.addEvent(EVENTID(EventCalendarExChanged));

	m_nYearMonthHeight.setSize(30.f, SLayoutSize::dp);
	m_nWeekHeight.setSize(20.f, SLayoutSize::dp);
	m_nFooterHeight.setSize(20.f, SLayoutSize::dp);

	m_crSelText = RGBA(0xFF, 0xFF, 0xFF, 255);
	m_crSelDayBack = RGBA(0x0, 0x7A, 0xCC, 255);
	m_crOtherDayText = RGBA(0xA0, 0xA0, 0xA0, 255);

	// 设置 默认 的 背景色  和 悬浮 字体颜色
	GetStyle().m_crBg = RGBA(255, 255, 255, 255);
	m_crHoverText = RGBA(0, 0x7A, 0xCC, 255);

	m_pSkinDay = NULL;
	m_pSkinWeek = NULL;
	m_pSkinPrev = NULL;
	m_pSkinNext = NULL;
	m_showType = SHOW_MONTH;

	TCHAR sztext[][3] = { _T("日"),_T("一"),_T("二"),_T("三"),_T("四"),_T("五"),_T("六") };
	for (int i = 0; i < 7; i++)
	{
		m_strWeek[i].SetText(sztext[i]);
	}
}

int SCalendar::OnCreate(LPVOID)
{
	int nRet = SWindow::OnCreate(NULL);
	if (nRet != 0)
		return nRet;
	for (int i = 0; i < 7; i++)
	{
		m_strWeek[i].SetCtxProvider(this);
		m_strWeek[i].TranslateText();
	}
	return nRet;
}

WORD SCalendar::GetYear()
{
	return m_iYear;
}

WORD SCalendar::GetMonth()
{
	return m_iMonth;
}

WORD SCalendar::GetDay()
{
	if (m_nSelItem < 0 || m_nSelItem >= 42)
		return 1;

	return m_arrDays[m_nSelItem].iDay;
}

void SCalendar::GetDate(WORD &iYear, WORD &iMonth, WORD &iDay)
{
	iYear = m_iYear;
	iMonth = m_iMonth;
	iDay = GetDay();
}

BOOL SCalendar::SetDate(WORD iYear, WORD iMonth, WORD iDay, int nBtnType, bool bNotify)
{
	short iWeek = SCalendarCore::GetDayOfWeek(iYear, iMonth, 1);		// 计算出 当月 1号 是星期几
	if (iWeek < 0) return FALSE;

	WORD nDayCount = SCalendarCore::GetDaysOfMonth(iYear, iMonth);

	if (iWeek > 0)				// 如果 不是星期天 先计算 上个月 的最后几天
	{
		WORD nFrontDayCount = 0;
		if (1 == iMonth)
			nFrontDayCount = SCalendarCore::GetDaysOfMonth(iYear - 1, 12);
		else
			nFrontDayCount = SCalendarCore::GetDaysOfMonth(iYear, iMonth - 1);


		for (int i = 0; i < iWeek; ++i)
		{
			m_arrDays[i].nType = -1;
			m_arrDays[i].iDay = nFrontDayCount - iWeek + i + 1;
		}
	}


	// 计算 当月 的 天数 位置
	for (int i = 0; i < nDayCount; ++i)
	{
		m_arrDays[iWeek + i].nType = 0;
		m_arrDays[iWeek + i].iDay = i + 1;
	}

	int nNextDay = 1;
	for (int i = iWeek + nDayCount; i < 42; ++i)
	{
		m_arrDays[i].nType = 1;
		m_arrDays[i].iDay = nNextDay++;
	}
	m_iYear = iYear;
	m_iMonth = iMonth;

	m_nHoverItem = HIT_NULL;
	m_nSelItem = iWeek % 7 + iDay - 1;
	Invalidate();

	if (bNotify)
	{
		EventCalendarExChanged evt(this);
		evt.iNewDay = iDay;
		evt.iNewMonth = m_iMonth;
		evt.iNewYear = m_iYear;
		evt.nBtnType = nBtnType;
		FireEvent(evt);
	}

	return TRUE;

}
void SCalendar::SetShowType(int showType)
{
	m_showType = showType;
}

void SCalendar::SetYearDecadeCentury()
{
	if (m_showType == SHOW_YEAR)
	{
		for (WORD i = 0; i < 12; i++)
		{
			m_arrMonthOrYear[i].iMonthOrYear = i + 1;
			m_arrMonthOrYear[i].nType = 0;
		}
		//计算m_nSelItem
		m_nSelItem = m_iMonth - 1;
	}
	if (m_showType == SHOW_YEAR_DECADE)
	{
		//年代头年份
		int decadeFirstYear = m_iYear - m_iYear % 10;
		//计算 显示

		m_arrMonthOrYear[0].iMonthOrYear = decadeFirstYear - 1;
		m_arrMonthOrYear[0].nType = -1;

		for (WORD i = 1; i <= 10; i++)
		{
			m_arrMonthOrYear[i].iMonthOrYear = decadeFirstYear + i - 1;
			m_arrMonthOrYear[i].nType = 0;
		}

		m_arrMonthOrYear[11].iMonthOrYear = decadeFirstYear + 10;
		m_arrMonthOrYear[11].nType = -1;

		//计算m_nSelItem
		m_nSelItem = m_iYear % 10 + 1;
	}
	else if (m_showType == SHOW_YEAR_CENTURY)
	{
		//世纪头 年份 2019 -19 = 2000
		int centiryFirstYear = m_iYear - m_iYear % 100;
		//计算 显示

		m_arrMonthOrYear[0].iMonthOrYear = centiryFirstYear - 10;
		m_arrMonthOrYear[0].nType = -1;

		for (WORD i = 1; i <= 10; i++)
		{
			m_arrMonthOrYear[i].iMonthOrYear = centiryFirstYear + (i - 1) * 10;
			m_arrMonthOrYear[i].nType = 0;
		}

		m_arrMonthOrYear[11].iMonthOrYear = centiryFirstYear + 100;
		m_arrMonthOrYear[11].nType = -1;

		//计算m_nSelItem
		m_nSelItem = m_iYear % 100 / 10 + 1;
	}
}

void SCalendar::SetLastMonth()
{
	if (1 == m_iMonth)
	{
		m_iMonth = 12;
		--m_iYear;
	}
	else
		--m_iMonth;

	SetDate(m_iYear, m_iMonth, 1, HIT_LEFT, true);
}

void SCalendar::SetNextMonth()
{
	if (12 == m_iMonth)
	{
		m_iMonth = 1;
		++m_iYear;
	}
	else
		++m_iMonth;

	SetDate(m_iYear, m_iMonth, 1, HIT_RIGHT, true);
}

void SCalendar::SetLastYear()
{
	--m_iYear;
	SetYearDecadeCentury();
}

void SCalendar::SetNextYear()
{
	++m_iYear;
	SetYearDecadeCentury();
}
void SCalendar::SetLastYearDecade()
{
	m_iYear -= 10;
	SetYearDecadeCentury();
}
void SCalendar::SetNextYearDecade()
{
	m_iYear += 10;
	SetYearDecadeCentury();
}
void SCalendar::SetLastYearCentury()
{
	if (m_iYear <= 1600)
	{
		return;
	}
	m_iYear -= 100;
	SetYearDecadeCentury();
}
void SCalendar::SetNextYearCentury()
{
	m_iYear += 100;
	SetYearDecadeCentury();
}

void SCalendar::SetYearMonth(int iYear, int iMonth)
{
	m_iYear = iYear;
	m_iMonth = iMonth;
	SetYearDecadeCentury();
}

void SCalendar::DrawYearMonth(IRenderTarget* pRT, const CRect& rect)
{
	if (rect.IsRectEmpty())
		return;
	int nHeight = m_nYearMonthHeight.toPixelSize(GetScale());

	// 绘制 上一个 按钮
	DWORD dwPrevState = 0;
	if (HIT_LEFT == m_nHoverItem)
	{
		dwPrevState = 1;
	}

	if (m_pSkinPrev)
	{
		SIZE si = m_pSkinPrev->GetSkinSize();
		int n = (nHeight - si.cy) / 2;
		CRect rcItem = rect;
		rcItem.left += n;
		rcItem.right = rcItem.left + si.cx;
		rcItem.top += n;
		rcItem.bottom = rcItem.top + si.cy;
		m_pSkinPrev->DrawByIndex(pRT, rcItem, dwPrevState);
	}

	// 绘制 年月
	COLORREF crText = pRT->GetTextColor();
	if (HIT_YEAR == m_nHoverItem)
	{
		pRT->SetTextColor(m_crHoverText);
	}
	else
		pRT->SetTextColor(crText);

	SStringT szYearMonth;
	if (m_showType == SHOW_MONTH)
	{
		szYearMonth.Format(_T("%04d年%d月"), m_iYear, m_iMonth);
	}
	else if (m_showType == SHOW_YEAR)
	{
		szYearMonth.Format(_T("%04d年"), m_iYear);
	}
	else if (m_showType == SHOW_YEAR_DECADE)
	{
		//年代头年份
		int decadeFirstYear = m_iYear - m_iYear % 10;
		szYearMonth.Format(_T("%04d-%04d"), decadeFirstYear, decadeFirstYear + 9);
	}
	else if (m_showType == SHOW_YEAR_CENTURY)
	{
		//世纪头年份
		int centuryFirstYear = m_iYear - m_iYear % 100;
		szYearMonth.Format(_T("%04d-%04d"), centuryFirstYear, centuryFirstYear + 99);
	}
	CRect rc = rect;
	pRT->DrawText(szYearMonth, -1, rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

	pRT->SetTextColor(crText);

	// 绘制 下一个 按钮
	DWORD dwNextState = 0;
	if (HIT_RIGHT == m_nHoverItem)
	{
		dwNextState = 1;
	}

	if (m_pSkinNext)
	{
		SIZE si = m_pSkinNext->GetSkinSize();
		int n = (nHeight - si.cy) / 2;
		CRect rcItem = rect;
		rcItem.right -= n;
		rcItem.left = rcItem.right - si.cx;
		rcItem.top += n;
		rcItem.bottom = rcItem.top + si.cy;

		m_pSkinNext->DrawByIndex(pRT, rcItem, dwNextState);
	}
}

void SCalendar::DrawWeek(IRenderTarget* pRT, const CRect& rect)
{
	if (rect.IsRectEmpty())
		return;
	if (m_pSkinWeek)
	{
		m_pSkinWeek->DrawByIndex(pRT, rect, 0);
	}
	int nWid = rect.Width() / 7;
	CRect rcItem = rect;
	rcItem.right = rcItem.left + nWid;

	COLORREF crTxt = pRT->GetTextColor();
	for (int i = 0; i < 7; i++)
	{
		pRT->SetTextColor(crTxt);

		pRT->DrawText(m_strWeek[i].GetText(FALSE), m_strWeek[i].GetText(FALSE).GetLength(), rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		rcItem.OffsetRect(nWid, 0);
	}
	pRT->SetTextColor(crTxt);
}

void SCalendar::DrawDay(IRenderTarget* pRT, CRect& rcDay, int nItem)
{
	if (nItem < 0 || nItem >= 42)
		return;

	wDayInfo& dayInfo = m_arrDays[nItem];

	COLORREF crText = pRT->GetTextColor();
	CPoint ptRound(1, 1);

	// 

	if (0 != dayInfo.nType)
	{
		pRT->SetTextColor(m_crOtherDayText);
	}
	else if (m_Today.wDay == dayInfo.iDay && m_Today.wYear == m_iYear && m_Today.wMonth == m_iMonth)
	{
		// today 框 就用 textcolor
		pRT->DrawRoundRect(rcDay, ptRound);
	}
	DWORD dwState = 0;
	if (m_nSelItem == nItem)
	{
		if (CR_INVALID != m_crSelDayBack)
			pRT->FillSolidRoundRect(rcDay, ptRound, m_crSelDayBack);

		if (CR_INVALID != m_crSelText)
			pRT->SetTextColor(m_crSelText);

		dwState = 2;
	}
	else if (m_nHoverItem == nItem)				// 
	{
		pRT->SetTextColor(m_crHoverText);
		dwState = 1;
	}

	if (NULL != m_pSkinDay)
		m_pSkinDay->DrawByIndex(pRT, rcDay, dwState);

	SStringT sDay;
	sDay.Format(_T("%d"), dayInfo.iDay);
	pRT->DrawText(sDay, -1, rcDay, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

	pRT->SetTextColor(crText);
}

void SCalendar::DrawToday(IRenderTarget* pRT, CRect& rcDay)
{
	if (rcDay.IsRectEmpty())
		return;

	COLORREF crText = pRT->GetTextColor();
	if (HIT_TODAY == m_nHoverItem)
	{
		pRT->SetTextColor(m_crHoverText);
	}

	SStringT szToday;
	szToday.Format(_T("今天:%04d/%d/%d"), m_Today.wYear, m_Today.wMonth, m_Today.wDay);
	//CRect rc = rect;
	m_rcToday = rcDay;
	// 计算 文本 
	pRT->DrawText(szToday, -1, m_rcToday, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_CALCRECT);
	m_rcToday.bottom = m_rcToday.top + rcDay.Height();	//高度 不变
	m_rcToday.OffsetRect((rcDay.Width() - m_rcToday.Width()) / 2, 0);
	pRT->DrawText(szToday, -1, m_rcToday, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	pRT->SetTextColor(crText);
}

void SCalendar::DrawYearDecadeCentury(IRenderTarget *pRT, const CRect& rect, int nItem)
{
	if (nItem < 0 || nItem >= 42)
		return;

	wMonthOrYearInfo& monthYearInfo = m_arrMonthOrYear[nItem];

	COLORREF crText = pRT->GetTextColor();
	CPoint ptRound(1, 1);

	// 

	if (0 != monthYearInfo.nType)
	{
		pRT->SetTextColor(m_crOtherDayText);
	}
	// 		else if (m_Today.wDay == dayInfo.iDay && m_Today.wYear == m_iYear && m_Today.wMonth == m_iMonth)
	// 		{
	// 			// today 框 就用 textcolor
	// 			pRT->DrawRoundRect(rcDay, ptRound);
	// 		}
	DWORD dwState = 0;
	if (m_nSelItem == nItem)
	{
		if (CR_INVALID != m_crSelDayBack)
			pRT->FillSolidRoundRect(rect, ptRound, m_crSelDayBack);

		if (CR_INVALID != m_crSelText)
			pRT->SetTextColor(m_crSelText);

		dwState = 2;
	}
	else if (m_nHoverItem == nItem)				// 
	{
		pRT->SetTextColor(m_crHoverText);
		dwState = 1;
	}

	if (NULL != m_pSkinDay)
		m_pSkinDay->DrawByIndex(pRT, rect, dwState);

	CRect rcItem = rect;
	SStringT sDay;
	if (m_showType == SHOW_YEAR)
	{
		sDay.Format(_T("%d月"), monthYearInfo.iMonthOrYear);
		pRT->DrawText(sDay, sDay.GetLength(), rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}
	else if (m_showType == SHOW_YEAR_DECADE)
	{
		sDay.Format(_T("%d"), monthYearInfo.iMonthOrYear);
		pRT->DrawText(sDay, sDay.GetLength(), rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}
	else if (m_showType == SHOW_YEAR_CENTURY)
	{
		sDay.Format(_T("%d-\n%d"), monthYearInfo.iMonthOrYear, monthYearInfo.iMonthOrYear + 9);
		CRect rcTxt = rcItem;
		pRT->DrawText(sDay, sDay.GetLength(), rcTxt, DT_CALCRECT);
		rcItem.DeflateRect((rcItem.Width() - rcTxt.Width()) / 2, (rcItem.Height() - rcTxt.Height()) / 2);
		pRT->DrawText(sDay, sDay.GetLength(), rcItem, 0);
	}


	pRT->SetTextColor(crText);
}

void SCalendar::OnPaint(IRenderTarget * pRT)
{
	// 字体 都用 默认 的 
	if (m_showType == SHOW_MONTH)
	{
		OnPaintMonth(pRT);
	}
	else if (m_showType == SHOW_YEAR || m_showType == SHOW_YEAR_DECADE || m_showType == SHOW_YEAR_CENTURY)
	{
		OnPaintYearDecadeCentury(pRT);
	}
}
void SCalendar::OnPaintMonth(IRenderTarget *pRT)
{
	SPainter painter;
	BeforePaint(pRT, painter);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcYear(rcClient);			// 年月 区域
	rcYear.bottom = rcYear.top + m_nYearMonthHeight.toPixelSize(GetScale());
	DrawYearMonth(pRT, rcYear);

	CRect rcWeek(rcClient);		// 星期 区域
	rcWeek.top = rcYear.bottom;
	rcWeek.bottom = rcWeek.top + m_nWeekHeight.toPixelSize(GetScale());
	DrawWeek(pRT, rcWeek);

	// 计算 天 
	m_rcDays = rcClient;
	m_rcDays.top = rcWeek.bottom;
	m_rcDays.bottom = rcClient.bottom - m_nFooterHeight.toPixelSize(GetScale());

	CRect rcItem;
	for (int i = 0; i < 42; ++i)
	{
		GetItemRect(i, rcItem);
		DrawDay(pRT, rcItem, i);
	}

	CRect rcToday(rcClient);
	rcToday.top = m_rcDays.bottom;
	DrawToday(pRT, rcToday);

	AfterPaint(pRT, painter);
}
void SCalendar::OnPaintYearDecadeCentury(IRenderTarget *pRT)
{
	SPainter painter;
	BeforePaint(pRT, painter);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcYear(rcClient);			// 年月 区域
	rcYear.bottom = rcYear.top + m_nYearMonthHeight.toPixelSize(GetScale());
	DrawYearMonth(pRT, rcYear);

	CRect rcMonth(rcClient);		// 中间月份 区域
	rcMonth.top = rcYear.bottom;
	rcMonth.bottom = rcClient.bottom - m_nFooterHeight.toPixelSize(GetScale());

	CRect rcItem;
	int iWidth = rcMonth.Width() / 4;
	int iHeight = rcMonth.Height() / 3;
	for (int i = 0; i < 12; i++)
	{
		rcItem.left = rcMonth.left + i % 4 * iWidth;
		rcItem.top = rcMonth.top + i / 4 * iHeight;
		rcItem.right = rcItem.left + iWidth;
		rcItem.bottom = rcItem.top + iHeight;
		DrawYearDecadeCentury(pRT, rcItem, i);
	}

	// 计算 今天
	CRect rcToday(rcClient);
	rcToday.top = rcMonth.bottom;
	DrawToday(pRT, rcToday);

	AfterPaint(pRT, painter);
}

void SCalendar::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	int nItem = HitTest(point);
	if (nItem < 0)
	{
		if (HIT_LEFT == nItem)
		{
			if (m_showType == SHOW_MONTH)
			{
				SetLastMonth();
			}
			else if (m_showType == SHOW_YEAR)
			{
				SetLastYear();
			}
			else if (m_showType == SHOW_YEAR_DECADE)
			{
				SetLastYearDecade();
			}
			else if (m_showType == SHOW_YEAR_CENTURY)
			{
				SetLastYearCentury();
			}
		}
		else if (HIT_RIGHT == nItem)
		{
			if (m_showType == SHOW_MONTH)
			{
				SetNextMonth();
			}
			else if (m_showType == SHOW_YEAR)
			{
				SetNextYear();
			}
			else if (m_showType == SHOW_YEAR_DECADE)
			{
				SetNextYearDecade();
			}
			else if (m_showType == SHOW_YEAR_CENTURY)
			{
				SetNextYearCentury();
			}
		}
		else if (HIT_YEAR == nItem)
		{
			if (m_showType != SHOW_YEAR_CENTURY)
			{
				m_showType--;
			}
			SetYearDecadeCentury();
		}

	}
	else if (HIT_TODAY == nItem)
	{
		SetShowType(SHOW_MONTH);
		SetDate(m_Today.wYear, m_Today.wMonth, m_Today.wDay, HIT_TODAY, true);
	}
	else //if (m_nSelItem != nItem)
	{
		m_showTypeLbdown = m_showType;
		if (m_showType == SHOW_MONTH)
		{
			wDayInfo& dayInfo = m_arrDays[nItem];

			m_nSelItem = nItem;
			EventCalendarExChanged evt(this);
			evt.nBtnType = nItem;		// 天数 就是 0-41
			evt.iNewDay = dayInfo.iDay;
			if (dayInfo.nType < 0)		// 点击 上一个月 的天
			{
				if (1 == m_iMonth)
				{
					m_iMonth = 12;
					--m_iYear;
				}
				else
					--m_iMonth;
			}
			else if (dayInfo.nType > 0)	//点击 下一个月 的 天
			{
				if (12 == m_iMonth)
				{
					m_iMonth = 1;
					++m_iYear;
				}
				else
					++m_iMonth;
			}

			evt.iNewMonth = m_iMonth;
			evt.iNewYear = m_iYear;
			FireEvent(evt);
		}
		else
		{
			m_nSelItem = nItem;
			wMonthOrYearInfo& info = m_arrMonthOrYear[nItem];

			int tmpYear = 0, tmpMonth = 0;

			if (m_showType == SHOW_YEAR)
			{
				tmpYear = m_iYear;
				tmpMonth = info.iMonthOrYear;
			}
			else if (m_showType == SHOW_YEAR_DECADE)
			{
				tmpYear = info.iMonthOrYear;
				tmpMonth = m_iMonth;
			}
			else if (m_showType == SHOW_YEAR_CENTURY)
			{
				tmpYear = info.iMonthOrYear;
				tmpMonth = m_iMonth;
			}
			if (m_showType != SHOW_MONTH)
			{
				m_showType++;
			}
			if (m_showType != SHOW_MONTH)
			{
				SetYearMonth(tmpYear, tmpMonth);
			}
			else
			{
				SetDate(tmpYear, tmpMonth, 1, HIT_NULL);
			}
		}
	}
	Invalidate();
}

void SCalendar::OnLButtonUp(UINT nFlags, CPoint point)
{
	int nItem = HitTest(point);
	// 只有在显示天数， 点击天  才 发送 cmd 事件  使dropwnd关闭
	//其他情况，让event mute静止
	if (!(m_showType == SHOW_MONTH && m_showTypeLbdown == SHOW_MONTH && nItem >= 0 && nItem < 42))
	{
		GetEventSet()->setMutedState(true);
	}

	__super::OnLButtonUp(nFlags, point);

	if (!(m_showType == SHOW_MONTH && m_showTypeLbdown == SHOW_MONTH  && nItem >= 0 && nItem < 42))
	{
		GetEventSet()->setMutedState(false);
	}
}

void SCalendar::OnMouseMove(UINT nFlags, CPoint pt)
{
	int nItem = HitTest(pt);
	m_nHoverItem = nItem;
	Invalidate();
}

void SCalendar::OnMouseLeave()
{
	m_nHoverItem = HIT_NULL;
	Invalidate();
}

BOOL SCalendar::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 0)			// 上滚  上一个
	{
		if (m_showType == SHOW_MONTH)
		{
			SetLastMonth();
		}
		else if (m_showType == SHOW_YEAR)
		{
			SetLastYear();
		}
		else if (m_showType == SHOW_YEAR_DECADE)
		{
			SetLastYearDecade();
		}
		else if (m_showType == SHOW_YEAR_CENTURY)
		{
			SetLastYearCentury();
		}
	}
	else							// 下滚   下一个 
	{
		if (m_showType == SHOW_MONTH)
		{
			SetNextMonth();
		}
		else if (m_showType == SHOW_YEAR)
		{
			SetNextYear();
		}
		else if (m_showType == SHOW_YEAR_DECADE)
		{
			SetNextYearDecade();
		}
		else if (m_showType == SHOW_YEAR_CENTURY)
		{
			SetNextYearCentury();
		}

	}


	return TRUE;
}

int SCalendar::HitTest(const CPoint& pt)
{
	if (m_showType == SHOW_MONTH)
	{
		if (pt.y < m_rcDays.top)				//  小于  表示  在 天  的上面
		{
			if (pt.y >(m_rcDays.top - m_nWeekHeight.toPixelSize(GetScale())))
				return HIT_NULL;

			int nYearHei = m_nYearMonthHeight.toPixelSize(GetScale());
			if ((pt.x - m_rcDays.left) < nYearHei)
				return HIT_LEFT;
			else if ((m_rcDays.right - pt.x) < nYearHei)
				return HIT_RIGHT;

			return HIT_YEAR;
		}
		else if (pt.y > m_rcDays.bottom)
		{
			if (m_rcToday.PtInRect(pt))
				return HIT_TODAY;

			return 43;
		}

		int nDay_X = pt.x - m_rcDays.left;
		int nDay_Y = pt.y - m_rcDays.top;
		int nWid = m_rcDays.Width() / 7;
		int nHei = m_rcDays.Height() / 6;

		int nRow = nDay_X / nWid;
		if (nRow > 6)			// 处理 边缘 
			nRow = 6;

		return nRow + (nDay_Y / nHei) * 7;
	}
	else if (m_showType == SHOW_YEAR || m_showType == SHOW_YEAR_DECADE || m_showType == SHOW_YEAR_CENTURY)
	{
		//多出星期的高度
		CRect rcDays = m_rcDays;
		rcDays.top = rcDays.top - m_nWeekHeight.toPixelSize(GetScale());

		if (pt.y < rcDays.top)				//  小于  表示  在 天  的上面
		{
			int nYearHei = m_nYearMonthHeight.toPixelSize(GetScale());
			if ((pt.x - rcDays.left) < nYearHei)
				return HIT_LEFT;
			else if ((rcDays.right - pt.x) < nYearHei)
				return HIT_RIGHT;

			return HIT_YEAR;
		}
		else if (pt.y > rcDays.bottom)
		{
			if (m_rcToday.PtInRect(pt))
				return HIT_TODAY;

			return 43;
		}

		int nDay_X = pt.x - rcDays.left;
		int nDay_Y = pt.y - rcDays.top;
		int nWid = m_rcDays.Width() / 4;
		int nHei = m_rcDays.Height() / 3;

		int nRow = nDay_X / nWid;
		if (nRow > 3)			// 处理 边缘 
			nRow = 3;

		return nRow + (nDay_Y / nHei) * 4;

	}
	return 43;
}

void SCalendar::GetItemRect(int nItem, CRect& rcItem)
{
	int nItemWidth = m_rcDays.Width() / 7;
	int nItemHeight = m_rcDays.Height() / 6;


	rcItem.left = (nItem % 7) * nItemWidth;
	rcItem.right = rcItem.left + nItemWidth;


	rcItem.top = (nItem / 7) * nItemHeight;
	rcItem.bottom = rcItem.top + nItemHeight;
	rcItem.OffsetRect(m_rcDays.TopLeft());
}
}//end of namespace
