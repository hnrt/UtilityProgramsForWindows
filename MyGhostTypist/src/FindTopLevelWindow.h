#pragma once


#include <Windows.h>


namespace hnrt
{
    class FindTopLevelWindow
    {
    public:

        FindTopLevelWindow(PCWSTR pszClassName, PCWSTR pszWindowText, DWORD dwProcessId);
        FindTopLevelWindow(const FindTopLevelWindow&) = delete;
        ~FindTopLevelWindow() = default;
        void operator =(const FindTopLevelWindow&) = delete;
        HWND Run();

    private:

        static BOOL CALLBACK EnumTopLevelCallback(HWND hwnd, LPARAM lParam);

        PCWSTR m_pszClassName;
        PCWSTR m_pszWindowText;
        DWORD m_dwProcessId;
        HWND m_hwnd;
    };
}
