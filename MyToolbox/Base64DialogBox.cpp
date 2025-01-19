#include "pch.h"
#include "Base64DialogBox.h"
#include "MyToolbox.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/FileMapper.h"
#include "hnrt/FileWriter.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Debug.h"
#include "resource.h"


#define REGVAL_FORMAT L"Format"
#define REGVAL_CODEPAGE L"CodePage"
#define REGVAL_LINEBREAK L"LineBreak"
#define REGVAL_ORGPATH L"OriginalPath"
#define REGVAL_ENCPATH L"EncodedPath"
#define REGVAL_CHARSPERLINE L"CharsPerLine"


#define FLAG_ENCODING_SUCCESSFUL (1UL<<0)
#define FLAG_ENCODING_ERROR (1UL<<1)
#define MASK_ENCODING (FLAG_ENCODING_SUCCESSFUL|FLAG_ENCODING_ERROR)
#define FLAG_DECODING_SUCCESSFUL (1UL<<2)
#define FLAG_DECODING_ERROR (1UL<<3)
#define MASK_DECODING (FLAG_DECODING_SUCCESSFUL|FLAG_DECODING_ERROR)
#define FLAG_BUSY (1UL<<4)


using namespace hnrt;


static String BreakLines(String sz, int charsPerLine)
{
    return sz.Replace(L"\r\n", L"").Replace(L"\n", L"").Wrap(charsPerLine);
}


Base64DialogBox::Base64DialogBox()
    : MyDialogBox(IDD_BS64, L"BASE64")
    , m_dwFlags(0)
    , m_Original()
    , m_szOriginalPath()
    , m_szEncodedPath()
{
}


void Base64DialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    HFONT hFont = GetApp<MyToolbox>().GetFontForData();
    SetFont(IDC_BS64_ORG_EDIT, hFont);
    SetFont(IDC_BS64_ENC_EDIT, hFont);
    InitializeCodePageComboBox(IDC_BS64_ORG_CODEPAGE_COMBO);
    InitializeLineBreakComboBox(IDC_BS64_ORG_LINEBREAK_COMBO);
    InitializeLineLengthComboBox();
    ButtonCheck(IDC_BS64_ORG_HEX_RADIO);
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
        String szFormat = RegistryValue::GetSZ(hKey, REGVAL_FORMAT, String(L"Hex"));
        if (!StrCmp(szFormat, -1, L"Text", -1))
        {
            ButtonCheck(IDC_BS64_ORG_HEX_RADIO, FALSE);
            ButtonCheck(IDC_BS64_ORG_TEXT_RADIO, TRUE);
        }
        else
        {
            ButtonCheck(IDC_BS64_ORG_HEX_RADIO, TRUE);
            ButtonCheck(IDC_BS64_ORG_TEXT_RADIO, FALSE);
        }
        ComboBoxSetSelection(IDC_BS64_ORG_CODEPAGE_COMBO, RegistryValue::GetDWORD(hKey, REGVAL_CODEPAGE, CP_UTF8));
        ComboBoxSetSelection(IDC_BS64_ORG_LINEBREAK_COMBO, RegistryValue::GetDWORD(hKey, REGVAL_LINEBREAK, 0x0d0a));
        StrCpy(m_szOriginalPath, RegistryValue::GetSZ(hKey, REGVAL_ORGPATH));
        StrCpy(m_szEncodedPath, RegistryValue::GetSZ(hKey, REGVAL_ENCPATH));
        ComboBoxSetSelection(IDC_BS64_ENC_LINELENGTH_COMBO, RegistryValue::GetDWORD(hKey, REGVAL_CHARSPERLINE));
    }
    m_menuView
        .Add(ResourceString(IDS_MENU_BS64), IDM_VIEW_BS64);
}


