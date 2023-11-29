#include "pch.h"
#include "hnrt/ResourceString.h"


using namespace hnrt;


HINSTANCE ResourceString::m_hInstance = NULL;


ResourceString::ResourceString(UINT uId)
    : String()
{
    LPWSTR psz = nullptr;
    size_t len = LoadStringW(m_hInstance, uId, reinterpret_cast<LPWSTR>(&psz), 0);
    if (len)
    {
        (void)Assign(psz, len);
    }
}


ResourceString::ResourceString(const ResourceString& other)
    : String(other)
{
}


ResourceString::~ResourceString()
{
    String::~String();
}


ResourceString& ResourceString::operator =(const ResourceString& other)
{
    return reinterpret_cast<ResourceString&>(Assign(other));
}
