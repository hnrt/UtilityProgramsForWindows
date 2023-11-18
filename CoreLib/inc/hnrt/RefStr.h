#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/Heap.h"
#include "hnrt/StringBuffer.h"


namespace hnrt
{
    class RefStr
        : public RefObj
    {
    public:

        RefStr(const RefStr&) = delete;
        RefStr(PCWSTR);
        RefStr(PCWSTR, size_t);
        RefStr(PCWSTR, va_list);
        RefStr(PCWSTR, PCWSTR);
        RefStr(PCWSTR, PCWSTR, PCWSTR);
        RefStr(PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        RefStr(PCWSTR, PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        RefStr(PCSTR);
        RefStr(PCSTR, size_t);
        RefStr(UINT, PCSTR);
        RefStr(UINT, PCSTR, size_t);
        RefStr(StringBuffer&);
        virtual ~RefStr();
        void operator =(const RefStr&) = delete;
        PCWSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCWSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        PWSTR m_psz;
        size_t m_len;
    };

    inline RefStr::RefStr(PCWSTR psz)
        : RefObj()
        , m_psz(Clone(psz))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(PCWSTR psz, size_t cch)
        : RefObj()
        , m_psz(Clone(psz, cch))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(PCWSTR pszFormat, va_list argList)
        : RefObj()
        , m_psz(VaFormat(pszFormat, argList))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2)
        : RefObj()
        , m_psz(Concat(psz1, psz2))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3, psz4))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3, psz4, psz5))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(PCSTR psz)
        : RefObj()
        , m_psz(ToUcs(psz))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(PCSTR psz, size_t cb)
        : RefObj()
        , m_psz(ToUcs(psz, cb))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(UINT cp, PCSTR psz)
        : RefObj()
        , m_psz(ToUcs(cp, psz))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(UINT cp, PCSTR psz, size_t cb)
        : RefObj()
        , m_psz(ToUcs(cp, psz, cb))
        , m_len(wcslen(m_psz))
    {
    }

    inline RefStr::RefStr(StringBuffer& buf)
        : RefObj()
        , m_psz(buf.Detach())
        , m_len(0)
    {
        if (m_psz)
        {
            m_len = wcslen(m_psz);
        }
        else
        {
            m_psz = Clone(L"");
        }
    }

    inline RefStr::~RefStr()
    {
        free(m_psz);
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
