#include "pch.h"
#include "hnrt/StringUtils.h"


using namespace hnrt;


PWSTR hnrt::TrimString(PWSTR psz)
{
	PWCHAR p1 = psz;
	while (iswspace(*p1))
	{
		++p1;
	}
	PWCHAR p2 = p1 + wcslen(p1);
	while (p1 < p2)
	{
		--p2;
		if (!iswspace(*p2))
		{
			++p2;
			break;
		}
	}
	*p2 = L'\0';
	if (psz < p1)
	{
		wmemmove(psz, p1, p2 - p1 + 1);
	}
	return psz;
}
