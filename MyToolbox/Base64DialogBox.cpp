#include "pch.h"
#include "Base64DialogBox.h"
#include "MyToolbox.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/Base64.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/FileMapper.h"
#include "hnrt/FileWriter.h"
#include "hnrt/StringCommons.h"
#include "hnrt/StringUTF8.h"
#include "resource.h"


#define REGVAL_FORMAT L"Format"
#define REGVAL_CODEPAGE L"CodePage"
#define REGVAL_LINEBREAK L"LineBreak"
#define REGVAL_ORGPATH L"OriginalPath"
#define REGVAL_ENCPATH L"EncodedPath"
#define REGVAL_CHARSPERLINE L"CharsPerLine"


using namespace hnrt;


static String BreakLines(String sz, int charsPerLine)
{
    return sz.Replace(L"\r\n", L"").Replace(L"\n", L"").Wrap(charsPerLine);
}


Base64DialogBox::Base64DialogBox()
    : MyDialogBox(IDD_BS64, L"BASE64")
    , m_bEncodingError(FALSE)
    , m_bDecodingError(FALSE)
    , m_Original()
    , m_bEncoded(FALSE)
    , m_szOriginalPath()
    , m_szEncodedPath()
{
}


Base64DialogBox::~Base64DialogBox()
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
            OnCommand(IDC_BS64_ORG_TEXT_RADIO, 0);
        }
        else
        {
            ButtonCheck(IDC_BS64_ORG_HEX_RADIO, TRUE);
            ButtonCheck(IDC_BS64_ORG_TEXT_RADIO, FALSE);
            OnCommand(IDC_BS64_ORG_HEX_RADIO, 0);
        }
        ComboBoxSetSelection(IDC_BS64_ORG_CODEPAGE_COMBO, RegistryValue::GetDWORD(hKey, REGVAL_CODEPAGE, CP_UTF8));
        ComboBoxSetSelection(IDC_BS64_ORG_LINEBREAK_COMBO, RegistryValue::GetDWORD(hKey, REGVAL_LINEBREAK, 0x0d0a));
        StrCpy(m_szOriginalPath, RegistryValue::GetSZ(hKey, REGVAL_ORGPATH));
        StrCpy(m_szEncodedPath, RegistryValue::GetSZ(hKey, REGVAL_ENCPATH));
        ComboBoxSetSelection(IDC_BS64_ENC_LINELENGTH_COMBO, RegistryValue::GetDWORD(hKey, REGVAL_CHARSPERLINE));
    }
    OnCommand(IDC_BS64_ORG_EDIT | (EN_CHANGE << 16), 0);
    OnCommand(IDC_BS64_ENC_EDIT | (EN_CHANGE << 16), 0);
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
        .Insert(0, ResourceString(IDS_MENU_ORG_LOADFROM), IDM_FILE_LOAD1FROM)
        .Insert(1, ResourceString(IDS_MENU_ORG_SAVEAS), IDM_FILE_SAVE1AS)
        .InsertSeparator(2)
        .Insert(3, ResourceString(IDS_MENU_ENC_LOADFROM), IDM_FILE_LOAD2FROM)
        .Insert(4, ResourceString(IDS_MENU_ENC_SAVEAS), IDM_FILE_SAVE2AS)
        .InsertSeparator(5);
    m_menuView
        .Enable(IDM_VIEW_BS64, MF_DISABLED);
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
    case IDC_BS64_ORG_ENCODE_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            Encode();
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
    case IDC_BS64_ENC_DECODE_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            Decode();
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
            if (m_Original.Len)
            {
                ApplyOriginal();
            }
            else
            {
                m_bEncodingError = FALSE;
                SetText(IDC_BS64_ORG_STATIC, ResourceString(IDS_ORIGINAL));
                InvalidateRect(IDC_BS64_ORG_EDIT, NULL, TRUE);
            }
            UpdateControlsState();
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
            SetText(IDC_BS64_ENC_EDIT, BreakLines(GetText(IDC_BS64_ENC_EDIT), ComboBoxGetSelection(IDC_BS64_ENC_LINELENGTH_COMBO)));
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
            m_bEncodingError ? RGB_ERROR :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    case IDC_BS64_ENC_STATIC:
        SetTextColor(hdc,
            m_bDecodingError ? RGB_ERROR :
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
            m_bEncodingError ? RGB_ERROR :
            m_Original.Len ? RGB_SUCCESSFUL :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    case IDC_BS64_ENC_EDIT:
        SetTextColor(hdc,
            m_bDecodingError ? RGB_ERROR :
            m_bEncoded ? RGB_SUCCESSFUL :
            GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    default:
        break;
    }
    return 0;
}


