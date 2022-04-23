#include "pch.h"
#include "hnrt/WindowSize.h"


using namespace hnrt;


WindowSize::WindowSize()
	: m_cxInitial(0)
	, m_cyInitial(0)
	, m_cxMinimum(0)
	, m_cyMinimum(0)
	, m_cxClientInitial(0)
	, m_cyClientInitial(0)
	, m_cxClient(0)
	, m_cyClient(0)
    , m_cxBorder(0)
    , m_cyTitle(0)
{
}


void WindowSize::InitializeSize(HWND hwnd)
{
	RECT rect = { 0, 0, 0, 0 };
	GetWindowRect(hwnd, &rect);
	m_cxMinimum = m_cxInitial = rect.right - rect.left;
	m_cyMinimum = m_cyInitial = rect.bottom - rect.top;

	RECT rectClient = { 0, 0, 0, 0 };
	GetClientRect(hwnd, &rectClient);
	m_cxClient = m_cxClientInitial = rectClient.right;
	m_cyClient = m_cyClientInitial = rectClient.bottom;

    POINT pt = { 0, 0 };
    ClientToScreen(hwnd, &pt);
    m_cxBorder = pt.x - rect.left;
    m_cyTitle = pt.y - rect.top;
}


void WindowSize::SetMinimumSize(LONG cx, LONG cy)
{
    m_cxMinimum = cx;
    m_cyMinimum = cy;
}


void WindowSize::OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam, WindowLayout& rLayout)
{
    if (wParam == SIZE_MINIMIZED)
    {
        return;
    }

    RECT rect = { 0, 0, 0, 0 };
    GetWindowRect(hwnd, &rect);
    LONG cx = rect.right - rect.left;
    LONG cy = rect.bottom - rect.top;
    if (cx < m_cxMinimum || cy < m_cyMinimum)
    {
        SetWindowPos(hwnd, NULL, 0, 0, cx > m_cxMinimum ? cx : m_cxMinimum, cy > m_cyMinimum ? cy : m_cyMinimum, SWP_NOMOVE | SWP_NOZORDER);
        return;
    }

    LONG cxClient = LOWORD(lParam);
    LONG cyClient = HIWORD(lParam);
    LONG cxDelta = cxClient - m_cxClient;
    LONG cyDelta = cyClient - m_cyClient;
    m_cxClient = cxClient;
    m_cyClient = cyClient;
    if (cxDelta || cyDelta)
    {
        rLayout.UpdateLayout(hwnd, cxDelta, cyDelta);
    }
}
