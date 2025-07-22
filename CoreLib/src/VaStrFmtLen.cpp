#include "pch.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


SIZE_T hnrt::VaStrFmtLen(const WCHAR* format, va_list argList)
{
	va_list argList2;
	va_copy(argList2, argList);
	int cch = _vscwprintf(format, argList2);
	va_end(argList2);
	if (cch < 0)
	{
		throw Exception(L"_vscwprintf failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::VaStrFmtLen(const CHAR* format, va_list argList)
{
	va_list argList2;
	va_copy(argList2, argList);
	int cb = _vscprintf(format, argList2);
	va_end(argList2);
	if (cb < 0)
	{
		throw Exception(L"_vscprintf failed.");
	}
	return static_cast<SIZE_T>(cb);
}
