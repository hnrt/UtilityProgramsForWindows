#pragma once


#include <Windows.h>
#include <stdarg.h>


namespace hnrt
{
    class String
    {
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

        String() = delete;
    };

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
