#pragma once

#include <Windows.h>

namespace hnrt
{
    class MultibyteString
    {
    public:

        MultibyteString();
        MultibyteString(const MultibyteString&);
        MultibyteString(PCSTR, SSIZE_T = -1);
        MultibyteString(UINT, PCWSTR, SSIZE_T = -1);
        ~MultibyteString();
        operator PCSTR() const;
        MultibyteString& operator =(const MultibyteString&);

        PCSTR get_ptr() const;
        SIZE_T get_len() const;

        __declspec(property(get = get_ptr)) PCSTR Ptr;
        __declspec(property(get = get_len)) SIZE_T Len;

    private:

        PSTR m_psz;
    };

    inline MultibyteString::operator PCSTR() const
    {
        return Ptr;
    }

    inline PCSTR MultibyteString::get_ptr() const
    {
        return m_psz ? m_psz : "";
    }
}
