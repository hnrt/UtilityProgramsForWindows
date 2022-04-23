#include "pch.h"
#include "hnrt/WindowApp.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


WindowApp::WindowApp(PCWSTR pszClassName)
	: AnyApp()
    , WindowSize()
    , WindowLayout()
    , m_preferences(pszClassName)
{
}


void WindowApp::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(lpCmdLine);
    WindowPreferences& p = m_preferences;
    m_hwnd = CreateWindowExW(p.ExStyle, p.ClassName, p.WindowName, p.Style, p.X, p.Y, p.Width, p.Height, p.Parent, p.Menu, hInstance, this);
    if (!m_hwnd)
    {
        throw Win32Exception(GetLastError(), L"CreateWindow failed.");
    }
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}


void WindowApp::Close(HINSTANCE hInstance)
{
    HWND hwnd = Interlocked<HWND>::ExchangePointer(&m_hwnd, nullptr);
    if (hwnd)
    {
        DestroyWindow(hwnd);
        UnregisterClassW(m_preferences.ClassName, hInstance);
    }
}


void WindowApp::ProcessMessage(MSG* pMsg)
{
    if (m_hAccelTable && TranslateAcceleratorW(m_hwnd, m_hAccelTable, pMsg))
    {
        // OK
    }
    else
    {
        TranslateMessage(pMsg);
        DispatchMessageW(pMsg);
    }
}


LRESULT CALLBACK WindowApp::MessageCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_NCCREATE:
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams));
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    case WM_CREATE:
    {
        WindowApp* pThis = GetInstance(hwnd);
        pThis->InitializeSize(hwnd);
        pThis->OnCreate(hwnd);
        break;
    }
    case WM_DESTROY:
        GetInstance(hwnd)->OnDestroy(hwnd);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        break;
    case WM_CLOSE:
        GetInstance(hwnd)->OnClose(hwnd);
        break;
    case WM_SIZE:
        GetInstance(hwnd)->OnSize(hwnd, wParam, lParam);
        break;
    case WM_COMMAND:
        return GetInstance(hwnd)->OnCommand(hwnd, wParam, lParam);
    case WM_TIMER:
        return GetInstance(hwnd)->OnTimer(hwnd, wParam, lParam);
    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


WindowApp* WindowApp::GetInstance(HWND hwnd)
{
    return reinterpret_cast<WindowApp*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}


void WindowApp::OnCreate(HWND hwnd)
{
}


void WindowApp::OnDestroy(HWND hwnd)
{
}


void WindowApp::OnClose(HWND hwnd)
{
    UNREFERENCED_PARAMETER(hwnd);
    PostQuitMessage(EXIT_SUCCESS);
}


void WindowApp::OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    WindowSize::OnSize(hwnd, wParam, lParam, *this);
}


LRESULT WindowApp::OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProcW(hwnd, WM_COMMAND, wParam, lParam);
}


LRESULT WindowApp::OnTimer(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProcW(hwnd, WM_TIMER, wParam, lParam);
}
