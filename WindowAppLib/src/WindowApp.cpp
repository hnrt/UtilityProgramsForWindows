#include "pch.h"
#include "hnrt/WindowApp.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


WindowApp::WindowApp(PCWSTR pszClassName)
	: AnyApp()
    , WindowSize()
    , WindowLayout()
    , m_hwnd(nullptr)
    , m_class(pszClassName)
    , m_preferences(pszClassName)
{
    C.SetWindowProcedure(MessageCallback);
}


void WindowApp::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    AnyApp::Open(hInstance, lpCmdLine, nCmdShow);
    if (!C.SetInstance(hInstance).Register())
    {
        throw Win32Exception(GetLastError(), L"RegisterClass failed.");
    }
    if (!CreateWindowExW(P.ExStyle, P.ClassName, P.WindowName, P.Style, P.X, P.Y, P.Width, P.Height, P.Parent, P.Menu, hInstance, this))
    {
        throw Win32Exception(GetLastError(), L"CreateWindow failed.");
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
}


void WindowApp::Close()
{
    if (hwnd)
    {
        DestroyWindow(hwnd);
        UnregisterClassW(C.Name, C.Instance);
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
        GetInstance(hwnd)->OnCreate(hwnd);
        break;
    }
    case WM_DESTROY:
        GetInstance(hwnd)->OnDestroy(hwnd);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        break;
    case WM_CLOSE:
        GetInstance(hwnd)->OnClose();
        break;
    case WM_SIZE:
        GetInstance(hwnd)->OnSize(wParam, lParam);
        break;
    case WM_COMMAND:
        return GetInstance(hwnd)->OnCommand(wParam, lParam);
    case WM_TIMER:
        return GetInstance(hwnd)->OnTimer(wParam, lParam);
    case WM_NOTIFY:
        return GetInstance(hwnd)->OnNotify(wParam, lParam);
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
    m_hwnd = hwnd;
    InitializeSize(hwnd);
    OnCreate();
}


void WindowApp::OnDestroy(HWND hwnd)
{
    UNREFERENCED_PARAMETER(hwnd);
    OnDestroy();
    m_hwnd = nullptr;
}


void WindowApp::OnCreate()
{
}


void WindowApp::OnDestroy()
{
}


void WindowApp::OnClose()
{
    PostQuitMessage(EXIT_SUCCESS);
}


void WindowApp::OnSize(WPARAM wParam, LPARAM lParam)
{
    WindowSize::OnSize(hwnd, wParam, lParam, *this);
}


LRESULT WindowApp::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return DefWindowProcW(hwnd, WM_COMMAND, wParam, lParam);
}


LRESULT WindowApp::OnTimer(WPARAM wParam, LPARAM lParam)
{
    return DefWindowProcW(hwnd, WM_TIMER, wParam, lParam);
}


LRESULT WindowApp::OnNotify(WPARAM wParam, LPARAM lParam)
{
    return DefWindowProcW(hwnd, WM_NOTIFY, wParam, lParam);
}
