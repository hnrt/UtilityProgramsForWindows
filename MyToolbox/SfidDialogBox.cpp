#include "pch.h"
#include "SfidDialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Clipboard.h"
#include "hnrt/Exception.h"
#include "hnrt/Time.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/Buffer.h"
#include "hnrt/SFID.h"
#include "hnrt/SFIDException.h"
#include "hnrt/StringCommons.h"


#define REGVAL_LAST L"Last"


#define STATE_SUCCESSFUL 1
#define STATE_ERROR 2
#define STATE_CHANGING 3


using namespace hnrt;


SfidDialogBox::SfidDialogBox()
    : MyDialogBox(IDD_SFID, L"SFID")
    , m_State(STATE_SUCCESSFUL)
{
    m_LastModified.CursorChange = true;
}


void SfidDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    HFONT hFont = GetApp<MyToolbox>().GetFontForData();
    SetFont(IDC_SFID_EDIT, hFont);
    SetFont(IDC_SFID_KEYPREFIX_EDIT, hFont);
    SetFont(IDC_SFID_INSTANCE_EDIT, hFont);
    SetFont(IDC_SFID_UNIQUEID_EDIT, hFont);
    SetFont(IDC_SFID_CHECKSUM_EDIT, hFont);
    SetText(IDC_SFID_STATUS_STATIC);
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
        SetText(IDC_SFID_EDIT, RegistryValue::GetSZ(hKey, REGVAL_LAST));
    }
    ApplyModification(IDC_SFID_EDIT);
    m_menuView
        .Add(ResourceString(IDS_MENU_SFID), IDM_VIEW_SFID);
}


void SfidDialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_WRITE);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetSZ(hKey, REGVAL_LAST, GetText(IDC_SFID_EDIT));
    }
    MyDialogBox::OnDestroy();
}


void SfidDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_EDIT, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_STATUS_STATIC, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_COPY_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_UP_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_DN_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_RN_BUTTON, cxDelta, 0, 0, 0);
}


void SfidDialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
    if (m_LastModified)
    {
        ApplyModification(m_LastModified.By);
    }
    m_menuView
        .Enable(IDM_VIEW_SFID, MF_ENABLED);
}


void SfidDialogBox::OnTabSelectionChanged()
{
    MyDialogBox::OnTabSelectionChanged();
    m_menuEdit
        .Add(ResourceString(IDS_MENU_COPYALL), IDM_EDIT_COPYALL)
        .AddSeparator();
    AddEditControlMenus(m_CurrentEdit);
    m_menuEdit
        .AddSeparator()
        .Add(ResourceString(IDS_MENU_RENEW), IDM_EDIT_EXECUTE);
    m_menuView
        .Enable(IDM_VIEW_SFID, MF_DISABLED);
    UpdateControlsState();
    SetTimer(100);
    srand(static_cast<unsigned int>(FileTime().Intervals));
}


