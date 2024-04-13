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


#define SFID_TIMER100MS 8001
#define SFID_TIMER1000MS 8002


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
        ApplyModification(IDC_SFID_EDIT);
    }
    m_menuView
        .Add(ResourceString(IDS_MENU_SFID), IDM_VIEW_SFID);
}


void SfidDialogBox::OnDestroy()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
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
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_NEW_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_UP_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_DOWN_BUTTON, cxDelta, 0, 0, 0);
}


void SfidDialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
    KillTimer(hwnd, SFID_TIMER100MS);
    KillTimer(hwnd, SFID_TIMER1000MS);
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
        .Add(ResourceString(IDS_MENU_NEW), IDM_EDIT_EXECUTE);
    m_menuView
        .Enable(IDM_VIEW_SFID, MF_DISABLED);
    SetTimer(hwnd, SFID_TIMER100MS, 100, NULL);
    SetTimer(hwnd, SFID_TIMER1000MS, 1000, NULL);
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
    case IDC_SFID_DOWN_BUTTON:
        ChangeContent(-1);
        break;
    case IDC_SFID_NEW_BUTTON:
        ChangeContent(rand() + rand() * 32767LL);
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
    switch (wParam)
    {
    case SFID_TIMER100MS:
        if (m_LastModified.IsUpdateRequired)
        {
            ApplyModification(m_LastModified.By);
        }
        break;
    case SFID_TIMER1000MS:
        UpdateEditControlMenus(m_CurrentEdit);
        break;
    default:
        break;
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


void SfidDialogBox::OnEditChanged(int id)
{
    FilterText(id);
    MyDialogBox::OnEditChanged(id);
    if (m_State != STATE_CHANGING)
    {
        m_State = STATE_CHANGING;
        switch (id)
        {
        case IDC_SFID_EDIT:
            InvalidateRect(GetDlgItem(hwnd, IDC_SFID_EDIT), NULL, TRUE);
            EditSetReadOnly(IDC_SFID_KEYPREFIX_EDIT);
            EditSetReadOnly(IDC_SFID_INSTANCE_EDIT);
            EditSetReadOnly(IDC_SFID_UNIQUEID_EDIT);
            break;
        default:
            InvalidateRect(GetDlgItem(hwnd, IDC_SFID_KEYPREFIX_EDIT), NULL, TRUE);
            InvalidateRect(GetDlgItem(hwnd, IDC_SFID_INSTANCE_EDIT), NULL, TRUE);
            InvalidateRect(GetDlgItem(hwnd, IDC_SFID_UNIQUEID_EDIT), NULL, TRUE);
            EditSetReadOnly(IDC_SFID_EDIT);
            break;
        }
        SetStatusText(L"Editing...");
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


void SfidDialogBox::OnExecute()
{
    ChangeContent(rand() + rand() * 32767LL);
}


void SfidDialogBox::FilterText(int id)
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    int start = 0;
    int end = 0;
    EditGetSelection(id, start, end);
    String sz = GetText(id);
    Buffer<WCHAR> buf(sz.Len + 1);
    const WCHAR* pQ = &sz[0];
    const WCHAR* pR = pQ;
    WCHAR* pW = &buf[0];
    while (*pR)
    {
        // iswalnum does not work as expected for Japanese characters.
        if (SFID::IsValid(*pR))
        {
            *pW++ = *pR++;
        }
        else
        {
            int cur = static_cast<int>(pR - pQ);
            if (cur <= start)
            {
                start--;
                end--;
            }
            else if (cur <= end)
            {
                end--;
            }
            pQ++;
            pR++;
        }
    }
    if (&sz[0] < pQ)
    {
        *pW = L'\0';
        SetText(id, &buf[0]);
        EditSetSelection(id, start, end);
    }
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
            String sz2 = sz.Trim();
            if (sz2.Len == 0)
            {
                SetText(IDC_SFID_KEYPREFIX_EDIT);
                SetText(IDC_SFID_INSTANCE_EDIT);
                SetText(IDC_SFID_UNIQUEID_EDIT);
                SetText(IDC_SFID_CHECKSUM_EDIT);
                SetText(IDC_SFID_EDIT);
                EditSetReadOnly(IDC_SFID_KEYPREFIX_EDIT, FALSE);
                EditSetReadOnly(IDC_SFID_INSTANCE_EDIT, FALSE);
                EditSetReadOnly(IDC_SFID_UNIQUEID_EDIT, FALSE);
                break;
            }
            SFID id = SFID::Parse(sz2);
            SetText(IDC_SFID_KEYPREFIX_EDIT, id.KeyPrefix);
            EditSetSelection(IDC_SFID_KEYPREFIX_EDIT, static_cast<int>(id.KeyPrefix.Len));
            SetText(IDC_SFID_INSTANCE_EDIT, id.Instance);
            EditSetSelection(IDC_SFID_INSTANCE_EDIT, static_cast<int>(id.Instance.Len));
            SetText(IDC_SFID_UNIQUEID_EDIT, id.UniqueId);
            EditSetSelection(IDC_SFID_UNIQUEID_EDIT, static_cast<int>(id.UniqueId.Len));
            SetText(IDC_SFID_CHECKSUM_EDIT, id.Checksum);
            EditSetSelection(IDC_SFID_CHECKSUM_EDIT, static_cast<int>(id.Checksum.Len));
            if (StrCmp(id, -1, sz, sz.Len))
            {
                SetText(IDC_SFID_EDIT, id);
                EditSetSelection(IDC_SFID_EDIT, static_cast<int>(StrLen(id)));
            }
            if (sz2.Len == SFID_LENGTH_EXCLUDING_CHECKSUM)
            {
                SetStatusText(L"OK [ Checksum computed ]");
            }
            else
            {
                SetStatusText(L"OK");
            }
            EditSetReadOnly(IDC_SFID_KEYPREFIX_EDIT, FALSE);
            EditSetReadOnly(IDC_SFID_INSTANCE_EDIT, FALSE);
            EditSetReadOnly(IDC_SFID_UNIQUEID_EDIT, FALSE);
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
            SFID id = SFID::Parse(szKeyPrefix.Trim(), szInstance.Trim(), szUniqueId.Trim());
            if (id.KeyPrefix != szKeyPrefix)
            {
                SetText(IDC_SFID_KEYPREFIX_EDIT, id.KeyPrefix);
                EditSetSelection(IDC_SFID_KEYPREFIX_EDIT, static_cast<int>(id.KeyPrefix.Len));
            }
            if (id.Instance != szInstance)
            {
                SetText(IDC_SFID_INSTANCE_EDIT, id.Instance);
                EditSetSelection(IDC_SFID_INSTANCE_EDIT, static_cast<int>(id.Instance.Len));
            }
            if (id.UniqueId != szUniqueId)
            {
                SetText(IDC_SFID_UNIQUEID_EDIT, id.UniqueId);
                EditSetSelection(IDC_SFID_UNIQUEID_EDIT, static_cast<int>(id.UniqueId.Len));
            }
            SetText(IDC_SFID_CHECKSUM_EDIT, id.Checksum);
            EditSetSelection(IDC_SFID_CHECKSUM_EDIT, static_cast<int>(id.Checksum.Len));
            SetText(IDC_SFID_EDIT, id);
            EditSetSelection(IDC_SFID_EDIT, static_cast<int>(StrLen(id)));
            SetStatusText(L"OK");
            EditSetReadOnly(IDC_SFID_EDIT, FALSE);
        }
        catch (SFIDException e)
        {
            SetStatusTextOnError(L"%s", e.Message);
        }
        break;
    default:
        break;
    }
    SetLengthText(IDC_SFID_LENGTH_STATIC, SFID_LENGTH, GetTextLength(IDC_SFID_EDIT));
    SetLengthText(IDC_SFID_KLENGTH_STATIC, SFID_KEYPREFIX_LENGTH, GetTextLength(IDC_SFID_KEYPREFIX_EDIT));
    SetLengthText(IDC_SFID_ILENGTH_STATIC, SFID_INSTANCE_LENGTH, GetTextLength(IDC_SFID_INSTANCE_EDIT));
    SetLengthText(IDC_SFID_ULENGTH_STATIC, SFID_UNIQUEID_LENGTH, GetTextLength(IDC_SFID_UNIQUEID_EDIT));
    InvalidateRect(GetDlgItem(hwnd, IDC_SFID_EDIT), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_SFID_KEYPREFIX_EDIT), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_SFID_INSTANCE_EDIT), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_SFID_UNIQUEID_EDIT), NULL, TRUE);
    UpdateEditControlMenus(m_CurrentEdit);
    m_LastModified.By = 0;
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


void SfidDialogBox::SetLengthText(int id, int expected, int actual)
{
    if (actual == 0 || actual == expected)
    {
        SetText(id, String(PRINTF, ResourceString(IDS_SFID_LENGTH_FORMAT), expected));
    }
    else if (actual < expected)
    {
        SetText(id, String(PRINTF, ResourceString(IDS_SFID_LENGTH2_FORMAT), expected, L'-', expected - actual));
    }
    else //if (actual > expected)
    {
        SetText(id, String(PRINTF, ResourceString(IDS_SFID_LENGTH2_FORMAT), expected, L'+', actual - expected));
    }
}
