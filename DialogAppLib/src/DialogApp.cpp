#include "pch.h"
#include "hnrt/DialogApp.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Exception.h"


#pragma comment(lib,"Core")


using namespace hnrt;


DialogApp::DialogApp(UINT idTemplate)
    : DialogSize()
    , DialogLayout()
    , m_iExitCode(EXIT_FAILURE)
    , m_idTemplate(idTemplate)
    , m_hAccelTable(nullptr)
    , m_hwnd(nullptr)
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


void DialogApp::SetAccelerators(HINSTANCE hInstance, UINT id)
{
    m_hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(id));
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


void DialogApp::OnCreate(HWND hDlg)
{
    SetWindowLongPtrW(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    InitializeSize(hDlg);
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
    DialogSize::OnSize(hDlg, wParam, lParam, *this);
}


void DialogApp::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
}


void DialogApp::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
}
