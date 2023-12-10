#include "pch.h"
#include "hnrt/StringAcp.h"
#include "hnrt/RefMbs.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G - A C P   C L A S S
//
//////////////////////////////////////////////////////////////////////


inline PCSTR AddRef(PCSTR psz)
{
    if (psz)
    {
        RefMbs::Get(psz).AddRef();
    }
    return psz;
}


inline void Release(PCSTR psz)
{
    if (psz)
    {
        RefMbs::Get(psz).Release();
    }
}


const StringAcp StringAcp::Empty = StringAcp();


StringAcp::StringAcp()
    : m_psz(nullptr)
{
}


StringAcp::StringAcp(const StringAcp& other)
    : m_psz(AddRef(other.m_psz))
{
}


StringAcp::StringAcp(PCSTR psz, INT_PTR cb)
    : m_psz(RefMbs::Create(psz, cb))
{
}


StringAcp::StringAcp(PCSTR pszFormat, va_list argList)
    : m_psz(RefMbs::Create(pszFormat, argList))
{
}


StringAcp::StringAcp(StringOptions option, PCSTR psz, ...)
    : m_psz(nullptr)
{
    va_list argList;
    va_start(argList, psz);
    switch (option)
    {
    case PRINTF:
        m_psz = RefMbs::Create(psz, argList);
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
        m_psz = RefMbs::Create(option, psz, argList);
        break;
    case UPPERCASE:
    case LOWERCASE:
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
        m_psz = RefMbs::Create(option, psz);
        break;
    default:
        throw Exception(L"StringAcp::ctor: Bad option.");
    }
    va_end(argList);
}


StringAcp::StringAcp(PCSTR psz1, PCSTR psz2)
    : m_psz(RefMbs::Create(psz1, psz2))
{
}


StringAcp::StringAcp(size_t cb, CHAR fill)
    : m_psz(RefMbs::Create(cb, fill))
{
}


StringAcp::StringAcp(PCWSTR psz, INT_PTR cch)
    : m_psz(RefMbs::Create(CP_ACP, psz, cch))
{
}


StringAcp::~StringAcp()
{
    Release(InterlockedExchangePCSTR(&m_psz, nullptr));
}


StringAcp& StringAcp::ZeroFill()
{
    if (Len)
    {
        RefMbs::Get(m_psz).ZeroFill();
    }
    return *this;
}


StringAcp& StringAcp::Lettercase(StringOptions option)
{
    if (Len)
    {
        Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(option, Ptr)));
    }
    return *this;
}


StringAcp& StringAcp::Trim(StringOptions option)
{
    if (Len)
    {
        Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(option, Ptr)));
    }
    return *this;
}


StringAcp& StringAcp::Assign(const StringAcp& other)
{
    Release(InterlockedExchangePCSTR(&m_psz, AddRef(other.m_psz)));
    return *this;
}


StringAcp& StringAcp::Assign(PCSTR psz, INT_PTR cb)
{
    Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(psz, cb)));
    return *this;
}


StringAcp& StringAcp::Append(const StringAcp& other)
{
    if (other.Len)
    {
        Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(Ptr, other.Ptr)));
    }
    return *this;
}


StringAcp& StringAcp::Append(PCSTR psz, INT_PTR cb)
{
    if (psz && cb)
    {
        Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(Ptr, psz, cb)));
    }
    return *this;
}


StringAcp& StringAcp::Format(PCSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(pszFormat, argList)));
    va_end(argList);
    return *this;
}


StringAcp& StringAcp::VaFormat(PCSTR pszFormat, va_list argList)
{
    Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(pszFormat, argList)));
    return *this;
}


StringAcp& StringAcp::AppendFormat(PCSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(Ptr, pszFormat, argList)));
    va_end(argList);
    return *this;
}


StringAcp& StringAcp::VaAppendFormat(PCSTR pszFormat, va_list argList)
{
    Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(Ptr, pszFormat, argList)));
    return *this;
}


StringAcp& StringAcp::TruncateHead(size_t cb)
{
    if (m_psz)
    {
        RefMbs& rs = RefMbs::Get(m_psz);
        if (rs.RefCnt > 1)
        {
            Release(InterlockedExchangePCSTR(&m_psz, cb < rs.Len ? RefMbs::Create(&m_psz[cb]) : nullptr));
        }
        else if (cb < rs.Len)
        {
            size_t newLen = rs.Len - cb;
            PSTR psz = const_cast<PSTR>(m_psz);
            memmove_s(&psz[0], newLen, &psz[cb], newLen);
            memset(&psz[newLen], '\0', cb);
            rs.SetLength(newLen);
        }
        else
        {
            rs.ZeroFill();
        }
    }
    return *this;
}


