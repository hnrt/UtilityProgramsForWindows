#include "pch.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


SIZE_T hnrt::StrFmtLen(const WCHAR* format, ...)
{
	va_list argList;
	va_start(argList, format);
	int cch = _vscwprintf(format, argList);
	va_end(argList);
	if (cch < 0)
	{
		throw Exception(L"_vscwprintf failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::StrFmtLen(const CHAR* format, ...)
{
	va_list argList;
	va_start(argList, format);
	int cb = _vscprintf(format, argList);
	va_end(argList);
	if (cb < 0)
	{
		throw Exception(L"_vscprintf failed.");
	}
	return static_cast<SIZE_T>(cb);
}
