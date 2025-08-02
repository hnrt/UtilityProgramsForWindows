#include "pch.h"
#include "hnrt/Time.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


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
