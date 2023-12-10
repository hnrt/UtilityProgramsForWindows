#pragma once

#include <Windows.h>
#include "hnrt/StringOptions.h"

namespace hnrt
{
    class String
    {
    public:

        String();
        String(const String&);
        String(PCWSTR, INT_PTR = -1);
        String(PCWSTR, va_list);
        String(StringOptions, PCWSTR, ...);
        String(PCWSTR, PCWSTR);
        String(size_t, WCHAR);
        String(UINT, PCSTR, INT_PTR = -1);
        ~String();
        String& ZeroFill();
        String& Lettercase(StringOptions);
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
        String& TruncateHead(size_t);
        String& TruncateTail(size_t);
        int IndexOf(WCHAR, INT_PTR = 0) const;
        int IndexOf(const String&, INT_PTR = 0) const;
        bool StartsWith(PCWSTR, INT_PTR = -1) const;
        bool EndsWith(PCWSTR, INT_PTR = -1) const;
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

        PCWSTR m_psz;

        friend class StringCaseInsensitive;

    public:

        static const String Empty;

        static int Compare(PCWSTR psz1, PCWSTR psz2);
        static int Compare(PCWSTR psz1, PCWSTR psz2, INT_PTR cch2);
        static int Compare(PCWSTR psz1, INT_PTR cch1, PCWSTR psz2, INT_PTR cch2);
        static String ToHex(const void*, size_t, StringOptions = LOWERCASE);
    };

    inline String& String::Uppercase()
    {
        return Lettercase(UPPERCASE);
    }

    inline String& String::Lowercase()
    {
        return Lettercase(LOWERCASE);
    }

    inline String::operator PCWSTR() const
    {
        return Ptr;
    }

    inline String::operator bool() const
    {
        return m_psz != nullptr;
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
