#include "Credentials.h"
#include <Lmcons.h>
#include <sddl.h>
#pragma comment(lib,"Advapi32")
#include "hnrt/StringStore.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Debug.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


static const unsigned char s_KEY[SECRET_KEY_LENGTH] =
{
    0x6f, 0x09, 0xc5, 0x65, 0x8c, 0x57, 0x42, 0x2b, 0xac, 0x84, 0x2d, 0x51, 0xa3, 0xa0, 0xc5, 0x40,
    0xe7, 0x85, 0x70, 0x43, 0x80, 0x21, 0x4b, 0x2d, 0x8e, 0x85, 0xb5, 0x09, 0xfd, 0xaf, 0xb3, 0x50
};


static const unsigned char s_IV[SECRET_IV_LENGTH] =
{
    0x1c, 0x0c, 0xbf, 0x99, 0x06, 0x75, 0x4b, 0xea, 0x9e, 0x84, 0xac, 0xfa, 0x3f, 0xc6, 0x3a, 0x55
};


RefPtr<Credentials> Credentials::Create(LONG version)
{
    switch (version)
    {
    case 0:
    case 1:
        return RefPtr<Credentials>(new Credentials(s_KEY, s_IV));
    default:
        break;
    }
    WCHAR szUserName[UNLEN + 1] = { 0 };
    DWORD dwUserNameLen = _countof(szUserName);
    if (!GetUserNameW(szUserName, &dwUserNameLen))
    {
        throw Win32Exception(GetLastError(), L"Failed to get user name.");
    }
    DWORD cbSid = 0;
    DWORD cchDomainName = 0;
    SID_NAME_USE sidNameUse = SidTypeUser;
    if (!LookupAccountNameW(NULL, szUserName, nullptr, &cbSid, nullptr, &cchDomainName, &sidNameUse))
    {
        DWORD dwError = GetLastError();
        if (dwError != ERROR_INSUFFICIENT_BUFFER)
        {
            throw Win32Exception(GetLastError(), L"Failed to get the sid length.");
        }
    }
    Buffer<BYTE> sid(cbSid);
    Buffer<WCHAR> szDomainName(cchDomainName);
    if (!LookupAccountNameW(NULL, szUserName, sid.Ptr, &cbSid, szDomainName.Ptr, &cchDomainName, &sidNameUse))
    {
        throw Win32Exception(GetLastError(), L"Failed to get the sid length.");
    }
#if _DEBUG
    LPWSTR pszSid = nullptr;
    if (ConvertSidToStringSidW(sid.Ptr, &pszSid))
    {
        DBGPUT(L"UserName=%s DomainName=%s SID=%s", szUserName, szDomainName.Ptr, pszSid);
        LocalFree(reinterpret_cast<HLOCAL>(pszSid));
    }
    else
    {
        DBGPUT(L"UserName=%s DomainName=%s", szUserName, szDomainName.Ptr);
    }
#endif
    unsigned char key[SECRET_KEY_LENGTH];
    for (ULONG index = 0; index < SECRET_KEY_LENGTH; index++)
    {
        key[index] = s_KEY[index] ^ sid[index % cbSid];
    }
    unsigned char iv[SECRET_IV_LENGTH];
    for (ULONG index = 0; index < SECRET_IV_LENGTH; index++)
    {
        iv[index] = s_IV[index] ^ sid[index % cbSid];
    }
    return RefPtr<Credentials>(new Credentials(key, iv));
}


Credentials::Credentials(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH])
    : RefObj()
    , m_pszUsername(nullptr)
    , m_Password(key, iv)
    , m_pszKey(nullptr)
    , m_pCallback(nullptr)
{
}


RefPtr<Credentials> Credentials::Clone() const
{
    RefPtr<Credentials> pCloned = Credentials::Create();
    pCloned->Username = Username;
    pCloned->EncryptedPassword = EncryptedPassword;
    pCloned->Key = Key;
    return pCloned;
}


void Credentials::set_Username(PCWSTR psz)
{
    m_pszUsername = psz ? StringStore::Get(psz) : nullptr;
    if (m_pCallback)
    {
        m_pCallback->OnCredentialsUpdate(*this);
    }
}


void Credentials::set_Password(PCWSTR psz)
{
    m_Password.PlainText = psz;
    if (m_pCallback)
    {
        m_pCallback->OnCredentialsUpdate(*this);
    }
}


void Credentials::set_EncryptedPassword(PCWSTR psz)
{
    m_Password.Encrypted = psz;
    if (m_pCallback)
    {
        m_pCallback->OnCredentialsUpdate(*this);
    }
}


void Credentials::set_Key(PCWSTR psz)
{
    m_pszKey = psz ? StringStore::Get(psz) : nullptr;
    if (m_pCallback)
    {
        m_pCallback->OnCredentialsUpdate(*this);
    }
}
