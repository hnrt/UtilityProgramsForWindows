#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/RefStr.h"
#include "hnrt/RefMbs.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Heap.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/StringStore.h"
#include "hnrt/Exception.h"
#include "hnrt/Buffer.h"
#include <exception>


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


String::String(RefStr* ptr)
    : m_ptr(ptr)
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


String::~String()
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


String& String::operator =(const String& other)
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


String& String::operator +=(const String& other)
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(Ptr, other.Ptr));
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


String::operator PCWSTR() const
{
    return Ptr;
}


String::operator bool() const
{
    return m_ptr != nullptr;
}


PCWSTR String::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : L"";
}


size_t String::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
}


int String::Compare(PCWSTR psz1, PCWSTR psz2, size_t cch2)
{
    switch (CompareStringW(LOCALE_INVARIANT, 0, psz1, -1, psz2, static_cast<int>(cch2)))
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


int String::CaseCompare(PCWSTR psz1, PCWSTR psz2, size_t cch2)
{
    switch (CompareStringW(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, -1, psz2, static_cast<int>(cch2)))
    {
    case CSTR_EQUAL:
        return 0;
    case CSTR_LESS_THAN:
        return -1;
    case CSTR_GREATER_THAN:
        return 1;
    default:
        throw Exception(L"String::CaseCompare failed.");
    }
}


int String::Compare(PCWSTR psz1, size_t cch1, PCWSTR psz2, size_t cch2)
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


int String::CaseCompare(PCWSTR psz1, size_t cch1, PCWSTR psz2, size_t cch2)
{
    switch (CompareStringW(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
    {
    case CSTR_EQUAL:
        return 0;
    case CSTR_LESS_THAN:
        return -1;
    case CSTR_GREATER_THAN:
        return 1;
    default:
        throw Exception(L"String::CaseCompare failed.");
    }
}


int String::Compare(PCSTR psz1, PCSTR psz2, size_t cch2)
{
    switch (CompareStringA(LOCALE_INVARIANT, 0, psz1, -1, psz2, static_cast<int>(cch2)))
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


int String::CaseCompare(PCSTR psz1, PCSTR psz2, size_t cch2)
{
    switch (CompareStringA(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, -1, psz2, static_cast<int>(cch2)))
    {
    case CSTR_EQUAL:
        return 0;
    case CSTR_LESS_THAN:
        return -1;
    case CSTR_GREATER_THAN:
        return 1;
    default:
        throw Exception(L"String::CaseCompare failed.");
    }
}


int String::Compare(PCSTR psz1, size_t cch1, PCSTR psz2, size_t cch2)
{
    switch (CompareStringA(LOCALE_INVARIANT, 0, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
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


int String::CaseCompare(PCSTR psz1, size_t cch1, PCSTR psz2, size_t cch2)
{
    switch (CompareStringA(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
    {
    case CSTR_EQUAL:
        return 0;
    case CSTR_LESS_THAN:
        return -1;
    case CSTR_GREATER_THAN:
        return 1;
    default:
        throw Exception(L"String::CaseCompare failed.");
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


//////////////////////////////////////////////////////////////////////
//
//   C A S E S T R I N G   C L A S S
//
//////////////////////////////////////////////////////////////////////


CaseString::CaseString()
    : m_ptr(nullptr)
{
}


CaseString::CaseString(PCWSTR psz)
    : m_ptr(psz ? new RefStr(Clone(psz)) : nullptr)
{
}


CaseString::CaseString(PCWSTR psz, size_t cb)
    : m_ptr(psz ? new RefStr(Clone(psz, cb)) : nullptr)
{
}


CaseString::CaseString(RefStr* ptr)
    : m_ptr(ptr)
{
}


CaseString::CaseString(const String& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


CaseString::CaseString(const CaseString& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


CaseString::~CaseString()
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


String CaseString::ToString() const
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
    return String(m_ptr);
}


CaseString& CaseString::operator =(const String& other)
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


CaseString& CaseString::operator =(const CaseString& other)
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


bool CaseString::operator ==(const CaseString& other) const
{
    return String::CaseCompare(*this, other) == 0;
}


bool CaseString::operator !=(const CaseString& other) const
{
    return String::CaseCompare(*this, other) != 0;
}


bool CaseString::operator <(const CaseString& other) const
{
    return String::CaseCompare(*this, other) < 0;
}


bool CaseString::operator <=(const CaseString& other) const
{
    return String::CaseCompare(*this, other) <= 0;
}


bool CaseString::operator >(const CaseString& other) const
{
    return String::CaseCompare(*this, other) > 0;
}


bool CaseString::operator >=(const CaseString& other) const
{
    return String::CaseCompare(*this, other) >= 0;
}


CaseString CaseString::operator +(const CaseString& other) const
{
    return CaseString(new RefStr(Ptr, other.Ptr));
}


CaseString& CaseString::operator +=(const CaseString& other)
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(Ptr, other.Ptr));
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


CaseString::operator PCWSTR() const
{
    return Ptr;
}


CaseString::operator bool() const
{
    return m_ptr != nullptr;
}


PCWSTR CaseString::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : L"";
}


size_t CaseString::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
}


//////////////////////////////////////////////////////////////////////
//
//   A C P S T R I N G   C L A S S
//
//////////////////////////////////////////////////////////////////////


AcpString::AcpString()
    : m_ptr(nullptr)
{
}


AcpString::AcpString(PCSTR psz)
    : m_ptr(psz ? new RefMbs(Clone(psz)) : nullptr)
{
}


AcpString::AcpString(PCSTR psz, size_t cb)
    : m_ptr(psz ? new RefMbs(Clone(psz, cb)) : nullptr)
{
}


AcpString::AcpString(PCWSTR psz)
    : m_ptr(psz ? new RefMbs(ToAcp(psz)) : nullptr)
{
}


AcpString::AcpString(PCWSTR psz, size_t cb)
    : m_ptr(psz ? new RefMbs(ToAcp(psz, cb)) : nullptr)
{
}


AcpString::AcpString(RefMbs* ptr)
    : m_ptr(ptr)
{
}


AcpString::AcpString(const AcpString& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


AcpString::~AcpString()
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


AcpString& AcpString::operator =(const AcpString& other)
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, other.m_ptr);
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


bool AcpString::operator ==(const AcpString& other) const
{
    return String::Compare(*this, other) == 0;
}


bool AcpString::operator !=(const AcpString& other) const
{
    return String::Compare(*this, other) != 0;
}


bool AcpString::operator <(const AcpString& other) const
{
    return String::Compare(*this, other) < 0;
}


bool AcpString::operator <=(const AcpString& other) const
{
    return String::Compare(*this, other) <= 0;
}


bool AcpString::operator >(const AcpString& other) const
{
    return String::Compare(*this, other) > 0;
}


bool AcpString::operator >=(const AcpString& other) const
{
    return String::Compare(*this, other) >= 0;
}


AcpString AcpString::operator +(const AcpString& other) const
{
    return AcpString(new RefMbs(Ptr, other.Ptr));
}


AcpString& AcpString::operator +=(const AcpString& other)
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(Ptr, other.Ptr));
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


AcpString::operator PCSTR() const
{
    return get_ptr();
}


AcpString::operator bool() const
{
    return m_ptr != nullptr;
}


String AcpString::ToString() const
{
    return String(*this);
}


PCSTR AcpString::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : "";
}


size_t AcpString::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
}


//////////////////////////////////////////////////////////////////////
//
//   U T F 8   C L A S S
//
//////////////////////////////////////////////////////////////////////


UTF8::UTF8()
    : m_ptr(nullptr)
{
}


UTF8::UTF8(PCSTR psz)
    : m_ptr(psz ? new RefMbs(Clone(psz)) : nullptr)
{
}


UTF8::UTF8(PCSTR psz, size_t cb)
    : m_ptr(psz ? new RefMbs(Clone(psz, cb)) : nullptr)
{
}


UTF8::UTF8(PCWSTR psz)
    : m_ptr(psz ? new RefMbs(ToAcp(CP_UTF8, psz)) : nullptr)
{
}


UTF8::UTF8(PCWSTR psz, size_t cb)
    : m_ptr(psz ? new RefMbs(ToAcp(CP_UTF8, psz, cb)) : nullptr)
{
}


UTF8::UTF8(RefMbs* ptr)
    : m_ptr(ptr)
{
}


UTF8::UTF8(const UTF8& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


UTF8::~UTF8()
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


UTF8& UTF8::operator =(const UTF8& other)
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, other.m_ptr);
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


bool UTF8::operator ==(const UTF8& other) const
{
    return strcmp(*this, other) == 0;
}


bool UTF8::operator !=(const UTF8& other) const
{
    return strcmp(*this, other) != 0;
}


bool UTF8::operator <(const UTF8& other) const
{
    return strcmp(*this, other) < 0;
}


bool UTF8::operator <=(const UTF8& other) const
{
    return strcmp(*this, other) <= 0;
}


bool UTF8::operator >(const UTF8& other) const
{
    return strcmp(*this, other) > 0;
}


bool UTF8::operator >=(const UTF8& other) const
{
    return strcmp(*this, other) >= 0;
}


UTF8 UTF8::operator +(const UTF8& other) const
{
    return UTF8(new RefMbs(Ptr, other.Ptr));
}


UTF8& UTF8::operator +=(const UTF8& other)
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(Ptr, other.Ptr));
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


UTF8::operator PCSTR() const
{
    return Ptr;
}


UTF8::operator bool() const
{
    return m_ptr != nullptr;
}


String UTF8::ToString() const
{
    return String(CP_UTF8, *this);
}


PCSTR UTF8::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : "";
}


size_t UTF8::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
}
