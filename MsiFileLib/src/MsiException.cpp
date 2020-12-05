#include "pch.h"
#include "hnrt/MsiException.h"
#include "hnrt/String.h"


using namespace hnrt;


MsiException::MsiException(UINT error)
    : Exception()
    , m_error(error)
{
}


MsiException::MsiException(UINT error, PCWSTR pszFormat, ...)
    : Exception()
    , m_error(error)
{
    va_list argList;
    va_start(argList, pszFormat);
    m_pszMessage = String::VaFormat(pszFormat, argList);
    va_end(argList);
}


MsiException::MsiException(PCWSTR pszFormat, ...)
    : Exception()
    , m_error(0)
{
    va_list argList;
    va_start(argList, pszFormat);
    m_pszMessage = String::VaFormat(pszFormat, argList);
    va_end(argList);
}


MsiException::MsiException(const MsiException& src)
    : Exception(src)
    , m_error(src.m_error)
{
}


MsiException& MsiException::operator =(const MsiException& src)
{
    Exception::operator =(src);
    m_error = src.m_error;
    return *this;
}
