#include "FindWindowTarget.h"
#include "TargetConstants.h"
#include "hnrt/StringStore.h"
#include "hnrt/String.h"
#include "hnrt/Buffer.h"
#include "hnrt/WindowHelper.h"


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
    HWND hwnd1 = nullptr;
    HWND hwnd2 = nullptr;
    if (!String::Compare(m_pszClassName, L"ConsoleWindowClass"))
    {
        hwnd1 = FindWindowW(m_pszClassName, m_pszWindowText);
        if (!hwnd1)
        {
            return false;
        }
        hwnd2 = WindowHelper(hwnd1).FindImeWindow();
    }
    else
    {
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
                hwnd2 = WindowHelper(hwnd1).FindChildWindow(pCnSp + cchSp, pWtSp + cchSp);
            }
        }
        else
        {
            hwnd1 = hwnd2 = FindWindowW(m_pszClassName, m_pszWindowText);
        }
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
