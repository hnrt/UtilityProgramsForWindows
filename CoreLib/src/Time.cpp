#include "pch.h"
#include "hnrt/Time.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


FileTime::FileTime()
	: m_ft()
{
	GetSystemTimeAsFileTime(&m_ft);
}


FileTime::FileTime(const SYSTEMTIME& st)
	: m_ft()
{
	(void)From(st);
}


FileTime& FileTime::FromSystemTime()
{
	GetSystemTimeAsFileTime(&m_ft);
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
	if (!SystemTimeToFileTime(&st, &m_ft))
	{
		throw Win32Exception(GetLastError(), L"SystemTimeToFileTime failed.");
	}
	return *this;
}


SYSTEMTIME& FileTime::ToSystemTime(SYSTEMTIME& st) const
{
	if (!FileTimeToSystemTime(&m_ft, &st))
	{
		throw Win32Exception(GetLastError(), L"FileTimeToSystemTime failed.");
	}
	return st;
}


void hnrt::GetLastDayOfMonth(int year, int month, WORD& dayOfMonth)
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
		dayOfMonth = 31;
		break;
	case 2:
		if ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0))
		{
			dayOfMonth = 29;
		}
		else
		{
			dayOfMonth = 28;
		}
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		dayOfMonth = 30;
		break;
	default:
		throw Exception(L"GetLastDayOfMonth: Bad month: %d", month);
	}
}


void hnrt::GetLastDayOfMonth(WORD& year, WORD& month, WORD& dayOfMonth, int dayOfWeek)
{
	if (dayOfWeek < 0 || 6 < dayOfWeek)
	{
		throw Exception(L"GetLastDayOfMonth: Bad day of week: %d", dayOfWeek);
	}
	SYSTEMTIME st = { 0 };
	st.wYear = year;
	st.wMonth = month;
	st.wDay = dayOfMonth;
	FileTime ft(st);
	ft.ToSystemTime(st);
	while (true)
	{
		if (st.wDayOfWeek == dayOfWeek)
		{
			WORD last = 0;
			GetLastDayOfMonth(year, month, last);
			st.wDay += ((last - st.wDay) / 7) * 7;
			return;
		}
		ft.AddDays(1);
		ft.ToSystemTime(st);
	}
}


void hnrt::GetWeekDay(WORD& year, WORD& month, WORD& day)
{
	SYSTEMTIME st = { 0 };
	st.wYear = year;
	st.wMonth = month;
	st.wDay = day;
	FileTime ft(st);
	ft.ToSystemTime(st);
	while (st.wDayOfWeek == 0 || st.wDayOfWeek == 6)
	{
		ft.AddDays(1);
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
	while (true)
	{
		if (st.wDayOfWeek == dayOfWeek)
		{
			if ((st.wDay + 6) / 7 == ordinal)
			{
				year = st.wYear;
				month = st.wMonth;
				day = st.wDay;
				return;
			}
			ft.AddDays(7);
		}
		else
		{
			int delta = (dayOfWeek + 7 - st.wDayOfWeek) % 7;
			ft.AddDays(static_cast<LONGLONG>(delta));
		}
		ft.ToSystemTime(st);
	}
}
