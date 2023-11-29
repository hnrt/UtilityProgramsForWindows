#pragma once


#include "hnrt/String.h"


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
        const String& get_ClassName() const;
        void set_ClassName(const String& pszClassName);
        const String& get_WindowText() const;
        void set_WindowText(const String& pszWindowText);
        WindowHelper get_Parent() const;
        DWORD get_Style() const;
        DWORD get_ExStyle() const;
        DWORD get_Id() const;
        bool get_IsDisabled() const;
        bool get_IsVisible() const;
        bool get_IsChild() const;
        __declspec(property(get = get_ProcessId, put = set_ProcessId)) DWORD ProcessId;
        __declspec(property(get = get_ThreadId)) DWORD ThreadId;
        __declspec(property(get = get_ClassName, put = set_ClassName)) const String& ClassName;
        __declspec(property(get = get_WindowText, put = set_WindowText)) const String& WindowText;
        __declspec(property(get = get_Parent)) WindowHelper Parent;
        __declspec(property(get = get_Style)) DWORD Style;
        __declspec(property(get = get_ExStyle)) DWORD ExStyle;
        __declspec(property(get = get_Id)) DWORD Id;
        __declspec(property(get = get_IsDisabled)) bool IsDisabled;
        __declspec(property(get = get_IsVisible)) bool IsVisible;
        __declspec(property(get = get_IsChild)) bool IsChild;

        static HWND FindTopLevelWindow(const String& szClassName, const String& szWindowText, DWORD dwProcessId);
        HWND FindChildWindow(const String& szClassName, const String& szWindowText);
        HWND FindChildWindow2(const String& szClassName, const String& szWindowText);
        HWND FindImeWindow() const;

    private:

        HWND m_hwnd;
        mutable DWORD m_dwProcessId;
        mutable DWORD m_dwThreadId;
        mutable String m_szClassName;
        mutable String m_szWindowText;
    };

    inline WindowHelper::WindowHelper(HWND hwnd)
        : m_hwnd(hwnd)
        , m_dwProcessId(0)
        , m_dwThreadId(0)
        , m_szClassName()
        , m_szWindowText()
    {
    }

    inline WindowHelper::WindowHelper(HWND hwnd, int nDlgItem)
        : m_hwnd(GetDlgItem(hwnd, nDlgItem))
        , m_dwProcessId(0)
        , m_dwThreadId(0)
        , m_szClassName()
        , m_szWindowText()
    {
    }

    inline WindowHelper::WindowHelper(const WindowHelper& rhs)
        : m_hwnd(rhs.m_hwnd)
        , m_dwProcessId(rhs.m_dwProcessId)
        , m_dwThreadId(rhs.m_dwThreadId)
        , m_szClassName(rhs.m_szClassName)
        , m_szWindowText(rhs.m_szWindowText)
    {
    }

    inline WindowHelper& WindowHelper::operator =(const WindowHelper& rhs)
    {
        m_hwnd = rhs.m_hwnd;
        m_dwProcessId = rhs.m_dwProcessId;
        m_dwThreadId = rhs.m_dwThreadId;
        m_szClassName = rhs.m_szClassName;
        m_szWindowText = rhs.m_szWindowText;
        return *this;
    }

    inline WindowHelper& WindowHelper::operator =(HWND hwnd)
    {
        m_hwnd = hwnd;
        m_dwProcessId = 0;
        m_dwThreadId = 0;
        m_szClassName = String::Empty;
        m_szWindowText = String::Empty;
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
