#include "pch.h"
#include "hnrt/StringCommons.h"


long long hnrt::StrToLongLong(PCWSTR str, PWSTR* stop, int radix)
{
	return wcstoll(str, stop, radix);
}


long long hnrt::StrToLongLong(PCSTR str, PSTR* stop, int radix)
{
	return strtoll(str, stop, radix);
}
