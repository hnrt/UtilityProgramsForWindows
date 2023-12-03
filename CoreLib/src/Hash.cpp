#include "pch.h"
#include "hnrt/Hash.h"
#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/BCryptHashHandle.h"
#include "hnrt/CryptographyException.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Heap.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


Hash::Hash()
    : m_pszAlgorithm(nullptr)
    , m_pDataFeeder(nullptr)
    , m_dwValueLength(0UL)
    , m_pValue(nullptr)
    , m_sz()
{
}


Hash::Hash(PCWSTR pszAlgorithm, DataFeeder& dataFeeder)
	: m_pszAlgorithm(pszAlgorithm)
    , m_pDataFeeder(&dataFeeder)
    , m_dwValueLength(0UL)
    , m_pValue(nullptr)
	, m_sz()
{
}


Hash::Hash(const Hash& other)
    : m_pszAlgorithm(other.m_pszAlgorithm)
    , m_pDataFeeder(other.m_pDataFeeder)
    , m_dwValueLength(other.m_dwValueLength)
    , m_pValue(other.m_dwValueLength ? new BYTE[other.m_dwValueLength] : nullptr)
    , m_sz(other.m_sz)
{
    memcpy_s(m_pValue, m_dwValueLength, other.m_pValue, other.m_dwValueLength);
}


Hash::~Hash()
{
    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pValue, nullptr);
}


void Hash::operator =(const Hash& other)
{
    m_pszAlgorithm = other.m_pszAlgorithm;
    m_pDataFeeder = other.m_pDataFeeder;
    m_dwValueLength = other.m_dwValueLength;
    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pValue, other.m_dwValueLength ? new BYTE[other.m_dwValueLength] : nullptr);
    m_sz = other.m_sz;
    memcpy_s(m_pValue, m_dwValueLength, other.m_pValue, other.m_dwValueLength);
}


void Hash::Close()
{
    m_dwValueLength = 0UL;
    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pValue, nullptr);
    m_sz = String::Empty;
}


void Hash::Compute() const
{
    BCryptAlgHandle hAlg;
    BCryptHashHandle hHash;
    hAlg.Open(m_pszAlgorithm);
    DWORD dwValueLength = hAlg.HashLength;
    delete[] Interlocked<PUCHAR>::ExchangePointer(&m_pValue, new BYTE[dwValueLength]);
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


const String& Hash::get_Text() const
{
    if (!m_sz.Len)
    {
        if (!m_dwValueLength)
        {
            Compute();
        }
        DWORD dwLength = m_dwValueLength * 2;
        m_sz = String(dwLength, L' ');
        PWSTR psz = const_cast<PWSTR>(m_sz.Ptr);
        for (DWORD dwIndex = 0; dwIndex < m_dwValueLength; dwIndex++)
        {
            static const WCHAR hex[] = L"0123456789abcdef";
            psz[dwIndex * 2 + 0] = hex[(m_pValue[dwIndex] >> (4 * 1)) & 0xF];
            psz[dwIndex * 2 + 1] = hex[(m_pValue[dwIndex] >> (4 * 0)) & 0xF];
        }
    }
	return m_sz;
}


MD5Hash::MD5Hash(DataFeeder& rDataFeeder)
    : Hash(BCRYPT_MD5_ALGORITHM, rDataFeeder)
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


SHA1Hash::SHA1Hash(DataFeeder& rDataFeeder)
    : Hash(BCRYPT_SHA1_ALGORITHM, rDataFeeder)
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


SHA256Hash::SHA256Hash(DataFeeder& rDataFeeder)
    : Hash(BCRYPT_SHA256_ALGORITHM, rDataFeeder)
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


SHA384Hash::SHA384Hash(DataFeeder& rDataFeeder)
    : Hash(BCRYPT_SHA384_ALGORITHM, rDataFeeder)
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


SHA512Hash::SHA512Hash(DataFeeder& rDataFeeder)
    : Hash(BCRYPT_SHA512_ALGORITHM, rDataFeeder)
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