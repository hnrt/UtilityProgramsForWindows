#include "pch.h"
#include "hnrt/ComException.h"


using namespace hnrt;


ComException::ComException()
    : Exception()
    , m_hr(E_FAIL)
{
}


ComException::ComException(HRESULT hr, PCWSTR pszFormat, ...)
    : Exception()
    , m_hr(hr)
{
    va_list argList;
    va_start(argList, pszFormat);
    m_szMessage = String(pszFormat, argList);
    va_end(argList);
}


ComException::ComException(const ComException& src)
    : Exception(src)
    , m_hr(src.m_hr)
{
}


ComException& ComException::operator =(const ComException& src)
{
    Exception::operator =(src);
    m_hr = src.m_hr;
    return *this;
}
