#include "pch.h"
#include "hnrt/DialogApp.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Exception.h"
#include "hnrt/Interlocked.h"


#pragma comment(lib,"Core")


using namespace hnrt;


DialogApp::DialogApp(UINT idTemplate)
    : AnyApp()
    , WindowSize()
    , WindowLayout()
    , m_idTemplate(idTemplate)
{
}


void DialogApp::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(lpCmdLine);
    ResourceString::m_hInstance = hInstance;
    m_hwnd = CreateDialogParamW(hInstance, MAKEINTRESOURCE(m_idTemplate), NULL, MessageCallback, reinterpret_cast<LPARAM>(this));
    if (!m_hwnd)
    {
        throw Exception(L"CreateDialog failed.");
    }
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}


void DialogApp::Close(HINSTANCE hInstance)
{
    UNREFERENCED_PARAMETER(hInstance);
    HWND hwnd = Interlocked<HWND>::ExchangePointer(&m_hwnd, nullptr);
    if (hwnd)
    {
        DestroyWindow(hwnd);
    }
}


void DialogApp::ProcessMessage(MSG* pMsg)
{
    if (m_hAccelTable && TranslateAcceleratorW(m_hwnd, m_hAccelTable, pMsg))
    {
        // OK
    }
    else if (IsDialogMessageW(m_hwnd, pMsg))
    {
        // OK
    }
    else
    {
        TranslateMessage(pMsg);
        DispatchMessageW(pMsg);
    }
}


INT_PTR CALLBACK DialogApp::MessageCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        DialogApp* pThis = reinterpret_cast<DialogApp*>(lParam);
        SetWindowLongPtrW(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        pThis->InitializeSize(hDlg);
        pThis->OnCreate(hDlg);
        break;
    }
    case WM_DESTROY:
        GetInstance(hDlg)->OnDestroy(hDlg);
        SetWindowLongPtrW(hDlg, GWLP_USERDATA, 0);
        break;
    case WM_CLOSE:
        GetInstance(hDlg)->OnClose(hDlg);
        break;
    case WM_SIZE:
        GetInstance(hDlg)->OnSize(hDlg, wParam, lParam);
        break;
    case WM_COMMAND:
        return GetInstance(hDlg)->OnCommand(hDlg, wParam, lParam);
    case WM_TIMER:
        return GetInstance(hDlg)->OnTimer(hDlg, wParam, lParam);
    default:
        return FALSE;
    }
    return TRUE;
}


DialogApp* DialogApp::GetInstance(HWND hDlg)
{
    return reinterpret_cast<DialogApp*>(GetWindowLongPtrW(hDlg, GWLP_USERDATA));
}


void DialogApp::OnCreate(HWND hDlg)
{
}


void DialogApp::OnDestroy(HWND hDlg)
{
}


void DialogApp::OnClose(HWND hDlg)
{
    UNREFERENCED_PARAMETER(hDlg);
    PostQuitMessage(EXIT_SUCCESS);
}


void DialogApp::OnSize(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    WindowSize::OnSize(hDlg, wParam, lParam, *this);
}


INT_PTR DialogApp::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}


INT_PTR DialogApp::OnTimer(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}
