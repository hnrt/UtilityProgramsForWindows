#include "pch.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/Exception.h"


using namespace hnrt;


WindowLayoutSnapshot::WindowLayoutSnapshot()
	: m_rects()
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


WindowLayoutSnapshot& WindowLayoutSnapshot::Clear()
{
	m_rects.clear();
	return *this;
}


WindowLayoutSnapshot& WindowLayoutSnapshot::Add(int id, const RectangleMetrics& rect)
{
	m_rects.insert(std::pair<int, RectangleMetrics>(id, rect));
	return *this;
}


WindowLayoutSnapshot& WindowLayoutSnapshot::Add(HWND hwnd, int id)
{
	RectangleMetrics rect;
	HWND hwndItem = GetDlgItem(hwnd, id);
	rect.FromWindow(hwndItem);
	rect.ToClient(hwnd);
	return Add(id, rect);
}


WindowLayoutSnapshot& WindowLayoutSnapshot::Clone(WindowLayoutSnapshot& target)
{
	target.Clear();
	for (std::map<int, RectangleMetrics>::const_iterator iter = m_rects.begin(); iter != m_rects.end(); iter++)
	{
		target.Add(iter->first, iter->second);
	}
	return *this;
}

void WindowLayoutSnapshot::ApplyTo(HWND hwnd)
{
	for (std::map<int, RectangleMetrics>::const_iterator iter = m_rects.begin(); iter != m_rects.end(); iter++)
	{
		SetWindowGeometry(GetDlgItem(hwnd, iter->first), iter->second);
	}
}