INT_PTR SfidDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (m_cProcessing)
    {
        return TRUE;
    }
    UNREFERENCED_PARAMETER(lParam);
    UINT idChild = LOWORD(wParam);
    UINT idNotif = HIWORD(wParam);
    switch (idChild)
    {
    case IDC_SFID_COPY_BUTTON:
        OnCopyAll();
        break;
    case IDC_SFID_UP_BUTTON:
        ChangeContent(1);
        break;
    case IDC_SFID_DN_BUTTON:
        ChangeContent(-1);
        break;
    case IDC_SFID_RN_BUTTON:
        OnExecute();
        break;
    case IDC_SFID_EDIT:
    case IDC_SFID_KEYPREFIX_EDIT:
    case IDC_SFID_INSTANCE_EDIT:
    case IDC_SFID_UNIQUEID_EDIT:
    case IDC_SFID_CHECKSUM_EDIT:
        if (idNotif == EN_CHANGE)
        {
            OnEditChanged(idChild);
        }
        else if (idNotif == EN_SETFOCUS)
        {
            OnEditSetFocus(idChild);
        }
        else if (idNotif == EN_KILLFOCUS)
        {
            OnEditKillFocus(idChild);
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


INT_PTR SfidDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
    if (wParam == TIMERID(Id, 100))
    {
        if (m_LastModified.IsUpdateRequired)
        {
            ApplyModification(m_LastModified.By);
        }
    }
    else
    {
        MyDialogBox::OnTimer(wParam, lParam);
    }
    return 0;
}


INT_PTR SfidDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    int length;
    COLORREF color;
    switch (id)
    {
    case IDC_SFID_STATUS_STATIC:
        SetTextColor(hdc,
            m_State == STATE_SUCCESSFUL ? RGB_SUCCESSFUL :
            m_State == STATE_ERROR ? RGB_ERROR :
            m_State == STATE_CHANGING ? RGB_CHANGING :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    case IDC_SFID_LENGTH_STATIC:
        length = GetTextLength(IDC_SFID_EDIT);
        if (length == 0)
        {
            color = m_State == STATE_CHANGING ? RGB_ERROR : GetSysColor(COLOR_WINDOWTEXT);
        }
        else if (length == SFID_LENGTH_EXCLUDING_CHECKSUM || length == SFID_LENGTH)
        {
            color = m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT);
        }
        else if (SFID_LENGTH_EXCLUDING_CHECKSUM < length && length < SFID_LENGTH)
        {
            color = m_State == STATE_CHANGING ? RGB_TOO_MANY : RGB_ERROR;
        }
        else
        {
            color = RGB_ERROR;
        }
        SetTextColor(hdc, color);
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    case IDC_SFID_KLENGTH_STATIC:
        length = GetTextLength(IDC_SFID_KEYPREFIX_EDIT);
        if (length == 0 || length == SFID_KEYPREFIX_LENGTH)
        {
            color = m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT);
        }
        else
        {
            color = RGB_ERROR;
        }
        SetTextColor(hdc, color);
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    case IDC_SFID_ILENGTH_STATIC:
        length = GetTextLength(IDC_SFID_INSTANCE_EDIT);
        if (length == 0 || length == SFID_INSTANCE_LENGTH)
        {
            color = m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT);
        }
        else
        {
            color = RGB_ERROR;
        }
        SetTextColor(hdc, color);
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    case IDC_SFID_ULENGTH_STATIC:
        length = GetTextLength(IDC_SFID_UNIQUEID_EDIT);
        if (length == 0 || length == SFID_UNIQUEID_LENGTH)
        {
            color = m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT);
        }
        else
        {
            color = RGB_ERROR;
        }
        SetTextColor(hdc, color);
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    default:
        break;
    }
    return 0;
}


INT_PTR SfidDialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    int len = GetTextLength(id);
    switch (id)
    {
    case IDC_SFID_EDIT:
        SetTextColor(hdc,
            len == SFID_LENGTH ? (m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT)) :
            len == SFID_LENGTH_EXCLUDING_CHECKSUM ? RGB_GOOD :
            len < SFID_LENGTH_EXCLUDING_CHECKSUM ? RGB_ERROR :
            len < SFID_LENGTH ? RGB_TOO_MANY :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    case IDC_SFID_KEYPREFIX_EDIT:
        SetTextColor(hdc,
            len == SFID_KEYPREFIX_LENGTH ? (m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT)) :
            len < SFID_KEYPREFIX_LENGTH ? RGB_ERROR :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    case IDC_SFID_INSTANCE_EDIT:
        SetTextColor(hdc,
            len == SFID_INSTANCE_LENGTH ? (m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT)) :
            len < SFID_INSTANCE_LENGTH ? RGB_ERROR :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    case IDC_SFID_UNIQUEID_EDIT:
        SetTextColor(hdc,
            len == SFID_UNIQUEID_LENGTH ? (m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT)) :
            len < SFID_UNIQUEID_LENGTH ? RGB_ERROR :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    default:
        break;
    }
    return 0;
}


void SfidDialogBox::OnCopyAll()
{
    if (m_LastModified)
    {
        ApplyModification(m_LastModified.By);
    }
    if (!Clipboard::Write(hwnd, GetText(IDC_SFID_EDIT)))
    {
        MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
}


void SfidDialogBox::OnClear()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    SetText(IDC_SFID_EDIT);
    ApplyModification(IDC_SFID_EDIT);
}


void SfidDialogBox::OnExecute()
{
    ChangeContent(rand() + rand() * 32767LL);
}


