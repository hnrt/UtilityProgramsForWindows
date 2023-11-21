#pragma once


#include <Windows.h>
#include "hnrt/StringOptions.h"


namespace hnrt
{
    class RefMbs;

    class StringAcp
    {
    public:

        StringAcp();
        StringAcp(const StringAcp&);
        StringAcp(PCSTR);
        StringAcp(PCSTR, size_t);
        StringAcp(PCSTR, va_list);
        StringAcp(StringOptions, PCSTR, ...);
        StringAcp(PCWSTR);
        StringAcp(PCWSTR, size_t);
        ~StringAcp();
        operator PCSTR() const;
        operator bool() const;
        StringAcp& operator =(const StringAcp&);
        StringAcp& operator +=(const StringAcp&);
        bool operator ==(const StringAcp&) const;
        bool operator !=(const StringAcp&) const;
        bool operator <(const StringAcp&) const;
        bool operator <=(const StringAcp&) const;
        bool operator >(const StringAcp&) const;
        bool operator >=(const StringAcp&) const;
        StringAcp operator +(const StringAcp&) const;
        PCSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        StringAcp(RefMbs*);

        RefMbs* m_ptr;

    public:

        static int Compare(PCSTR psz1, PCSTR psz2);
        static int Compare(PCSTR psz1, PCSTR psz2, INT_PTR cb2);
        static int Compare(PCSTR psz1, INT_PTR cb1, PCSTR psz2, INT_PTR cb2);

        static int CaseCompare(PCSTR psz1, PCSTR psz2);
        static int CaseCompare(PCSTR psz1, PCSTR psz2, INT_PTR cb2);
        static int CaseCompare(PCSTR psz1, INT_PTR cb1, PCSTR psz2, INT_PTR cb2);
    };

    inline StringAcp::operator PCSTR() const
    {
        return Ptr;
    }

    inline StringAcp::operator bool() const
    {
        return m_ptr != nullptr;
    }

    inline int StringAcp::Compare(PCSTR psz1, PCSTR psz2)
    {
        return Compare(psz1, -1, psz2, -1);
    }

    inline int StringAcp::Compare(PCSTR psz1, PCSTR psz2, INT_PTR cb2)
    {
        return Compare(psz1, -1, psz2, cb2);
    }

    inline int StringAcp::CaseCompare(PCSTR psz1, PCSTR psz2)
    {
        return CaseCompare(psz1, -1, psz2, -1);
    }

    inline int StringAcp::CaseCompare(PCSTR psz1, PCSTR psz2, INT_PTR cb2)
    {
        return CaseCompare(psz1, -1, psz2, cb2);
    }

    class StringAcpLessThan
    {
    public:

        bool operator ()(PCSTR psz1, PCSTR psz2) const;
    };

    inline bool StringAcpLessThan::operator ()(PCSTR psz1, PCSTR psz2) const
    {
        return StringAcp::Compare(psz1, -1, psz2, -1) < 0;
    }

    class StringAcpCaseLessThan
    {
    public:

        bool operator ()(PCSTR psz1, PCSTR psz2) const;
    };

    inline bool StringAcpCaseLessThan::operator ()(PCSTR psz1, PCSTR psz2) const
    {
        return StringAcp::CaseCompare(psz1, -1, psz2, -1) < 0;
    }
}
