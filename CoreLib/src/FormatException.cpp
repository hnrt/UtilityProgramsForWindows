#include "pch.h"
#include "hnrt/FormatException.h"


using namespace hnrt;


FormatException::FormatException(SIZE_T uOffset)
	: Exception()
	, m_Offset(uOffset)
{
}


FormatException::FormatException(const FormatException& src)
	: Exception(src)
	, m_Offset(src.m_Offset)
{
}


FormatException& FormatException::operator =(const FormatException& src)
{
	Exception::operator =(src);
	m_Offset = src.m_Offset;
	return *this;
}
