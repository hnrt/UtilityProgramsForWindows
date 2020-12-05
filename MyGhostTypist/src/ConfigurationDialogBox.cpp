#include "ConfigurationDialogBox.h"
#include <windowsx.h>
#include "FindWindowTarget.h"
#include "Action.h"
#include "hnrt/Debug.h"
#include "hnrt/Buffer.h"
#include "hnrt/ResourceString.h"
#include "hnrt/StringStore.h"
#include "hnrt/String.h"
#include "hnrt/VirtualKey.h"
#include "hnrt/WhileInScope.h"
#include "resource.h"


using namespace hnrt;


#define TV_INIT         0
#define TV_UNSELECTED   1
#define TV_CLIST        2
#define TV_CRED         3
#define TV_TLIST        4
#define TV_TARGET       5
#define TV_FINDWINDOW   6
#define TV_ACTIONITEM   7


static PCWSTR s_pszDefaultLabel = nullptr;


#define DEFAULT_CREDENTIALS s_pszDefaultLabel


ConfigurationDialogBox::ConfigurationDialogBox(HINSTANCE hInstance)
    : m_hInstance(hInstance)
    , m_hwndParent(NULL)
    , m_bCapturing(false)
    , m_tv(*this)
    , m_cbCredentials()
    , m_tvstate(TV_INIT)
    , m_bChanging(false)
{
    if (!s_pszDefaultLabel)
    {
        s_pszDefaultLabel = StringStore::Get(ResourceString(IDS_DEFAULT_LABEL));
    }
}


ConfigurationDialogBox::~ConfigurationDialogBox()
{
}


bool ConfigurationDialogBox::Run(HWND hwndParent)
{
    m_hwndParent = hwndParent;
    bool bRet = DialogBoxParamW(m_hInstance, MAKEINTRESOURCEW(IDD_CONFIGURATION), hwndParent, MessageCallback, reinterpret_cast<LPARAM>(this)) ? true : false;
    m_hwndParent = NULL;
    return bRet;
}


INT_PTR ConfigurationDialogBox::MessageCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ConfigurationDialogBox* pThis = reinterpret_cast<ConfigurationDialogBox*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (pThis)
    {
        switch (uMsg)
        {
        case WM_COMMAND:
            return pThis->OnCommand(hwnd, wParam, lParam);
        case WM_NOTIFY:
            return pThis->OnNotify(hwnd, wParam, lParam);
        case WM_LBUTTONDOWN:
            return pThis->OnLeftButtonDown(hwnd, wParam, lParam);
        default:
            break;
        }
    }
    else if (uMsg == WM_INITDIALOG)
    {
        pThis = reinterpret_cast<ConfigurationDialogBox*>(lParam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, lParam);
        pThis->OnCreate(hwnd);
        return TRUE;
    }
    return FALSE;
}


void ConfigurationDialogBox::OnCreate(HWND hwnd)
{
    m_tv.Init(GetDlgItem(hwnd, IDC_TREE));
    m_cbCredentials.Init(GetDlgItem(hwnd, IDC_CREDKEY_COMBO));
    OnTreeViewUnselected(hwnd);
}


