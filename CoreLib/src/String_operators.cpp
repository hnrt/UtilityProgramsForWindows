#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


bool String::operator ==(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) == 0;
}


bool String::operator !=(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) != 0;
}


bool String::operator <(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) < 0;
}


bool String::operator <=(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) <= 0;
}


bool String::operator >(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) > 0;
}


bool String::operator >=(const String& other) const
{
    return StrCmp(Ptr, -1, other.Ptr, -1) >= 0;
}


String String::operator +(const String& other) const
{
    return String(Ptr, other.Ptr);
}
