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
        HANDLE* operator &();
        BOOL Close();
        void set_Value(HANDLE h);
        __declspec(property(put = set_Value)) HANDLE Value;

    protected:

        HANDLE m_h;
    };

    inline WindowsHandle::WindowsHandle(HANDLE h)
        : m_h(h)
    {
    }

    inline WindowsHandle::~WindowsHandle()
    {
        HANDLE h = reinterpret_cast<HANDLE>(InterlockedExchangePointer(reinterpret_cast<PVOID*>(&m_h), nullptr));
        if (h != nullptr && h != INVALID_HANDLE_VALUE)
        {
            CloseHandle(h);
        }
    }

    inline WindowsHandle::operator HANDLE() const
    {
        return m_h;
    }
    
    inline HANDLE* WindowsHandle::operator &()
    {
        HANDLE h = reinterpret_cast<HANDLE>(InterlockedExchangePointer(reinterpret_cast<PVOID*>(&m_h), nullptr));
        if (h != nullptr && h != INVALID_HANDLE_VALUE)
        {
            CloseHandle(h);
        }
        return &m_h;
    }

    inline BOOL WindowsHandle::Close()
    {
        HANDLE h = reinterpret_cast<HANDLE>(InterlockedExchangePointer(reinterpret_cast<PVOID*>(&m_h), nullptr));
        return CloseHandle(h);
    }

    inline void WindowsHandle::set_Value(HANDLE h)
    {
        h = reinterpret_cast<HANDLE>(InterlockedExchangePointer(reinterpret_cast<PVOID*>(&m_h), h));
        if (h != nullptr && h != INVALID_HANDLE_VALUE)
        {
            CloseHandle(h);
        }
    }
}
