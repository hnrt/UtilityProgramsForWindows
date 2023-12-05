#include "pch.h"
#include "hnrt/ByteString.h"
#include "hnrt/RefBin.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   B Y T E   S T R I N G   C L A S S
//
//////////////////////////////////////////////////////////////////////


inline void* AddRef(void* ptr)
{
    if (ptr)
    {
        RefBin::Get(ptr).AddRef();
    }
    return ptr;
}


inline void Release(void* ptr)
{
    if (ptr)
    {
        RefBin::Get(ptr).Release();
    }
}


ByteString::ByteString(size_t len)
    : m_ptr(len ? RefBin::Create(len) : nullptr)
{
}

ByteString::ByteString(const void* ptr, size_t len)
    : m_ptr(len ? RefBin::Create(ptr, len) : nullptr)
{
}


ByteString::ByteString(const ByteString& other)
    : m_ptr(AddRef(other.m_ptr))
{
}


ByteString::~ByteString()
{
    Release(_InterlockedExchangePointer(&m_ptr, nullptr));
}


ByteString& ByteString::operator =(const ByteString & other)
{
    Release(_InterlockedExchangePointer(&m_ptr, AddRef(other.m_ptr)));
    return *this;
}


ByteString& ByteString::operator +=(const ByteString& other)
{
    if (m_ptr)
    {
        if (other.m_ptr)
        {
            size_t len1 = RefBin::Get(m_ptr).Len;
            size_t len2 = RefBin::Get(other.m_ptr).Len;
            void* ptr1 = _InterlockedExchangePointer(&m_ptr, RefBin::Create(len1 + len2));
            memcpy_s(m_ptr, len1, ptr1, len1);
            memcpy_s(reinterpret_cast<unsigned char*>(m_ptr) + len1, len2, other.m_ptr, len2);
            Release(ptr1);
        }
    }
    else
    {
        m_ptr = AddRef(other.m_ptr);
    }
    return *this;
}


ByteString ByteString::operator +(const ByteString& other)
{
    ByteString bs(*this);
    bs += other;
    return bs;
}


ByteString& ByteString::Resize(size_t len)
{
    size_t len0 = Len;
    void* ptr0 = _InterlockedExchangePointer(&m_ptr, len ? RefBin::Create(len) : nullptr);
    if (ptr0)
    {
        if (m_ptr)
        {
            if (len0 > len)
            {
                len0 = len;
            }
            memcpy_s(m_ptr, len, ptr0, len0);
        }
        RefBin::Get(ptr0).Release();
    }
    return *this;
}


ByteString& ByteString::Fill(int value)
{
    memset(m_ptr, value, Len);
    return *this;
}


ByteString ByteString::Clone() const
{
    size_t len = Len;
    ByteString bs(len);
    memcpy_s(bs.m_ptr, len, m_ptr, len);
    return bs;
}


size_t ByteString::get_Len() const
{
    return m_ptr ? RefBin::Get(m_ptr).Len : 0;
}


int ByteString::Compare(const ByteString& bs1, const ByteString& bs2)
{
    size_t len1 = bs1.Len;
    size_t len2 = bs2.Len;
    if (len1 < len2)
    {
        int d = memcmp(bs1.m_ptr, bs2.m_ptr, len1);
        return d > 0 ? 1 : -1;
    }
    else
    {
        int d = memcmp(bs1.m_ptr, bs2.m_ptr, len2);
        return d < 0 ? -1 : len1 > len2 ? 1 : d > 0 ? 1 : 0;
    }
}


ByteString ByteString::FromHex(PCWSTR psz)
{
    const WCHAR* pCur = psz;
    WCHAR c = *pCur++;
    while (c)
    {
        if (iswxdigit(c))
        {
            c = *pCur++;
        }
        else
        {
            throw Exception(L"ByteString::FromHex(\"%s\"): Illegal character at %zu.", psz, (pCur - 1) - psz);
        }
        if (iswxdigit(c))
        {
            c = *pCur++;
        }
        else if (c)
        {
            throw Exception(L"ByteString::FromHex(\"%s\"): Illegal character at %zu.", psz, (pCur - 1) - psz);
        }
        else
        {
            throw Exception(L"ByteString::FromHex(\"%s\"): Unexpected end of string.", psz);
        }
    }
    ByteString bs(((pCur - 1) - psz) / 2ULL);
    unsigned char* pW = bs;
    pCur = psz;
    c = *pCur++;
    while (c)
    {
        unsigned char d;
        if (L'A' <= c && c <= L'F')
        {
            d = 10 + c - L'A';
        }
        else if (L'a' <= c && c <= L'f')
        {
            d = 10 + c - L'a';
        }
        else // if (L'0' <= c && c <= L'9')
        {
            d = c - L'0';
        }
        d <<= 4;
        c = *pCur++;
        if (L'A' <= c && c <= L'F')
        {
            d |= 10 + c - L'A';
        }
        else if (L'a' <= c && c <= L'f')
        {
            d |= 10 + c - L'a';
        }
        else // if (L'0' <= c && c <= L'9')
        {
            d |= c - L'0';
        }
        *pW++ = d;
        c = *pCur++;
    }
    return bs;
}


ByteString ByteString::FromHex(PCSTR psz)
{
    const CHAR* pCur = psz;
    CHAR c = *pCur++;
    while (c)
    {
        if (isxdigit(c))
        {
            c = *pCur++;
        }
        else
        {
            throw Exception(L"ByteString::FromHex(\"%S\"): Illegal character at %zu.", psz, (pCur - 1) - psz);
        }
        if (isxdigit(c))
        {
            c = *pCur++;
        }
        else if (c)
        {
            throw Exception(L"ByteString::FromHex(\"%S\"): Illegal character at %zu.", psz, (pCur - 1) - psz);
        }
        else
        {
            throw Exception(L"ByteString::FromHex(\"%S\"): Unexpected end of string.", psz);
        }
    }
    ByteString bs(((pCur - 1) - psz) / 2ULL);
    unsigned char* pW = bs;
    pCur = psz;
    c = *pCur++;
    while (c)
    {
        unsigned char d;
        if ('A' <= c && c <= 'F')
        {
            d = 10 + c - 'A';
        }
        else if ('a' <= c && c <= 'f')
        {
            d = 10 + c - 'a';
        }
        else // if ('0' <= c && c <= '9')
        {
            d = c - '0';
        }
        d <<= 4;
        c = *pCur++;
        if ('A' <= c && c <= 'F')
        {
            d |= 10 + c - 'A';
        }
        else if ('a' <= c && c <= 'f')
        {
            d |= 10 + c - 'a';
        }
        else // if ('0' <= c && c <= '9')
        {
            d |= c - '0';
        }
        *pW++ = d;
        c = *pCur++;
    }
    return bs;
}