void SfidDialogBox::OnEditChanged(int id)
{
    FilterText(id, &SFID::IsValid);
    MyDialogBox::OnEditChanged(id);
    if (m_State != STATE_CHANGING)
    {
        m_State = STATE_CHANGING;
        switch (id)
        {
        case IDC_SFID_EDIT:
            InvalidateRect(IDC_SFID_EDIT, NULL, TRUE);
            break;
        case IDC_SFID_KEYPREFIX_EDIT:
        case IDC_SFID_INSTANCE_EDIT:
        case IDC_SFID_UNIQUEID_EDIT:
            InvalidateRect(IDC_SFID_KEYPREFIX_EDIT, NULL, TRUE);
            InvalidateRect(IDC_SFID_INSTANCE_EDIT, NULL, TRUE);
            InvalidateRect(IDC_SFID_UNIQUEID_EDIT, NULL, TRUE);
            break;
        default:
            break;
        }
        SetStatusText(ResourceString(IDS_EDITING));
        UpdateControlsState();
    }
    switch (id)
    {
    case IDC_SFID_EDIT:
        SetLengthText(IDC_SFID_LENGTH_STATIC, SFID_LENGTH, GetTextLength(IDC_SFID_EDIT));
        break;
    case IDC_SFID_KEYPREFIX_EDIT:
        SetLengthText(IDC_SFID_KLENGTH_STATIC, SFID_KEYPREFIX_LENGTH, GetTextLength(IDC_SFID_KEYPREFIX_EDIT));
        break;
    case IDC_SFID_INSTANCE_EDIT:
        SetLengthText(IDC_SFID_ILENGTH_STATIC, SFID_INSTANCE_LENGTH, GetTextLength(IDC_SFID_INSTANCE_EDIT));
        break;
    case IDC_SFID_UNIQUEID_EDIT:
        SetLengthText(IDC_SFID_ULENGTH_STATIC, SFID_UNIQUEID_LENGTH, GetTextLength(IDC_SFID_UNIQUEID_EDIT));
        break;
    default:
        break;
    }
}


void SfidDialogBox::UpdateControlsState()
{
    if (m_State == STATE_SUCCESSFUL)
    {
        EditSetReadOnly(IDC_SFID_EDIT, FALSE);
        EditSetReadOnly(IDC_SFID_KEYPREFIX_EDIT, FALSE);
        EditSetReadOnly(IDC_SFID_INSTANCE_EDIT, FALSE);
        EditSetReadOnly(IDC_SFID_UNIQUEID_EDIT, FALSE);
    }
    else
    {
        switch (m_LastModified.By)
        {
        case IDC_SFID_EDIT:
            EditSetReadOnly(IDC_SFID_EDIT, FALSE);
            EditSetReadOnly(IDC_SFID_KEYPREFIX_EDIT, TRUE);
            EditSetReadOnly(IDC_SFID_INSTANCE_EDIT, TRUE);
            EditSetReadOnly(IDC_SFID_UNIQUEID_EDIT, TRUE);
            break;
        case IDC_SFID_KEYPREFIX_EDIT:
        case IDC_SFID_INSTANCE_EDIT:
        case IDC_SFID_UNIQUEID_EDIT:
            EditSetReadOnly(IDC_SFID_EDIT, TRUE);
            EditSetReadOnly(IDC_SFID_KEYPREFIX_EDIT, FALSE);
            EditSetReadOnly(IDC_SFID_INSTANCE_EDIT, FALSE);
            EditSetReadOnly(IDC_SFID_UNIQUEID_EDIT, FALSE);
            break;
        default:
            break;
        }
    }
    BOOL bValid = (m_State == STATE_SUCCESSFUL && GetTextLength(IDC_SFID_EDIT) == SFID_LENGTH) ? TRUE : FALSE;
    EnableWindow(IDC_SFID_COPY_BUTTON, bValid);
    EnableWindow(IDC_SFID_UP_BUTTON, bValid);
    EnableWindow(IDC_SFID_DN_BUTTON, bValid);
    EnableWindow(IDC_SFID_RN_BUTTON, bValid);
    m_menuEdit
        .Enable(IDM_EDIT_COPYALL, bValid ? MF_ENABLED : MF_DISABLED)
        .Enable(IDM_EDIT_EXECUTE, bValid ? MF_ENABLED : MF_DISABLED);
}


void SfidDialogBox::ChangeContent(LONGLONG delta)
{
    String szUniqueId = GetText(IDC_SFID_UNIQUEID_EDIT);
    if (szUniqueId.Len > SFID_UNIQUEID_LENGTH)
    {
        SetStatusTextOnError(L"%s: Too long", SFID_UNIQUEID_NAME);
        return;
    }
    LONGLONG value = SFID::ComputeSerialNumber(szUniqueId);
    if (value < 0)
    {
        SetStatusTextOnError(L"%s: Invalid", SFID_UNIQUEID_NAME);
        return;
    }
    SFID id;
    id.UniqueIdValue = value + delta;
    SetText(IDC_SFID_UNIQUEID_EDIT, id.UniqueId);
    ApplyModification(IDC_SFID_UNIQUEID_EDIT);
}


