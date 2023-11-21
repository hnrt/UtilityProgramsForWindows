#pragma once


#include <Windows.h>


namespace hnrt
{
	struct WindowClass
		: public WNDCLASSEXW
	{
		WindowClass(PCWSTR pszName);
		WindowClass(const WindowClass&) = default;
		~WindowClass() = default;
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
		BOOL Unregister();
	};
}
