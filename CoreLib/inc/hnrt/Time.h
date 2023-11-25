#pragma once


#include <Windows.h>


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

	void GetLastDayOfMonth(WORD& year, WORD& month, WORD& day, int dayOfWeek);

	void GetWeekDay(WORD& year, WORD& month, WORD& day);

	void GetDayOfWeek(WORD& year, WORD& month, WORD& day, int dayOfWeek, int ordinal);
}
