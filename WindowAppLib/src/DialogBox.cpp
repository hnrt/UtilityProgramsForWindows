#include "pch.h"
#include "hnrt/DialogBox.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/WindowHandle.h"
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
    if (!CreateDialogParamW(GetInstanceHandle(hwndParent), MAKEINTRESOURCE(m_idTemplate), hwndParent, MessageCallback, reinterpret_cast<LPARAM>(this)))
    {
        throw Win32Exception(GetLastError(), L"CreateDialog failed.");
    }
    SetWindowGeometry(m_hwnd, x, y, cx, cy);
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


UINT DialogBox::GetTextLength(int id)
{
    return static_cast<UINT>(SendMessage(id, WM_GETTEXTLENGTH));
}


PWCHAR DialogBox::GetText(int id, PWCHAR pBuf, size_t cch)
{
    SendMessage(id, WM_GETTEXT, cch, reinterpret_cast<LPARAM>(pBuf));
    return pBuf;
}


void DialogBox::SetText(int id, PCWSTR psz)
{
    SendMessage(id, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(psz ? psz : L""));
}


void DialogBox::CheckButton(int id, BOOL bCheck)
{
    SendMessage(id, BM_SETCHECK, bCheck ? BST_CHECKED : BST_UNCHECKED);
}


void DialogBox::UncheckButton(int id)
{
    SendMessage(id, BM_SETCHECK, BST_UNCHECKED);
}


int DialogBox::GetButtonState(int id)
{
    return static_cast<int>(SendMessage(id, BM_GETCHECK));
}


void DialogBox::AddStringToComboBox(int id, PCWSTR psz)
{
    SendMessage(id, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(psz));
}


int DialogBox::GetComboBoxSelection(int id, int defaultValue)
{
    LRESULT selected = SendMessage(id, CB_GETCURSEL);
    return selected != CB_ERR ? static_cast<int>(selected) : defaultValue;
}


void DialogBox::SetComboBoxSelection(int id, int index)
{
    LRESULT count = SendMessage(id, LB_GETCOUNT);
    if (count > 0)
    {
        if (index >= 0)
        {
            if (index < count)
            {
                SendMessage(id, CB_SETCURSEL, index);
            }
        }
        else
        {
            if (count + index >= 0)
            {
                SendMessage(id, CB_SETCURSEL, count + index);
            }
        }
    }
}


void DialogBox::SetComboBoxSelection(int id, PCWSTR psz)
{
    SendMessage(id, CB_SELECTSTRING, 0, reinterpret_cast<LPARAM>(psz));
}


void DialogBox::ClearComboBoxSelection(int id)
{
    SendMessage(id, CB_SETCURSEL, -1);
}


UINT DialogBox::GetListBoxTextLength(int id, int index, size_t defaultValue)
{
    LRESULT length = SendMessage(id, CB_GETLBTEXTLEN, index);
    return length != CB_ERR ? static_cast<UINT>(length) : static_cast<UINT>(defaultValue);

}


PWCHAR DialogBox::GetListBoxText(int id, int index, PWCHAR pBuf, PCWSTR pszDefault)
{
    LRESULT length = SendMessage(id, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(pBuf));
    if (length == CB_ERR)
    {
        wcscpy_s(pBuf, wcslen(pszDefault) + 1, pszDefault);
    }
    return pBuf;
}


INT_PTR CALLBACK DialogBox::MessageCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        SetWindowUserData<DialogBox>(hwnd, lParam)->OnCreate(hwnd);
        break;
    }
    case WM_DESTROY:
        ExchangeWindowUserData<DialogBox>(hwnd, nullptr)->OnDestroy(hwnd);
        break;
    case WM_CLOSE:
        GetWindowUserData<DialogBox>(hwnd)->OnClose();
        break;
    case WM_SIZE:
        GetWindowUserData<DialogBox>(hwnd)->OnSize(wParam, lParam);
        break;
    case WM_COMMAND:
        return GetWindowUserData<DialogBox>(hwnd)->OnCommand(wParam, lParam);
    case WM_TIMER:
        return GetWindowUserData<DialogBox>(hwnd)->OnTimer(wParam, lParam);
    case WM_NOTIFY:
        return GetWindowUserData<DialogBox>(hwnd)->OnNotify(wParam, lParam);
    case WM_CTLCOLORSTATIC:
        return GetWindowUserData<DialogBox>(hwnd)->OnControlColorStatic(wParam, lParam);
    default:
        return FALSE;
    }
    return TRUE;
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


INT_PTR DialogBox::OnNotify(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return FALSE;
}


INT_PTR DialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return 0;
}


INT_PTR DialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    return FALSE;
}
