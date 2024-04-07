#include "pch.h"
#include "CodeDialogBox.h"
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


#define REGVAL_GTIN13 L"GTIN13"
#define REGVAL_GTIN13_GS1CPLEN L"GS1CompanyPrefixLength"


#define CODE_TIMER100MS 9001


using namespace hnrt;


CodeDialogBox::CodeDialogBox()
	: MyDialogBox(IDD_CODE, L"CODE")
    , m_GS1CPLength(GS1COMPANYPREFIX_LENGTH7)
    , m_nGTIN13Change(0)
{
	m_LastModified.CursorChange = true;
}


void CodeDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    HFONT hFont = GetApp<MyToolbox>().GetFontForData();
    SetFont(IDC_CODE_GTIN13_EDIT, hFont);
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
        SetText(IDC_CODE_GTIN13_EDIT, RegistryValue::GetSZ(hKey, REGVAL_GTIN13));
    }
    m_menuView
        .Add(ResourceString(IDS_MENU_CODE), IDM_VIEW_CODE);
}


void CodeDialogBox::OnDestroy()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_WRITE);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetDWORD(hKey, REGVAL_GTIN13_GS1CPLEN, m_GS1CPLength);
        RegistryValue::SetSZ(hKey, REGVAL_GTIN13, GetText(IDC_CODE_GTIN13_EDIT));
    }
    MyDialogBox::OnDestroy();
}


void CodeDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_CODE_GTIN13_STATIC, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_CODE_GTIN13_EDIT, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_CODE_GTIN13_COPY_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_CODE_GTIN13_UP_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_CODE_GTIN13_DN_BUTTON, cxDelta, 0, 0, 0);
}


void CodeDialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
    KillTimer(hwnd, CODE_TIMER100MS);
    if (m_LastModified)
    {
        ApplyModification();
    }
    m_menuView
        .Enable(IDM_VIEW_CODE, MF_ENABLED);
}


void CodeDialogBox::OnTabSelectionChanged()
{
    MyDialogBox::OnTabSelectionChanged();
    AddEditControlMenus(m_CurrentEdit);
    m_menuView
        .Enable(IDM_VIEW_CODE, MF_DISABLED);
    SetTimer(hwnd, CODE_TIMER100MS, 100, NULL);
}


INT_PTR CodeDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
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
    case IDC_CODE_GTIN13_EDIT:
        if (idNotif == EN_CHANGE)
        {
            OnEditChanged(idChild);
            if (m_nGTIN13Change < 0)
            {
                m_nGTIN13Change = 1;
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
    case IDC_CODE_GTIN13_COPY_BUTTON:
        if (m_LastModified)
        {
            ApplyModification();
        }
        if (!Clipboard::Write(hwnd, GetText(IDC_CODE_GTIN13_EDIT)))
        {
            MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
        break;
    case IDC_CODE_GTIN13_UP_BUTTON:
        GTIN13Add(1);
        break;
    case IDC_CODE_GTIN13_DN_BUTTON:
        GTIN13Add(-1);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


INT_PTR CodeDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case CODE_TIMER100MS:
        if (m_LastModified.IsUpdateRequired)
        {
            ApplyModification();
        }
        break;
    default:
        break;
    }
    return 0;
}


INT_PTR CodeDialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    SIZE_T len = GetText(id).Len;
    switch (id)
    {
    case IDC_CODE_GTIN13_EDIT:
        SetTextColor(hdc,
            m_nGTIN13Change == 0 && len == GTIN13_LENGTH ? GetSysColor(COLOR_WINDOWTEXT) :
            len == GTIN13_LENGTH_EXCLUDING_CD || (m_nGTIN13Change > 0 && len == GTIN13_LENGTH) ? RGB_GOOD :
            RGB_ERROR);
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    default:
        break;
    }
    return 0;
}


void CodeDialogBox::ApplyModification()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (m_nGTIN13Change)
    {
        try
        {
            String sz = GetText(IDC_CODE_GTIN13_EDIT);
            if (sz.Len == 12 || sz.Len == 13)
            {
                int start = 0, end = 0;
                EditGetSelection(IDC_CODE_GTIN13_EDIT, start, end);
                GTIN13 jan = GTIN13::Parse(sz, m_GS1CPLength);
                SetText(IDC_CODE_GTIN13_EDIT, jan);
                if (start == sz.Len)
                {
                    EditSetSelection(IDC_CODE_GTIN13_EDIT, GTIN13_LENGTH, GTIN13_LENGTH);
                }
                else
                {
                    EditSetSelection(IDC_CODE_GTIN13_EDIT, start, end);
                }
                m_nGTIN13Change = 0;
            }
        }
        catch (GTIN13Exception e)
        {
            m_nGTIN13Change = -1;
            InvalidateRect(GetDlgItem(hwnd, IDC_CODE_GTIN13_EDIT), NULL, TRUE);
        }
    }
    UpdateEditControlMenus(m_CurrentEdit);
    m_LastModified.By = 0;
}


void CodeDialogBox::GTIN13Add(int delta)
{
    try
    {
        String sz = GetText(IDC_CODE_GTIN13_EDIT);
        if (sz.Len == 12 || sz.Len == 13)
        {
            WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
            GTIN13 jan = GTIN13::Parse(sz, m_GS1CPLength);
            jan += delta;
            SetText(IDC_CODE_GTIN13_EDIT, jan);
            m_nGTIN13Change = 0;
        }
    }
    catch (GTIN13Exception e)
    {
        m_nGTIN13Change = -1;
        InvalidateRect(GetDlgItem(hwnd, IDC_CODE_GTIN13_EDIT), NULL, TRUE);
    }
}
