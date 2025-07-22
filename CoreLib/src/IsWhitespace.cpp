#include "pch.h"
#include "hnrt/StringCommons.h"


BOOL hnrt::IsWhitespace(WCHAR c)
{
	return iswspace(c) ? TRUE : FALSE;
}


BOOL hnrt::IsWhitespace(CHAR c)
{
	return isspace(c) ? TRUE : FALSE;
}
