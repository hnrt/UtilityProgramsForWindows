#include "pch.h"
#include "DateTimeDialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/Menu.h"
#include "hnrt/Time.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Clipboard.h"
#include "hnrt/ResourceString.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"


#define REGVAL_FORMAT L"Format"
#define REGVAL_OFFSET L"Offset"
#define REGVAL_LAST L"Last"


#define DTTM_TIMER1SEC 8001


using namespace hnrt;


DateTimeDialogBox::DateTimeDialogBox()
    : MyDialogBox(IDD_DTTM, L"DateTime")
    , m_offset(0)
    , m_format(IDC_DTTM_ISO8601_RADIO)
    , m_lastModifiedAt(0)
{
}


void DateTimeDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    InitializeOffsetComboBox(IDC_DTTM_OFFSET_COMBO);
    SYSTEMTIME st = { 0 };
    ::GetSystemTime(&st);
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
        FileTime(RegistryValue::GetQWORD(hKey, REGVAL_LAST, FileTime().Intervals)).ToSystemTime(st);
        m_offset = static_cast<int>(RegistryValue::GetDWORD(hKey, REGVAL_OFFSET));
        m_format = RegistryValue::GetDWORD(hKey, REGVAL_FORMAT, IDC_DTTM_ISO8601_RADIO - IDC_DTTM_EDIT) + IDC_DTTM_EDIT;
    }
    SetSystemTime(st);
    ComboBoxSetSelection(IDC_DTTM_OFFSET_COMBO, m_offset);
    ButtonCheck(m_format);
    FormatString(m_format);
    m_menuView
        .Add(ResourceString(IDS_MENU_DTTM), IDM_VIEW_DTTM);

}


void DateTimeDialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_WRITE);
    if (rc == ERROR_SUCCESS)
    {
        SYSTEMTIME st = { 0 };
        GetSystemTime(st);
        RegistryValue::SetQWORD(hKey, REGVAL_LAST, FileTime(st).Intervals);
        RegistryValue::SetDWORD(hKey, REGVAL_OFFSET, static_cast<DWORD>(m_offset));
        RegistryValue::SetDWORD(hKey, REGVAL_FORMAT, m_format - IDC_DTTM_EDIT);
    }
    MyDialogBox::OnDestroy();
}


void DateTimeDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_DTTM_COPY_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_DTTM_NEW_BUTTON, cxDelta, 0, 0, 0);
}


void DateTimeDialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
    KillTimer(hwnd, DTTM_TIMER1SEC);
    m_menuView
        .Enable(IDM_VIEW_DTTM, MF_ENABLED);
}


void DateTimeDialogBox::OnTabSelectionChanged()
{
    MyDialogBox::OnTabSelectionChanged();
    m_menuView
        .Enable(IDM_VIEW_DTTM, MF_DISABLED);
    SetTimer(hwnd, DTTM_TIMER1SEC, 1000, NULL);
}


