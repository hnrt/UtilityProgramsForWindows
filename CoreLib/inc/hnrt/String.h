#pragma once


#include <Windows.h>
#include <stdarg.h>


namespace hnrt
{
    class StringBuffer;

    enum StringOptions
    {
        PRINTF = 1,
        CONCAT,
        UPPERCASE,
        LOWERCASE,
        TRIM,
        TRIM_HEAD,
        TRIM_TAIL
    };

    class RefStr;

    class String
    {
    public:

        String();
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
        operator bool() const;
        PCWSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCWSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        String(RefStr*);

        RefStr* m_ptr;

        friend class CaseString;

    public:

        static const String Empty;

        static int Compare(PCWSTR psz1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int CaseCompare(PCWSTR psz1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int Compare(PCWSTR psz1, size_t cch1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int CaseCompare(PCWSTR psz1, size_t cch1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));

        static int Compare(PCSTR psz1, PCSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int CaseCompare(PCSTR psz1, PCSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int Compare(PCSTR psz1, size_t cch1, PCSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static int CaseCompare(PCSTR psz1, size_t cch1, PCSTR psz2, size_t cch2 = static_cast<size_t>(-1));

        static PCWSTR Copy(PCWSTR psz, size_t cch = static_cast<size_t>(-1));
    };

    class CaseString
    {
    public:

        CaseString();
        CaseString(PCWSTR);
        CaseString(PCWSTR, size_t);
        CaseString(const String&);
        CaseString(const CaseString&);
        ~CaseString();
        String ToString() const;
        CaseString& operator =(const String&);
        CaseString& operator =(const CaseString&);
        bool operator ==(const CaseString&) const;
        bool operator !=(const CaseString&) const;
        bool operator <(const CaseString&) const;
        bool operator <=(const CaseString&) const;
        bool operator >(const CaseString&) const;
        bool operator >=(const CaseString&) const;
        CaseString operator +(const CaseString&) const;
        CaseString& operator +=(const CaseString&);
        operator PCWSTR() const;
        operator bool() const;
        PCWSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCWSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        CaseString(RefStr*);

        RefStr* m_ptr;
    };

    class RefMbs;

    class AcpString
    {
    public:

        AcpString();
        AcpString(PCSTR);
        AcpString(PCSTR, size_t);
        AcpString(PCWSTR);
        AcpString(PCWSTR, size_t);
        AcpString(const AcpString&);
        ~AcpString();
        AcpString& operator =(const AcpString&);
        bool operator ==(const AcpString&) const;
        bool operator !=(const AcpString&) const;
        bool operator <(const AcpString&) const;
        bool operator <=(const AcpString&) const;
        bool operator >(const AcpString&) const;
        bool operator >=(const AcpString&) const;
        AcpString operator +(const AcpString&) const;
        AcpString& operator +=(const AcpString&);
        operator PCSTR() const;
        operator bool() const;
        String ToString() const;
        PCSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        AcpString(RefMbs*);

        RefMbs* m_ptr;
    };

    class UTF8
    {
    public:

        UTF8();
        UTF8(PCSTR);
        UTF8(PCSTR, size_t);
        UTF8(PCWSTR);
        UTF8(PCWSTR, size_t);
        UTF8(const UTF8&);
        ~UTF8();
        UTF8& operator =(const UTF8&);
        bool operator ==(const UTF8&) const;
        bool operator !=(const UTF8&) const;
        bool operator <(const UTF8&) const;
        bool operator <=(const UTF8&) const;
        bool operator >(const UTF8&) const;
        bool operator >=(const UTF8&) const;
        UTF8 operator +(const UTF8&) const;
        UTF8& operator +=(const UTF8&);
        operator PCSTR() const;
        operator bool() const;
        String ToString() const;
        PCSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        UTF8(RefMbs*);

        RefMbs* m_ptr;
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
