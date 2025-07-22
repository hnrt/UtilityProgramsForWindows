#include "pch.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


SIZE_T hnrt::StrCpy(WCHAR* dest, const WCHAR* src, SSIZE_T count)
{
	SIZE_T cch = StrLen(src, count);
	wmemcpy_s(dest, cch, src, cch);
	dest[cch] = L'\0';
	return cch;
}


SIZE_T hnrt::StrCpy(CHAR* dest, const CHAR* src, SSIZE_T count)
{
	SIZE_T cb = StrLen(src, count);
	memcpy_s(dest, cb, src, cb);
	dest[cb] = '\0';
	return cb;
}
