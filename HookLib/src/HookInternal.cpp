#include "pch.h"
#include "HookInternal.h"
#include "hnrt/Debug.h"


using namespace hnrt;


HookInternal::HookInternal()
    : m_Lock(SPIN_LOCK_INITIALIZER)
    , m_ModuleOverrideMap()
    , m_Originals()
    , m_Replacements()
    , m_TimeZone()
    , m_TimeDisplacement(0)
{
    DBGPUT(L"HookInternal::ctor: Started.");
    GetTimeZoneInformation(&m_TimeZone);
    InitializeMaps();
    Install();
    DBGPUT(L"HookInternal::ctor: Ended.");
}


HookInternal::~HookInternal()
{
    DBGPUT(L"HookInternal::dtor: Started.");
    Uninstall();
    DBGPUT(L"HookInternal::dtor: Ended.");
}


void* HookInternal::getOriginal(const char* pszKey) const
{
    Map::const_iterator iter = m_Originals.find(pszKey);
    return iter != m_Originals.cend() ? iter->second : nullptr;
}


void* HookInternal::getReplacement(const char* pszKey) const
{
    Map::const_iterator iter = m_Replacements.find(pszKey);
    return iter != m_Replacements.cend() ? iter->second : nullptr;
}


void HookInternal::ApplyTimeDisplacementTo(FILETIME& ft) const
{
    LARGE_INTEGER li = { 0 };
    li.HighPart = ft.dwHighDateTime;
    li.LowPart = ft.dwLowDateTime;
    li.QuadPart += m_TimeDisplacement;
    ft.dwHighDateTime = li.HighPart;
    ft.dwLowDateTime = li.LowPart;
}


void HookInternal::ApplyTimeZoneTo(SYSTEMTIME& st) const
{
    SYSTEMTIME utcTime = st;
    SystemTimeToTzSpecificLocalTime(&m_TimeZone, &utcTime, &st);
}
