#pragma once


#include <Windows.h>
#include <list>


namespace hnrt
{
    class WindowInfo
    {
    public:

        WindowInfo(HWND hwnd);
        WindowInfo(const WindowInfo&) = delete;
        ~WindowInfo();
        void operator =(const WindowInfo&) = delete;
        void FindChildren();
        HWND get_Handle() const;
        PCWSTR get_ClassName() const;
        DWORD get_Style() const;
        DWORD get_ExStyle() const;
        DWORD get_Id() const;
        PCWSTR get_Text() const;
        bool get_Disabled() const;
        bool get_Visible() const;
        std::list<WindowInfo*>::const_iterator Begin() const;
        std::list<WindowInfo*>::const_iterator End() const;
        __declspec(property(get = get_Handle)) HWND Handle;
        __declspec(property(get = get_ClassName)) PCWSTR ClassName;
        __declspec(property(get = get_Style)) DWORD Style;
        __declspec(property(get = get_ExStyle)) DWORD ExStyle;
        __declspec(property(get = get_Id)) DWORD Id;
        __declspec(property(get = get_Text)) PCWSTR Text;
        __declspec(property(get = get_Disabled)) bool IsDisabled;
        __declspec(property(get = get_Visible)) bool IsVisible;

    private:

        static BOOL CALLBACK EnumChildCallback(
            _In_ HWND   hwnd,
            _In_ LPARAM lParam);

        HWND m_hwnd;
        wchar_t* m_pszClassName;
        DWORD m_dwStyle;
        DWORD m_dwExStyle;
        DWORD m_dwId;
        wchar_t* m_pszText;
        std::list<WindowInfo*> m_children;
    };

    inline HWND WindowInfo::get_Handle() const
    {
        return m_hwnd;
    }

    inline PCWSTR WindowInfo::get_ClassName() const
    {
        return m_pszClassName;
    }

    inline DWORD WindowInfo::get_Style() const
    {
        return m_dwStyle;
    }

    inline DWORD WindowInfo::get_ExStyle() const
    {
        return m_dwExStyle;
    }

    inline DWORD WindowInfo::get_Id() const
    {
        return m_dwId;
    }

    inline PCWSTR WindowInfo::get_Text() const
    {
        return m_pszText;
    }

    inline bool WindowInfo::get_Disabled() const
    {
        return (m_dwStyle & WS_DISABLED) == WS_DISABLED;
    }

    inline bool WindowInfo::get_Visible() const
    {
        return (m_dwStyle & WS_VISIBLE) == WS_VISIBLE;
    }

    inline std::list<WindowInfo*>::const_iterator WindowInfo::Begin() const
    {
        return m_children.begin();
    }

    inline std::list<WindowInfo*>::const_iterator WindowInfo::End() const
    {
        return m_children.end();
    }
}
