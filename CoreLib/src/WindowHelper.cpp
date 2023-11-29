#include "pch.h"
#include "hnrt/WindowHelper.h"
#include "hnrt/WindowsHandle.h"
#include "hnrt/Buffer.h"
#include <TlHelp32.h>
#include <list>


using namespace hnrt;


const String& WindowHelper::get_ClassName() const
{
    if (!m_szClassName.Len)
    {
        if (m_hwnd)
        {
            for (int size = 16; ; size *= 2)
            {
                Buffer<WCHAR> buf(size);
                wmemset(buf, 0, size);
                int rc = GetClassNameW(m_hwnd, buf, size);
                if (rc < static_cast<int>(buf.Len) - 1)
                {
                    m_szClassName = buf;
                    break;
                }
            }
        }
    }
    return m_szClassName;
}


void WindowHelper::set_ClassName(const String& szClassName)
{
    m_szClassName = szClassName;
}


const String& WindowHelper::get_WindowText() const
{
    if (!m_szWindowText.Len)
    {
        if (m_hwnd)
        {
            int size = GetWindowTextLengthW(m_hwnd) + 1;
            Buffer<WCHAR> buf(size);
            wmemset(buf, 0, size);
            GetWindowTextW(m_hwnd, buf, size);
            m_szWindowText = String(buf);
        }
    }
    return m_szWindowText;
}


void WindowHelper::set_WindowText(const String& szWindowText)
{
    m_szWindowText = szWindowText;
}


static BOOL CALLBACK EnumTopLevelCallback(HWND hwnd, LPARAM lParam)
{
    WindowHelper* pThis = reinterpret_cast<WindowHelper*>(lParam);
    WindowHelper cur = hwnd;
    if (cur.ClassName == pThis->ClassName && cur.WindowText == pThis->WindowText)
    {
        if (cur.ProcessId == pThis->ProcessId)
        {
            *pThis = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}


HWND WindowHelper::FindTopLevelWindow(const String& szClassName, const String& szWindowText, DWORD dwProcessId)
{
    WindowHelper w;
    w.ClassName = szClassName;
    w.WindowText = szWindowText;
    w.ProcessId = dwProcessId;
    EnumWindows(EnumTopLevelCallback, reinterpret_cast<LPARAM>(&w));
    return w;
}


static BOOL CALLBACK EnumChildCallback(HWND hwnd, LPARAM lParam)
{
    WindowHelper* pThis = reinterpret_cast<WindowHelper*>(lParam);
    WindowHelper cur = hwnd;
    if (cur.ClassName == pThis->ClassName && cur.WindowText == pThis->WindowText)
    {
        *pThis = hwnd;
        return FALSE;
    }
    return TRUE;
}


HWND WindowHelper::FindChildWindow(const String& szClassName, const String& szWindowText)
{
    WindowHelper w;
    w.ClassName = szClassName;
    w.WindowText = szWindowText;
    EnumChildWindows(m_hwnd, EnumChildCallback, reinterpret_cast<LPARAM>(&w));
    return w;
}


struct EnumChildCallback2Context
{
    WindowHelper w;
    std::list<HWND> hh;
};


static BOOL CALLBACK EnumChildCallback2(HWND hwnd, LPARAM lParam)
{
    EnumChildCallback2Context* pCC = reinterpret_cast<EnumChildCallback2Context*>(lParam);
    WindowHelper cur = hwnd;
    if (cur.ClassName == pCC->w.ClassName && cur.WindowText == pCC->w.WindowText)
    {
        pCC->w = hwnd;
        return FALSE;
    }
    pCC->hh.push_back(hwnd);
    return TRUE;
}


HWND WindowHelper::FindChildWindow2(const String& szClassName, const String& szWindowText)
{
    EnumChildCallback2Context cc;
    cc.w.ClassName = szClassName;
    cc.w.WindowText = szWindowText;
    EnumChildWindows(m_hwnd, EnumChildCallback2, reinterpret_cast<LPARAM>(&cc));
    while (!cc.w.m_hwnd && cc.hh.size())
    {
        HWND hwnd = cc.hh.front();
        cc.hh.pop_front();
        EnumChildWindows(hwnd, EnumChildCallback2, reinterpret_cast<LPARAM>(&cc));
    }
    return cc.w.m_hwnd;
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
