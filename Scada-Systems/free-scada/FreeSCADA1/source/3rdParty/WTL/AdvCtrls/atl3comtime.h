///////////////////////////////////////////////////////////// 
// 
// Здесь могла бы быть ваша реклама
//
// 
// 
// 
// 
// 

#ifndef __ATLCOMTIME_H__
#define __ATLCOMTIME_H__

#pragma once

#ifndef __ATLBASE_H__
#error atl3comtime.h requires altbase.h to be included first
#endif

#include "math.h"
#include "time.h"

const int maxTimeBufferSize = 128;
const long maxDaysInSpan  =	3615897L;

extern __declspec(selectany) const TCHAR * const szInvalidDateTime = _T("Invalid DateTime");
extern __declspec(selectany) const TCHAR * const szInvalidDateTimeSpan = _T("Invalid DateTimeSpan");

class COleDateTimeSpan
{
public:
	enum DateTimeSpanStatus
	{
		valid = 0,
		invalid = 1,    // Invalid span (out of range, etc.)
		null = 2,       // Literally has no value
	};

	COleDateTimeSpan():m_span(0),m_status(valid)
	{
		
	}

	COleDateTimeSpan(double dblSpanSrc):m_span(dblSpanSrc),
		m_status(valid)
	{
		CheckRange();
	}

	void SetStatus(DateTimeSpanStatus status)
	{
		m_status = status;
	}

	DateTimeSpanStatus GetStatus() const
	{
		return m_status;
	}

	LONG GetDays() const
	{
		ATLASSERT(GetStatus() == valid);
		return LONG(m_span);
	}
	
	COleDateTimeSpan operator+(const COleDateTimeSpan& dateSpan) const
	{
		COleDateTimeSpan dateSpanTemp;

		// If either operand Null, result Null
		if (GetStatus() == null || dateSpan.GetStatus() == null)
		{
			dateSpanTemp.SetStatus(null);
			return dateSpanTemp;
		}

		// If either operand Invalid, result Invalid
		if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
		{
			dateSpanTemp.SetStatus(invalid);
			return dateSpanTemp;
		}

		// Add spans and validate within legal range
		dateSpanTemp.m_span = m_span + dateSpan.m_span;
		dateSpanTemp.CheckRange();

		return dateSpanTemp;
	}

	operator double() const
	{
		return m_span;
	}
/*
	COleDateTimeSpan operator-(const COleDateTimeSpan& dateSpan) const
	{
		COleDateTimeSpan dateSpanTemp;

		// If either operand Null, result Null
		if (GetStatus() == null || dateSpan.GetStatus() == null)
		{
			dateSpanTemp.SetStatus(null);
			return dateSpanTemp;
		}

		// If either operand Invalid, result Invalid
		if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
		{
			dateSpanTemp.SetStatus(invalid);
			return dateSpanTemp;
		}

		// Subtract spans and validate within legal range
		dateSpanTemp.m_span = m_span - dateSpan.m_span;
		dateSpanTemp.CheckRange();

		return dateSpanTemp;
	}

	COleDateTimeSpan operator-() const
	{
		return -this->m_span;
	}
	*/
	
protected:
	void CheckRange()
	{
		if(m_span < -maxDaysInSpan || m_span > maxDaysInSpan)
			m_status = invalid;
	}

private:
	double m_span;
	DateTimeSpanStatus m_status;
};

class COleDateTime
{
public:
	static COleDateTime WINAPI GetCurrentTime()
	{
		return COleDateTime(time(NULL));
	}

	enum DateTimeStatus
	{
		error = -1,
		valid = 0,
		invalid = 1,    // Invalid date (out of range, etc.)
		null = 2,       // Literally has no value
	};

	COleDateTime():m_dt(0), m_status(valid)
	{		
	}

	COleDateTime(time_t timeSrc):m_dt(0), m_status(valid)
	{
		*this = timeSrc;
	}
	
	COleDateTime(int nYear, int nMonth, int nDay, 
		int nHour, int nMin, int nSec)
	{
		SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
	}
	