void Base64DialogBox::OnEditChanged(int id)
{
    MyDialogBox::OnEditChanged(id);
    switch (id)
    {
    case IDC_BS64_ORG_EDIT:
        if (m_Original.Len)
        {
            m_Original.Resize(0);
            InvalidateRect(IDC_BS64_ORG_EDIT, NULL, TRUE);
            if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
            {
                DisableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
                DisableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
            }
        }
        if (m_bEncodingError)
        {
            m_bEncodingError = FALSE;
            SetText(IDC_BS64_ORG_STATIC, ResourceString(IDS_ORIGINAL));
            InvalidateRect(IDC_BS64_ORG_STATIC, NULL, TRUE);
            InvalidateRect(IDC_BS64_ORG_EDIT, NULL, TRUE);
        }
        break;
    case IDC_BS64_ENC_EDIT:
        if (m_bEncoded)
        {
            m_bEncoded = FALSE;
            InvalidateRect(IDC_BS64_ENC_EDIT, NULL, TRUE);
        }
        if (m_bDecodingError)
        {
            m_bDecodingError = FALSE;
            SetText(IDC_BS64_ENC_STATIC, ResourceString(IDS_ENCODED));
            InvalidateRect(IDC_BS64_ENC_STATIC, NULL, TRUE);
            InvalidateRect(IDC_BS64_ENC_EDIT, NULL, TRUE);
        }
        break;
    default:
        return;
    }
    UpdateControlsState();
}


void Base64DialogBox::OnLoad1From()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (GetLoadFromFileName(m_szOriginalPath, ResourceString(IDS_TITLE_LOAD_ORIGINAL)))
    {
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
        UpdateControlsState();
        m_LastModified.By = 0;
    }
}


void Base64DialogBox::OnSave1As()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    ByteString bs;
    if (ParseOriginal(bs))
    {
        if (GetSaveAsFileName(m_szOriginalPath, ResourceString(IDS_TITLE_SAVE_ORIGINAL)))
        {
            try
            {
                FileWriter(m_szOriginalPath).Write(bs.Ptr, bs.Len);
            }
            catch (Exception e)
            {
                MessageBoxW(hwnd, e.Message, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
            }
        }
    }
}


void Base64DialogBox::OnLoad2From()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (GetLoadFromFileName(m_szEncodedPath, ResourceString(IDS_TITLE_LOAD_ENCODED)))
    {
        try
        {
            FileMapper fm(m_szEncodedPath);
            SetEncoded(String(CP_UTF8, reinterpret_cast<PCSTR>(fm.Ptr), fm.Len), FALSE);
        }
        catch (Exception e)
        {
            MessageBoxW(hwnd, e.Message, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
        UpdateControlsState();
        m_LastModified.By = 0;
    }
}


void Base64DialogBox::OnSave2As()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (GetSaveAsFileName(m_szEncodedPath, ResourceString(IDS_TITLE_SAVE_ENCODED)))
    {
        try
        {
            StringUTF8 sz(GetText(IDC_BS64_ENC_EDIT));
            FileWriter(m_szEncodedPath).Write(sz.Ptr, sz.Len);
        }
        catch (Exception e)
        {
            MessageBoxW(hwnd, e.Message, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
    }
}


void Base64DialogBox::OnClear()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    m_Original.Resize(0);
    SetText(IDC_BS64_ORG_EDIT);
    SetText(IDC_BS64_ENC_EDIT);
    UpdateControlsState();
    m_LastModified.By = 0;
}


void Base64DialogBox::Encode()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    ByteString bs;
    if (ParseOriginal(bs))
    {
        Base64Encoder encoder;
        encoder.Append(bs.Ptr, bs.Len);
        encoder.End();
        SetEncoded(BreakLines(String(encoder), ComboBoxGetSelection(IDC_BS64_ENC_LINELENGTH_COMBO)), TRUE);
    }
    else
    {
        SetEncoded();
    }
    UpdateControlsState();
    m_LastModified.By = 0;
}


void Base64DialogBox::Decode()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    String szEncoded = GetText(IDC_BS64_ENC_EDIT);
    Base64Decoder decoder;
    if (decoder.Parse(szEncoded))
    {
        m_Original = decoder;
        SetEncoded(NULL);
        ApplyOriginal();
    }
    else
    {
        m_Original.Resize(0);
        SetEncodedError(L"Parse error");
        SetOriginal();
    }
    UpdateControlsState();
    m_LastModified.By = 0;
}


