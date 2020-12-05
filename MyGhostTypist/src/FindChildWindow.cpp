#include "FindChildWindow.h"
#include "hnrt/StringStore.h"


using namespace hnrt;


FindChildWindow::FindChildWindow(PCWSTR pszClassName, PCWSTR pszWindowText)
    : m_pszClassName(StringStore::Get(pszClassName))
    , m_pszWindowText(StringStore::Get(pszWindowText))
    , m_hwnd(nullptr)
{
}


HWND FindChildWindow::Run(HWND hwnd)
{
    m_hwnd = nullptr;
    EnumChildWindows(hwnd, EnumChildCallback, reinterpret_cast<LPARAM>(this));
    return m_hwnd;
}


BOOL CALLBACK FindChildWindow::EnumChildCallback(HWND hwnd, LPARAM lParam)
{
    FindChildWindow* pThis = reinterpret_cast<FindChildWindow*>(lParam);
    WCHAR szClassName[MAX_PATH];
    WCHAR szWindowText[MAX_PATH];
    GetClassNameW(hwnd, szClassName, _countof(szClassName));
    GetWindowTextW(hwnd, szWindowText, _countof(szWindowText));
    if (!wcscmp(szClassName, pThis->m_pszClassName) && !wcscmp(szWindowText, pThis->m_pszWindowText))
    {
        pThis->m_hwnd = hwnd;
        return FALSE;
    }
    return TRUE;
}
