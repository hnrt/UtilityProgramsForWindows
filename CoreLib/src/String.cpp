#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/RefStr.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G   C L A S S
//
//////////////////////////////////////////////////////////////////////


inline PCWSTR AddRef(PCWSTR psz)
{
    if (psz)
    {
        RefStr::Get(psz).AddRef();
    }
    return psz;
}


inline void Release(PCWSTR psz)
{
    if (psz)
    {
        RefStr::Get(psz).Release();
    }
}


const String String::Empty = String();


String::String()
    : m_psz(nullptr)
{
}


String::String(const String& other)
    : m_psz(AddRef(other.m_psz))
{
}


String::String(PCWSTR psz, INT_PTR cch)
    : m_psz(RefStr::Create(psz, cch))
{
}


String::String(PCWSTR pszFormat, va_list argList)
    : m_psz(RefStr::Create(pszFormat, argList))
{
}


String::String(StringOptions option, PCWSTR psz, ...)
    : m_psz(nullptr)
{
    va_list argList;
    va_start(argList, psz);
    switch (option)
    {
    case PRINTF:
        m_psz = RefStr::Create(psz, argList);
        break;
    case CONCAT:
    case CONCAT2:
    case CONCAT3:
    case CONCAT4:
    case CONCAT5:
    case CONCAT6:
    case CONCAT7:
    case CONCAT8:
    case CONCAT9:
        m_psz = RefStr::Create(option, psz, argList);
        break;
    case UPPERCASE:
    case LOWERCASE:
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
        m_psz = RefStr::Create(option, psz);
        break;
    default:
        throw Exception(L"String::ctor: Bad option.");
    }
    va_end(argList);
}


String::String(PCWSTR psz1, PCWSTR psz2)
    : m_psz(RefStr::Create(psz1, psz2))
{
}


String::String(size_t cch, WCHAR fill)
    : m_psz(RefStr::Create(cch, fill))
{
}


String::String(UINT cp, PCSTR psz, INT_PTR cb)
    : m_psz(RefStr::Create(cp, psz, cb))
{
}


String::~String()
{
    Release(InterlockedExchangePCWSTR(&m_psz, nullptr));
}


String& String::ZeroFill()
{
    if (Len)
    {
        RefStr::Get(m_psz).ZeroFill();
    }
    return *this;
}


String& String::Lettercase(StringOptions option)
{
    if (Len)
    {
        Release(InterlockedExchangePCWSTR(&m_psz, RefStr::Create(option, Ptr)));
    }
    return *this;
}


String& String::Trim(StringOptions option)
{
    if (Len)
    {
        Release(InterlockedExchangePCWSTR(&m_psz, RefStr::Create(option, Ptr)));
    }
    return *this;
}


String& String::Assign(const String& other)
{
    Release(InterlockedExchangePCWSTR(&m_psz, AddRef(other.m_psz)));
    return *this;
}


String& String::Assign(PCWSTR psz, INT_PTR cch)
{
    Release(InterlockedExchangePCWSTR(&m_psz, RefStr::Create(psz, cch)));
    return *this;
}


String& String::Append(const String& other)
{
    if (other.Len)
    {
        Release(InterlockedExchangePCWSTR(&m_psz, RefStr::Create(Ptr, other.Ptr)));
    }
    return *this;
}


String& String::Append(PCWSTR psz, INT_PTR cch)
{
    if (psz && cch)
    {
        Release(InterlockedExchangePCWSTR(&m_psz, RefStr::Create(Ptr, psz, cch)));
    }
    return *this;
}


String& String::Format(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    Release(InterlockedExchangePCWSTR(&m_psz, RefStr::Create(pszFormat, argList)));
    va_end(argList);
    return *this;
}


String& String::VaFormat(PCWSTR pszFormat, va_list argList)
{
    Release(InterlockedExchangePCWSTR(&m_psz, RefStr::Create(pszFormat, argList)));
    return *this;
}


String& String::AppendFormat(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    Release(InterlockedExchangePCWSTR(&m_psz, RefStr::Create(Ptr, pszFormat, argList)));
    va_end(argList);
    return *this;
}


String& String::VaAppendFormat(PCWSTR pszFormat, va_list argList)
{
    Release(InterlockedExchangePCWSTR(&m_psz, RefStr::Create(Ptr, pszFormat, argList)));
    return *this;
}


String& String::TruncateHead(size_t cch)
{
    if (m_psz)
    {
        RefStr& rs = RefStr::Get(m_psz);
        if (rs.RefCnt > 1)
        {
            Release(InterlockedExchangePCWSTR(&m_psz, cch < rs.Len ? RefStr::Create(&m_psz[cch]) : nullptr));
        }
        else if (cch < rs.Len)
        {
            size_t newLen = rs.Len - cch;
            PWSTR psz = const_cast<PWSTR>(m_psz);
            wmemmove_s(&psz[0], newLen, &psz[cch], newLen);
            wmemset(&psz[newLen], L'\0', cch);
            rs.SetLength(newLen);
        }
        else
        {
            rs.ZeroFill();
        }
    }
    return *this;
}


