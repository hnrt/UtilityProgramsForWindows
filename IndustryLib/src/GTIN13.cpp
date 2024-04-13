#include "pch.h"
#include "hnrt/GTIN13.h"
#include "hnrt/GTIN13Exception.h"
#include "hnrt/StringCommons.h"
#include <math.h>


using namespace hnrt;


GTIN13 GTIN13::Parse(PCWSTR psz, int nGS1CompanyPrefixLength)
{
	GTIN13 instance(nGS1CompanyPrefixLength);
	if (!psz)
	{
		throw GTIN13Exception(GTIN13_EMPTY, 0, L"Null was given.");
	}
	else if (!psz[0])
	{
		throw GTIN13Exception(GTIN13_EMPTY, 0, L"An empty string was given.");
	}
	for (int offset = 0; offset < nGS1CompanyPrefixLength; offset++)
	{
		if (!IsDigit(psz[offset]))
		{
			throw GTIN13Exception(GTIN13_MALFORMED_GS1COMPANYPREFIX, offset, L"Malformed GS1 Company Prefix at offset %d.", offset);
		}
	}
	for (int offset = nGS1CompanyPrefixLength; offset < GTIN13_LENGTH_EXCLUDING_CD; offset++)
	{
		if (!IsDigit(psz[offset]))
		{
			throw GTIN13Exception(GTIN13_MALFORMED_ITEMREFERENCE, offset, L"Malformed Item Reference at offset %d.", offset);
		}
	}
	WCHAR cd = ComputeCheckDigit(psz);
	if (psz[GTIN13_LENGTH_EXCLUDING_CD] != L'\0' && psz[GTIN13_LENGTH_EXCLUDING_CD] != cd)
	{
		throw GTIN13Exception(GTIN13_INCORRECT_CHECKDIGIT, GTIN13_LENGTH_EXCLUDING_CD, L"Incorrect check digit. Expected=%c", cd);
	}
	MemCpy(instance.m_d, psz, GTIN13_LENGTH_EXCLUDING_CD);
	instance.m_d[GTIN13_LENGTH_EXCLUDING_CD] = cd;
	instance.m_d[GTIN13_LENGTH] = L'\0';
	return instance;
}


WCHAR GTIN13::ComputeCheckDigit(const WCHAR s[])
{
	int sum0 = (s[0] - L'0') + (s[2] - L'0') + (s[4] - L'0') + (s[6] - L'0') + (s[8] - L'0') + (s[10] - L'0');
	int sum1 = (s[1] - L'0') + (s[3] - L'0') + (s[5] - L'0') + (s[7] - L'0') + (s[9] - L'0') + (s[11] - L'0');
	return L"0987654321"[(sum0 + sum1 * 3) % 10];
}


GTIN13::GTIN13(int nGS1CompanyPrefixLength)
	: m_d()
	, m_GS1CompanyPrefixLength(nGS1CompanyPrefixLength)
{
	switch (nGS1CompanyPrefixLength)
	{
	case GS1COMPANYPREFIX_LENGTH7:
	case GS1COMPANYPREFIX_LENGTH9:
	case GS1COMPANYPREFIX_LENGTH10:
		break;
	default:
		throw GTIN13Exception(GTIN13_INVALID_GS1COMPANYPREFIXLENGTH, nGS1CompanyPrefixLength, L"GS1 Company Prefix: Bad length: %d", nGS1CompanyPrefixLength);
	}
}


GTIN13& GTIN13::operator =(PCWSTR psz)
{
	GTIN13 instance = Parse(psz, m_GS1CompanyPrefixLength);
	MemCpy(m_d, instance.m_d, GTIN13_LENGTH);
	return *this;
}


GTIN13& GTIN13::operator +=(int delta)
{
	int nItemReference = StrToLong(ItemReference, nullptr, 10);
	nItemReference += delta;
	int nUpperBound = static_cast<int>(pow(10, ItemReferenceLenth));
	if (nItemReference >= nUpperBound)
	{
		nItemReference %= nUpperBound;
	}
	else if (nItemReference < 0)
	{
		do {
			nItemReference += nUpperBound;
		} while (nItemReference < 0);
	}
	_snwprintf_s(m_d, _TRUNCATE, L"%s%0*d", GS1CompanyPrefix.Ptr, ItemReferenceLenth, nItemReference);
	m_d[GTIN13_LENGTH_EXCLUDING_CD] = ComputeCheckDigit(m_d);
	m_d[GTIN13_LENGTH] = L'\0';
	return *this;
}


GTIN13& GTIN13::operator -=(int delta)
{
	return GTIN13::operator +=(-delta);
}


BOOL GTIN13::IsValid(WCHAR c)
{
	return IsDigit(c);
}
