#ifndef STime_h__
#define STime_h__

#pragma  once

#include <time.h>

namespace SOUI
{    
    class SOUI_EXP CTimeSpan
    {
    public:
        CTimeSpan();
        CTimeSpan( __time64_t time );
        CTimeSpan( LONG lDays, int nHours, int nMins, int nSecs );

        LONGLONG GetDays() const;
        LONGLONG GetTotalHours() const;
        LONG GetHours() const;
        LONGLONG GetTotalMinutes() const;
        LONG GetMinutes() const;
        LONGLONG GetTotalSeconds() const;
        LONG GetSeconds() const;

        __time64_t GetTimeSpan() const;

        CTimeSpan operator+( CTimeSpan span ) const;
        CTimeSpan operator-( CTimeSpan span ) const;
        CTimeSpan& operator+=( CTimeSpan span );
        CTimeSpan& operator-=( CTimeSpan span );
        bool operator==( CTimeSpan span ) const;
        bool operator!=( CTimeSpan span ) const;
        bool operator<( CTimeSpan span ) const;
        bool operator>( CTimeSpan span ) const;
        bool operator<=( CTimeSpan span ) const;
        bool operator>=( CTimeSpan span ) const;

    private:
        __time64_t m_timeSpan;
    };


class SOUI_EXP CTime
{
public:
    static CTime GetCurrentTime();
	static uint64_t GetCurrentTimeMs();
    CTime(__time64_t tm=0):m_time(tm){}
    CTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
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

    CTime& operator=( __time64_t time );

    CTime& operator+=( CTimeSpan span );
    CTime& operator-=( CTimeSpan span );

    CTimeSpan operator-( CTime time ) const;
    CTime operator-( CTimeSpan span ) const;
    CTime operator+( CTimeSpan span ) const;

    bool operator==( CTime time ) const;
    bool operator!=( CTime time ) const;
    bool operator<( CTime time ) const;
    bool operator>( CTime time ) const;
    bool operator<=( CTime time ) const;
    bool operator>=( CTime time ) const;

protected:

    __time64_t m_time;
};



}//end of namespace SOUI


#endif // STime_h__
