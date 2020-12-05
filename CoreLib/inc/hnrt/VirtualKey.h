#pragma once


#include <Windows.h>


namespace hnrt
{
    class VirtualKey
    {
    public:

        VirtualKey(unsigned char value);
        VirtualKey(WORD value);
        VirtualKey(WPARAM value);
        VirtualKey(const VirtualKey&);
        VirtualKey(PCWSTR psz);
        ~VirtualKey() = default;
        VirtualKey& operator =(const VirtualKey&);
        unsigned char get_Value() const;
        operator unsigned char() const;
        PCWSTR get_Name() const;

        __declspec(property(get = get_Value)) unsigned char Value;
        __declspec(property(get = get_Name)) PCWSTR Name;

    private:

        unsigned char m_value;
    };

    inline VirtualKey::VirtualKey(unsigned char value)
        : m_value(value)
    {
    }

    inline VirtualKey::VirtualKey(WORD value)
        : m_value(static_cast<unsigned char>(value & 0xFF))
    {
    }

    inline VirtualKey::VirtualKey(WPARAM value)
        : m_value(static_cast<unsigned char>(value & 0xFF))
    {
    }

    inline VirtualKey::VirtualKey(const VirtualKey& src)
        : m_value(src.m_value)
    {
    }

    inline VirtualKey& VirtualKey::operator =(const VirtualKey& src)
    {
        m_value = src.m_value;
        return *this;
    }

    inline unsigned char VirtualKey::get_Value() const
    {
        return m_value;
    }

    inline VirtualKey::operator unsigned char() const
    {
        return get_Value();
    }
}
