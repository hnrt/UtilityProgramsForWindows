#include "pch.h"
#include "hnrt/HexFormatException.h"


using namespace hnrt;


HexFormatException::HexFormatException()
	: Exception()
	, m_Offset(0)
{
}


HexFormatException::HexFormatException(SIZE_T uOffset, PCWSTR pszFormat, ...)
	: Exception()
	, m_Offset(uOffset)
{
	va_list argList;
	va_start(argList, pszFormat);
	m_szMessage = String(pszFormat, argList);
	va_end(argList);
}


HexFormatException::HexFormatException(const HexFormatException& src)
	: Exception(src)
	, m_Offset(src.m_Offset)
{
}


HexFormatException& HexFormatException::operator =(const HexFormatException& src)
{
	Exception::operator =(src);
	m_Offset = src.m_Offset;
	return *this;
}
