#include "pch.h"
#include "hnrt/StringCommons.h"


double hnrt::StrToDouble(PCWSTR str, PWSTR* stop)
{
	return wcstod(str, stop);
}


double hnrt::StrToDouble(PCSTR str, PSTR* stop)
{
	return strtod(str, stop);
}
