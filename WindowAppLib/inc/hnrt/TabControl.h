#pragma once


#include <Windows.h>


namespace hnrt
{
	class TabControl
	{
	public:

		TabControl(HWND hwnd = NULL);
		TabControl(const TabControl&) = delete;
		~TabControl() = default;
		void operator = (const TabControl&) = delete;
		operator HWND() const;
		TabControl& Open(HWND hwndParent);
		TabControl& Open(HWND hwndParent, int cx, int cy);
		TabControl& Open(HWND hwndParent, int x, int y, int cx, int cy);
		TabControl& GetViewRect(PRECT);
		INT GetItemCount() const;
		INT GetCurrentItem() const;
		TabControl& SetCurrentItem(INT iCurrentItem);
		TabControl& RemoveItem(INT iIndex);

		__declspec(property(get = GetItemCount)) INT ItemCount;
		__declspec(property(get = GetCurrentItem, put = SetCurrentItem)) INT CurrentItem;

	private:

		HWND m_hwnd;
	};
}