	COleDateTime(DATE dtSrc):m_dt(dtSrc), m_status(valid)
	{		
	}

	COleDateTime(const SYSTEMTIME& systimeSrc): m_dt(0), m_status(valid)
	{
		*this = systimeSrc;
	}
	
	DateTimeStatus GetStatus() const
	{
		return m_status;
	}

	void SetStatus(DateTimeStatus status)
	{
		m_status = status;
	}
	
	int SetDateTime(int nYear, int nMonth, int nDay,
		int nHour, int nMin, int nSec)
	{
		SYSTEMTIME st;
		::ZeroMemory(&st, sizeof(SYSTEMTIME));

		st.wYear = WORD(nYear);
		st.wMonth = WORD(nMonth);
		st.wDay = WORD(nDay);
		st.wHour = WORD(nHour);
		st.wMinute = WORD(nMin);
		st.wSecond = WORD(nSec);

		return m_status = ::SystemTimeToVariantTime(&st, &m_dt) ? valid : invalid;
	}
	
	COleDateTime& operator=(const time_t& timeSrc)
	{
		SYSTEMTIME st;

		m_status = GetAsSystemTime(timeSrc,st) &&
			::SystemTimeToVariantTime(&st, &m_dt) ? valid : invalid;

		return *this;
	}

	COleDateTime& operator=(const SYSTEMTIME& systimeSrc)
	{
		m_status = ::SystemTimeToVariantTime(const_cast<SYSTEMTIME *>(&systimeSrc), &m_dt) ?
			valid : invalid;
		return *this;
	}

	bool GetAsSystemTime(const time_t& t,SYSTEMTIME& timeDest)
	{
		struct tm* ptm = localtime(&t);
		if (!ptm)
			return false;

		timeDest.wYear = (WORD) (1900 + ptm->tm_year);
		timeDest.wMonth = (WORD) (1 + ptm->tm_mon);
		timeDest.wDayOfWeek = (WORD) ptm->tm_wday;
		timeDest.wDay = (WORD) ptm->tm_mday;
		timeDest.wHour = (WORD) ptm->tm_hour;
		timeDest.wMinute = (WORD) ptm->tm_min;
		timeDest.wSecond = (WORD) ptm->tm_sec;
		timeDest.wMilliseconds = 0;

		return true;
	}

	bool operator==(const COleDateTime& date) const 
	{
		ATLASSERT(GetStatus() == valid);
		ATLASSERT(date.GetStatus() == valid);
		return( m_dt == date.m_dt );
	}
	
	bool operator!=(const COleDateTime& date) const
	{
		ATLASSERT(GetStatus() == valid);
		ATLASSERT(date.GetStatus() == valid);
		return( m_dt != date.m_dt );
	}
	
	bool operator<=(const COleDateTime& date) const
	{
		ATLASSERT(GetStatus() == valid);
		ATLASSERT(date.GetStatus() == valid);
		return( DoubleFromDate( m_dt ) <= DoubleFromDate( date.m_dt ) );
	}
	
	bool operator>=(const COleDateTime& date) const
	{
		ATLASSERT(GetStatus() == valid);
		ATLASSERT(date.GetStatus() == valid);
		return( DoubleFromDate( m_dt ) >= DoubleFromDate( date.m_dt ) );
	}

	bool operator>(const COleDateTime& date) const
	{
		ATLASSERT(GetStatus() == valid);
		ATLASSERT(date.GetStatus() == valid);
		return( DoubleFromDate( m_dt ) > DoubleFromDate( date.m_dt ) );
	}

	bool operator<(const COleDateTime& date) const
	{
		ATLASSERT(GetStatus() == valid);
		ATLASSERT(date.GetStatus() == valid);
		return( DoubleFromDate( m_dt ) < DoubleFromDate( date.m_dt ) );
	}

	COleDateTime operator-(COleDateTimeSpan dateSpan) const
	{
		ATLASSERT(GetStatus() == valid);
		ATLASSERT(dateSpan.GetStatus() == valid);
		return( COleDateTime( DateFromDouble( DoubleFromDate( m_dt )-(double)dateSpan ) ) );
	}

