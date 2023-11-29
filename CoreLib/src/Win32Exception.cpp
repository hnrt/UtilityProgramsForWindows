#include "pch.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/String.h"


using namespace hnrt;


Win32Exception::Win32Exception(unsigned long error)
    : Exception()
    , m_error(error)
{
}


Win32Exception::Win32Exception(unsigned long error, PCWSTR pszFormat, ...)
    : Exception()
    , m_error(error)
{
    va_list argList;
    va_start(argList, pszFormat);
    m_szMessage = String(pszFormat, argList);
    va_end(argList);
}


Win32Exception::Win32Exception(const Win32Exception& src)
    : Exception(src)
    , m_error(src.m_error)
{
}


Win32Exception& Win32Exception::operator =(const Win32Exception& src)
{
    Exception::operator =(src);
    m_error = src.m_error;
    return *this;
}
