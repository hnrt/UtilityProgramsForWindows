#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"


using namespace hnrt;


String::String()
    : m_psz(StringAddRef(Empty.m_psz))
{
}


SIZE_T String::get_Len() const
{
    return m_psz ? RefStr::Get(m_psz).Length : 0;
}
