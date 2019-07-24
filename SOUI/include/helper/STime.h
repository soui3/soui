#ifndef STime_h__
#define STime_h__

#pragma  once

#include <time.h>

namespace SOUI
{    
    class SOUI_EXP STimeSpan
    {
    public:
        STimeSpan();
        STimeSpan( __time64_t time );
        STimeSpan( LONG lDays, int nHours, int nMins, int nSecs );

        LONGLONG GetDays() const;
        LONGLONG GetTotalHours() const;
        LONG GetHours() const;
        LONGLONG GetTotalMinutes() const;
        LONG GetMinutes() const;
        LONGLONG GetTotalSeconds() const;
        LONG GetSeconds() const;

        __time64_t GetTimeSpan() const;

        STimeSpan operator+( STimeSpan span ) const;
        STimeSpan operator-( STimeSpan span ) const;
        STimeSpan& operator+=( STimeSpan span );
        STimeSpan& operator-=( STimeSpan span );
        bool operator==( STimeSpan span ) const;
        bool operator!=( STimeSpan span ) const;
        bool operator<( STimeSpan span ) const;
        bool operator>( STimeSpan span ) const;
        bool operator<=( STimeSpan span ) const;
        bool operator>=( STimeSpan span ) const;

    private:
        __time64_t m_timeSpan;
    };


class SOUI_EXP STime
{
public:
    static STime GetCurrentTime();
	static uint64_t GetCurrentTimeMs();
    STime(__time64_t tm=0):m_time(tm){}
    STime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
        int nDST = -1 );

    void SetDate(int nYear, int nMonth, int nDay);
    void SetDateTime(int nYear, int nMonth, int nDay,
        int nHour, int nMin, int nSec,int nDST=-1);

    __time64_t    GetTime() const;
    struct tm* GetLocalTm(struct tm* ptm) const;
    bool GetAsSystemTime(SYSTEMTIME& timeDest) const;
    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;
    int GetHour() const;
    int GetMinute() const;
    int GetSecond() const;
    int GetDayOfWeek() const;

    SStringT Format(LPCTSTR pszFormat) const;

    STime& operator=( __time64_t time );

    STime& operator+=( STimeSpan span );
    STime& operator-=( STimeSpan span );

    STimeSpan operator-( STime time ) const;
    STime operator-( STimeSpan span ) const;
    STime operator+( STimeSpan span ) const;

    bool operator==( STime time ) const;
    bool operator!=( STime time ) const;
    bool operator<( STime time ) const;
    bool operator>( STime time ) const;
    bool operator<=( STime time ) const;
    bool operator>=( STime time ) const;

protected:

    __time64_t m_time;
};



}//end of namespace SOUI


#endif // STime_h__