INT_PTR DateTimeDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    UINT idChild = LOWORD(wParam);
    UINT idNotif = HIWORD(wParam);
    switch (idChild)
    {
    case IDC_DTTM_COPY_BUTTON:
        OnCopy();
        break;
    case IDC_DTTM_NEW_BUTTON:
        OnExecute();
        break;
    case IDC_DTTM_YEAR_EDIT:
    case IDC_DTTM_MONTH_EDIT:
    case IDC_DTTM_DAY_EDIT:
    case IDC_DTTM_HOUR_EDIT:
    case IDC_DTTM_MINUTE_EDIT:
    case IDC_DTTM_SECOND_EDIT:
    case IDC_DTTM_MILLISECOND_EDIT:
        if (idNotif == EN_UPDATE)
        {
            m_lastModifiedAt = FileTime().Milliseconds;
        }
        break;
    case IDC_DTTM_YEAR_CHECK:
    case IDC_DTTM_MONTH_CHECK:
    case IDC_DTTM_DAY_CHECK:
    case IDC_DTTM_HOUR_CHECK:
    case IDC_DTTM_MINUTE_CHECK:
    case IDC_DTTM_SECOND_CHECK:
    case IDC_DTTM_MILLISECOND_CHECK:
    case IDC_DTTM_OFFSET_CHECK:
        EnableWindow(idChild - 1, !ButtonIsChecked(idChild));
        break;
    case IDC_DTTM_OFFSET_COMBO:
        if (idNotif == CBN_SELCHANGE)
        {
            OnOffsetChange();
        }
        break;
    case IDC_DTTM_ISO8601_RADIO:
    case IDC_DTTM_DATETIME_RADIO:
    case IDC_DTTM_DATETIME_NOSEP_RADIO:
    case IDC_DTTM_ISO8601_MS_RADIO:
    case IDC_DTTM_DATETIME_MS_RADIO:
    case IDC_DTTM_DATETIME_MS_NOSEP_RADIO:
    case IDC_DTTM_DATEONLY_RADIO:
    case IDC_DTTM_DATEONLY_SLASH_RADIO:
    case IDC_DTTM_DATEONLY_NOSEP_RADIO:
    case IDC_DTTM_TIMEONLY_RADIO:
    case IDC_DTTM_TIMEONLY_NOSEP_RADIO:
    case IDC_DTTM_TIMEONLY_MS_RADIO:
    case IDC_DTTM_TIMEONLY_MS_NOSEP_RADIO:
    case IDC_DTTM_SECONDS_RADIO:
    case IDC_DTTM_MILLISECONDS_RADIO:
    case IDC_DTTM_FILETIME_RADIO:
        FormatString(idChild);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


INT_PTR DateTimeDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case DTTM_TIMER1SEC:
    {
        LONGLONG threshold = FileTime().AddSeconds(-3).Milliseconds;
        if (0 < m_lastModifiedAt && m_lastModifiedAt <= threshold)
        {
            m_lastModifiedAt = 0;
            SYSTEMTIME st = { 0 };
            GetSystemTime(st);
            SetSystemTime(st);
            FormatString(m_format);
        }
        break;
    }
    default:
        break;
    }
    return 0;
}


