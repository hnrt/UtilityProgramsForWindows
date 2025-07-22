#include "pch.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


SIZE_T hnrt::StrFmt(WCHAR* buf, SIZE_T bufsz, const WCHAR* format, ...)
{
	va_list argList;
	va_start(argList, format);
	int cch = _vsnwprintf_s(buf, bufsz, _TRUNCATE, format, argList);
	va_end(argList);
	if (cch < 0)
	{
		throw Exception(L"_vsnwprintf_s failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::StrFmt(CHAR* psz, SIZE_T bufsz, const CHAR* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	int cb = _vsnprintf_s(psz, bufsz, _TRUNCATE, pszFormat, argList);
	va_end(argList);
	if (cb < 0)
	{
		throw Exception(L"_vsnprintf_s failed.");
	}
	return static_cast<SIZE_T>(cb);
}
