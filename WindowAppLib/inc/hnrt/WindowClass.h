#pragma once


#include <Windows.h>


namespace hnrt
{
	class WindowClass
	{
	public:

		WindowClass(PCWSTR pszName);
		WindowClass(const WindowClass&) = delete;
		~WindowClass() = default;
		void operator =(const WindowClass&) = delete;
		WindowClass& SetStyle(UINT value);
		WindowClass& SetWindowProcedure(WNDPROC value);
		WindowClass& SetClassExtra(int value);
		WindowClass& SetWindowExtra(int value);
		WindowClass& SetInstance(HINSTANCE value);
		WindowClass& SetIcon(HICON value);
		WindowClass& SetCursor(HCURSOR value);
		WindowClass& SetBackground(int value);
		WindowClass& SetMenuName(UINT value);
		WindowClass& SetSmallIcon(HICON value);
		ATOM Register();

		PCWSTR get_Name() const;
		HINSTANCE get_Instance() const;

		__declspec(property(get = get_Name)) PCWSTR Name;
		__declspec(property(get = get_Instance)) HINSTANCE Instance;

	protected:

		WNDCLASSEXW m_wc;
	};

	inline PCWSTR WindowClass::get_Name() const
	{
		return m_wc.lpszClassName;
	}

	inline HINSTANCE WindowClass::get_Instance() const
	{
		return m_wc.hInstance;
	}
}