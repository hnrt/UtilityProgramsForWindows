#pragma once


#include <Windows.h>


namespace hnrt
{
	class WindowLayout
	{
	public:

		WindowLayout() = default;
		WindowLayout(const WindowLayout&) = delete;
		virtual ~WindowLayout() = default;
		void operator =(const WindowLayout&) = delete;
		virtual void UpdateLayout(HWND hwnd, LONG cxDelta, LONG cyDelta);

		static void UpdateLayout(HWND hwndParent, HWND hwnd, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate = FALSE);
		static void UpdateLayout(HWND hwndDialog, UINT id, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate = FALSE);
	};
}
