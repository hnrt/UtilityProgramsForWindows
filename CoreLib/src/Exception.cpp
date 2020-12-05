#include "pch.h"
#include "hnrt/Exception.h"
#include "hnrt/String.h"


using namespace hnrt;


Exception::Exception()
    : m_pszMessage(nullptr)
{
}


Exception::Exception(PCWSTR pszFormat, ...)
    : m_pszMessage(nullptr)
{
    va_list argList;
    va_start(argList, pszFormat);
    m_pszMessage = String::VaFormat(pszFormat, argList);
    va_end(argList);
}


Exception::Exception(const Exception& src)
    : m_pszMessage(src.m_pszMessage)
{
}


Exception& Exception::operator =(const Exception& src)
{
    m_pszMessage = src.m_pszMessage;
    return *this;
}
