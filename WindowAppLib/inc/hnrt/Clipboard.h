#pragma once


#include <Windows.h>


namespace hnrt
{
	class Clipboard
	{
	public:

		static bool Copy(HWND hwnd, const WCHAR* pText, size_t cch = static_cast<size_t>(-1));
		static bool Copy(HWND hwnd, HWND hwndText);
		static bool Copy(HWND hwnd, HWND hwndDialog, int idControl);
	};
}
