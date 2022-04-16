#include "framework.h"
#include "GuidGen2.h"


#pragma comment(lib, "Ole32")


using namespace hnrt;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    _wsetlocale(LC_ALL, L"");

    GuidGenerator2 app;

    HWND hDlg = app.CreateDialog(hInstance);
    if (!hDlg)
    {
        MessageBoxW(NULL, L"CreateDialog failed.", L"ERROR", MB_OK | MB_ICONERROR);
        return 1;
    }
    ShowWindow(hDlg, nCmdShow);
    UpdateWindow(hDlg);

    HACCEL hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(IDR_ACCELERATOR1));

    while (1)
    {
        MSG msg;
        BOOL bRet = GetMessageW(&msg, NULL, 0, 0);
        if (bRet == -1)
        {
            MessageBoxW(hDlg, L"GetMessage failed.", L"ERROR", MB_OK | MB_ICONERROR);
            return 1;
        }
        else if (!bRet)
        {
            break;
        }
        if (TranslateAcceleratorW(hDlg, hAccelTable, &msg))
        {
            continue;
        }
        else if (IsDialogMessage(hDlg, &msg))
        {
            continue;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DestroyWindow(hDlg);

    return 0;
}


#define REG_KEY L"SOFTWARE\\hnrt\\GuidGen2"
#define REG_NAME L"Format"


GuidGenerator2::GuidGenerator2()
    : m_guid()
    , m_szFormatted()
    , m_uCurrentlySelected(IDC_RADIO_UPPERCASE)
{
    memset(&m_guid, 0, sizeof(m_guid));
    memset(&m_szFormatted, 0, sizeof(m_szFormatted));
}


HWND GuidGenerator2::CreateDialog(HINSTANCE hInstance)
{
    return CreateDialogParamW(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, ProcessMessage, reinterpret_cast<LPARAM>(this));
}


INT_PTR CALLBACK GuidGenerator2::ProcessMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        reinterpret_cast<GuidGenerator2*>(lParam)->OnCreate(hDlg);
        return (INT_PTR)TRUE;

    case WM_DESTROY:
        GetInstance(hDlg)->OnDestory(hDlg);
        return (INT_PTR)TRUE;

    case WM_CLOSE:
        GetInstance(hDlg)->OnExit(hDlg);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_EXIT:
            GetInstance(hDlg)->OnExit(hDlg);
            return (INT_PTR)TRUE;
        case IDC_BUTTON_COPY:
        case IDM_COPY:
            GetInstance(hDlg)->CopyToClipboard(hDlg);
            return (INT_PTR)TRUE;
        case IDC_BUTTON_NEW:
            GetInstance(hDlg)->ChangeGuid(hDlg);
            return (INT_PTR)TRUE;
        case IDC_RADIO_UPPERCASE:
        case IDC_RADIO_LOWERCASE:
        case IDC_RADIO_IMPLEMENT:
        case IDC_RADIO_DEFINE:
        case IDC_RADIO_STRUCT:
        case IDC_RADIO_REGISTRY:
        case IDC_RADIO_SQUARE:
        case IDC_RADIO_ANGLE:
            GetInstance(hDlg)->ChangeFormat(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        default:
            break;
        }
        break;

    default:
        break;
    }

    return (INT_PTR)FALSE;
}


GuidGenerator2* GuidGenerator2::GetInstance(HWND hDlg)
{
    return reinterpret_cast<GuidGenerator2*>(GetWindowLongPtrW(hDlg, GWLP_USERDATA));
}


void GuidGenerator2::OnCreate(HWND hDlg)
{
    SetWindowLongPtr(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    HKEY hKey = NULL;
    DWORD dwRet = RegOpenKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, KEY_READ, &hKey);
    if (dwRet == ERROR_SUCCESS)
    {
        DWORD dwType = 0, dwValue = 0, dwLen = sizeof(DWORD);
        dwRet = RegQueryValueExW(hKey, REG_NAME, NULL, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &dwLen);
        if (dwRet == ERROR_SUCCESS && dwType == REG_DWORD)
        {
            m_uCurrentlySelected = dwValue + IDC_RADIO_UPPERCASE - 1;
        }
        RegCloseKey(hKey);
    }
    ChangeGuid(hDlg);
    SendDlgItemMessageW(hDlg, m_uCurrentlySelected, BM_SETCHECK, BST_CHECKED, 0);
}