void DateTimeDialogBox::OnCopy()
{
    if (m_lastModifiedAt)
    {
        m_lastModifiedAt = 0;
        SYSTEMTIME st = { 0 };
        GetSystemTime(st);
        SetSystemTime(st);
        FormatString(m_format);
    }
    if (!Clipboard::Write(hwnd, GetText(IDC_DTTM_EDIT)))
    {
        MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
}


void DateTimeDialogBox::OnExecute()
{
    m_lastModifiedAt = 0;
    UpdateDateTime();
    FormatString(m_format);
}


void DateTimeDialogBox::OnOffsetChange()
{
    m_lastModifiedAt = 0;
    SYSTEMTIME st = { 0 };
    GetSystemTime(st);
    m_offset = ComboBoxGetSelection(IDC_DTTM_OFFSET_COMBO);
    SetSystemTime(st);
    FormatString(m_format);
}


void DateTimeDialogBox::GetSystemTime(SYSTEMTIME& st)
{
    st.wYear = 2099;
    st.wMonth = 12;
    st.wDayOfWeek = 0;
    st.wDay = 31;
    st.wHour = 23;
    st.wMinute = 59;
    st.wSecond = 59;
    st.wMilliseconds = 999;
    long year = wcstol(GetText(IDC_DTTM_YEAR_EDIT), nullptr, 10);
    if (year < 1970)
    {
        st.wYear = 1970;
    }
    else if (year < static_cast<long>(st.wYear))
    {
        st.wYear = static_cast<WORD>(year);
    }
    long month = wcstol(GetText(IDC_DTTM_MONTH_EDIT), nullptr, 10);
    if (month < 1)
    {
        st.wMonth = 1;
    }
    else if (month < static_cast<long>(st.wMonth))
    {
        st.wMonth = static_cast<WORD>(month);
    }
    long day = wcstol(GetText(IDC_DTTM_DAY_EDIT), nullptr, 10);
    if (day < 1)
    {
        st.wDay = 1;
    }
    else
    {
        st.wDay = GetLastDayOfMonth(st.wYear, st.wMonth);
        if (day < static_cast<long>(st.wDay))
        {
            st.wDay = static_cast<WORD>(day);
        }
    }
    long hour = wcstol(GetText(IDC_DTTM_HOUR_EDIT), nullptr, 10);
    if (hour < 0)
    {
        st.wHour = 0;
    }
    else if (hour < static_cast<long>(st.wHour))
    {
        st.wHour = static_cast<WORD>(hour);
    }
    long minute = wcstol(GetText(IDC_DTTM_MINUTE_EDIT), nullptr, 10);
    if (minute < 0)
    {
        st.wMinute = 0;
    }
    else if (minute < static_cast<long>(st.wMinute))
    {
        st.wMinute = static_cast<WORD>(minute);
    }
    long second = wcstol(GetText(IDC_DTTM_SECOND_EDIT), nullptr, 10);
    if (second < 0)
    {
        st.wSecond = 0;
    }
    else if (second < static_cast<long>(st.wSecond))
    {
        st.wSecond = static_cast<WORD>(second);
    }
    long millisecond = wcstol(GetText(IDC_DTTM_MILLISECOND_EDIT), nullptr, 10);
    if (millisecond < 0)
    {
        st.wMilliseconds = 0;
    }
    else if (millisecond < static_cast<long>(st.wMilliseconds))
    {
        st.wMilliseconds = static_cast<WORD>(millisecond);
    }
    FileTime ft(st);
    ft.AddMinutes(-m_offset);
    ft.ToSystemTime(st);
}


void DateTimeDialogBox::SetSystemTime(SYSTEMTIME& st)
{
    FileTime ft(st);
    ft.AddMinutes(m_offset);
    ft.ToSystemTime(st);
    SetText(IDC_DTTM_YEAR_EDIT, String(PRINTF, L"%d", st.wYear));
    SetText(IDC_DTTM_MONTH_EDIT, String(PRINTF, L"%d", st.wMonth));
    SetText(IDC_DTTM_DAY_EDIT, String(PRINTF, L"%d", st.wDay));
    SetText(IDC_DTTM_HOUR_EDIT, String(PRINTF, L"%d", st.wHour));
    SetText(IDC_DTTM_MINUTE_EDIT, String(PRINTF, L"%d", st.wMinute));
    SetText(IDC_DTTM_SECOND_EDIT, String(PRINTF, L"%d", st.wSecond));
    SetText(IDC_DTTM_MILLISECOND_EDIT, String(PRINTF, L"%d", st.wMilliseconds));
}


static String ToOffsetString(int offset)
{
    return offset > 0 ? String(PRINTF, L"+%02d:%02d", offset / 60, offset % 60) :
        offset < 0 ? String(PRINTF, L"-%02d:%02d", -offset / 60, -offset % 60) :
        String(L"Z");
}


void DateTimeDialogBox::UpdateDateTime()
{
    SYSTEMTIME st = { 0 };
    FileTime ft;
    ft.AddMinutes(m_offset);
    ft.ToSystemTime(st);
    if (!ButtonIsChecked(IDC_DTTM_YEAR_CHECK))
    {
        SetText(IDC_DTTM_YEAR_EDIT, String(PRINTF, L"%d", st.wYear));
    }
    if (!ButtonIsChecked(IDC_DTTM_MONTH_CHECK))
    {
        SetText(IDC_DTTM_MONTH_EDIT, String(PRINTF, L"%d", st.wMonth));
    }
    if (!ButtonIsChecked(IDC_DTTM_DAY_CHECK))
    {
        SetText(IDC_DTTM_DAY_EDIT, String(PRINTF, L"%d", st.wDay));
    }
    if (!ButtonIsChecked(IDC_DTTM_HOUR_CHECK))
    {
        SetText(IDC_DTTM_HOUR_EDIT, String(PRINTF, L"%d", st.wHour));
    }
    if (!ButtonIsChecked(IDC_DTTM_MINUTE_CHECK))
    {
        SetText(IDC_DTTM_MINUTE_EDIT, String(PRINTF, L"%d", st.wMinute));
    }
    if (!ButtonIsChecked(IDC_DTTM_SECOND_CHECK))
    {
        SetText(IDC_DTTM_SECOND_EDIT, String(PRINTF, L"%d", st.wSecond));
    }
    if (!ButtonIsChecked(IDC_DTTM_MILLISECOND_CHECK))
    {
        SetText(IDC_DTTM_MILLISECOND_EDIT, String(PRINTF, L"%d", st.wMilliseconds));
    }
}


void DateTimeDialogBox::FormatString(int id)
{
    SYSTEMTIME st = { 0 };
    st.wYear = static_cast<WORD>(wcstol(GetText(IDC_DTTM_YEAR_EDIT), nullptr, 10));
    st.wMonth = static_cast<WORD>(wcstol(GetText(IDC_DTTM_MONTH_EDIT), nullptr, 10));
    st.wDay = static_cast<WORD>(wcstol(GetText(IDC_DTTM_DAY_EDIT), nullptr, 10));
    st.wHour = static_cast<WORD>(wcstol(GetText(IDC_DTTM_HOUR_EDIT), nullptr, 10));
    st.wMinute = static_cast<WORD>(wcstol(GetText(IDC_DTTM_MINUTE_EDIT), nullptr, 10));
    st.wSecond = static_cast<WORD>(wcstol(GetText(IDC_DTTM_SECOND_EDIT), nullptr, 10));
    st.wMilliseconds = static_cast<WORD>(wcstol(GetText(IDC_DTTM_MILLISECOND_EDIT), nullptr, 10));
    switch (id)
    {
    case IDC_DTTM_ISO8601_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u-%02u-%02uT%02u:%02u:%02u%s",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, ToOffsetString(m_offset)));
        break;
    case IDC_DTTM_DATETIME_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u-%02u-%02uT%02u:%02u:%02u",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond));
        break;
    case IDC_DTTM_DATETIME_NOSEP_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u%02u%02u%02u%02u%02u",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond));
        break;
    case IDC_DTTM_ISO8601_MS_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u-%02u-%02uT%02u:%02u:%02u.%03u%s",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, ToOffsetString(m_offset)));
        break;
    case IDC_DTTM_DATETIME_MS_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u-%02u-%02uT%02u:%02u:%02u.%03u",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
        break;
    case IDC_DTTM_DATETIME_MS_NOSEP_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u%02u%02u%02u%02u%02u%03u",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
        break;
    case IDC_DTTM_DATEONLY_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u-%02u-%02u",
            st.wYear, st.wMonth, st.wDay));
        break;
    case IDC_DTTM_DATEONLY_SLASH_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u/%02u/%02u",
            st.wYear, st.wMonth, st.wDay));
        break;
    case IDC_DTTM_DATEONLY_NOSEP_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u%02u%02u",
            st.wYear, st.wMonth, st.wDay));
        break;
    case IDC_DTTM_TIMEONLY_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%02u:%02u:%02u",
            st.wHour, st.wMinute, st.wSecond));
        break;
    case IDC_DTTM_TIMEONLY_NOSEP_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%02u%02u%02u",
            st.wHour, st.wMinute, st.wSecond));
        break;
    case IDC_DTTM_TIMEONLY_MS_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%02u:%02u:%02u.%03u",
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
        break;
    case IDC_DTTM_TIMEONLY_MS_NOSEP_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%02u%02u%02u%03u",
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
        break;
    case IDC_DTTM_SECONDS_RADIO:
    {
        FileTime ft(st);
        ft.AddMinutes(-m_offset);
        SYSTEMTIME ue = { 0 };
        ue.wYear = 1970;
        ue.wMonth = 1;
        ue.wDay = 1;
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%llu", ft.Seconds - FileTime(ue).Seconds));
        break;
    }
    case IDC_DTTM_MILLISECONDS_RADIO:
    {
        FileTime ft(st);
        ft.AddMinutes(-m_offset);
        SYSTEMTIME ue = { 0 };
        ue.wYear = 1970;
        ue.wMonth = 1;
        ue.wDay = 1;
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%llu", ft.Milliseconds - FileTime(ue).Milliseconds));
        break;
    }
    case IDC_DTTM_FILETIME_RADIO:
    {
        FileTime ft(st);
        ft.AddMinutes(-m_offset);
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%llu", ft.Intervals));
        break;
    }
    default:
        break;
    }
    m_format = id;
}
