#include "pch.h"
#include "hnrt/Time.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


FileTime::FileTime()
	: FILETIME()
{
	GetSystemTimeAsFileTime(this);
}


FileTime::FileTime(const SYSTEMTIME& st)
	: FILETIME()
{
	(void)From(st);
}


FileTime& FileTime::FromSystemTime()
{
	GetSystemTimeAsFileTime(this);
	return *this;
}


FileTime& FileTime::FromLocalTime()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	return From(st);
}


FileTime& FileTime::From(const SYSTEMTIME& st)
{
	if (!SystemTimeToFileTime(&st, this))
	{
		throw Win32Exception(GetLastError(), L"SystemTimeToFileTime failed.");
	}
	return *this;
}


SYSTEMTIME& FileTime::ToSystemTime(SYSTEMTIME& st) const
{
	if (!FileTimeToSystemTime(this, &st))
	{
		throw Win32Exception(GetLastError(), L"FileTimeToSystemTime failed.");
	}
	// Validate returned values just in case
	if (st.wMonth < 1 || 12 < st.wMonth)
	{
		throw Exception(L"FileTimeToSystemTime returned an invalid month: %d", st.wMonth);
	}
	if (6 < st.wDayOfWeek) // 0=SUN 1=MON 2=TUE 3=WED 4=THU 5=FRI 6=SAT
	{
		throw Exception(L"FileTimeToSystemTime returned an invalid day of week: %d", st.wDayOfWeek);
	}
	if (st.wDay < 1 || 31 < st.wDay)
	{
		throw Exception(L"FileTimeToSystemTime returned an invalid day: %d", st.wDay);
	}
	if (23 < st.wHour)
	{
		throw Exception(L"FileTimeToSystemTime returned an invalid hour: %d", st.wHour);
	}
	if (59 < st.wMinute)
	{
		throw Exception(L"FileTimeToSystemTime returned an invalid minute: %d", st.wMinute);
	}
	if (59 < st.wSecond)
	{
		throw Exception(L"FileTimeToSystemTime returned an invalid second: %d", st.wSecond);
	}
	if (999 < st.wMilliseconds)
	{
		throw Exception(L"FileTimeToSystemTime returned an invalid millisecond: %d", st.wMilliseconds);
	}
	return st;
}


WORD hnrt::GetLastDayOfMonth(int year, int month)
{
	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	case 2:
		if ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0))
		{
			return 29;
		}
		else
		{
			return 28;
		}
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	default:
		throw Exception(L"GetLastDayOfMonth: Bad month: %d", month);
	}
}


void hnrt::GetLastDayOfMonth(WORD& year, WORD& month, WORD& day, int dayOfWeek)
{
	if (dayOfWeek < 0 || 6 < dayOfWeek)
	{
		throw Exception(L"GetLastDayOfMonth: Bad day of week: %d", dayOfWeek);
	}
	SYSTEMTIME st = { 0 };
	st.wYear = year;
	st.wMonth = month;
	st.wDay = day;
	FileTime ft(st);
	ft.ToSystemTime(st);
	WORD delta = (dayOfWeek + 7 - st.wDayOfWeek) % 7;
	if (delta)
	{
		ft.AddDays(delta);
		ft.ToSystemTime(st);
	}
	WORD last = GetLastDayOfMonth(year, month);
	st.wDay += ((last - st.wDay) / 7) * 7;
	year = st.wYear;
	month = st.wMonth;
	day = st.wDay;
}


void hnrt::GetWeekDay(WORD& year, WORD& month, WORD& day)
{
	SYSTEMTIME st = { 0 };
	st.wYear = year;
	st.wMonth = month;
	st.wDay = day;
	FileTime ft(st);
	ft.ToSystemTime(st);
	if (st.wDayOfWeek == 0)
	{
		ft.AddDays(1);
		ft.ToSystemTime(st);
	}
	else if (st.wDayOfWeek == 6)
	{
		ft.AddDays(2);
		ft.ToSystemTime(st);
	}
	year = st.wYear;
	month = st.wMonth;
	day = st.wDay;
}


void hnrt::GetDayOfWeek(WORD& year, WORD& month, WORD& day, int dayOfWeek, int ordinal)
{
	if (dayOfWeek < 0 || 6 < dayOfWeek)
	{
		throw Exception(L"GetDayOfWeek: Bad day of week: %d", dayOfWeek);
	}
	if (ordinal < 1 || 5 < ordinal)
	{
		throw Exception(L"GetDayOfWeek: Bad ordinal: %d", ordinal);
	}
	SYSTEMTIME st = { 0 };
	st.wYear = year;
	st.wMonth = month;
	st.wDay = day;
	FileTime ft(st);
	ft.ToSystemTime(st);
	WORD delta = (dayOfWeek + 7 - st.wDayOfWeek) % 7;
	if (delta)
	{
		ft.AddDays(delta);
		ft.ToSystemTime(st);
	}
	while (true)
	{
		if ((st.wDay + 6) / 7 == ordinal)
		{
			break;
		}
		ft.AddDays(7);
		ft.ToSystemTime(st);
	}
	year = st.wYear;
	month = st.wMonth;
	day = st.wDay;
}


String hnrt::SystemTimeToString(const SYSTEMTIME& st)
{
	return String(PRINTF, L"%04d-%02d-%02dT%02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}
