#include "pch.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


SIZE_T hnrt::CountCharacter(PCWCH str, SSIZE_T count, int codepoint)
{
	SIZE_T retval = 0;
	PCWCH pCur = str;
	PCWCH pEnd = str + (count < 0 ? StrLen(str) : count);
	while (pCur < pEnd)
	{
		int c = *pCur++;
		if (IS_HIGH_SURROGATE(c) && pCur < pEnd && IS_LOW_SURROGATE(*pCur))
		{
			int c1 = c;
			int c2 = *pCur++;
			if (IS_LOW_SURROGATE(c2))
			{
				c = (((c1 - HIGH_SURROGATE_START) << 10) | ((c2 - LOW_SURROGATE_START) << 0)) + 0x10000;
			}
			else
			{
				pCur--;
			}
		}
		if (c == codepoint)
		{
			retval++;
		}
	}
	return retval;
}


SIZE_T hnrt::CountCharacter(PCCH str, SSIZE_T count, int codepoint)
{
	SIZE_T retval = 0;
	PCCH pCur = str;
	PCCH pEnd = str + (count < 0 ? StrLen(str) : count);
	while (pCur < pEnd)
	{
		int c = *pCur++;
		if (c == codepoint)
		{
			retval++;
		}
	}
	return retval;
}
