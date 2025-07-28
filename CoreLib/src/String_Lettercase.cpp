#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::Lettercase(StringOptions option)
{
    if (Len)
    {
        StrCase(option, m_psz, Len);
    }
    return *this;
}
