#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/Heap.h"


namespace hnrt
{
    class RefStr
        : public RefObj
    {
    public:

        RefStr();
        RefStr(PCWSTR);
        RefStr(PCWSTR, size_t);
        RefStr(PCWSTR, va_list);
        RefStr(PCWSTR, PCWSTR);
        RefStr(PCWSTR, PCWSTR, PCWSTR);
        RefStr(PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        RefStr(PCWSTR, PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        RefStr(const RefStr&) = delete;
        virtual ~RefStr();
        void operator =(const RefStr&) = delete;
        PCWSTR get_ptr() const;
        PCWSTR get_str() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCWSTR Ptr;
        __declspec(property(get = get_str)) PCWSTR Str;
        __declspec(property(get = get_len)) size_t Len;

    private:

        PWSTR m_psz;
    };

    inline RefStr::RefStr()
        : RefObj()
        , m_psz(nullptr)
    {
    }

    inline RefStr::RefStr(PCWSTR psz)
        : RefObj()
        , m_psz(psz ? Clone(psz) : nullptr)
    {
    }

    inline RefStr::RefStr(PCWSTR psz, size_t cch)
        : RefObj()
        , m_psz(psz ? Clone(psz, cch) : nullptr)
    {
    }

    inline RefStr::RefStr(PCWSTR pszFormat, va_list argList)
        : RefObj()
        , m_psz(VaFormat(pszFormat, argList))
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2)
        : RefObj()
        , m_psz(Concat(psz1, psz2))
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3))
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3, psz4))
    {
    }

    inline RefStr::RefStr(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5)
        : RefObj()
        , m_psz(Concat(psz1, psz2, psz3, psz4, psz5))
    {
    }

    inline RefStr::~RefStr()
    {
        free(m_psz);
    }

    inline PCWSTR RefStr::get_ptr() const
    {
        return m_psz;
    }

    inline PCWSTR RefStr::get_str() const
    {
        return m_psz ? m_psz : L"";
    }

    inline size_t RefStr::get_len() const
    {
        return m_psz ? wcslen(m_psz) : 0;
    }
}
