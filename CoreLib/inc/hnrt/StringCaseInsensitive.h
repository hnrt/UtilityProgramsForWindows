#pragma once

#include <Windows.h>

namespace hnrt
{
    class String;

    class StringCaseInsensitive
    {
    public:

        StringCaseInsensitive();
        StringCaseInsensitive(const StringCaseInsensitive&);
        StringCaseInsensitive(PCWSTR, SSIZE_T = -1);
        StringCaseInsensitive(const String&);
        ~StringCaseInsensitive();
        operator PCWSTR() const;
        operator bool() const;
        StringCaseInsensitive& operator =(const StringCaseInsensitive&);
        StringCaseInsensitive& operator =(const String&);
        StringCaseInsensitive& operator +=(const StringCaseInsensitive&);
        StringCaseInsensitive& operator +=(const String&);
        bool operator ==(const StringCaseInsensitive&) const;
        bool operator !=(const StringCaseInsensitive&) const;
        bool operator <(const StringCaseInsensitive&) const;
        bool operator <=(const StringCaseInsensitive&) const;
        bool operator >(const StringCaseInsensitive&) const;
        bool operator >=(const StringCaseInsensitive&) const;
        StringCaseInsensitive operator +(const StringCaseInsensitive&) const;
        StringCaseInsensitive operator +(const String&) const;
        PCWSTR get_ptr() const;
        SIZE_T get_len() const;

        __declspec(property(get = get_ptr)) PCWSTR Ptr;
        __declspec(property(get = get_len)) SIZE_T Len;

    private:

        PWSTR m_psz;

    public:

        static int Compare(PCWSTR psz1, PCWSTR psz2);
        static int Compare(PCWSTR psz1, PCWSTR psz2, SSIZE_T cch2);
        static int Compare(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2);
    };

    inline StringCaseInsensitive::operator PCWSTR() const
    {
        return Ptr;
    }

    inline StringCaseInsensitive::operator bool() const
    {
        return m_psz != nullptr;
    }

    inline PCWSTR StringCaseInsensitive::get_ptr() const
    {
        return m_psz ? m_psz : L"";
    }

    inline int StringCaseInsensitive::Compare(PCWSTR psz1, PCWSTR psz2)
    {
        return Compare(psz1, -1, psz2, -1);
    }

    inline int StringCaseInsensitive::Compare(PCWSTR psz1, PCWSTR psz2, SSIZE_T cch2)
    {
        return Compare(psz1, -1, psz2, cch2);
    }

    typedef StringCaseInsensitive StringCase;

    class StringCaseLessThan
    {
    public:

        bool operator ()(PCWSTR psz1, PCWSTR psz2) const;
    };

    inline bool StringCaseLessThan::operator ()(PCWSTR psz1, PCWSTR psz2) const
    {
        return StringCaseInsensitive::Compare(psz1, psz2) < 0;
    }
}
