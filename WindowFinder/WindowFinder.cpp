#include "pch.h"
#include "WindowFinder.h"
#include "hnrt/Args.h"
#include "hnrt/Exception.h"


using namespace hnrt;


WindowFinder::WindowFinder()
    : m_windowList()
    , m_dwStyleMask(WS_DISABLED | WS_VISIBLE)
    , m_dwStyleRequired(WS_VISIBLE)
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
    EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(this));

    for (std::list<WindowInfo*>::const_iterator iter = m_windowList.begin(); iter != m_windowList.end(); iter++)
    {
        WindowInfo* pInfo = *iter;
        if ((pInfo->Style & m_dwStyleMask) != m_dwStyleRequired)
        {
            continue;
        }
        pInfo->FindChildren();
        wprintf(L"\"%s\" %s %08lX %08lX%s\n",
            pInfo->Text ? (*iter)->Text : L"",
            pInfo->ClassName,
            pInfo->Style,
            pInfo->ExStyle,
            pInfo->IsDisabled && !pInfo->IsVisible ? L" [DISABLED+INVISIBLE]" :
            pInfo->IsDisabled ? L" [DISABLED]" :
            !pInfo->IsVisible ? L" [INVISIBLE]" :
            L"");
        for (std::list<WindowInfo*>::const_iterator iter = pInfo->Begin(); iter != pInfo->End(); iter++)
        {
            Put(*iter, 1);
        }
    }
}


BOOL CALLBACK WindowFinder::EnumWindowsCallback(
    _In_ HWND   hwnd,
    _In_ LPARAM lParam)
{
    WindowFinder* pThis = reinterpret_cast<WindowFinder*>(lParam);
    pThis->m_windowList.push_back(new WindowInfo(hwnd));
    return TRUE;
}


void WindowFinder::Put(const WindowInfo* pInfo, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        wprintf(L"    ");
    }
    wprintf(L"\"%s\" %s %08lX %08lX %ld\n",
        pInfo->Text ? pInfo->Text : L"",
        pInfo->ClassName,
        pInfo->Style,
        pInfo->ExStyle,
        pInfo->Id);
    for (std::list<WindowInfo*>::const_iterator iter = pInfo->Begin(); iter != pInfo->End(); iter++)
    {
        Put(*iter, depth + 1);
    }
}