INT_PTR ConfigurationDialogBox::OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case IDOK:
        EndDialog(hwnd, TRUE);
        return TRUE;

    case IDCANCEL:
        EndDialog(hwnd, FALSE);
        return TRUE;

    case IDC_UP_BUTTON:
        return OnUpButtonClicked(hwnd);

    case IDC_DOWN_BUTTON:
        return OnDownButtonClicked(hwnd);

    case IDC_ADD_BUTTON:
        return OnAddButtonClicked(hwnd);

    case IDC_REMOVE_BUTTON:
        return OnRemoveButtonClicked(hwnd);

    case IDC_USERNAME_EDIT:
    case IDC_PASSWORD_EDIT:
    case IDC_CREDKEY_EDIT:
    case IDC_TARGET_EDIT:
    case IDC_UNICODE_EDIT:
        return OnEditNotified(hwnd, LOWORD(wParam), HIWORD(wParam));

    case IDC_CREDKEY_COMBO:
        return OnComboBoxNotified(hwnd, LOWORD(wParam), HIWORD(wParam));

    case IDC_SHOWPASSWORD_CHECK:
        return OnShowPasswordClicked(hwnd);

    case IDC_FINDWINDOW_BUTTON:
        return OnFindWindowButtonClicked(hwnd);

    case IDC_USERNAME_BUTTON:
        return OnUsernameButtonClicked(hwnd);

    case IDC_PASSWORD_BUTTON:
        return OnPasswordButtonClicked(hwnd);

    case IDC_DELETESEQUENCE_BUTTON:
        return OnDeleteSequenceButtonClicked(hwnd);

    case IDC_UNICODE_BUTTON:
        return OnUnicodeButtonClicked(hwnd);

    case IDC_RETURN_BUTTON:
        return OnReturnButtonClicked(hwnd);

    case IDC_TAB_BUTTON:
        return OnTabButtonClicked(hwnd);

    default:
        break;
    }
    return FALSE;
}


INT_PTR ConfigurationDialogBox::OnNotify(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case IDC_TREE:
        m_tv.OnNotify(hwnd, wParam, lParam);
        return TRUE;
    default:
        return FALSE;
    }
}


INT_PTR ConfigurationDialogBox::OnUpButtonClicked(HWND hwnd)
{
    if (m_tv.IsCredentialsSelected)
    {
        m_tv.MoveUpSelectedCredentials();
    }
    else if (m_tv.IsActionItemSelected)
    {
        m_tv.MoveUpSelectedActionItem();
    }
    else if (m_tv.IsFindWindowSelected || m_tv.IsTargetSelected)
    {
        m_tv.MoveUpSelectedTarget();
    }
    return TRUE;
}


INT_PTR ConfigurationDialogBox::OnDownButtonClicked(HWND hwnd)
{
    if (m_tv.IsCredentialsSelected)
    {
        m_tv.MoveDownSelectedCredentials();
    }
    else if (m_tv.IsActionItemSelected)
    {
        m_tv.MoveDownSelectedActionItem();
    }
    else if (m_tv.IsFindWindowSelected || m_tv.IsTargetSelected)
    {
        m_tv.MoveDownSelectedTarget();
    }
    return TRUE;
}


INT_PTR ConfigurationDialogBox::OnAddButtonClicked(HWND hwnd)
{
    if (m_tv.IsCredentialsSelected || m_tv.IsCredentialsListSelected)
    {
        m_tv.AddCredentials();
        UpdateMoveButtons(hwnd);
        SetFocus(GetDlgItem(hwnd, IDC_USERNAME_EDIT));
        return TRUE;
    }
    else if (m_tv.IsActionItemSelected || m_tv.IsFindWindowSelected || m_tv.IsTargetSelected || m_tv.IsTargetListSelected)
    {
        m_tv.AddTarget();
        UpdateMoveButtons(hwnd);
        SetFocus(GetDlgItem(hwnd, IDC_TARGET_EDIT));
        return TRUE;
    }
    return FALSE;
}


INT_PTR ConfigurationDialogBox::OnRemoveButtonClicked(HWND hwnd)
{
    if (m_tv.IsCredentialsSelected)
    {
        m_tv.RemoveSelectedCredentials();
        return TRUE;
    }
    else if (m_tv.IsFindWindowSelected || m_tv.IsTargetSelected)
    {
        m_tv.RemoveSelectedTarget();
        return TRUE;
    }
    else if (m_tv.IsActionItemSelected)
    {
        m_tv.RemoveSelectedActionItem();
        return TRUE;
    }
    return FALSE;
}


