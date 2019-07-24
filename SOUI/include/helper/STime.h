#ifndef STime_h__
#define STime_h__

#pragma  once

#include <time.h>

namespace SOUI
{    
    class SOUI_EXP CTimeSpan
    {
    public:
        CTimeSpan() throw();
        CTimeSpan( __time64_t time ) throw();
        CTimeSpan( LONG lDays, int nHours, int nMins, int nSecs ) throw();

        LONGLONG GetDays() const throw();
        LONGLONG GetTotalHours() const throw();
        LONG GetHours() const throw();
        LONGLONG GetTotalMinutes() const throw();
        LONG GetMinutes() const throw();
        LONGLONG GetTotalSeconds() const throw();
        LONG GetSeconds() const throw();

        __time64_t GetTimeSpan() const throw();

        CTimeSpan operator+( CTimeSpan span ) const throw();
        CTimeSpan operator-( CTimeSpan span ) const throw();
        CTimeSpan& operator+=( CTimeSpan span ) throw();
        CTimeSpan& operator-=( CTimeSpan span ) throw();
        bool operator==( CTimeSpan span ) const throw();
        bool operator!=( CTimeSpan span ) const throw();
        bool operator<( CTimeSpan span ) const throw();
        bool operator>( CTimeSpan span ) const throw();
        bool operator<=( CTimeSpan span ) const throw();
        bool operator>=( CTimeSpan span ) const throw();

    private:
        __time64_t m_timeSpan;
    };


class SOUI_EXP CTime
{
public:
    // Attributes
    enum DateTimeStatus
    {
        error = -1,
        valid = 0,
        invalid = 1,    // Invalid date (out of range, etc.)
        null = 2,       // Literally has no value
    };

    static CTime GetCurrentTime() throw();
    CTime(__time64_t tm=0):m_time(tm){}
    CTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
        int nDST = -1 );

    int SetDate(int nYear, int nMonth, int nDay) throw();
    int SetDateTime(int nYear, int nMonth, int nDay,
        int nHour, int nMin, int nSec,int nDST=-1) throw();

    __time64_t    GetTime() const throw();
    struct tm* GetLocalTm(struct tm* ptm) const;
    bool GetAsSystemTime(SYSTEMTIME& timeDest) const throw();
    int GetStatus()
    {
        return m_status;
    }

    void SetStatus(int nStatus)
    {
        m_status=nStatus;
    }
    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;
    int GetHour() const;
    int GetMinute() const;
    int GetSecond() const;
    int GetDayOfWeek() const;

    SStringT Format(
        LPCTSTR pszFormat 
        ) const;

    CTime& operator=( __time64_t time ) throw();

    CTime& operator+=( CTimeSpan span ) throw();
    CTime& operator-=( CTimeSpan span ) throw();

    CTimeSpan operator-( CTime time ) const throw();
    CTime operator-( CTimeSpan span ) const throw();
    CTime operator+( CTimeSpan span ) const throw();

    bool operator==( CTime time ) const throw();
    bool operator!=( CTime time ) const throw();
    bool operator<( CTime time ) const throw();
    bool operator>( CTime time ) const throw();
    bool operator<=( CTime time ) const throw();
    bool operator>=( CTime time ) const throw();

protected:

    __time64_t m_time;
    int m_status;
};



}//end of namespace SOUI


#endif // STime_h__
