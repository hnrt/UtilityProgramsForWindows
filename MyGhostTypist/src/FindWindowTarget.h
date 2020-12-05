#pragma once


#include <Windows.h>
#include "ActionTarget.h"


namespace hnrt
{
    class FindWindowTarget
        : public ActionTarget
    {
    public:

        FindWindowTarget(PCWSTR pszName, PCWSTR pszClassName, PCWSTR pszWindowText);
        FindWindowTarget(const FindWindowTarget&) = delete;
        virtual ~FindWindowTarget() = default;
        void operator =(const FindWindowTarget&) = delete;
        virtual RefPtr<Target> Clone() const;
        void Set(PCWSTR pszClassName, PCWSTR pszWindowText);
        void Set(PCWSTR pszClassName, PCWSTR pszWindowText, PCWSTR pszClassName2, PCWSTR pszWindowText2);
        bool Find(HWND* phwnd1 = nullptr, HWND* phwnd2 = nullptr) const;
        bool FindConsole(HWND* phwnd1 = nullptr, HWND* phwnd2 = nullptr) const;
        PCWSTR get_ClassName() const;
        PCWSTR get_WindowText() const;
        __declspec(property(get = get_ClassName)) PCWSTR ClassName;
        __declspec(property(get = get_WindowText)) PCWSTR WindowText;

    protected:

        PCWSTR m_pszClassName;
        PCWSTR m_pszWindowText;
    };

    inline PCWSTR FindWindowTarget::get_ClassName() const
    {
        return m_pszClassName;
    }

    inline PCWSTR FindWindowTarget::get_WindowText() const
    {
        return m_pszWindowText;
    }
}