INT_PTR ConfigurationDialogBox::OnEditNotified(HWND hwnd, WORD wControlId, WORD wNotification)
{
    if (!m_bChanging && wNotification == EN_CHANGE)
    {
        HWND hwndEdit = GetDlgItem(hwnd, wControlId);
        LRESULT n = GetWindowTextLengthW(hwndEdit);
        Buffer<wchar_t> buf(n + 1);
        GetWindowTextW(hwndEdit, &buf, static_cast<int>(n + 1));
        switch (wControlId)
        {
        case IDC_USERNAME_EDIT:
            if (m_tv.IsCredentialsSelected)
            {
                m_tv.SelectedCredentials->Username = buf;
            }
            return TRUE;
        case IDC_PASSWORD_EDIT:
            if (m_tv.IsCredentialsSelected)
            {
                m_tv.SelectedCredentials->Password = buf;
            }
            return TRUE;
        case IDC_CREDKEY_EDIT:
            if (m_tv.IsCredentialsSelected)
            {
                m_tv.SelectedCredentials->Key = buf;
            }
            return TRUE;
        case IDC_TARGET_EDIT:
            if (m_tv.IsTargetSelected || m_tv.IsFindWindowSelected || m_tv.IsActionItemSelected)
            {
                m_tv.SelectedTarget->Name = buf;
            }
            return TRUE;
        case IDC_UNICODE_EDIT:
            if (m_tv.IsActionItemSelected && m_tv.SelectedTarget->IsTypeAction)
            {
                PCWSTR psz = m_tv.SelectedActionItem;
                if (Action::GetClass(psz) == AC_TYPEUNICODE)
                {
                    m_tv.SelectedActionItem = Action::TypeUnicode(buf);
                }
            }
            return TRUE;
        default:
            break;
        }
    }
    return FALSE;
}


INT_PTR ConfigurationDialogBox::OnComboBoxNotified(HWND hwnd, WORD wControlId, WORD wNotification)
{
    if (!m_bChanging && (wNotification == CBN_SELCHANGE || wNotification == CBN_EDITCHANGE))
    {
        switch (wControlId)
        {
        case IDC_CREDKEY_COMBO:
            if (m_tv.IsActionItemSelected && m_tv.SelectedTarget->IsTypeAction)
            {
                PCWSTR pszSelected = m_cbCredentials.Selected;
                PCWSTR pszAction = m_tv.SelectedActionItem;
                switch (Action::GetClass(pszAction))
                {
                case AC_TYPEUSERNAME:
                    if (*pszSelected && wcscmp(pszSelected, DEFAULT_CREDENTIALS))
                    {
                        m_tv.SelectedActionItem = Action::TypeUsername(pszSelected);
                    }
                    else
                    {
                        m_tv.SelectedActionItem = Action::TypeUsername();
                    }
                    break;
                case AC_TYPEPASSWORD:
                    if (*pszSelected && wcscmp(pszSelected, DEFAULT_CREDENTIALS))
                    {
                        m_tv.SelectedActionItem = Action::TypePassword(pszSelected);
                    }
                    else
                    {
                        m_tv.SelectedActionItem = Action::TypePassword();
                    }
                    break;
                default:
                    break;
                }
            }
            return TRUE;
        default:
            break;
        }
    }
    return FALSE;
}


INT_PTR ConfigurationDialogBox::OnShowPasswordClicked(HWND hwnd)
{
    WPARAM c = SendDlgItemMessageW(hwnd, IDC_SHOWPASSWORD_CHECK, BM_GETCHECK, 0, 0) == BST_CHECKED ? 0 : L'*';
    HWND hwndEdit = GetDlgItem(hwnd, IDC_PASSWORD_EDIT);
    SendMessageW(hwndEdit, EM_SETPASSWORDCHAR, c, 0);
    SetFocus(hwndEdit);
    return TRUE;
}


INT_PTR ConfigurationDialogBox::OnFindWindowButtonClicked(HWND hwnd)
{
    m_bCapturing = true;
    SetCapture(hwnd);
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    return TRUE;
}


