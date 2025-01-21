#include "pch.h"
#include "hnrt/ByteString.h"
#include "hnrt/RefBin.h"
#include "hnrt/StringCommons.h"
#include "hnrt/HexFormatException.h"
#include "hnrt/Base64FormatException.h"
#include "hnrt/CharacterMappingException.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   B Y T E   S T R I N G   C L A S S
//
//////////////////////////////////////////////////////////////////////


static const WCHAR EXTRA_BYTE_FOLLOWS[] = L"An extra byte follows.";
static const WCHAR ILLEGAL_CHARACTER_AT[] = L"Illegal character at %zu.";
static const WCHAR PREMATURE_END_OF_STRING[] = L"Premature end of string.";
static const WCHAR MALFORMED_STRING[] = L"Malformed string.";
static const WCHAR UNMAPPABLE_CHARACTERS[] = L"One or more unmappable characters.";
static const WCHAR INCONSISTENT_NUMBER_OF_CHARACTERS[] = L"Inconsistent number of characters.";


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


static const WCHAR s_HexEncodingTable[2][17] = {
    L"0123456789abcdef",
    L"0123456789ABCDEF"
};


String ByteString::ToHex(StringOptions LetterCase) const
{
    if (Len > 0)
    {
        String sz(Len * 2, L'\0');
        PWCH pDst = const_cast<PWCH>(sz.Ptr);
        const BYTE* pCur = reinterpret_cast<const BYTE*>(m_ptr);
        const BYTE* pEnd = reinterpret_cast<const BYTE*>(m_ptr) + Len;
        PCWSTR pszCharacters = s_HexEncodingTable[LetterCase == StringOptions::LOWERCASE ? 0 : 1];
        while (pCur < pEnd)
        {
            BYTE b = *pCur++;
            *pDst++ = pszCharacters[(b >> 4) & 0xF];
            *pDst++ = pszCharacters[(b >> 0) & 0xF];
        }
        return sz;
    }
    else
    {
        return String::Empty;
    }
}


