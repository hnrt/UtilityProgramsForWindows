#pragma once


#include "hnrt/RefObj.h"
#include "hnrt/Heap.h"
#include "hnrt/StringBuffer.h"


namespace hnrt
{
    class RefMbs
        : public RefObj
    {
    public:

        RefMbs();
        RefMbs(PCSTR);
        RefMbs(PCSTR, size_t);
        RefMbs(PCSTR, PCSTR);
        RefMbs(UINT, PCWSTR);
        RefMbs(UINT, PCWSTR, size_t);
        RefMbs(UINT, StringBuffer&);
        RefMbs(const RefMbs&) = delete;
        virtual ~RefMbs();
        void operator =(const RefMbs&) = delete;
        PCSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        PSTR m_psz;
    };

    inline RefMbs::RefMbs()
        : RefObj()
        , m_psz(nullptr)
    {
    }

    inline RefMbs::RefMbs(PCSTR psz)
        : RefObj()
        , m_psz(Clone(psz))
    {
    }

    inline RefMbs::RefMbs(PCSTR psz, size_t cb)
        : RefObj()
        , m_psz(Clone(psz, cb))
    {
    }

    inline RefMbs::RefMbs(PCSTR psz1, PCSTR psz2)
        : RefObj()
        , m_psz(Concat(psz1, psz2))
    {
    }

    inline RefMbs::RefMbs(UINT cp, PCWSTR psz)
        : RefObj()
        , m_psz(ToAcp(cp, psz))
    {
    }

    inline RefMbs::RefMbs(UINT cp, PCWSTR psz, size_t cb)
        : RefObj()
        , m_psz(ToAcp(cp, psz, cb))
    {
    }

    inline RefMbs::RefMbs(UINT cp, StringBuffer& buf)
        : RefObj()
        , m_psz(ToAcp(cp, buf))
    {
    }

    inline RefMbs::~RefMbs()
    {
        free(m_psz);
    }

    inline PCSTR RefMbs::get_ptr() const
    {
        return m_psz ? m_psz : "";
    }

    inline size_t RefMbs::get_len() const
    {
        return m_psz ? strlen(m_psz) : 0;
    }
}