INT_PTR ConfigurationDialogBox::OnLeftButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    DBGFNC(L"WM_LBUTTONDOWN");
    if (m_bCapturing)
    {
        m_bCapturing = false;
        ReleaseCapture();
        LONG x = GET_X_LPARAM(lParam);
        LONG y = GET_Y_LPARAM(lParam);
        DBGPUT(L"relative=(%ld,%ld)", x, y);
        POINT ptBase = { 0, 0 };
        ClientToScreen(hwnd, &ptBase);
        x += ptBase.x;
        y += ptBase.y;
        DBGPUT(L"absolute=(%ld,%ld)", x, y);
        POINT pt = { x, y };
        HWND hwndInterest = WindowFromPoint(pt);
        if (hwndInterest)
        {
            RefPtr<Target> pTarget = m_tv.SelectedTarget;
            if (pTarget->IsTypeNull)
            {
                m_tv.AddFindWindow(L"", L""); // discards current pTarget - needs to be refreshed
                pTarget = m_tv.SelectedTarget;
            }
            DWORD dwProcessId = 0;
            DWORD dwThreadId = GetWindowThreadProcessId(hwndInterest, &dwProcessId);
            WCHAR szClassName[MAX_PATH] = { 0 };
            WCHAR szWindowText[MAX_PATH] = { 0 };
            GetClassNameW(hwndInterest, szClassName, _countof(szClassName));
            GetWindowTextW(hwndInterest, szWindowText, _countof(szWindowText));
            DBGPUT(L"class=\"%s\" text=\"%s\" process=%lu thread=%lu", szClassName, szWindowText, dwProcessId, dwThreadId);
            if (FindWindowW(szClassName, szWindowText) == hwndInterest || !(hwndInterest = GetParent(hwndInterest)))
            {
                pTarget->FindWindowTargetPtr->Set(szClassName, szWindowText);
            }
            else
            {
                DBGPUT(L"Checking parent...");
                WCHAR szClassName0[MAX_PATH];
                WCHAR szWindowText0[MAX_PATH];
                wcscpy_s(szClassName0, szClassName);
                wcscpy_s(szWindowText0, szWindowText);
                while (true)
                {
                    dwProcessId = 0;
                    dwThreadId = GetWindowThreadProcessId(hwndInterest, &dwProcessId);
                    GetClassNameW(hwndInterest, szClassName, _countof(szClassName));
                    GetWindowTextW(hwndInterest, szWindowText, _countof(szWindowText));
                    DBGPUT(L"class=\"%s\" text=\"%s\" process=%lu thread=%lu", szClassName, szWindowText, dwProcessId, dwThreadId);
                    if (FindWindowW(szClassName, szWindowText) == hwndInterest || !(hwndInterest = GetParent(hwndInterest)))
                    {
                        break;
                    }
                }
                pTarget->FindWindowTargetPtr->Set(szClassName, szWindowText, szClassName0, szWindowText0);
            }
            OnFindWindowSelected(hwnd, pTarget->FindWindowTargetPtr);
        }
        SetForegroundWindow(hwnd);
        return TRUE;
    }
    return FALSE;
}


INT_PTR ConfigurationDialogBox::OnUsernameButtonClicked(HWND hwnd)
{
    if (m_tv.IsActionItemSelected || m_tv.IsFindWindowSelected || m_tv.IsTargetSelected)
    {
        PCWSTR pszSelected = m_cbCredentials.Selected;
        if (*pszSelected && wcscmp(pszSelected, DEFAULT_CREDENTIALS))
        {
            m_tv.AddActionItem(Action::TypeUsername(pszSelected));
        }
        else
        {
            m_tv.AddActionItem(Action::TypeUsername());
        }
    }
    return TRUE;
}


INT_PTR ConfigurationDialogBox::OnPasswordButtonClicked(HWND hwnd)
{
    if (m_tv.IsActionItemSelected || m_tv.IsFindWindowSelected || m_tv.IsTargetSelected)
    {
        PCWSTR pszSelected = m_cbCredentials.Selected;
        if (*pszSelected && wcscmp(pszSelected, DEFAULT_CREDENTIALS))
        {
            m_tv.AddActionItem(Action::TypePassword(pszSelected));
        }
        else
        {
            m_tv.AddActionItem(Action::TypePassword());
        }
    }
    return TRUE;
}


