#include "pch.h"
#include "hnrt/StringCommons.h"


DWORD hnrt::CountStrings(PWCHAR pszz)
{
	DWORD dwCount = 0;
	PWCHAR pCur = pszz;
	do
	{
		dwCount++;
		pCur += wcslen(pCur) + 1;
	} while (*pCur);
	return dwCount;
}


DWORD hnrt::CountStrings(PCHAR pszz)
{
	DWORD dwCount = 0;
	PCHAR pCur = pszz;
	do
	{
		dwCount++;
		pCur += strlen(pCur) + 1;
	} while (*pCur);
	return dwCount;
}
