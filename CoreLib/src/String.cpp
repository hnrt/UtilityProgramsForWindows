#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/RefStr.h"
#include "hnrt/Interlocked.h"
#include "hnrt/StringStore.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G   C L A S S
//
//////////////////////////////////////////////////////////////////////


const String String::Empty = String();


String::String()
    : m_ptr(nullptr)
{
}


String::String(const String& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


String::String(RefStr* ptr)
    : m_ptr(ptr)
{
}


String::String(PCWSTR psz)
    : m_ptr(psz ? new RefStr(psz) : nullptr)
{
}


String::String(PCWSTR psz, size_t cch)
    : m_ptr(psz ? new RefStr(psz, cch) : nullptr)
{
}


String::String(PCWSTR pszFormat, va_list argList)
    : m_ptr(new RefStr(pszFormat, argList))
{
}


String::String(StringOptions option, PCWSTR psz, ...)
    : m_ptr(nullptr)
{
    switch (option)
    {
    case PRINTF:
    {
        va_list argList;
        va_start(argList, psz);
        m_ptr = new RefStr(psz, argList);
        va_end(argList);
        break;
    }
    case CONCAT:
    {
        StringBuffer buf(MAX_PATH);
        va_list argList;
        va_start(argList, psz);
        while (psz)
        {
            buf += psz;
            psz = va_arg(argList, PCWSTR);
        }
        va_end(argList);
        m_ptr = new RefStr(buf);
        break;
    }
    case CONCAT2:
    case CONCAT3:
    case CONCAT4:
    case CONCAT5:
    case CONCAT6:
    case CONCAT7:
    case CONCAT8:
    case CONCAT9:
    {
        StringBuffer buf(MAX_PATH);
        va_list argList;
        va_start(argList, psz);
        buf += psz;
        switch (option)
        {
        case CONCAT9:
            buf += va_arg(argList, PCWSTR);
            //FALLTHROUGH
        case CONCAT8:
            buf += va_arg(argList, PCWSTR);
            //FALLTHROUGH
        case CONCAT7:
            buf += va_arg(argList, PCWSTR);
            //FALLTHROUGH
        case CONCAT6:
            buf += va_arg(argList, PCWSTR);
            //FALLTHROUGH
        case CONCAT5:
            buf += va_arg(argList, PCWSTR);
            //FALLTHROUGH
        case CONCAT4:
            buf += va_arg(argList, PCWSTR);
            //FALLTHROUGH
        case CONCAT3:
            buf += va_arg(argList, PCWSTR);
            //FALLTHROUGH
        case CONCAT2:
        default:
            buf += va_arg(argList, PCWSTR);
            break;
        }
        va_end(argList);
        m_ptr = new RefStr(buf);
        break;
    }
    case UPPERCASE:
    {
        StringBuffer buf(wcslen(psz) + 1);
        _wcsupr_s(buf.Append(psz), buf.Cap);
        m_ptr = new RefStr(buf);
        break;
    }
    case LOWERCASE:
    {
        StringBuffer buf(wcslen(psz) + 1);
        _wcslwr_s(buf.Append(psz), buf.Cap);
        m_ptr = new RefStr(buf);
        break;
    }
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
    {
        const WCHAR* pCur = psz;
        const WCHAR* pStart;
        WCHAR c = *pCur++;
        if (option == TRIM_TAIL)
        {
            pStart = pCur - 1;
        }
        else
        {
            while (iswspace(c))
            {
                c = *pCur++;
            }
            if (c)
            {
                pStart = pCur - 1;
                if (option == TRIM_HEAD)
                {
                    m_ptr = new RefStr(pStart);
                    break;
                }
                c = *pCur++;
            }
            else
            {
                m_ptr = new RefStr(L"");
                break;
            }
        }
        while (true)
        {
            if (iswspace(c))
            {
                const WCHAR* pEnd = pCur - 1;
                c = *pCur++;
                while (iswspace(c))
                {
                    c = *pCur++;
                }
                if (c)
                {
                    c = *pCur++;
                }
                else
                {
                    m_ptr = new RefStr(pStart, pEnd - pStart);
                    break;
                }
            }
            else if (c)
            {
                c = *pCur++;
            }
            else
            {
                m_ptr = new RefStr(pStart);
                break;
            }
        }
        break;
    }
    default:
        throw Exception(L"String::ctor: Bad option.");
    }
}


String::String(PCWSTR psz1, PCWSTR psz2)
    : m_ptr(new RefStr(psz1, psz2))
{
}


String::String(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3)
    : m_ptr(new RefStr(psz1, psz2, psz3))
{
}


String::String(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4)
    : m_ptr(new RefStr(psz1, psz2, psz3, psz4))
{
}


String::String(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5)
    : m_ptr(new RefStr(psz1, psz2, psz3, psz4, psz5))
{
}


String::String(PCSTR psz)
    : m_ptr(psz ? new RefStr(psz) : nullptr)
{
}


String::String(PCSTR psz, size_t cb)
    : m_ptr(psz ? new RefStr(psz, cb) : nullptr)
{
}


String::String(UINT cp, PCSTR psz)
    : m_ptr(psz ? new RefStr(cp, psz) : nullptr)
{
}


String::String(UINT cp, PCSTR psz, size_t cb)
    : m_ptr(psz ? new RefStr(cp, psz, cb) : nullptr)
{
}


String::String(StringBuffer& buf)
    : m_ptr(new RefStr(buf))
{
}


String::~String()
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


String& String::ZeroFill()
{
    if (m_ptr)
    {
        m_ptr->ZeroFill();
    }
    return *this;
}


String& String::Assign(const String& other)
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, other.m_ptr);
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