INT_PTR ConfigurationDialogBox::OnDeleteSequenceButtonClicked(HWND hwnd)
{
    if (m_tv.IsActionItemSelected || m_tv.IsFindWindowSelected || m_tv.IsTargetSelected)
    {
        m_tv.AddActionItem(Action::TypeDeleteSequence());
    }
    return TRUE;
}


INT_PTR ConfigurationDialogBox::OnUnicodeButtonClicked(HWND hwnd)
{
    if (m_tv.IsActionItemSelected || m_tv.IsFindWindowSelected || m_tv.IsTargetSelected)
    {
        m_tv.AddActionItem(Action::TypeUnicode(L""));
        SetFocus(GetDlgItem(hwnd, IDC_UNICODE_EDIT));
    }
    return TRUE;
}


INT_PTR ConfigurationDialogBox::OnReturnButtonClicked(HWND hwnd)
{
    if (m_tv.IsActionItemSelected || m_tv.IsFindWindowSelected || m_tv.IsTargetSelected)
    {
        m_tv.AddActionItem(VirtualKey(static_cast<unsigned char>(VK_RETURN)).Name);
    }
    return TRUE;
}


INT_PTR ConfigurationDialogBox::OnTabButtonClicked(HWND hwnd)
{
    if (m_tv.IsActionItemSelected || m_tv.IsFindWindowSelected || m_tv.IsTargetSelected)
    {
        m_tv.AddActionItem(VirtualKey(static_cast<unsigned char>(VK_TAB)).Name);
    }
    return TRUE;
}


void ConfigurationDialogBox::OnTreeViewUnselected(HWND hwnd)
{
    DBGFNC(L"ConfigurationDialogBox::OnTreeViewUnselected");
    WhileInScope<bool> wis(m_bChanging, true, false);
    LONG prev = InterlockedExchange(&m_tvstate, TV_UNSELECTED);
    if (prev != TV_UNSELECTED)
    {
        EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_ADD_BUTTON), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_BUTTON), FALSE);
        // Credentials
        DisableCredentialsGroup(hwnd);
        // Target
        DisableTargetGroup(hwnd);
    }
}


void ConfigurationDialogBox::OnCredentialsListSelected(HWND hwnd)
{
    DBGFNC(L"ConfigurationDialogBox::OnCredentialsListSelected");
    WhileInScope<bool> wis(m_bChanging, true, false);;
    LONG prev = InterlockedExchange(&m_tvstate, TV_CLIST);
    if (prev != TV_CLIST)
    {
        EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_ADD_BUTTON), TRUE);
        EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_BUTTON), FALSE);
    }
    // Credentials
    if (prev != TV_CLIST)
    {
        DisableCredentialsGroup(hwnd);
    }
    // Target
    if (prev != TV_CRED && prev != TV_CLIST)
    {
        DisableTargetGroup(hwnd);
    }
}


void ConfigurationDialogBox::OnCredentialsSelected(HWND hwnd, RefPtr<Credentials> pC)
{
    DBGFNC(L"ConfigurationDialogBox::OnCredentialsSelected");
    DBGPUT(L"%s %s", pC->Username, pC->Key ? pC->Key : L"");
    WhileInScope<bool> wis(m_bChanging, true, false);;
    LONG prev = InterlockedExchange(&m_tvstate, TV_CRED);
    EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), pC.Ptr != m_tv.CredentialsList[(size_t)0].Ptr ? TRUE : FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), pC.Ptr != m_tv.CredentialsList[m_tv.CredentialsList.Count - 1].Ptr ? TRUE : FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_ADD_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_BUTTON), m_tv.CredentialsList.Count > 1 ? TRUE : FALSE);
    // Credentials
    if (prev != TV_CRED)
    {
        EnableWindow(GetDlgItem(hwnd, IDC_USERNAME_EDIT), TRUE);
        EnableWindow(GetDlgItem(hwnd, IDC_PASSWORD_EDIT), TRUE);
        EnableWindow(GetDlgItem(hwnd, IDC_SHOWPASSWORD_CHECK), TRUE);
        EnableWindow(GetDlgItem(hwnd, IDC_CREDKEY_EDIT), TRUE);
    }
    SetDlgItemTextW(hwnd, IDC_USERNAME_EDIT, pC->Username);
    SetDlgItemTextW(hwnd, IDC_PASSWORD_EDIT, pC->Password);
    pC->ClearPlainText();
    SetDlgItemTextW(hwnd, IDC_CREDKEY_EDIT, pC->Key ? pC->Key : L"");
    // Target
    if (prev != TV_CRED && prev != TV_CLIST)
    {
        DisableTargetGroup(hwnd);
    }
}


