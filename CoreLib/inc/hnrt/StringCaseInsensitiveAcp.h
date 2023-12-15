#pragma once

#include <Windows.h>

namespace hnrt
{
    class StringAcp;

    class StringCaseInsensitiveAcp
    {
    public:

        StringCaseInsensitiveAcp();
        StringCaseInsensitiveAcp(const StringCaseInsensitiveAcp&);
        StringCaseInsensitiveAcp(PCSTR, SSIZE_T = -1);
        StringCaseInsensitiveAcp(const StringAcp&);
        ~StringCaseInsensitiveAcp();
        operator PCSTR() const;
        operator bool() const;
        StringCaseInsensitiveAcp& operator =(const StringCaseInsensitiveAcp&);
        StringCaseInsensitiveAcp& operator =(const StringAcp&);
        StringCaseInsensitiveAcp& operator +=(const StringCaseInsensitiveAcp&);
        StringCaseInsensitiveAcp& operator +=(const StringAcp&);
        bool operator ==(const StringCaseInsensitiveAcp&) const;
        bool operator !=(const StringCaseInsensitiveAcp&) const;
        bool operator <(const StringCaseInsensitiveAcp&) const;
        bool operator <=(const StringCaseInsensitiveAcp&) const;
        bool operator >(const StringCaseInsensitiveAcp&) const;
        bool operator >=(const StringCaseInsensitiveAcp&) const;
        StringCaseInsensitiveAcp operator +(const StringCaseInsensitiveAcp&) const;
        StringCaseInsensitiveAcp operator +(const StringAcp&) const;
        PCSTR get_ptr() const;
        SIZE_T get_len() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len)) SIZE_T Len;

    private:

        PSTR m_psz;

    public:

        static int Compare(PCSTR psz1, PCSTR psz2);
        static int Compare(PCSTR psz1, PCSTR psz2, SSIZE_T cch2);
        static int Compare(PCSTR psz1, SSIZE_T cch1, PCSTR psz2, SSIZE_T cch2);
    };

    inline StringCaseInsensitiveAcp::operator PCSTR() const
    {
        return Ptr;
    }

    inline StringCaseInsensitiveAcp::operator bool() const
    {
        return m_psz != nullptr;
    }

    inline PCSTR StringCaseInsensitiveAcp::get_ptr() const
    {
        return m_psz ? m_psz : "";
    }

    inline int StringCaseInsensitiveAcp::Compare(PCSTR psz1, PCSTR psz2)
    {
        return Compare(psz1, -1, psz2, -1);
    }

    inline int StringCaseInsensitiveAcp::Compare(PCSTR psz1, PCSTR psz2, SSIZE_T cch2)
    {
        return Compare(psz1, -1, psz2, cch2);
    }

    typedef StringCaseInsensitiveAcp StringCaseAcp;

    class StringCaseAcpLessThan
    {
    public:

        bool operator ()(PCSTR psz1, PCSTR psz2) const;
    };

    inline bool StringCaseAcpLessThan::operator ()(PCSTR psz1, PCSTR psz2) const
    {
        return StringCaseInsensitiveAcp::Compare(psz1, psz2) < 0;
    }
}
