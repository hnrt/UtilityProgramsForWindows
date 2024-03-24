#pragma once

#include <Windows.h>
#include "hnrt/StringOptions.h"

namespace hnrt
{
    class StringAcp
    {
    public:

        StringAcp();
        StringAcp(const StringAcp&);
        StringAcp(PCSTR, SSIZE_T = -1);
        StringAcp(PCSTR, va_list);
        StringAcp(StringOptions, PCSTR, ...);
        StringAcp(PCSTR, PCSTR);
        StringAcp(SIZE_T);
        StringAcp(SIZE_T, CHAR);
        StringAcp(PCWSTR, SSIZE_T = -1);
        ~StringAcp();
        StringAcp& ZeroFill();
        StringAcp& Lettercase(StringOptions);
        StringAcp& Uppercase();
        StringAcp& Lowercase();
        StringAcp& Trim(StringOptions = TRIM);
        StringAcp& Assign(const StringAcp&);
        StringAcp& Assign(PCSTR, SSIZE_T = -1);
        StringAcp& Append(const StringAcp&);
        StringAcp& Append(PCSTR, SSIZE_T = -1);
        StringAcp& Format(PCSTR, ...);
        StringAcp& VaFormat(PCSTR, va_list);
        StringAcp& AppendFormat(PCSTR, ...);
        StringAcp& VaAppendFormat(PCSTR, va_list);
        StringAcp& TruncateHead(SIZE_T);
        StringAcp& TruncateTail(SIZE_T);
        int IndexOf(CHAR, SIZE_T = 0) const;
        int IndexOf(const StringAcp&, SIZE_T = 0) const;
        bool StartsWith(PCSTR, SSIZE_T = -1) const;
        bool EndsWith(PCSTR, SSIZE_T = -1) const;
        StringAcp Wrap(UINT, PCSTR = "\r\n") const;
        long ToLong(long defaultValue = 0L, BOOL* pbSuccessful = nullptr, int nRadix = 10);
        unsigned long ToUnsignedLong(unsigned long defaultValue = 0L, BOOL* pbSuccessful = nullptr, int nRadix = 10);
        long long ToLongLong(long long defaultValue = 0L, BOOL* pbSuccessful = nullptr, int nRadix = 10);
        unsigned long long ToUnsignedLongLong(unsigned long long defaultValue = 0L, BOOL* pbSuccessful = nullptr, int nRadix = 10);
        double ToDouble(double defaultValue = 0L, BOOL* pbSuccessful = nullptr);
        operator PCSTR() const;
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

        friend class StringCaseInsensitiveAcp;

    public:

        static const StringAcp Empty;

        static int Compare(PCSTR psz1, PCSTR psz2);
        static int Compare(PCSTR psz1, PCSTR psz2, SSIZE_T cb2);
        static int Compare(PCSTR psz1, SSIZE_T cb1, PCSTR psz2, SSIZE_T cb2);
        static StringAcp ToHex(const void*, SIZE_T, StringOptions = LOWERCASE);
    };

    inline StringAcp& StringAcp::Uppercase()
    {
        return Lettercase(UPPERCASE);
    }

    inline StringAcp& StringAcp::Lowercase()
    {
        return Lettercase(LOWERCASE);
    }

    inline PCSTR StringAcp::get_ptr() const
    {
        return m_psz ? m_psz : "";
    }

    inline PCHAR StringAcp::get_buf() const
    {
        return m_psz;
    }

    inline BOOL StringAcp::is_set() const
    {
        return m_psz ? TRUE : FALSE;
    }

    inline StringAcp::operator PCSTR() const
    {
        return Ptr;
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

    inline int StringAcp::Compare(PCSTR psz1, PCSTR psz2, SSIZE_T cb2)
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
