#pragma once


#include <Windows.h>


namespace hnrt
{
    class ResourceString
    {
    public:

        static HINSTANCE m_hInstance;

        ResourceString(UINT uId);
        ResourceString(const ResourceString&) = delete;
        ~ResourceString();
        void operator =(const ResourceString&) = delete;
        PCWSTR get_Ptr() const;
        operator PCWSTR () const;
        PCWSTR operator & () const;
        __declspec(property(get = get_Ptr)) PCWSTR Ptr;

    private:

        PWSTR m_psz;
    };

    inline PCWSTR ResourceString::get_Ptr() const
    {
        return m_psz ? m_psz : L"";
    }

    inline ResourceString::operator PCWSTR () const
    {
        return get_Ptr();
    }

    inline PCWSTR ResourceString::operator & () const
    {
        return get_Ptr();
    }
}
