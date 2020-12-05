#include "pch.h"
#include "WindowFinder.h"
#include "hnrt/Args.h"
#include "hnrt/Exception.h"
#include "hnrt/WindowHelper.h"


using namespace hnrt;


WindowFinder::WindowFinder()
    : m_windowList()
    , m_dwStyleMask(WS_DISABLED | WS_VISIBLE)
    , m_dwStyleRequired(WS_VISIBLE)
    , m_Depth(0)
{
}


WindowFinder::~WindowFinder()
{
}


void WindowFinder::ParseCommandLine(int argc, wchar_t* argv[])
{
    Args args(argc, argv);
    while (args)
    {
        if (!wcscmp(*args, L"-disabled"))
        {
            m_dwStyleMask &= ~WS_DISABLED;
            m_dwStyleRequired &= ~WS_DISABLED;
            args++;
        }
        else if (!wcscmp(*args, L"-invisible"))
        {
            m_dwStyleMask &= ~WS_VISIBLE;
            m_dwStyleRequired &= ~WS_VISIBLE;
            args++;
        }
        else
        {
            throw Exception(L"Bad command line syntax.");
        }
    }
}


void WindowFinder::Run()
{
    m_Depth = 0;
    EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(this));
}


BOOL CALLBACK WindowFinder::EnumWindowsCallback(HWND hwnd, LPARAM lParam)
{
    WindowFinder* pThis = reinterpret_cast<WindowFinder*>(lParam);
    return pThis->Put(hwnd);
}


BOOL WindowFinder::Put(HWND hwnd)
{
    WindowHelper w(hwnd);
    if (!m_Depth)
    {
        if ((w.Style & m_dwStyleMask) != m_dwStyleRequired)
        {
            return TRUE;
        }
        wprintf(L"\"%s\" %s %08lX %08lX%s\n",
            w.WindowText ? w.WindowText : L"",
            w.ClassName,
            w.Style,
            w.ExStyle,
            w.IsDisabled && !w.IsVisible ? L" [DISABLED+INVISIBLE]" :
            w.IsDisabled ? L" [DISABLED]" :
            !w.IsVisible ? L" [INVISIBLE]" :
            L"");
    }
    else
    {
        for (int i = 0; i < m_Depth; i++)
        {
            wprintf(L"    ");
        }
        wprintf(L"\"%s\" %s %08lX %08lX %ld\n",
            w.WindowText ? w.WindowText : L"",
            w.ClassName,
            w.Style,
            w.ExStyle,
            w.Id);
    }
    m_Depth++;
    EnumChildWindows(w, EnumWindowsCallback, reinterpret_cast<LPARAM>(this));
    m_Depth--;
    return TRUE;
}
