#include "pch.h"
#include "hnrt/WindowClass.h"


using namespace hnrt;


WindowClass::WindowClass(PCWSTR pszName)
	: WNDCLASSEXW()
{
    cbSize = sizeof(WNDCLASSEXW);
    lpszClassName = pszName;
}


WindowClass& WindowClass::SetStyle(UINT value)
{
    style = value;
    return *this;
}


WindowClass& WindowClass::SetWindowProcedure(WNDPROC value)
{
    lpfnWndProc = value;
    return *this;
}


WindowClass& WindowClass::SetClassExtra(int value)
{
    cbClsExtra = value;
    return *this;
}


WindowClass& WindowClass::SetWindowExtra(int value)
{
    cbWndExtra = value;
    return *this;
}


WindowClass& WindowClass::SetInstance(HINSTANCE value)
{
    hInstance = value;
    return *this;
}


WindowClass& WindowClass::SetIcon(HICON value)
{
    hIcon = value;
    return *this;
}


WindowClass& WindowClass::SetCursor(HCURSOR value)
{
    hCursor = value;
    return *this;
}


WindowClass& WindowClass::SetBackground(int value)
{
    hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(value));
    return *this;
}


WindowClass& WindowClass::SetMenuName(UINT value)
{
    lpszMenuName = MAKEINTRESOURCEW(value);
    return *this;
}


WindowClass& WindowClass::SetSmallIcon(HICON value)
{
    hIconSm = value;
    return *this;
}


ATOM WindowClass::Register()
{
    return RegisterClassExW(this);
}


BOOL WindowClass::Unregister()
{
    return UnregisterClassW(lpszClassName, hInstance);
}