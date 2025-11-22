#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


String String::Lettercase(StringOptions option) const
{
    String s(Ptr, Len);
    if (s.Len)
    {
        StrCase(option, s.m_psz, s.Len);
    }
    return s;
}
