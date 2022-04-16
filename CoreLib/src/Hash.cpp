#include "pch.h"
#include "hnrt/Hash.h"
#include "hnrt/Exception.h"
#include "hnrt/Heap.h"
#include "hnrt/StringBuffer.h"


#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)
#endif


#pragma comment(lib, "Bcrypt")


using namespace hnrt;


Hash::Hash()
    : m_pszAlgId(L"")
    , m_hAlg()
    , m_hHash()
    , m_pValue(nullptr)
    , m_dwValueLength(0UL)
    , m_psz(nullptr)
{
}


Hash::Hash(PCWSTR pszAlgId)
	: m_pszAlgId(pszAlgId)
    , m_hAlg()
    , m_hHash()
	, m_pValue(nullptr)
	, m_dwValueLength(0UL)
	, m_psz(nullptr)
{
    NTSTATUS status = BCryptOpenAlgorithmProvider(&m_hAlg, pszAlgId, NULL, 0);
    if (status != STATUS_SUCCESS)
    {
        throw Exception(L"BCryptOpenAlgorithmProvider(%s) failed(%ld).", pszAlgId, status);
    }

    DWORD dwObjectLength = 0UL;
    ULONG objectLength = 0UL;
    status = BCryptGetProperty(m_hAlg, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&dwObjectLength), sizeof(dwObjectLength), &objectLength, 0);
    if (status != STATUS_SUCCESS)
    {
        m_hAlg.Close();
        throw Exception(L"BCryptGetProperty(%s,BCRYPT_OBJECT_LENGTH) failed(%ld).", pszAlgId, status);
    }

    ULONG valueLength = 0UL;
    status = BCryptGetProperty(m_hAlg, BCRYPT_HASH_LENGTH, reinterpret_cast<PUCHAR>(&m_dwValueLength), sizeof(m_dwValueLength), &valueLength, 0);
    if (status != STATUS_SUCCESS)
    {
        m_hAlg.Close();
        throw Exception(L"BCryptGetProperty(%s,BCRYPT_HASH_LENGTH) failed(%ld).", pszAlgId, status);
    }

    status = BCryptCreateHash(m_hAlg, &m_hHash, m_hHash.AllocateObject(dwObjectLength), dwObjectLength, NULL, 0, 0);
    if (status != STATUS_SUCCESS)
    {
        m_hAlg.Close();
        m_hHash.AllocateObject(0UL);
        throw Exception(L"BCryptCreateHash(%s) failed(%ld).", pszAlgId, status);
    }
}


Hash::Hash(const Hash& src)
    : m_pszAlgId(src.m_pszAlgId)
    , m_hAlg()
    , m_hHash()
    , m_pValue(nullptr)
    , m_dwValueLength(src.m_dwValueLength)
    , m_psz(nullptr)
{
    if (!src.m_pValue)
    {
        throw Exception(L"Hash(%s)::Hash: Source not computed yet.", m_pszAlgId);
    }

    m_pValue = new BYTE[src.m_dwValueLength];
    memcpy_s(m_pValue, m_dwValueLength, src.m_pValue, src.m_dwValueLength);

    if (src.m_psz)
    {
        size_t size = wcslen(src.m_psz) + 1;
        m_psz = Allocate<WCHAR>(size);
        wmemcpy_s(m_psz, size, src.m_psz, size);
    }
}


Hash::~Hash()
{
	delete[] m_pValue;

    free(m_psz);
}


void Hash::operator =(const Hash& src)
{
    if (!src.m_pValue)
    {
        throw Exception(L"Hash(%s)::operator =: Source not computed yet.", m_pszAlgId);
    }

    if (wcscmp(m_pszAlgId, src.m_pszAlgId))
    {
        m_pszAlgId = src.m_pszAlgId;
        if (m_dwValueLength != src.m_dwValueLength)
        {
            delete[] m_pValue;
            m_pValue = new BYTE[src.m_dwValueLength];
            m_dwValueLength = src.m_dwValueLength;
        }
    }

    memcpy_s(m_pValue, m_dwValueLength, src.m_pValue, src.m_dwValueLength);

    if (src.m_psz)
    {
        size_t size = wcslen(src.m_psz) + 1;
        m_psz = Allocate(m_psz, size);
        wmemcpy_s(m_psz, size, src.m_psz, size);
    }
    else
    {
        free(m_psz);
        m_psz = nullptr;
    }
}