void Base64DialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_WRITE);
    if (rc == ERROR_SUCCESS)
    {
        if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
        {
            RegistryValue::SetSZ(hKey, REGVAL_FORMAT, L"Hex");
        }
        else
        {
            RegistryValue::SetSZ(hKey, REGVAL_FORMAT, L"Text");
        }
        RegistryValue::SetDWORD(hKey, REGVAL_CODEPAGE, ComboBoxGetSelection(IDC_BS64_ORG_CODEPAGE_COMBO));
        RegistryValue::SetDWORD(hKey, REGVAL_LINEBREAK, ComboBoxGetSelection(IDC_BS64_ORG_LINEBREAK_COMBO));
        RegistryValue::SetSZ(hKey, REGVAL_ORGPATH, m_szOriginalPath);
        RegistryValue::SetSZ(hKey, REGVAL_ENCPATH, m_szEncodedPath);
        RegistryValue::SetDWORD(hKey, REGVAL_CHARSPERLINE, ComboBoxGetSelection(IDC_BS64_ENC_LINELENGTH_COMBO));
    }
    SetFont(IDC_BS64_ORG_EDIT, NULL);
    SetFont(IDC_BS64_ENC_EDIT, NULL);
    MyDialogBox::OnDestroy();
}


void Base64DialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayoutSnapshot before, after;

    before.AddAllChildren(hDlg).Clone(after);

    after[IDC_BS64_ORG_STATIC].right += cxDelta;
    after[IDC_BS64_ORG_EDIT].right += cxDelta;

    MoveHorizontally(after[IDC_BS64_ORG_COPY_BUTTON], cxDelta);
    MoveHorizontally(after[IDC_BS64_ORG_ENCODE_BUTTON], cxDelta);
    MoveHorizontally(after[IDC_BS64_ORG_HEX_RADIO], cxDelta);
    MoveHorizontally(after[IDC_BS64_ORG_TEXT_RADIO], cxDelta);
    MoveHorizontally(after[IDC_BS64_ORG_CODEPAGE_COMBO], cxDelta);
    MoveHorizontally(after[IDC_BS64_ORG_LINEBREAK_COMBO], cxDelta);

    after[IDC_BS64_ENC_STATIC].right += cxDelta;
    after[IDC_BS64_ENC_EDIT].right += cxDelta;

    MoveHorizontally(after[IDC_BS64_ENC_COPY_BUTTON], cxDelta);
    MoveHorizontally(after[IDC_BS64_ENC_DECODE_BUTTON], cxDelta);
    MoveHorizontally(after[IDC_BS64_ENC_LINELENGTH_STATIC], cxDelta);
    MoveHorizontally(after[IDC_BS64_ENC_LINELENGTH_COMBO], cxDelta);

    ExtendVertically(after[IDC_BS64_ORG_EDIT], after[IDC_BS64_ENC_EDIT], cyDelta);
    LONG dy = after[IDC_BS64_ORG_EDIT].bottom - before[IDC_BS64_ORG_EDIT].bottom;
    MoveVertically(after[IDC_BS64_ENC_STATIC], dy);
    MoveVertically(after[IDC_BS64_ENC_COPY_BUTTON], dy);
    MoveVertically(after[IDC_BS64_ENC_DECODE_BUTTON], dy);
    MoveVertically(after[IDC_BS64_ENC_LINELENGTH_STATIC], dy);
    MoveVertically(after[IDC_BS64_ENC_LINELENGTH_COMBO], dy);

    after.Apply();
}


void Base64DialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
    if (m_LastModified)
    {
    }
    m_menuView
        .Enable(IDM_VIEW_BS64, MF_ENABLED);
}


void Base64DialogBox::OnTabSelectionChanged()
{
    MyDialogBox::OnTabSelectionChanged();
    m_menuFile
        .Insert(0, ResourceString(IDS_MENU_NEW), IDM_FILE_NEW)
        .InsertSeparator(1)
        .Insert(2, ResourceString(IDS_MENU_ORG_LOADFROM), IDM_FILE_LOAD1FROM)
        .Insert(3, ResourceString(IDS_MENU_ORG_SAVEAS), IDM_FILE_SAVE1AS)
        .InsertSeparator(4)
        .Insert(5, ResourceString(IDS_MENU_ENC_LOADFROM), IDM_FILE_LOAD2FROM)
        .Insert(6, ResourceString(IDS_MENU_ENC_SAVEAS), IDM_FILE_SAVE2AS)
        .InsertSeparator(7);
    m_menuEdit
        .Insert(0, ResourceString(IDS_MENU_ENCODE), IDM_EDIT_EXECUTE1)
        .Insert(1, ResourceString(IDS_MENU_COPYORIGINAL), IDM_EDIT_COPYRESULT1)
        .InsertSeparator(2)
        .Insert(3, ResourceString(IDS_MENU_DECODE), IDM_EDIT_EXECUTE2)
        .Insert(4, ResourceString(IDS_MENU_COPYENCODED), IDM_EDIT_COPYRESULT2)
        .InsertSeparator(5);
    m_menuView
        .Enable(IDM_VIEW_BS64, MF_DISABLED);
    UpdateControlsState(0);
}


