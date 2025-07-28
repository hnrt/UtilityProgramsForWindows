#pragma once

#include <Windows.h>

namespace hnrt
{
    class MultibyteString
    {
    private:

        PSTR m_psz;

    public:

        MultibyteString();
        MultibyteString(const MultibyteString&);
        MultibyteString(PCSTR, SSIZE_T = -1);
        MultibyteString(UINT, PCWSTR, SSIZE_T = -1);
        ~MultibyteString();

        operator PCSTR() const;
        MultibyteString& operator =(const MultibyteString&);

    public:

        PCSTR get_Ptr() const;
        SIZE_T get_Len() const;

        __declspec(property(get = get_Ptr)) PCSTR Ptr;
        __declspec(property(get = get_Len)) SIZE_T Len;
    };

    inline MultibyteString::operator PCSTR() const
    {
        return Ptr;
    }

    inline PCSTR MultibyteString::get_Ptr() const
    {
        return m_psz ? m_psz : "";
    }

    class String;

    MultibyteString ToAcp(const String&);

    MultibyteString ToUTF8(const String&);
}