static const WCHAR s_Base64EncodingTable[65] = { L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" };


String ByteString::ToBase64() const
{
    if (Len > 0)
    {
        SIZE_T rem = Len % 3;
        String sz(((Len + 2) / 3) * 4, L'\0');
        PWCH pDst = const_cast<PWCH>(sz.Ptr);
        const BYTE* pCur = reinterpret_cast<const BYTE*>(m_ptr);
        const BYTE* pEnd = reinterpret_cast<const BYTE*>(m_ptr) + Len - rem;
        while (pCur < pEnd)
        {
            int b0 = static_cast<int>(*pCur++);
            int b1 = static_cast<int>(*pCur++);
            int b2 = static_cast<int>(*pCur++);
            *pDst++ = s_Base64EncodingTable[b0 >> 2];
            *pDst++ = s_Base64EncodingTable[((b0 << 4) | (b1 >> 4)) & 0x3F];
            *pDst++ = s_Base64EncodingTable[((b1 << 2) | (b2 >> 6)) & 0x3F];
            *pDst++ = s_Base64EncodingTable[b2 & 0x3F];
        }
        if (rem == 1)
        {
            int b0 = static_cast<int>(*pCur++);
            *pDst++ = s_Base64EncodingTable[b0 >> 2];
            *pDst++ = s_Base64EncodingTable[(b0 << 4) & 0x3F];
            *pDst++ = L'=';
            *pDst++ = L'=';

        }
        else if (rem == 2)
        {
            int b0 = static_cast<int>(*pCur++);
            int b1 = static_cast<int>(*pCur++);
            *pDst++ = s_Base64EncodingTable[b0 >> 2];
            *pDst++ = s_Base64EncodingTable[((b0 << 4) | (b1 >> 4)) & 0x3F];
            *pDst++ = s_Base64EncodingTable[(b1 << 2) & 0x3F];
            *pDst++ = L'=';
        }
        return sz;
    }
    else
    {
        return String::Empty;
    }
}


String ByteString::ToString(UINT uCodePage, bool bStrict) const
{
    if (Len > 0)
    {
        if (uCodePage == CP_UTF16)
        {
            if (bStrict && (Len % sizeof(WCHAR)) != 0)
            {
                throw CharacterMappingException(EXTRA_BYTE_FOLLOWS);
            }
            LPCWCH pch = reinterpret_cast<LPCWCH>(Ptr);
            int cch = static_cast<int>(Len / sizeof(WCHAR));
            if (bStrict && CountCharacter(pch, cch, 0) > 0)
            {
                throw CharacterMappingException(MALFORMED_STRING);
            }
            int cb = WideCharToMultiByte(CP_UTF8, bStrict ? WC_ERR_INVALID_CHARS : 0, pch, cch, NULL, 0, NULL, NULL);
            if (cb == 0)
            {
                throw CharacterMappingException(GetLastError());
            }
            return String(pch, cch);
        }
        else if (uCodePage)
        {
            LPCCH pb = reinterpret_cast<LPCCH>(Ptr);
            int cb = static_cast<int>(Len);
            DWORD dwFlags = MB_PRECOMPOSED | (bStrict ? MB_ERR_INVALID_CHARS : 0);
            int cch = MultiByteToWideChar(uCodePage, dwFlags, pb, cb, NULL, 0);
            if (cch == 0)
            {
                throw CharacterMappingException(GetLastError());
            }
            String sz(cch, L'\0');
            LPWCH pch = const_cast<LPWCH>(sz.Ptr);
            cch = MultiByteToWideChar(uCodePage, dwFlags, pb, cb, pch, cch);
            if (cch == 0)
            {
                throw CharacterMappingException(GetLastError());
            }
            else if (cch != sz.Len)
            {
                throw CharacterMappingException(INCONSISTENT_NUMBER_OF_CHARACTERS);
            }
            else if (bStrict && (CountCharacter(pch, cch, REPLACEMENT_CHARACTER) > 0 || CountCharacter(pch, cch, 0) > 0))
            {
                throw CharacterMappingException(UNMAPPABLE_CHARACTERS);
            }
            return sz;
        }
        else
        {
            static const BYTE bomUTF8[] = { 0xEF, 0xBB, 0xBF };
            static const BYTE bomUTF16[] = { 0xFF, 0xFE };
            if (Len >= 3 && !memcmp(Ptr, bomUTF8, 3))
            {
                try
                {
                    return ToString(CP_UTF8, true);
                }
                catch (...)
                {
                }
            }
            else if (Len >= 2 && !memcmp(Ptr, bomUTF16, 2))
            {
                try
                {
                    return ToString(CP_UTF16, true);
                }
                catch (...)
                {
                }
            }
            else
            {
                try
                {
                    return ToString(CP_UTF8, true);
                }
                catch (...)
                {
                }
                try
                {
                    return ToString(CP_UTF16, true);
                }
                catch (...)
                {
                }
            }
            UINT acp = GetACP();
            if (acp)
            {
                try
                {
                    return ToString(acp, true);
                }
                catch (...)
                {
                }
            }
            UINT cps[] = { 932, 936, 949, 950, 1250, 1251, 1252, 1253, 1254, 1255, 1256, 1257, 1258 };
            for (UINT index = 0; index < _countof(cps); index++)
            {
                if (cps[index] == acp)
                {
                    continue;
                }
                try
                {
                    return ToString(cps[index], true);
                }
                catch (...)
                {
                }
            }
            throw CharacterMappingException(UNMAPPABLE_CHARACTERS);
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
    SIZE_T len = 0;
    PCWCH pCur = psz;
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
            throw HexFormatException((pCur - 1) - psz, ILLEGAL_CHARACTER_AT, (pCur - 1) - psz);
        }
        if (iswxdigit(c))
        {
            c = *pCur++;
        }
        else
        {
            throw HexFormatException((pCur - 1) - psz, c ? ILLEGAL_CHARACTER_AT : PREMATURE_END_OF_STRING, (pCur - 1) - psz);
        }
        len++;
    }
    ByteString bs(len);
    PBYTE pW = bs;
    pCur = psz;
    c = *pCur++;
    while (c)
    {
        BYTE d;
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
    SIZE_T len = 0;
    PCCH pCur = psz;
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
            throw HexFormatException((pCur - 1) - psz, ILLEGAL_CHARACTER_AT, (pCur - 1) - psz);
        }
        if (isxdigit(c))
        {
            c = *pCur++;
        }
        else
        {
            throw HexFormatException((pCur - 1) - psz, c ? ILLEGAL_CHARACTER_AT : PREMATURE_END_OF_STRING, (pCur - 1) - psz);
        }
        len++;
    }
    ByteString bs(len);
    PBYTE pW = bs;
    pCur = psz;
    c = *pCur++;
    while (c)
    {
        BYTE d;
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


static const signed char s_Base64DecodingTable[128] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};


ByteString ByteString::FromBase64(PCWSTR psz)
{
    SIZE_T len = 0;
    PCWCH pCur = psz;
    WCHAR c = *pCur++;
    while (c)
    {
        int b0 = c < 0x80 ? s_Base64DecodingTable[c] : -1;
        if (b0 >= 0)
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
            throw Base64FormatException((pCur - 1) - psz, ILLEGAL_CHARACTER_AT, (pCur - 1) - psz);
        }
        int b1 = c < 0x80 ? s_Base64DecodingTable[c] : -1;
        if (b1 >= 0)
        {
            c = *pCur++;
        }
        else
        {
            throw Base64FormatException((pCur - 1) - psz, c ? ILLEGAL_CHARACTER_AT : PREMATURE_END_OF_STRING, (pCur - 1) - psz);
        }
        if (c == L'=')
        {
            c = *pCur++;
            if (c == L'=')
            {
                c = *pCur++;
            }
            else
            {
                throw Base64FormatException((pCur - 1) - psz, c ? ILLEGAL_CHARACTER_AT : PREMATURE_END_OF_STRING, (pCur - 1) - psz);
            }
            while (iswspace(c))
            {
                c = *pCur++;
            }
            if (c)
            {
                throw Base64FormatException((pCur - 1) - psz, ILLEGAL_CHARACTER_AT, (pCur - 1) - psz);
            }
            len++;
            break;
        }
        int b2 = c < 0x80 ? s_Base64DecodingTable[c] : -1;
        if (b2 >= 0)
        {
            c = *pCur++;
        }
        else
        {
            throw Base64FormatException((pCur - 1) - psz, c ? ILLEGAL_CHARACTER_AT : PREMATURE_END_OF_STRING, (pCur - 1) - psz);
        }
        if (c == L'=')
        {
            c = *pCur++;
            while (iswspace(c))
            {
                c = *pCur++;
            }
            if (c)
            {
                throw Base64FormatException((pCur - 1) - psz, ILLEGAL_CHARACTER_AT, (pCur - 1) - psz);
            }
            len += 2;
            break;
        }
        int b3 = c < 0x80 ? s_Base64DecodingTable[c] : -1;
        if (b3 >= 0)
        {
            c = *pCur++;
        }
        else
        {
            throw Base64FormatException((pCur - 1) - psz, c ? ILLEGAL_CHARACTER_AT : PREMATURE_END_OF_STRING, (pCur - 1) - psz);
        }
        len += 3;
    }
#pragma warning(disable:6385)
    ByteString bs(len);
    SIZE_T rem = len % 3;
    PBYTE pW = bs;
    PBYTE pX = pW + len - rem;
    pCur = psz;
    while (pW < pX)
    {
        while (iswspace(*pCur))
        {
            pCur++;
        }
        int b0 = s_Base64DecodingTable[*pCur++];
        int b1 = s_Base64DecodingTable[*pCur++];
        int b2 = s_Base64DecodingTable[*pCur++];
        int b3 = s_Base64DecodingTable[*pCur++];
        *pW++ = static_cast<BYTE>((b0 << 2) | (b1 >> 4));
        *pW++ = static_cast<BYTE>(((b1 & 0x0F) << 4) | (b2 >> 2));
        *pW++ = static_cast<BYTE>(((b2 & 0x03) << 6) | b3);
    }
    if (rem)
    {
        while (iswspace(*pCur))
        {
            pCur++;
        }
        if (rem == 1)
        {
            int b0 = s_Base64DecodingTable[*pCur++];
            int b1 = s_Base64DecodingTable[*pCur++];
            *pW++ = static_cast<BYTE>((b0 << 2) | (b1 >> 4));
        }
        else // if (rem == 2)
        {
            int b0 = s_Base64DecodingTable[*pCur++];
            int b1 = s_Base64DecodingTable[*pCur++];
            int b2 = s_Base64DecodingTable[*pCur++];
            *pW++ = static_cast<BYTE>((b0 << 2) | (b1 >> 4));
            *pW++ = static_cast<BYTE>(((b1 & 0x0F) << 4) | (b2 >> 2));
        }
    }
#pragma warning(default:6385)
    return bs;
}


