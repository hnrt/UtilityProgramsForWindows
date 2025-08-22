#pragma once

#include <Windows.h>
#include "hnrt/StringOptions.h"
#include "hnrt/LineBreak.h"

namespace hnrt
{
    class String
    {
    private:

        PWSTR m_psz;

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
        String& Assign(const String&);
        String& Assign(PCWSTR, SSIZE_T = -1);
        String& Append(const String&);
        String& Append(PCWSTR, SSIZE_T = -1);
        String& VaFormat(PCWSTR, va_list);
        String& AppendFormat(PCWSTR, ...);
        String& VaAppendFormat(PCWSTR, va_list);
        String& TruncateHead(SIZE_T);
        String& TruncateTail(SIZE_T);

        String Lettercase(StringOptions);
        String Uppercase();
        String Lowercase();
        String Trim(StringOptions = TRIM);
        String Replace(PCWSTR, PCWSTR, int = INT_MAX) const;
        String Wrap(UINT, PCWSTR = L"\r\n") const;
        String ChangeLineBreak(LineBreak) const;

        int IndexOf(int, SIZE_T = 0) const;
        int IndexOf(const String&, SIZE_T = 0) const;
        bool StartsWith(PCWSTR, SSIZE_T = -1) const;
        bool EndsWith(PCWSTR, SSIZE_T = -1) const;

        long ToLong(long defaultValue = 0L, BOOL* pbSuccessful = nullptr, int nRadix = 10);
        unsigned long ToUnsignedLong(unsigned long defaultValue = 0L, BOOL* pbSuccessful = nullptr, int nRadix = 10);
        long long ToLongLong(long long defaultValue = 0L, BOOL* pbSuccessful = nullptr, int nRadix = 10);
        unsigned long long ToUnsignedLongLong(unsigned long long defaultValue = 0L, BOOL* pbSuccessful = nullptr, int nRadix = 10);
        double ToDouble(double defaultValue = 0L, BOOL* pbSuccessful = nullptr);

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

    public:

        PCWSTR get_Ptr() const;
        SIZE_T get_Len() const;
        PWCHAR get_Buf() const;
        BOOL is_set() const;

        __declspec(property(get = get_Ptr)) PCWSTR Ptr;
        __declspec(property(get = get_Len)) SIZE_T Len;
        __declspec(property(get = get_Buf)) PWCHAR Buf;
        __declspec(property(get = is_set)) BOOL IsSet;

    public:

        static const String Empty;

        static String Format(PCWSTR, ...);
        static int Compare(PCWSTR psz1, PCWSTR psz2);
        static int Compare(PCWSTR psz1, PCWSTR psz2, SSIZE_T cch2);
        static int Compare(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2);
        static int CompareCaseInsensitively(PCWSTR psz1, PCWSTR psz2);
        static int CompareCaseInsensitively(PCWSTR psz1, PCWSTR psz2, SSIZE_T cch2);
        static int CompareCaseInsensitively(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2);
    };

    inline String String::Uppercase()
    {
        return Lettercase(UPPERCASE);
    }

    inline String String::Lowercase()
    {
        return Lettercase(LOWERCASE);
    }

    inline String::operator PCWSTR() const
    {
        return Ptr;
    }

    inline String& String::operator =(const String& src)
    {
        return Assign(src);
    }

    inline String& String::operator =(PCWSTR psz)
    {
        return Assign(psz);
    }

    inline String& String::operator +=(const String& src)
    {
        return Append(src);
    }

    inline String& String::operator +=(PCWSTR psz)
    {
        return Append(psz);
    }

    inline PCWSTR String::get_Ptr() const
    {
        return m_psz ? m_psz : L"";
    }

    inline PWCHAR String::get_Buf() const
    {
        return m_psz;
    }

    inline BOOL String::is_set() const
    {
        return m_psz ? TRUE : FALSE;
    }

    inline int String::Compare(PCWSTR psz1, PCWSTR psz2)
    {
        return Compare(psz1, -1, psz2, -1);
    }

    inline int String::Compare(PCWSTR psz1, PCWSTR psz2, SSIZE_T cch2)
    {
        return Compare(psz1, -1, psz2, cch2);
    }

    inline int String::CompareCaseInsensitively(PCWSTR psz1, PCWSTR psz2)
    {
        return CompareCaseInsensitively(psz1, -1, psz2, -1);
    }

    inline int String::CompareCaseInsensitively(PCWSTR psz1, PCWSTR psz2, SSIZE_T cch2)
    {
        return CompareCaseInsensitively(psz1, -1, psz2, cch2);
    }
}
