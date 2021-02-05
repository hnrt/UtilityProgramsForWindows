#pragma once


#include <Windows.h>
#include "hnrt/Buffer.h"


namespace hnrt
{
    class WindowHelper
    {
    public:

        WindowHelper(HWND hwnd = nullptr);
        WindowHelper(HWND hwnd, int nDlgItem);
        WindowHelper(const WindowHelper& rhs);
        ~WindowHelper() = default;
        WindowHelper& operator =(const WindowHelper& rhs);
        WindowHelper& operator =(HWND hwnd);
        operator HWND() const;
        DWORD get_ProcessId() const;
        void set_ProcessId(DWORD dwProcessId);
        DWORD get_ThreadId() const;
        PCWSTR get_ClassName() const;
        void set_ClassName(PCWSTR pszClassName);
        PCWSTR get_WindowText() const;
        void set_WindowText(PCWSTR pszWindowText);
        WindowHelper get_Parent() const;
        DWORD get_Style() const;
        DWORD get_ExStyle() const;
        DWORD get_Id() const;
        bool get_IsDisabled() const;
        bool get_IsVisible() const;
        bool get_IsChild() const;
        __declspec(property(get = get_ProcessId, put = set_ProcessId)) DWORD ProcessId;
        __declspec(property(get = get_ThreadId)) DWORD ThreadId;
        __declspec(property(get = get_ClassName, put = set_ClassName)) PCWSTR ClassName;
        __declspec(property(get = get_WindowText, put = set_WindowText)) PCWSTR WindowText;
        __declspec(property(get = get_Parent)) WindowHelper Parent;
        __declspec(property(get = get_Style)) DWORD Style;
        __declspec(property(get = get_ExStyle)) DWORD ExStyle;
        __declspec(property(get = get_Id)) DWORD Id;
        __declspec(property(get = get_IsDisabled)) bool IsDisabled;
        __declspec(property(get = get_IsVisible)) bool IsVisible;
        __declspec(property(get = get_IsChild)) bool IsChild;

        static HWND FindTopLevelWindow(PCWSTR pszClassName, PCWSTR pszWindowText, DWORD dwProcessId);
        HWND FindChildWindow(PCWSTR pszClassName, PCWSTR pszWindowText);
        HWND FindChildWindow2(PCWSTR pszClassName, PCWSTR pszWindowText);
        HWND FindImeWindow() const;

    private:

        HWND m_hwnd;
        mutable DWORD m_dwProcessId;
        mutable DWORD m_dwThreadId;
        mutable Buffer<WCHAR> m_ClassName;
        mutable Buffer<WCHAR> m_WindowText;
    };

    inline WindowHelper::WindowHelper(HWND hwnd)
        : m_hwnd(hwnd)
        , m_dwProcessId(0)
        , m_dwThreadId(0)
        , m_ClassName()
        , m_WindowText()
    {
    }

    inline WindowHelper::WindowHelper(HWND hwnd, int nDlgItem)
        : m_hwnd(GetDlgItem(hwnd, nDlgItem))
        , m_dwProcessId(0)
        , m_dwThreadId(0)
        , m_ClassName()
        , m_WindowText()
    {
    }

    inline WindowHelper::WindowHelper(const WindowHelper& rhs)
        : m_hwnd(rhs.m_hwnd)
        , m_dwProcessId(rhs.m_dwProcessId)
        , m_dwThreadId(rhs.m_dwThreadId)
        , m_ClassName(rhs.m_ClassName.Len)
        , m_WindowText(rhs.m_WindowText.Len)
    {
        if (m_ClassName.Len)
        {
            wcscpy_s(m_ClassName, m_ClassName.Len, rhs.m_ClassName);
        }
        if (m_WindowText.Len)
        {
            wcscpy_s(m_WindowText, m_WindowText.Len, rhs.m_WindowText);
        }
    }

    inline WindowHelper& WindowHelper::operator =(const WindowHelper& rhs)
    {
        m_hwnd = rhs.m_hwnd;
        m_dwProcessId = rhs.m_dwProcessId;
        m_dwThreadId = rhs.m_dwThreadId;
        m_ClassName.Resize(rhs.m_ClassName.Len);
        m_WindowText.Resize(rhs.m_WindowText.Len);
        if (m_ClassName.Len)
        {
            wcscpy_s(m_ClassName, m_ClassName.Len, rhs.m_ClassName);
        }
        if (m_WindowText.Len)
        {
            wcscpy_s(m_WindowText, m_WindowText.Len, rhs.m_WindowText);
        }
        return *this;
    }

    inline WindowHelper& WindowHelper::operator =(HWND hwnd)
    {
        m_hwnd = hwnd;
        m_dwProcessId = 0;
        m_dwThreadId = 0;
        m_ClassName.Resize(0);
        m_WindowText.Resize(0);
        return *this;
    }

    inline WindowHelper::operator HWND() const
    {
        return m_hwnd;
    }

    inline DWORD WindowHelper::get_ProcessId() const
    {
        if (!m_dwProcessId)
        {
            m_dwThreadId = GetWindowThreadProcessId(m_hwnd, &m_dwProcessId);
        }
        return m_dwProcessId;
    }

    inline void WindowHelper::set_ProcessId(DWORD dwProcessId)
    {
        m_dwProcessId = dwProcessId;
    }

    inline DWORD WindowHelper::get_ThreadId() const
    {
        if (!m_dwThreadId)
        {
            m_dwThreadId = GetWindowThreadProcessId(m_hwnd, &m_dwProcessId);
        }
        return m_dwThreadId;
    }

    inline PCWSTR WindowHelper::get_ClassName() const
    {
        if (!m_ClassName.Len)
        {
            m_ClassName.Resize(16);
            memset(m_ClassName, 0, m_ClassName.Len * sizeof(WCHAR));
            while (true)
            {
                int rc = GetClassNameW(m_hwnd, m_ClassName, static_cast<int>(m_ClassName.Len));
                if (rc < static_cast<int>(m_ClassName.Len) - 1)
                {
                    break;
                }
                m_ClassName.Resize(m_ClassName.Len * 2);
            }
        }
        return m_ClassName;
    }

    inline PCWSTR WindowHelper::get_WindowText() const
    {
        if (m_hwnd)
        {
            m_WindowText.Resize(0);
            m_WindowText.Resize(static_cast<size_t>(GetWindowTextLengthW(m_hwnd)) + 1);
            GetWindowTextW(m_hwnd, m_WindowText, static_cast<int>(m_WindowText.Len));
        }
        return m_WindowText;
    }

    inline WindowHelper WindowHelper::get_Parent() const
    {
        return WindowHelper(GetParent(m_hwnd));
    }

    inline DWORD WindowHelper::get_Style() const
    {
        return GetWindowLongW(m_hwnd, GWL_STYLE);
    }

    inline DWORD WindowHelper::get_ExStyle() const
    {
        return GetWindowLongW(m_hwnd, GWL_EXSTYLE);
    }

    inline DWORD WindowHelper::get_Id() const
    {
        return GetWindowLongW(m_hwnd, GWL_ID);
    }

    inline bool WindowHelper::get_IsDisabled() const
    {
        return (Style & WS_DISABLED) ? true : false;
    }

    inline bool WindowHelper::get_IsVisible() const
    {
        return (Style & WS_VISIBLE) ? true : false;
    }

    inline bool WindowHelper::get_IsChild() const
    {
        return (Style & WS_CHILD) ? true : false;
    }
}
