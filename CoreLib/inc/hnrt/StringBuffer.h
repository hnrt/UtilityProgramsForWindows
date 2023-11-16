#pragma once


#include <Windows.h>
#include <stdarg.h>


namespace hnrt
{
    class StringBuffer
    {
    public:

        StringBuffer(size_t capacity);
        StringBuffer(const StringBuffer&) = delete;
        ~StringBuffer();
        void operator =(const StringBuffer&) = delete;
        StringBuffer& operator +=(PCWSTR psz);
        PWCHAR get_Ptr() const;
        size_t get_Cap() const;
        size_t get_Len() const;
        void set_Len(size_t value);
        operator PCWSTR () const;
        operator PWSTR ();
        PCWSTR operator &() const;
        PWCHAR operator &();
        PWCHAR Resize(size_t capacity);
        PWCHAR Detach();
        StringBuffer& Append(PCWSTR psz);
        StringBuffer& Append(PCWSTR psz, size_t cch);
        StringBuffer& AppendFormat(PCWSTR pszFormat, ...);
        StringBuffer& VaAppendFormat(PCWSTR pszFormat, va_list argList);

        __declspec(property(get = get_Ptr)) PWCHAR Ptr;
        __declspec(property(get = get_Cap)) size_t Cap;
        __declspec(property(get = get_Len, put = set_Len)) size_t Len;

    protected:

        PWCHAR m_ptr;
        size_t m_cap;
        size_t m_len;
    };

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

    inline StringBuffer::operator PCWSTR () const
    {
        return m_ptr;
    }

    inline StringBuffer::operator PWSTR ()
    {
        return m_ptr;
    }

    inline PCWSTR StringBuffer::operator &() const
    {
        return m_ptr;
    }

    inline PWCHAR StringBuffer::operator &()
    {
        return m_ptr;
    }
}
