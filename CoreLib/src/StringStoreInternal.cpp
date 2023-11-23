#include "pch.h"
#include "StringStoreInternal.h"
#include "hnrt/Heap.h"


using namespace hnrt;


static StringStoreInternal* s_pSingleton = nullptr;


StringStoreInternal& StringStoreInternal::Instance()
{
    if (!s_pSingleton)
    {
        s_pSingleton = new StringStoreInternal();
    }
    return *s_pSingleton;
}


StringStoreInternal::~StringStoreInternal()
{
}


StringStoreInternal::StringStoreInternal()
    : m_lockUcs(SPIN_LOCK_INITIALIZER)
    , m_setUcs()
    , m_lockAcp(SPIN_LOCK_INITIALIZER)
    , m_setAcp()
{
}


PCWSTR StringStoreInternal::Get(PCWSTR psz)
{
    if (!psz || !*psz)
    {
        return L"";
    }
    SpinLock lock(m_lockUcs);
    auto iter = m_setUcs.find(psz);
    if (iter != m_setUcs.end())
    {
        return *iter;
    }
    auto iter2 = m_setUcs.insert(String(psz));
    return *(iter2.first);
}


PCSTR StringStoreInternal::Get(PCSTR psz)
{
    if (!psz || !*psz)
    {
        return "";
    }
    SpinLock lock(m_lockAcp);
    auto iter = m_setAcp.find(psz);
    if (iter != m_setAcp.end())
    {
        return *iter;
    }
    auto iter2 = m_setAcp.insert(StringAcp(psz));
    return *(iter2.first);
}
