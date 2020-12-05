#pragma once


#include <Windows.h>


namespace hnrt
{
    class FindChildWindow
    {
    public:

        FindChildWindow(PCWSTR pszClassName, PCWSTR pszWindowText);
        FindChildWindow(const FindChildWindow&) = delete;
        ~FindChildWindow() = default;
        void operator =(const FindChildWindow&) = delete;
        HWND Run(HWND hwnd);

    private:

        static BOOL CALLBACK EnumChildCallback(HWND hwnd, LPARAM lParam);

        PCWSTR m_pszClassName;
        PCWSTR m_pszWindowText;
        HWND m_hwnd;
    };
}
