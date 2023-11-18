#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/Heap.h"


namespace hnrt
{
    class RefMbs
        : public RefObj
    {
    public:

        RefMbs(const RefMbs&) = delete;
        RefMbs(PCSTR);
        RefMbs(PCSTR, size_t);
        RefMbs(PCSTR, va_list);
        RefMbs(PCSTR, PCSTR);
        RefMbs(UINT, PCWSTR);
        RefMbs(UINT, PCWSTR, size_t);
        virtual ~RefMbs();
        void operator =(const RefMbs&) = delete;
        PCSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        PSTR m_psz;
        size_t m_len;
    };

    inline RefMbs::RefMbs(PCSTR psz)
        : RefObj()
        , m_psz(Clone(psz))
        , m_len(strlen(m_psz))
    {
    }

    inline RefMbs::RefMbs(PCSTR psz, size_t cb)
        : RefObj()
        , m_psz(Clone(psz, cb))
        , m_len(strlen(m_psz))
    {
    }

    inline RefMbs::RefMbs(PCSTR pszFormat, va_list argList)
        : RefObj()
        , m_psz(VaFormat(pszFormat, argList))
        , m_len(strlen(m_psz))
    {
    }

    inline RefMbs::RefMbs(PCSTR psz1, PCSTR psz2)
        : RefObj()
        , m_psz(Concat(psz1, psz2))
        , m_len(strlen(m_psz))
    {
    }

    inline RefMbs::RefMbs(UINT cp, PCWSTR psz)
        : RefObj()
        , m_psz(ToAcp(cp, psz))
        , m_len(strlen(m_psz))
    {
    }

    inline RefMbs::RefMbs(UINT cp, PCWSTR psz, size_t cb)
        : RefObj()
        , m_psz(ToAcp(cp, psz, cb))
        , m_len(strlen(m_psz))
    {
    }

    inline RefMbs::~RefMbs()
    {
        free(m_psz);
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
