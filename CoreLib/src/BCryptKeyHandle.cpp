#include "pch.h"
#include "hnrt/BCryptKeyHandle.h"
#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/BCryptAuthenticatedCipherModeInfo.h"
#include "hnrt/CryptographyException.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Debug.h"


using namespace hnrt;


void BCryptKeyHandle::Generate(const BCryptAlgHandle& hAlg, void* pKey, size_t cbKey)
{
    if (m_h)
    {
        Close();
    }

    DWORD dwObjectLength = hAlg.ObjectLength;

    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pObject, new UCHAR[dwObjectLength]);

    NTSTATUS status = BCryptGenerateSymmetricKey(hAlg, &m_h, m_pObject, dwObjectLength, reinterpret_cast<PUCHAR>(pKey), static_cast<ULONG>(cbKey), 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptGenerateSymmetricKey(%p,%zu) failed with status of %s.", pKey, cbKey, BCryptErrorLabel(status));
    }

    DBGPUT(L"BCryptKeyHandle@%p::Generate(%s-%zu-%s)", this, hAlg.AlgorithmName, KeyLength, hAlg.ChainingModeShort);
}


void BCryptKeyHandle::Import(const BCryptAlgHandle& hAlg, const ByteString& keyBlob)
{
    if (m_h)
    {
        Close();
    }

    DWORD dwObjectLength = hAlg.ObjectLength;

    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pObject, new UCHAR[dwObjectLength]);

    NTSTATUS status = BCryptImportKey(hAlg, NULL, BCRYPT_OPAQUE_KEY_BLOB, &m_h, m_pObject, dwObjectLength, const_cast<PUCHAR>((const unsigned char*)keyBlob), static_cast<ULONG>(keyBlob.Len), 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptImportKey failed with status of %s.", BCryptErrorLabel(status));
    }

    DBGPUT(L"BCryptKeyHandle@%p::Import(%s-%lu-%s)", this, hAlg.AlgorithmName, KeyLength, hAlg.ChainingModeShort);
}


void BCryptKeyHandle::Close()
{
    BCRYPT_KEY_HANDLE h = Interlocked<BCRYPT_KEY_HANDLE>::ExchangePointer(&m_h, nullptr);
    if (h)
    {
        NTSTATUS status = BCryptDestroyKey(h);
        if (status != STATUS_SUCCESS)
        {
            Debug::Put(L"BCryptDestroyKey(%p) failed with status of %s.", h, BCryptErrorLabel(status));
        }
    }

    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pObject, nullptr);

    DBGPUT(L"BCryptKeyHandle@%p::Close", this);
}


ByteString BCryptKeyHandle::Export() const
{
    ULONG cbKeyBlob = ~0;
    NTSTATUS status = BCryptExportKey(m_h, NULL, BCRYPT_OPAQUE_KEY_BLOB, NULL, 0, &cbKeyBlob, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptExportKey(OPAQUE_KEY_BLOB) failed with status of %s.", BCryptErrorLabel(status));
    }

    ByteString keyBlob(cbKeyBlob);

    status = BCryptExportKey(m_h, NULL, BCRYPT_OPAQUE_KEY_BLOB, keyBlob, cbKeyBlob, &cbKeyBlob, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptExportKey(OPAQUE_KEY_BLOB,%p,%lu) failed with status of %s.", keyBlob, cbKeyBlob, BCryptErrorLabel(status));
    }

    return keyBlob;
}


ByteString BCryptKeyHandle::Encrypt(void* pData, size_t cbData, void* pIV, size_t cbIV, ULONG dwFlags)
{
    ULONG cbCipherText = ~0;
    NTSTATUS status = BCryptEncrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), NULL, reinterpret_cast<PUCHAR>(pIV), static_cast<ULONG>(cbIV), NULL, 0, &cbCipherText, dwFlags);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptEncrypt(%p,%zu,%p,%zu) failed with status of %s.", pData, cbData, pIV, cbIV, BCryptErrorLabel(status));
    }

    ByteString encrypted(cbCipherText);

    status = BCryptEncrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), NULL, reinterpret_cast<PUCHAR>(pIV), static_cast<ULONG>(cbIV), encrypted, cbCipherText, &cbCipherText, dwFlags);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptEncrypt(%p,%zu,%p,%zu,%lu) failed with status of %s.", pData, cbData, pIV, cbIV, cbCipherText, BCryptErrorLabel(status));
    }

    return encrypted;
}


ByteString BCryptKeyHandle::Decrypt(void* pData, size_t cbData, void* pIV, size_t cbIV, ULONG dwFlags)
{
    ULONG cbPlainText = ~0;
    NTSTATUS status = BCryptDecrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), NULL, reinterpret_cast<PUCHAR>(pIV), static_cast<ULONG>(cbIV), NULL, 0, &cbPlainText, dwFlags);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptDecrypt(%p,%zu,%p,%zu) failed with status of %s.", pData, cbData, pIV, cbIV, BCryptErrorLabel(status));
    }

    ByteString decrypted(cbPlainText);

    status = BCryptDecrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), NULL, reinterpret_cast<PUCHAR>(pIV), static_cast<ULONG>(cbIV), decrypted, cbPlainText, &cbPlainText, dwFlags);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptDecrypt(%p,%zu,%p,%zu,%lu) failed with status of %s.", pData, cbData, pIV, cbIV, cbPlainText, BCryptErrorLabel(status));
    }

    return decrypted;
}


ByteString BCryptKeyHandle::Encrypt(void* pData, size_t cbData, BCryptAuthenticatedCipherModeInfo& info, void* pIV, size_t cbIV)
{
    ULONG cbCipherText = ~0;
    NTSTATUS status = BCryptEncrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), NULL, NULL, 0, NULL, 0, &cbCipherText, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptEncrypt(%p,%zu) failed with status of %s.", pData, cbData, BCryptErrorLabel(status));
    }

    ByteString encrypted(cbData);

    status = BCryptEncrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), &info, reinterpret_cast<PUCHAR>(pIV), static_cast<ULONG>(cbIV), encrypted, cbCipherText, &cbCipherText, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptEncrypt(%p,%zu,%lu) failed with status of %s.", pData, cbData, cbCipherText, BCryptErrorLabel(status));
    }

    return encrypted;
}


ByteString BCryptKeyHandle::Decrypt(void* pData, size_t cbData, BCryptAuthenticatedCipherModeInfo& info, void* pIV, size_t cbIV)
{
    ULONG cbPlainText = ~0;
    NTSTATUS status = BCryptDecrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), NULL, NULL, 0, NULL, 0, &cbPlainText, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptDecrypt(%p,%zu) failed with status of %s.", pData, cbData, BCryptErrorLabel(status));
    }

    ByteString decrypted(cbPlainText);

    status = BCryptDecrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), &info, reinterpret_cast<PUCHAR>(pIV), static_cast<ULONG>(cbIV), decrypted, cbPlainText, &cbPlainText, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptDecrypt(%p,%zu,%lu) failed with status of %s.", pData, cbData, cbPlainText, BCryptErrorLabel(status));
    }

    return decrypted;
}