void ConfigurationDialogBox::OnTargetListSelected(HWND hwnd)
{
    DBGFNC(L"ConfigurationDialogBox::OnTargetListSelected");
    WhileInScope<bool> wis(m_bChanging, true, false);;
    LONG prev = InterlockedExchange(&m_tvstate, TV_TLIST);
    if (prev != TV_TLIST)
    {
        EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_ADD_BUTTON), TRUE);
        EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_BUTTON), FALSE);
    }
    // Credentials
    if (prev == TV_CRED)
    {
        DisableCredentialsGroup(hwnd);
    }
    // Target
    if (prev != TV_TLIST)
    {
        DisableTargetGroup(hwnd);
    }
}


void ConfigurationDialogBox::OnTargetSelected(HWND hwnd, RefPtr<Target> pTarget)
{
    DBGFNC(L"ConfigurationDialogBox::OnTargetSelected");
    DBGPUT(L"%s", pTarget->Name);
    WhileInScope<bool> wis(m_bChanging, true, false);;
    LONG prev = InterlockedExchange(&m_tvstate, TV_TARGET);
    EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), pTarget.Ptr != m_tv.TargetList[0].Ptr ? TRUE : FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), pTarget.Ptr != m_tv.TargetList[m_tv.TargetList.Count - 1].Ptr ? TRUE : FALSE);
    if (prev != TV_ACTIONITEM && prev != TV_FINDWINDOW && prev != TV_TARGET)
    {
        EnableWindow(GetDlgItem(hwnd, IDC_ADD_BUTTON), TRUE);
        EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_BUTTON), TRUE);
    }
    // Credentials
    if (prev == TV_CRED)
    {
        DisableCredentialsGroup(hwnd);
    }
    // Target
    EnableWindow(GetDlgItem(hwnd, IDC_TARGET_EDIT), TRUE);
    BOOL bCanType = pTarget->IsTypeFindWindow ? TRUE : FALSE;
    EnableWindow(GetDlgItem(hwnd, IDC_FINDWINDOW_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_USERNAME_BUTTON), bCanType);
    EnableWindow(GetDlgItem(hwnd, IDC_PASSWORD_BUTTON), bCanType);
    EnableWindow(GetDlgItem(hwnd, IDC_CREDKEY_COMBO), bCanType);
    EnableWindow(GetDlgItem(hwnd, IDC_UNICODE_EDIT), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_UNICODE_BUTTON), bCanType);
    EnableWindow(GetDlgItem(hwnd, IDC_RETURN_BUTTON), bCanType);
    EnableWindow(GetDlgItem(hwnd, IDC_TAB_BUTTON), bCanType);
    EnableWindow(GetDlgItem(hwnd, IDC_DELETESEQUENCE_BUTTON), bCanType);
    SetDlgItemTextW(hwnd, IDC_TARGET_EDIT, pTarget->Name);
    SetDlgItemTextW(hwnd, IDC_UNICODE_EDIT, L"");
    if (prev == TV_TLIST || prev == TV_CRED || prev == TV_CLIST || prev == TV_UNSELECTED)
    {
        LoadCredKeyCombo(hwnd);
    }
}


