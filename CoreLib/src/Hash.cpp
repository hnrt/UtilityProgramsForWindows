#include "pch.h"
#include "hnrt/Hash.h"
#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/BCryptHashHandle.h"
#include "hnrt/Interlocked.h"
#include "hnrt/ByteDataFeeder.h"


using namespace hnrt;


#define BYTEDATAFEEDER(ptr, cb) ByteDataFeeder(ptr, cb, static_cast<DWORD>(cb) < 65536UL ? static_cast<DWORD>(cb) : 65536UL)


Hash::Hash()
    : m_pszAlgorithm(nullptr)
    , m_pDataFeeder()
    , m_dwValueLength(0UL)
    , m_pValue(nullptr)
{
}


Hash::Hash(PCWSTR pszAlgorithm)
	: m_pszAlgorithm(pszAlgorithm)
    , m_pDataFeeder()
    , m_dwValueLength(0UL)
    , m_pValue(nullptr)
{
}


Hash::Hash(PCWSTR pszAlgorithm, RefPtr<DataFeeder> pDataFeeder)
    : m_pszAlgorithm(pszAlgorithm)
    , m_pDataFeeder(pDataFeeder)
    , m_dwValueLength(0UL)
    , m_pValue(nullptr)
{
}


Hash::Hash(const Hash& other)
    : m_pszAlgorithm(other.m_pszAlgorithm)
    , m_pDataFeeder(other.m_pDataFeeder)
    , m_dwValueLength(other.m_dwValueLength)
    , m_pValue(other.m_dwValueLength ? new BYTE[other.m_dwValueLength] : nullptr)
{
    memcpy_s(m_pValue, m_dwValueLength, other.m_pValue, other.m_dwValueLength);
}


Hash::~Hash()
{
    delete[] Interlocked<PBYTE>::ExchangePointer(&m_pValue, nullptr);
}


void Hash::operator =(const Hash& other)
{
    m_pszAlgorithm = other.m_pszAlgorithm;
    m_pDataFeeder = other.m_pDataFeeder;
    m_dwValueLength = other.m_dwValueLength;
    delete[] Interlocked<PBYTE>::ExchangePointer(&m_pValue, other.m_dwValueLength ? new BYTE[other.m_dwValueLength] : nullptr);
    memcpy_s(m_pValue, m_dwValueLength, other.m_pValue, other.m_dwValueLength);
}


void Hash::Close()
{
    m_dwValueLength = 0UL;
    delete[] Interlocked<PBYTE>::ExchangePointer(&m_pValue, nullptr);
    m_pDataFeeder = RefPtr<DataFeeder>();
}


Hash& Hash::Set(RefPtr<DataFeeder> pDataFeeder)
{
    m_pDataFeeder = pDataFeeder;
    return *this;
}


Hash& Hash::Set(const void* ptr, size_t cb)
{
    m_pDataFeeder = RefPtr<DataFeeder>(new BYTEDATAFEEDER(ptr, cb));
    return *this;
}


void Hash::Compute() const
{
    BCryptAlgHandle hAlg;
    BCryptHashHandle hHash;
    hAlg.Open(m_pszAlgorithm);
    DWORD dwValueLength = hAlg.HashLength;
    delete[] Interlocked<PBYTE>::ExchangePointer(&m_pValue, new BYTE[dwValueLength]);
    hHash.Open(hAlg);
    if (m_pDataFeeder)
    {
        while (m_pDataFeeder->HasNext())
        {
            hHash.Feed(m_pDataFeeder->Data, m_pDataFeeder->DataLength);
        }
    }
    hHash.Finalize(m_pValue, dwValueLength);
    m_dwValueLength = dwValueLength;
}


const BYTE* Hash::get_Value() const
{
    if (!m_dwValueLength)
    {
        Compute();
    }
    return m_pValue;
}


DWORD Hash::get_ValueLength() const
{
    if (!m_dwValueLength)
    {
        Compute();
    }
    return m_dwValueLength;
}


MD5Hash::MD5Hash(RefPtr<DataFeeder> pDataFeeder)
    : Hash(BCRYPT_MD5_ALGORITHM, pDataFeeder)
{
}


MD5Hash::MD5Hash(const void* ptr, size_t cb)
    : Hash(BCRYPT_MD5_ALGORITHM, RefPtr<DataFeeder>(new BYTEDATAFEEDER(ptr, cb)))
{
}


MD5Hash::MD5Hash(const MD5Hash& src)
    : Hash(src)
{
}


void MD5Hash::operator =(const MD5Hash& src)
{
    Hash::operator =(src);
}


SHA1Hash::SHA1Hash(RefPtr<DataFeeder> pDataFeeder)
    : Hash(BCRYPT_SHA1_ALGORITHM, pDataFeeder)
{
}


SHA1Hash::SHA1Hash(const void* ptr, size_t cb)
    : Hash(BCRYPT_SHA1_ALGORITHM, RefPtr<DataFeeder>(new BYTEDATAFEEDER(ptr, cb)))
{
}


SHA1Hash::SHA1Hash(const SHA1Hash& src)
    : Hash(src)
{
}


void SHA1Hash::operator =(const SHA1Hash& src)
{
    Hash::operator =(src);
}


SHA256Hash::SHA256Hash(RefPtr<DataFeeder> pDataFeeder)
    : Hash(BCRYPT_SHA256_ALGORITHM, pDataFeeder)
{
}


SHA256Hash::SHA256Hash(const void* ptr, size_t cb)
    : Hash(BCRYPT_SHA256_ALGORITHM, RefPtr<DataFeeder>(new BYTEDATAFEEDER(ptr, cb)))
{
}


SHA256Hash::SHA256Hash(const SHA256Hash& src)
    : Hash(src)
{
}


void SHA256Hash::operator =(const SHA256Hash& src)
{
    Hash::operator =(src);
}


SHA384Hash::SHA384Hash(RefPtr<DataFeeder> pDataFeeder)
    : Hash(BCRYPT_SHA384_ALGORITHM, pDataFeeder)
{
}


SHA384Hash::SHA384Hash(const void* ptr, size_t cb)
    : Hash(BCRYPT_SHA384_ALGORITHM, RefPtr<DataFeeder>(new BYTEDATAFEEDER(ptr, cb)))
{
}


SHA384Hash::SHA384Hash(const SHA384Hash& src)
    : Hash(src)
{
}


void SHA384Hash::operator =(const SHA384Hash& src)
{
    Hash::operator =(src);
}


SHA512Hash::SHA512Hash(RefPtr<DataFeeder> pDataFeeder)
    : Hash(BCRYPT_SHA512_ALGORITHM, pDataFeeder)
{
}


SHA512Hash::SHA512Hash(const void* ptr, size_t cb)
    : Hash(BCRYPT_SHA512_ALGORITHM, RefPtr<DataFeeder>(new BYTEDATAFEEDER(ptr, cb)))
{
}


SHA512Hash::SHA512Hash(const SHA512Hash& src)
    : Hash(src)
{
}


void SHA512Hash::operator =(const SHA512Hash& src)
{
    Hash::operator =(src);
}