INT_PTR Base64DialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (m_cProcessing)
    {
        return TRUE;
    }
    UINT idChild = LOWORD(wParam);
    UINT idNotif = HIWORD(wParam);
    switch (idChild)
    {
    case IDM_EDIT_EXECUTE1:
        Encode();
        SetFocus(IDC_BS64_ENC_COPY_BUTTON);
        break;
    case IDM_EDIT_COPYRESULT1:
        CopyAllText(IDC_BS64_ORG_EDIT);
        break;
    case IDM_EDIT_EXECUTE2:
        Decode();
        SetFocus(IDC_BS64_ORG_COPY_BUTTON);
        break;
    case IDM_EDIT_COPYRESULT2:
        CopyAllText(IDC_BS64_ENC_EDIT);
        break;
    case IDC_BS64_ORG_ENCODE_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            Encode();
            SetFocus(IDC_BS64_ENC_COPY_BUTTON);
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_BS64_ORG_COPY_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            CopyAllText(IDC_BS64_ORG_EDIT);
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_BS64_ENC_DECODE_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            Decode();
            SetFocus(IDC_BS64_ORG_COPY_BUTTON);
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_BS64_ENC_COPY_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            CopyAllText(IDC_BS64_ENC_EDIT);
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_BS64_ORG_EDIT:
    case IDC_BS64_ENC_EDIT:
        return OnEditCommand(wParam, lParam);
    case IDC_BS64_ORG_HEX_RADIO:
    case IDC_BS64_ORG_TEXT_RADIO:
        if (idNotif == BN_CLICKED)
        {
            ChangeOriginalFormat();
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_BS64_ORG_CODEPAGE_COMBO:
        if (idNotif == CBN_SELCHANGE)
        {
            if (m_Original.Len && ButtonIsChecked(IDC_BS64_ORG_TEXT_RADIO))
            {
                ApplyOriginal();
            }
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_BS64_ENC_LINELENGTH_COMBO:
        if (idNotif == CBN_SELCHANGE)
        {
            ChangeLinesPerLine();
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


INT_PTR Base64DialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    switch (id)
    {
    case IDC_BS64_ORG_STATIC:
        SetTextColor(hdc,
            (m_dwFlags & FLAG_ENCODING_ERROR) ? RGB_ERROR :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    case IDC_BS64_ENC_STATIC:
        SetTextColor(hdc,
            (m_dwFlags & FLAG_DECODING_ERROR) ? RGB_ERROR :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    default:
        break;
    }
    return 0;
}


INT_PTR Base64DialogBox::OnControlColorEdit(WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    int len = GetTextLength(id);
    switch (id)
    {
    case IDC_BS64_ORG_EDIT:
        SetTextColor(hdc,
            (m_dwFlags & FLAG_ENCODING_ERROR) ? RGB_ERROR :
            m_Original.Len ? RGB_SUCCESSFUL :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    case IDC_BS64_ENC_EDIT:
        SetTextColor(hdc,
            (m_dwFlags & FLAG_DECODING_ERROR) ? RGB_ERROR :
            (m_dwFlags & FLAG_ENCODING_SUCCESSFUL) ? RGB_SUCCESSFUL :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    default:
        break;
    }
    return 0;
}


void Base64DialogBox::OnNew()
{
    SetTextAndNotify(IDC_BS64_ORG_EDIT);
    SetTextAndNotify(IDC_BS64_ENC_EDIT);
}


void Base64DialogBox::OnEditChanged(int id)
{
    MyDialogBox::OnEditChanged(id);
    switch (id)
    {
    case IDC_BS64_ORG_EDIT:
        try
        {
            if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
            {
                m_Original = ByteString::FromHex(GetText(IDC_BS64_ORG_EDIT));
            }
            else
            {
                UINT cp = static_cast<UINT>(ComboBoxGetSelection(IDC_BS64_ORG_CODEPAGE_COMBO));
                LineBreak lb = static_cast<LineBreak>(ComboBoxGetSelection(IDC_BS64_ORG_LINEBREAK_COMBO));
                m_Original = ByteString::FromString(GetText(IDC_BS64_ORG_EDIT), cp, lb);
            }
            if ((m_dwFlags & FLAG_ENCODING_ERROR))
            {
                m_dwFlags &= ~MASK_ENCODING;
                SetText(IDC_BS64_ORG_STATIC, ResourceString(IDS_ORIGINAL));
                InvalidateRect(IDC_BS64_ORG_STATIC, NULL, TRUE);
            }
            InvalidateRect(IDC_BS64_ORG_EDIT, NULL, TRUE);
        }
        catch (Exception e)
        {
            m_Original.Resize(0);
            SetOriginalError(e.Message);
        }
        UpdateControlsState(IDC_BS64_ORG_EDIT);
        break;
    case IDC_BS64_ENC_EDIT:
        DBGPUT(L"Base64DialogBox::OnEditChanged(IDC_BS64_ENC_EDIT): Len=%u", GetTextLength(IDC_BS64_ENC_EDIT));
        if ((m_dwFlags & FLAG_ENCODING_SUCCESSFUL))
        {
            m_dwFlags &= ~MASK_ENCODING;
            InvalidateRect(IDC_BS64_ENC_EDIT, NULL, TRUE);
        }
        if ((m_dwFlags & FLAG_DECODING_ERROR))
        {
            m_dwFlags &= ~MASK_DECODING;
            SetText(IDC_BS64_ENC_STATIC, ResourceString(IDS_ENCODED));
            InvalidateRect(IDC_BS64_ENC_STATIC, NULL, TRUE);
            InvalidateRect(IDC_BS64_ENC_EDIT, NULL, TRUE);
        }
        UpdateControlsState(IDC_BS64_ENC_EDIT);
        break;
    default:
        return;
    }
}


void Base64DialogBox::OnLoad1From()
{
    if (GetLoadFromFileName(m_szOriginalPath, ResourceString(IDS_TITLE_LOAD_ORIGINAL)))
    {
        UpdateControlsState(-1);
        try
        {
            FileMapper fm(m_szOriginalPath);
            m_Original = ByteString(fm.Ptr, fm.Len);
            ApplyOriginal();
        }
        catch (Exception e)
        {
            MessageBoxW(hwnd, e.Message, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
    }
}


void Base64DialogBox::OnSave1As()
{
    if (GetSaveAsFileName(m_szOriginalPath, ResourceString(IDS_TITLE_SAVE_ORIGINAL)))
    {
        UpdateControlsState(-1);
        try
        {
            String sz = GetText(IDC_BS64_ORG_EDIT);
            ByteString bs;
            if (sz.Len)
            {
                if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
                {
                    bs = ByteString::FromHex(sz);
                }
                else
                {
                    UINT cp = static_cast<UINT>(ComboBoxGetSelection(IDC_BS64_ORG_CODEPAGE_COMBO));
                    LineBreak lb = static_cast<LineBreak>(ComboBoxGetSelection(IDC_BS64_ORG_LINEBREAK_COMBO));
                    bs = ByteString::FromString(sz, cp, lb);
                }
            }
            else
            {
                bs.Resize(0);
            }
            FileWriter(m_szOriginalPath).Write(bs.Ptr, bs.Len);
        }
        catch (Exception e)
        {
            SetOriginal();
            MessageBoxW(hwnd, e.Message, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
        UpdateControlsState(0);
    }
}


void Base64DialogBox::OnLoad2From()
{
    if (GetLoadFromFileName(m_szEncodedPath, ResourceString(IDS_TITLE_LOAD_ENCODED)))
    {
        UpdateControlsState(-1);
        try
        {
            FileMapper fm(m_szEncodedPath);
            SetEncoded(String(CP_UTF8, reinterpret_cast<PCSTR>(fm.Ptr), fm.Len), FALSE);
        }
        catch (Exception e)
        {
            SetEncoded();
            MessageBoxW(hwnd, e.Message, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
    }
}


void Base64DialogBox::OnSave2As()
{
    if (GetSaveAsFileName(m_szEncodedPath, ResourceString(IDS_TITLE_SAVE_ENCODED)))
    {
        UpdateControlsState(-1);
        try
        {
            ByteString encoded = ByteString::FromString(GetText(IDC_BS64_ENC_EDIT), CP_UTF8);
            FileWriter(m_szEncodedPath).Write(encoded.Ptr, encoded.Len);
        }
        catch (Exception e)
        {
            MessageBoxW(hwnd, e.Message, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
        UpdateControlsState(0);
    }
}


void Base64DialogBox::Encode()
{
    UpdateControlsState(-1);
    try
    {
        SetEncoded(BreakLines(m_Original.ToBase64(), ComboBoxGetSelection(IDC_BS64_ENC_LINELENGTH_COMBO)), TRUE);
    }
    catch (Exception e)
    {
        SetOriginalError(e.Message);
    }
}


void Base64DialogBox::Decode()
{
    UpdateControlsState(-1);
    try
    {
        m_Original = ByteString::FromBase64(GetText(IDC_BS64_ENC_EDIT));
        SetEncoded(NULL);
        ApplyOriginal();
    }
    catch (Exception e)
    {
        SetEncodedError(e.Message);
    }
}


void Base64DialogBox::ChangeOriginalFormat()
{
    UpdateControlsState(-1);
    ApplyOriginal();
}


void Base64DialogBox::ChangeLinesPerLine()
{
    UpdateControlsState(-1);
    SetTextAndNotify(IDC_BS64_ENC_EDIT, BreakLines(GetText(IDC_BS64_ENC_EDIT), ComboBoxGetSelection(IDC_BS64_ENC_LINELENGTH_COMBO)));
}


void Base64DialogBox::ApplyOriginal()
{
    if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
    {
        SetOriginal(m_Original.ToHex(StringOptions::UPPERCASE));
    }
    else
    {
        try
        {
            UINT cp = static_cast<UINT>(ComboBoxGetSelection(IDC_BS64_ORG_CODEPAGE_COMBO));
            SetOriginal(m_Original.ToString(cp));
        }
        catch (Exception e)
        {
            SetOriginalError(e.Message);
        }
    }
}


void Base64DialogBox::SetOriginal(PCWSTR pszContent)
{
    m_dwFlags &= ~FLAG_ENCODING_ERROR;
    SetText(IDC_BS64_ORG_STATIC, ResourceString(IDS_ORIGINAL));
    if (pszContent)
    {
        SetText(IDC_BS64_ORG_EDIT, pszContent);
        UpdateControlsState(IDC_BS64_ORG_EDIT);
    }
    else
    {
        InvalidateRect(IDC_BS64_ORG_EDIT, NULL, TRUE);
    }
}


void Base64DialogBox::SetOriginalError(PCWSTR pszReason)
{
    m_dwFlags |= FLAG_ENCODING_ERROR;
    SetText(IDC_BS64_ORG_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ORIGINAL), pszReason));
    InvalidateRect(IDC_BS64_ORG_EDIT, NULL, TRUE);
    UpdateControlsState(IDC_BS64_ORG_EDIT);
}


void Base64DialogBox::SetEncoded(PCWSTR pszContent, BOOL bEncoded)
{
    m_dwFlags &= ~FLAG_DECODING_ERROR;
    m_dwFlags = bEncoded ? (m_dwFlags | FLAG_ENCODING_SUCCESSFUL) : (m_dwFlags & ~FLAG_ENCODING_SUCCESSFUL);
    SetText(IDC_BS64_ENC_STATIC, ResourceString(IDS_ENCODED));
    if (pszContent)
    {
        SetTextAndNotify(IDC_BS64_ENC_EDIT, pszContent);
    }
    else
    {
        InvalidateRect(IDC_BS64_ENC_EDIT, NULL, TRUE);
    }
}


void Base64DialogBox::SetEncodedError(PCWSTR pszReason)
{
    m_dwFlags |= FLAG_DECODING_ERROR;
    m_dwFlags &= ~FLAG_ENCODING_SUCCESSFUL;
    SetText(IDC_BS64_ENC_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ENCODED), pszReason));
    InvalidateRect(IDC_BS64_ENC_EDIT, NULL, TRUE);
    UpdateControlsState(IDC_BS64_ENC_EDIT);
}


void Base64DialogBox::UpdateControlsState(int id)
{
    switch (id)
    {
    case IDC_BS64_ORG_EDIT:
    {
        BOOL bEnabled = GetTextLength(IDC_BS64_ORG_EDIT) > 0;
        EnableWindow(IDC_BS64_ORG_COPY_BUTTON, bEnabled);
        EnableWindow(IDC_BS64_ORG_ENCODE_BUTTON, bEnabled);
        EnableWindow(IDC_BS64_ORG_CODEPAGE_COMBO, bEnabled || ButtonIsChecked(IDC_BS64_ORG_TEXT_RADIO));
        EnableWindow(IDC_BS64_ORG_LINEBREAK_COMBO, bEnabled || ButtonIsChecked(IDC_BS64_ORG_TEXT_RADIO));
        m_menuEdit
            .Enable(IDM_EDIT_EXECUTE1, bEnabled ? MF_ENABLED : MF_DISABLED)
            .Enable(IDM_EDIT_COPYRESULT1, bEnabled ? MF_ENABLED : MF_DISABLED);
        if ((m_dwFlags & FLAG_BUSY))
        {
            m_dwFlags &= ~FLAG_BUSY;
            EditSetReadOnly(IDC_BS64_ORG_EDIT, FALSE);
            EditSetReadOnly(IDC_BS64_ENC_EDIT, FALSE);
            UpdateControlsState(IDC_BS64_ENC_EDIT);
        }
        break;
    }
    case IDC_BS64_ENC_EDIT:
    {
        BOOL bEnabled = GetTextLength(IDC_BS64_ENC_EDIT) > 0;
        EnableWindow(IDC_BS64_ENC_COPY_BUTTON, bEnabled);
        EnableWindow(IDC_BS64_ENC_DECODE_BUTTON, bEnabled);
        m_menuEdit
            .Enable(IDM_EDIT_EXECUTE2, bEnabled ? MF_ENABLED : MF_DISABLED)
            .Enable(IDM_EDIT_COPYRESULT2, bEnabled ? MF_ENABLED : MF_DISABLED);
        if ((m_dwFlags & FLAG_BUSY))
        {
            m_dwFlags &= ~FLAG_BUSY;
            EditSetReadOnly(IDC_BS64_ORG_EDIT, FALSE);
            EditSetReadOnly(IDC_BS64_ENC_EDIT, FALSE);
            UpdateControlsState(IDC_BS64_ORG_EDIT);
        }
        break;
    }
    case 0:
        UpdateControlsState(IDC_BS64_ORG_EDIT);
        UpdateControlsState(IDC_BS64_ENC_EDIT);
        break;
    case -1:
        m_dwFlags |= FLAG_BUSY;
        EditSetReadOnly(IDC_BS64_ORG_EDIT, TRUE);
        EditSetReadOnly(IDC_BS64_ENC_EDIT, TRUE);
        DisableWindow(IDC_BS64_ORG_COPY_BUTTON);
        DisableWindow(IDC_BS64_ORG_ENCODE_BUTTON);
        DisableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
        DisableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
        DisableWindow(IDC_BS64_ENC_COPY_BUTTON);
        DisableWindow(IDC_BS64_ENC_DECODE_BUTTON);
        m_menuEdit
            .Enable(IDM_EDIT_EXECUTE1, MF_DISABLED)
            .Enable(IDM_EDIT_COPYRESULT1, MF_DISABLED)
            .Enable(IDM_EDIT_EXECUTE2, MF_DISABLED)
            .Enable(IDM_EDIT_COPYRESULT2, MF_DISABLED);
        break;
    default:
        break;
    }
}


void Base64DialogBox::InitializeLineLengthComboBox() const
{
    ComboBoxAdd(IDC_BS64_ENC_LINELENGTH_COMBO, ResourceString(IDS_NO_LINE_BREAK), 0);
    ComboBoxAdd(IDC_BS64_ENC_LINELENGTH_COMBO, L"72", 72);
    ComboBoxAdd(IDC_BS64_ENC_LINELENGTH_COMBO, L"144", 144);
    ComboBoxAdd(IDC_BS64_ENC_LINELENGTH_COMBO, L"216", 216);
}


BOOL Base64DialogBox::GetLoadFromFileName(PWSTR psz, PCWSTR pszTitle) const
{
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrTitle = pszTitle;
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = psz;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    return GetOpenFileNameW(&ofn);
}


BOOL Base64DialogBox::GetSaveAsFileName(PWSTR psz, PCWSTR pszTitle) const
{
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrTitle = pszTitle;
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = psz;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    return GetSaveFileNameW(&ofn);
}
