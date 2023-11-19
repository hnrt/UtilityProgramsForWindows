#pragma once


#include <Windows.h>
#include <stdarg.h>


namespace hnrt
{
    class String;

    class StringBuffer
    {
    public:

        StringBuffer(const StringBuffer&);
        StringBuffer(size_t);
        StringBuffer(INT_PTR, PCWSTR);
        StringBuffer(const String&);
        ~StringBuffer();
        PWCHAR Detach();
        StringBuffer& Resize(size_t);
        StringBuffer& Assign(const StringBuffer&);
        StringBuffer& Assign(const String&);
        StringBuffer& Assign(PCWSTR, INT_PTR = -1);
        StringBuffer& Format(PCWSTR pszFormat, ...);
        StringBuffer& VaFormat(PCWSTR pszFormat, va_list argList);
        StringBuffer& Append(const StringBuffer&);
        StringBuffer& Append(const String&);
        StringBuffer& Append(PCWSTR, INT_PTR = -1);
        StringBuffer& AppendFormat(PCWSTR pszFormat, ...);
        StringBuffer& VaAppendFormat(PCWSTR pszFormat, va_list argList);
        operator PCWSTR() const;
        operator PWSTR();
        PCWSTR operator &() const;
        PWCHAR operator &();
        StringBuffer& operator =(const StringBuffer&);
        StringBuffer& operator =(const String&);
        StringBuffer& operator =(PCWSTR);
        StringBuffer& operator +=(const StringBuffer&);
        StringBuffer& operator +=(const String&);
        StringBuffer& operator +=(PCWSTR);
        PWCHAR get_Ptr() const;
        size_t get_Cap() const;
        size_t get_Len() const;
        void set_Len(size_t);

        __declspec(property(get = get_Ptr)) PWCHAR Ptr;
        __declspec(property(get = get_Cap)) size_t Cap;
        __declspec(property(get = get_Len, put = set_Len)) size_t Len;

    protected:

        void CheckCapacity(size_t);

        PWCHAR m_ptr;
        size_t m_cap;
        size_t m_len;
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

    inline PWCHAR StringBuffer::get_Ptr() const
    {
        return m_ptr;
    }

    inline size_t StringBuffer::get_Cap() const
    {
        return m_cap;
    }

    inline size_t StringBuffer::get_Len() const
    {
        return m_len;
    }
}
