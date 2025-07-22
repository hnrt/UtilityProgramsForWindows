#include "pch.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


int hnrt::LastIndexOf(const WCHAR* str, int c, SSIZE_T size)
{
	if (c)
	{
		if (IS_BMP(c))
		{
			const WCHAR* pCur = str + (size < 0 ? StrLen(str) : size);
			while (str <= --pCur)
			{
				if (pCur[0] == c)
				{
					return static_cast<int>(pCur - str);
				}
			}
		}
		else
		{
			int c1 = HIGH_SURROGATE(c);
			int c2 = LOW_SURROGATE(c);
			const WCHAR* pCur = str + (size < 0 ? StrLen(str) : size);
			while (str < --pCur)
			{
				if (pCur[0] == c2 && pCur[-1] == c1)
				{
					return static_cast<int>(&pCur[-1] - str);
				}
			}
		}
		return -1;
	}
	else if (size > 0)
	{
		const WCHAR* p = wmemchr(str, L'\0', size);
		return p ? static_cast<int>(p - str) : -1;
	}
	else if (size < 0)
	{
		return static_cast<int>(StrLen(str));
	}
	else
	{
		return -1;
	}
}


int hnrt::LastIndexOf(const CHAR* str, int c, SSIZE_T size)
{
	if (c)
	{
		const CHAR* pCur = str + StrLen(str, size);
		while (str <= --pCur)
		{
			if (*pCur == c)
			{
				return static_cast<int>(pCur - str);
			}
		}
		return -1;
	}
	else if (size > 0)
	{
		const CHAR* p = reinterpret_cast<const CHAR*>(memchr(str, '\0', size));
		return p ? static_cast<int>(p - str) : -1;
	}
	else if (size < 0)
	{
		return static_cast<int>(StrLen(str));
	}
	else
	{
		return -1;
	}
}


int hnrt::LastIndexOf(const WCHAR* str, const WCHAR* seq, SSIZE_T size)
{
	SIZE_T cch = StrLen(seq);
	if (cch)
	{
		WCHAR c = seq[--cch];
		if (cch)
		{
			const WCHAR* pCur = str + StrLen(str, size);
			const WCHAR* pEnd = str + cch;
			while (pEnd <= --pCur)
			{
				if (*pCur == c)
				{
					const WCHAR* pHead = pCur - cch;
					if (!wmemcmp(pHead, seq, cch))
					{
						return static_cast<int>(pHead - str);
					}
				}
			}
			return -1;
		}
		else
		{
			return LastIndexOf(str, c, size);
		}
	}
	else
	{
		return size ? static_cast<int>(StrLen(str, size)) : -1;
	}
}


int hnrt::LastIndexOf(const CHAR* str, const CHAR* seq, SSIZE_T size)
{
	SIZE_T cb = StrLen(seq);
	if (cb)
	{
		int c = seq[--cb] & 0xFF;
		if (cb)
		{
			const CHAR* pCur = str + StrLen(str, size);
			const CHAR* pEnd = str + cb;
			while (pEnd <= --pCur)
			{
				if (*pCur == c)
				{
					const CHAR* pHead = pCur - cb;
					if (!memcmp(pHead, seq, cb))
					{
						return static_cast<int>(pHead - str);
					}
				}
			}
			return -1;
		}
		else
		{
			return LastIndexOf(str, c, size);
		}
	}
	else
	{
		return size ? static_cast<int>(StrLen(str, size)) : -1;
	}
}
