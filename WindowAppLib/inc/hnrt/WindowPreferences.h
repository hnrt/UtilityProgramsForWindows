#pragma once


#include <Windows.h>


namespace hnrt
{
	class WindowPreferences
	{
	public:

		WindowPreferences(PCWSTR pszClassName);
		WindowPreferences(const WindowPreferences&) = delete;
		~WindowPreferences() = default;
		void operator =(const WindowPreferences&) = delete;
		WindowPreferences& SetExStyle(DWORD value);
		WindowPreferences& SetWindowName(PCWSTR value);
		WindowPreferences& SetStyle(DWORD value);
		WindowPreferences& SetX(int value);
		WindowPreferences& SetY(int value);
		WindowPreferences& SetWidth(int value);
		WindowPreferences& SetHeight(int value);
		WindowPreferences& SetParent(HWND value);
		WindowPreferences& SetMenu(HMENU value);
		DWORD get_ExStyle() const;
		PCWSTR get_ClassName() const;
		PCWSTR get_WindowName() const;
		DWORD get_Style() const;
		int get_X() const;
		int get_Y() const;
		int get_Width() const;
		int get_Height() const;
		HWND get_Parent() const;
		HMENU get_Menu() const;

		__declspec(property(get = get_ExStyle)) DWORD ExStyle;
		__declspec(property(get = get_ClassName)) PCWSTR ClassName;
		__declspec(property(get = get_WindowName)) PCWSTR WindowName;
		__declspec(property(get = get_Style)) DWORD Style;
		__declspec(property(get = get_X)) int X;
		__declspec(property(get = get_Y)) int Y;
		__declspec(property(get = get_Width)) int Width;
		__declspec(property(get = get_Height)) int Height;
		__declspec(property(get = get_Parent)) HWND Parent;
		__declspec(property(get = get_Menu)) HMENU Menu;

	private:

		DWORD m_dwExStyle;
		PCWSTR m_pszClassName;
		PCWSTR m_pszWindowName;
		DWORD m_dwStyle;
		int m_x;
		int m_y;
		int m_cx;
		int m_cy;
		HWND m_hwndParent;
		HMENU m_hmenu;
	};

	inline DWORD WindowPreferences::get_ExStyle() const
	{
		return m_dwExStyle;
	}

	inline PCWSTR WindowPreferences::get_ClassName() const
	{
		return m_pszClassName;
	}

	inline PCWSTR WindowPreferences::get_WindowName() const
	{
		return m_pszWindowName;
	}

	inline DWORD WindowPreferences::get_Style() const
	{
		return m_dwStyle;
	}

	inline int WindowPreferences::get_X() const
	{
		return m_x;
	}

	inline int WindowPreferences::get_Y() const
	{
		return m_y;
	}

	inline int WindowPreferences::get_Width() const
	{
		return m_cx;
	}

	inline int WindowPreferences::get_Height() const
	{
		return m_cy;
	}

	inline HWND WindowPreferences::get_Parent() const
	{
		return m_hwndParent;
	}

	inline HMENU WindowPreferences::get_Menu() const
	{
		return m_hmenu;
	}
}
