#include "pch.h"
#include "hnrt/Time.h"
#include "hnrt/Exception.h"


using namespace hnrt;


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
