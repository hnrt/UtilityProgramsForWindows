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
    if (!psz)
    {
        return L"";
    }
    else if (!*psz)
    {
        return L"";
    }
    SpinLock lock(m_lockUcs);
    UcsStringSet::const_iterator iter = m_setUcs.find(psz);
    if (iter != m_setUcs.end())
    {
        return *iter;
    }
    PWSTR psz2 = Clone(psz);
    m_setUcs.insert(psz2);
    return psz2;
}


PCSTR StringStoreInternal::Get(PCSTR psz)
{
    if (!psz)
    {
        return "";
    }
    else if (!*psz)
    {
        return "";
    }
    SpinLock lock(m_lockAcp);
    AcpStringSet::const_iterator iter = m_setAcp.find(psz);
    if (iter != m_setAcp.end())
    {
        return *iter;
    }
    PSTR psz2 = Clone(psz);
    m_setAcp.insert(psz2);
    return psz2;
}


PCWSTR StringStoreInternal::Set(PWSTR psz)
{
    if (!psz)
    {
        return L"";
    }
    else if (!*psz)
    {
        free(psz);
        return L"";
    }
    SpinLock lock(m_lockUcs);
    UcsStringSet::const_iterator iter = m_setUcs.find(psz);
    if (iter != m_setUcs.end())
    {
        free(psz);
        return *iter;
    }
    m_setUcs.insert(psz);
    return psz;
}


PCSTR StringStoreInternal::Set(char* psz)
{
    if (!psz)
    {
        return "";
    }
    else if (!*psz)
    {
        free(psz);
        return "";
    }
    SpinLock lock(m_lockAcp);
    AcpStringSet::const_iterator iter = m_setAcp.find(psz);
    if (iter != m_setAcp.end())
    {
        free(psz);
        return *iter;
    }
    m_setAcp.insert(psz);
    return psz;
}


PCWSTR StringStoreInternal::Set(StringBuffer& buf)
{
    if (!buf.Ptr)
    {
        return L"";
    }
    else if (!*buf.Ptr)
    {
        return L"";
    }
    SpinLock lock(m_lockUcs);
    UcsStringSet::const_iterator iter = m_setUcs.find(buf.Ptr);
    if (iter != m_setUcs.end())
    {
        return *iter;
    }
    PWSTR psz = buf.Detach();
    m_setUcs.insert(psz);
    return psz;
}