void SfidDialogBox::ApplyModification(int id)
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    SetStatusText(L"");
    switch (id)
    {
    case IDC_SFID_EDIT:
        try
        {
            String sz = GetText(IDC_SFID_EDIT);
            if (sz.Len == 0)
            {
                SetText(IDC_SFID_KEYPREFIX_EDIT);
                SetText(IDC_SFID_INSTANCE_EDIT);
                SetText(IDC_SFID_UNIQUEID_EDIT);
                SetText(IDC_SFID_CHECKSUM_EDIT);
                break;
            }
            SFID id = SFID::Parse(sz);
            SetText(IDC_SFID_KEYPREFIX_EDIT, id.KeyPrefix);
            EditSetSelection(IDC_SFID_KEYPREFIX_EDIT, SFID_KEYPREFIX_LENGTH);
            SetText(IDC_SFID_INSTANCE_EDIT, id.Instance);
            EditSetSelection(IDC_SFID_INSTANCE_EDIT, SFID_INSTANCE_LENGTH);
            SetText(IDC_SFID_UNIQUEID_EDIT, id.UniqueId);
            EditSetSelection(IDC_SFID_UNIQUEID_EDIT, SFID_UNIQUEID_LENGTH);
            SetText(IDC_SFID_CHECKSUM_EDIT, id.Checksum);
            EditSetSelection(IDC_SFID_CHECKSUM_EDIT, SFID_CHECKSUM_LENGTH);
            if (StrCmp(id, -1, sz, sz.Len))
            {
                SetText(IDC_SFID_EDIT, id);
                EditSetSelection(IDC_SFID_EDIT, SFID_LENGTH);
            }
            if (sz.Len == SFID_LENGTH_EXCLUDING_CHECKSUM)
            {
                SetStatusText(ResourceString(IDS_OK_CHECKSUM_COMPUTED));
            }
            else
            {
                SetStatusText(ResourceString(IDS_OK));
            }
        }
        catch (SFIDException e)
        {
            SetStatusTextOnError(L"%s", e.Message);
        }
        break;
    case IDC_SFID_KEYPREFIX_EDIT:
    case IDC_SFID_INSTANCE_EDIT:
    case IDC_SFID_UNIQUEID_EDIT:
        try
        {
            String szKeyPrefix = GetText(IDC_SFID_KEYPREFIX_EDIT);
            String szInstance = GetText(IDC_SFID_INSTANCE_EDIT);
            String szUniqueId = GetText(IDC_SFID_UNIQUEID_EDIT);
            if (szKeyPrefix.Len == 0 && szInstance.Len == 0 && szUniqueId.Len == 0)
            {
                SetText(IDC_SFID_CHECKSUM_EDIT);
                SetText(IDC_SFID_EDIT);
                break;
            }
            SFID id = SFID::Parse(szKeyPrefix, szInstance, szUniqueId);
            SetText(IDC_SFID_CHECKSUM_EDIT, id.Checksum);
            EditSetSelection(IDC_SFID_CHECKSUM_EDIT, SFID_CHECKSUM_LENGTH);
            SetText(IDC_SFID_EDIT, id);
            EditSetSelection(IDC_SFID_EDIT, SFID_LENGTH);
            SetStatusText(ResourceString(IDS_OK));
        }
        catch (SFIDException e)
        {
            SetStatusTextOnError(L"%s", e.Message);
        }
        break;
    default:
        break;
    }
    m_LastModified.By = 0;
    SetLengthText(IDC_SFID_LENGTH_STATIC, SFID_LENGTH, GetTextLength(IDC_SFID_EDIT));
    SetLengthText(IDC_SFID_KLENGTH_STATIC, SFID_KEYPREFIX_LENGTH, GetTextLength(IDC_SFID_KEYPREFIX_EDIT));
    SetLengthText(IDC_SFID_ILENGTH_STATIC, SFID_INSTANCE_LENGTH, GetTextLength(IDC_SFID_INSTANCE_EDIT));
    SetLengthText(IDC_SFID_ULENGTH_STATIC, SFID_UNIQUEID_LENGTH, GetTextLength(IDC_SFID_UNIQUEID_EDIT));
    InvalidateRect(IDC_SFID_EDIT, NULL, TRUE);
    InvalidateRect(IDC_SFID_KEYPREFIX_EDIT, NULL, TRUE);
    InvalidateRect(IDC_SFID_INSTANCE_EDIT, NULL, TRUE);
    InvalidateRect(IDC_SFID_UNIQUEID_EDIT, NULL, TRUE);
    UpdateEditControlMenus(m_CurrentEdit);
    UpdateControlsState();
}


void SfidDialogBox::SetStatusText(PCWSTR pszFormat, ...)
{
    if (m_cProcessing)
    {
        m_State = STATE_SUCCESSFUL;
    }
    va_list argList;
    va_start(argList, pszFormat);
    SetText(IDC_SFID_STATUS_STATIC, String(pszFormat, argList));
    va_end(argList);
}


void SfidDialogBox::SetStatusTextOnError(PCWSTR pszFormat, ...)
{
    m_State = STATE_ERROR;
    va_list argList;
    va_start(argList, pszFormat);
    SetText(IDC_SFID_STATUS_STATIC, String(pszFormat, argList));
    va_end(argList);
}
