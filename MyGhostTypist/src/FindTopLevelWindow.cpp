#include "FindTopLevelWindow.h"
#include "hnrt/StringStore.h"


using namespace hnrt;


FindTopLevelWindow::FindTopLevelWindow(PCWSTR pszClassName, PCWSTR pszWindowText, DWORD dwProcessId)
    : m_pszClassName(StringStore::Get(pszClassName))
    , m_pszWindowText(StringStore::Get(pszWindowText))
    , m_dwProcessId(dwProcessId)
    , m_hwnd(nullptr)
{
}


HWND FindTopLevelWindow::Run()
{
    m_hwnd = nullptr;
    EnumWindows(EnumTopLevelCallback, reinterpret_cast<LPARAM>(this));
    return m_hwnd;
}


BOOL CALLBACK FindTopLevelWindow::EnumTopLevelCallback(HWND hwnd, LPARAM lParam)
{
    FindTopLevelWindow* pThis = reinterpret_cast<FindTopLevelWindow*>(lParam);
    WCHAR szClassName[MAX_PATH];
    WCHAR szWindowText[MAX_PATH];
    GetClassNameW(hwnd, szClassName, _countof(szClassName));
    GetWindowTextW(hwnd, szWindowText, _countof(szWindowText));
    if (!wcscmp(szClassName, pThis->m_pszClassName) && !wcscmp(szWindowText, pThis->m_pszWindowText))
    {
        DWORD dwProcessId = 0;
        DWORD dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId);
        if (dwProcessId == pThis->m_dwProcessId)
        {
            pThis->m_hwnd = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}
