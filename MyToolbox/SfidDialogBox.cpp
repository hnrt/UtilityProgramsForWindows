#include "pch.h"
#include "SfidDialogBox.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Clipboard.h"
#include "hnrt/Exception.h"
#include "hnrt/Time.h"


#define REGVAL_LAST L"Last"


#define SFID_TIMER100MS 8001


#define KEYPREFIX_NAME L"Key Prefix"
#define KEYPREFIX_OFFSET 0
#define KEYPREFIX_LENGTH 3

#define INSTANCE_NAME L"Instance"
#define INSTANCE_OFFSET (KEYPREFIX_OFFSET + KEYPREFIX_LENGTH)
#define INSTANCE_LENGTH 2

#define RESERVED_NAME L"Reserved"
#define RESERVED_VALUE L"0"
#define RESERVED_OFFSET (INSTANCE_OFFSET + INSTANCE_LENGTH)
#define RESERVED_LENGTH 1

#define UNIQUEID_NAME L"Unique Indentifier"
#define UNIQUEID_OFFSET (RESERVED_OFFSET + RESERVED_LENGTH)
#define UNIQUEID_LENGTH 9

#define CHECKSUM_NAME L"Checksum"
#define CHECKSUM_OFFSET (UNIQUEID_OFFSET + UNIQUEID_LENGTH)
#define CHECKSUM_LENGTH 3


#define STATE_IDLE 1
#define STATE_TYPING 2
#define STATE_SUCCESSFUL 3
#define STATE_ERROR 4


using namespace hnrt;


SfidDialogBox::SfidDialogBox()
    : MyDialogBox(IDD_SFID, L"SFID")
    , m_State(STATE_IDLE)
    , m_ActiveEdit(0)
{
    m_LastModified.CursorChange = true;
}


void SfidDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    SetText(IDC_SFID_STATUS_STATIC);
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
        SetText(IDC_SFID_EDIT, RegistryValue::GetSZ(hKey, REGVAL_LAST));
        ApplyModification();
    }
    m_menuView
        .Add(ResourceString(IDS_MENU_SFID), IDM_VIEW_SFID);
}


void SfidDialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_WRITE);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetSZ(hKey, REGVAL_LAST, GetText(IDC_SFID_EDIT));
    }
    MyDialogBox::OnDestroy();
}


void SfidDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_EDIT, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_STATUS_STATIC, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_COPY_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_NEW_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_UP_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_SFID_DOWN_BUTTON, cxDelta, 0, 0, 0);
}


void SfidDialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
    KillTimer(hwnd, SFID_TIMER100MS);
    if (m_LastModified)
    {
        ApplyModification();
    }
    m_menuView
        .Enable(IDM_VIEW_SFID, MF_ENABLED);
}


void SfidDialogBox::OnTabSelectionChanged()
{
    MyDialogBox::OnTabSelectionChanged();
    m_menuEdit
        .Add(ResourceString(IDS_MENU_CUT), IDM_EDIT_CUT)
        .Add(ResourceString(IDS_MENU_COPY), IDM_EDIT_COPY)
        .Add(ResourceString(IDS_MENU_PASTE), IDM_EDIT_PASTE)
        .Add(ResourceString(IDS_MENU_DELETE), IDM_EDIT_DELETE)
        .AddSeparator()
        .Add(ResourceString(IDS_MENU_SELECTALL), IDM_EDIT_SELECTALL)
        .AddSeparator()
        .Add(ResourceString(IDS_MENU_CLEAR), IDM_EDIT_CLEAR)
        .AddSeparator()
        .Add(ResourceString(IDS_MENU_COPYALL), IDM_EDIT_COPYALL)
        .AddSeparator()
        .Add(ResourceString(IDS_MENU_NEW), IDM_EDIT_EXECUTE)
        ;
    m_menuView
        .Enable(IDM_VIEW_SFID, MF_DISABLED);
    SetTimer(hwnd, SFID_TIMER100MS, 100, NULL);
}


