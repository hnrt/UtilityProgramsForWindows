#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"


using namespace hnrt;


String::String()
    : m_psz(nullptr)
{
}


SIZE_T String::get_Length() const
{
    return m_psz ? RefStr::Get(m_psz).Length : 0;
}


void String::set_Length(SIZE_T length)
{
    RefStr::Get(m_psz).Truncate(length);
}
