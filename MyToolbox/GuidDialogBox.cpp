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
#include "hnrt/WhileInScope.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"
#include "hnrt/Debug.h"


using namespace hnrt;


constexpr auto REGVAL_FORMAT = L"Format";
constexpr auto REGVAL_LAST = L"Last";


GuidDialogBox::GuidDialogBox()
    : MyDialogBox(IDD_GUID, L"Guid", 0, IDC_GUID_EDIT)
    , m_guid()
    , m_szFormatted()
    , m_uCurrentlySelected(IDC_GUID_UPPERCASE_RADIO)
{
}


void GuidDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    SetFont(IDC_GUID_EDIT, GetApp<MyToolbox>().GetFontForData());
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
    ButtonCheck(m_uCurrentlySelected);
    if (initData.Len == sizeof(m_guid))
    {
        memcpy_s(&m_guid, initData.Len, initData.Ptr, initData.Len);
        ChangeFormat();
    }
    else
    {
        ChangeGuid();
    }
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
        RegistryValue::SetSZ(hKey, REGVAL_LAST, ByteString(&m_guid, sizeof(m_guid)).ToHex(UPPERCASE));
    }
    SetFont(IDC_GUID_EDIT, NULL);
    MyDialogBox::OnDestroy();
}


void GuidDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_EDIT, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_COPY_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_GUID_NEW_BUTTON, cxDelta, 0, 0, 0);
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
    m_menuFile
        .Insert(0, ResourceString(IDS_MENU_NEW), IDM_FILE_NEW)
        .InsertSeparator(1);
    m_menuEdit
        .Insert(0, ResourceString(IDS_MENU_COPYRESULT), IDM_EDIT_COPYRESULT)
        .InsertSeparator(1);
    m_menuView
        .Enable(IDM_VIEW_GUID, MF_DISABLED);
}


