#include "pch.h"
#include "SecretInternal.h"
#include "hnrt/MultibyteString.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


using namespace hnrt;


constexpr auto KEY_SALT = 0x43;
constexpr auto IV_SALT = 0x12;


SecretInternal::SecretInternal()
    : Secret()
    , m_IV(SECRET_IV_LENGTH)
    , m_hAlg()
    , m_hKey()
    , m_Processed()
{
    m_hAlg.Open(BCRYPT_AES_ALGORITHM);
    m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC;
}


SecretInternal::SecretInternal(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH])
    : Secret()
    , m_IV(SECRET_IV_LENGTH)
    , m_hAlg()
    , m_hKey()
    , m_Processed()
{
    m_hAlg.Open(BCRYPT_AES_ALGORITHM);
    m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC;
    SetKey(key, SECRET_KEY_LENGTH);
    SetIV(iv, SECRET_IV_LENGTH);
}


SecretInternal::SecretInternal(PCWSTR pszKey, PCWSTR pszIV)
    : Secret()
    , m_IV(SECRET_IV_LENGTH)
    , m_hAlg()
    , m_hKey()
    , m_Processed()
{
    m_hAlg.Open(BCRYPT_AES_ALGORITHM);
    m_hAlg.ChainingMode = BCRYPT_CHAIN_MODE_CBC;
    SetKey(pszKey);
    SetIV(pszIV);
}


SecretInternal::~SecretInternal()
{
    memset(m_IV, 0, sizeof(m_IV));
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

    AddSalt(key, SECRET_KEY_LENGTH, KEY_SALT);

    m_hKey.Generate(m_hAlg, key, SECRET_KEY_LENGTH);
}


void SecretInternal::SetIV(const unsigned char* ptr, size_t len)
{
    if (len < SECRET_IV_LENGTH)
    {
        memcpy_s(m_IV, SECRET_IV_LENGTH, ptr, len);
        memset((unsigned char*)m_IV + len, 0, SECRET_IV_LENGTH - len);
    }
    else
    {
        memcpy_s(m_IV, SECRET_IV_LENGTH, ptr, SECRET_IV_LENGTH);
    }
}


void SecretInternal::SetKey(PCWSTR psz)
{
    ByteString key(SECRET_KEY_LENGTH);
    MultibyteString deserialized = ToUTF8(String(psz));
    memcpy_s(key, key.Len, deserialized, deserialized.Len < key.Len ? deserialized.Len : key.Len);
    AddSalt(key, SECRET_KEY_LENGTH, KEY_SALT);
    m_hKey.Generate(m_hAlg, key, SECRET_KEY_LENGTH);
}


void SecretInternal::SetIV(PCWSTR psz)
{
    m_IV.Fill(0);
    MultibyteString deserialized = ToUTF8(String(psz));
    memcpy_s(m_IV, m_IV.Len, deserialized, deserialized.Len < m_IV.Len ? deserialized.Len : m_IV.Len);
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

    m_Processed = m_hKey.Encrypt(src.Ptr, src.Len, iv, SECRET_IV_LENGTH);
}


void SecretInternal::Decrypt(const void* ptr, size_t len)
{
    ByteString iv(SECRET_IV_LENGTH);
    memcpy_s(iv, iv.Len, m_IV, SECRET_IV_LENGTH);
    AddSalt(iv, iv.Len, IV_SALT);

    ByteString decrypted = m_hKey.Decrypt(const_cast<void*>(ptr), len, iv, SECRET_IV_LENGTH);

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
