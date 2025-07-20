#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::Trim(StringOptions option)
{
    if (Len)
    {
        Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(option, Ptr)));
    }
    return *this;
}
