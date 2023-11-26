#pragma once


#include <Windows.h>
#include "hnrt/StringOptions.h"


namespace hnrt
{
    class RefStr;

    class String
    {
    public:

        String();
        String(const String&);
        String(PCWSTR, INT_PTR = -1);
        String(PCWSTR, va_list);
        String(StringOptions, PCWSTR, ...);
        String(PCWSTR, PCWSTR);
        String(PCWSTR, PCWSTR, PCWSTR);
        String(PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        String(PCWSTR, PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        String(UINT, PCSTR, INT_PTR = -1);
        ~String();
        String& ZeroFill();
        String& Uppercase();
        String& Lowercase();
        String& Trim(StringOptions = TRIM);
        String& Assign(const String&);
        String& Assign(PCWSTR, INT_PTR = -1);
        String& Append(const String&);
        String& Append(PCWSTR, INT_PTR = -1);
        String& Format(PCWSTR, ...);
        String& VaFormat(PCWSTR, va_list);
        String& AppendFormat(PCWSTR, ...);
        String& VaAppendFormat(PCWSTR, va_list);
        int IndexOf(WCHAR, INT_PTR = 0);
        operator PCWSTR() const;
        operator bool() const;
        String& operator =(const String&);
        String& operator =(PCWSTR);
        String& operator +=(const String&);
        String& operator +=(PCWSTR);
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

        RefStr* m_ptr;

        friend class StringCaseInsensitive;

    public:

        static const String Empty;

        static int Compare(PCWSTR psz1, PCWSTR psz2);
        static int Compare(PCWSTR psz1, PCWSTR psz2, INT_PTR cch2);
        static int Compare(PCWSTR psz1, INT_PTR cch1, PCWSTR psz2, INT_PTR cch2);
    };

    inline String::operator PCWSTR() const
    {
        return Ptr;
    }

    inline String::operator bool() const
    {
        return m_ptr != nullptr;
    }

    inline String& String::operator =(const String& other)
    {
        return Assign(other);
    }

    inline String& String::operator =(PCWSTR psz)
    {
        return Assign(psz);
    }

    inline String& String::operator +=(const String& other)
    {
        return Append(other);
    }

    inline String& String::operator +=(PCWSTR psz)
    {
        return Append(psz);
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
        return String::Compare(psz1, -1, psz2, -1) < 0;
    }
}
