#pragma once


#include <Windows.h>


namespace hnrt
{
	class WindowStyle
	{
	public:

		WindowStyle(HWND hwnd);
		WindowStyle(const WindowStyle& src);
		~WindowStyle() = default;
		void operator =(const WindowStyle& src) = delete;
		operator DWORD() const;
		WindowStyle& Apply();
		WindowStyle& SetVisibility(bool bVisible);

		bool get_Visibility() const;

		__declspec(property(get = get_Visibility)) bool IsVisible;

	private:

		HWND m_hwnd;
		DWORD m_dwStyle;
	};

	inline WindowStyle::operator DWORD() const
	{
		return m_dwStyle;
	}

	inline bool WindowStyle::get_Visibility() const
	{
		return (m_dwStyle & WS_VISIBLE) != 0 ? true : false;
	}
}
