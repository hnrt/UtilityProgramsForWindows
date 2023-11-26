#pragma once


#include <Windows.h>


namespace hnrt
{
    class RefMbs;

    class StringUTF8
    {
    public:

        StringUTF8();
        StringUTF8(const StringUTF8&);
        StringUTF8(PCSTR, INT_PTR = -1);
        StringUTF8(PCWSTR, INT_PTR = -1);
        ~StringUTF8();
        operator PCSTR() const;
        operator bool() const;
        StringUTF8& operator =(const StringUTF8&);
        StringUTF8& operator +=(const StringUTF8&);
        bool operator ==(const StringUTF8&) const;
        bool operator !=(const StringUTF8&) const;
        bool operator <(const StringUTF8&) const;
        bool operator <=(const StringUTF8&) const;
        bool operator >(const StringUTF8&) const;
        bool operator >=(const StringUTF8&) const;
        StringUTF8 operator +(const StringUTF8&) const;
        PCSTR get_ptr() const;
        size_t get_len() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len)) size_t Len;

    private:

        StringUTF8(RefMbs*);

        RefMbs* m_ptr;
    };

    inline StringUTF8::operator PCSTR() const
    {
        return Ptr;
    }

    inline StringUTF8::operator bool() const
    {
        return m_ptr != nullptr;
    }
}
