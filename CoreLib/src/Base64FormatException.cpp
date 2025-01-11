#include "pch.h"
#include "hnrt/Base64FormatException.h"


using namespace hnrt;


Base64FormatException::Base64FormatException(SIZE_T uOffset, PCWSTR pszFormat, ...)
	: FormatException(uOffset)
{
	va_list argList;
	va_start(argList, pszFormat);
	m_szMessage = String(pszFormat, argList);
	va_end(argList);
}


Base64FormatException::Base64FormatException(const Base64FormatException& src)
	: FormatException(src)
{
}


Base64FormatException& Base64FormatException::operator =(const Base64FormatException& src)
{
	FormatException::operator =(src);
	return *this;
}
