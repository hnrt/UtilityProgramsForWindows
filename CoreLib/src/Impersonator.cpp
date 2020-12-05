#include "pch.h"
#include "hnrt/Impersonator.h"
#include "hnrt/SidPtr.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


Impersonator::Impersonator(DWORD dwAttributes, PSID psid)
    : m_hToken1()
    , m_hToken2()
{
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE | TOKEN_ADJUST_DEFAULT | TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY, &m_hToken1))
    {
        throw Win32Exception(GetLastError(), L"OpenProcessToken(DUPLICATE|ADJUST_DEFAULT|QUERY|ASSIGN_PRIMARY) failed.");
    }

    if (!DuplicateTokenEx(m_hToken1, 0, NULL, SecurityImpersonation, TokenPrimary, &m_hToken2))
    {
        throw Win32Exception(GetLastError(), L"DuplicateTokenEx(SecurityImpersonation,TokenPrimary) failed.");
    }

    TOKEN_MANDATORY_LABEL tml = { 0 };
    tml.Label.Attributes = dwAttributes;
    tml.Label.Sid = psid;
    if (!SetTokenInformation(m_hToken2, TokenIntegrityLevel, &tml, sizeof(TOKEN_MANDATORY_LABEL)))
    {
        throw Win32Exception(GetLastError(), L"SetTokenInformation(TokenIntegrityLevel) failed.");
    }

    if (!ImpersonateLoggedOnUser(m_hToken2))
    {
        throw Win32Exception(GetLastError(), L"ImpersonateLoggedOnUser failed.");
    }
}


Impersonator::~Impersonator()
{
    RevertToSelf();
}


LowIntegrityImpersonator::LowIntegrityImpersonator()
    : Impersonator(SE_GROUP_INTEGRITY, SidPtr::LowIntegrity)
{
}
