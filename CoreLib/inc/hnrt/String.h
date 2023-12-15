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
        String(PCWSTR, SSIZE_T = -1);
        String(PCWSTR, va_list);
        String(StringOptions, PCWSTR, ...);
        String(PCWSTR, PCWSTR);
        String(SIZE_T);
        String(SIZE_T, WCHAR);
        String(UINT, PCSTR, SSIZE_T = -1);
        ~String();
        String& ZeroFill();
        String& Lettercase(StringOptions);
        String& Uppercase();
        String& Lowercase();
        String& Trim(StringOptions = TRIM);
        String& Assign(const String&);
        String& Assign(PCWSTR, SSIZE_T = -1);
        String& Append(const String&);
        String& Append(PCWSTR, SSIZE_T = -1);
        String& Format(PCWSTR, ...);
        String& VaFormat(PCWSTR, va_list);
        String& AppendFormat(PCWSTR, ...);
        String& VaAppendFormat(PCWSTR, va_list);
        String& TruncateHead(SIZE_T);
        String& TruncateTail(SIZE_T);
        int IndexOf(WCHAR, SIZE_T = 0) const;
        int IndexOf(const String&, SIZE_T = 0) const;
        bool StartsWith(PCWSTR, SSIZE_T = -1) const;
        bool EndsWith(PCWSTR, SSIZE_T = -1) const;
        operator PCWSTR() const;
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
        SIZE_T get_len() const;
        PWCHAR get_buf() const;

        __declspec(property(get = get_ptr)) PCWSTR Ptr;
        __declspec(property(get = get_len)) SIZE_T Len;
        __declspec(property(get = get_buf)) PWCHAR Buf;

    private:

        PWSTR m_psz;

        friend class StringCaseInsensitive;

    public:

        static const String Empty;

        static int Compare(PCWSTR psz1, PCWSTR psz2);
        static int Compare(PCWSTR psz1, PCWSTR psz2, SSIZE_T cch2);
        static int Compare(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2);
        static String ToHex(const void*, SIZE_T, StringOptions = LOWERCASE);
    };

    inline String& String::Uppercase()
    {
        return Lettercase(UPPERCASE);
    }

    inline String& String::Lowercase()
    {
        return Lettercase(LOWERCASE);
    }

    inline PCWSTR String::get_ptr() const
    {
        return m_psz ? m_psz : L"";
    }

    inline PWCHAR String::get_buf() const
    {
        return m_psz;
    }

    inline String::operator PCWSTR() const
    {
        return Ptr;
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

    inline int String::Compare(PCWSTR psz1, PCWSTR psz2, SSIZE_T cch2)
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
