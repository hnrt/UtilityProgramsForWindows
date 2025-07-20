#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::ZeroFill()
{
    if (Len)
    {
        RefStr::GetThis(m_psz)->Fill(L'\0');
    }
    return *this;
}
