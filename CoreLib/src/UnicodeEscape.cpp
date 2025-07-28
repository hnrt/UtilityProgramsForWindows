#include "pch.h"
#include "hnrt/UnicodeEscape.h"
#include "hnrt/RefStr.h"


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


String hnrt::FromNativeToAscii(PCWSTR psz, bool bLowercase)
{
	String szText(wcslen(psz) * 6ULL);
	WCHAR* pOut = szText.Buf;
	const WCHAR* pCur = psz;
	const WCHAR* pHex = szHex[bLowercase ? 1 : 0];
	WCHAR c;
	while ((c = *pCur++) != L'\0')
	{
		if (0 < c && c < 0x80)
		{
			*pOut++ = c;
		}
		else
		{
			pOut += 6;
			pOut[-1] = pHex[c & 0xF]; c >>= 4;
			pOut[-2] = pHex[c & 0xF]; c >>= 4;
			pOut[-3] = pHex[c & 0xF]; c >>= 4;
			pOut[-4] = pHex[c & 0xF];
			pOut[-5] = L'u';
			pOut[-6] = L'\\';
		}
	}
	RefStr::Get(szText.Buf).Truncate(pOut - szText.Buf);
	return szText;
}


String hnrt::FromAsciiToNative(PCWSTR psz)
{
	String szText(wcslen(psz));
	WCHAR* pOut = szText.Buf;
	const WCHAR* pCur = psz;
	WCHAR c;
	while ((c = *pCur++) != L'\0')
	{
		if (c == L'\\')
		{
			c = *pCur++;
			if (c == L'\0')
			{
				*pOut++ = L'\\';
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
				*pOut++ = L'\\';
			}
		}
		*pOut++ = c;
	}
	RefStr::Get(szText.Buf).Truncate(pOut - szText.Buf);
	return szText;
}
