#pragma once


#include <Windows.h>
#include <list>


namespace hnrt
{
    class WindowFinder
    {
    public:

        WindowFinder();
        WindowFinder(const WindowFinder&) = delete;
        ~WindowFinder();
        void operator =(const WindowFinder&) = delete;
        void ParseCommandLine(int argc, wchar_t* argv[]);
        void Run();

    private:

        BOOL Put(HWND hwnd);

        static BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);

        std::list<HWND> m_windowList;
        DWORD m_dwStyleMask;
        DWORD m_dwStyleRequired;
        int m_Depth;
    };
}
