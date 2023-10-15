#include "pch.h"
#include "hnrt/Time.h"


ULONGLONG hnrt::GetCurrentMicroSeconds()
{
	LARGE_INTEGER li = { 0 };
	FILETIME ft = { 0 };
	SYSTEMTIME st = { 0 };
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	return (li.QuadPart + 5) / 10;
}