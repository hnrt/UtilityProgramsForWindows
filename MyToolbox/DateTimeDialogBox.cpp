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
#define REGVAL_FIXED L"Fixed"


#define DTTM_TIMER1SEC 8001


using namespace hnrt;


DateTimeDialogBox::DateTimeDialogBox()
    : MyDialogBox(IDD_DTTM, L"DateTime")
    , m_offset(0)
    , m_format(IDC_DTTM_ISO8601_RADIO)
    , m_lastModifiedBy(0)
    , m_lastModifiedAt(0)
{
}


void DateTimeDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    InitializeOffsetComboBox(IDC_DTTM_OFFSET_COMBO);
    SYSTEMTIME st = { 0 };
    ::GetSystemTime(&st);
    DWORD dwFixed = 0;
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
        FileTime(RegistryValue::GetQWORD(hKey, REGVAL_LAST, FileTime().Intervals)).ToSystemTime(st);
        m_offset = static_cast<int>(RegistryValue::GetDWORD(hKey, REGVAL_OFFSET));
        m_format = RegistryValue::GetDWORD(hKey, REGVAL_FORMAT, IDC_DTTM_ISO8601_RADIO - IDC_DTTM_EDIT) + IDC_DTTM_EDIT;
        dwFixed = RegistryValue::GetDWORD(hKey, REGVAL_FIXED);
    }
    ComboBoxSetSelection(IDC_DTTM_OFFSET_COMBO, m_offset);
    ButtonCheck(m_format);
    InitializeFixedButtons(dwFixed);
    SetDateTimeInUTC(st);
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
        GetDateTimeInUTC(st);
        RegistryValue::SetQWORD(hKey, REGVAL_LAST, FileTime(st).Intervals);
        RegistryValue::SetDWORD(hKey, REGVAL_OFFSET, m_offset);
        RegistryValue::SetDWORD(hKey, REGVAL_FORMAT, m_format - IDC_DTTM_EDIT);
        RegistryValue::SetDWORD(hKey, REGVAL_FIXED, GetFixedButtonFlags());
    }
    MyDialogBox::OnDestroy();
}


void DateTimeDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_DTTM_EDIT, 0, 0, cxDelta, 0);
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
    m_menuEdit
        .Add(ResourceString(IDS_MENU_COPY), IDM_EDIT_COPY);
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
    case IDC_DTTM_EDIT:
    case IDC_DTTM_YEAR_EDIT:
    case IDC_DTTM_MONTH_EDIT:
    case IDC_DTTM_DAY_EDIT:
    case IDC_DTTM_HOUR_EDIT:
    case IDC_DTTM_MINUTE_EDIT:
    case IDC_DTTM_SECOND_EDIT:
    case IDC_DTTM_MILLISECOND_EDIT:
        if (idNotif == EN_CHANGE)
        {
            SetLastModifiedBy(idChild);
        }
        break;
    case IDC_DTTM_YEAR_CHECK:
    case IDC_DTTM_MONTH_CHECK:
    case IDC_DTTM_DAY_CHECK:
    case IDC_DTTM_HOUR_CHECK:
    case IDC_DTTM_MINUTE_CHECK:
    case IDC_DTTM_SECOND_CHECK:
    case IDC_DTTM_MILLISECOND_CHECK:
        EditSetReadOnly(idChild - 1, ButtonIsChecked(idChild));
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
    case IDC_DTTM_ISO8601_Z_RADIO:
    case IDC_DTTM_ISO8601_Z_MS_RADIO:
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
        if (IsModified(3))
        {
            ApplyModification();
        }
        break;
    default:
        break;
    }
    return 0;
}


