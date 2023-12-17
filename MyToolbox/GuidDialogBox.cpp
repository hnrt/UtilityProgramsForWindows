#include "pch.h"
#include "GuidDialogBox.h"
#include "resource.h"
#include "hnrt/Menu.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Clipboard.h"
#include "hnrt/ResourceString.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"


#define REG_KEY L"SOFTWARE\\hnrt\\MyToolbox\\Guid"
#define REG_NAME L"Format"
#define REG_NAME_LAST L"Last"


using namespace hnrt;


GuidDialogBox::GuidDialogBox()
    : MyDialogBox(IDD_GUID)
    , m_guid()
    , m_szFormatted()
    , m_uCurrentlySelected(IDC_GUID_RADIO_UPPERCASE)
{
}


void GuidDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REG_KEY, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
        m_uCurrentlySelected = RegistryValue::GetDWORD(hKey, REG_NAME, 1) + IDC_GUID_RADIO_UPPERCASE - 1;
    }
    ChangeGuid();
    CheckButton(m_uCurrentlySelected);
    m_menuView
        .Add(ResourceString(IDS_MENU_GUID), IDM_VIEW_GUID);
}


void GuidDialogBox::OnDestroy()
{
    RegistryKey hKey;
    DWORD dwRet = hKey.Create(HKEY_CURRENT_USER, REG_KEY, 0, KEY_WRITE);
    if (dwRet == ERROR_SUCCESS)
    {
        dwRet = RegistryValue::SetDWORD(hKey, REG_NAME, m_uCurrentlySelected - IDC_GUID_RADIO_UPPERCASE + 1);
        if (dwRet != ERROR_SUCCESS)
        {
            Debug::Put(L"Failed to set DWORD to HKCU\\%s\\%s: %s", REG_KEY, REG_NAME, ErrorMessage::Get(dwRet));
        }
        dwRet = RegistryValue::SetSZ(hKey, REG_NAME_LAST, m_szFormatted);
        if (dwRet != ERROR_SUCCESS)
        {
            Debug::Put(L"Failed to set SZ to HKCU\\%s\\%s: %s", REG_KEY, REG_NAME_LAST, ErrorMessage::Get(dwRet));
        }
    }
    MyDialogBox::OnDestroy();
}


void GuidDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_BUTTON_NEW, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_BUTTON_COPY, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_FORMAT_GROUPBOX, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_RESULT_GROUPBOX, 0, 0, cxDelta, cyDelta);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_RESULT, 0, 0, cxDelta, cyDelta);
}


void GuidDialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
    m_menuView
        .Enable(IDM_VIEW_GUID, MF_ENABLED);
}


void GuidDialogBox::OnTabSelectionChanged()
{
    MyDialogBox::OnTabSelectionChanged();
    m_menuEdit
        .Add(ResourceString(IDS_MENU_NEW), IDM_EDIT_EXECUTE)
        .Add(ResourceString(IDS_MENU_COPYRESULT), IDM_EDIT_COPY);
    m_menuView
        .Enable(IDM_VIEW_GUID, MF_DISABLED);
}


INT_PTR GuidDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case IDC_GUID_BUTTON_COPY:
        OnCopy();
        break;
    case IDC_GUID_BUTTON_NEW:
        ChangeGuid();
        break;
    case IDC_GUID_RADIO_UPPERCASE:
    case IDC_GUID_RADIO_LOWERCASE:
    case IDC_GUID_RADIO_IMPLEMENT:
    case IDC_GUID_RADIO_DEFINE:
    case IDC_GUID_RADIO_STRUCT:
    case IDC_GUID_RADIO_REGISTRY:
    case IDC_GUID_RADIO_SQUARE:
    case IDC_GUID_RADIO_ANGLE:
        ChangeFormat(LOWORD(wParam));
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