INT_PTR SfidDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    UINT idChild = LOWORD(wParam);
    UINT idNotif = HIWORD(wParam);
    switch (idChild)
    {
    case IDC_SFID_COPY_BUTTON:
        OnCopy();
        break;
    case IDC_SFID_NEW_BUTTON:
        NewContent();
        break;
    case IDC_SFID_UP_BUTTON:
        ChangeContent(1);
        break;
    case IDC_SFID_DOWN_BUTTON:
        ChangeContent(-1);
        break;
    case IDC_SFID_EDIT:
    case IDC_SFID_KEYPREFIX_EDIT:
    case IDC_SFID_INSTANCE_EDIT:
    case IDC_SFID_UNIQUEID_EDIT:
        if (idNotif == EN_CHANGE)
        {
            if (!m_LastModified)
            {
                m_State = STATE_TYPING;
                SetText(IDC_SFID_STATUS_STATIC, L"Changing...");
            }
            m_LastModified.By = idChild;
        }
        else if (idNotif == EN_SETFOCUS)
        {
            m_ActiveEdit = idChild;
        }
        else if (idNotif == EN_KILLFOCUS)
        {
            m_ActiveEdit = 0;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


INT_PTR SfidDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case SFID_TIMER100MS:
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


INT_PTR SfidDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    switch (id)
    {
    case IDC_SFID_STATUS_STATIC:
        SetTextColor(hdc, m_State == STATE_TYPING ? RGB(200, 200, 0)
            : m_State == STATE_SUCCESSFUL ? RGB(51, 102, 0)
            : m_State == STATE_ERROR ? RGB(153, 0, 0)
            : RGB(0, 0, 0));
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    default:
        break;
    }
    return 0;
}


void SfidDialogBox::OnCut()
{
    if (m_ActiveEdit)
    {
        EditCut(m_ActiveEdit);
    }
}


void SfidDialogBox::OnCopy()
{
    if (m_ActiveEdit)
    {
        EditCopy(m_ActiveEdit);
    }
}


void SfidDialogBox::OnPaste()
{
    if (m_ActiveEdit)
    {
        EditPaste(m_ActiveEdit);
    }
}


void SfidDialogBox::OnDelete()
{
    if (m_ActiveEdit)
    {
        EditDelete(m_ActiveEdit);
    }
}


void SfidDialogBox::OnSelectAll()
{
    if (m_ActiveEdit)
    {
        EditSelectAll(m_ActiveEdit);
    }
}


void SfidDialogBox::OnCopyAll()
{
    if (m_LastModified)
    {
        ApplyModification();
    }
    if (!Clipboard::Write(hwnd, GetText(IDC_SFID_EDIT)))
    {
        MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
}


void SfidDialogBox::OnClear()
{
    if (m_ActiveEdit)
    {
        EditClear(m_ActiveEdit);
    }
}


void SfidDialogBox::OnExecute()
{
    NewContent();
}


void SfidDialogBox::NewContent()
{
    LARGE_INTEGER value = { 0 };
    srand(static_cast<unsigned int>(FileTime().Intervals));
    value.LowPart = rand();
    value.HighPart = rand();
    SetText(IDC_SFID_UNIQUEID_EDIT, Base62Encode(value.QuadPart, UNIQUEID_LENGTH));
    ApplyModification();
}


void SfidDialogBox::ChangeContent(LONGLONG delta)
{
    String szUniqueId = GetText(IDC_SFID_UNIQUEID_EDIT);
    if (szUniqueId.Len > UNIQUEID_LENGTH)
    {
        m_State = STATE_ERROR;
        SetText(IDC_SFID_STATUS_STATIC, String(PRINTF, L"%s: Too long", UNIQUEID_NAME));
        return;
    }
    LONGLONG value = ComputeSerialNumber(szUniqueId);
    if (value < 0)
    {
        m_State = STATE_ERROR;
        SetText(IDC_SFID_STATUS_STATIC, String(PRINTF, L"%s: Invalid", UNIQUEID_NAME));
        return;
    }
    LONGLONG max = ComputeSerialNumber(L"zzzzzzzzz");
    value += delta;
    if (value < 0)
    {
        value += max + 1;
    }
    else if (value > max)
    {
        value -= max + 1;
    }
    SetText(IDC_SFID_UNIQUEID_EDIT, Base62Encode(value, UNIQUEID_LENGTH));
    ApplyModification();
}


static const WCHAR szZeros[] = { L'0', L'0', L'0', L'0', L'0', L'0', L'0', L'0', L'0' };


void SfidDialogBox::ApplyModification()
{
    m_State = STATE_ERROR;
    SetText(IDC_SFID_STATUS_STATIC);
    switch (m_LastModified.By)
    {
    case IDC_SFID_EDIT:
    {
        String sz = GetText(IDC_SFID_EDIT).Trim();
        String szKeyPrefix(&sz[KEYPREFIX_OFFSET], KEYPREFIX_LENGTH);
        SetText(IDC_SFID_KEYPREFIX_EDIT, szKeyPrefix);
        SetText(IDC_SFID_INSTANCE_EDIT);
        SetText(IDC_SFID_UNIQUEID_EDIT);
        SetText(IDC_SFID_CHECKSUM_EDIT);
        if (sz.Len == 0)
        {
            break;
        }
        else if (!ParseBase62(szKeyPrefix, KEYPREFIX_LENGTH, KEYPREFIX_NAME))
        {
            break;
        }
        else if (sz.Len == KEYPREFIX_OFFSET + KEYPREFIX_LENGTH)
        {
            break;
        }
        String szInstance(&sz[INSTANCE_OFFSET], INSTANCE_LENGTH);
        SetText(IDC_SFID_INSTANCE_EDIT, szInstance);
        if (!ParseBase62(szInstance, INSTANCE_LENGTH, INSTANCE_NAME))
        {
            break;
        }
        else if (sz.Len == INSTANCE_OFFSET + INSTANCE_LENGTH)
        {
            m_State = STATE_SUCCESSFUL;
            break;
        }
        String szReserved(&sz[RESERVED_OFFSET], RESERVED_LENGTH);
        if (szReserved != String(RESERVED_VALUE))
        {
            SetText(IDC_SFID_STATUS_STATIC, String(PRINTF, L"%s: Is not \"%s\"", RESERVED_NAME, RESERVED_VALUE));
            break;
        }
        else if (sz.Len == RESERVED_OFFSET + RESERVED_LENGTH)
        {
            m_State = STATE_SUCCESSFUL;
            break;
        }
        String szUniqueId(&sz[UNIQUEID_OFFSET], UNIQUEID_LENGTH);
        SetText(IDC_SFID_UNIQUEID_EDIT, szUniqueId);
        if (!ParseBase62(szUniqueId, UNIQUEID_LENGTH, UNIQUEID_NAME))
        {
            break;
        }
        String szExpected = ComputeChecksum(String(sz, CHECKSUM_OFFSET));
        SetText(IDC_SFID_CHECKSUM_EDIT, szExpected);
        if (sz.Len == CHECKSUM_OFFSET)
        {
            m_State = STATE_SUCCESSFUL;
            SetText(IDC_SFID_EDIT, String(PRINTF, L"%s%s%s%s%s", szKeyPrefix, szInstance, RESERVED_VALUE, szUniqueId, szExpected));
            SetText(IDC_SFID_STATUS_STATIC, L"OK (Checksum computed)");
            break;
        }
        String szChecksum(&sz[CHECKSUM_OFFSET]);
        if (szChecksum == szExpected)
        {
            m_State = STATE_SUCCESSFUL;
            SetText(IDC_SFID_STATUS_STATIC, L"OK");
        }
        else
        {
            SetText(IDC_SFID_STATUS_STATIC, String(PRINTF, L"%s: Incorrect", CHECKSUM_NAME));
        }
        break;
    }
    case IDC_SFID_KEYPREFIX_EDIT:
    case IDC_SFID_INSTANCE_EDIT:
    case IDC_SFID_UNIQUEID_EDIT:
    {
        String szKeyPrefix = GetText(IDC_SFID_KEYPREFIX_EDIT);
        String szInstance = GetText(IDC_SFID_INSTANCE_EDIT);
        String szUniqueId = GetText(IDC_SFID_UNIQUEID_EDIT);
        String sz(PRINTF, L"%s%s%s%s", szKeyPrefix, szInstance, RESERVED_VALUE, szUniqueId);
        SetText(IDC_SFID_EDIT, sz);
        if (m_LastModified.By == IDC_SFID_KEYPREFIX_EDIT)
        {
            if (!ParseBase62(szKeyPrefix, KEYPREFIX_LENGTH, KEYPREFIX_NAME)
                || !ParseBase62(szInstance, INSTANCE_LENGTH, INSTANCE_NAME)
                || !ParseBase62(szUniqueId, UNIQUEID_LENGTH, UNIQUEID_NAME))
            {
                break;
            }
        }
        else if (m_LastModified.By == IDC_SFID_INSTANCE_EDIT)
        {
            if (!ParseBase62(szInstance, INSTANCE_LENGTH, INSTANCE_NAME)
                || !ParseBase62(szKeyPrefix, KEYPREFIX_LENGTH, KEYPREFIX_NAME)
                || !ParseBase62(szUniqueId, UNIQUEID_LENGTH, UNIQUEID_NAME))
            {
                break;
            }
        }
        else
        {
            if (!ParseBase62(szUniqueId, UNIQUEID_LENGTH, UNIQUEID_NAME)
                || !ParseBase62(szKeyPrefix, KEYPREFIX_LENGTH, KEYPREFIX_NAME)
                || !ParseBase62(szInstance, INSTANCE_LENGTH, INSTANCE_NAME))
            {
                break;
            }
        }
        if (szKeyPrefix.Len < KEYPREFIX_LENGTH)
        {
            szKeyPrefix = String(szZeros, KEYPREFIX_LENGTH - szKeyPrefix.Len) + szKeyPrefix;
            SetText(IDC_SFID_KEYPREFIX_EDIT, szKeyPrefix);
        }
        if (szInstance.Len < INSTANCE_LENGTH)
        {
            szInstance = String(szZeros, INSTANCE_LENGTH - szInstance.Len) + szInstance;
            SetText(IDC_SFID_INSTANCE_EDIT, szInstance);
        }
        if (szUniqueId.Len < UNIQUEID_LENGTH)
        {
            szUniqueId = String(szZeros, UNIQUEID_LENGTH - szUniqueId.Len) + szUniqueId;
            SetText(IDC_SFID_UNIQUEID_EDIT, szUniqueId);
        }
        if (sz.Len < CHECKSUM_OFFSET)
        {
            sz = String(PRINTF, L"%s%s%s%s", szKeyPrefix, szInstance, RESERVED_VALUE, szUniqueId);
            SetText(IDC_SFID_EDIT, sz);
        }
        String szChecksum = ComputeChecksum(sz);
        SetText(IDC_SFID_CHECKSUM_EDIT, szChecksum);
        SetText(IDC_SFID_EDIT, sz + szChecksum);
        m_State = STATE_SUCCESSFUL;
        SetText(IDC_SFID_STATUS_STATIC, L"OK");
        break;
    }
    default:
        break;
    }
    m_LastModified.By = 0;
}


static const WCHAR SuffixTable[32] =
{
    L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P',
    L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'0', L'1', L'2', L'3', L'4', L'5'
};


String SfidDialogBox::ComputeChecksum(String sz) const
{
    int a1 = 0;
    int a2 = 0;
    int a3 = 0;
    for (int index = 0; index < 5; index++)
    {
        WCHAR c1 = sz[1 * 5 - 1 - index];
        WCHAR c2 = sz[2 * 5 - 1 - index];
        WCHAR c3 = sz[3 * 5 - 1 - index];
        int b1 = (static_cast<unsigned>(c1 - L'A') <= static_cast<unsigned>(L'Z' - L'A')) ? 1 : 0;
        int b2 = (static_cast<unsigned>(c2 - L'A') <= static_cast<unsigned>(L'Z' - L'A')) ? 1 : 0;
        int b3 = (static_cast<unsigned>(c3 - L'A') <= static_cast<unsigned>(L'Z' - L'A')) ? 1 : 0;
        a1 = (a1 << 1) | b1;
        a2 = (a2 << 1) | b2;
        a3 = (a3 << 1) | b3;
    }
    return String(PRINTF, L"%c%c%c", SuffixTable[a1], SuffixTable[a2], SuffixTable[a3]);
}


static const WCHAR Base62EncodingTable[62] =
{
    L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F',
    L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P', L'Q', L'R', L'S', L'T', L'U', L'V',
    L'W', L'X', L'Y', L'Z', L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l',
    L'm', L'n', L'o', L'p', L'q', L'r', L's', L't', L'u', L'v', L'w', L'x', L'y', L'z'
};


String SfidDialogBox::Base62Encode(ULONGLONG value, int length) const
{
    switch (length)
    {
    case KEYPREFIX_LENGTH:
    case INSTANCE_LENGTH:
    case UNIQUEID_LENGTH:
        break;
    default:
        throw Exception(L"SfidDialogBox::Base62Encode: Bad length.");
    }
    WCHAR buf[10] = { L'0', L'0', L'0', L'0', L'0', L'0', L'0', L'0', L'0', L'\0' };
    int index = length;
    buf[index] = L'\0';
    do
    {
        buf[--index] = Base62EncodingTable[value % 62];
        value /= 62;
    } while (index > 0 && value > 0ULL);
    return String(buf);
}


static const int Base62DecodingTable[128] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
    -1, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1,
};


LONGLONG SfidDialogBox::ComputeSerialNumber(String sz) const
{
    ULONGLONG value = 0;
    int length = static_cast<int>(sz.Len);
    for (int index = 0; index < length; index++)
    {
        WCHAR c = sz[index];
        int d = c < 128 ? Base62DecodingTable[c] : -1;
        if (d < 0)
        {
            return -1LL;
        }
        value = value * 62 + d;
    }
    return value;
}


bool SfidDialogBox::ParseBase62(PCWSTR psz, int length, PCWSTR pszName) const
{
    for (int index = 0; index < length; index++)
    {
        WCHAR c = psz[index];
        if (!psz[index])
        {
            return true;
        }
        int d = c < 128 ? Base62DecodingTable[c] : -1;
        if (d < 0)
        {
            SetText(IDC_SFID_STATUS_STATIC, String(PRINTF, L"%s: [%d] is invalid", pszName, index));
            return false;
        }
    }
    if (!psz[length])
    {
        return true;
    }
    else
    {
        SetText(IDC_SFID_STATUS_STATIC, String(PRINTF, L"%s: Too long", pszName));
        return false;
    }
}
