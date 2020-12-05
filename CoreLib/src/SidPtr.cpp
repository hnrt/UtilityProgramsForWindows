#include "pch.h"
#include "hnrt/SidPtr.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


SidPtr SidPtr::LowIntegrity(SECURITY_MANDATORY_LABEL_AUTHORITY, SECURITY_MANDATORY_LOW_RID);
SidPtr SidPtr::Everyone(SECURITY_WORLD_SID_AUTHORITY, SECURITY_WORLD_RID);
SidPtr SidPtr::AuthenticatedUser(SECURITY_NT_AUTHORITY, SECURITY_AUTHENTICATED_USER_RID);
SidPtr SidPtr::Administrators(SECURITY_NT_AUTHORITY, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS);
SidPtr SidPtr::Users(SECURITY_NT_AUTHORITY, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS);


SidPtr::SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0)
    : m_psid(nullptr)
{
    if (!AllocateAndInitializeSid(&auth, 1, dwSubAuth0, 0, 0, 0, 0, 0, 0, 0, &m_psid))
    {
        throw Win32Exception(GetLastError(), L"Allocate SID(1) failed.");
    }
}


SidPtr::SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1)
    : m_psid(nullptr)
{
    if (!AllocateAndInitializeSid(&auth, 2, dwSubAuth0, dwSubAuth1, 0, 0, 0, 0, 0, 0, &m_psid))
    {
        throw Win32Exception(GetLastError(), L"Allocate SID(2) failed.");
    }
}


SidPtr::SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2)
    : m_psid(nullptr)
{
    if (!AllocateAndInitializeSid(&auth, 3, dwSubAuth0, dwSubAuth1, dwSubAuth2, 0, 0, 0, 0, 0, &m_psid))
    {
        throw Win32Exception(GetLastError(), L"Allocate SID(3) failed.");
    }
}


SidPtr::SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2, DWORD dwSubAuth3)
    : m_psid(nullptr)
{
    if (!AllocateAndInitializeSid(&auth, 4, dwSubAuth0, dwSubAuth1, dwSubAuth2, dwSubAuth3, 0, 0, 0, 0, &m_psid))
    {
        throw Win32Exception(GetLastError(), L"Allocate SID(4) failed.");
    }
}


SidPtr::SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2, DWORD dwSubAuth3, DWORD dwSubAuth4)
    : m_psid(nullptr)
{
    if (!AllocateAndInitializeSid(&auth, 5, dwSubAuth0, dwSubAuth1, dwSubAuth2, dwSubAuth3, dwSubAuth4, 0, 0, 0, &m_psid))
    {
        throw Win32Exception(GetLastError(), L"Allocate SID(5) failed.");
    }
}


SidPtr::SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2, DWORD dwSubAuth3, DWORD dwSubAuth4, DWORD dwSubAuth5)
    : m_psid(nullptr)
{
    if (!AllocateAndInitializeSid(&auth, 6, dwSubAuth0, dwSubAuth1, dwSubAuth2, dwSubAuth3, dwSubAuth4, dwSubAuth5, 0, 0, &m_psid))
    {
        throw Win32Exception(GetLastError(), L"Allocate SID(6) failed.");
    }
}


SidPtr::SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2, DWORD dwSubAuth3, DWORD dwSubAuth4, DWORD dwSubAuth5, DWORD dwSubAuth6)
    : m_psid(nullptr)
{
    if (!AllocateAndInitializeSid(&auth, 7, dwSubAuth0, dwSubAuth1, dwSubAuth2, dwSubAuth3, dwSubAuth4, dwSubAuth5, dwSubAuth6, 0, &m_psid))
    {
        throw Win32Exception(GetLastError(), L"Allocate SID(7) failed.");
    }
}


SidPtr::SidPtr(SID_IDENTIFIER_AUTHORITY auth, DWORD dwSubAuth0, DWORD dwSubAuth1, DWORD dwSubAuth2, DWORD dwSubAuth3, DWORD dwSubAuth4, DWORD dwSubAuth5, DWORD dwSubAuth6, DWORD dwSubAuth7)
    : m_psid(nullptr)
{
    if (!AllocateAndInitializeSid(&auth, 8, dwSubAuth0, dwSubAuth1, dwSubAuth2, dwSubAuth3, dwSubAuth4, dwSubAuth5, dwSubAuth6, dwSubAuth7, &m_psid))
    {
        throw Win32Exception(GetLastError(), L"Allocate SID(8) failed.");
    }
}


SidPtr::~SidPtr()
{
    if (m_psid)
    {
        FreeSid(m_psid);
    }
}
