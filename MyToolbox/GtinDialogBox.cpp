#include "pch.h"
#include "GtinDialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Clipboard.h"
#include "hnrt/Time.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/StringCommons.h"
#include "hnrt/GTIN13.h"
#include "hnrt/GTIN13Exception.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Debug.h"


#define REGVAL_GTIN13 L"GTIN13"
#define REGVAL_GTIN13_GS1CPLEN L"GS1CompanyPrefixLength"


#define STATE_SUCCESSFUL 1
#define STATE_ERROR 2
#define STATE_CHANGING 3


using namespace hnrt;


GtinDialogBox::GtinDialogBox()
	: MyDialogBox(IDD_GTIN, L"GTIN")
    , m_GS1CPLength(GS1COMPANYPREFIX_LENGTH7)
    , m_State(STATE_SUCCESSFUL)
{
	m_LastModified.CursorChange = true;
}


void GtinDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    HFONT hFont = GetApp<MyToolbox>().GetFontForData();
    SetFont(IDC_GTIN_EDIT, hFont);
    SetFont(IDC_GTIN_CP_EDIT, hFont);
    SetFont(IDC_GTIN_IR_EDIT, hFont);
    SetFont(IDC_GTIN_CD_EDIT, hFont);
    SetText(IDC_GTIN_STATUS_STATIC);
    ComboBoxAdd(IDC_GTIN_CPL_COMBO, String(PRINTF, L"%d", GS1COMPANYPREFIX_LENGTH7), GS1COMPANYPREFIX_LENGTH7);
    ComboBoxAdd(IDC_GTIN_CPL_COMBO, String(PRINTF, L"%d", GS1COMPANYPREFIX_LENGTH9), GS1COMPANYPREFIX_LENGTH9);
    ComboBoxAdd(IDC_GTIN_CPL_COMBO, String(PRINTF, L"%d", GS1COMPANYPREFIX_LENGTH10), GS1COMPANYPREFIX_LENGTH10);
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
        m_GS1CPLength = RegistryValue::GetDWORD(hKey, REGVAL_GTIN13_GS1CPLEN, m_GS1CPLength);
        switch (m_GS1CPLength)
        {
        case GS1COMPANYPREFIX_LENGTH7:
        case GS1COMPANYPREFIX_LENGTH9:
        case GS1COMPANYPREFIX_LENGTH10:
            break;
        default:
            m_GS1CPLength = GS1COMPANYPREFIX_LENGTH7;
            break;
        }
        SetText(IDC_GTIN_EDIT, RegistryValue::GetSZ(hKey, REGVAL_GTIN13));
    }
    ComboBoxSetSelection(IDC_GTIN_CPL_COMBO, m_GS1CPLength);
    SetText(IDC_GTIN_CP_STATIC, String(PRINTF, L"%d chars", m_GS1CPLength));
    SetText(IDC_GTIN_IR_STATIC, String(PRINTF, L"%d chars", GTIN13(m_GS1CPLength).ItemReferenceLenth));
    ApplyModification(IDC_GTIN_EDIT);
    m_menuView
        .Add(ResourceString(IDS_MENU_GTIN), IDM_VIEW_GTIN);
}


void GtinDialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_WRITE);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetDWORD(hKey, REGVAL_GTIN13_GS1CPLEN, m_GS1CPLength);
        RegistryValue::SetSZ(hKey, REGVAL_GTIN13, GetText(IDC_GTIN_EDIT));
    }
    MyDialogBox::OnDestroy();
}


void GtinDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_GTIN_STATIC, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GTIN_EDIT, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GTIN_STATUS_STATIC, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GTIN_CPL_STATIC, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GTIN_CPL_COMBO, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GTIN_COPY_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GTIN_UP_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GTIN_DN_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GTIN_RN_BUTTON, cxDelta, 0, 0, 0);
}


void GtinDialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
    if (m_LastModified)
    {
        ApplyModification(m_LastModified.By);
    }
    m_menuView
        .Enable(IDM_VIEW_GTIN, MF_ENABLED);
}


void GtinDialogBox::OnTabSelectionChanged()
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
        .Enable(IDM_VIEW_GTIN, MF_DISABLED);
    UpdateControlsState();
    SetTimer(100);
    srand(static_cast<unsigned int>(FileTime().Intervals));
}


