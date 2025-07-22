#include "pch.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


int hnrt::IndexOf(const WCHAR* str, int c, SSIZE_T size)
{
	if (IS_BMP(c))
	{
		const WCHAR* p = wmemchr(str, static_cast<wchar_t>(c), size >= 0 ? size : c ? StrLen(str) : INT_MAX);
		return p ? static_cast<int>(p - str) : -1;
	}
	else
	{
		WCHAR seq[3] = { 0 };
		seq[0] = HIGH_SURROGATE(c);
		seq[1] = LOW_SURROGATE(c);
		return IndexOf(str, seq, size);
	}
}


int hnrt::IndexOf(const CHAR* str, int c, SSIZE_T size)
{
	const CHAR* p = reinterpret_cast<const CHAR*>(memchr(str, c & 0xFF, size >= 0 ? size : c ? StrLen(str) : INT_MAX));
	return p ? static_cast<int>(p - str) : -1;
}


int hnrt::IndexOf(const WCHAR* str, const WCHAR* seq, SSIZE_T size)
{
	WCHAR c = *seq++;
	if (c)
	{
		SIZE_T cch = StrLen(seq);
		if (cch)
		{
			const WCHAR* pCur = str;
			const WCHAR* pEnd = str + (size < 0 ? StrLen(str) : size) - cch;
			if (pCur < pEnd)
			{
				const WCHAR* pStop;
				while ((pStop = wmemchr(pCur, c, pEnd - pCur)))
				{
					pCur = pStop + 1;
					if (!wmemcmp(pCur, seq, cch))
					{
						return static_cast<int>(pStop - str);
					}
				}
			}
			return -1;
		}
		else
		{
			return IndexOf(str, c, size);
		}
	}
	else
	{
		return size ? 0 : -1;
	}
}


int hnrt::IndexOf(const CHAR* str, const CHAR* seq, SSIZE_T size)
{
	int c = *seq++ & 0xFF;
	if (c)
	{
		SIZE_T cb = StrLen(seq);
		if (cb)
		{
			const CHAR* pCur = str;
			const CHAR* pEnd = str + (size < 0 ? StrLen(str) : size) - cb;
			if (pCur < pEnd)
			{
				const CHAR* pStop;
				while ((pStop = reinterpret_cast<const CHAR*>(memchr(pCur, c, pEnd - pCur))))
				{
					pCur = pStop + 1;
					if (!memcmp(pCur, seq, cb))
					{
						return static_cast<int>(pStop - str);
					}
				}
			}
			return -1;
		}
		else
		{
			return IndexOf(str, c, size);
		}
	}
	else
	{
		return size ? 0 : -1;
	}
}
