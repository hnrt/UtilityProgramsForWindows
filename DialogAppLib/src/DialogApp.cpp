#include "pch.h"
#include "hnrt/DialogApp.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Exception.h"


#pragma comment(lib,"Core")


using namespace hnrt;


DialogApp::DialogApp(UINT idTemplate)
    : m_iExitCode(EXIT_FAILURE)
    , m_idTemplate(idTemplate)
    , m_hAccelTable(nullptr)
    , m_hwnd(nullptr)
	, m_cxInitial(0)
    , m_cyInitial(0)
    , m_cxMinimum(0)
    , m_cyMinimum(0)
    , m_cxClientInitial(0)
    , m_cyClientInitial(0)
    , m_cxClient(0)
    , m_cyClient(0)
{
}


void DialogApp::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(lpCmdLine);
    ResourceString::m_hInstance = hInstance;
    m_hwnd = CreateDialogParamW(hInstance, MAKEINTRESOURCE(m_idTemplate), NULL, ProcessMessage, reinterpret_cast<LPARAM>(this));
    if (!m_hwnd)
    {
        throw Exception(L"CreateDialog failed.");
    }
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
    //m_hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(IDR_ACCELERATOR1));
}


void DialogApp::Run()
{
    if (!m_hwnd)
    {
        return;
    }
    while (1)
    {
        MSG msg;
        BOOL bRet = GetMessageW(&msg, NULL, 0, 0);
        if (bRet == -1)
        {
            throw Exception(L"GetMessage failed.");
        }
        else if (!bRet)
        {
            m_iExitCode = static_cast<int>(msg.wParam);
            break;
        }
        if (m_hAccelTable && TranslateAcceleratorW(m_hwnd, m_hAccelTable, &msg))
        {
            continue;
        }
        else if (IsDialogMessageW(m_hwnd, &msg))
        {
            continue;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
}


void DialogApp::Close()
{
    if (!m_hwnd)
    {
        return;
    }
    DestroyWindow(m_hwnd);
}


int DialogApp::TryProcessMessage()
{
    MSG msg;
    if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            PostQuitMessage(static_cast<int>(msg.wParam));
            return -1;
        }
        if (m_hAccelTable && TranslateAcceleratorW(m_hwnd, m_hAccelTable, &msg))
        {
            // OK
        }
        else if (IsDialogMessageW(m_hwnd, &msg))
        {
            // OK
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}


INT_PTR CALLBACK DialogApp::ProcessMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        reinterpret_cast<DialogApp*>(lParam)->OnCreate(hDlg);
        break;
    case WM_DESTROY:
        GetInstance(hDlg)->OnDestory(hDlg);
        break;
    case WM_CLOSE:
        GetInstance(hDlg)->OnClose(hDlg);
        break;
    case WM_SIZE:
        GetInstance(hDlg)->OnSize(hDlg, wParam, lParam);
        break;
    case WM_COMMAND:
        GetInstance(hDlg)->OnCommand(hDlg, wParam, lParam);
        break;
    default:
        return static_cast<INT_PTR>(FALSE);
    }
    return static_cast<INT_PTR>(TRUE);
}


DialogApp* DialogApp::GetInstance(HWND hDlg)
{
    return reinterpret_cast<DialogApp*>(GetWindowLongPtrW(hDlg, GWLP_USERDATA));
}


void DialogApp::UpdateLayout(HWND hDlg, UINT id, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate)
{
    HWND hwndChild = GetDlgItem(hDlg, id);
    UINT uFlags = SWP_NOZORDER;
    LONG x, y, cx, cy;
    RECT rect;
    GetWindowRect(hwndChild, &rect);
    if (dx || dy)
    {
        POINT pt = { rect.left, rect.top };
        ScreenToClient(hDlg, &pt);
        x = pt.x + dx;
        y = pt.y + dy;
    }
    else
    {
        x = 0;
        y = 0;
        uFlags |= SWP_NOMOVE;
    }
    if (dcx || dcy)
    {
        cx = rect.right - rect.left + dcx;
        cy = rect.bottom - rect.top + dcy;
    }
    else
    {
        cx = 0;
        cy = 0;
        uFlags |= SWP_NOSIZE;
    }
    SetWindowPos(hwndChild, NULL, x, y, cx, cy, uFlags);
    if (bInvalidate)
    {
        InvalidateRect(hwndChild, NULL, TRUE);
    }
}


void DialogApp::OnCreate(HWND hDlg)
{
    SetWindowLongPtrW(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    RECT rect = { 0, 0, 0, 0 };
    GetWindowRect(hDlg, &rect);
    m_cxMinimum = m_cxInitial = rect.right - rect.left;
    m_cyMinimum = m_cyInitial = rect.bottom - rect.top;
    GetClientRect(hDlg, &rect);
    m_cxClient = m_cxClientInitial = rect.right;
    m_cyClient = m_cyClientInitial = rect.bottom;
}


void DialogApp::OnDestory(HWND hDlg)
{
    SetWindowLongPtrW(hDlg, GWLP_USERDATA, 0);
}


void DialogApp::OnClose(HWND hDlg)
{
    UNREFERENCED_PARAMETER(hDlg);
    PostQuitMessage(EXIT_SUCCESS);
}


void DialogApp::OnSize(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
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
        UpdateLayout(hDlg, cxDelta, cyDelta);
    }
}


void DialogApp::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
}


void DialogApp::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
}
