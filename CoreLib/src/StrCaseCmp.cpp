#include "pch.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


int hnrt::StrCaseCmp(PCWSTR str1, SSIZE_T count1, PCWSTR str2, SSIZE_T count2)
{
	switch (CompareStringW(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, str1, static_cast<int>(count1), str2, static_cast<int>(count2)))
	{
	case CSTR_EQUAL:
		return 0;
	case CSTR_LESS_THAN:
		return -1;
	case CSTR_GREATER_THAN:
		return 1;
	default:
		throw Exception(L"CompareStringW(LOCALE_INVARIANT,LINGUISTIC_IGNORECASE) failed.");
	}
}


int hnrt::StrCaseCmp(PCSTR str1, SSIZE_T count1, PCSTR str2, SSIZE_T count2)
{
	switch (CompareStringA(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, str1, static_cast<int>(count1), str2, static_cast<int>(count2)))
	{
	case CSTR_EQUAL:
		return 0;
	case CSTR_LESS_THAN:
		return -1;
	case CSTR_GREATER_THAN:
		return 1;
	default:
		throw Exception(L"CompareStringA(LOCALE_INVARIANT,LINGUISTIC_IGNORECASE) failed.");
	}
}
