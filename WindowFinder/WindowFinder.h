#pragma once


#include "WindowInfo.h"
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

        static BOOL CALLBACK EnumWindowsCallback(
            _In_ HWND   hwnd,
            _In_ LPARAM lParam);
        static void Put(const WindowInfo* pInfo, int depth);

        std::list<WindowInfo*> m_windowList;
        DWORD m_dwStyleMask;
        DWORD m_dwStyleRequired;
    };
}