#pragma once


#include <Windows.h>
#include <stdarg.h>


namespace hnrt
{
    class RefStr;

    class String
    {
    public:

        static String Format2(PCWSTR pszFormat, ...);

        String();
        String(PCWSTR);
        String(PCWSTR, size_t);
        String(PCWSTR, va_list);
        String(PCWSTR, PCWSTR);
        String(PCWSTR, PCWSTR, PCWSTR);
        String(PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        String(PCWSTR, PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        String(const String&);
        ~String();
        String& operator =(const String&);
        bool operator ==(const String&) const;
        bool operator !=(const String&) const;
        bool operator <(const String&) const;
        bool operator <=(const String&) const;
        bool operator >(const String&) const;
        bool operator >=(const String&) const;
        String operator +(const String&) const;
        String& operator +=(const String&);
        operator PCWSTR() const;
        PCWSTR get_ptr() const;
        PCWSTR get_str() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCWSTR Ptr;
        __declspec(property(get = get_str)) PCWSTR Str;
        __declspec(property(get = get_len)) size_t Len;

    private:

        RefStr* m_ptr;

    public:

        static PCWSTR Copy(PCWSTR psz, size_t cch = static_cast<size_t>(-1));
        static PCWSTR Append(PCWSTR psz1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static PCWSTR Format(PCWSTR pszFormat, ...);
        static PCWSTR VaFormat(PCWSTR pszFormat, va_list argList);
        static PCWSTR AppendFormat(PCWSTR psz, PCWSTR pszFormat, ...);
        static PCWSTR VaAppendFormat(PCWSTR psz, PCWSTR pszFormat, va_list argList);
        static PCWSTR Trim(PCWSTR psz, size_t cch = static_cast<size_t>(-1));
        static PCWSTR TrimHead(PCWSTR psz, size_t cch = static_cast<size_t>(-1));
        static PCWSTR TrimTail(PCWSTR psz, size_t cch = static_cast<size_t>(-1));
        static int Compare(PCWSTR psz1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int CaseCompare(PCWSTR psz1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int Compare(PCWSTR psz1, size_t cch1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int CaseCompare(PCWSTR psz1, size_t cch1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));

        static int Compare(PCSTR psz1, PCSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int CaseCompare(PCSTR psz1, PCSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int Compare(PCSTR psz1, size_t cch1, PCSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int CaseCompare(PCSTR psz1, size_t cch1, PCSTR psz2, size_t cch2 = static_cast<size_t>(-1));

        static PCWSTR ToUcs(PCSTR psz, size_t cch = static_cast<size_t>(-1));
        static PCSTR ToAcp(PCWSTR psz, size_t cch = static_cast<size_t>(-1));
    };

    String FormatString(PCWSTR pszFormat, ...);

    class StringLessThan
    {
    public:

        bool operator ()(PCWSTR psz1, PCWSTR psz2) const;
    };

    inline bool StringLessThan::operator ()(PCWSTR psz1, PCWSTR psz2) const
    {
        return String::Compare(psz1, psz2) < 0;
    }

    class StringCaseLessThan
    {
    public:

        bool operator ()(PCWSTR psz1, PCWSTR psz2) const;
    };

    inline bool StringCaseLessThan::operator ()(PCWSTR psz1, PCWSTR psz2) const
    {
        return String::CaseCompare(psz1, psz2) < 0;
    }

    class AcpStringLessThan
    {
    public:

        bool operator ()(PCSTR psz1, PCSTR psz2) const;
    };

    inline bool AcpStringLessThan::operator ()(PCSTR psz1, PCSTR psz2) const
    {
        return String::Compare(psz1, psz2) < 0;
    }

    class AcpStringCaseLessThan
    {
    public:

        bool operator ()(PCSTR psz1, PCSTR psz2) const;
    };

    inline bool AcpStringCaseLessThan::operator ()(PCSTR psz1, PCSTR psz2) const
    {
        return String::CaseCompare(psz1, psz2) < 0;
    }
}
