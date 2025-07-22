#include "pch.h"
#include "hnrt/StringCommons.h"


long hnrt::StrToLong(PCWSTR str, PWSTR* stop, int radix)
{
	return wcstol(str, stop, radix);
}


long hnrt::StrToLong(PCSTR str, PSTR* stop, int radix)
{
	return strtol(str, stop, radix);
}
