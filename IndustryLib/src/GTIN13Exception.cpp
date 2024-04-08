#include "pch.h"
#include "hnrt/GTIN13Exception.h"


using namespace hnrt;


GTIN13Exception::GTIN13Exception(int nReason, int nOffset, PCWSTR pszFormat, ...)
	: AnyCodeException(nReason, nOffset)
{
	va_list argList;
	va_start(argList, pszFormat);
	m_szMessage.VaFormat(pszFormat, argList);
	va_end(argList);
}


GTIN13Exception::GTIN13Exception(const GTIN13Exception& other)
	: AnyCodeException(other)
{
}


GTIN13Exception& GTIN13Exception::operator =(const GTIN13Exception& other)
{
	(void)AnyCodeException::operator =(other);
	return *this;
}
