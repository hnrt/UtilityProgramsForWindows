#include "pch.h"
#include "GTIN13.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


WCHAR GTIN13::ComputeCheckDigit(const WCHAR s[])
{
	int sum0 = (s[0] - L'0') + (s[2] - L'0') + (s[4] - L'0') + (s[6] - L'0') + (s[8] - L'0') + (s[10] - L'0');
	int sum1 = (s[1] - L'0') + (s[3] - L'0') + (s[5] - L'0') + (s[7] - L'0') + (s[9] - L'0') + (s[11] - L'0');
	return L"0987654321"[(sum0 + sum1 * 3) % 10];
}


static void CheckGS1CompanyPrefixLength(int nGS1CompanyPrefixLength)
{
	switch (nGS1CompanyPrefixLength)
	{
	case GS1COMPANYPREFIX_LENGTH7:
	case GS1COMPANYPREFIX_LENGTH9:
	case GS1COMPANYPREFIX_LENGTH10:
		break;
	default:
		throw Exception(L"GTIN13::ctor: Bad length: %d", nGS1CompanyPrefixLength);
	}
}


GTIN13::GTIN13(int nGS1CompanyPrefixLength)
	: m_d()
	, m_GS1CompanyPrefixLength(nGS1CompanyPrefixLength)
{
	CheckGS1CompanyPrefixLength(nGS1CompanyPrefixLength);
	Set(L"");
}


GTIN13::GTIN13(PCWSTR psz, int nGS1CompanyPrefixLength)
	: m_d()
	, m_GS1CompanyPrefixLength(nGS1CompanyPrefixLength)
{
	CheckGS1CompanyPrefixLength(nGS1CompanyPrefixLength);
	Set(psz);
}


void GTIN13::Set(PCWSTR psz)
{
	SIZE_T length = 0;
	while (length < GTIN13_LENGTH_EXCLUDING_CD)
	{
		if (IsDigit(psz[length]))
		{
			length++;
		}
		else
		{
			break;
		}
	}
	if (length == GTIN13_LENGTH_EXCLUDING_CD)
	{
		MemCpy(m_d, psz, GTIN13_LENGTH_EXCLUDING_CD);
	}
	else if (length >= m_GS1CompanyPrefixLength)
	{
		WCHAR* pDst = &m_d[0];
		pDst += MemCpy(pDst, psz, m_GS1CompanyPrefixLength);
		pDst += MemSet(pDst, L'0', GTIN13_LENGTH_EXCLUDING_CD - length);
		MemCpy(pDst, psz + m_GS1CompanyPrefixLength, length - m_GS1CompanyPrefixLength);
	}
	else if (length >= GS1PREFIX_LENGTH)
	{
		WCHAR* pDst = &m_d[0];
		pDst += MemCpy(pDst, psz, GS1PREFIX_LENGTH);
		pDst += MemSet(pDst, L'0', m_GS1CompanyPrefixLength - length);
		pDst += MemCpy(pDst, psz + GS1PREFIX_LENGTH, length - GS1PREFIX_LENGTH);
		MemSet(pDst, L'0', ItemReferenceLenth);
	}
	else
	{
		MemSet(m_d, L'0', GTIN13_LENGTH_EXCLUDING_CD);
	}
	m_d[GTIN13_LENGTH_EXCLUDING_CD] = ComputeCheckDigit(m_d);
	m_d[GTIN13_LENGTH] = L'\0';
}
