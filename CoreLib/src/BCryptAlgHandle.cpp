#include "pch.h"
#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/CryptographyException.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Debug.h"


using namespace hnrt;


void BCryptAlgHandle::Open(PCWSTR pszAlgorithm)
{
    if (m_h)
    {
        Close();
    }

    NTSTATUS status = BCryptOpenAlgorithmProvider(&m_h, pszAlgorithm, NULL, 0);
    if (status != STATUS_SUCCESS)
    {
        throw CryptographyException(status, L"BCryptOpenAlgorithmProvider(%s) failed with status of %s.", pszAlgorithm, BCryptErrorLabel(status));
    }

    DBGPUT(L"BCryptAlgHandle@%p::Open(%s)", this, pszAlgorithm);
}


void BCryptAlgHandle::Close()
{
    BCRYPT_ALG_HANDLE h = Interlocked<BCRYPT_ALG_HANDLE>::ExchangePointer(&m_h, nullptr);
    if (h)
    {
        NTSTATUS status = BCryptCloseAlgorithmProvider(h, 0);
        if (status != STATUS_SUCCESS)
        {
            Debug::Put(L"BCryptCloseAlgorithmProvider(%p) failed with status of %s.", h, BCryptErrorLabel(status));
        }
    }

    DBGPUT(L"BCryptAlgHandle@%p::Close", this);
}


String BCryptAlgHandle::get_ChainingModeShort() const
{
    static const WCHAR szLeader[] = { L"ChainingMode" };
    String value = GetPropertyString(BCRYPT_CHAINING_MODE);
    return value.StartsWith(szLeader) ? String(value.Ptr + wcslen(szLeader)) : value;
}
