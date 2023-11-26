#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/Heap.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


namespace hnrt
{
    class RefMbs
        : public RefObj
    {
    public:

        RefMbs(const RefMbs&) = delete;
        RefMbs(PCSTR, INT_PTR = -1);
        RefMbs(PCSTR, va_list);
        RefMbs(PCSTR, PCSTR);
        RefMbs(PCSTR, PCSTR, PCSTR);
        RefMbs(PCSTR, PCSTR, PCSTR, PCSTR);
        RefMbs(PCSTR, PCSTR, PCSTR, PCSTR, PCSTR);
        RefMbs(UINT, PCWSTR, INT_PTR = -1);
        RefMbs(StringOptions, ...);
        virtual ~RefMbs();
        void operator =(const RefMbs&) = delete;
        PSTR Exchange(PSTR psz);
        PSTR Detach();
        void ZeroFill();
        RefMbs& Assign(PCSTR, INT_PTR = -1);
        RefMbs& Append(PCSTR, INT_PTR = -1);
        PCSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        PSTR m_psz;
        size_t m_len;
        size_t m_cap;
    };

    inline RefMbs::RefMbs(PCSTR psz, INT_PTR cb)
        : RefObj()
        , m_psz(nullptr)
        , m_len(0)
        , m_cap(0)
    {
        m_len = cb > 0 ? strnlen(psz, cb) : cb < 0 ? strlen(psz) : 0;
        m_cap = m_len + 1;
        m_psz = Allocate<CHAR>(m_cap);
        memcpy_s(m_psz, m_len, psz, m_len);
        m_psz[m_len] = '\0';
    }

    inline RefMbs::RefMbs(PCSTR pszFormat, va_list argList)
        : RefObj()
        , m_psz(VaFormat(pszFormat, argList))
        , m_len(strlen(m_psz))
        , m_cap(m_len + 1)
    {
    }

    inline RefMbs::RefMbs(PCSTR psz1, PCSTR psz2)
        : RefObj()
        , m_psz(Concat(psz1, psz2))
        , m_len(strlen(m_psz))
        , m_cap(m_len + 1)
    {
    }

    inline RefMbs::RefMbs(PCSTR psz1, PCSTR psz2, PCSTR psz3)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3))
        , m_len(strlen(m_psz))
        , m_cap(m_len + 1)
    {
    }

    inline RefMbs::RefMbs(PCSTR psz1, PCSTR psz2, PCSTR psz3, PCSTR psz4)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3, psz4))
        , m_len(strlen(m_psz))
        , m_cap(m_len + 1)
    {
    }

    inline RefMbs::RefMbs(PCSTR psz1, PCSTR psz2, PCSTR psz3, PCSTR psz4, PCSTR psz5)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3, psz4, psz5))
        , m_len(strlen(m_psz))
        , m_cap(m_len + 1)
    {
    }

    inline RefMbs::RefMbs(UINT cp, PCWSTR psz, INT_PTR cch)
        : RefObj()
        , m_psz(ToAcp(cp, psz, cch))
        , m_len(strlen(m_psz))
        , m_cap(cch >= 0 ? cch + 1 : m_len + 1)
    {
    }

    inline RefMbs::RefMbs(StringOptions option, ...)
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
            m_psz = Clone(va_arg(argList, PCSTR));
            m_len = strlen(m_psz);
            m_cap = m_len + 1;
            StringUtils::Uppercase(m_psz, m_cap);
            break;
        case LOWERCASE:
            m_psz = Clone(va_arg(argList, PCSTR));
            m_len = strlen(m_psz);
            m_cap = m_len + 1;
            StringUtils::Lowercase(m_psz, m_cap);
            break;
        case TRIM:
        case TRIM_HEAD:
        case TRIM_TAIL:
        {
            PCSTR psz = va_arg(argList, PCSTR);
            int start = 0;
            int end = 0;
            StringUtils::TrimScan(option, psz, start, end);
            m_len = static_cast<size_t>(end) - static_cast<size_t>(start);
            m_cap = m_len + 1;
            m_psz = Allocate<CHAR>(m_cap);
            memcpy_s(m_psz, m_len, psz + start, m_len);
            m_psz[m_len] = '\0';
            break;
        }
        case IMMEDIATE_TEXT:
            m_psz = va_arg(argList, PSTR);
            m_len = strlen(m_psz);
            m_cap = m_len + 1;
            break;
        case STATIC_TEXT:
            m_psz = va_arg(argList, PSTR);
            m_len = strlen(m_psz);
            m_cap = 0; // indicates not to free m_psz
            break;
        default:
            throw Exception(L"RefMbs::ctor: Bad option.");
        }
        va_end(argList);
    }

    inline RefMbs::~RefMbs()
    {
        if (m_cap)
        {
            Deallocate(m_psz);
        }
    }

    inline PSTR RefMbs::Exchange(PSTR psz)
    {
        if (m_psz && !m_cap)
        {
            m_psz = Clone(m_psz);
        }
        psz = Interlocked<PSTR>::ExchangePointer(&m_psz, psz);
        if (m_psz)
        {
            m_len = strlen(m_psz);
            m_cap = m_len + 1;
        }
        else
        {
            m_len = 0;
            m_cap = 0;
        }
        return psz;
    }

    inline PSTR RefMbs::Detach()
    {
        return Exchange(nullptr);
    }

    inline void RefMbs::ZeroFill()
    {
        if (m_psz)
        {
            if (m_cap)
            {
                memset(m_psz, '\0', m_cap);
                m_len = 0;
            }
        }
    }

    inline RefMbs& RefMbs::Assign(PCSTR psz, INT_PTR cb)
    {
        size_t len;
        if (cb > 0)
        {
            len = strnlen(psz, cb);
        }
        else if (cb < 0)
        {
            cb = len = strlen(psz);
        }
        else
        {
            len = 0;
        }
        size_t required = cb + 1;
        if (m_cap < required)
        {
            m_psz = m_cap ? Allocate(m_psz, required) : Allocate<CHAR>(required);
            m_cap = required;
        }
        memcpy_s(m_psz, len, psz, len);
        m_len = len;
        m_psz[m_len] = '\0';
        return *this;
    }

    inline RefMbs& RefMbs::Append(PCSTR psz, INT_PTR cb)
    {
        size_t len;
        if (cb > 0)
        {
            len = strnlen(psz, cb);
        }
        else if (cb < 0)
        {
            cb = len = strlen(psz);
        }
        else
        {
            return *this;
        }
        size_t required = m_len + cb + 1;
        if (m_cap < required)
        {
            if (m_cap)
            {
                m_psz = Allocate(m_psz, required);
            }
            else if (m_psz)
            {
                // DO NOT FREE m_psz
                PSTR psz2 = Allocate<CHAR>(required);
                memcpy_s(psz2, m_len, m_psz, m_len);
                m_psz = psz2;
            }
            else
            {
                m_psz = Allocate<CHAR>(required);
            }
            m_cap = required;
        }
        memcpy_s(m_psz + m_len, cb, psz, len);
        m_len += len;
        m_psz[m_len] = '\0';
        return *this;
    }

    inline PCSTR RefMbs::get_ptr() const
    {
        return m_psz;
    }

    inline size_t RefMbs::get_len() const
    {
        return m_len;
    }
}
