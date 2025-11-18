#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String::String(const String& src)
    : m_psz(StringAddRef(src.m_psz))
{
}