void DateTimeDialogBox::OnCopy()
{
    if (IsModified())
    {
        ApplyModification();
    }
    if (!Clipboard::Write(hwnd, GetText(IDC_DTTM_EDIT)))
    {
        MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
}


void DateTimeDialogBox::OnExecute()
{
    UpdateDateTime();
    FormatString();
}


void DateTimeDialogBox::OnOffsetChange()
{
    SYSTEMTIME st = { 0 };
    GetDateTimeInUTC(st);
    m_offset = ComboBoxGetSelection(IDC_DTTM_OFFSET_COMBO);
    SetDateTimeInUTC(st);
    FormatString();
}


void DateTimeDialogBox::ApplyModification()
{
    if (m_lastModifiedBy == IDC_DTTM_EDIT)
    {
        String sz = GetText(IDC_DTTM_EDIT).Trim();
        wchar_t* pStop = nullptr;
        long long value = wcstoll(sz, &pStop, 10);
        if (pStop && !*pStop && value >= 0)
        {
            SYSTEMTIME st = { 0 };
            switch (m_format)
            {
            case IDC_DTTM_SECONDS_RADIO:
                FileTime((FILETIME_UNIX_EPOCH_TIME_IN_SECONDS + value) * 10000000LL).ToSystemTime(st);
                break;
            case IDC_DTTM_MILLISECONDS_RADIO:
                FileTime((FILETIME_UNIX_EPOCH_TIME_IN_MILLISECONDS + value) * 10000LL).ToSystemTime(st);
                break;
            case IDC_DTTM_FILETIME_RADIO:
            default:
                FileTime(value).ToSystemTime(st);
                break;
            }
            SetDateTimeInUTC(st);
            FormatString();
        }
        else
        {
            ResetLastModifiedBy();
        }
    }
    else
    {
        SYSTEMTIME st = { 0 };
        GetDateTimeInUTC(st);
        SetDateTimeInUTC(st);
        FormatString();
    }
}


void DateTimeDialogBox::GetDateTimeInUTC(SYSTEMTIME& st) const
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
    FileTime(st).AddMinutes(-m_offset).ToSystemTime(st);
}


void DateTimeDialogBox::SetDateTimeInUTC(SYSTEMTIME& st) const
{
    FileTime(st).AddMinutes(m_offset).ToSystemTime(st);
    SetText(IDC_DTTM_YEAR_EDIT, String(PRINTF, L"%d", st.wYear));
    SetText(IDC_DTTM_MONTH_EDIT, String(PRINTF, L"%d", st.wMonth));
    SetText(IDC_DTTM_DAY_EDIT, String(PRINTF, L"%d", st.wDay));
    SetText(IDC_DTTM_HOUR_EDIT, String(PRINTF, L"%d", st.wHour));
    SetText(IDC_DTTM_MINUTE_EDIT, String(PRINTF, L"%d", st.wMinute));
    SetText(IDC_DTTM_SECOND_EDIT, String(PRINTF, L"%d", st.wSecond));
    SetText(IDC_DTTM_MILLISECOND_EDIT, String(PRINTF, L"%d", st.wMilliseconds));
}


void DateTimeDialogBox::UpdateDateTime() const
{
    SYSTEMTIME st = { 0 };
    FileTime().AddMinutes(m_offset).ToSystemTime(st);
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


static String ToOffsetString(int offset)
{
    return offset >= 0 ?
        String(PRINTF, L"+%02d:%02d", offset / 60, offset % 60) :
        String(PRINTF, L"-%02d:%02d", -offset / 60, -offset % 60);
}


void DateTimeDialogBox::FormatString(int id)
{
    if (id)
    {
        m_format = id;
        UpdateEditReadOnly();
    }
    SYSTEMTIME st = { 0 };
    st.wYear = static_cast<WORD>(wcstol(GetText(IDC_DTTM_YEAR_EDIT), nullptr, 10));
    st.wMonth = static_cast<WORD>(wcstol(GetText(IDC_DTTM_MONTH_EDIT), nullptr, 10));
    st.wDay = static_cast<WORD>(wcstol(GetText(IDC_DTTM_DAY_EDIT), nullptr, 10));
    st.wHour = static_cast<WORD>(wcstol(GetText(IDC_DTTM_HOUR_EDIT), nullptr, 10));
    st.wMinute = static_cast<WORD>(wcstol(GetText(IDC_DTTM_MINUTE_EDIT), nullptr, 10));
    st.wSecond = static_cast<WORD>(wcstol(GetText(IDC_DTTM_SECOND_EDIT), nullptr, 10));
    st.wMilliseconds = static_cast<WORD>(wcstol(GetText(IDC_DTTM_MILLISECOND_EDIT), nullptr, 10));
    switch (m_format)
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
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%llu",
            FileTime(st).AddMinutes(-m_offset).Seconds - FILETIME_UNIX_EPOCH_TIME_IN_SECONDS));
        break;
    case IDC_DTTM_MILLISECONDS_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%llu",
            FileTime(st).AddMinutes(-m_offset).Milliseconds - FILETIME_UNIX_EPOCH_TIME_IN_MILLISECONDS));
        break;
    case IDC_DTTM_FILETIME_RADIO:
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%llu",
            FileTime(st).AddMinutes(-m_offset).Intervals));
        break;
    case IDC_DTTM_ISO8601_Z_RADIO:
        FileTime(st).AddMinutes(-m_offset).ToSystemTime(st);
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u-%02u-%02uT%02u:%02u:%02uZ",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond));
        break;
    case IDC_DTTM_ISO8601_Z_MS_RADIO:
        FileTime(st).AddMinutes(-m_offset).ToSystemTime(st);
        SetText(IDC_DTTM_EDIT, String(PRINTF, L"%04u-%02u-%02uT%02u:%02u:%02u.%03uZ",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
        break;
    default:
        break;
    }
    ResetLastModifiedBy();
}