String& String::TruncateTail(size_t cch)
{
    if (m_psz)
    {
        RefStr& rs = RefStr::Get(m_psz);
        if (rs.RefCnt > 1)
        {
            Release(InterlockedExchangePCWSTR(&m_psz, cch < rs.Len ? RefStr::Create(m_psz, rs.Len - cch) : nullptr));
        }
        else if (cch < rs.Len)
        {
            size_t newLen = rs.Len - cch;
            PWSTR psz = const_cast<PWSTR>(m_psz);
            wmemset(&psz[newLen], L'\0', cch);
            rs.SetLength(newLen);
        }
        else
        {
            rs.ZeroFill();
        }
    }
    return *this;
}


int String::IndexOf(WCHAR c, INT_PTR fromIndex) const
{
    if (m_psz && static_cast<size_t>(fromIndex) < Len)
    {
        PCWSTR pCur = wmemchr(m_psz + fromIndex, c, Len - fromIndex);
        return pCur ? static_cast<int>(pCur - m_psz) : -1;
    }
    return -1;
}


int String::IndexOf(const String& s, INT_PTR fromIndex) const
{
    if (m_psz && static_cast<size_t>(fromIndex) < Len)
    {
        PCWSTR psz = s.Ptr;
        INT_PTR cch = s.Len;
        if (cch)
        {
            cch--;
            WCHAR c = *psz++;
            PCWSTR pCur = wmemchr(m_psz + fromIndex, c, Len - fromIndex);
            if (cch)
            {
                PCWSTR pEnd = m_psz + Len;
                while (pCur)
                {
                    pCur++;
                    if (pEnd < pCur + cch)
                    {
                        break;
                    }
                    if (!Compare(psz, cch, pCur, cch))
                    {
                        return static_cast<int>((pCur - 1) - m_psz);
                    }
                    pCur = wmemchr(pCur, c, pEnd - pCur);
                }
            }
            else if (pCur)
            {
                return static_cast<int>(pCur - m_psz);
            }
        }
    }
    return -1;
}


bool String::StartsWith(PCWSTR psz, INT_PTR cch) const
{
    size_t cchActual = cch > 0 ? wcsnlen(psz, cch) : cch < 0 ? wcslen(psz) : 0;
    if (cchActual == 0)
    {
        return true;
    }
    else if (cchActual <= Len)
    {
        return Compare(m_psz, cchActual, psz, cchActual) == 0;
    }
    else
    {
        return false;
    }
}


bool String::EndsWith(PCWSTR psz, INT_PTR cch) const
{
    size_t cchActual = cch > 0 ? wcsnlen(psz, cch) : cch < 0 ? wcslen(psz) : 0;
    if (cchActual == 0)
    {
        return true;
    }
    else if (cchActual <= Len)
    {
        return Compare(m_psz + Len - cchActual, cchActual, psz, cchActual) == 0;
    }
    else
    {
        return false;
    }
}


bool String::operator ==(const String& other) const
{
    return Compare(Ptr, other.Ptr) == 0;
}


bool String::operator !=(const String& other) const
{
    return Compare(Ptr, other.Ptr) != 0;
}


bool String::operator <(const String& other) const
{
    return Compare(Ptr, other.Ptr) < 0;
}


bool String::operator <=(const String& other) const
{
    return Compare(Ptr, other.Ptr) <= 0;
}


bool String::operator >(const String& other) const
{
    return Compare(Ptr, other.Ptr) > 0;
}


bool String::operator >=(const String& other) const
{
    return Compare(Ptr, other.Ptr) >= 0;
}


String String::operator +(const String& other) const
{
    return String(Ptr, other.Ptr);
}


PCWSTR String::get_ptr() const
{
    return m_psz ? m_psz : L"";
}


size_t String::get_len() const
{
    return m_psz ? RefStr::Get(m_psz).Len : 0;
}


int String::Compare(PCWSTR psz1, INT_PTR cch1, PCWSTR psz2, INT_PTR cch2)
{
    switch (CompareStringW(LOCALE_INVARIANT, 0, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
    {
    case CSTR_EQUAL:
        return 0;
    case CSTR_LESS_THAN:
        return -1;
    case CSTR_GREATER_THAN:
        return 1;
    default:
        throw Exception(L"String::Compare failed.");
    }
}


String String::ToHex(const void* ptr, size_t len, StringOptions option)
{
    static const WCHAR HexLU[2][16] = {
        { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'a', L'b', L'c', L'd', L'e', L'f' },
        { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' }
    };
    const WCHAR* hex = HexLU[option == UPPERCASE ? 1 : 0];
    String sz(len * 2, L'*');
    PWCHAR pW = const_cast<PWCHAR>(sz.m_psz);
    const unsigned char* pCur = reinterpret_cast<const unsigned char*>(ptr);
    const unsigned char* pEnd = pCur + len;
    while (pCur < pEnd)
    {
        unsigned char c = *pCur++;
        pW[0] = hex[(c >> 4) & 0xF];
        pW[1] = hex[(c >> 0) & 0xF];
        pW += 2;
    }
    return sz;
}
