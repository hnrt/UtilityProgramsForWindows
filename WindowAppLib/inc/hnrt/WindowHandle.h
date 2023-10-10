#pragma once


#include <Windows.h>


namespace hnrt
{
	inline HINSTANCE GetInstanceHandle(HWND hwnd)
	{
		return reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hwnd, GWLP_HINSTANCE));
	}

	template<class T>
	T* GetWindowUserData(HWND hwnd)
	{
		return reinterpret_cast<T*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	}

	template<class T>
	T* SetWindowUserData(HWND hwnd, void* value)
	{
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(value));
		return reinterpret_cast<T*>(value);
	}

	template<class T>
	T* SetWindowUserData(HWND hwnd, LONG_PTR value)
	{
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, value);
		return reinterpret_cast<T*>(value);
	}

	template<class T>
	T* ExchangeWindowUserData(HWND hwnd, void* value)
	{
		T* ptr = GetWindowUserData<T>(hwnd);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(value));
		return ptr;
	}

	template<class T>
	T* ExchangeWindowUserData(HWND hwnd, LONG_PTR value)
	{
		T* ptr = GetWindowUserData<T>(hwnd);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, value);
		return ptr;
	}

	inline void SetWindowPosition(HWND hwnd, int x, int y, UINT extraFlags = 0)
	{
		SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | extraFlags);
	}

	inline void SetWindowPosition(HWND hwnd, const POINT& pt, UINT extraFlags = 0)
	{
		SetWindowPos(hwnd, NULL, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | extraFlags);
	}

	inline void SetWindowSize(HWND hwnd, int cx, int cy, UINT extraFlags = 0)
	{
		SetWindowPos(hwnd, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | extraFlags);
	}

	inline void SetWindowSize(HWND hwnd, const RECT& rect, UINT extraFlags = 0)
	{
		SetWindowPos(hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE | extraFlags);
	}

	inline void SetWindowGeometry(HWND hwnd, int x, int y, int cx, int cy, UINT extraFlags = 0)
	{
		SetWindowPos(hwnd, NULL, x, y, cx, cy, SWP_NOZORDER | extraFlags);
	}

	inline void SetWindowGeometry(HWND hwnd, const RECT& rect, UINT extraFlags = 0)
	{
		SetWindowPos(hwnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | extraFlags);
	}

	inline HFONT GetFont(HWND hwnd)
	{
		return reinterpret_cast<HFONT>(::SendMessageW(hwnd, WM_GETFONT, 0, 0));
	}

	inline void SetFont(HWND hwnd, HFONT hFont, BOOL bInvalidate = FALSE)
	{
		::SendMessageW(hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), bInvalidate);
	}

	inline HWND GetRootWindow(HWND hwnd)
	{
		return GetAncestor(hwnd, GA_ROOT);
	}
}
