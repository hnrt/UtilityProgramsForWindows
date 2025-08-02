#include "pch.h"
#include "hnrt/Time.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


FileTime& FileTime::From(const SYSTEMTIME& st)
{
	if (!SystemTimeToFileTime(&st, this))
	{
		throw Win32Exception(GetLastError(), L"SystemTimeToFileTime failed.");
	}
	return *this;
}
