#include "pch.h"
#include "hnrt/UnicodeEscape.h"


static const WCHAR szHex[2][17] =
{
	{ L"0123456789ABCDEF" },
	{ L"0123456789abcdef" }
};


static int ToHex(WCHAR c)
{
	return (L'0' <= c && c <= L'9') ? (c - L'0')
		: (L'A' <= c && c <= L'F') ? (10 + c - L'A')
		: (L'a' <= c && c <= L'f') ? (10 + c - L'a')
		: -1;
}


static bool IsHex(WCHAR c)
{
	return (L'0' <= c && c <= L'9')
		|| (L'A' <= c && c <= L'F')
		|| (L'a' <= c && c <= L'f');
}


using namespace hnrt;


int hnrt::FromNativeToAscii(PCWSTR psz, PWCHAR pOut, bool bLowercase)
{
	int indexLettercase = bLowercase ? 1 : 0;
	WCHAR* pOutEnd = pOut;
	const WCHAR* pCur = psz;
	WCHAR c;
	if (pOut)
	{
		while ((c = *pCur++) != L'\0')
		{
			if (0 < c && c < 0x80)
			{
				*pOutEnd++ = c;
			}
			else
			{
				pOutEnd += 6;
				pOutEnd[-1] = szHex[indexLettercase][c & 0xF]; c >>= 4;
				pOutEnd[-2] = szHex[indexLettercase][c & 0xF]; c >>= 4;
				pOutEnd[-3] = szHex[indexLettercase][c & 0xF]; c >>= 4;
				pOutEnd[-4] = szHex[indexLettercase][c & 0xF];
				pOutEnd[-5] = L'u';
				pOutEnd[-6] = L'\\';
			}
		}
		*pOutEnd = L'\0';
	}
	else
	{
		while ((c = *pCur++) != L'\0')
		{
			if (0 < c && c < 0x80)
			{
				pOutEnd++;
			}
			else
			{
				pOutEnd += 6;
			}
		}
	}
	return static_cast<int>(pOutEnd - pOut);
}


int hnrt::FromAsciiToNative(PCWSTR psz, PWCHAR pOut)
{
	WCHAR* pOutEnd = pOut;
	const WCHAR* pCur = psz;
	WCHAR c;
	if (pOut)
	{
		while ((c = *pCur++) != L'\0')
		{
			if (c == L'\\')
			{
				c = *pCur++;
				if (c == L'\0')
				{
					*pOutEnd++ = L'\\';
					break;
				}
				else if ((c == L'u' || c == L'U') && IsHex(pCur[0]) && IsHex(pCur[1]) && IsHex(pCur[2]) && IsHex(pCur[3]))
				{
					c = ToHex(*pCur++);
					c <<= 4;
					c |= ToHex(*pCur++);
					c <<= 4;
					c |= ToHex(*pCur++);
					c <<= 4;
					c |= ToHex(*pCur++);
				}
				else
				{
					*pOutEnd++ = L'\\';
				}
			}
			*pOutEnd++ = c;
		}
		*pOutEnd = L'\0';
	}
	else
	{
		while ((c = *pCur++) != L'\0')
		{
			if (c == L'\\')
			{
				c = *pCur++;
				if (c == L'\0')
				{
					pOutEnd++;
					break;
				}
				else if ((c == L'u' || c == L'U') && IsHex(pCur[0]) && IsHex(pCur[1]) && IsHex(pCur[2]) && IsHex(pCur[3]))
				{
					pCur += 4;
				}
				else
				{
					pOutEnd++;
				}
			}
			pOutEnd++;
		}
	}
	return static_cast<int>(pOutEnd - pOut);
}
