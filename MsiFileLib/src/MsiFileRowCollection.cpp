#include "pch.h"
#include "MsiFileRowCollection.h"
#include "hnrt/Exception.h"


using namespace hnrt;


MsiFileRowCollection::MsiFileRowCollection()
    : IMsiFileRowCollection()
    , m_refs(1)
    , m_map()
{
}


long MsiFileRowCollection::AddRef()
{
    long refs = _InterlockedIncrement(&m_refs);
    return refs;
}


long MsiFileRowCollection::Release()
{
    long refs = _InterlockedDecrement(&m_refs);
    if (!refs)
    {
        delete this;
    }
    return refs;
}


long MsiFileRowCollection::get_Count() const
{
    return static_cast<long>(m_map.size());
}


void MsiFileRowCollection::Add(PCWSTR pszFile, long rowNo)
{
    if (!pszFile)
    {
        throw Exception(L"MsiFileRowCollection::Add: Null File.");
    }
    if (rowNo < 0L)
    {
        throw Exception(L"MsiFileRowCollection::Add: Row number out of range.");
    }
    m_map.insert_or_assign(pszFile, rowNo);
}


void MsiFileRowCollection::Remove(PCWSTR pszFile)
{
    if (!pszFile)
    {
        throw Exception(L"MsiFileRowCollection::Remove: Null File.");
    }
    m_map.erase(pszFile);
}


long MsiFileRowCollection::Find(PCWSTR pszFile) const
{
    Map::const_iterator iter = m_map.find(pszFile);
    return iter != m_map.end() ? iter->second : -1L;
}
