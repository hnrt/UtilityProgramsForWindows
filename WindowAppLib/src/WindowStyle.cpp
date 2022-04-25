#include "pch.h"
#include "hnrt/WindowStyle.h"


using namespace hnrt;


WindowStyle::WindowStyle(HWND hwnd)
	: m_hwnd(hwnd)
	, m_dwStyle(GetWindowLongW(hwnd, GWL_STYLE))
{
}


WindowStyle::WindowStyle(const WindowStyle& src)
	: m_hwnd(src.m_hwnd)
	, m_dwStyle(src.m_dwStyle)
{
}


WindowStyle& WindowStyle::Apply()
{
	SetWindowLongW(m_hwnd, GWL_STYLE, m_dwStyle);
	return *this;
}


WindowStyle& WindowStyle::SetVisibility(bool bVisible)
{
	if (bVisible)
	{
		m_dwStyle |= WS_VISIBLE;
	}
	else
	{
		m_dwStyle &= ~WS_VISIBLE;
	}
	return *this;
}
