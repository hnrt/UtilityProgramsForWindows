#include "pch.h"
#include "hnrt/GTIN13Exception.h"


using namespace hnrt;


GTIN13Exception::GTIN13Exception(int nReason, int nOffset, PCWSTR pszFormat, ...)
	: Exception()
	, m_nReason(nReason)
	, m_nOffset(nOffset)
{
	va_list argList;
	va_start(argList, pszFormat);
	m_szMessage.VaFormat(pszFormat, argList);
	va_end(argList);
}


GTIN13Exception::GTIN13Exception(const GTIN13Exception& other)
	: Exception(other)
	, m_nReason(other.m_nReason)
	, m_nOffset(other.m_nOffset)
{
}


GTIN13Exception& GTIN13Exception::operator =(const GTIN13Exception& other)
{
	(void)Exception::operator =(other);
	m_nReason = other.m_nReason;
	m_nOffset = other.m_nOffset;
	return *this;
}
