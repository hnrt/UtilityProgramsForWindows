#include "pch.h"
#include "hnrt/StringCommons.h"


SIZE_T hnrt::MemCpy(WCHAR* dest, const WCHAR* src, SIZE_T count)
{
	wmemcpy_s(dest, count, src, count);
	return count;
}


SIZE_T hnrt::MemCpy(CHAR* dest, const CHAR* src, SIZE_T count)
{
	memcpy_s(dest, count, src, count);
	return count;
}
