#include "pch.h"
#include "hnrt/StringCommons.h"


SIZE_T hnrt::MemMove(WCHAR* dest, const WCHAR* src, SIZE_T count)
{
	wmemmove_s(dest, count, src, count);
	return count;
}


SIZE_T hnrt::MemMove(CHAR* dest, const CHAR* src, SIZE_T count)
{
	memmove_s(dest, count, src, count);
	return count;
}
