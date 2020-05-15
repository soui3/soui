#include "stdafx.h"
#include "sclock.h"

namespace SOUI{

SClock::SClock()
{
}

void SClock::OnPaint(SOUI::IRenderTarget * pRT)
{
	__super::OnPaint(pRT);

	CRect rcClient;
	GetClientRect(&rcClient);

	CPoint center = rcClient.CenterPoint();

	// 计算矩形
	// 35 * 16
	CRect rcDraw(center, SOUI::CPoint(center.x + 200, center.y + 32));
	rcDraw.OffsetRect(-35, -16);
	CRect rcSrc(0, 0, 200, 32);


    SYSTEMTIME last_refresh_time;
    ::GetLocalTime(&last_refresh_time);

    {
        double angle = GetHourAngle(last_refresh_time.wHour,last_refresh_time.wMinute);
        SMatrix matrix = InitMatrix(angle,  center);
        pRT->SetTransform(matrix.GetData(), NULL);
        pRT->DrawBitmapEx(rcDraw, pointer_hour, &rcSrc, EM_STRETCH, 255);
    }

    {
        double angle = GetMinuteSecondAngle(last_refresh_time.wMinute);
        SMatrix matrix = InitMatrix(angle, center);
        pRT->SetTransform(matrix.GetData(), NULL);
        pRT->DrawBitmapEx(rcDraw, pointer_minute, &rcSrc, EM_STRETCH, 255);
    }

    {
        double angle = GetMinuteSecondAngle(last_refresh_time.wSecond);
        SMatrix matrix = InitMatrix(angle, center);
        pRT->SetTransform(matrix.GetData(), NULL);
        pRT->DrawBitmapEx(rcDraw, pointer_second, &rcSrc, EM_STRETCH, 255);
    }
	pRT->SetTransform(SMatrix().GetData(),NULL);
}

SMatrix SClock::InitMatrix(double angle, CPoint &center)
{
	SMatrix mtx;
	mtx.setRotate(angle-90, center.x, center.y);
	return mtx;
}


double SClock::GetMinuteSecondAngle(int nValue)
{
    return (double)nValue * ((double)360 / 60);
}

double SClock::GetHourAngle(int nHour,int nMinute)
{
	double base = (double)(nHour % 12) * ((double)360 / 12);
	base += (double)nMinute * ((double)360 / 12 / 60);
	return base;
}

void SClock::OnTimer(char cTimerID)
{
	Invalidate();
}

int SClock::OnCreate(void*)
{
	SetTimer(TIMER_REFRESH, 200);
	return 0;
}

}