#pragma once


#include <Windows.h>
#include <stdarg.h>
#include "hnrt/StringOptions.h"


namespace hnrt
{
    class RefStr;
    class StringBuffer;

    class String
    {
    public:

        String();
        String(const String&);
        String(PCWSTR);
        String(PCWSTR, size_t);
        String(PCWSTR, va_list);
        String(StringOptions, PCWSTR, ...);
        String(PCWSTR, PCWSTR);
        String(PCWSTR, PCWSTR, PCWSTR);
        String(PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        String(PCWSTR, PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        String(PCSTR);
        String(PCSTR, size_t);
        String(UINT, PCSTR);
        String(UINT, PCSTR, size_t);
        String(StringBuffer&);
        ~String();
        operator PCWSTR() const;
        operator bool() const;
        String& operator =(const String&);
        String& operator +=(const String&);
        bool operator ==(const String&) const;
        bool operator !=(const String&) const;
        bool operator <(const String&) const;
        bool operator <=(const String&) const;
        bool operator >(const String&) const;
        bool operator >=(const String&) const;
        String operator +(const String&) const;
        PCWSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCWSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        String(RefStr*);

        RefStr* m_ptr;

        friend class StringCaseInsensitive;

    public:

        static const String Empty;

        static int Compare(PCWSTR psz1, PCWSTR psz2);
        static int Compare(PCWSTR psz1, PCWSTR psz2, INT_PTR cch2);
        static int Compare(PCWSTR psz1, INT_PTR cch1, PCWSTR psz2, INT_PTR cch2);

        static PCWSTR Copy(PCWSTR psz, size_t cch = static_cast<size_t>(-1));
    };

    inline String::operator PCWSTR() const
    {
        return Ptr;
    }

    inline String::operator bool() const
    {
        return m_ptr != nullptr;
    }

    inline int String::Compare(PCWSTR psz1, PCWSTR psz2)
    {
        return Compare(psz1, -1, psz2, -1);
    }

    inline int String::Compare(PCWSTR psz1, PCWSTR psz2, INT_PTR cch2)
    {
        return Compare(psz1, -1, psz2, cch2);
    }

    class StringLessThan
    {
    public:

        bool operator ()(PCWSTR psz1, PCWSTR psz2) const;
    };

    inline bool StringLessThan::operator ()(PCWSTR psz1, PCWSTR psz2) const
    {
        return String::Compare(psz1, psz2) < 0;
    }
}
