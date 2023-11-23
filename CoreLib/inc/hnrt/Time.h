#pragma once


#include <Windows.h>


namespace hnrt
{
	// Intervals in 100 nanoseconds since 1601-01-01T00:00:00.000Z
	class FileTime
	{
	public:

		FileTime();
		FileTime(const FILETIME&);
		FileTime(LONGLONG);
		FileTime(const SYSTEMTIME&);
		FileTime(const FileTime&);
		~FileTime() = default;
		FileTime& operator =(const FileTime&);
		FileTime& operator -=(const FileTime&);
		LONGLONG operator -(const FileTime&) const;
		bool operator <(const FileTime&) const;
		bool operator <=(const FileTime&) const;
		bool operator >(const FileTime&) const;
		bool operator >=(const FileTime&) const;
		operator const FILETIME& () const;
		operator FILETIME& ();
		const FILETIME* operator &() const;
		FILETIME* operator &();
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

	private:

		FILETIME m_ft;
	};

	inline FileTime::FileTime(const FILETIME& ft)
		: m_ft(ft)
	{
	}

	inline FileTime::FileTime(LONGLONG value)
		: m_ft()
	{
		LARGE_INTEGER li = { 0 };
		li.QuadPart = value;
		m_ft.dwLowDateTime = li.LowPart;
		m_ft.dwHighDateTime = li.HighPart;
	}

	inline FileTime::FileTime(const FileTime& src)
		: m_ft(src.m_ft)
	{
	}

	inline FileTime& FileTime::operator =(const FileTime& other)
	{
		m_ft = other.m_ft;
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
		return (m_ft.dwHighDateTime < other.m_ft.dwHighDateTime) || ((m_ft.dwHighDateTime == other.m_ft.dwHighDateTime) && (m_ft.dwLowDateTime < other.m_ft.dwLowDateTime));
	}

	inline bool FileTime::operator <=(const FileTime& other) const
	{
		return (m_ft.dwHighDateTime < other.m_ft.dwHighDateTime) || ((m_ft.dwHighDateTime == other.m_ft.dwHighDateTime) && (m_ft.dwLowDateTime <= other.m_ft.dwLowDateTime));
	}

	inline bool FileTime::operator >(const FileTime& other) const
	{
		return (m_ft.dwHighDateTime > other.m_ft.dwHighDateTime) || ((m_ft.dwHighDateTime == other.m_ft.dwHighDateTime) && (m_ft.dwLowDateTime > other.m_ft.dwLowDateTime));
	}

	inline bool FileTime::operator >=(const FileTime& other) const
	{
		return (m_ft.dwHighDateTime > other.m_ft.dwHighDateTime) || ((m_ft.dwHighDateTime == other.m_ft.dwHighDateTime) && (m_ft.dwLowDateTime >= other.m_ft.dwLowDateTime));
	}

	inline FileTime::operator const FILETIME& () const
	{
		return m_ft;
	}

	inline FileTime::operator FILETIME& ()
	{
		return m_ft;
	}

	inline const FILETIME* FileTime::operator &() const
	{
		return &m_ft;
	}

	inline FILETIME* FileTime::operator &()
	{
		return &m_ft;
	}

	inline FileTime& FileTime::From(LONGLONG value)
	{
		LARGE_INTEGER li = { 0 };
		li.QuadPart = value;
		m_ft.dwLowDateTime = li.LowPart;
		m_ft.dwHighDateTime = li.HighPart;
		return *this;
	}

	inline FileTime& FileTime::AddMilliseconds(LONGLONG milliseconds)
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = m_ft.dwLowDateTime;
		li.HighPart = m_ft.dwHighDateTime;
		li.QuadPart += milliseconds * 10000LL;
		m_ft.dwLowDateTime = li.LowPart;
		m_ft.dwHighDateTime = li.HighPart;
		return *this;
	}

	inline FileTime& FileTime::AddSeconds(LONGLONG seconds)
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = m_ft.dwLowDateTime;
		li.HighPart = m_ft.dwHighDateTime;
		li.QuadPart += seconds * 10000000LL;
		m_ft.dwLowDateTime = li.LowPart;
		m_ft.dwHighDateTime = li.HighPart;
		return *this;
	}

	inline FileTime& FileTime::AddMinutes(LONGLONG seconds)
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = m_ft.dwLowDateTime;
		li.HighPart = m_ft.dwHighDateTime;
		li.QuadPart += seconds * 10000000LL * 60LL;
		m_ft.dwLowDateTime = li.LowPart;
		m_ft.dwHighDateTime = li.HighPart;
		return *this;
	}

	inline FileTime& FileTime::AddHours(LONGLONG seconds)
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = m_ft.dwLowDateTime;
		li.HighPart = m_ft.dwHighDateTime;
		li.QuadPart += seconds * 10000000LL * 60LL * 60LL;
		m_ft.dwLowDateTime = li.LowPart;
		m_ft.dwHighDateTime = li.HighPart;
		return *this;
	}

	inline FileTime& FileTime::AddDays(LONGLONG seconds)
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = m_ft.dwLowDateTime;
		li.HighPart = m_ft.dwHighDateTime;
		li.QuadPart += seconds * 10000000LL * 60LL * 60LL * 24LL;
		m_ft.dwLowDateTime = li.LowPart;
		m_ft.dwHighDateTime = li.HighPart;
		return *this;
	}

	inline LONGLONG FileTime::GetIntervals() const
	{
		LARGE_INTEGER li = { 0 };
		li.LowPart = m_ft.dwLowDateTime;
		li.HighPart = m_ft.dwHighDateTime;
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

	void GetLastDayOfMonth(int year, int month, WORD& dayOfMonth);

	void GetLastDayOfMonth(WORD& year, WORD& month, WORD& dayOfMonth, int dayOfWeek);

	void GetWeekDay(WORD& year, WORD& month, WORD& day);

	void GetDayOfWeek(WORD& year, WORD& month, WORD& day, int dayOfWeek, int ordinal);
}
