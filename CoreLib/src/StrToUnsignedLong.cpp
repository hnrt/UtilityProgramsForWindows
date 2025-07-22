#include "pch.h"
#include "hnrt/StringCommons.h"


unsigned long hnrt::StrToUnsignedLong(PCWSTR str, PWSTR* stop, int radix)
{
	return wcstoul(str, stop, radix);
}


unsigned long hnrt::StrToUnsignedLong(PCSTR str, PSTR* stop, int radix)
{
	return strtoul(str, stop, radix);
}
