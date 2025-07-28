#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::ZeroFill()
{
    if (Len)
    {
        MemSet(m_psz, L'\0', Len);
    }
    return *this;
}
