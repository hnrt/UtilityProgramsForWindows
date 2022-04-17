#include "pch.h"
#include "hnrt/WindowsHandle.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


WindowsHandle::WindowsHandle(HANDLE h)
    : m_h(h)
{
}


WindowsHandle::~WindowsHandle()
{
    HANDLE h = Interlocked<HANDLE>::ExchangePointer(&m_h, nullptr);
    if (h != nullptr && h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(h);
    }
}


void WindowsHandle::operator =(HANDLE h)
{
    h = Interlocked<HANDLE>::ExchangePointer(&m_h, h);
    if (h != nullptr && h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(h);
    }
}


HANDLE* WindowsHandle::operator &()
{
    HANDLE h = Interlocked<HANDLE>::ExchangePointer(&m_h, nullptr);
    if (h != nullptr && h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(h);
    }
    return &m_h;
}


BOOL WindowsHandle::Close()
{
    HANDLE h = Interlocked<HANDLE>::ExchangePointer(&m_h, nullptr);
    return CloseHandle(h);
}


void WindowsHandle::set_Value(HANDLE h)
{
    operator =(h);
}