void GuidGenerator2::OnDestory(HWND hDlg)
{
    HKEY hKey = NULL;
    DWORD dwDisposition = 0;
    DWORD dwRet = RegCreateKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, NULL, 0, KEY_WRITE, NULL, &hKey, &dwDisposition);
    if (dwRet == ERROR_SUCCESS)
    {
        DWORD dwValue = m_uCurrentlySelected - IDC_RADIO_UPPERCASE + 1, dwLen = sizeof(DWORD);
        dwRet = RegSetValueExW(hKey, REG_NAME, 0, REG_DWORD, reinterpret_cast<LPBYTE>(&dwValue), dwLen);
        if (dwRet == ERROR_SUCCESS)
        {
            // OK
        }
        RegCloseKey(hKey);
    }
    SetWindowLongPtr(hDlg, GWLP_USERDATA, 0L);
}


void GuidGenerator2::OnExit(HWND hDlg)
{
    UNREFERENCED_PARAMETER(hDlg);
    PostQuitMessage(EXIT_SUCCESS);
}


void GuidGenerator2::ChangeGuid(HWND hDlg)
{
    HRESULT hRes = CoCreateGuid(&m_guid);
    if (hRes == S_OK)
    {
        ChangeFormat(hDlg);
    }
    else
    {
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"CoCreateGuid failed. (%08X)", (int)hRes);
        SendDlgItemMessageW(hDlg, IDC_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(m_szFormatted));
        EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_COPY), FALSE);
    }
}


void GuidGenerator2::ChangeFormat(HWND hDlg, UINT uSelected)
{
    if (uSelected)
    {
        m_uCurrentlySelected = uSelected;
    }
    OLECHAR buf[64];
    if (!StringFromGUID2(m_guid, buf, _countof(buf)))
    {
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"StringFromGUID2 failed.");
        SendDlgItemMessageW(hDlg, IDC_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(m_szFormatted));
        EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_COPY), FALSE);
        return;
    }
    GUID& guid = m_guid;
    switch (m_uCurrentlySelected)
    {
    case IDC_RADIO_LOWERCASE:
        _wcslwr_s(buf);
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"%.36s", &buf[1]);
        break;
    case IDC_RADIO_IMPLEMENT:
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
    case IDC_RADIO_DEFINE:
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
    case IDC_RADIO_STRUCT:
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
    case IDC_RADIO_REGISTRY:
        /*
        * {CC9CCEEF-195A-4539-9378-D083818A61C6}
        */
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"%s", buf);
        break;
    case IDC_RADIO_SQUARE:
        /*
        * [Guid("CC9CCEEF-195A-4539-9378-D083818A61C6")]
        */
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"[Guid(\"%.36s\")]", &buf[1]);
        break;
    case IDC_RADIO_ANGLE:
        /*
        * <Guid("CC9CCEEF-195A-4539-9378-D083818A61C6")>
        */
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"<Guid(\"%.36s\")>", &buf[1]);
        break;
    case IDC_RADIO_UPPERCASE:
    default:
        _snwprintf_s(m_szFormatted, _TRUNCATE, L"%.36s", &buf[1]);
        break;
    }
    SendDlgItemMessageW(hDlg, IDC_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(m_szFormatted));
    EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_COPY), TRUE);
}


void GuidGenerator2::CopyToClipboard(HWND hDlg)
{
    SIZE_T cbLen = (wcslen(m_szFormatted) + 1) * sizeof(WCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, cbLen);
    if (hMem == NULL)
    {
        MessageBoxW(hDlg, L"Failed to copy text to Clipboard.", L"ERROR", MB_OK | MB_ICONERROR);
        return;
    }
    memcpy_s(GlobalLock(hMem), cbLen, m_szFormatted, cbLen);
    GlobalUnlock(hMem);
    OpenClipboard(hDlg);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, hMem);
    CloseClipboard();
}
