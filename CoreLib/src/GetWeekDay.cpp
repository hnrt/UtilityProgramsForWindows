#include "pch.h"
#include "hnrt/Time.h"


using namespace hnrt;


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
