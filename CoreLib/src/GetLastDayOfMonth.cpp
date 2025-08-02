#include "pch.h"
#include "hnrt/Time.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


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
