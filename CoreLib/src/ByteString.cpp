#include "pch.h"
#include "hnrt/ByteString.h"
#include "hnrt/RefBin.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Win32Exception.h"


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


const unsigned char& ByteString::operator [](INT_PTR index) const
{
    INT_PTR len = Len;
    if (0 <= index && index < len)
    {
        return reinterpret_cast<const unsigned char*>(m_ptr)[index];
    }
    index += len;
    if (0 <= index && index < len)
    {
        return reinterpret_cast<const unsigned char*>(m_ptr)[index];
    }
    else
    {
        throw Exception(L"ByteString::operator []: Index out of range. Len=%zu Index=%zd", len, index - len);
    }
}


unsigned char& ByteString::operator [](INT_PTR index)
{
    INT_PTR len = Len;
    if (0 <= index && index < len)
    {
        return reinterpret_cast<unsigned char*>(m_ptr)[index];
    }
    index += len;
    if (0 <= index && index < len)
    {
        return reinterpret_cast<unsigned char*>(m_ptr)[index];
    }
    else
    {
        throw Exception(L"ByteString::operator []: Index out of range. Len=%zu Index=%zd", len, index - len);
    }
}


ByteString& ByteString::Resize(size_t len)
{
    size_t len0 = Len;
    if (len != len0)
    {
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
        else
        {
            len0 = 0;
        }
        if (len0 < len)
        {
            memset(reinterpret_cast<unsigned char*>(m_ptr) + len0, 0, len - len0);
        }
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


ByteString ByteString::Pkcs5Padding(DWORD dwBlockLength) const
{
    size_t PayloadLength = Len;
    size_t PaddingLength = dwBlockLength - Len % dwBlockLength;
    ByteString bs(PayloadLength + PaddingLength);
    memcpy_s(bs.m_ptr, PayloadLength, m_ptr, PayloadLength);
    memset(reinterpret_cast<unsigned char*>(bs.m_ptr) + PayloadLength, static_cast<int>(PaddingLength), PaddingLength);
    return bs;
}


ByteString& ByteString::RemovePkcs5Padding(DWORD dwBlockLength)
{
    size_t len = Len;
    if (len < dwBlockLength)
    {
        throw Exception(L"ByteString is too short.");
    }
    size_t PaddingLength = reinterpret_cast<unsigned char*>(m_ptr)[len - 1];
    if (PaddingLength > dwBlockLength)
    {
        throw Exception(L"Padding looks broken.");
    }
    return Resize(len - PaddingLength);
}


String ByteString::ToString(UINT uCodePage) const
{
    if (Len > 0)
    {
        if (uCodePage == CP_UTF16)
        {
            if ((Len % sizeof(WCHAR)) != 0)
            {
                throw Exception(L"ByteString::ToString: An extra byte follows.");
            }
            int len2 = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, reinterpret_cast<LPCWCH>(Ptr), static_cast<int>(Len / sizeof(WCHAR)), NULL, 0, NULL, NULL);
            if (len2 == 0)
            {
                throw Exception(L"ByteString::ToString: No translation.");
            }
            return String(reinterpret_cast<LPCWCH>(Ptr), static_cast<int>(Len / sizeof(WCHAR)));
        }
        else
        {
            int len2 = MultiByteToWideChar(uCodePage, MB_PRECOMPOSED, reinterpret_cast<LPCCH>(Ptr), static_cast<int>(Len), NULL, 0);
            if (len2 == 0)
            {
                throw Exception(L"ByteString::ToString: No translation.");
            }
            String sz(len2, L'\0');
            len2 = MultiByteToWideChar(uCodePage, MB_PRECOMPOSED, reinterpret_cast<LPCCH>(Ptr), static_cast<int>(Len), const_cast<LPWCH>(sz.Ptr), static_cast<int>(sz.Len));
            if (len2 == 0)
            {
                throw Exception(L"ByteString::ToString: No translation.");
            }
            return sz;
        }
    }
    else
    {
        return String::Empty;
    }
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
    size_t len = 0;
    const WCHAR* pCur = psz;
    WCHAR c = *pCur++;
    while (c)
    {
        if (iswxdigit(c))
        {
            c = *pCur++;
        }
        else if (iswspace(c))
        {
            c = *pCur++;
            continue;
        }
        else
        {
            throw Exception(L"ByteString::FromHex(\"%s\"): Illegal character at %zu.", psz, (pCur - 1) - psz);
        }
        if (iswxdigit(c))
        {
            len++;
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
    ByteString bs(len);
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
        else if (L'0' <= c && c <= L'9')
        {
            d = c - L'0';
        }
        else
        {
            c = *pCur++;
            continue;
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
    size_t len = 0;
    const CHAR* pCur = psz;
    CHAR c = *pCur++;
    while (c)
    {
        if (isxdigit(c))
        {
            c = *pCur++;
        }
        else if (isspace(c))
        {
            c = *pCur++;
            continue;
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
    ByteString bs(len);
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
        else if ('0' <= c && c <= '9')
        {
            d = c - '0';
        }
        else
        {
            c = *pCur++;
            continue;
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


ByteString ByteString::ToText(UINT uCodePage, PCWSTR psz, SSIZE_T len)
{
    if (len < 0)
    {
        len = StrLen(psz);
    }
    if (len > 0)
    {
        if (uCodePage == CP_UTF16)
        {
            return ByteString(psz, len * sizeof(WCHAR));
        }
        else
        {
            DWORD dwFlags = uCodePage == CP_EUCJP ? 0 : WC_ERR_INVALID_CHARS;
            BOOL bConvError = FALSE;
            int len2 = WideCharToMultiByte(uCodePage, dwFlags, psz, static_cast<int>(len), NULL, 0, NULL, &bConvError);
            if (len2 == 0)
            {
                throw Win32Exception(GetLastError(), L"ByteString: Conversion error.");
            }
            else if (bConvError)
            {
                throw Win32Exception(ERROR_INVALID_DATA, L"ByteString: Conversion error.");
            }
            ByteString bs(len2);
            len2 = WideCharToMultiByte(uCodePage, dwFlags, psz, static_cast<int>(len), reinterpret_cast<LPSTR>(bs.Ptr), static_cast<int>(bs.Len), NULL, &bConvError);
            if (len2 == 0)
            {
                throw Win32Exception(GetLastError(), L"ByteString: Conversion error.");
            }
            else if (bConvError)
            {
                throw Win32Exception(ERROR_INVALID_DATA, L"ByteString: Conversion error.");
            }
            return bs;
        }
    }
    else
    {
        return ByteString(0);
    }
}