ByteString ByteString::FromBase64(PCSTR psz)
{
    SIZE_T len = 0;
    PCCH pCur = psz;
    CHAR c = *pCur++;
    while (c)
    {
        int b0 = c < 0x80 ? s_Base64DecodingTable[c] : -1;
        if (b0 >= 0)
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
            throw Base64FormatException((pCur - 1) - psz, ILLEGAL_CHARACTER_AT, (pCur - 1) - psz);
        }
        int b1 = c < 0x80 ? s_Base64DecodingTable[c] : -1;
        if (b1 >= 0)
        {
            c = *pCur++;
        }
        else
        {
            throw Base64FormatException((pCur - 1) - psz, c ? ILLEGAL_CHARACTER_AT : PREMATURE_END_OF_STRING, (pCur - 1) - psz);
        }
        if (c == '=')
        {
            c = *pCur++;
            if (c == '=')
            {
                c = *pCur++;
            }
            else
            {
                throw Base64FormatException((pCur - 1) - psz, c ? ILLEGAL_CHARACTER_AT : PREMATURE_END_OF_STRING, (pCur - 1) - psz);
            }
            while (isspace(c))
            {
                c = *pCur++;
            }
            if (c)
            {
                throw Base64FormatException((pCur - 1) - psz, ILLEGAL_CHARACTER_AT, (pCur - 1) - psz);
            }
            len++;
            break;
        }
        int b2 = c < 0x80 ? s_Base64DecodingTable[c] : -1;
        if (b2 >= 0)
        {
            c = *pCur++;
        }
        else
        {
            throw Base64FormatException((pCur - 1) - psz, c ? ILLEGAL_CHARACTER_AT : PREMATURE_END_OF_STRING, (pCur - 1) - psz);
        }
        if (c == '=')
        {
            c = *pCur++;
            while (isspace(c))
            {
                c = *pCur++;
            }
            if (c)
            {
                throw Base64FormatException((pCur - 1) - psz, ILLEGAL_CHARACTER_AT, (pCur - 1) - psz);
            }
            len += 2;
            break;
        }
        int b3 = c < 0x80 ? s_Base64DecodingTable[c] : -1;
        if (b3 >= 0)
        {
            c = *pCur++;
        }
        else
        {
            throw Base64FormatException((pCur - 1) - psz, c ? ILLEGAL_CHARACTER_AT : PREMATURE_END_OF_STRING, (pCur - 1) - psz);
        }
        len += 3;
    }
