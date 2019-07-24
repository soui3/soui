#include <souistd.h>
#include <helper/STime.h>

namespace SOUI
{
	static const int kMaxTimeBufferSize = 128;

	CTimeSpan::CTimeSpan() throw() :
		m_timeSpan(0)
	{
	}

	CTimeSpan::CTimeSpan(__time64_t time) throw() :
		m_timeSpan(time)
	{
	}

	CTimeSpan::CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs) throw()
	{
		m_timeSpan = nSecs + 60 * (nMins + 60 * (nHours + __int64(24) * lDays));
	}

	LONGLONG CTimeSpan::GetDays() const throw()
	{
		return(m_timeSpan / (24 * 3600));
	}

	LONGLONG CTimeSpan::GetTotalHours() const throw()
	{
		return(m_timeSpan / 3600);
	}

	LONG CTimeSpan::GetHours() const throw()
	{
		return(LONG(GetTotalHours() - (GetDays() * 24)));
	}

	LONGLONG CTimeSpan::GetTotalMinutes() const throw()
	{
		return(m_timeSpan / 60);
	}

	LONG CTimeSpan::GetMinutes() const throw()
	{
		return(LONG(GetTotalMinutes() - (GetTotalHours() * 60)));
	}

	LONGLONG CTimeSpan::GetTotalSeconds() const throw()
	{
		return(m_timeSpan);
	}

	LONG CTimeSpan::GetSeconds() const throw()
	{
		return(LONG(GetTotalSeconds() - (GetTotalMinutes() * 60)));
	}

	__time64_t CTimeSpan::GetTimeSpan() const throw()
	{
		return(m_timeSpan);
	}

	CTimeSpan CTimeSpan::operator+(CTimeSpan span) const throw()
	{
		return(CTimeSpan(m_timeSpan + span.m_timeSpan));
	}

	CTimeSpan CTimeSpan::operator-(CTimeSpan span) const throw()
	{
		return(CTimeSpan(m_timeSpan - span.m_timeSpan));
	}

	CTimeSpan& CTimeSpan::operator+=(CTimeSpan span) throw()
	{
		m_timeSpan += span.m_timeSpan;
		return(*this);
	}

	CTimeSpan& CTimeSpan::operator-=(CTimeSpan span) throw()
	{
		m_timeSpan -= span.m_timeSpan;
		return(*this);
	}

	bool CTimeSpan::operator==(CTimeSpan span) const throw()
	{
		return(m_timeSpan == span.m_timeSpan);
	}

	bool CTimeSpan::operator!=(CTimeSpan span) const throw()
	{
		return(m_timeSpan != span.m_timeSpan);
	}

	bool CTimeSpan::operator<(CTimeSpan span) const throw()
	{
		return(m_timeSpan < span.m_timeSpan);
	}

	bool CTimeSpan::operator>(CTimeSpan span) const throw()
	{
		return(m_timeSpan > span.m_timeSpan);
	}

	bool CTimeSpan::operator<=(CTimeSpan span) const throw()
	{
		return(m_timeSpan <= span.m_timeSpan);
	}

	bool CTimeSpan::operator>=(CTimeSpan span) const throw()
	{
		return(m_timeSpan >= span.m_timeSpan);
	}


	/////////////////////////////////////////////////////////////////////////

	CTime::CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
		int nDST)
	{
		SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
	}

	int CTime::SetDateTime(int nYear, int nMonth, int nDay,
		int nHour, int nMin, int nSec, int nDst) throw()
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

		m_status = valid;
		return m_status;//todo:
	}

	int CTime::SetDate(int nYear, int nMonth, int nDay) throw()
	{
		return SetDateTime(nYear, nMonth, nDay, 0, 0, 0);
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

	bool CTime::GetAsSystemTime(SYSTEMTIME& timeDest) const throw()
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

	CTime CTime::GetCurrentTime() throw()
	{
		return(CTime(::_time64(NULL)));
	}

	__time64_t CTime::GetTime() const throw()
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

	CTime& CTime::operator=(__time64_t time) throw()
	{
		m_time = time;

		return(*this);
	}

	bool CTime::operator==(CTime time) const throw()
	{
		return(m_time == time.m_time);
	}

	bool CTime::operator!=(CTime time) const throw()
	{
		return(m_time != time.m_time);
	}

	bool CTime::operator<(CTime time) const throw()
	{
		return(m_time < time.m_time);
	}

	bool CTime::operator>(CTime time) const throw()
	{
		return(m_time > time.m_time);
	}

	bool CTime::operator<=(CTime time) const throw()
	{
		return(m_time <= time.m_time);
	}

	bool CTime::operator>=(CTime time) const throw()
	{
		return(m_time >= time.m_time);
	}


	CTime& CTime::operator+=(CTimeSpan span) throw()
	{
		m_time += span.GetTimeSpan();

		return(*this);
	}

	CTime& CTime::operator-=(CTimeSpan span) throw()
	{
		m_time -= span.GetTimeSpan();

		return(*this);
	}

	CTimeSpan CTime::operator-(CTime time) const throw()
	{
		return(CTimeSpan(m_time - time.m_time));
	}

	CTime CTime::operator-(CTimeSpan span) const throw()
	{
		return(CTime(m_time - span.GetTimeSpan()));
	}

	CTime CTime::operator+(CTimeSpan span) const throw()
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