INT_PTR GuidDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (m_cProcessing)
    {
        return TRUE;
    }
    UINT idChild = LOWORD(wParam);
    UINT idNotif = HIWORD(wParam);
    switch (idChild)
    {
    case IDC_GUID_NEW_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            ChangeGuid();
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_GUID_COPY_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            OnCopyResult();
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_GUID_EDIT:
        return OnEditCommand(wParam, lParam);
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
        else
        {
            return FALSE;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


void GuidDialogBox::OnNew()
{
    ChangeGuid();
}


void GuidDialogBox::OnCopyResult()
{
    if (!Clipboard::Write(hwnd, m_szFormatted))
    {
        MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
}


void GuidDialogBox::ChangeGuid()
{
    HRESULT hRes = CoCreateGuid(&m_guid);
    if (hRes == S_OK)
    {
        SetFlags(0, FLAG_PANE1_ERROR);
        ChangeFormat();
        EnableWindow(IDC_GUID_COPY_BUTTON);
    }
    else
    {
        SetFlags(FLAG_PANE1_ERROR);
        m_szFormatted.Format(L"CoCreateGuid failed. (%08X)", (int)hRes);
        SetText(IDC_GUID_EDIT, m_szFormatted);
        DisableWindow(IDC_GUID_COPY_BUTTON);
    }
}


void GuidDialogBox::ChangeFormat(UINT uSelected)
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (uSelected)
    {
        m_uCurrentlySelected = uSelected;
    }
    OLECHAR buf[64];
    if (!StringFromGUID2(m_guid, buf, _countof(buf)))
    {
        SetFlags(FLAG_PANE1_ERROR);
        m_szFormatted = L"StringFromGUID2 failed.";
        SetText(IDC_GUID_EDIT, m_szFormatted);
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
        m_szFormatted = ByteString(&m_guid, sizeof(m_guid)).ToHex(UPPERCASE);
        break;
    case IDC_GUID_UPPERCASE_RADIO:
    default:
        m_szFormatted.Assign(&buf[1], 36);
        break;
    }
    SetFlags(0, FLAG_PANE1_ERROR);
    SetText(IDC_GUID_EDIT, m_szFormatted);
    EnableWindow(IDC_GUID_COPY_BUTTON);
}


void GuidDialogBox::ParseContent()
{
    try
    {
        String sz = GetText(IDC_GUID_EDIT);
        PCWCHAR pCur = sz;
        GUID guid = { 0 };
        switch (m_uCurrentlySelected)
        {
        case IDC_GUID_UPPERCASE_RADIO:
        case IDC_GUID_LOWERCASE_RADIO:
            pCur = ParseChar(Parse32Bits(SkipWS(pCur), guid.Data1), L'-');
            pCur = ParseChar(Parse16Bits(pCur, guid.Data2), L'-');
            pCur = ParseChar(Parse16Bits(pCur, guid.Data3), L'-');
            pCur = ParseChar(Parse8Bits(pCur, &guid.Data4[0], 2), L'-');
            ParseChar(SkipWS(Parse8Bits(pCur, &guid.Data4[2], 6)), L'\0');
            break;
        case IDC_GUID_IMPLEMENT_RADIO:
            /*
            * // {CC9CCEEF-195A-4539-9378-D083818A61C6}
            * IMPLEMENT_OLECREATE(<<class>>, <<external_name>>,
            * 0xcc9cceef, 0x195a, 0x4539, 0x93, 0x78, 0xd0, 0x83, 0x81, 0x8a, 0x61, 0xc6);
            */
        case IDC_GUID_DEFINE_RADIO:
            /*
            * // {CC9CCEEF-195A-4539-9378-D083818A61C6}
            * DEFINE_GUID(<<name>>,
            * 0xcc9cceef, 0x195a, 0x4539, 0x93, 0x78, 0xd0, 0x83, 0x81, 0x8a, 0x61, 0xc6);
            */
            pCur = ParseChar(SkipWS(Parse32Bits(SkipTo(pCur, L"0x"), guid.Data1)), L',');
            pCur = ParseChar(SkipWS(Parse16Bits(ParseString(SkipWS(pCur), L"0x"), guid.Data2)), L',');
            pCur = ParseChar(SkipWS(Parse16Bits(ParseString(SkipWS(pCur), L"0x"), guid.Data3)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[0], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[1], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[2], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[3], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[4], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[5], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[6], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[7], 1)), L')');
            break;
        case IDC_GUID_STRUCT_RADIO:
            /*
            * // {CC9CCEEF-195A-4539-9378-D083818A61C6}
            * static const GUID <<name>> =
            * { 0xcc9cceef, 0x195a, 0x4539, { 0x93, 0x78, 0xd0, 0x83, 0x81, 0x8a, 0x61, 0xc6 } };
            */
            pCur = ParseChar(Parse32Bits(SkipTo(pCur, L"0x"), guid.Data1), L',');
            pCur = ParseChar(Parse16Bits(ParseString(SkipWS(pCur), L"0x"), guid.Data2), L',');
            pCur = ParseChar(Parse16Bits(ParseString(SkipWS(pCur), L"0x"), guid.Data3), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(ParseChar(SkipWS(pCur), L'{')), L"0x"), &guid.Data4[0], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[1], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[2], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[3], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[4], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[5], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[6], 1)), L',');
            pCur = ParseChar(SkipWS(Parse8Bits(ParseString(SkipWS(pCur), L"0x"), &guid.Data4[7], 1)), L'}');
            break;
        case IDC_GUID_REGISTRY_RADIO:
            /*
            * {CC9CCEEF-195A-4539-9378-D083818A61C6}
            */
            pCur = ParseChar(Parse32Bits(ParseChar(SkipWS(pCur), L'{'), guid.Data1), L'-');
            pCur = ParseChar(Parse16Bits(pCur, guid.Data2), L'-');
            pCur = ParseChar(Parse16Bits(pCur, guid.Data3), L'-');
            pCur = ParseChar(Parse8Bits(pCur, &guid.Data4[0], 2), L'-');
            pCur = ParseChar(Parse8Bits(pCur, &guid.Data4[2], 6), L'}');
            break;
        case IDC_GUID_SQUARE_RADIO:
            /*
            * [Guid("CC9CCEEF-195A-4539-9378-D083818A61C6")]
            */
            pCur = ParseChar(Parse32Bits(ParseString(SkipWS(pCur), L"[Guid(\""), guid.Data1), L'-');
            pCur = ParseChar(Parse16Bits(pCur, guid.Data2), L'-');
            pCur = ParseChar(Parse16Bits(pCur, guid.Data3), L'-');
            pCur = ParseChar(Parse8Bits(pCur, &guid.Data4[0], 2), L'-');
            pCur = ParseString(Parse8Bits(pCur, &guid.Data4[2], 6), L"\")]");
            break;
        case IDC_GUID_ANGLE_RADIO:
            /*
            * <Guid("CC9CCEEF-195A-4539-9378-D083818A61C6")>
            */
            pCur = ParseChar(Parse32Bits(ParseString(SkipWS(pCur), L"<Guid(\""), guid.Data1), L'-');
            pCur = ParseChar(Parse16Bits(pCur, guid.Data2), L'-');
            pCur = ParseChar(Parse16Bits(pCur, guid.Data3), L'-');
            pCur = ParseChar(Parse8Bits(pCur, &guid.Data4[0], 2), L'-');
            pCur = ParseString(Parse8Bits(pCur, &guid.Data4[2], 6), L"\")>");
            break;
        case IDC_GUID_HEX_RADIO:
            ParseChar(SkipWS(Parse8Bits(SkipWS(pCur), reinterpret_cast<unsigned char*>(&guid), sizeof(guid))), L'\0');
            break;
        default:
            return;
        }
        SetFlags(0, FLAG_PANE1_ERROR);
        m_guid = guid;
    }
    catch (Exception e)
    {
        SetFlags(FLAG_PANE1_ERROR, 0);
    }
}


