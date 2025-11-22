#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


String& String::ZeroFill()
{
    if (Len)
    {
        MemSet(m_psz, L'\0', Len);
    }
    return *this;
}
