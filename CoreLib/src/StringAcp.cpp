#include "pch.h"
#include "hnrt/StringAcp.h"
#include "hnrt/RefMbs.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G - A C P   C L A S S
//
//////////////////////////////////////////////////////////////////////


StringAcp::StringAcp()
    : m_ptr(nullptr)
{
}


StringAcp::StringAcp(const StringAcp& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


StringAcp::StringAcp(RefMbs* ptr)
    : m_ptr(ptr)
{
}


StringAcp::StringAcp(PCSTR psz)
    : m_ptr(psz && *psz ? new RefMbs(psz) : nullptr)
{
}


StringAcp::StringAcp(PCSTR psz, size_t cb)
    : m_ptr(psz && cb ? new RefMbs(psz, cb) : nullptr)
{
}


StringAcp::StringAcp(PCSTR psz, va_list argList)
    : m_ptr(new RefMbs(psz, argList))
{
}


StringAcp::StringAcp(StringOptions option, PCSTR psz, ...)
    : m_ptr(nullptr)
{
    switch (option)
    {
    case PRINTF:
    {
        va_list argList;
        va_start(argList, psz);
        m_ptr = new RefMbs(psz, argList);
        va_end(argList);
        break;
    }
    case CONCAT:
    {
        size_t cb1 = strlen(psz);
        size_t required = cb1;
        PCSTR psz2;
        va_list argList1;
        va_start(argList1, psz);
        while ((psz2 = va_arg(argList1, PCSTR)))
        {
            required += strlen(psz2);
        }
        va_end(argList1);
        PSTR psz1 = Allocate<CHAR>(++required);
        memcpy_s(psz1, cb1 + 1, psz, cb1 + 1);
        va_list argList2;
        va_start(argList2, psz);
        while ((psz2 = va_arg(argList2, PCSTR)))
        {
            size_t cb2 = strlen(psz2);
            memcpy_s(psz1 + cb1, cb2 + 1, psz2, cb2 + 1);
            cb1 += cb2;
        }
        va_end(argList2);
        m_ptr = new RefMbs(IMMEDIATE, psz1);
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
        size_t cb1 = strlen(psz);
        size_t required = cb1;
        va_list argList1;
        va_start(argList1, psz);
        switch (option)
        {
        case CONCAT9:
            required += strlen(va_arg(argList1, PCSTR));
            //FALLTHROUGH
        case CONCAT8:
            required += strlen(va_arg(argList1, PCSTR));
            //FALLTHROUGH
        case CONCAT7:
            required += strlen(va_arg(argList1, PCSTR));
            //FALLTHROUGH
        case CONCAT6:
            required += strlen(va_arg(argList1, PCSTR));
            //FALLTHROUGH
        case CONCAT5:
            required += strlen(va_arg(argList1, PCSTR));
            //FALLTHROUGH
        case CONCAT4:
            required += strlen(va_arg(argList1, PCSTR));
            //FALLTHROUGH
        case CONCAT3:
            required += strlen(va_arg(argList1, PCSTR));
            //FALLTHROUGH
        case CONCAT2:
        default:
            required += strlen(va_arg(argList1, PCSTR));
            break;
        }
        va_end(argList1);
        PSTR psz1 = Allocate<CHAR>(++required);
        memcpy_s(psz1, cb1 + 1, psz, cb1 + 1);
        PCSTR psz2;
        size_t cb2;
        va_list argList2;
        va_start(argList2, psz);
        switch (option)
        {
        case CONCAT9:
            psz2 = va_arg(argList2, PCSTR);
            cb2 = strlen(psz2);
            memcpy_s(psz1 + cb1, cb2 + 1, psz2, cb2 + 1);
            cb1 += cb2;
            //FALLTHROUGH
        case CONCAT8:
            psz2 = va_arg(argList2, PCSTR);
            cb2 = strlen(psz2);
            memcpy_s(psz1 + cb1, cb2 + 1, psz2, cb2 + 1);
            cb1 += cb2;
            //FALLTHROUGH
        case CONCAT7:
            psz2 = va_arg(argList2, PCSTR);
            cb2 = strlen(psz2);
            memcpy_s(psz1 + cb1, cb2 + 1, psz2, cb2 + 1);
            cb1 += cb2;
            //FALLTHROUGH
        case CONCAT6:
            psz2 = va_arg(argList2, PCSTR);
            cb2 = strlen(psz2);
            memcpy_s(psz1 + cb1, cb2 + 1, psz2, cb2 + 1);
            cb1 += cb2;
            //FALLTHROUGH
        case CONCAT5:
            psz2 = va_arg(argList2, PCSTR);
            cb2 = strlen(psz2);
            memcpy_s(psz1 + cb1, cb2 + 1, psz2, cb2 + 1);
            cb1 += cb2;
            //FALLTHROUGH
        case CONCAT4:
            psz2 = va_arg(argList2, PCSTR);
            cb2 = strlen(psz2);
            memcpy_s(psz1 + cb1, cb2 + 1, psz2, cb2 + 1);
            cb1 += cb2;
            //FALLTHROUGH
        case CONCAT3:
            psz2 = va_arg(argList2, PCSTR);
            cb2 = strlen(psz2);
            memcpy_s(psz1 + cb1, cb2 + 1, psz2, cb2 + 1);
            cb1 += cb2;
            //FALLTHROUGH
        case CONCAT2:
        default:
            psz2 = va_arg(argList2, PCSTR);
            cb2 = strlen(psz2);
            memcpy_s(psz1 + cb1, cb2 + 1, psz2, cb2 + 1);
            cb1 += cb2;
            break;
        }
        va_end(argList2);
        m_ptr = new RefMbs(IMMEDIATE, psz1);
        break;
    }
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
    {
        const CHAR* pCur = psz;
        const CHAR* pStart;
        CHAR c = *pCur++;
        if (option == TRIM_TAIL)
        {
            pStart = pCur - 1;
        }
        else
        {
            while (isspace(c))
            {
                c = *pCur++;
            }
            if (c)
            {
                pStart = pCur - 1;
                if (option == TRIM_HEAD)
                {
                    m_ptr = new RefMbs(pStart);
                    break;
                }
                c = *pCur++;
            }
            else
            {
                m_ptr = new RefMbs("");
                break;
            }
        }
        while (true)
        {
            if (isspace(c))
            {
                const CHAR* pEnd = pCur - 1;
                c = *pCur++;
                while (isspace(c))
                {
                    c = *pCur++;
                }
                if (c)
                {
                    c = *pCur++;
                }
                else
                {
                    m_ptr = new RefMbs(pStart, pEnd - pStart);
                    break;
                }
            }
            else if (c)
            {
                c = *pCur++;
            }
            else
            {
                m_ptr = new RefMbs(pStart);
                break;
            }
        }
        break;
    }
    case UPPERCASE:
    case LOWERCASE:
    case IMMEDIATE:
    case STATIC:
        m_ptr = new RefMbs(option, psz);
        break;
    default:
        throw Exception(L"StringAcp::ctor: Bad option.");
    }
}


StringAcp::StringAcp(PCWSTR psz)
    : m_ptr(psz && *psz ? new RefMbs(CP_ACP, psz) : nullptr)
{
}


StringAcp::StringAcp(PCWSTR psz, size_t cch)
    : m_ptr(psz && cch ? new RefMbs(CP_ACP, psz, cch) : nullptr)
{
}


StringAcp::~StringAcp()
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


StringAcp& StringAcp::operator =(const StringAcp& other)
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


StringAcp& StringAcp::operator +=(const StringAcp& other)
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(Ptr, other.Ptr));
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


