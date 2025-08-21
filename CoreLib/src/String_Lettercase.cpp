#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String String::Lettercase(StringOptions option)
{
    String s(Ptr, Len);
    if (s.Len)
    {
        StrCase(option, s.m_psz, s.Len);
    }
    return s;
}
