#include "pch.h"
#include "hnrt/StringCommons.h"


BOOL hnrt::IsDigit(WCHAR c)
{
	return iswdigit(c) ? TRUE : FALSE;
}


BOOL hnrt::IsDigit(CHAR c)
{
	return isdigit(c) ? TRUE : FALSE;
}
