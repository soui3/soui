#include <souistd.h>
#include <helper/STime.h>

namespace SOUI
{
	static const int kMaxTimeBufferSize = 128;

	CTimeSpan::CTimeSpan()  :
		m_timeSpan(0)
	{
	}

	CTimeSpan::CTimeSpan(__time64_t time)  :
		m_timeSpan(time)
	{
	}

	CTimeSpan::CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs) 
	{
		m_timeSpan = nSecs + 60 * (nMins + 60 * (nHours + __int64(24) * lDays));
	}

	LONGLONG CTimeSpan::GetDays() const 
	{
		return(m_timeSpan / (24 * 3600));
	}

	LONGLONG CTimeSpan::GetTotalHours() const 
	{
		return(m_timeSpan / 3600);
	}

	LONG CTimeSpan::GetHours() const 
	{
		return(LONG(GetTotalHours() - (GetDays() * 24)));
	}

	LONGLONG CTimeSpan::GetTotalMinutes() const 
	{
		return(m_timeSpan / 60);
	}

	LONG CTimeSpan::GetMinutes() const 
	{
		return(LONG(GetTotalMinutes() - (GetTotalHours() * 60)));
	}

	LONGLONG CTimeSpan::GetTotalSeconds() const 
	{
		return(m_timeSpan);
	}

	LONG CTimeSpan::GetSeconds() const 
	{
		return(LONG(GetTotalSeconds() - (GetTotalMinutes() * 60)));
	}

	__time64_t CTimeSpan::GetTimeSpan() const 
	{
		return(m_timeSpan);
	}

	CTimeSpan CTimeSpan::operator+(CTimeSpan span) const 
	{
		return(CTimeSpan(m_timeSpan + span.m_timeSpan));
	}

	CTimeSpan CTimeSpan::operator-(CTimeSpan span) const 
	{
		return(CTimeSpan(m_timeSpan - span.m_timeSpan));
	}

	CTimeSpan& CTimeSpan::operator+=(CTimeSpan span) 
	{
		m_timeSpan += span.m_timeSpan;
		return(*this);
	}

	CTimeSpan& CTimeSpan::operator-=(CTimeSpan span) 
	{
		m_timeSpan -= span.m_timeSpan;
		return(*this);
	}

	bool CTimeSpan::operator==(CTimeSpan span) const 
	{
		return(m_timeSpan == span.m_timeSpan);
	}

	bool CTimeSpan::operator!=(CTimeSpan span) const 
	{
		return(m_timeSpan != span.m_timeSpan);
	}

	bool CTimeSpan::operator<(CTimeSpan span) const 
	{
		return(m_timeSpan < span.m_timeSpan);
	}

	bool CTimeSpan::operator>(CTimeSpan span) const 
	{
		return(m_timeSpan > span.m_timeSpan);
	}

	bool CTimeSpan::operator<=(CTimeSpan span) const 
	{
		return(m_timeSpan <= span.m_timeSpan);
	}

	bool CTimeSpan::operator>=(CTimeSpan span) const 
	{
		return(m_timeSpan >= span.m_timeSpan);
	}


	/////////////////////////////////////////////////////////////////////////

	CTime::CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
		int nDST)
	{
		SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
	}

	void CTime::SetDateTime(int nYear, int nMonth, int nDay,
		int nHour, int nMin, int nSec, int nDst) 
	{
		struct tm atm;

		atm.tm_sec = nSec;
		atm.tm_min = nMin;
		atm.tm_hour = nHour;
		atm.tm_mday = nDay;
		atm.tm_mon = nMonth - 1;        // tm_mon is 0 based
		atm.tm_year = nYear - 1900;     // tm_year is 1900 based
		atm.tm_isdst = nDst;

		m_time = _mktime64(&atm);
	}

	void CTime::SetDate(int nYear, int nMonth, int nDay)
	{
		SetDateTime(nYear, nMonth, nDay, 0, 0, 0);
	}

	struct tm* CTime::GetLocalTm(struct tm* ptm) const
	{
		// Ensure ptm is valid
		SASSERT(ptm != NULL);

		if (ptm != NULL)
		{
			struct tm ptmTemp;
			errno_t err = _localtime64_s(&ptmTemp, &m_time);

			if (err != 0)
			{
				return NULL;    // indicates that m_time was not initialized!
			}

			*ptm = ptmTemp;
			return ptm;
		}

		return NULL;
	}

	bool CTime::GetAsSystemTime(SYSTEMTIME& timeDest) const 
	{
		struct tm ttm;
		struct tm* ptm;

		ptm = GetLocalTm(&ttm);

		if (!ptm) { return false; }

		timeDest.wYear = (WORD)(1900 + ptm->tm_year);
		timeDest.wMonth = (WORD)(1 + ptm->tm_mon);
		timeDest.wDayOfWeek = (WORD)ptm->tm_wday;
		timeDest.wDay = (WORD)ptm->tm_mday;
		timeDest.wHour = (WORD)ptm->tm_hour;
		timeDest.wMinute = (WORD)ptm->tm_min;
		timeDest.wSecond = (WORD)ptm->tm_sec;
		timeDest.wMilliseconds = 0;

		return true;
	}

	CTime CTime::GetCurrentTime()
	{
		return(CTime(::_time64(NULL)));
	}

	uint64_t CTime::GetCurrentTimeMs() {
		SYSTEMTIME st;
		GetSystemTime(&st); // We use UTC time

		tm current;
		memset(&current, 0, sizeof(current));

		current.tm_year = st.wYear - 1900;
		current.tm_mon = st.wMonth - 1;
		current.tm_mday = st.wDay;
		current.tm_hour = st.wHour;
		current.tm_min = st.wMinute;
		current.tm_sec = st.wSecond;
		time_t currentTime = _mkgmtime(&current);
		return static_cast<uint64_t>(currentTime) * 1000 + st.wMilliseconds;
	}

	__time64_t CTime::GetTime() const 
	{
		return(m_time);
	}

	int CTime::GetYear() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? (ptm->tm_year) + 1900 : 0;
	}

	int CTime::GetMonth() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_mon + 1 : 0;
	}

	int CTime::GetDay() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_mday : 0;
	}

	int CTime::GetHour() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_hour : -1;
	}

	int CTime::GetMinute() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_min : -1;
	}

	int CTime::GetSecond() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_sec : -1;
	}

	int CTime::GetDayOfWeek() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_wday + 1 : 0;
	}

	CTime& CTime::operator=(__time64_t time) 
	{
		m_time = time;

		return(*this);
	}

	bool CTime::operator==(CTime time) const 
	{
		return(m_time == time.m_time);
	}

	bool CTime::operator!=(CTime time) const 
	{
		return(m_time != time.m_time);
	}

	bool CTime::operator<(CTime time) const 
	{
		return(m_time < time.m_time);
	}

	bool CTime::operator>(CTime time) const 
	{
		return(m_time > time.m_time);
	}

	bool CTime::operator<=(CTime time) const 
	{
		return(m_time <= time.m_time);
	}

	bool CTime::operator>=(CTime time) const 
	{
		return(m_time >= time.m_time);
	}


	CTime& CTime::operator+=(CTimeSpan span) 
	{
		m_time += span.GetTimeSpan();

		return(*this);
	}

	CTime& CTime::operator-=(CTimeSpan span) 
	{
		m_time -= span.GetTimeSpan();

		return(*this);
	}

	CTimeSpan CTime::operator-(CTime time) const 
	{
		return(CTimeSpan(m_time - time.m_time));
	}

	CTime CTime::operator-(CTimeSpan span) const 
	{
		return(CTime(m_time - span.GetTimeSpan()));
	}

	CTime CTime::operator+(CTimeSpan span) const 
	{
		return(CTime(m_time + span.GetTimeSpan()));
	}

	SStringT CTime::Format(LPCTSTR pszFormat) const
	{
		if (pszFormat == NULL)
		{
			return pszFormat;
		}

		TCHAR szBuffer[kMaxTimeBufferSize];

		struct tm ptmTemp;
		errno_t err = _localtime64_s(&ptmTemp, &m_time);
		if (err != 0 || !_tcsftime(szBuffer, kMaxTimeBufferSize, pszFormat, &ptmTemp))
		{
			szBuffer[0] = '\0';
		}

		return szBuffer;
	}
}