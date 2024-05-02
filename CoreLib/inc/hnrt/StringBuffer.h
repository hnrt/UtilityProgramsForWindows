#pragma once


#include <Windows.h>


namespace hnrt
{
    class String;

    class StringBuffer
    {
    public:

        StringBuffer(const StringBuffer&);
        StringBuffer(SIZE_T = 0);
        StringBuffer(SSIZE_T, PCWSTR);
        StringBuffer(const String&);
        ~StringBuffer();
        PWCHAR Detach();
        StringBuffer& Resize(SIZE_T);
        StringBuffer& Assign(const StringBuffer&);
        StringBuffer& Assign(const String&);
        StringBuffer& Assign(PCWSTR, SSIZE_T = -1);
        StringBuffer& Assign(int);
        StringBuffer& Format(PCWSTR pszFormat, ...);
        StringBuffer& VaFormat(PCWSTR pszFormat, va_list argList);
        StringBuffer& Append(const StringBuffer&);
        StringBuffer& Append(const String&);
        StringBuffer& Append(PCWSTR, SSIZE_T = -1);
        StringBuffer& Append(int);
        StringBuffer& AppendFormat(PCWSTR pszFormat, ...);
        StringBuffer& VaAppendFormat(PCWSTR pszFormat, va_list argList);
        StringBuffer& Replace(WCHAR, WCHAR, SIZE_T = 0, int = -1, SIZE_T* = nullptr);
        operator PCWSTR() const;
        operator PWSTR();
        PCWSTR operator &() const;
        PWCHAR operator &();
        StringBuffer& operator =(const StringBuffer&);
        StringBuffer& operator =(const String&);
        StringBuffer& operator =(PCWSTR);
        StringBuffer& operator =(int);
        StringBuffer& operator +=(const StringBuffer&);
        StringBuffer& operator +=(const String&);
        StringBuffer& operator +=(PCWSTR);
        StringBuffer& operator +=(int);
        PWCHAR get_Ptr() const;
        SIZE_T get_Cap() const;
        SIZE_T get_Len() const;
        void set_Len(SIZE_T);

        __declspec(property(get = get_Ptr)) PWCHAR Ptr;
        __declspec(property(get = get_Cap)) SIZE_T Cap;
        __declspec(property(get = get_Len, put = set_Len)) SIZE_T Len;

    protected:

        void CheckCapacity(SIZE_T);

        PWCHAR m_ptr;
        SIZE_T m_cap;
        SIZE_T m_len;
        SIZE_T m_inc;
    };

    inline StringBuffer::operator PCWSTR() const
    {
        return m_ptr ? m_ptr : L"";
    }

    inline StringBuffer::operator PWSTR()
    {
        return m_ptr;
    }

    inline PCWSTR StringBuffer::operator &() const
    {
        return m_ptr ? m_ptr : L"";
    }

    inline PWCHAR StringBuffer::operator &()
    {
        return m_ptr;
    }

    inline StringBuffer& StringBuffer::operator =(const StringBuffer& other)
    {
        return Assign(other);
    }

    inline StringBuffer& StringBuffer::operator =(const String& sz)
    {
        return Assign(sz);
    }

    inline StringBuffer& StringBuffer::operator =(PCWSTR psz)
    {
        return Assign(psz);
    }

    inline StringBuffer& StringBuffer::operator =(int c)
    {
        return Assign(c);
    }

    inline StringBuffer& StringBuffer::operator +=(const StringBuffer& other)
    {
        return Append(other);
    }

    inline StringBuffer& StringBuffer::operator +=(const String& sz)
    {
        return Append(sz);
    }

    inline StringBuffer& StringBuffer::operator +=(PCWSTR psz)
    {
        return Append(psz);
    }

    inline StringBuffer& StringBuffer::operator +=(int c)
    {
        return Append(c);
    }

    inline PWCHAR StringBuffer::get_Ptr() const
    {
        return m_ptr;
    }

    inline SIZE_T StringBuffer::get_Cap() const
    {
        return m_cap;
    }

    inline SIZE_T StringBuffer::get_Len() const
    {
        return m_len;
    }
}
