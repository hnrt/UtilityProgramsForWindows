#include "pch.h"
#include "hnrt/TabControl.h"
#include "hnrt/TabControlItem.h"
#include "hnrt/WindowDesign.h"
#include "hnrt/Exception.h"
#include <CommCtrl.h>


using namespace hnrt;


TabControl::TabControl(HWND hwnd)
	: m_hwnd(hwnd)
{
}


TabControl::operator HWND() const
{
	return m_hwnd;
}


TabControl& TabControl::Open(HWND hwndParent)
{
	RectangleMetrics rect;
	rect.FromClient(hwndParent);
	return Open(hwndParent, 0, 0, rect.cx, rect.cy);
}


TabControl& TabControl::Open(HWND hwndParent, int cx, int cy)
{
	return Open(hwndParent, 0, 0, cx, cy);
}


TabControl& TabControl::Open(HWND hwndParent, int x, int y, int cx, int cy)
{
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hwndParent, GWLP_HINSTANCE));
	m_hwnd = CreateWindowExW(0, WC_TABCONTROLW, NULL, WS_CHILD | WS_VISIBLE | TCS_MULTILINE, x, y, cx, cy, hwndParent, NULL, hInstance, NULL);
	return *this;
}


TabControl& TabControl::GetViewRect(PRECT pRect)
{
	RectangleMetrics rect;
	rect.FromWindow(m_hwnd);
	rect.ToClient(GetParent(m_hwnd));
	SendMessageW(m_hwnd, TCM_ADJUSTRECT, FALSE, reinterpret_cast<LPARAM>(&rect));
	memcpy_s(pRect, sizeof(*pRect), &static_cast<RECT&>(rect), sizeof(RECT));
	return *this;
}


INT TabControl::GetItemCount() const
{
	LRESULT count = SendMessageW(m_hwnd, TCM_GETITEMCOUNT, 0, 0);
	if (count < 0)
	{
		throw Exception(L"TabControl::GetItemCount failed.");
	}
	return static_cast<INT>(count);
}


INT TabControl::GetCurrentItem() const
{
	LRESULT index = SendMessageW(m_hwnd, TCM_GETCURSEL, 0, 0);
	if (index < 0)
	{
		throw Exception(L"TabControl::GetCurrentItem failed.");
	}
	return static_cast<INT>(index);
}


TabControl& TabControl::SetCurrentItem(INT iCurrentItem)
{
	SendMessageW(m_hwnd, TCM_SETCURSEL, iCurrentItem, 0);
	return *this;
}


TabControl& TabControl::RemoveItem(INT iIndex)
{
	if (iIndex >= 0)
	{
		if (!SendMessageW(m_hwnd, TCM_DELETEITEM, iIndex, 0))
		{
			throw Exception(L"TabControl::RemoveItem failed.");
		}
	}
	else
	{
		LRESULT count = SendMessageW(m_hwnd, TCM_GETITEMCOUNT, 0, 0);
		if (!SendMessageW(m_hwnd, TCM_DELETEITEM, count + iIndex, 0))
		{
			throw Exception(L"TabControl::RemoveItem failed.");
		}
	}
	return *this;
}


bool TabControl::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMHDR p = reinterpret_cast<LPNMHDR>(lParam);
	if (p->hwndFrom == m_hwnd)
	{
#pragma warning(disable: 26454)
		if (p->code == TCN_SELCHANGING)
#pragma warning(default: 26454)
		{
			OnTabSelectionChanging();
			return true;
		}
#pragma warning(disable: 26454)
		else if (p->code == TCN_SELCHANGE)
#pragma warning(default: 26454)
		{
			OnTabSelectionChanged();
			return true;
		}
	}
	return false;
}


void TabControl::OnTabSelectionChanging()
{
}


void TabControl::OnTabSelectionChanged()
{
}
