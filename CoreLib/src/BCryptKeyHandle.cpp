#include "pch.h"
#include "hnrt/BCryptKeyHandle.h"
#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/CryptographyException.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Debug.h"


#pragma comment(lib, "Bcrypt")


using namespace hnrt;


void BCryptKeyHandle::Generate(const BCryptAlgHandle& hAlg, void* pKey, size_t cbKey)
{
    if (m_h)
    {
        Close();
    }

    DWORD dwObjectLength = hAlg.ObjectLength;

    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pObject, new BYTE[dwObjectLength]);

    NTSTATUS status = BCryptGenerateSymmetricKey(hAlg, &m_h, m_pObject, dwObjectLength, reinterpret_cast<PUCHAR>(pKey), static_cast<ULONG>(cbKey), 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptGenerateSymmetricKey(%p,%zu) failed with status of %s.", pKey, cbKey, BCryptErrorLabel(status));
    }

    DBGPUT(L"BCryptKeyHandle@%p::Generate(%s): Done.", this, hAlg.AlgorithmName);
}


void BCryptKeyHandle::Import(const BCryptAlgHandle& hAlg, RefPtr<SecretBuffer> pKeyBlob)
{
    if (m_h)
    {
        Close();
    }

    DWORD dwObjectLength = hAlg.ObjectLength;

    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pObject, new UCHAR[dwObjectLength]);

    NTSTATUS status = BCryptImportKey(hAlg, NULL, BCRYPT_OPAQUE_KEY_BLOB, &m_h, m_pObject, dwObjectLength, pKeyBlob->Ptr, static_cast<ULONG>(pKeyBlob->Len), 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptImportKey failed with status of %s.", BCryptErrorLabel(status));
    }

    DBGPUT(L"BCryptKeyHandle@%p::Import(%s): Done.", this, hAlg.AlgorithmName);
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

    DBGPUT(L"BCryptKeyHandle@%p::Close: Done.", this);
}


RefPtr<SecretBuffer> BCryptKeyHandle::Export() const
{
    ULONG cbKeyBlob = ~0;
    NTSTATUS status = BCryptExportKey(m_h, NULL, BCRYPT_OPAQUE_KEY_BLOB, NULL, 0, &cbKeyBlob, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptExportKey(OPAQUE_KEY_BLOB) failed with status of %s.", BCryptErrorLabel(status));
    }

    RefPtr<SecretBuffer> pKeyBlob(new SecretBuffer(cbKeyBlob));

    status = BCryptExportKey(m_h, NULL, BCRYPT_OPAQUE_KEY_BLOB, pKeyBlob->Ptr, cbKeyBlob, &cbKeyBlob, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptExportKey(OPAQUE_KEY_BLOB,%p,%lu) failed with status of %s.", pKeyBlob->Ptr, cbKeyBlob, BCryptErrorLabel(status));
    }

    return pKeyBlob;
}


RefPtr<SecretBuffer> BCryptKeyHandle::Encrypt(void* pData, size_t cbData, void* pIV, size_t cbIV)
{
    ULONG cbCipherText = ~0;
    NTSTATUS status = BCryptEncrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), NULL, reinterpret_cast<PUCHAR>(pIV), static_cast<ULONG>(cbIV), NULL, 0, &cbCipherText, BCRYPT_BLOCK_PADDING);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptEncrypt(%p,%zu,%p,%zu) failed with status of %s.", pData, cbData, pIV, cbIV, BCryptErrorLabel(status));
    }

    RefPtr<SecretBuffer> pEncrypted(new SecretBuffer(cbCipherText));

    status = BCryptEncrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), NULL, reinterpret_cast<PUCHAR>(pIV), static_cast<ULONG>(cbIV), reinterpret_cast<PUCHAR>(pEncrypted->Ptr), cbCipherText, &cbCipherText, BCRYPT_BLOCK_PADDING);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptEncrypt(%p,%zu,%p,%zu,%lu) failed with status of %s.", pData, cbData, pIV, cbIV, cbCipherText, BCryptErrorLabel(status));
    }

    return pEncrypted;
}


RefPtr<SecretBuffer> BCryptKeyHandle::Decrypt(void* pData, size_t cbData, void* pIV, size_t cbIV)
{
    ULONG cbPlainText = ~0;
    NTSTATUS status = BCryptDecrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), NULL, reinterpret_cast<PUCHAR>(pIV), static_cast<ULONG>(cbIV), NULL, 0, &cbPlainText, BCRYPT_BLOCK_PADDING);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptDecrypt(%p,%zu,%p,%zu) failed with status of %s.", pData, cbData, pIV, cbIV, BCryptErrorLabel(status));
    }

    RefPtr<SecretBuffer> pDecrypted(new SecretBuffer(cbPlainText));

    status = BCryptDecrypt(m_h, reinterpret_cast<PUCHAR>(pData), static_cast<ULONG>(cbData), NULL, reinterpret_cast<PUCHAR>(pIV), static_cast<ULONG>(cbIV), reinterpret_cast<PUCHAR>(pDecrypted->Ptr), cbPlainText, &cbPlainText, BCRYPT_BLOCK_PADDING);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptDecrypt(%p,%zu,%p,%zu,%lu) failed with status of %s.", pData, cbData, pIV, cbIV, cbPlainText, BCryptErrorLabel(status));
    }

    return pDecrypted;
}
