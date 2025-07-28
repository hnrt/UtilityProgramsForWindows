#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String::~String()
{
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, nullptr));
}
