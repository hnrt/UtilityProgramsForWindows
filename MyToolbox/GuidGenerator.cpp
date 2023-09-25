#include "pch.h"
#include "GuidGenerator.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"


#define REG_KEY L"SOFTWARE\\hnrt\\MyToolbox\\Guid"
#define REG_NAME L"Format"
#define REG_NAME_LAST L"Last"


using namespace hnrt;


GuidGenerator::GuidGenerator()
    : DialogBox(IDD_GUID)
    , m_guid()
    , m_szFormatted()
    , m_uCurrentlySelected(IDC_GUID_RADIO_UPPERCASE)
{
}


void GuidGenerator::OnCreate()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REG_KEY, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue value;
        m_uCurrentlySelected = value.GetDWORD(hKey, REG_NAME, 1) + IDC_GUID_RADIO_UPPERCASE - 1;
    }
    ChangeGuid();
    SendMessage(m_uCurrentlySelected, BM_SETCHECK, BST_CHECKED, 0);
}


void GuidGenerator::OnDestroy()
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
}


void GuidGenerator::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_BUTTON_NEW, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_BUTTON_COPY, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_FORMAT_GROUPBOX, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_RESULT_GROUPBOX, 0, 0, cxDelta, cyDelta);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_RESULT, 0, 0, cxDelta, cyDelta);
}


INT_PTR GuidGenerator::OnCommand(WPARAM wParam, LPARAM lParam)
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


void GuidGenerator::ChangeGuid()
{
    HRESULT hRes = CoCreateGuid(&m_guid);
    if (hRes == S_OK)
    {
        ChangeFormat();
    }
    else
    {
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"CoCreateGuid failed. (%08X)", (int)hRes);
        SendMessage(IDC_GUID_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(m_szFormatted));
        DisableWindow(IDC_GUID_BUTTON_COPY);
    }
}


void GuidGenerator::ChangeFormat(UINT uSelected)
{
    if (uSelected)
    {
        m_uCurrentlySelected = uSelected;
    }
    OLECHAR buf[64];
    if (!StringFromGUID2(m_guid, buf, _countof(buf)))
    {
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"StringFromGUID2 failed.");
        SendMessage(IDC_GUID_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(m_szFormatted));
        DisableWindow(IDC_GUID_BUTTON_COPY);
        return;
    }
    GUID& guid = m_guid;
    switch (m_uCurrentlySelected)
    {
    case IDC_GUID_RADIO_LOWERCASE:
        _wcslwr_s(buf);
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"%.36s", &buf[1]);
        break;
    case IDC_GUID_RADIO_IMPLEMENT:
        /*
        * // {CC9CCEEF-195A-4539-9378-D083818A61C6}
        * IMPLEMENT_OLECREATE(<<class>>, <<external_name>>,
        * 0xcc9cceef, 0x195a, 0x4539, 0x93, 0x78, 0xd0, 0x83, 0x81, 0x8a, 0x61, 0xc6);
        */
        _snwprintf_s(m_szFormatted, _TRUNCATE,
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
        _snwprintf_s(m_szFormatted, _TRUNCATE,
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
        _snwprintf_s(m_szFormatted, _TRUNCATE,
            L"// %s\r\nstatic const GUID <<name>> = \r\n{ 0x%08lx, 0x%04x, 0x%04x, { 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x } };",
            buf, guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
        break;
    case IDC_GUID_RADIO_REGISTRY:
        /*
        * {CC9CCEEF-195A-4539-9378-D083818A61C6}
        */
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"%s", buf);
        break;
    case IDC_GUID_RADIO_SQUARE:
        /*
        * [Guid("CC9CCEEF-195A-4539-9378-D083818A61C6")]
        */
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"[Guid(\"%.36s\")]", &buf[1]);
        break;
    case IDC_GUID_RADIO_ANGLE:
        /*
        * <Guid("CC9CCEEF-195A-4539-9378-D083818A61C6")>
        */
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"<Guid(\"%.36s\")>", &buf[1]);
        break;
    case IDC_GUID_RADIO_UPPERCASE:
    default:
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"%.36s", &buf[1]);
        break;
    }
    SendMessage(IDC_GUID_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(m_szFormatted));
    EnableWindow(IDC_GUID_BUTTON_COPY);
}


void GuidGenerator::OnCopy()
{
    SIZE_T cbLen = (wcslen(m_szFormatted) + 1) * sizeof(WCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, cbLen);
    if (hMem == NULL)
    {
        MessageBoxW(hwnd, L"Failed to copy text to Clipboard.", L"ERROR", MB_OK | MB_ICONERROR);
        return;
    }
    memcpy_s(GlobalLock(hMem), cbLen, m_szFormatted, cbLen);
    GlobalUnlock(hMem);
    OpenClipboard(hwnd);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, hMem);
    CloseClipboard();
}