PCWCHAR GuidDialogBox::Parse32Bits(PCWCHAR pCur, unsigned long& parsed)
{
    PWCHAR pStop = nullptr;
    unsigned long value = StrToUnsignedLong(pCur, &pStop, 16);
    if (pStop == pCur + 8)
    {
        parsed = value;
        return pStop;
    }
    else
    {
        throw Exception(L"Parse error.");
    }
}


PCWCHAR GuidDialogBox::Parse16Bits(PCWCHAR pCur, unsigned short& parsed)
{
    PWCHAR pStop = nullptr;
    unsigned long value = StrToUnsignedLong(pCur, &pStop, 16);
    if (pStop == pCur + 4)
    {
        parsed = static_cast<unsigned short>(value);
        return pStop;
    }
    else
    {
        throw Exception(L"Parse error.");
    }
}


PCWCHAR GuidDialogBox::Parse8Bits(PCWCHAR pCur, unsigned char parsed[], size_t length)
{
    for (size_t index = 0; index < length; index++)
    {
        unsigned char b;
        WCHAR c = *pCur++;
        if (L'0' <= c && c <= L'9')
        {
            b = c - L'0';
        }
        else if (L'A' <= c && c <= L'F')
        {
            b = c - L'A' + 10;
        }
        else if (L'a' <= c && c <= L'f')
        {
            b = c - L'a' + 10;
        }
        else
        {
            throw Exception(L"Parse error.");
        }
        b <<= 4;
        c = *pCur++;
        if (L'0' <= c && c <= L'9')
        {
            b |= c - L'0';
        }
        else if (L'A' <= c && c <= L'F')
        {
            b |= c - L'A' + 10;
        }
        else if (L'a' <= c && c <= L'f')
        {
            b |= c - L'a' + 10;
        }
        else
        {
            throw Exception(L"Parse error.");
        }
        parsed[index] = b;
    }
    return pCur;
}


PCWCHAR GuidDialogBox::ParseChar(PCWCHAR pCur, WCHAR c)
{
    if (*pCur == c)
    {
        return pCur + 1;
    }
    else
    {
        throw Exception(L"Parse error.");
    }
}


PCWCHAR GuidDialogBox::ParseString(PCWCHAR pCur, PCWSTR psz)
{
    SIZE_T m = StrLen(pCur);
    SIZE_T n = StrLen(psz);
    if (m >= n && !StrCmp(pCur, n, psz, n))
    {
        return pCur + n;
    }
    else
    {
        throw Exception(L"Parse error.");
    }
}


PCWCHAR GuidDialogBox::SkipTo(PCWCHAR pCur, WCHAR c)
{
    while (true)
    {
        WCHAR next = *pCur++;
        if (next == c)
        {
            break;
        }
        else if (next == L'\0')
        {
            throw Exception(L"Parse error.");
        }

    }
    return pCur;
}


PCWCHAR GuidDialogBox::SkipTo(PCWCHAR pCur, PCWSTR psz)
{
    PCWCHAR pEnd = pCur + StrLen(pCur);
    WCHAR c = *psz++;
    SIZE_T n = StrLen(psz);
    while (true)
    {
        pCur = SkipTo(pCur, c);
        if (pCur + n <= pEnd && !StrCmp(pCur, n, psz, n))
        {
            return pCur + n;
        }
    }
}


PCWCHAR GuidDialogBox::SkipWS(PCWCHAR pCur)
{
    while (iswspace(*pCur))
    {
        pCur++;
    }
    return pCur;
}
