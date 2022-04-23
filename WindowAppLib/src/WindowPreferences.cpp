#include "pch.h"
#include "hnrt/WindowPreferences.h"


using namespace hnrt;



WindowPreferences::WindowPreferences(PCWSTR pszClassName)
	: m_dwExStyle(0)
	, m_pszClassName(pszClassName)
	, m_pszWindowName(nullptr)
	, m_dwStyle(WS_OVERLAPPEDWINDOW)
	, m_x(CW_USEDEFAULT)
	, m_y(CW_USEDEFAULT)
	, m_cx(CW_USEDEFAULT)
	, m_cy(CW_USEDEFAULT)
	, m_hwndParent(nullptr)
{
}


WindowPreferences& WindowPreferences::SetExStyle(DWORD value)
{
	m_dwExStyle = value;
	return *this;
}


WindowPreferences& WindowPreferences::SetWindowName(PCWSTR value)
{
	m_pszWindowName = value;
	return *this;
}


WindowPreferences& WindowPreferences::SetStyle(DWORD value)
{
	m_dwStyle = value;
	return *this;
}


WindowPreferences& WindowPreferences::SetX(int value)
{
	m_x = value;
	return *this;
}


WindowPreferences& WindowPreferences::SetY(int value)
{
	m_y = value;
	return *this;
}


WindowPreferences& WindowPreferences::SetWidth(int value)
{
	m_cx = value;
	return *this;
}


WindowPreferences& WindowPreferences::SetHeight(int value)
{
	m_cy = value;
	return *this;
}


WindowPreferences& WindowPreferences::SetParent(HWND value)
{
	m_hwndParent = value;
	return *this;
}


WindowPreferences& WindowPreferences::SetMenu(HMENU value)
{
	m_hmenu = value;
	return *this;
}
