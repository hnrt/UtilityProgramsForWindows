#include "pch.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/Exception.h"


using namespace hnrt;


WindowLayoutSnapshot::WindowLayoutSnapshot(HWND hwndDialog)
	: m_hwndDialog(hwndDialog)
	, m_rects()
{
}


const RectangleMetrics& WindowLayoutSnapshot::operator [](int id) const
{
	std::map<int, RectangleMetrics>::const_iterator iter = m_rects.find(id);
	if (iter == m_rects.end())
	{
		throw Exception(L"WindowLayoutSnapshot::operator[]: No such entry. id=%d", id);
	}
	return iter->second;
}


RectangleMetrics& WindowLayoutSnapshot::operator [](int id)
{
	std::map<int, RectangleMetrics>::iterator iter = m_rects.find(id);
	if (iter == m_rects.end())
	{
		throw Exception(L"WindowLayoutSnapshot::operator[]: No such entry. id=%d", id);
	}
	return iter->second;
}


HWND WindowLayoutSnapshot::GetWindow() const
{
	return m_hwndDialog;
}


WindowLayoutSnapshot& WindowLayoutSnapshot::SetWindow(HWND hwndDialog)
{
	m_hwndDialog = hwndDialog;
	return *this;
}


WindowLayoutSnapshot& WindowLayoutSnapshot::Clear()
{
	m_rects.clear();
	return *this;
}


WindowLayoutSnapshot& WindowLayoutSnapshot::Add(int id)
{
	RectangleMetrics rect;
	HWND hwndItem = GetDlgItem(m_hwndDialog, id);
	rect.FromWindow(hwndItem);
	rect.ToClient(m_hwndDialog);
	return Add(id, rect);
}


WindowLayoutSnapshot& WindowLayoutSnapshot::Add(int id, const RectangleMetrics& rect)
{
	if (rect.cx > 0 && rect.cy > 0)
	{
		m_rects.insert(std::pair<int, RectangleMetrics>(id, rect));
	}
	return *this;
}


static BOOL CALLBACK AddChild(_In_ HWND hwnd, _In_ LPARAM lParam)
{
	WindowLayoutSnapshot* pThis = reinterpret_cast<WindowLayoutSnapshot*>(lParam);
	int id = GetDlgCtrlID(hwnd);
	if (id > 0)
	{
		pThis->Add(id);
	}
	return TRUE;
}


WindowLayoutSnapshot& WindowLayoutSnapshot::AddAllChildren(HWND hwndDialog)
{
	if (hwndDialog)
	{
		m_hwndDialog = hwndDialog;
	}
	EnumChildWindows(m_hwndDialog, AddChild, reinterpret_cast<LPARAM>(this));
	return *this;
}


WindowLayoutSnapshot& WindowLayoutSnapshot::Clone(WindowLayoutSnapshot& target)
{
	target.SetWindow(m_hwndDialog);
	target.Clear();
	for (std::map<int, RectangleMetrics>::const_iterator iter = m_rects.begin(); iter != m_rects.end(); iter++)
	{
		target.Add(iter->first, iter->second);
	}
	return *this;
}

void WindowLayoutSnapshot::Apply()
{
	for (std::map<int, RectangleMetrics>::const_iterator iter = m_rects.begin(); iter != m_rects.end(); iter++)
	{
		SetWindowGeometry(GetDlgItem(m_hwndDialog, iter->first), iter->second);
	}
}
