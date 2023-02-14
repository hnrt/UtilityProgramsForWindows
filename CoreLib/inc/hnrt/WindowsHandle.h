#pragma once


#include <Windows.h>


namespace hnrt
{
    class WindowsHandle
    {
    public:

        WindowsHandle(HANDLE h = nullptr);
        WindowsHandle(const WindowsHandle&) = delete;
        ~WindowsHandle();
        void operator =(const WindowsHandle&) = delete;
        operator HANDLE() const;
        void operator =(HANDLE h);
        HANDLE* operator &();
        BOOL Close();
        HANDLE get_Value() const;
        void set_Value(HANDLE h);
        bool get_Validity() const;

        __declspec(property(get = get_Value, put = set_Value)) HANDLE Value;
        __declspec(property(get = get_Validity)) bool isValid;

    protected:

        HANDLE m_h;
    };

    inline WindowsHandle::operator HANDLE() const
    {
        return m_h;
    }

    inline HANDLE WindowsHandle::get_Value() const
    {
        return m_h;
    }

    inline bool WindowsHandle::get_Validity() const
    {
        return m_h != nullptr && m_h != INVALID_HANDLE_VALUE;
    }
}
