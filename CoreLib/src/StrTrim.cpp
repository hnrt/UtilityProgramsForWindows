#include "pch.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


template<typename T>
void DoTrimScan(const T* str, int& start, int& end, StringOptions option)
{
	const T* pCur = str;
	T c = *pCur++;
	if (option == TRIM_TAIL)
	{
		start = 0;
	}
	else
	{
		while (IsWhitespace(c))
		{
			c = *pCur++;
		}
		start = static_cast<int>(pCur - 1 - str);
		if (c)
		{
			c = *pCur++;
		}
	}
	if (option == TRIM_HEAD)
	{
		while (c)
		{
			c = *pCur++;
		}
		end = static_cast<int>(pCur - 1 - str);
	}
	else
	{
		while (true)
		{
			if (IsWhitespace(c))
			{
				end = static_cast<int>(pCur - 1 - str);
				c = *pCur++;
				while (IsWhitespace(c))
				{
					c = *pCur++;
				}
				if (c)
				{
					c = *pCur++;
				}
				else
				{
					break;
				}
			}
			else if (c)
			{
				c = *pCur++;
			}
			else
			{
				end = static_cast<int>(pCur - 1 - str);
				break;
			}
		}
	}
}


void hnrt::StrTrimScan(const WCHAR* str, int& start, int& end, StringOptions option)
{
	DoTrimScan(str, start, end, option);
}


void hnrt::StrTrimScan(const CHAR* str, int& start, int& end, StringOptions option)
{
	DoTrimScan(str, start, end, option);
}


WCHAR* hnrt::StrTrim(WCHAR* str, StringOptions option)
{
	int start = 0;
	int end = 0;
	StrTrimScan(str, start, end, option);
	int len = end - start;
	wmemmove_s(str, len, str + start, len);
	str[len] = L'\0';
	return str;
}


CHAR* hnrt::StrTrim(CHAR* str, StringOptions option)
{
	int start = 0;
	int end = 0;
	StrTrimScan(str, start, end, option);
	int len = end - start;
	memmove_s(str, len, str + start, len);
	str[len] = '\0';
	return str;
}