bool StringAcp::operator ==(const StringAcp& other) const
{
    return Compare(*this, other) == 0;
}


bool StringAcp::operator !=(const StringAcp& other) const
{
    return Compare(*this, other) != 0;
}


bool StringAcp::operator <(const StringAcp& other) const
{
    return Compare(*this, other) < 0;
}


bool StringAcp::operator <=(const StringAcp& other) const
{
    return Compare(*this, other) <= 0;
}


bool StringAcp::operator >(const StringAcp& other) const
{
    return Compare(*this, other) > 0;
}


bool StringAcp::operator >=(const StringAcp& other) const
{
    return Compare(*this, other) >= 0;
}


StringAcp StringAcp::operator +(const StringAcp& other) const
{
    return StringAcp(new RefMbs(Ptr, other.Ptr));
}


PCSTR StringAcp::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : "";
}


size_t StringAcp::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
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


int StringAcp::CaseCompare(PCSTR psz1, INT_PTR cb1, PCSTR psz2, INT_PTR cb2)
{
    switch (CompareStringA(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, static_cast<int>(cb1), psz2, static_cast<int>(cb2)))
    {
    case CSTR_EQUAL:
        return 0;
    case CSTR_LESS_THAN:
        return -1;
    case CSTR_GREATER_THAN:
        return 1;
    default:
        throw Exception(L"StringAcp::CaseCompare failed.");
    }
}
