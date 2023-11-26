#pragma once


#include <Windows.h>


namespace hnrt
{
    class RefMbs;
    class StringAcp;

    class StringCaseInsensitiveAcp
    {
    public:

        StringCaseInsensitiveAcp();
        StringCaseInsensitiveAcp(const StringCaseInsensitiveAcp&);
        StringCaseInsensitiveAcp(PCSTR, INT_PTR = -1);
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
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        RefMbs* m_ptr;

    public:

        static int Compare(PCSTR psz1, PCSTR psz2);
        static int Compare(PCSTR psz1, PCSTR psz2, INT_PTR cch2);
        static int Compare(PCSTR psz1, INT_PTR cch1, PCSTR psz2, INT_PTR cch2);
    };

    inline StringCaseInsensitiveAcp::operator PCSTR() const
    {
        return Ptr;
    }

    inline StringCaseInsensitiveAcp::operator bool() const
    {
        return m_ptr != nullptr;
    }

    inline int StringCaseInsensitiveAcp::Compare(PCSTR psz1, PCSTR psz2)
    {
        return Compare(psz1, -1, psz2, -1);
    }

    inline int StringCaseInsensitiveAcp::Compare(PCSTR psz1, PCSTR psz2, INT_PTR cch2)
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
