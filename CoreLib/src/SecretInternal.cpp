#include "pch.h"
#include "SecretInternal.h"
#include "hnrt/BCryptKeyHandle.h"
#include "hnrt/Debug.h"
#include "hnrt/Exception.h"


#pragma comment(lib, "Bcrypt")


using namespace hnrt;


#define KEY_SALT 0x43
#define IV_SALT 0x12


SecretInternal::SecretInternal()
    : Secret()
    , m_IV()
    , m_hAlg()
    , m_KeyBlob()
    , m_Processed()
{
    Initialize();
    memset(m_IV, 0, sizeof(m_IV));
}


SecretInternal::SecretInternal(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH])
    : Secret()
    , m_IV()
    , m_hAlg()
    , m_KeyBlob()
    , m_Processed()
{
    Initialize();
    SetKey(key, SECRET_KEY_LENGTH);
    SetIV(iv, SECRET_IV_LENGTH);
}


SecretInternal::SecretInternal(PCWSTR pszKey, PCWSTR pszIV)
    : Secret()
    , m_IV()
    , m_hAlg()
    , m_KeyBlob()
    , m_Processed()
{
    Initialize();
    SetKey(pszKey);
    SetIV(pszIV);
}


SecretInternal::~SecretInternal()
{
    memset(m_IV, 0, sizeof(m_IV));
}


void SecretInternal::Initialize()
{
    m_hAlg.Open(BCRYPT_AES_ALGORITHM);
    m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC;
}


void SecretInternal::InitializeKeyBlob(unsigned char key[SECRET_KEY_LENGTH])
{
    AddSalt(key, SECRET_KEY_LENGTH, KEY_SALT);
    BCryptKeyHandle hKey;
    hKey.Generate(m_hAlg, key, SECRET_KEY_LENGTH);
    m_KeyBlob = hKey.Export();
    memset(key, 0, SECRET_KEY_LENGTH);
}


void SecretInternal::AddSalt(unsigned char* ptr, size_t len, unsigned char salt)
{
    for (size_t index = 0; index < len; index++)
    {
        ptr[index] ^= salt;
        salt = ptr[index];
    }
}


void SecretInternal::SetKey(const unsigned char* ptr, size_t len)
{
    unsigned char key[SECRET_KEY_LENGTH];

    if (len < SECRET_KEY_LENGTH)
    {
        memcpy_s(key, SECRET_KEY_LENGTH, ptr, len);
        memset(key + len, 0, SECRET_KEY_LENGTH - len);
    }
    else
    {
        memcpy_s(key, SECRET_KEY_LENGTH, ptr, SECRET_KEY_LENGTH);
    }

    InitializeKeyBlob(key);
}


void SecretInternal::SetIV(const unsigned char* ptr, size_t len)
{
    if (len < SECRET_IV_LENGTH)
    {
        memcpy_s(m_IV, SECRET_IV_LENGTH, ptr, len);
        memset(m_IV + len, 0, SECRET_IV_LENGTH - len);
    }
    else
    {
        memcpy_s(m_IV, SECRET_IV_LENGTH, ptr, SECRET_IV_LENGTH);
    }
}


void SecretInternal::SetKey(PCWSTR psz)
{
    unsigned char key[SECRET_KEY_LENGTH] = { 0 };

    int n = WideCharToMultiByte(CP_UTF8, 0, psz, static_cast<int>(wcslen(psz)), reinterpret_cast<LPSTR>(key), SECRET_KEY_LENGTH, NULL, NULL);
    if (n < 0)
    {
        throw Exception(L"SecretInternal::SetKey: Conversion failed.");
    }
    else if (n < SECRET_KEY_LENGTH)
    {
        memset(key + n, 0, static_cast<size_t>(SECRET_KEY_LENGTH) - n);
    }

    InitializeKeyBlob(key);
}


void SecretInternal::SetIV(PCWSTR psz)
{
    int n = WideCharToMultiByte(CP_UTF8, 0, psz, static_cast<int>(wcslen(psz)), reinterpret_cast<LPSTR>(m_IV), SECRET_IV_LENGTH, NULL, NULL);
    if (n < 0)
    {
        throw Exception(L"SecretInternal::SetIV: Conversion failed.");
    }
    else if (n < SECRET_IV_LENGTH)
    {
        memset(m_IV + n, 0, static_cast<size_t>(SECRET_IV_LENGTH) - n);
    }
}


void SecretInternal::ClearBuffer()
{
    m_Processed = ByteString();
}


void SecretInternal::Encrypt(const void* ptr, size_t len)
{
    ByteString src(sizeof(ULONG) + len);
    *reinterpret_cast<ULONG*>(src.Ptr) = static_cast<ULONG>(len);
    memcpy_s((char*)src + sizeof(ULONG), src.Len - sizeof(ULONG), ptr, len);

    ByteString iv(SECRET_IV_LENGTH);
    memcpy_s(iv, iv.Len, m_IV, SECRET_IV_LENGTH);
    AddSalt(iv, iv.Len, IV_SALT);

    BCryptKeyHandle key;
    key.Import(m_hAlg, m_KeyBlob);
    m_Processed = key.Encrypt(src.Ptr, src.Len, iv, SECRET_IV_LENGTH);
}


void SecretInternal::Decrypt(const void* ptr, size_t len)
{
    ByteString iv(SECRET_IV_LENGTH);
    memcpy_s(iv, iv.Len, m_IV, SECRET_IV_LENGTH);
    AddSalt(iv, iv.Len, IV_SALT);

    BCryptKeyHandle key;
    key.Import(m_hAlg, m_KeyBlob);
    ByteString decrypted = key.Decrypt(const_cast<void*>(ptr), len, iv, SECRET_IV_LENGTH);

    m_Processed = ByteString(*reinterpret_cast<ULONG*>(decrypted.Ptr));
    memcpy_s(m_Processed, m_Processed.Len, (char*)decrypted + sizeof(ULONG), m_Processed.Len);
}


const void* SecretInternal::get_Ptr() const
{
    return m_Processed;
}


size_t SecretInternal::get_Len() const
{
    return m_Processed.Len;
}