#pragma warning(disable:6385)
    ByteString bs(len);
    SIZE_T rem = len % 3;
    PBYTE pW = bs;
    PBYTE pX = pW + len - rem;
    pCur = psz;
    while (pW < pX)
    {
        while (isspace(*pCur))
        {
            pCur++;
        }
        int b0 = s_Base64DecodingTable[*pCur++];
        int b1 = s_Base64DecodingTable[*pCur++];
        int b2 = s_Base64DecodingTable[*pCur++];
        int b3 = s_Base64DecodingTable[*pCur++];
        *pW++ = static_cast<BYTE>((b0 << 2) | (b1 >> 4));
        *pW++ = static_cast<BYTE>(((b1 & 0x0F) << 4) | (b2 >> 2));
        *pW++ = static_cast<BYTE>(((b2 & 0x03) << 6) | b3);
    }
    if (rem)
    {
        while (isspace(*pCur))
        {
            pCur++;
        }
        if (rem == 1)
        {
            int b0 = s_Base64DecodingTable[*pCur++];
            int b1 = s_Base64DecodingTable[*pCur++];
            *pW++ = static_cast<BYTE>((b0 << 2) | (b1 >> 4));
        }
        else // if (rem == 2)
        {
            int b0 = s_Base64DecodingTable[*pCur++];
            int b1 = s_Base64DecodingTable[*pCur++];
            int b2 = s_Base64DecodingTable[*pCur++];
            *pW++ = static_cast<BYTE>((b0 << 2) | (b1 >> 4));
            *pW++ = static_cast<BYTE>(((b1 & 0x0F) << 4) | (b2 >> 2));
        }
    }
#pragma warning(default:6385)
    return bs;
}


ByteString ByteString::FromString(const String& strSource, UINT uCodePage, LineBreak lbSpec)
{
    if (strSource.Len > 0)
    {
        String str = strSource.ChangeLineBreak(lbSpec);
        if (uCodePage == CP_UTF16)
        {
            return ByteString(str.Ptr, str.Len * sizeof(WCHAR));
        }
        else
        {
            DWORD dwFlags =
                uCodePage == CP_UTF8 ? (WC_ERR_INVALID_CHARS | WC_NO_BEST_FIT_CHARS) :
                uCodePage == CP_GB18030 ? (WC_ERR_INVALID_CHARS | WC_NO_BEST_FIT_CHARS) :
                WC_NO_BEST_FIT_CHARS;
            BOOL bConvError = FALSE;
            PBOOL pbConvError = uCodePage == CP_UTF8 ? NULL : &bConvError;
            int cb = WideCharToMultiByte(uCodePage, dwFlags, str.Ptr, static_cast<int>(str.Len), NULL, 0, NULL, pbConvError);
            if (cb == 0)
            {
                throw CharacterMappingException(GetLastError());
            }
            else if (bConvError)
            {
                throw CharacterMappingException(UNMAPPABLE_CHARACTERS);
            }
            ByteString bs(cb);
            cb = WideCharToMultiByte(uCodePage, dwFlags, str.Ptr, static_cast<int>(str.Len), reinterpret_cast<LPSTR>(bs.Ptr), static_cast<int>(bs.Len), NULL, pbConvError);
            if (cb == 0)
            {
                throw CharacterMappingException(GetLastError());
            }
            else if (bConvError)
            {
                throw CharacterMappingException(UNMAPPABLE_CHARACTERS);
            }
            else if (cb != bs.Len)
            {
                throw CharacterMappingException(INCONSISTENT_NUMBER_OF_CHARACTERS);
            }
            return bs;
        }
    }
    else
    {
        return ByteString(0);
    }
}