void ConfigurationDialogBox::OnFindWindowSelected(HWND hwnd, FindWindowTarget* pTarget)
{
    DBGFNC(L"ConfigurationDialogBox::OnFindWindowSelected");
    DBGPUT(L"%s", pTarget->Name);
    WhileInScope<bool> wis(m_bChanging, true, false);;
    LONG prev = InterlockedExchange(&m_tvstate, TV_FINDWINDOW);
    EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), FALSE);
    if (prev != TV_ACTIONITEM && prev != TV_FINDWINDOW && prev != TV_TARGET)
    {
        EnableWindow(GetDlgItem(hwnd, IDC_ADD_BUTTON), TRUE);
        EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_BUTTON), TRUE);
    }
    // Credentials
    if (prev == TV_CRED)
    {
        DisableCredentialsGroup(hwnd);
    }
    // Target
    EnableWindow(GetDlgItem(hwnd, IDC_TARGET_EDIT), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_FINDWINDOW_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_USERNAME_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_PASSWORD_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_CREDKEY_COMBO), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_UNICODE_EDIT), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_UNICODE_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_RETURN_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_TAB_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_DELETESEQUENCE_BUTTON), TRUE);
    SetDlgItemTextW(hwnd, IDC_TARGET_EDIT, pTarget->Name);
    SetDlgItemTextW(hwnd, IDC_UNICODE_EDIT, L"");
    if (prev == TV_CRED || prev == TV_CLIST || prev == TV_UNSELECTED)
    {
        LoadCredKeyCombo(hwnd);
    }
}


void ConfigurationDialogBox::OnActionItemSelected(HWND hwnd, ActionTarget* pTarget, size_t index)
{
    DBGFNC(L"ConfigurationDialogBox::OnTargetItemSelected");
    DBGPUT(L"%s: %s", pTarget->Name, (*pTarget)[index]);
    WhileInScope<bool> wis(m_bChanging, true, false);;
    LONG prev = InterlockedExchange(&m_tvstate, TV_ACTIONITEM);
    EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), index > 0 ? TRUE : FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), index + 1 < pTarget->Count ? TRUE : FALSE);
    if (prev != TV_ACTIONITEM && prev != TV_FINDWINDOW && prev != TV_TARGET)
    {
        EnableWindow(GetDlgItem(hwnd, IDC_ADD_BUTTON), TRUE);
        EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_BUTTON), TRUE);
    }
    // Credentials
    if (prev == TV_CRED)
    {
        DisableCredentialsGroup(hwnd);
    }
    // Target
    EnableWindow(GetDlgItem(hwnd, IDC_TARGET_EDIT), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_FINDWINDOW_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_USERNAME_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_PASSWORD_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_CREDKEY_COMBO), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_UNICODE_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_RETURN_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_TAB_BUTTON), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_DELETESEQUENCE_BUTTON), TRUE);
    SetDlgItemTextW(hwnd, IDC_TARGET_EDIT, pTarget->Name);
    if (prev == TV_CRED || prev == TV_CLIST || prev == TV_UNSELECTED)
    {
        LoadCredKeyCombo(hwnd);
    }
    PCWSTR pszAction = (*pTarget)[index];
    PCWSTR pszValue;
    switch (Action::GetClass(pszAction))
    {
    case AC_TYPEUSERNAME:
    case AC_TYPEPASSWORD:
        pszValue = Action::GetValue(pszAction);
        if (pszValue && *pszValue)
        {
            m_cbCredentials.Set(pszValue);
        }
        else
        {
            m_cbCredentials.Select(DEFAULT_CREDENTIALS);
        }
        EnableWindow(GetDlgItem(hwnd, IDC_UNICODE_EDIT), FALSE);
        SetDlgItemTextW(hwnd, IDC_UNICODE_EDIT, L"");
        break;
    case AC_TYPEUNICODE:
        pszValue = Action::GetValue(pszAction);
        EnableWindow(GetDlgItem(hwnd, IDC_UNICODE_EDIT), TRUE);
        SetDlgItemTextW(hwnd, IDC_UNICODE_EDIT, pszValue);
        break;
    default:
        EnableWindow(GetDlgItem(hwnd, IDC_UNICODE_EDIT), FALSE);
        SetDlgItemTextW(hwnd, IDC_UNICODE_EDIT, L"");
        break;
    }
}


