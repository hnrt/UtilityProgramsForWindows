#include "pch.h"
#include "hnrt/HexFormatException.h"


using namespace hnrt;


HexFormatException::HexFormatException(SIZE_T uOffset, PCWSTR pszFormat, ...)
	: FormatException(uOffset)
{
	va_list argList;
	va_start(argList, pszFormat);
	m_szMessage = String(pszFormat, argList);
	va_end(argList);
}


HexFormatException::HexFormatException(const HexFormatException& src)
	: FormatException(src)
{
}


HexFormatException& HexFormatException::operator =(const HexFormatException& src)
{
	FormatException::operator =(src);
	return *this;
}
