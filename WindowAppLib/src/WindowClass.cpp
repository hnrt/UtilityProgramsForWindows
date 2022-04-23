#include "pch.h"
#include "hnrt/WindowClass.h"


using namespace hnrt;


WindowClass::WindowClass(PCWSTR pszName)
	: m_wc()
{
    ZeroMemory(&m_wc, sizeof(m_wc));
    m_wc.cbSize = sizeof(m_wc);
    m_wc.lpszClassName = pszName;
}


WindowClass& WindowClass::SetStyle(UINT value)
{
    m_wc.style = value;
    return *this;
}


WindowClass& WindowClass::SetWindowProcedure(WNDPROC value)
{
    m_wc.lpfnWndProc = value;
    return *this;
}


WindowClass& WindowClass::SetClassExtra(int value)
{
    m_wc.cbClsExtra = value;
    return *this;
}


WindowClass& WindowClass::SetWindowExtra(int value)
{
    m_wc.cbWndExtra = value;
    return *this;
}


WindowClass& WindowClass::SetInstance(HINSTANCE value)
{
    m_wc.hInstance = value;
    return *this;
}


WindowClass& WindowClass::SetIcon(HICON value)
{
    m_wc.hIcon = value;
    return *this;
}


WindowClass& WindowClass::SetCursor(HCURSOR value)
{
    m_wc.hCursor = value;
    return *this;
}


WindowClass& WindowClass::SetBackground(int value)
{
    m_wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(value));
    return *this;
}


WindowClass& WindowClass::SetMenuName(UINT value)
{
    m_wc.lpszMenuName = MAKEINTRESOURCEW(value);
    return *this;
}


WindowClass& WindowClass::SetSmallIcon(HICON value)
{
    m_wc.hIconSm = value;
    return *this;
}


ATOM WindowClass::Register()
{
    return RegisterClassExW(&m_wc);
}
