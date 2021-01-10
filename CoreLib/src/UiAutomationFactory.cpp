#include "pch.h"
#include "hnrt/UiAutomationFactory.h"
#include "UiAutomationInternal.h"


using namespace hnrt;


RefPtr<UiAutomation> UiAutomationFactory::Create(HWND hwnd)
{
    auto ptr = RefPtr<UiAutomation>(new UiAutomationInternal(hwnd));
    if (ptr->IsValid)
    {
        return ptr;
    }
    return RefPtr<UiAutomation>();
}


RefPtr<UiAutomation> UiAutomationFactory::Create(HWND hwnd, POINT pt)
{
    auto ptr1 = RefPtr<UiAutomation>(new UiAutomationInternal(hwnd));
    if (ptr1->IsValid)
    {
        auto ptr2 = ptr1->HitTest(pt);
        if (ptr2->IsValid)
        {
            return ptr2;
        }
    }
    return RefPtr<UiAutomation>();
}


RefPtr<UiAutomation> UiAutomationFactory::Create(HWND hwnd, PCWSTR pszName, DWORD dwRole)
{
    auto ptr = RefPtr<UiAutomation>(new UiAutomationInternal(hwnd));
    if (ptr->IsValid)
    {
        return ptr->Find(pszName, dwRole);
    }
    return RefPtr<UiAutomation>();
}
