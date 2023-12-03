#include "pch.h"
#include "hnrt/BCryptHashHandle.h"
#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/CryptographyException.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Debug.h"


#pragma comment(lib, "Bcrypt")


using namespace hnrt;


void BCryptHashHandle::Open(const BCryptAlgHandle& hAlg)
{
    if (m_h)
    {
        Close();
    }

    ULONG cbObjectLength = hAlg.ObjectLength;

    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pObject, new UCHAR[cbObjectLength]);

    NTSTATUS status = BCryptCreateHash(hAlg, &m_h, m_pObject, cbObjectLength, NULL, 0, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptCreateHash failed with status of %s.", BCryptErrorLabel(status));
    }

    DBGPUT(L"BCryptHashHandle@%p::Open(%s): Done.", this, hAlg.AlgorithmName);
}


void BCryptHashHandle::Close()
{
    BCRYPT_HASH_HANDLE h = Interlocked<BCRYPT_HASH_HANDLE>::ExchangePointer(&m_h, nullptr);
    if (h)
    {
        NTSTATUS status = BCryptDestroyHash(h);
        if (status != STATUS_SUCCESS)
        {
            Debug::Put(L"BCryptDestroyHash(%p) failed with status of %s.", h, BCryptErrorLabel(status));
        }
    }

    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pObject, nullptr);

    DBGPUT(L"BCryptHashHandle@%p::Close: Done.", this);
}


void BCryptHashHandle::Feed(void* ptr, size_t len)
{
    NTSTATUS status = BCryptHashData(m_h, reinterpret_cast<PUCHAR>(ptr), static_cast<ULONG>(len), 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptHashData(%p,%zu) failed with status of %s.", ptr, len, BCryptErrorLabel(status));
    }
}


void BCryptHashHandle::Finalize(void* ptr, size_t len)
{
    NTSTATUS status = BCryptFinishHash(m_h, reinterpret_cast<PUCHAR>(ptr), static_cast<ULONG>(len), 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptFinishHash(%p,%zu) failed with status of %s.", ptr, len, BCryptErrorLabel(status));
    }
}
