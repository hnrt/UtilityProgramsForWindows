#include "pch.h"
#include "hnrt/CryptographyException.h"


using namespace hnrt;


CryptographyException::CryptographyException(LONG status, PCWSTR pszFormat, ...)
	: Exception()
	, m_Status(status)
{
	va_list argList;
	va_start(argList, pszFormat);
	m_szMessage.VaFormat(pszFormat, argList);
	va_end(argList);
}


CryptographyException::CryptographyException(const CryptographyException& other)
	: Exception(other)
	, m_Status(other.m_Status)
{
}


CryptographyException& CryptographyException::operator =(const CryptographyException& other)
{
	m_szMessage = other.m_szMessage;
	m_Status = other.m_Status;
	return *this;
}
