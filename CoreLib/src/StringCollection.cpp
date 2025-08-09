#include "pch.h"
#include "hnrt/StringCollection.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"
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
    Resize(0);
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
    if (dwIndex < m_dwCount)
    {
        m_dwCount--;
        while (dwIndex < m_dwCount)
        {
            m_pBase[dwIndex] = m_pBase[dwIndex + 1];
            dwIndex++;
        }
        m_pBase[m_dwCount].~String();
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
    for (DWORD dwIndex = 0; dwIndex < Count; dwIndex++)
    {
        if (!String::Compare(operator [](dwIndex), psz))
        {
            return true;
        }
    }
    return false;
}
