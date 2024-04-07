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
#include "hnrt/Debug.h"


#define REGVAL_GTIN13 L"GTIN13"
#define REGVAL_GTIN13_GS1CPLEN L"GS1CompanyPrefixLength"


#define CODE_TIMER100MS 9001
#define CODE_TIMER1000MS 9002


using namespace hnrt;


GtinDialogBox::GtinDialogBox()
	: MyDialogBox(IDD_GTIN, L"GTIN")
    , m_GS1CPLength(GS1COMPANYPREFIX_LENGTH7)
    , m_nGTIN13Change(0)
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
    KillTimer(hwnd, CODE_TIMER100MS);
    KillTimer(hwnd, CODE_TIMER1000MS);
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
    AddEditControlMenus(m_CurrentEdit);
    m_menuView
        .Enable(IDM_VIEW_GTIN, MF_DISABLED);
    SetTimer(hwnd, CODE_TIMER100MS, 100, NULL);
    SetTimer(hwnd, CODE_TIMER1000MS, 1000, NULL);
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
    case IDC_GTIN_EDIT:
    case IDC_GTIN_CP_EDIT:
    case IDC_GTIN_IR_EDIT:
    case IDC_GTIN_CD_EDIT:
        if (idNotif == EN_CHANGE)
        {
            OnEditChanged(idChild);
            if (m_nGTIN13Change <= 0)
            {
                m_nGTIN13Change = 1;
                switch (idChild)
                {
                case IDC_GTIN_EDIT:
                    EditSetReadOnly(IDC_GTIN_CP_EDIT);
                    EditSetReadOnly(IDC_GTIN_IR_EDIT);
                    break;
                case IDC_GTIN_CP_EDIT:
                case IDC_GTIN_IR_EDIT:
                default:
                    EditSetReadOnly(IDC_GTIN_EDIT);
                    SetText(IDC_GTIN_CD_EDIT);
                    break;
                }
                EnableWindow(IDC_GTIN_COPY_BUTTON, FALSE);
                EnableWindow(IDC_GTIN_UP_BUTTON, FALSE);
                EnableWindow(IDC_GTIN_DN_BUTTON, FALSE);
            }
            else
            {
                m_nGTIN13Change++;
            }
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
    case IDC_GTIN_CPL_COMBO:
        if (idNotif == CBN_SELCHANGE)
        {
            SetGS1CPLength(ComboBoxGetSelection(IDC_GTIN_CPL_COMBO, GS1COMPANYPREFIX_LENGTH7));
        }
        break;
    case IDC_GTIN_COPY_BUTTON:
        if (m_LastModified)
        {
            ApplyModification(m_LastModified.By);
        }
        if (!Clipboard::Write(hwnd, GetText(IDC_GTIN_EDIT)))
        {
            MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
        break;
    case IDC_GTIN_UP_BUTTON:
        GTIN13Add(1);
        break;
    case IDC_GTIN_DN_BUTTON:
        GTIN13Add(-1);
        break;
    case IDC_GTIN_RN_BUTTON:
        GTIN13Add(rand());
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


INT_PTR GtinDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case CODE_TIMER100MS:
        if (m_LastModified.IsUpdateRequired)
        {
            ApplyModification(m_LastModified.By);
        }
        break;
    case CODE_TIMER1000MS:
        UpdateEditControlMenus(m_CurrentEdit);
        break;
    default:
        break;
    }
    return 0;
}


