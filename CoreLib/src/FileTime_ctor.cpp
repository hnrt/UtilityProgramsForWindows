#include "pch.h"
#include "hnrt/Time.h"


using namespace hnrt;


FileTime::FileTime()
	: FILETIME()
{
	GetSystemTimeAsFileTime(this);
}


FileTime::FileTime(const SYSTEMTIME& st)
	: FILETIME()
{
	From(st);
}


FileTime::FileTime(LONGLONG value)
	: FILETIME()
{
	LARGE_INTEGER li = { 0 };
	li.QuadPart = value;
	dwLowDateTime = li.LowPart;
	dwHighDateTime = li.HighPart;
}
