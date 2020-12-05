#include "FindWindowTarget.h"
#include <TlHelp32.h>
#include "TargetConstants.h"
#include "hnrt/StringStore.h"
#include "hnrt/String.h"
#include "hnrt/Buffer.h"
#include "hnrt/WindowsHandle.h"
#include "FindChildWindow.h"
#include "FindTopLevelWindow.h"


using namespace hnrt;


#define SEPARATOR L"@@@###@@@"


FindWindowTarget::FindWindowTarget(PCWSTR pszName, PCWSTR pszClassName, PCWSTR pszWindowText)
    : ActionTarget(FINDWINDOW_TARGET_TYPE, pszName)
    , m_pszClassName(StringStore::Get(pszClassName))
    , m_pszWindowText(StringStore::Get(pszWindowText))
{
}


RefPtr<Target> FindWindowTarget::Clone() const
{
    FindWindowTarget* pCloned = new FindWindowTarget(m_pszName, m_pszClassName, m_pszWindowText);
    pCloned->m_bIsVisible = m_bIsVisible;
    for (size_t index = 0; index < this->Count; index++)
    {
        pCloned->Append((*this)[index]);
    }
    return RefPtr<Target>(pCloned);
}


void FindWindowTarget::Set(PCWSTR pszClassName, PCWSTR pszWindowText)
{
    m_pszClassName = StringStore::Get(pszClassName);
    m_pszWindowText = StringStore::Get(pszWindowText);
    if (m_pCallback)
    {
        m_pCallback->OnTargetUpdate(*this);
    }
}


void FindWindowTarget::Set(PCWSTR pszClassName, PCWSTR pszWindowText, PCWSTR pszClassName2, PCWSTR pszWindowText2)
{
    m_pszClassName = String::Format(L"%s%s%s", pszClassName, SEPARATOR, pszClassName2);
    m_pszWindowText = String::Format(L"%s%s%s", pszWindowText, SEPARATOR, pszWindowText2);
    if (m_pCallback)
    {
        m_pCallback->OnTargetUpdate(*this);
    }
}


bool FindWindowTarget::Find(HWND* phwnd1, HWND* phwnd2) const
{
    if (!wcscmp(m_pszClassName, L"ConsoleWindowClass"))
    {
        return FindConsole(phwnd1, phwnd2);
    }
    HWND hwnd1 = nullptr;
    HWND hwnd2 = nullptr;
    PCWSTR pCnSp = wcsstr(m_pszClassName, SEPARATOR);
    PCWSTR pWtSp = wcsstr(m_pszWindowText, SEPARATOR);
    if (pCnSp && pWtSp)
    {
        Buffer<WCHAR> szCn(pCnSp - m_pszClassName + 1);
        Buffer<WCHAR> szWt(pWtSp - m_pszWindowText + 1);
        wcsncpy_s(szCn, szCn.Len, m_pszClassName, szCn.Len - 1);
        wcsncpy_s(szWt, szWt.Len, m_pszWindowText, szWt.Len - 1);
        HWND hwnd1 = FindWindowW(szCn, szWt);
        if (hwnd1)
        {
            size_t cchSp = wcslen(SEPARATOR);
            hwnd2 = FindChildWindow(pCnSp + cchSp, pWtSp + cchSp).Run(hwnd1);
        }
    }
    else
    {
        hwnd1 = hwnd2 = FindWindowW(m_pszClassName, m_pszWindowText);
    }
    if (phwnd1)
    {
        *phwnd1 = hwnd1;
    }
    if (phwnd2)
    {
        *phwnd2 = hwnd2;
    }
    return hwnd1 && hwnd2;
}


bool FindWindowTarget::FindConsole(HWND* phwnd1, HWND* phwnd2) const
{
    HWND hwnd1 = FindWindowW(m_pszClassName, m_pszWindowText);
    if (!hwnd1)
    {
        return false;
    }
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hwnd1, &dwProcessId);
    WindowsHandle hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(pe);
    if (!Process32FirstW(hSnapshot, &pe))
    {
        return false;
    }
    while (pe.th32ParentProcessID != dwProcessId)
    {
        if (!Process32NextW(hSnapshot, &pe))
        {
            return false;
        }
    }
    HWND hwnd2 = FindTopLevelWindow(L"IME", L"Default IME", pe.th32ProcessID).Run();
    if (!hwnd2)
    {
        return false;
    }
    if (phwnd1)
    {
        *phwnd1 = hwnd1;
    }
    if (phwnd2)
    {
        *phwnd2 = hwnd2;
    }
    return true;
}
