#include "pch.h"
#include "hnrt/StringCommons.h"


unsigned long long hnrt::StrToUnsignedLongLong(PCWSTR str, PWSTR* stop, int radix)
{
	return wcstoull(str, stop, radix);
}


unsigned long long hnrt::StrToUnsignedLongLong(PCSTR str, PSTR* stop, int radix)
{
	return strtoull(str, stop, radix);
}
