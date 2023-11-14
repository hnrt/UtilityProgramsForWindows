#include "pch.h"
#include "hnrt/Exception.h"


using namespace hnrt;


Exception::Exception()
    : m_Message()
{
}


Exception::Exception(PCWSTR pszFormat, ...)
    : m_Message()
{
    va_list argList;
    va_start(argList, pszFormat);
    m_Message = String(pszFormat, argList);
    va_end(argList);
}


Exception::Exception(const Exception& src)
    : m_Message(src.m_Message)
{
}


Exception& Exception::operator =(const Exception& src)
{
    m_Message = src.m_Message;
    return *this;
}
