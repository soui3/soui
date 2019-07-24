#include <souistd.h>
#include <helper/STime.h>

namespace SOUI
{
	static const int kMaxTimeBufferSize = 128;

	STimeSpan::STimeSpan()  :
		m_timeSpan(0)
	{
	}

	STimeSpan::STimeSpan(__time64_t time)  :
		m_timeSpan(time)
	{
	}

	STimeSpan::STimeSpan(LONG lDays, int nHours, int nMins, int nSecs) 
	{
		m_timeSpan = nSecs + 60 * (nMins + 60 * (nHours + __int64(24) * lDays));
	}

	LONGLONG STimeSpan::GetDays() const 
	{
		return(m_timeSpan / (24 * 3600));
	}

	LONGLONG STimeSpan::GetTotalHours() const 
	{
		return(m_timeSpan / 3600);
	}

	LONG STimeSpan::GetHours() const 
	{
		return(LONG(GetTotalHours() - (GetDays() * 24)));
	}

	LONGLONG STimeSpan::GetTotalMinutes() const 
	{
		return(m_timeSpan / 60);
	}

	LONG STimeSpan::GetMinutes() const 
	{
		return(LONG(GetTotalMinutes() - (GetTotalHours() * 60)));
	}

	LONGLONG STimeSpan::GetTotalSeconds() const 
	{
		return(m_timeSpan);
	}

	LONG STimeSpan::GetSeconds() const 
	{
		return(LONG(GetTotalSeconds() - (GetTotalMinutes() * 60)));
	}

	__time64_t STimeSpan::GetTimeSpan() const 
	{
		return(m_timeSpan);
	}

	STimeSpan STimeSpan::operator+(STimeSpan span) const 
	{
		return(STimeSpan(m_timeSpan + span.m_timeSpan));
	}

	STimeSpan STimeSpan::operator-(STimeSpan span) const 
	{
		return(STimeSpan(m_timeSpan - span.m_timeSpan));
	}

	STimeSpan& STimeSpan::operator+=(STimeSpan span) 
	{
		m_timeSpan += span.m_timeSpan;
		return(*this);
	}

	STimeSpan& STimeSpan::operator-=(STimeSpan span) 
	{
		m_timeSpan -= span.m_timeSpan;
		return(*this);
	}

	bool STimeSpan::operator==(STimeSpan span) const 
	{
		return(m_timeSpan == span.m_timeSpan);
	}

	bool STimeSpan::operator!=(STimeSpan span) const 
	{
		return(m_timeSpan != span.m_timeSpan);
	}

	bool STimeSpan::operator<(STimeSpan span) const 
	{
		return(m_timeSpan < span.m_timeSpan);
	}

	bool STimeSpan::operator>(STimeSpan span) const 
	{
		return(m_timeSpan > span.m_timeSpan);
	}

	bool STimeSpan::operator<=(STimeSpan span) const 
	{
		return(m_timeSpan <= span.m_timeSpan);
	}

	bool STimeSpan::operator>=(STimeSpan span) const 
	{
		return(m_timeSpan >= span.m_timeSpan);
	}


	/////////////////////////////////////////////////////////////////////////

	STime::STime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
		int nDST)
	{
		SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
	}

	void STime::SetDateTime(int nYear, int nMonth, int nDay,
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

	void STime::SetDate(int nYear, int nMonth, int nDay)
	{
		SetDateTime(nYear, nMonth, nDay, 0, 0, 0);
	}

	struct tm* STime::GetLocalTm(struct tm* ptm) const
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

	bool STime::GetAsSystemTime(SYSTEMTIME& timeDest) const 
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

	STime STime::GetCurrentTime()
	{
		return(STime(::_time64(NULL)));
	}

	uint64_t STime::GetCurrentTimeMs() {
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

	__time64_t STime::GetTime() const 
	{
		return(m_time);
	}

	int STime::GetYear() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? (ptm->tm_year) + 1900 : 0;
	}

	int STime::GetMonth() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_mon + 1 : 0;
	}

	int STime::GetDay() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_mday : 0;
	}

	int STime::GetHour() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_hour : -1;
	}

	int STime::GetMinute() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_min : -1;
	}

	int STime::GetSecond() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_sec : -1;
	}

	int STime::GetDayOfWeek() const
	{
		struct tm ttm;
		struct tm * ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_wday + 1 : 0;
	}

	STime& STime::operator=(__time64_t time) 
	{
		m_time = time;

		return(*this);
	}

	bool STime::operator==(STime time) const 
	{
		return(m_time == time.m_time);
	}

	bool STime::operator!=(STime time) const 
	{
		return(m_time != time.m_time);
	}

	bool STime::operator<(STime time) const 
	{
		return(m_time < time.m_time);
	}

	bool STime::operator>(STime time) const 
	{
		return(m_time > time.m_time);
	}

	bool STime::operator<=(STime time) const 
	{
		return(m_time <= time.m_time);
	}

	bool STime::operator>=(STime time) const 
	{
		return(m_time >= time.m_time);
	}


	STime& STime::operator+=(STimeSpan span) 
	{
		m_time += span.GetTimeSpan();

		return(*this);
	}

	STime& STime::operator-=(STimeSpan span) 
	{
		m_time -= span.GetTimeSpan();

		return(*this);
	}

	STimeSpan STime::operator-(STime time) const 
	{
		return(STimeSpan(m_time - time.m_time));
	}

	STime STime::operator-(STimeSpan span) const 
	{
		return(STime(m_time - span.GetTimeSpan()));
	}

	STime STime::operator+(STimeSpan span) const 
	{
		return(STime(m_time + span.GetTimeSpan()));
	}

	SStringT STime::Format(LPCTSTR pszFormat) const
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