#pragma once

#include <Windows.h>
#include "hnrt/StringOptions.h"

namespace hnrt
{
    class StringUTF8
    {
    public:

        StringUTF8();
        StringUTF8(const StringUTF8&);
        StringUTF8(PCSTR, SSIZE_T = -1);
        StringUTF8(PCSTR, va_list);
        StringUTF8(StringOptions, PCSTR, ...);
        StringUTF8(PCSTR, PCSTR);
        StringUTF8(SIZE_T);
        StringUTF8(SIZE_T, CHAR);
        StringUTF8(PCWSTR, SSIZE_T = -1);
        ~StringUTF8();
        StringUTF8& ZeroFill();
        StringUTF8& Lettercase(StringOptions);
        StringUTF8& Uppercase();
        StringUTF8& Lowercase();
        StringUTF8& Trim(StringOptions = TRIM);
        StringUTF8& Assign(const StringUTF8&);
        StringUTF8& Assign(PCSTR, SSIZE_T = -1);
        StringUTF8& Append(const StringUTF8&);
        StringUTF8& Append(PCSTR, SSIZE_T = -1);
        StringUTF8& Format(PCSTR, ...);
        StringUTF8& VaFormat(PCSTR, va_list);
        StringUTF8& AppendFormat(PCSTR, ...);
        StringUTF8& VaAppendFormat(PCSTR, va_list);
        StringUTF8& TruncateHead(SIZE_T);
        StringUTF8& TruncateTail(SIZE_T);
        int IndexOf(CHAR, SIZE_T = 0) const;
        int IndexOf(const StringUTF8&, SIZE_T = 0) const;
        bool StartsWith(PCSTR, SSIZE_T = -1) const;
        bool EndsWith(PCSTR, SSIZE_T = -1) const;
        StringUTF8 Wrap(UINT, PCSTR = "\r\n") const;
        long ToLong(long defaultValue = 0L, BOOL* pbSuccessful = nullptr, int nRadix = 10);
        long long ToLongLong(long long defaultValue = 0L, BOOL* pbSuccessful = nullptr, int nRadix = 10);
        double ToDouble(double defaultValue = 0L, BOOL* pbSuccessful = nullptr);
        operator PCSTR() const;
        StringUTF8& operator =(const StringUTF8&);
        StringUTF8& operator =(PCSTR);
        StringUTF8& operator +=(const StringUTF8&);
        StringUTF8& operator +=(PCSTR);
        bool operator ==(const StringUTF8&) const;
        bool operator !=(const StringUTF8&) const;
        bool operator <(const StringUTF8&) const;
        bool operator <=(const StringUTF8&) const;
        bool operator >(const StringUTF8&) const;
        bool operator >=(const StringUTF8&) const;
        StringUTF8 operator +(const StringUTF8&) const;
        PCSTR get_ptr() const;
        SIZE_T get_len() const;
        void set_len(SIZE_T);
        PCHAR get_buf() const;
        BOOL is_set() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len, put = set_len)) SIZE_T Len;
        __declspec(property(get = get_buf)) PCHAR Buf;
        __declspec(property(get = is_set)) BOOL IsSet;

    private:

        PSTR m_psz;

    public:

        static const StringUTF8 Empty;

        static int Compare(PCSTR psz1, PCSTR psz2);
        static int Compare(PCSTR psz1, PCSTR psz2, SSIZE_T cb2);
        static int Compare(PCSTR psz1, SSIZE_T cb1, PCSTR psz2, SSIZE_T cb2);
        static StringUTF8 ToHex(const void*, SIZE_T, StringOptions = LOWERCASE);
    };

    inline StringUTF8& StringUTF8::Uppercase()
    {
        return Lettercase(UPPERCASE);
    }

    inline StringUTF8& StringUTF8::Lowercase()
    {
        return Lettercase(LOWERCASE);
    }

    inline PCSTR StringUTF8::get_ptr() const
    {
        return m_psz ? m_psz : "";
    }

    inline PCHAR StringUTF8::get_buf() const
    {
        return m_psz;
    }

    inline BOOL StringUTF8::is_set() const
    {
        return m_psz ? TRUE : FALSE;
    }

    inline StringUTF8::operator PCSTR() const
    {
        return Ptr;
    }

    inline StringUTF8& StringUTF8::operator =(const StringUTF8& other)
    {
        return Assign(other);
    }

    inline StringUTF8& StringUTF8::operator =(PCSTR psz)
    {
        return Assign(psz);
    }

    inline StringUTF8& StringUTF8::operator +=(const StringUTF8& other)
    {
        return Append(other);
    }

    inline StringUTF8& StringUTF8::operator +=(PCSTR psz)
    {
        return Append(psz);
    }

    inline int StringUTF8::Compare(PCSTR psz1, PCSTR psz2)
    {
        return Compare(psz1, -1, psz2, -1);
    }

    inline int StringUTF8::Compare(PCSTR psz1, PCSTR psz2, SSIZE_T cb2)
    {
        return Compare(psz1, -1, psz2, cb2);
    }
}