String& String::Assign(PCWSTR psz)
{
    if (psz)
    {
        if (m_ptr && m_ptr->RefCnt == 1)
        {
            m_ptr->Assign(psz);
        }
        else
        {
            RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(psz));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    else
    {
        RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, nullptr);
        if (ptr)
        {
            ptr->Release();
        }
    }
    return *this;
}


String& String::Assign(StringBuffer& buf)
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, buf ? new RefStr(buf) : nullptr);
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


String& String::Append(const String& other)
{
    if (other.Len > 0)
    {
        RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(Ptr, other.Ptr));
        if (ptr)
        {
            ptr->Release();
        }
    }
    return *this;
}


String& String::Append(PCWSTR psz)
{
    if (psz && *psz)
    {
        if (m_ptr && m_ptr->RefCnt == 1)
        {
            m_ptr->Append(psz);
        }
        else
        {
            RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(Ptr, psz));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


bool String::operator ==(const String& other) const
{
    return Compare(*this, other) == 0;
}


bool String::operator !=(const String& other) const
{
    return Compare(*this, other) != 0;
}


bool String::operator <(const String& other) const
{
    return Compare(*this, other) < 0;
}


bool String::operator <=(const String& other) const
{
    return Compare(*this, other) <= 0;
}


bool String::operator >(const String& other) const
{
    return Compare(*this, other) > 0;
}


bool String::operator >=(const String& other) const
{
    return Compare(*this, other) >= 0;
}


String String::operator +(const String& other) const
{
    return String(Ptr, other.Ptr);
}


PCWSTR String::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : L"";
}


size_t String::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
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


PCWSTR String::Copy(PCWSTR psz, size_t cch)
{
    if (!psz)
    {
        return L"";
    }
    else if (cch == static_cast<size_t>(-1))
    {
        return StringStore::Get(psz);
    }
    else if (cch < MAX_PATH)
    {
        WCHAR buf[MAX_PATH];
        wcsncpy_s(buf, psz, cch);
        return StringStore::Get(buf);
    }
    else
    {
        StringBuffer buf(cch + 1);
        wcsncpy_s(buf, buf.Cap, psz, cch);
        return StringStore::Set(buf);
    }
}
