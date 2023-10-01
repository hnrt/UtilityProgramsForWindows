#include "pch.h"
#include "hnrt/DialogApp.h"
#include "hnrt/Exception.h"


using namespace hnrt;


DialogApp::DialogApp(UINT idTemplate)
    : AnyApp()
    , DialogBox(idTemplate)
{
}


void DialogApp::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    AnyApp::Open(hInstance, lpCmdLine, nCmdShow);
    DialogBox::Open(hInstance);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
}


void DialogApp::SetText(PCWSTR psz)
{
    SetWindowTextW(hwnd, psz);
}


void DialogApp::ProcessMessage(MSG* pMsg)
{
    if (m_hAccelTable && TranslateAcceleratorW(hwnd, m_hAccelTable, pMsg))
    {
        // OK
    }
    else if (IsDialogMessageW(hwnd, pMsg))
    {
        // OK
    }
    else
    {
        TranslateMessage(pMsg);
        DispatchMessageW(pMsg);
    }
}


void DialogApp::OnClose()
{
    PostQuitMessage(EXIT_SUCCESS);
}
