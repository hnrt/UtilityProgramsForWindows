#include "pch.h"
#include "WindowInfo.h"
#include "hnrt/Heap.h"


using namespace hnrt;


static wchar_t* GetClazzName(HWND hwnd)
{
    int size = 32;
    while (true)
    {
        wchar_t* psz = Allocate<wchar_t>(size);
        int rc = GetClassNameW(hwnd, psz, size);
        if (!rc)
        {
            free(psz);
            return nullptr;
        }
        else if (rc < size - 1)
        {
            return Allocate(psz, wcslen(psz) + 1);
        }
        free(psz);
        size += size < 65536 ? size : 65536;
    }
}


static DWORD GetStyle(HWND hwnd)
{
    return GetWindowLong(hwnd, GWL_STYLE);
}


static DWORD GetExStyle(HWND hwnd)
{
    return GetWindowLong(hwnd, GWL_EXSTYLE);
}


static DWORD GetId(HWND hwnd)
{
    return GetWindowLong(hwnd, GWL_ID);
}


static wchar_t* GetText(HWND hwnd)
{
    int size = 32;
    while (true)
    {
        wchar_t* psz = Allocate<wchar_t>(size);
        int rc = GetWindowTextW(hwnd, psz, size);
        if (!rc)
        {
            free(psz);
            return nullptr;
        }
        else if (rc < size - 1)
        {
            return Allocate(psz, wcslen(psz) + 1);
        }
        free(psz);
        size += size < 65536 ? size : 65536;
    }
}


WindowInfo::WindowInfo(HWND hwnd)
    : m_hwnd(hwnd)
    , m_pszClassName(GetClazzName(hwnd))
    , m_dwStyle(GetStyle(hwnd))
    , m_dwExStyle(GetExStyle(hwnd))
    , m_dwId(GetId(hwnd))
    , m_pszText(GetText(hwnd))
    , m_children()
{
}


WindowInfo::~WindowInfo()
{
    free(m_pszClassName);
    free(m_pszText);
}


void WindowInfo::FindChildren()
{
    m_children.clear();
    EnumChildWindows(m_hwnd, EnumChildCallback, reinterpret_cast<LPARAM>(this));
}


BOOL CALLBACK WindowInfo::EnumChildCallback(
    _In_ HWND   hwnd,
    _In_ LPARAM lParam)
{
    WindowInfo* pThis = reinterpret_cast<WindowInfo*>(lParam);
    WindowInfo* pChild = new WindowInfo(hwnd);
    pThis->m_children.push_back(pChild);
    pChild->FindChildren();
    return TRUE;
}
