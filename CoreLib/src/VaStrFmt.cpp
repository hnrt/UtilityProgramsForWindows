#include "pch.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


SIZE_T hnrt::VaStrFmt(WCHAR* psz, SIZE_T bufsz, const WCHAR* pszFormat, va_list argList)
{
	int cch = _vsnwprintf_s(psz, bufsz, _TRUNCATE, pszFormat, argList);
	if (cch < 0)
	{
		throw Exception(L"_vsnwprintf_s failed.");
	}
	return static_cast<SIZE_T>(cch);
}


SIZE_T hnrt::VaStrFmt(CHAR* psz, SIZE_T bufsz, const CHAR* pszFormat, va_list argList)
{
	int cb = _vsnprintf_s(psz, bufsz, _TRUNCATE, pszFormat, argList);
	if (cb < 0)
	{
		throw Exception(L"_vsnprintf_s failed.");
	}
	return static_cast<SIZE_T>(cb);
}
