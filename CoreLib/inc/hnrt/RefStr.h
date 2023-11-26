#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/Heap.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


namespace hnrt
{
    class RefStr
        : public RefObj
    {
    public:

        RefStr(const RefStr&) = delete;
        RefStr(PCWSTR, INT_PTR = -1);
        RefStr(PCWSTR, va_list);
        RefStr(PCWSTR, PCWSTR);
        RefStr(PCWSTR, PCWSTR, PCWSTR);
        RefStr(PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        RefStr(PCWSTR, PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        RefStr(UINT, PCSTR, INT_PTR = -1);
        RefStr(StringOptions, ...);
        virtual ~RefStr();
        void operator =(const RefStr&) = delete;
        PWSTR Exchange(PWSTR);
        PWSTR Detach();
        void ZeroFill();
        RefStr& Assign(PCWSTR, INT_PTR = -1);
        RefStr& Append(PCWSTR, INT_PTR = -1);
        PCWSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCWSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        PWSTR m_psz;
        size_t m_len;
        size_t m_cap;
    };

    inline RefStr::RefStr(PCWSTR psz, INT_PTR cch)
        : RefObj()
        , m_psz(nullptr)
        , m_len(0)
        , m_cap(0)
    {
        m_len = cch > 0 ? wcsnlen(psz, cch) : cch < 0 ? wcslen(psz) : 0;
        m_cap = m_len + 1;
        m_psz = Allocate<WCHAR>(m_cap);
        wmemcpy_s(m_psz, m_len, psz, m_len);
        m_psz[m_len] = L'\0';
    }

    inline RefStr::RefStr(PCWSTR pszFormat, va_list argList)
        : RefObj()
        , m_psz(VaFormat(pszFormat, argList))
        , m_len(wcslen(m_psz))
        , m_cap(m_len + 1)
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2)
        : RefObj()
        , m_psz(Concat(psz1, psz2))
        , m_len(wcslen(m_psz))
        , m_cap(m_len + 1)
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3))
        , m_len(wcslen(m_psz))
        , m_cap(m_len + 1)
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3, psz4))
        , m_len(wcslen(m_psz))
        , m_cap(m_len + 1)
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3, psz4, psz5))
        , m_len(wcslen(m_psz))
        , m_cap(m_len + 1)
    {
    }

    inline RefStr::RefStr(UINT cp, PCSTR psz, INT_PTR cb)
        : RefObj()
        , m_psz(ToUcs(cp, psz, cb))
        , m_len(wcslen(m_psz))
        , m_cap(cb >= 0 ? cb + 1 : m_len + 1)
    {
    }

    inline RefStr::RefStr(StringOptions option, ...)
        : RefObj()
        , m_psz(nullptr)
        , m_len(0)
        , m_cap(0)
    {
        va_list argList;
        va_start(argList, option);
        switch (option)
        {
        case UPPERCASE:
            m_psz = Clone(va_arg(argList, PCWSTR));
            m_len = wcslen(m_psz);
            m_cap = m_len + 1;
            StringUtils::Uppercase(m_psz, m_cap);
            break;
        case LOWERCASE:
            m_psz = Clone(va_arg(argList, PCWSTR));
            m_len = wcslen(m_psz);
            m_cap = m_len + 1;
            StringUtils::Lowercase(m_psz, m_cap);
            break;
        case TRIM:
        case TRIM_HEAD:
        case TRIM_TAIL:
        {
            PCWSTR psz = va_arg(argList, PCWSTR);
            int start = 0;
            int end = 0;
            StringUtils::TrimScan(option, psz, start, end);
            m_len = static_cast<size_t>(end) - static_cast<size_t>(start);
            m_cap = m_len + 1;
            m_psz = Allocate<WCHAR>(m_cap);
            wmemcpy_s(m_psz, m_len, psz + start, m_len);
            m_psz[m_len] = L'\0';
            break;
        }
        case IMMEDIATE_TEXT:
            m_psz = va_arg(argList, PWSTR);
            m_len = wcslen(m_psz);
            m_cap = m_len + 1;
            break;
        case STATIC_TEXT:
            m_psz = va_arg(argList, PWSTR);
            m_len = wcslen(m_psz);
            m_cap = 0; // indicates not to free m_psz
            break;
        default:
            throw Exception(L"RefStr::ctor: Bad option.");
        }
        va_end(argList);
    }

    inline RefStr::~RefStr()
    {
        if (m_cap)
        {
            Deallocate(m_psz);
        }
    }

    inline PWSTR RefStr::Exchange(PWSTR psz)
    {
        if (m_psz && !m_cap)
        {
            m_psz = Clone(m_psz);
        }
        psz = Interlocked<PWSTR>::ExchangePointer(&m_psz, psz);
        if (m_psz)
        {
            m_len = wcslen(m_psz);
            m_cap = m_len + 1;
        }
        else
        {
            m_len = 0;
            m_cap = 0;
        }
        return psz;
    }

    inline PWSTR RefStr::Detach()
    {
        return Exchange(nullptr);
    }

    inline void RefStr::ZeroFill()
    {
        if (m_psz)
        {
            if (m_cap)
            {
                wmemset(m_psz, L'\0', m_cap);
                m_len = 0;
            }
        }
    }

    inline RefStr& RefStr::Assign(PCWSTR psz, INT_PTR cch)
    {
        size_t len;
        if (cch > 0)
        {
            len = wcsnlen(psz, cch);
        }
        else if (cch < 0)
        {
            cch = len = wcslen(psz);
        }
        else
        {
            len = 0;
        }
        size_t required = cch + 1;
        if (m_cap < required)
        {
            m_psz = m_cap ? Allocate(m_psz, required) : Allocate<WCHAR>(required);
            m_cap = required;
        }
        wmemcpy_s(m_psz, len, psz, len);
        m_len = len;
        m_psz[m_len] = L'\0';
        return *this;
    }

    inline RefStr& RefStr::Append(PCWSTR psz, INT_PTR cch)
    {
        size_t len;
        if (cch > 0)
        {
            len = wcsnlen(psz, cch);
        }
        else if (cch < 0)
        {
            cch = len = wcslen(psz);
        }
        else
        {
            return *this;
        }
        size_t required = m_len + cch + 1;
        if (m_cap < required)
        {
            if (m_cap)
            {
                m_psz = Allocate(m_psz, required);
            }
            else if (m_psz)
            {
                // DO NOT FREE m_psz
                PWSTR psz2 = Allocate<WCHAR>(required);
                wmemcpy_s(psz2, m_len, m_psz, m_len);
                m_psz = psz2;
            }
            else
            {
                m_psz = Allocate<WCHAR>(required);
            }
            m_cap = required;
        }
        wmemcpy_s(m_psz + m_len, cch, psz, len);
        m_len += len;
        m_psz[m_len] = L'\0';
        return *this;
    }

    inline PCWSTR RefStr::get_ptr() const
    {
        return m_psz;
    }

    inline size_t RefStr::get_len() const
    {
        return m_len;
    }
}