INT_PTR GtinDialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    SIZE_T len = GetText(id).Len;
    switch (id)
    {
    case IDC_GTIN_EDIT:
        SetTextColor(hdc,
            m_nGTIN13Change == 0 && len == GTIN13_LENGTH ? GetSysColor(COLOR_WINDOWTEXT) :
            len == GTIN13_LENGTH_EXCLUDING_CD || (m_nGTIN13Change > 0 && len == GTIN13_LENGTH) ? RGB_GOOD :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    case IDC_GTIN_CP_EDIT:
        SetTextColor(hdc,
            m_nGTIN13Change == 0 && len == m_GS1CPLength ? GetSysColor(COLOR_WINDOWTEXT) :
            m_nGTIN13Change > 0 && len == m_GS1CPLength ? RGB_GOOD :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    case IDC_GTIN_IR_EDIT:
    {
        int expected = GTIN13(m_GS1CPLength).ItemReferenceLenth;
        SetTextColor(hdc,
            m_nGTIN13Change == 0 && len == expected ? GetSysColor(COLOR_WINDOWTEXT) :
            m_nGTIN13Change > 0 && len == expected ? RGB_GOOD :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    }
    default:
        break;
    }
    return 0;
}


void GtinDialogBox::SetGS1CPLength(int length)
{
    m_GS1CPLength = length;
    GTIN13 jan(length);
    SetText(IDC_GTIN_CP_STATIC, String(PRINTF, L"%d chars", jan.GS1CompanyPrefixLength));
    SetText(IDC_GTIN_IR_STATIC, String(PRINTF, L"%d chars", jan.ItemReferenceLenth));
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
    if (id == IDC_GTIN_EDIT)
    {
        try
        {
            String sz = GetText(IDC_GTIN_EDIT);
            if (sz.Len >= m_GS1CPLength)
            {
                GTIN13 jan(m_GS1CPLength);
                String szCP(&sz[0], jan.GS1CompanyPrefixLength);
                String szIR(&sz[jan.GS1CompanyPrefixLength], jan.ItemReferenceLenth);
                SetText(IDC_GTIN_CP_EDIT, szCP);
                EditSetSelection(IDC_GTIN_CP_EDIT, static_cast<int>(szCP.Len), static_cast<int>(szCP.Len));
                SetText(IDC_GTIN_IR_EDIT, szIR);
                EditSetSelection(IDC_GTIN_IR_EDIT, static_cast<int>(szIR.Len), static_cast<int>(szIR.Len));
                SetText(IDC_GTIN_CD_EDIT);
            }
            else
            {
                SetText(IDC_GTIN_CP_EDIT);
                SetText(IDC_GTIN_IR_EDIT);
                SetText(IDC_GTIN_CD_EDIT);
            }
            if (sz.Len == GTIN13_LENGTH_EXCLUDING_CD || sz.Len == GTIN13_LENGTH)
            {
                int start = 0, end = 0;
                EditGetSelection(IDC_GTIN_EDIT, start, end);
                GTIN13 jan = GTIN13::Parse(sz, m_GS1CPLength);
                m_nGTIN13Change = 0;
                SetText(IDC_GTIN_EDIT, jan);
                if (start == sz.Len)
                {
                    EditSetSelection(IDC_GTIN_EDIT, GTIN13_LENGTH, GTIN13_LENGTH);
                }
                else
                {
                    EditSetSelection(IDC_GTIN_EDIT, start, end);
                }
                SetText(IDC_GTIN_CD_EDIT, String(PRINTF, L"%c", jan.CheckDigit));
                EditSetReadOnly(IDC_GTIN_CP_EDIT, FALSE);
                EditSetReadOnly(IDC_GTIN_IR_EDIT, FALSE);
                EnableWindow(IDC_GTIN_COPY_BUTTON);
                EnableWindow(IDC_GTIN_UP_BUTTON);
                EnableWindow(IDC_GTIN_DN_BUTTON);
            }
        }
        catch (GTIN13Exception e)
        {
            m_nGTIN13Change = -1;
        }
    }
    else if (id == IDC_GTIN_CP_EDIT || id == IDC_GTIN_IR_EDIT)
    {
        try
        {
            String szCP = GetText(IDC_GTIN_CP_EDIT);
            String szIR = GetText(IDC_GTIN_IR_EDIT);
            SetText(IDC_GTIN_CD_EDIT);
            if (szCP.Len == m_GS1CPLength && szIR.Len == GTIN13(m_GS1CPLength).ItemReferenceLenth)
            {
                GTIN13 jan = GTIN13::Parse(String(szCP, szIR), m_GS1CPLength);
                m_nGTIN13Change = 0;
                SetText(IDC_GTIN_CD_EDIT, String(PRINTF, L"%c", jan.CheckDigit));
                SetText(IDC_GTIN_EDIT, jan);
                EditSetSelection(IDC_GTIN_EDIT, GTIN13_LENGTH, GTIN13_LENGTH);
                EditSetReadOnly(IDC_GTIN_EDIT, FALSE);
                EnableWindow(IDC_GTIN_COPY_BUTTON);
                EnableWindow(IDC_GTIN_UP_BUTTON);
                EnableWindow(IDC_GTIN_DN_BUTTON);
            }
            else
            {
                String sz(szCP, szIR);
                SetText(IDC_GTIN_EDIT, sz);
                EditSetSelection(IDC_GTIN_EDIT, static_cast<int>(sz.Len), static_cast<int>(sz.Len));
                SetText(IDC_GTIN_CD_EDIT);
            }
        }
        catch (GTIN13Exception e)
        {
            m_nGTIN13Change = -1;
        }
    }
    InvalidateRect(GetDlgItem(hwnd, IDC_GTIN_EDIT), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_GTIN_CP_EDIT), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_GTIN_IR_EDIT), NULL, TRUE);
    UpdateEditControlMenus(m_CurrentEdit);
    m_LastModified.By = 0;
}
