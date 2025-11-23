#include "pch.h"
#include <string.h>
#include <Windows.h>
#include "hnrt/StringCollection.h"
#include "hnrt/Array.h"
#include "hnrt/String.h"


using namespace hnrt;


StringCollection::StringCollection()
    : Array<String>()
{
}


StringCollection::StringCollection(const StringCollection& src)
    : Array<String>(src)
{
}


StringCollection& StringCollection::operator =(const StringCollection& src)
{
    Array<String>::operator =(src);
    return *this;
}


StringCollection& StringCollection::Clear()
{
    SetCapacity(0);
    return *this;
}


StringCollection& StringCollection::Add(PCWSTR psz)
{
    Array<String>::operator +=(String(psz));
    return *this;
}


StringCollection& StringCollection::Add(PCWSTR psz, size_t cch)
{
    Array<String>::operator +=(String(psz, cch));
    return *this;
}


StringCollection& StringCollection::RemoveAt(DWORD dwIndex)
{
    if (dwIndex < Length)
    {
        DWORD dwEnd = static_cast<DWORD>(Length);
        dwEnd--;
        while (dwIndex < dwEnd)
        {
            m_pBase[dwIndex] = m_pBase[dwIndex + 1];
            dwIndex++;
        }
        Length--;
    }
    return *this;
}


StringCollection& StringCollection::Split(PCWSTR psz, WCHAR sep, bool bAll)
{
    PCWSTR pStop;
    while ((pStop = wcschr(psz, sep)))
    {
        size_t len = pStop - psz;
        if (bAll || len)
        {
            Add(psz, len);
        }
        psz = pStop + 1;
    }
    if (bAll || *psz)
    {
        Add(psz);
    }
    return *this;
}


bool StringCollection::Contains(PCWSTR psz) const
{
    for (DWORD dwIndex = 0; dwIndex < Length; dwIndex++)
    {
        if (!String::Compare(At(dwIndex), psz))
        {
            return true;
        }
    }
    return false;
}