void GuidDialogBox::OnCopy()
{
    if (!Clipboard::Write(hwnd, m_szFormatted))
    {
        MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
}


void GuidDialogBox::OnExecute()
{
    ChangeGuid();
}


void GuidDialogBox::ChangeGuid()
{
    HRESULT hRes = CoCreateGuid(&m_guid);
    if (hRes == S_OK)
    {
        ChangeFormat();
    }
    else
    {
        m_szFormatted.Format(L"CoCreateGuid failed. (%08X)", (int)hRes);
        SetText(IDC_GUID_RESULT, m_szFormatted);
        DisableWindow(IDC_GUID_BUTTON_COPY);
    }
}


void GuidDialogBox::ChangeFormat(UINT uSelected)
{
    if (uSelected)
    {
        m_uCurrentlySelected = uSelected;
    }
    OLECHAR buf[64];
    if (!StringFromGUID2(m_guid, buf, _countof(buf)))
    {
        m_szFormatted = L"StringFromGUID2 failed.";
        SetText(IDC_GUID_RESULT, m_szFormatted);
        DisableWindow(IDC_GUID_BUTTON_COPY);
        return;
    }
    GUID& guid = m_guid;
    switch (m_uCurrentlySelected)
    {
    case IDC_GUID_RADIO_LOWERCASE:
        m_szFormatted.Assign(&buf[1], 36).Lowercase();
        break;
    case IDC_GUID_RADIO_IMPLEMENT:
        /*
        * // {CC9CCEEF-195A-4539-9378-D083818A61C6}
        * IMPLEMENT_OLECREATE(<<class>>, <<external_name>>,
        * 0xcc9cceef, 0x195a, 0x4539, 0x93, 0x78, 0xd0, 0x83, 0x81, 0x8a, 0x61, 0xc6);
        */
        m_szFormatted.Format(
            L"// %s\r\nIMPLEMENT_OLECREATE(<<class>>, <<external_name>>,\r\n0x%08lx, 0x%04x, 0x%04x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x);",
            buf, guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
        break;
    case IDC_GUID_RADIO_DEFINE:
        /*
        * // {CC9CCEEF-195A-4539-9378-D083818A61C6}
        * DEFINE_GUID(<<name>>,
        * 0xcc9cceef, 0x195a, 0x4539, 0x93, 0x78, 0xd0, 0x83, 0x81, 0x8a, 0x61, 0xc6);
        */
        m_szFormatted.Format(
            L"// %s\r\nDEFINE_GUID(<<name>>,\r\n0x%08lx, 0x%04x, 0x%04x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x);",
            buf, guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
        break;
    case IDC_GUID_RADIO_STRUCT:
        /*
        * // {CC9CCEEF-195A-4539-9378-D083818A61C6}
        * static const GUID <<name>> =
        * { 0xcc9cceef, 0x195a, 0x4539, { 0x93, 0x78, 0xd0, 0x83, 0x81, 0x8a, 0x61, 0xc6 } };
        */
        m_szFormatted.Format(
            L"// %s\r\nstatic const GUID <<name>> = \r\n{ 0x%08lx, 0x%04x, 0x%04x, { 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x } };",
            buf, guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
        break;
    case IDC_GUID_RADIO_REGISTRY:
        /*
        * {CC9CCEEF-195A-4539-9378-D083818A61C6}
        */
        m_szFormatted = buf;
        break;
    case IDC_GUID_RADIO_SQUARE:
        /*
        * [Guid("CC9CCEEF-195A-4539-9378-D083818A61C6")]
        */
        m_szFormatted.Format(L"[Guid(\"%.36s\")]", & buf[1]);
        break;
    case IDC_GUID_RADIO_ANGLE:
        /*
        * <Guid("CC9CCEEF-195A-4539-9378-D083818A61C6")>
        */
        m_szFormatted.Format(L"<Guid(\"%.36s\")>", &buf[1]);
        break;
    case IDC_GUID_RADIO_UPPERCASE:
    default:
        m_szFormatted.Assign(&buf[1], 36);
        break;
    }
    SetText(IDC_GUID_RESULT, m_szFormatted);
    EnableWindow(IDC_GUID_BUTTON_COPY);
}
