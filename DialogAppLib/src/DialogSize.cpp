#include "pch.h"
#include "hnrt/DialogSize.h"


using namespace hnrt;


DialogSize::DialogSize()
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


void DialogSize::Initialize(HWND hDlg)
{
	RECT rect = { 0, 0, 0, 0 };
	GetWindowRect(hDlg, &rect);
	m_cxMinimum = m_cxInitial = rect.right - rect.left;
	m_cyMinimum = m_cyInitial = rect.bottom - rect.top;

	RECT rectClient = { 0, 0, 0, 0 };
	GetClientRect(hDlg, &rectClient);
	m_cxClient = m_cxClientInitial = rectClient.right;
	m_cyClient = m_cyClientInitial = rectClient.bottom;

    POINT pt = { 0, 0 };
    ClientToScreen(hDlg, &pt);
    m_cxBorder = pt.x - rect.left;
    m_cyTitle = pt.y - rect.top;
}


void DialogSize::SetMinimum(LONG cx, LONG cy)
{
    m_cxMinimum = cx;
    m_cyMinimum = cy;
}


void DialogSize::OnSize(HWND hDlg, WPARAM wParam, LPARAM lParam, DialogLayout& rLayout)
{
    if (wParam == SIZE_MINIMIZED)
    {
        return;
    }

    RECT rect = { 0, 0, 0, 0 };
    GetWindowRect(hDlg, &rect);
    LONG cx = rect.right - rect.left;
    LONG cy = rect.bottom - rect.top;
    if (cx < m_cxMinimum || cy < m_cyMinimum)
    {
        SetWindowPos(hDlg, NULL, 0, 0, cx > m_cxMinimum ? cx : m_cxMinimum, cy > m_cyMinimum ? cy : m_cyMinimum, SWP_NOMOVE | SWP_NOZORDER);
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
        rLayout.UpdateLayout(hDlg, cxDelta, cyDelta);
    }
}
