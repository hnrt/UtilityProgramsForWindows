#include "pch.h"
#include "hnrt/WindowSize.h"
#include "hnrt/WindowDesign.h"


using namespace hnrt;


WindowSize::WindowSize()
	: m_cxInitial(0)
	, m_cyInitial(0)
	, m_cxMinimum(0)
	, m_cyMinimum(0)
    , m_cxWindow(0)
    , m_cyWindow(0)
	, m_cxClient(0)
	, m_cyClient(0)
    , m_cxLeftFrame(0)
    , m_cxRightFrame(0)
    , m_cyTopFrame(0)
    , m_cyBottomFrame(0)
{
}


void WindowSize::InitializeSize(HWND hwnd)
{
    RectangleMetrics rect;
    rect.FromWindow(hwnd);
	m_cxMinimum = m_cxInitial = m_cxWindow = rect.Width;
	m_cyMinimum = m_cyInitial = m_cyWindow = rect.Height;

    RectangleMetrics rectClient;
    rectClient.FromClient(hwnd);
    m_cxClient = rectClient.Width;
    m_cyClient = rectClient.Height;

    MeasureFrame(hwnd);
}


void WindowSize::MeasureFrame(HWND hwnd)
{
    RectangleMetrics rect;
    rect.FromWindow(hwnd);

    RectangleMetrics rectClient;
    rectClient.FromClient(hwnd);
    rectClient.ToScreen(hwnd);

    m_cxLeftFrame = rectClient.left - rect.left;
    m_cxRightFrame = rect.right - rectClient.right;
    m_cyTopFrame = rectClient.top - rect.top;
    m_cyBottomFrame = rect.bottom - rectClient.bottom;

    if (m_cxLeftFrame < 0) m_cxLeftFrame = 0;
    if (m_cxRightFrame < m_cxLeftFrame) m_cxRightFrame = m_cxLeftFrame;
    if (m_cyTopFrame < 0) m_cyTopFrame = 0;
    if (m_cyBottomFrame < m_cxLeftFrame) m_cyBottomFrame = m_cxLeftFrame;
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

    LONG cxClient = LOWORD(lParam);
    LONG cyClient = HIWORD(lParam);
    LONG cxDelta = cxClient - m_cxClient;
    LONG cyDelta = cyClient - m_cyClient;

    RectangleMetrics rect;
    rect.FromWindow(hwnd);
    m_cxWindow = rect.Width;
    m_cyWindow = rect.Height;

    if (m_cxWindow < m_cxMinimum || m_cyWindow < m_cyMinimum)
    {
        SetWindowPos(hwnd, NULL, 0, 0, m_cxWindow > m_cxMinimum ? m_cxWindow : m_cxMinimum, m_cyWindow > m_cyMinimum ? m_cyWindow : m_cyMinimum, SWP_NOMOVE | SWP_NOZORDER);
    }
    else if (cxDelta || cyDelta)
    {
        m_cxClient = cxClient;
        m_cyClient = cyClient;
        rLayout.UpdateLayout(hwnd, cxDelta, cyDelta);
    }
}
