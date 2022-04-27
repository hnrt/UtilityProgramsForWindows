#include "hnrt/Heap.h"
#include "hnrt/Interlocked.h"
#include "hnrt/WindowStyle.h"
#include "ButtonCollection.h"


using namespace hnrt;


ButtonCollection::ButtonCollection()
	: m_cButtons(0)
	, m_hButtons(nullptr)
{
}


ButtonCollection::~ButtonCollection()
{
	if (m_hButtons)
	{
		Deallocate(m_hButtons);
	}
}


HWND ButtonCollection::operator [](ULONG index) const
{
	if (index < m_cButtons)
	{
		return m_hButtons[index];
	}
	else
	{
		return nullptr;
	}
}


void ButtonCollection::Resize(ULONG count)
{
	if (count)
	{
		m_hButtons = Allocate(m_hButtons, count);
	}
	else if (m_hButtons)
	{
		Deallocate(m_hButtons);
	}
}


void ButtonCollection::Add(HINSTANCE hInstance, HWND hwndParent, UINT id, PCWSTR pszLabel, HFONT hFont, bool bVisible)
{
	HWND hwndButton = CreateWindowExW(0, L"Button", pszLabel, WS_CHILD | (bVisible ? WS_VISIBLE : 0) | WS_DISABLED | BS_CENTER | BS_VCENTER, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndParent, NULL, hInstance, NULL);
	SetWindowLongW(hwndButton, GWL_ID, id);
	SendMessageW(hwndButton, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
	m_hButtons[m_cButtons++] = hwndButton;
}


void ButtonCollection::RemoveAll()
{
	while (m_cButtons > 0)
	{
		m_cButtons--;
		HWND hwndButton = Interlocked<HWND>::ExchangePointer(&m_hButtons[m_cButtons], nullptr);
		if (hwndButton)
		{
			DestroyWindow(hwndButton);
		}
	}
}


void ButtonCollection::ArrangePositions(LONG& x, LONG& y, LONG cx, LONG cy, LONG marginTop, LONG marginBottom)
{
	for (ULONG i = 0; i < m_cButtons; i++)
	{
		if (!WindowStyle(m_hButtons[i]).IsVisible)
		{
			continue;
		}
		y += marginTop;
		SetWindowPos(m_hButtons[i], NULL, x, y, cx, cy, SWP_NOZORDER);
		y += cy + marginBottom;
	}
}
