#include "pch.h"
#include "hnrt/SFIDException.h"


using namespace hnrt;


SFIDException::SFIDException(int nReason, int nOffset, PCWSTR pszFormat, ...)
	: AnyCodeException(nReason, Offset)
{
	va_list argList;
	va_start(argList, pszFormat);
	m_szMessage.VaFormat(pszFormat, argList);
	va_end(argList);
}


SFIDException::SFIDException(const SFIDException& other)
	: AnyCodeException(other)
{
}


SFIDException& SFIDException::operator =(const SFIDException& other)
{
	(void)AnyCodeException::operator =(other);
	return *this;
}