INT_PTR GtinDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
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
    case IDC_GTIN_COPY_BUTTON:
        OnCopyAll();
        break;
    case IDC_GTIN_UP_BUTTON:
        GTIN13Add(1);
        break;
    case IDC_GTIN_DN_BUTTON:
        GTIN13Add(-1);
        break;
    case IDC_GTIN_RN_BUTTON:
        OnExecute();
        break;
    case IDC_GTIN_CPL_COMBO:
        if (idNotif == CBN_SELCHANGE)
        {
            SetGS1CPLength(ComboBoxGetSelection(IDC_GTIN_CPL_COMBO, GS1COMPANYPREFIX_LENGTH7));
        }
        break;
    case IDC_GTIN_EDIT:
    case IDC_GTIN_CP_EDIT:
    case IDC_GTIN_IR_EDIT:
    case IDC_GTIN_CD_EDIT:
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


INT_PTR GtinDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
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


INT_PTR GtinDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    int length;
    COLORREF color;
    switch (id)
    {
    case IDC_GTIN_STATUS_STATIC:
        SetTextColor(hdc,
            m_State == STATE_SUCCESSFUL ? RGB_SUCCESSFUL :
            m_State == STATE_ERROR ? RGB_ERROR :
            m_State == STATE_CHANGING ? RGB_CHANGING :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    case IDC_GTIN_LENGTH_STATIC:
        length = GetTextLength(IDC_GTIN_EDIT);
        if (length == 0)
        {
            color = m_State == STATE_CHANGING ? RGB_ERROR : GetSysColor(COLOR_WINDOWTEXT);
        }
        else if (length == GTIN13_LENGTH_EXCLUDING_CD || length == GTIN13_LENGTH)
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
    case IDC_GTIN_CP_STATIC:
        length = GetTextLength(IDC_GTIN_CP_EDIT);
        if (length == 0 || length == m_GS1CPLength)
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
    case IDC_GTIN_IR_STATIC:
        length = GetTextLength(IDC_GTIN_IR_EDIT);
        if (length == 0 || length == GTIN13(m_GS1CPLength).ItemReferenceLenth)
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


INT_PTR GtinDialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    int len = GetTextLength(id);
    switch (id)
    {
    case IDC_GTIN_EDIT:
        SetTextColor(hdc,
            len == GTIN13_LENGTH ? (m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT)) :
            len == GTIN13_LENGTH_EXCLUDING_CD ? RGB_GOOD :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    case IDC_GTIN_CP_EDIT:
        SetTextColor(hdc,
            len == m_GS1CPLength ? (m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT)) :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    case IDC_GTIN_IR_EDIT:
        SetTextColor(hdc,
            len == GTIN13(m_GS1CPLength).ItemReferenceLenth ? (m_State == STATE_CHANGING ? RGB_GOOD : GetSysColor(COLOR_WINDOWTEXT)) :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    default:
        break;
    }
    return 0;
}


void GtinDialogBox::OnCopyAll()
{
    if (m_LastModified)
    {
        ApplyModification(m_LastModified.By);
    }
    if (!Clipboard::Write(hwnd, GetText(IDC_GTIN_EDIT)))
    {
        MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
}


void GtinDialogBox::OnClear()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    SetText(IDC_GTIN_EDIT);
    ApplyModification(IDC_GTIN_EDIT);
}


void GtinDialogBox::OnExecute()
{
    GTIN13Add(rand());
}


void GtinDialogBox::OnEditChanged(int id)
{
    FilterText(id, &GTIN13::IsValid);
    MyDialogBox::OnEditChanged(id);
    if (m_State != STATE_CHANGING)
    {
        m_State = STATE_CHANGING;
        switch (id)
        {
        case IDC_GTIN_EDIT:
            InvalidateRect(IDC_GTIN_EDIT, NULL, TRUE);
            break;
        case IDC_GTIN_CP_EDIT:
        case IDC_GTIN_IR_EDIT:
            InvalidateRect(IDC_GTIN_CP_EDIT, NULL, TRUE);
            InvalidateRect(IDC_GTIN_IR_EDIT, NULL, TRUE);
            break;
        default:
            break;
        }
        SetStatusText(ResourceString(IDS_EDITING));
        UpdateControlsState();
    }
    switch (id)
    {
    case IDC_GTIN_EDIT:
        SetLengthText(IDC_GTIN_LENGTH_STATIC, GTIN13_LENGTH, GetTextLength(IDC_GTIN_EDIT));
        break;
    case IDC_GTIN_CP_EDIT:
        SetLengthText(IDC_GTIN_CP_STATIC, m_GS1CPLength, GetTextLength(IDC_GTIN_CP_EDIT));
        break;
    case IDC_GTIN_IR_EDIT:
        SetLengthText(IDC_GTIN_IR_STATIC, GTIN13(m_GS1CPLength).ItemReferenceLenth, GetTextLength(IDC_GTIN_IR_EDIT));
        break;
    default:
        break;
    }
}


void GtinDialogBox::UpdateControlsState()
{
    if (m_State == STATE_SUCCESSFUL)
    {
        EditSetReadOnly(IDC_GTIN_EDIT, FALSE);
        EditSetReadOnly(IDC_GTIN_CP_EDIT, FALSE);
        EditSetReadOnly(IDC_GTIN_IR_EDIT, FALSE);
    }
    else
    {
        switch (m_LastModified.By)
        {
        case IDC_GTIN_EDIT:
            EditSetReadOnly(IDC_GTIN_EDIT, FALSE);
            EditSetReadOnly(IDC_GTIN_CP_EDIT, TRUE);
            EditSetReadOnly(IDC_GTIN_IR_EDIT, TRUE);
            break;
        case IDC_GTIN_CP_EDIT:
        case IDC_GTIN_IR_EDIT:
            EditSetReadOnly(IDC_GTIN_EDIT, TRUE);
            EditSetReadOnly(IDC_GTIN_CP_EDIT, FALSE);
            EditSetReadOnly(IDC_GTIN_IR_EDIT, FALSE);
            break;
        default:
            break;
        }
    }
    BOOL bValid = (m_State == STATE_SUCCESSFUL && GetTextLength(IDC_GTIN_EDIT) == GTIN13_LENGTH) ? TRUE : FALSE;
    EnableWindow(IDC_GTIN_COPY_BUTTON, bValid);
    EnableWindow(IDC_GTIN_UP_BUTTON, bValid);
    EnableWindow(IDC_GTIN_DN_BUTTON, bValid);
    EnableWindow(IDC_GTIN_RN_BUTTON, bValid);
    m_menuEdit
        .Enable(IDM_EDIT_COPYALL, bValid ? MF_ENABLED : MF_DISABLED)
        .Enable(IDM_EDIT_EXECUTE, bValid ? MF_ENABLED : MF_DISABLED);
}


void GtinDialogBox::SetGS1CPLength(int length)
{
    m_GS1CPLength = length;
    SetLengthText(IDC_GTIN_LENGTH_STATIC, GTIN13_LENGTH, GetTextLength(IDC_GTIN_EDIT));
    SetLengthText(IDC_GTIN_CP_STATIC, m_GS1CPLength, GetTextLength(IDC_GTIN_CP_EDIT));
    SetLengthText(IDC_GTIN_IR_STATIC, GTIN13(m_GS1CPLength).ItemReferenceLenth, GetTextLength(IDC_GTIN_IR_EDIT));
    ApplyModification(IDC_GTIN_EDIT);
}


void GtinDialogBox::GTIN13Add(int delta)
{
    try
    {
        String sz = GetText(IDC_GTIN_EDIT);
        if (sz.Len == GTIN13_LENGTH_EXCLUDING_CD || sz.Len == GTIN13_LENGTH)
        {
            WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
            GTIN13 jan = GTIN13::Parse(sz, m_GS1CPLength);
            jan += delta;
            SetText(IDC_GTIN_EDIT, jan);
            ApplyModification(IDC_GTIN_EDIT);
        }
    }
    catch (GTIN13Exception e)
    {
    }
}


void GtinDialogBox::ApplyModification(int id)
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    SetStatusText(L"");
    switch (id)
    {
    case IDC_GTIN_EDIT:
        try
        {
            String sz = GetText(IDC_GTIN_EDIT);
            if (sz.Len == 0)
            {
                SetText(IDC_GTIN_CP_EDIT);
                SetText(IDC_GTIN_IR_EDIT);
                SetText(IDC_GTIN_CD_EDIT);
                break;
            }
            GTIN13 jan = GTIN13::Parse(sz, m_GS1CPLength);
            SetText(IDC_GTIN_CP_EDIT, jan.GS1CompanyPrefix);
            EditSetSelection(IDC_GTIN_CP_EDIT, jan.GS1CompanyPrefixLength);
            SetText(IDC_GTIN_IR_EDIT, jan.ItemReference);
            EditSetSelection(IDC_GTIN_IR_EDIT, jan.ItemReferenceLenth);
            SetText(IDC_GTIN_CD_EDIT, String(PRINTF, L"%c", jan.CheckDigit));
            EditSetSelection(IDC_GTIN_CD_EDIT, 1);
            if (StrCmp(jan, -1, sz, sz.Len))
            {
                SetText(IDC_GTIN_EDIT, jan);
                EditSetSelection(IDC_GTIN_EDIT, GTIN13_LENGTH);
            }
            if (sz.Len == GTIN13_LENGTH_EXCLUDING_CD)
            {
                SetStatusText(ResourceString(IDS_OK_CHECKDIGIT_COMPUTED));
            }
            else
            {
                SetStatusText(ResourceString(IDS_OK));
            }
        }
        catch (GTIN13Exception e)
        {
            SetStatusTextOnError(L"%s", e.Message);
        }
        break;
    case IDC_GTIN_CP_EDIT:
    case IDC_GTIN_IR_EDIT:
        try
        {
            String szCP = GetText(IDC_GTIN_CP_EDIT);
            String szIR = GetText(IDC_GTIN_IR_EDIT);
            if (szCP.Len == 0 && szIR.Len == 0)
            {
                SetText(IDC_GTIN_CD_EDIT);
                SetText(IDC_GTIN_EDIT);
                break;
            }
            GTIN13 jan = GTIN13::Parse(String(szCP, szIR), m_GS1CPLength);
            SetText(IDC_GTIN_CD_EDIT, String(PRINTF, L"%c", jan.CheckDigit));
            EditSetSelection(IDC_GTIN_CD_EDIT, 1);
            SetText(IDC_GTIN_EDIT, jan);
            EditSetSelection(IDC_GTIN_EDIT, GTIN13_LENGTH);
            SetStatusText(ResourceString(IDS_OK));
        }
        catch (GTIN13Exception e)
        {
            SetStatusTextOnError(L"%s", e.Message);
        }
        break;
    default:
        break;
    }
    m_LastModified.By = 0;
    SetLengthText(IDC_GTIN_LENGTH_STATIC, GTIN13_LENGTH, GetTextLength(IDC_GTIN_EDIT));
    SetLengthText(IDC_GTIN_CP_STATIC, m_GS1CPLength, GetTextLength(IDC_GTIN_CP_EDIT));
    SetLengthText(IDC_GTIN_IR_STATIC, GTIN13(m_GS1CPLength).ItemReferenceLenth, GetTextLength(IDC_GTIN_IR_EDIT));
    InvalidateRect(IDC_GTIN_EDIT, NULL, TRUE);
    InvalidateRect(IDC_GTIN_CP_EDIT, NULL, TRUE);
    InvalidateRect(IDC_GTIN_IR_EDIT, NULL, TRUE);
    UpdateEditControlMenus(m_CurrentEdit);
    UpdateControlsState();
}


void GtinDialogBox::SetStatusText(PCWSTR pszFormat, ...)
{
    if (m_cProcessing)
    {
        m_State = STATE_SUCCESSFUL;
    }
    va_list argList;
    va_start(argList, pszFormat);
    SetText(IDC_GTIN_STATUS_STATIC, String(pszFormat, argList));
    va_end(argList);
}


void GtinDialogBox::SetStatusTextOnError(PCWSTR pszFormat, ...)
{
    m_State = STATE_ERROR;
    va_list argList;
    va_start(argList, pszFormat);
    SetText(IDC_GTIN_STATUS_STATIC, String(pszFormat, argList));
    va_end(argList);
}
