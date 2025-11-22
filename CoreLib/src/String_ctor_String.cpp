#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"


using namespace hnrt;


String::String(const String& src)
    : m_psz(StringAddRef(src.m_psz))
{
}
