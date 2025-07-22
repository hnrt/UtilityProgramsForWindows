#include "pch.h"
#include "hnrt/StringCommons.h"


SIZE_T hnrt::StrLen(const WCHAR* str)
{
	return wcslen(str);
}


SIZE_T hnrt::StrLen(const CHAR* str)
{
	return strlen(str);
}


SIZE_T hnrt::StrLen(const WCHAR* str, SSIZE_T numberOfElements)
{
	return numberOfElements < 0 ? wcslen(str) : wcsnlen(str, numberOfElements);
}


SIZE_T hnrt::StrLen(const CHAR* str, SSIZE_T numberOfElements)
{
	return numberOfElements < 0 ? strlen(str) : strnlen(str, numberOfElements);
}