StringAcp& StringAcp::TruncateTail(size_t cb)
{
    if (m_psz)
    {
        RefMbs& rs = RefMbs::Get(m_psz);
        if (rs.RefCnt > 1)
        {
            Release(InterlockedExchangePCSTR(&m_psz, cb < rs.Len ? RefMbs::Create(m_psz, rs.Len - cb) : nullptr));
        }
        else if (cb < rs.Len)
        {
            size_t newLen = rs.Len - cb;
            PSTR psz = const_cast<PSTR>(m_psz);
            memset(&psz[newLen], '\0', cb);
            rs.SetLength(newLen);
        }
        else
        {
            rs.ZeroFill();
        }
    }
    return *this;
}


int StringAcp::IndexOf(CHAR c, INT_PTR fromIndex) const
{
    if (m_psz && static_cast<size_t>(fromIndex) < Len)
    {
        PCSTR pCur = reinterpret_cast<PCSTR>(memchr(m_psz + fromIndex, c, Len - fromIndex));
        return pCur ? static_cast<int>(pCur - m_psz) : -1;
    }
    return -1;
}


int StringAcp::IndexOf(const StringAcp& s, INT_PTR fromIndex) const
{
    if (m_psz && static_cast<size_t>(fromIndex) < Len)
    {
        PCSTR psz = s.Ptr;
        INT_PTR cb = s.Len;
        if (cb)
        {
            cb--;
            CHAR c = *psz++;
            PCSTR pCur = reinterpret_cast<PCSTR>(memchr(m_psz + fromIndex, c, Len - fromIndex));
            if (cb)
            {
                PCSTR pEnd = m_psz + Len;
                while (pCur)
                {
                    pCur++;
                    if (pEnd < pCur + cb)
                    {
                        break;
                    }
                    if (!Compare(psz, cb, pCur, cb))
                    {
                        return static_cast<int>((pCur - 1) - m_psz);
                    }
                    pCur = reinterpret_cast<PCSTR>(memchr(pCur, c, pEnd - pCur));
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


bool StringAcp::StartsWith(PCSTR psz, INT_PTR cb) const
{
    size_t cbActual = cb > 0 ? strnlen(psz, cb) : cb < 0 ? strlen(psz) : 0;
    if (cbActual == 0)
    {
        return true;
    }
    else if (cbActual <= Len)
    {
        return Compare(m_psz, cbActual, psz, cbActual) == 0;
    }
    else
    {
        return false;
    }
}


bool StringAcp::EndsWith(PCSTR psz, INT_PTR cb) const
{
    size_t cbActual = cb > 0 ? strnlen(psz, cb) : cb < 0 ? strlen(psz) : 0;
    if (cbActual == 0)
    {
        return true;
    }
    else if (cbActual <= Len)
    {
        return Compare(m_psz + Len - cbActual, cbActual, psz, cbActual) == 0;
    }
    else
    {
        return false;
    }
}


bool StringAcp::operator ==(const StringAcp& other) const
{
    return Compare(Ptr, other.Ptr) == 0;
}


bool StringAcp::operator !=(const StringAcp& other) const
{
    return Compare(Ptr, other.Ptr) != 0;
}


bool StringAcp::operator <(const StringAcp& other) const
{
    return Compare(Ptr, other.Ptr) < 0;
}


bool StringAcp::operator <=(const StringAcp& other) const
{
    return Compare(Ptr, other.Ptr) <= 0;
}


bool StringAcp::operator >(const StringAcp& other) const
{
    return Compare(Ptr, other.Ptr) > 0;
}


bool StringAcp::operator >=(const StringAcp& other) const
{
    return Compare(Ptr, other.Ptr) >= 0;
}


StringAcp StringAcp::operator +(const StringAcp& other) const
{
    return StringAcp(Ptr, other.Ptr);
}


PCSTR StringAcp::get_ptr() const
{
    return m_psz ? m_psz : "";
}


size_t StringAcp::get_len() const
{
    return m_psz ? RefMbs::Get(m_psz).Len : 0;
}


int StringAcp::Compare(PCSTR psz1, INT_PTR cb1, PCSTR psz2, INT_PTR cb2)
{
    switch (CompareStringA(LOCALE_INVARIANT, 0, psz1, static_cast<int>(cb1), psz2, static_cast<int>(cb2)))
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


StringAcp StringAcp::ToHex(const void* ptr, size_t len, StringOptions option)
{
    static const CHAR HexLU[2][16] = {
        { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' },
        { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' }
    };
    const CHAR* hex = HexLU[option == UPPERCASE ? 1 : 0];
    StringAcp sz(len * 2, '*');
    PCHAR pW = const_cast<PCHAR>(sz.m_psz);
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
