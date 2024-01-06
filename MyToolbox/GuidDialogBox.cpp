#include "pch.h"
#include "GuidDialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/Menu.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Clipboard.h"
#include "hnrt/ResourceString.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"


#define REGVAL_FORMAT L"Format"
#define REGVAL_LAST L"Last"


using namespace hnrt;


GuidDialogBox::GuidDialogBox()
    : MyDialogBox(IDD_GUID, L"Guid")
    , m_guid()
    , m_szFormatted()
    , m_uCurrentlySelected(IDC_GUID_UPPERCASE_RADIO)
{
}


void GuidDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    ByteString initData;
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
        m_uCurrentlySelected = RegistryValue::GetDWORD(hKey, REGVAL_FORMAT, 1) + IDC_GUID_UPPERCASE_RADIO - 1;
        try
        {
            initData = ByteString::FromHex(RegistryValue::GetSZ(hKey, REGVAL_LAST));
        }
        catch (...)
        {
        }
    }
    SetFont(IDC_GUID_RESULT_STATIC, GetApp<MyToolbox>().GetFontForData());
    if (initData.Len == sizeof(m_guid))
    {
        memcpy_s(&m_guid, initData.Len, initData.Ptr, initData.Len);
        ChangeFormat();
    }
    else
    {
        ChangeGuid();
    }
    ButtonCheck(m_uCurrentlySelected);
    m_menuView
        .Add(ResourceString(IDS_MENU_GUID), IDM_VIEW_GUID);
}


void GuidDialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_WRITE);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetDWORD(hKey, REGVAL_FORMAT, m_uCurrentlySelected - IDC_GUID_UPPERCASE_RADIO + 1);
        RegistryValue::SetSZ(hKey, REGVAL_LAST, String::ToHex(&m_guid, sizeof(m_guid), UPPERCASE));
    }
    SetFont(IDC_GUID_RESULT_STATIC, NULL);
    MyDialogBox::OnDestroy();
}


void GuidDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_NEW_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_COPY_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_FORMAT_GROUP, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_RESULT_GROUP, 0, 0, cxDelta, cyDelta);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_RESULT_STATIC, 0, 0, cxDelta, cyDelta);
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
    UNREFERENCED_PARAMETER(lParam);
    UINT idChild = LOWORD(wParam);
    UINT idNotif = HIWORD(wParam);
    switch (idChild)
    {
    case IDC_GUID_COPY_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            OnCopy();
        }
        break;
    case IDC_GUID_NEW_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            ChangeGuid();
        }
        break;
    case IDC_GUID_UPPERCASE_RADIO:
    case IDC_GUID_LOWERCASE_RADIO:
    case IDC_GUID_IMPLEMENT_RADIO:
    case IDC_GUID_DEFINE_RADIO:
    case IDC_GUID_STRUCT_RADIO:
    case IDC_GUID_REGISTRY_RADIO:
    case IDC_GUID_SQUARE_RADIO:
    case IDC_GUID_ANGLE_RADIO:
    case IDC_GUID_HEX_RADIO:
        if (idNotif == BN_CLICKED)
        {
            ChangeFormat(idChild);
        }
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
        SetText(IDC_GUID_RESULT_STATIC, m_szFormatted);
        DisableWindow(IDC_GUID_COPY_BUTTON);
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
        SetText(IDC_GUID_RESULT_STATIC, m_szFormatted);
        DisableWindow(IDC_GUID_COPY_BUTTON);
        return;
    }
    GUID& guid = m_guid;
    switch (m_uCurrentlySelected)
    {
    case IDC_GUID_LOWERCASE_RADIO:
        m_szFormatted.Assign(&buf[1], 36).Lowercase();
        break;
    case IDC_GUID_IMPLEMENT_RADIO:
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
    case IDC_GUID_DEFINE_RADIO:
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
    case IDC_GUID_STRUCT_RADIO:
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
    case IDC_GUID_REGISTRY_RADIO:
        /*
        * {CC9CCEEF-195A-4539-9378-D083818A61C6}
        */
        m_szFormatted = buf;
        break;
    case IDC_GUID_SQUARE_RADIO:
        /*
        * [Guid("CC9CCEEF-195A-4539-9378-D083818A61C6")]
        */
        m_szFormatted.Format(L"[Guid(\"%.36s\")]", & buf[1]);
        break;
    case IDC_GUID_ANGLE_RADIO:
        /*
        * <Guid("CC9CCEEF-195A-4539-9378-D083818A61C6")>
        */
        m_szFormatted.Format(L"<Guid(\"%.36s\")>", &buf[1]);
        break;
    case IDC_GUID_HEX_RADIO:
        m_szFormatted = String::ToHex(&m_guid, sizeof(m_guid), UPPERCASE);
        break;
    case IDC_GUID_UPPERCASE_RADIO:
    default:
        m_szFormatted.Assign(&buf[1], 36);
        break;
    }
    SetText(IDC_GUID_RESULT_STATIC, m_szFormatted);
    EnableWindow(IDC_GUID_COPY_BUTTON);
}
