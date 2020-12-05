#include "pch.h"
#include "hnrt/WindowHelper.h"
#include "hnrt/String.h"
#include "hnrt/WindowsHandle.h"
#include <TlHelp32.h>


using namespace hnrt;


void WindowHelper::set_ClassName(PCWSTR pszClassName)
{
    m_ClassName.Resize(wcslen(pszClassName) + 1);
    wcscpy_s(m_ClassName, m_ClassName.Len, pszClassName);
}


void WindowHelper::set_WindowText(PCWSTR pszWindowText)
{
    m_WindowText.Resize(wcslen(pszWindowText) + 1);
    wcscpy_s(m_WindowText, m_WindowText.Len, pszWindowText);
}


static BOOL CALLBACK EnumTopLevelCallback(HWND hwnd, LPARAM lParam)
{
    WindowHelper* pThis = reinterpret_cast<WindowHelper*>(lParam);
    WindowHelper cur = hwnd;
    if (!String::Compare(cur.ClassName, pThis->ClassName) && !String::Compare(cur.WindowText, pThis->WindowText))
    {
        if (cur.ProcessId == pThis->ProcessId)
        {
            *pThis = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}


HWND WindowHelper::FindTopLevelWindow(PCWSTR pszClassName, PCWSTR pszWindowText, DWORD dwProcessId)
{
    WindowHelper w;
    w.ClassName = pszClassName;
    w.WindowText = pszWindowText;
    w.ProcessId = dwProcessId;
    EnumWindows(EnumTopLevelCallback, reinterpret_cast<LPARAM>(&w));
    return w;
}


static BOOL CALLBACK EnumChildCallback(HWND hwnd, LPARAM lParam)
{
    WindowHelper* pThis = reinterpret_cast<WindowHelper*>(lParam);
    WindowHelper cur = hwnd;
    if (!String::Compare(cur.ClassName, pThis->ClassName) && !String::Compare(cur.WindowText, pThis->WindowText))
    {
        *pThis = hwnd;
        return FALSE;
    }
    return TRUE;
}


HWND WindowHelper::FindChildWindow(PCWSTR pszClassName, PCWSTR pszWindowText)
{
    WindowHelper w;
    w.ClassName = pszClassName;
    w.WindowText = pszWindowText;
    EnumChildWindows(m_hwnd, EnumChildCallback, reinterpret_cast<LPARAM>(&w));
    return w;
}


HWND WindowHelper::FindImeWindow() const
{
    WindowHelper w;
    WindowsHandle hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(pe);
    if (!Process32FirstW(hSnapshot, &pe))
    {
        return nullptr;
    }
    while (pe.th32ParentProcessID != ProcessId)
    {
        if (!Process32NextW(hSnapshot, &pe))
        {
            return nullptr;
        }
    }
    return FindTopLevelWindow(L"IME", L"Default IME", pe.th32ProcessID);
}
