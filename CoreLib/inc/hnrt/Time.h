#pragma once


#include "hnrt/String.h"


namespace hnrt
{
	// Intervals in 100 nanoseconds since 1601-01-01T00:00:00.000Z
	struct FileTime
		: public FILETIME
	{
		FileTime();
		FileTime(LONGLONG);
		FileTime(const SYSTEMTIME&);
		FileTime(const FILETIME&);
		FileTime(const FileTime&);
		~FileTime() = default;
		FileTime& operator =(const FileTime&);
		FileTime& operator -=(const FileTime&);
		LONGLONG operator -(const FileTime&) const;
		bool operator <(const FileTime&) const;
		bool operator <=(const FileTime&) const;
		bool operator >(const FileTime&) const;
		bool operator >=(const FileTime&) const;
		FileTime& FromSystemTime();
		FileTime& FromLocalTime();
		FileTime& From(const SYSTEMTIME&);
		FileTime& From(LONGLONG);
		FileTime& AddMilliseconds(LONGLONG);
		FileTime& AddSeconds(LONGLONG);
		FileTime& AddMinutes(LONGLONG);
		FileTime& AddHours(LONGLONG);
		FileTime& AddDays(LONGLONG);
		LONGLONG GetIntervals() const;
		LONGLONG GetMicroseconds() const;
		LONGLONG GetMilliseconds() const;
		LONGLONG GetSeconds() const;
		SYSTEMTIME& ToSystemTime(SYSTEMTIME&) const;

		__declspec(property(get = GetIntervals)) LONGLONG Intervals;
		__declspec(property(get = GetMicroseconds)) LONGLONG Microseconds;
		__declspec(property(get = GetMilliseconds)) LONGLONG Milliseconds;
		__declspec(property(get = GetSeconds)) LONGLONG Seconds;
	};

	inline FileTime::FileTime(LONGLONG value)
		: FILETIME()
	{
		LARGE_INTEGER li = { 0 };
		li.QuadPart = value;
		dwLowDateTime = li.LowPart;
		dwHighDateTime = li.HighPart;
	}

	inline FileTime::FileTime(const FILETIME& src)
		: FILETIME(src)
	{
	}

	inline FileTime::FileTime(const FileTime& src)
		: FILETIME(src)
	{
	}

	inline FileTime& FileTime::operator =(const FileTime& other)
	{
		*this = other;
		return *this;
	}

	inline FileTime& FileTime::operator -=(const FileTime& other)
	{
		return From(Intervals - other.Intervals);
	}

	inline LONGLONG FileTime::operator -(const FileTime& other) const
	{
		return Intervals - other.Intervals;
	}

	inline bool FileTime::operator <(const FileTime& other) const
	{
		return (dwHighDateTime < other.dwHighDateTime) || ((dwHighDateTime == other.dwHighDateTime) && (dwLowDateTime < other.dwLowDateTime));
	}

	inline bool FileTime::operator <=(const FileTime& other) const
	{
		return (dwHighDateTime < other.dwHighDateTime) || ((dwHighDateTime == other.dwHighDateTime) && (dwLowDateTime <= other.dwLowDateTime));
	}

	inline bool FileTime::operator >(const FileTime& other) const
	{
		return (dwHighDateTime > other.dwHighDateTime) || ((dwHighDateTime == other.dwHighDateTime) && (dwLowDateTime > other.dwLowDateTime));
	}

	inline bool FileTime::operator >=(const FileTime& other) const
	{
		return (dwHighDateTime > other.dwHighDateTime) || ((dwHighDateTime == other.dwHighDateTime) && (dwLowDateTime >= other.dwLowDateTime));
	}

	inline FileTime& FileTime::From(LONGLONG value)
	{
		LARGE_INTEGER li = { 0 };
		li.QuadPart = value;
		dwLowDateTime = li.LowPart;
		dwHighDateTime = li.HighPart;
		return *this;
	}