BOOL Base64DialogBox::ParseOriginal(ByteString& bs)
{
    String sz = GetText(IDC_BS64_ORG_EDIT);
    if (sz.Len)
    {
        try
        {
            if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
            {
                bs = ByteString::FromHex(sz);
            }
            else
            {
                if (ComboBoxGetSelection(IDC_BS64_ORG_LINEBREAK_COMBO) == 0x0a)
                {
                    sz = sz.Replace(L"\r\n", L"\n");
                }
                bs = ByteString::FromString(sz, ComboBoxGetSelection(IDC_BS64_ORG_CODEPAGE_COMBO));
            }
        }
        catch (Exception e)
        {
            SetOriginalError(L"Parse error");
            return FALSE;
        }
    }
    else
    {
        bs.Resize(0);
    }
    return TRUE;
}


void Base64DialogBox::ApplyOriginal()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (m_Original.Len)
    {
        if (ButtonIsChecked(IDC_BS64_ORG_HEX_RADIO))
        {
            SetOriginal(String::ToHex(m_Original.Ptr, m_Original.Len, UPPERCASE));
        }
        else
        {
            try
            {
                SetOriginal(m_Original.ToString(ComboBoxGetSelection(IDC_BS64_ORG_CODEPAGE_COMBO)));
            }
            catch (Exception e)
            {
                SetOriginalError(L"No translation");
            }
        }
    }
    else
    {
        SetOriginal();
    }
}


void Base64DialogBox::SetOriginal(PCWSTR pszContent)
{
    m_bEncodingError = FALSE;
    SetText(IDC_BS64_ORG_STATIC, ResourceString(IDS_ORIGINAL));
    if (pszContent)
    {
        SetText(IDC_BS64_ORG_EDIT, pszContent);
    }
    else
    {
        InvalidateRect(IDC_BS64_ORG_EDIT, NULL, TRUE);
    }
}


void Base64DialogBox::SetOriginalError(PCWSTR pszReason)
{
    m_bEncodingError = TRUE;
    SetText(IDC_BS64_ORG_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ORIGINAL), pszReason));
    InvalidateRect(IDC_BS64_ORG_EDIT, NULL, TRUE);
}


void Base64DialogBox::SetEncoded(PCWSTR pszContent, BOOL bEncoded)
{
    m_bDecodingError = FALSE;
    m_bEncoded = bEncoded;
    SetText(IDC_BS64_ENC_STATIC, ResourceString(IDS_ENCODED));
    if (pszContent)
    {
        SetText(IDC_BS64_ENC_EDIT, pszContent);
    }
    else
    {
        InvalidateRect(IDC_BS64_ENC_EDIT, NULL, TRUE);
    }
}


void Base64DialogBox::SetEncodedError(PCWSTR pszReason)
{
    m_bDecodingError = FALSE;
    m_bEncoded = FALSE;
    SetText(IDC_BS64_ENC_STATIC, String(PRINTF, L"%s [%s]", ResourceString(IDS_ENCODED), pszReason));
    InvalidateRect(IDC_BS64_ENC_EDIT, NULL, TRUE);
}


void Base64DialogBox::UpdateControlsState()
{
    if (GetTextLength(IDC_BS64_ORG_EDIT) > 0)
    {
        EnableWindow(IDC_BS64_ORG_COPY_BUTTON);
        EnableWindow(IDC_BS64_ORG_ENCODE_BUTTON);
    }
    else
    {
        DisableWindow(IDC_BS64_ORG_COPY_BUTTON);
        DisableWindow(IDC_BS64_ORG_ENCODE_BUTTON);
    }
    if (m_Original.Len > 0 || ButtonIsChecked(IDC_BS64_ORG_TEXT_RADIO))
    {
        EnableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
        EnableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
    }
    else
    {
        DisableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
        DisableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
    }
    if (GetTextLength(IDC_BS64_ENC_EDIT) > 0)
    {
        EnableWindow(IDC_BS64_ENC_COPY_BUTTON);
        EnableWindow(IDC_BS64_ENC_DECODE_BUTTON);
    }
    else
    {
        DisableWindow(IDC_BS64_ENC_COPY_BUTTON);
        DisableWindow(IDC_BS64_ENC_DECODE_BUTTON);
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