void DateTimeDialogBox::UpdateEditReadOnly() const
{
    switch (m_format)
    {
    case IDC_DTTM_SECONDS_RADIO:
    case IDC_DTTM_MILLISECONDS_RADIO:
    case IDC_DTTM_FILETIME_RADIO:
        EditSetReadOnly(IDC_DTTM_EDIT, FALSE);
        break;
    default:
        EditSetReadOnly(IDC_DTTM_EDIT, TRUE);
        break;
    }
}


void DateTimeDialogBox::InitializeFixedButtons(DWORD dwFixed) const
{
    BOOL bMillisecond = (dwFixed & 1) ? TRUE : FALSE; dwFixed >>= 1;
    BOOL bSecond = (dwFixed & 1) ? TRUE : FALSE; dwFixed >>= 1;
    BOOL bMinute = (dwFixed & 1) ? TRUE : FALSE; dwFixed >>= 1;
    BOOL bHour = (dwFixed & 1) ? TRUE : FALSE; dwFixed >>= 1;
    BOOL bDay = (dwFixed & 1) ? TRUE : FALSE; dwFixed >>= 1;
    BOOL bMonth = (dwFixed & 1) ? TRUE : FALSE; dwFixed >>= 1;
    BOOL bYear = (dwFixed & 1) ? TRUE : FALSE;
    EditSetReadOnly(IDC_DTTM_YEAR_EDIT, bYear);
    ButtonCheck(IDC_DTTM_YEAR_CHECK, bYear);
    EditSetReadOnly(IDC_DTTM_MONTH_EDIT, bMonth);
    ButtonCheck(IDC_DTTM_MONTH_CHECK, bMonth);
    EditSetReadOnly(IDC_DTTM_DAY_EDIT, bDay);
    ButtonCheck(IDC_DTTM_DAY_CHECK, bDay);
    EditSetReadOnly(IDC_DTTM_HOUR_EDIT, bHour);
    ButtonCheck(IDC_DTTM_HOUR_CHECK, bHour);
    EditSetReadOnly(IDC_DTTM_MINUTE_EDIT, bMinute);
    ButtonCheck(IDC_DTTM_MINUTE_CHECK, bMinute);
    EditSetReadOnly(IDC_DTTM_SECOND_EDIT, bSecond);
    ButtonCheck(IDC_DTTM_SECOND_CHECK, bSecond);
    EditSetReadOnly(IDC_DTTM_MILLISECOND_EDIT, bMillisecond);
    ButtonCheck(IDC_DTTM_MILLISECOND_CHECK, bMillisecond);
}


DWORD DateTimeDialogBox::GetFixedButtonFlags() const
{
    DWORD dwFixed = ButtonIsChecked(IDC_DTTM_YEAR_CHECK) ? 1 : 0;
    dwFixed = (dwFixed << 1) | (ButtonIsChecked(IDC_DTTM_MONTH_CHECK) ? 1 : 0);
    dwFixed = (dwFixed << 1) | (ButtonIsChecked(IDC_DTTM_DAY_CHECK) ? 1 : 0);
    dwFixed = (dwFixed << 1) | (ButtonIsChecked(IDC_DTTM_HOUR_CHECK) ? 1 : 0);
    dwFixed = (dwFixed << 1) | (ButtonIsChecked(IDC_DTTM_MINUTE_CHECK) ? 1 : 0);
    dwFixed = (dwFixed << 1) | (ButtonIsChecked(IDC_DTTM_SECOND_CHECK) ? 1 : 0);
    dwFixed = (dwFixed << 1) | (ButtonIsChecked(IDC_DTTM_MILLISECOND_CHECK) ? 1 : 0);
    return dwFixed;
}


void DateTimeDialogBox::SetLastModifiedBy(int id)
{
    m_lastModifiedBy = id;
    m_lastModifiedAt = FileTime().Milliseconds;
}


void DateTimeDialogBox::ResetLastModifiedBy()
{
    m_lastModifiedBy = 0;
    m_lastModifiedAt = 0;
}


bool DateTimeDialogBox::IsModified(int nGracePeriodInSeconds) const
{
    return m_lastModifiedAt == 0 ? false
        : nGracePeriodInSeconds == 0 ? true
        : m_lastModifiedAt <= FileTime().AddSeconds(-nGracePeriodInSeconds).Milliseconds;
}
