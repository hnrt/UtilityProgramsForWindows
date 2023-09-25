#include "pch.h"
#include "hnrt/DialogBox.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/WindowDesign.h"


using namespace hnrt;


DialogBox::DialogBox(UINT idTemplate)
    : WindowSize()
    , WindowLayout()
    , m_hwnd(nullptr)
    , m_idTemplate(idTemplate)
    , m_hFont()
{
}


DialogBox::~DialogBox()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
    }
}


void DialogBox::Open(HINSTANCE hInstance)
{
    if (!CreateDialogParamW(hInstance, MAKEINTRESOURCE(m_idTemplate), NULL, MessageCallback, reinterpret_cast<LPARAM>(this)))
    {
        throw Win32Exception(GetLastError(), L"CreateDialog failed.");
    }
}


void DialogBox::Open(HWND hwndParent)
{
    RectangleMetrics rect;
    rect.FromClient(hwndParent);
    DialogBox::Open(hwndParent, 0, 0, rect.cx, rect.cy);
}


void DialogBox::Open(HWND hwndParent, int cx, int cy)
{
    DialogBox::Open(hwndParent, 0, 0, cx, cy);
}


void DialogBox::Open(HWND hwndParent, int x, int y, int cx, int cy)
{
    HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hwndParent, GWLP_HINSTANCE));
    if (!CreateDialogParamW(hInstance, MAKEINTRESOURCE(m_idTemplate), hwndParent, MessageCallback, reinterpret_cast<LPARAM>(this)))
    {
        throw Win32Exception(GetLastError(), L"CreateDialog failed.");
    }
    SetWindowPos(m_hwnd, NULL, x, y, cx, cy, SWP_NOZORDER);
}


void DialogBox::Close()
{
    DestroyWindow(m_hwnd);
}


static BOOL CALLBACK ApplyFontToChild(HWND hwnd, LPARAM lParam)
{
    SendMessageW(hwnd, WM_SETFONT, static_cast<WPARAM>(lParam), TRUE);
    return TRUE;
}


void DialogBox::SetFont(HFONT hFont)
{
    m_hFont = hFont;
    EnumChildWindows(m_hwnd, ApplyFontToChild, reinterpret_cast<LPARAM>(hFont));
}


BOOL DialogBox::EnableWindow(int id, BOOL bEnabled)
{
    return ::EnableWindow(GetDlgItem(m_hwnd, id), bEnabled);
}


BOOL DialogBox::DisableWindow(int id)
{
    return ::EnableWindow(GetDlgItem(m_hwnd, id), FALSE);
}


LRESULT DialogBox::SendMessage(int id, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return SendDlgItemMessageW(m_hwnd, id, msg, wParam, lParam);
}


INT_PTR CALLBACK DialogBox::MessageCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        DialogBox* pThis = reinterpret_cast<DialogBox*>(lParam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        pThis->OnCreate(hwnd);
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
        return FALSE;
    }
    return TRUE;
}


DialogBox* DialogBox::GetInstance(HWND hwnd)
{
    return reinterpret_cast<DialogBox*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}


void DialogBox::OnCreate(HWND hwnd)
{
    m_hwnd = hwnd;
    InitializeSize(hwnd);
    OnCreate();
}


void DialogBox::OnDestroy(HWND hwnd)
{
    UNREFERENCED_PARAMETER(hwnd);
    OnDestroy();
    m_hwnd = nullptr;
}


void DialogBox::OnCreate()
{
}


void DialogBox::OnDestroy()
{
}


void DialogBox::OnClose()
{
}


void DialogBox::OnSize(WPARAM wParam, LPARAM lParam)
{
    WindowSize::OnSize(m_hwnd, wParam, lParam, *this);
}


INT_PTR DialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return FALSE;
}


INT_PTR DialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return FALSE;
}


INT_PTR DialogBox::OnNotify(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return FALSE;
}