	inline FileTime& FileTime::AddMilliseconds(LONGLONG milliseconds)
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = dwLowDateTime;
		li.HighPart = dwHighDateTime;
		li.QuadPart += milliseconds * 10000LL;
		dwLowDateTime = li.LowPart;
		dwHighDateTime = li.HighPart;
		return *this;
	}

	inline FileTime& FileTime::AddSeconds(LONGLONG seconds)
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = dwLowDateTime;
		li.HighPart = dwHighDateTime;
		li.QuadPart += seconds * 10000000LL;
		dwLowDateTime = li.LowPart;
		dwHighDateTime = li.HighPart;
		return *this;
	}

	inline FileTime& FileTime::AddMinutes(LONGLONG minutes)
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = dwLowDateTime;
		li.HighPart = dwHighDateTime;
		li.QuadPart += minutes * 10000000LL * 60LL;
		dwLowDateTime = li.LowPart;
		dwHighDateTime = li.HighPart;
		return *this;
	}

	inline FileTime& FileTime::AddHours(LONGLONG hours)
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = dwLowDateTime;
		li.HighPart = dwHighDateTime;
		li.QuadPart += hours * 10000000LL * 60LL * 60LL;
		dwLowDateTime = li.LowPart;
		dwHighDateTime = li.HighPart;
		return *this;
	}

	inline FileTime& FileTime::AddDays(LONGLONG days)
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = dwLowDateTime;
		li.HighPart = dwHighDateTime;
		li.QuadPart += days * 10000000LL * 60LL * 60LL * 24LL;
		dwLowDateTime = li.LowPart;
		dwHighDateTime = li.HighPart;
		return *this;
	}

	inline LONGLONG FileTime::GetIntervals() const
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = dwLowDateTime;
		li.HighPart = dwHighDateTime;
		return li.QuadPart;
	}

	inline LONGLONG FileTime::GetMicroseconds() const
	{
		return (Intervals + 5LL) / 10LL;
	}

	inline LONGLONG FileTime::GetMilliseconds() const
	{
		return (Intervals + 5000LL) / 10000LL;
	}

	inline LONGLONG FileTime::GetSeconds() const
	{
		return (Intervals + 5000000LL) / 10000000LL;
	}

	WORD GetLastDayOfMonth(int year, int month);

	/// <summary>
	/// Returns the day that meets the following conditions:
	/// 1. Be equal to or later than the specified year-month-day.
	/// 2. Matches with the specified day of the week.
	/// 3. Be within 7 days to the last day of the month.
	/// </summary>
	/// <param name="year">[IN/OUT] year</param>
	/// <param name="month">[IN/OUT] month</param>
	/// <param name="day">[IN/OUT] day</param>
	/// <param name="dayOfWeek">[IN] Day of Week: 0=SUN,1=MON,2=TUE,3=WED,4=THU,5=FRI,6=SAT</param>
	void GetLastDayOfMonth(WORD& year, WORD& month, WORD& day, int dayOfWeek);

	/// <summary>
	/// Returns the weekday that is equal to or later than the specified year-month-day.
	/// </summary>
	/// <param name="year">[IN/OUT] year</param>
	/// <param name="month">[IN/OUT] month</param>
	/// <param name="day">[IN/OUT] day</param>
	void GetWeekDay(WORD& year, WORD& month, WORD& day);

	/// <summary>
	/// Returns the day that meets the following conditions:
	/// 1. Be equal to or later than the specified year-month-day.
	/// 2. Matches with the specified day of the week.
	/// 3. Matches with the ordinal number of the day of week in the month.
	/// </summary>
	/// <param name="year">[IN/OUT] year</param>
	/// <param name="month">[IN/OUT] month</param>
	/// <param name="day">[IN/OUT] day</param>
	/// <param name="dayOfWeek">[IN] Day of Week: 0=SUN,1=MON,2=TUE,3=WED,4=THU,5=FRI,6=SAT</param>
	/// <param name="ordinal">[IN] Ordinal number of the day of week in the month, from 1 through 5.</param>
	void GetDayOfWeek(WORD& year, WORD& month, WORD& day, int dayOfWeek, int ordinal);

	String SystemTimeToString(const SYSTEMTIME&);
}

//
// FILETIME values at 1970-01-01T00:00:00.000Z
//
#define FILETIME_UNIX_EPOCH_TIME_IN_100NANOSECONDS 116444736000000000LL
#define FILETIME_UNIX_EPOCH_TIME_IN_MICROSECONDS   11644473600000000LL
#define FILETIME_UNIX_EPOCH_TIME_IN_MILLISECONDS   11644473600000LL
#define FILETIME_UNIX_EPOCH_TIME_IN_SECONDS        11644473600LL
