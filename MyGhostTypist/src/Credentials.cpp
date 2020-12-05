#include "Credentials.h"
#include "hnrt/StringStore.h"


using namespace hnrt;


RefPtr<Credentials> Credentials::Create()
{
    return RefPtr<Credentials>(new Credentials());
}


static const unsigned char s_KEY[SECRET_KEY_LENGTH] =
{
    0x6f, 0x09, 0xc5, 0x65, 0x8c, 0x57, 0x42, 0x2b, 0xac, 0x84, 0x2d, 0x51, 0xa3, 0xa0, 0xc5, 0x40,
    0xe7, 0x85, 0x70, 0x43, 0x80, 0x21, 0x4b, 0x2d, 0x8e, 0x85, 0xb5, 0x09, 0xfd, 0xaf, 0xb3, 0x50
};


static const unsigned char s_IV[SECRET_IV_LENGTH] =
{
    0x1c, 0x0c, 0xbf, 0x99, 0x06, 0x75, 0x4b, 0xea, 0x9e, 0x84, 0xac, 0xfa, 0x3f, 0xc6, 0x3a, 0x55
};


Credentials::Credentials()
    : RefObj()
    , m_pszUsername(nullptr)
    , m_Password(s_KEY, s_IV)
    , m_pszKey(nullptr)
    , m_pCallback(nullptr)
{
}


RefPtr<Credentials> Credentials::Clone() const
{
    Credentials* pCloned = new Credentials();
    pCloned->m_pszUsername = m_pszUsername;
    pCloned->m_Password.Encrypted = m_Password.Encrypted;
    pCloned->m_pszKey = m_pszKey;
    return RefPtr<Credentials>(pCloned);
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
