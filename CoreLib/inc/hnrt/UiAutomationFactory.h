#pragma once


#include "hnrt/UiAutomation.h"


namespace hnrt
{
    class UiAutomationFactory
    {
    public:

        static RefPtr<UiAutomation> Create(HWND hwnd);
        static RefPtr<UiAutomation> Create(HWND hwnd, POINT pt);
        static RefPtr<UiAutomation> Create(HWND hwnd, PCWSTR pszName, DWORD dwRole);

    };
}
