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
        StringAcp(PCSTR, INT_PTR = -1);
        StringAcp(PCSTR, va_list);
        StringAcp(StringOptions, PCSTR, ...);
        StringAcp(PCSTR, PCSTR);
        StringAcp(PCSTR, PCSTR, PCSTR);
        StringAcp(PCSTR, PCSTR, PCSTR, PCSTR);
        StringAcp(PCSTR, PCSTR, PCSTR, PCSTR, PCSTR);
        StringAcp(PCWSTR, INT_PTR = -1);
        ~StringAcp();
        StringAcp& ZeroFill();
        StringAcp& Uppercase();
        StringAcp& Lowercase();
        StringAcp& Trim(StringOptions = TRIM);
        StringAcp& Assign(const StringAcp&);
        StringAcp& Assign(PCSTR, INT_PTR = -1);
        StringAcp& Append(const StringAcp&);
        StringAcp& Append(PCSTR, INT_PTR = -1);
        StringAcp& Format(PCSTR, ...);
        StringAcp& VaFormat(PCSTR, va_list);
        StringAcp& AppendFormat(PCSTR, ...);
        StringAcp& VaAppendFormat(PCSTR, va_list);
        int IndexOf(CHAR, INT_PTR = 0);
        operator PCSTR() const;
        operator bool() const;
        StringAcp& operator =(const StringAcp&);
        StringAcp& operator =(PCSTR);
        StringAcp& operator +=(const StringAcp&);
        StringAcp& operator +=(PCSTR);
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

        RefMbs* m_ptr;

        friend class StringCaseInsensitiveAcp;

    public:

        static const StringAcp Empty;

        static int Compare(PCSTR psz1, PCSTR psz2);
        static int Compare(PCSTR psz1, PCSTR psz2, INT_PTR cb2);
        static int Compare(PCSTR psz1, INT_PTR cb1, PCSTR psz2, INT_PTR cb2);
    };

    inline StringAcp::operator PCSTR() const
    {
        return Ptr;
    }

    inline StringAcp::operator bool() const
    {
        return m_ptr != nullptr;
    }

    inline StringAcp& StringAcp::operator =(const StringAcp& other)
    {
        return Assign(other);
    }

    inline StringAcp& StringAcp::operator =(PCSTR psz)
    {
        return Assign(psz);
    }

    inline StringAcp& StringAcp::operator +=(const StringAcp& other)
    {
        return Append(other);
    }

    inline StringAcp& StringAcp::operator +=(PCSTR psz)
    {
        return Append(psz);
    }

    inline int StringAcp::Compare(PCSTR psz1, PCSTR psz2)
    {
        return Compare(psz1, -1, psz2, -1);
    }

    inline int StringAcp::Compare(PCSTR psz1, PCSTR psz2, INT_PTR cb2)
    {
        return Compare(psz1, -1, psz2, cb2);
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
}
