#include "pch.h"
#include "hnrt/Exception.h"


using namespace hnrt;


Exception::Exception()
    : m_szMessage()
{
}


Exception::Exception(PCWSTR pszFormat, ...)
    : m_szMessage()
{
    va_list argList;
    va_start(argList, pszFormat);
    m_szMessage = String(pszFormat, argList);
    va_end(argList);
}


Exception::Exception(const Exception& src)
    : m_szMessage(src.m_szMessage)
{
}


Exception& Exception::operator =(const Exception& src)
{
    m_szMessage = src.m_szMessage;
    return *this;
}
