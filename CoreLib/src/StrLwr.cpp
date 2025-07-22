#include "pch.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


SIZE_T hnrt::StrLwr(WCHAR* str, SSIZE_T count)
{
	SIZE_T cch = count < 0 ? StrLen(str) : count;
	str[cch] = L'\0';
	errno_t ret = _wcslwr_s(str, cch + 1);
	if (ret)
	{
		WCHAR sz[256] = { 0 };
		throw Exception(L"%s", !_wcserror_s(sz, ret) ? sz : String(PRINTF, L"_wcslwr_s failed with errno of %d", ret));
	}
	return count < 0 ? cch : StrLen(str, count);
}


SIZE_T hnrt::StrLwr(CHAR* str, SSIZE_T count)
{
	SIZE_T cb = count < 0 ? StrLen(str) : count;
	str[cb] = '\0';
	errno_t ret = _strlwr_s(str, cb + 1);
	if (ret)
	{
		WCHAR sz[256] = { 0 };
		throw Exception(L"%s", !_wcserror_s(sz, ret) ? sz : String(PRINTF, L"_strlwr_s failed with errno of %d", ret));
	}
	return count < 0 ? cb : StrLen(str, count);
}
