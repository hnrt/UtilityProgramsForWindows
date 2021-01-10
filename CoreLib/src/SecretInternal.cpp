#include "pch.h"
#include "SecretInternal.h"
#include "hnrt/BCryptKeyHandle.h"
#include "hnrt/Debug.h"
#include "hnrt/Exception.h"


#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif


#pragma comment(lib, "Bcrypt")


using namespace hnrt;


#define KEY_SALT 0x43
#define IV_SALT 0x12


SecretInternal::SecretInternal()
    : Secret()
    , m_hAlg()
    , m_Object()
    , m_KeyBlob()
    , m_Processed()
{
    Initialize();
    memset(m_IV, 0, sizeof(m_IV));
}


SecretInternal::SecretInternal(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH])
    : Secret()
    , m_hAlg()
    , m_Object()
    , m_KeyBlob()
    , m_Processed()
{
    Initialize();
    SetKey(key, SECRET_KEY_LENGTH);
    SetIV(iv, SECRET_IV_LENGTH);
}


SecretInternal::SecretInternal(PCWSTR pszKey, PCWSTR pszIV)
    : Secret()
    , m_hAlg()
    , m_Object()
    , m_KeyBlob()
    , m_Processed()
{
    Initialize();
    SetKey(pszKey);
    SetIV(pszIV);
}


SecretInternal::~SecretInternal()
{
    m_Processed.Resize(0);
    m_KeyBlob.Resize(0);
    memset(m_IV, 0, sizeof(m_IV));
}


void SecretInternal::Initialize()
{
    NTSTATUS status = BCryptOpenAlgorithmProvider(&m_hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptOpenAlgorithmProvider(AES) failed. status=%08lX", status);
    }

    status = BCryptSetProperty(m_hAlg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptSetProperty(CHAINING_MODE,CBC) failed. status=%08lX", status);
    }

    DWORD cbObject = ~0;
    ULONG cbResult = ~0;
    status = BCryptGetProperty(m_hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&cbObject, sizeof(cbObject), &cbResult, 0);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptGetProperty(OBJECT_LENGTH) failed. status=%08lX", status);
    }

    m_Object.Resize(cbObject);
}


void SecretInternal::InitializeKeyBlob(unsigned char key[SECRET_KEY_LENGTH])
{
    AddSalt(key, SECRET_KEY_LENGTH, KEY_SALT);

    BCryptKeyHandle hKey;
    NTSTATUS status = BCryptGenerateSymmetricKey(m_hAlg, &hKey, m_Object.Ptr, (ULONG)m_Object.Len, key, SECRET_KEY_LENGTH, 0);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptGenerateSymmetricKey failed. status=%08lX", status);
    }

    ULONG cbKeyBlob = ~0;
    status = BCryptExportKey(hKey, NULL, BCRYPT_OPAQUE_KEY_BLOB, NULL, 0, &cbKeyBlob, 0);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptExportKey(size) failed. status=%08lX", status);
    }

    m_KeyBlob.Resize(cbKeyBlob);

    status = BCryptExportKey(hKey, NULL, BCRYPT_OPAQUE_KEY_BLOB, m_KeyBlob.Ptr, cbKeyBlob, &cbKeyBlob, 0);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptExportKey failed. status=%08lX", status);
    }

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
    unsigned char key[SECRET_KEY_LENGTH];

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
    m_Processed.Resize(0);
}


void SecretInternal::Encrypt(const void* ptr, size_t len)
{
    NTSTATUS status;

    SecretBuffer src(sizeof(ULONG) + len);
    *reinterpret_cast<ULONG*>(src.Ptr) = static_cast<ULONG>(len);
    memcpy_s(src.Ptr + sizeof(ULONG), src.Len - sizeof(ULONG), ptr, len);

    BCryptKeyHandle key;
    status = BCryptImportKey(m_hAlg, NULL, BCRYPT_OPAQUE_KEY_BLOB, &key, m_Object.Ptr, (ULONG)m_Object.Len, m_KeyBlob.Ptr, (ULONG)m_KeyBlob.Len, 0);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptImportKey failed. status=%08lX", status);
    }

    SecretBuffer iv(SECRET_IV_LENGTH);
    memcpy_s(iv.Ptr, iv.Len, m_IV, SECRET_IV_LENGTH);
    AddSalt(iv.Ptr, iv.Len, IV_SALT);

    ULONG cbCipherText = ~0;
    status = BCryptEncrypt(key, src, static_cast<ULONG>(src.Len), NULL, iv, SECRET_IV_LENGTH, NULL, 0, &cbCipherText, BCRYPT_BLOCK_PADDING);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptEncrypt(size) failed. status=%08lX", status);
    }

    m_Processed.Resize(cbCipherText);

    status = BCryptEncrypt(key, src, static_cast<ULONG>(src.Len), NULL, iv, SECRET_IV_LENGTH, m_Processed, cbCipherText, &cbCipherText, BCRYPT_BLOCK_PADDING);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptEncrypt failed. status=%08lX", status);
    }
}


void SecretInternal::Decrypt(const void* ptr, size_t len)
{
    NTSTATUS status;

    BCryptKeyHandle key;
    status = BCryptImportKey(m_hAlg, NULL, BCRYPT_OPAQUE_KEY_BLOB, &key, m_Object.Ptr, (ULONG)m_Object.Len, m_KeyBlob.Ptr, (ULONG)m_KeyBlob.Len, 0);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptImportKey failed. status=%08lX", status);
    }

    SecretBuffer iv(SECRET_IV_LENGTH);
    memcpy_s(iv.Ptr, iv.Len, m_IV, SECRET_IV_LENGTH);
    AddSalt(iv.Ptr, iv.Len, IV_SALT);

    ULONG cbPlainText = ~0;
    status = BCryptDecrypt(key, reinterpret_cast<PUCHAR>(const_cast<void*>(ptr)), static_cast<ULONG>(len), NULL, iv, SECRET_IV_LENGTH, NULL, 0, &cbPlainText, BCRYPT_BLOCK_PADDING);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptDecrypt(size) failed. status=%08lX", status);
    }

    SecretBuffer tmp(cbPlainText);

    status = BCryptDecrypt(key, reinterpret_cast<PUCHAR>(const_cast<void*>(ptr)), static_cast<ULONG>(len), NULL, iv, SECRET_IV_LENGTH, tmp, cbPlainText, &cbPlainText, BCRYPT_BLOCK_PADDING);
    if (!NT_SUCCESS(status))
    {
        throw Exception(L"BCryptDecrypt failed. status=%08lX", status);
    }

    m_Processed.Resize(*reinterpret_cast<ULONG*>(tmp.Ptr));
    memcpy_s(m_Processed.Ptr, m_Processed.Len, tmp.Ptr + sizeof(ULONG), m_Processed.Len);
}


const void* SecretInternal::get_Ptr() const
{
    return m_Processed.Ptr;
}


size_t SecretInternal::get_Len() const
{
    return m_Processed.Len;
}
