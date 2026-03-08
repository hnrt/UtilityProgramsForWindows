#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


String::String()
    : m_psz(nullptr)
{
}


SIZE_T String::get_Length() const
{
    return m_psz ? RefStr::Get(m_psz).Length : 0;
}


void String::set_Length(SIZE_T length)
{
    if (m_psz)
    {
        if (length)
        {
            SIZE_T len0 = RefStr::Get(m_psz).Length;
            if (len0 < length)
            {
                PWSTR psz0 = Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(length));
                MemCpy(m_psz, psz0, len0);
                MemSet(m_psz + len0, 0, length + 1 - len0);
                StringRelease(psz0);
            }
            else if (length < len0)
            {
                PWSTR psz0 = Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(length));
                MemCpy(m_psz, psz0, length);
                m_psz[length] = L'\0';
                StringRelease(psz0);
            }
        }
        else
        {
            StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, nullptr));
        }
    }
    else if (length)
    {
        m_psz = RefStr::Create(length);
        MemSet(m_psz, 0, length + 1);
    }
}
