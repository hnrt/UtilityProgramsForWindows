#pragma once


#include <Windows.h>


namespace hnrt
{
    class WindowsMessage
    {
    public:

        WindowsMessage(UINT uMsg);
        WindowsMessage(const WindowsMessage&);
        ~WindowsMessage() = default;
        WindowsMessage& operator =(const WindowsMessage&);
        UINT get_Value() const;
        operator UINT() const;
        PCWSTR get_Name() const;

        __declspec(property(get = get_Value)) UINT Value;
        __declspec(property(get = get_Name)) PCWSTR Name;

    private:

        UINT m_value;
    };

    inline WindowsMessage::WindowsMessage(UINT uMsg)
        : m_value(uMsg)
    {
    }

    inline WindowsMessage::WindowsMessage(const WindowsMessage& src)
        : m_value(src.m_value)
    {
    }

    inline WindowsMessage& WindowsMessage::operator =(const WindowsMessage& src)
    {
        m_value = src.m_value;
        return *this;
    }

    inline UINT WindowsMessage::get_Value() const
    {
        return m_value;
    }

    inline WindowsMessage::operator UINT() const
    {
        return get_Value();
    }
}
