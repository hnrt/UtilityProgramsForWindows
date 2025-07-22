#include "pch.h"
#include "hnrt/StringCommons.h"


SIZE_T hnrt::MemSet(WCHAR* dest, WCHAR c, SIZE_T count)
{
	wmemset(dest, c, count);
	return count;
}


SIZE_T hnrt::MemSet(CHAR* dest, CHAR c, SIZE_T count)
{
	memset(dest, c, count);
	return count;
}