void ConfigurationDialogBox::DisableCredentialsGroup(HWND hwnd)
{
    EnableWindow(GetDlgItem(hwnd, IDC_USERNAME_EDIT), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_PASSWORD_EDIT), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_SHOWPASSWORD_CHECK), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_CREDKEY_EDIT), FALSE);
    SetDlgItemTextW(hwnd, IDC_USERNAME_EDIT, L"");
    SetDlgItemTextW(hwnd, IDC_PASSWORD_EDIT, L"");
    SetDlgItemTextW(hwnd, IDC_CREDKEY_EDIT, L"");
}


void ConfigurationDialogBox::DisableTargetGroup(HWND hwnd)
{
    EnableWindow(GetDlgItem(hwnd, IDC_TARGET_EDIT), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_FINDWINDOW_BUTTON), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_USERNAME_BUTTON), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_PASSWORD_BUTTON), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_CREDKEY_COMBO), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_UNICODE_EDIT), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_UNICODE_BUTTON), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_RETURN_BUTTON), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_TAB_BUTTON), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_DELETESEQUENCE_BUTTON), FALSE);
    SetDlgItemTextW(hwnd, IDC_TARGET_EDIT, L"");
    SetDlgItemTextW(hwnd, IDC_UNICODE_EDIT, L"");
    m_cbCredentials.Clear();
}


void ConfigurationDialogBox::UpdateMoveButtons(HWND hwnd)
{
    if (m_tv.IsCredentialsSelected)
    {
        size_t index = m_tv.SelectedCredentialsIndex;
        EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), index > 0 ? TRUE : FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), index < m_tv.CredentialsList.Count - 1 ? TRUE : FALSE);
    }
    else if (m_tv.IsTargetSelected || m_tv.IsFindWindowSelected)
    {
        size_t index = m_tv.SelectedTargetIndex;
        EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), index > 0 ? TRUE : FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), index < m_tv.TargetList.Count - 1 ? TRUE : FALSE);
    }
    else if (m_tv.IsActionItemSelected)
    {
        size_t index = m_tv.SelectedActionItemIndex;
        EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), index > 0 ? TRUE : FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), index < m_tv.SelectedTarget->ActionTargetPtr->Count - 1 ? TRUE : FALSE);
    }
    else
    {
        EnableWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), FALSE);
    }
}


void ConfigurationDialogBox::LoadCredKeyCombo(HWND hwnd)
{
    m_cbCredentials.Clear();
    m_cbCredentials.Add(DEFAULT_CREDENTIALS);
    for (size_t index = 0; index < m_tv.CredentialsList.Count; index++)
    {
        if (m_tv.CredentialsList[index]->Key && m_tv.CredentialsList[index]->Key[0])
        {
            m_cbCredentials.Add(m_tv.CredentialsList[index]->Key);
        }
    }
    if (m_tv.IsActionItemSelected || m_tv.IsFindWindowSelected || m_tv.IsTargetSelected)
    {
        RefPtr<Target> pTarget = m_tv.SelectedTarget;
        if (pTarget->IsTypeAction)
        {
            for (ActionListConstIter iter = pTarget->ActionTargetPtr->Begin(); iter != pTarget->ActionTargetPtr->End(); iter++)
            {
                PCWSTR psz = *iter;
                switch (Action::GetClass(psz))
                {
                case AC_TYPEUSERNAME:
                case AC_TYPEPASSWORD:
                    psz = Action::GetValue(psz);
                    if (!m_cbCredentials.Select(psz ? psz : DEFAULT_CREDENTIALS))
                    {
                        m_cbCredentials.Set(psz ? psz : DEFAULT_CREDENTIALS);
                    }
                    return;
                default:
                    break;
                }
            }
        }
    }
    m_cbCredentials.Select(DEFAULT_CREDENTIALS);
}
