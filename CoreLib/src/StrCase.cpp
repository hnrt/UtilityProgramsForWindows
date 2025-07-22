#include "pch.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


SIZE_T hnrt::StrCase(StringOptions option, WCHAR* str, SSIZE_T count)
{
	switch (option)
	{
	case UPPERCASE:
		return StrUpr(str, count);
	case LOWERCASE:
		return StrLwr(str, count);
	default:
		return 0;
	}
}


SIZE_T hnrt::StrCase(StringOptions option, CHAR* str, SSIZE_T count)
{
	switch (option)
	{
	case UPPERCASE:
		return StrUpr(str, count);
	case LOWERCASE:
		return StrLwr(str, count);
	default:
		return 0;
	}
}