void Hash::Close()
{
    m_pszAlgId = L"";
    m_hAlg.Close();
    m_hHash.Close();
    m_hHash.AllocateObject(0UL);
    delete[] m_pValue;
    m_pValue = nullptr;
    m_dwValueLength = 0UL;
    free(m_psz);
    m_psz = nullptr;
}


void Hash::Compute(DataFeeder& rDataFeeder)
{
    if (m_pValue)
    {
        throw Exception(L"Hash(%s)::Compute: Already computed.", m_pszAlgId);
    }
    NTSTATUS status;
    while (rDataFeeder.HasNext())
    {
        status = BCryptHashData(m_hHash, rDataFeeder.Data, rDataFeeder.DataLength, 0);
        if (status != STATUS_SUCCESS)
        {
            throw Exception(L"BCryptHashData(%s) failed(%ld).", m_pszAlgId, status);
        }
    }
    m_pValue = new BYTE[m_dwValueLength];
    status = BCryptFinishHash(m_hHash, m_pValue, m_dwValueLength, 0);
    if (status != STATUS_SUCCESS)
    {
        memset(m_pValue, 0, m_dwValueLength);
        throw Exception(L"BCryptFinishHash(%s) failed(%ld).", m_pszAlgId, status);
    }

}


PCWSTR Hash::get_Text() const
{
    if (!m_psz)
    {
        if (!m_pValue)
        {
            throw Exception(L"Hash(%s)::Compute: Not computed yet.", m_pszAlgId);
        }
        StringBuffer sb(m_dwValueLength * 2ULL + 1ULL);
        for (DWORD dwIndex = 0; dwIndex < m_dwValueLength; dwIndex++)
        {
            static const WCHAR hex[] = L"0123456789abcdef";
            sb.AppendFormat(L"%c%c", hex[(m_pValue[dwIndex] >> (4 * 1)) & 0xF], hex[(m_pValue[dwIndex] >> (4 * 0)) & 0xF]);
        }
        const_cast<Hash*>(this)->m_psz = sb.Detach();
    }
	return m_psz;
}


MD5Hash::MD5Hash(DataFeeder& rDataFeeder)
    : Hash(BCRYPT_MD5_ALGORITHM)
{
    try
    {
        Compute(rDataFeeder);
    }
    catch (Exception e)
    {
        Close();
        throw e;
    }
}


MD5Hash::MD5Hash(const MD5Hash& src)
    : Hash(src)
{
}


void MD5Hash::operator =(const MD5Hash& src)
{
    Hash::operator =(src);
}


SHA1Hash::SHA1Hash(DataFeeder& rDataFeeder)
    : Hash(BCRYPT_SHA1_ALGORITHM)
{
    try
    {
        Compute(rDataFeeder);
    }
    catch (Exception e)
    {
        Close();
        throw e;
    }
}


SHA1Hash::SHA1Hash(const SHA1Hash& src)
    : Hash(src)
{
}


void SHA1Hash::operator =(const SHA1Hash& src)
{
    Hash::operator =(src);
}


SHA256Hash::SHA256Hash(DataFeeder& rDataFeeder)
    : Hash(BCRYPT_SHA256_ALGORITHM)
{
    try
    {
        Compute(rDataFeeder);
    }
    catch (Exception e)
    {
        Close();
        throw e;
    }
}


SHA256Hash::SHA256Hash(const SHA256Hash& src)
    : Hash(src)
{
}


void SHA256Hash::operator =(const SHA256Hash& src)
{
    Hash::operator =(src);
}


SHA384Hash::SHA384Hash(DataFeeder& rDataFeeder)
    : Hash(BCRYPT_SHA384_ALGORITHM)
{
    try
    {
        Compute(rDataFeeder);
    }
    catch (Exception e)
    {
        Close();
        throw e;
    }
}


SHA384Hash::SHA384Hash(const SHA384Hash& src)
    : Hash(src)
{
}


void SHA384Hash::operator =(const SHA384Hash& src)
{
    Hash::operator =(src);
}


SHA512Hash::SHA512Hash(DataFeeder& rDataFeeder)
    : Hash(BCRYPT_SHA512_ALGORITHM)
{
    try
    {
        Compute(rDataFeeder);
    }
    catch (Exception e)
    {
        Close();
        throw e;
    }
}


SHA512Hash::SHA512Hash(const SHA512Hash& src)
    : Hash(src)
{
}


void SHA512Hash::operator =(const SHA512Hash& src)
{
    Hash::operator =(src);
}