	COleDateTime operator+(COleDateTimeSpan dateSpan) const
	{
		ATLASSERT(GetStatus() == valid);
		ATLASSERT(dateSpan.GetStatus() == valid);
		return( COleDateTime( DateFromDouble( DoubleFromDate( m_dt )+(double)dateSpan ) ) );
	}
	
	COleDateTime& operator+=(COleDateTimeSpan dateSpan)
	{
		ATLASSERT(GetStatus() == valid);
		ATLASSERT(dateSpan.GetStatus() == valid);
		m_dt = DateFromDouble( DoubleFromDate( m_dt )+(double)dateSpan );
		return( *this );
	}

	COleDateTimeSpan operator-(const COleDateTime& date) const
	{
		ATLASSERT(GetStatus() == valid);
		ATLASSERT(date.GetStatus() == valid);
		return DoubleFromDate(m_dt) - DoubleFromDate(date.m_dt);
	}

	CString Format(LPCTSTR lpszFormat) const
	{
		// If null, return empty string
		if(GetStatus() == null)
			return _T("");

		// If invalid, return DateTime global string
		if(GetStatus() == invalid)
		{
			return szInvalidDateTime;
		}

		UDATE ud;
		if (S_OK != VarUdateFromDate(m_dt, 0, &ud))
		{
			return szInvalidDateTime;
		}

		struct tm tmTemp;
		tmTemp.tm_sec	= ud.st.wSecond;
		tmTemp.tm_min	= ud.st.wMinute;
		tmTemp.tm_hour	= ud.st.wHour;
		tmTemp.tm_mday	= ud.st.wDay;
		tmTemp.tm_mon	= ud.st.wMonth - 1;
		tmTemp.tm_year	= ud.st.wYear - 1900;
		tmTemp.tm_wday	= ud.st.wDayOfWeek;
		tmTemp.tm_yday	= ud.wDayOfYear - 1;
		tmTemp.tm_isdst	= 0;

		CString strDate;
		LPTSTR lpszTemp = strDate.GetBufferSetLength(256);
		_tcsftime(lpszTemp, strDate.GetLength(), lpszFormat, &tmTemp);
		strDate.ReleaseBuffer();

		return strDate;
	}
	
	bool GetAsSystemTime(SYSTEMTIME& sysTime) const
	{
		return GetStatus() == valid && ::VariantTimeToSystemTime(m_dt, &sysTime);
	}

	int GetYear() const
	{
		SYSTEMTIME st;
		return GetAsSystemTime(st) ? st.wYear : error;
	}

	// Month of year (1 = January)
	int GetMonth() const
	{
		SYSTEMTIME st;
		return GetAsSystemTime(st) ? st.wMonth : error;
	}

	// Day of month (1-31)
	int GetDay() const
	{
		SYSTEMTIME st;
		return GetAsSystemTime(st) ? st.wDay : error;
	}

	int GetDayOfWeek() const
	{
		SYSTEMTIME st;
		return GetAsSystemTime(st) ? st.wDayOfWeek + 1 : error;
	}
	
protected:
	static double WINAPI DoubleFromDate( DATE date )
	{
		double fTemp;

		// No problem if positive
		if( date >= 0 )
		{
			return( date );
		}

		// If negative, must convert since negative dates not continuous
		// (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
		fTemp = ceil( date );

		return( fTemp-(date-fTemp) );
	}
	
	static DATE WINAPI DateFromDouble(double f)
	{
		double fTemp;

		// No problem if positive
		if( f >= 0 )
		{
			return( f );
		}

		// If negative, must convert since negative dates not continuous
		// (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
		fTemp = floor( f ); // fTemp is now whole part

		return( fTemp+(fTemp-f) );
	}

// Attributes
private:
	DATE m_dt;
	DateTimeStatus m_status;
};

#endif	// __ATLCOMTIME_H__
