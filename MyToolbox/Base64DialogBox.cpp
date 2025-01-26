#include "pch.h"
#include "Base64DialogBox.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/ResourceString.h"
#include "hnrt/WindowLayoutSnapshot.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/FileMapper.h"
#include "hnrt/FileWriter.h"
#include "hnrt/StringCommons.h"
#include "hnrt/NumberText.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"


constexpr auto REGVAL_FORMAT = L"Format";
constexpr auto REGVAL_CODEPAGE = L"CodePage";
constexpr auto REGVAL_LINEBREAK = L"LineBreak";
constexpr auto REGVAL_ORGPATH = L"OriginalPath";
constexpr auto REGVAL_ENCPATH = L"EncodedPath";
constexpr auto REGVAL_CHARSPERLINE = L"CharsPerLine";


using namespace hnrt;


Base64DialogBox::Base64DialogBox()
    : MyDialogBox(IDD_BS64, L"BASE64", IDC_BS64_STATUS_STATIC, IDC_BS64_ORG_EDIT, IDC_BS64_ENC_EDIT)
    , m_OriginalDataDisplayMode(DataDisplayMode::HEX)
    , m_CodePage(CP_UTF8)
    , m_LineBreak(LineBreak::CRLF)
    , m_CharsPerLine(0)
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
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
        m_OriginalDataDisplayMode = static_cast<DataDisplayMode>(RegistryValue::GetDWORD(hKey, REGVAL_FORMAT, DataDisplayMode::HEX));
        m_CodePage = RegistryValue::GetDWORD(hKey, REGVAL_CODEPAGE, m_CodePage);
        m_LineBreak = static_cast<LineBreak>(RegistryValue::GetDWORD(hKey, REGVAL_LINEBREAK, m_LineBreak));
        m_CharsPerLine = RegistryValue::GetDWORD(hKey, REGVAL_CHARSPERLINE);
        StrCpy(m_szOriginalPath, RegistryValue::GetSZ(hKey, REGVAL_ORGPATH));
        StrCpy(m_szEncodedPath, RegistryValue::GetSZ(hKey, REGVAL_ENCPATH));
    }
    switch (m_OriginalDataDisplayMode)
    {
    case DataDisplayMode::HEX:
        ButtonCheck(IDC_BS64_ORG_HEX_RADIO);
        break;
    case DataDisplayMode::TEXT:
        ButtonCheck(IDC_BS64_ORG_TEXT_RADIO);
        break;
    default:
        m_OriginalDataDisplayMode = DataDisplayMode::HEX;
        ButtonCheck(IDC_BS64_ORG_HEX_RADIO);
        break;
    }
    ButtonCheck(DataDisplayModeToControlId(m_OriginalDataDisplayMode));
    InitializeCodePageComboBox(IDC_BS64_ORG_CODEPAGE_COMBO, m_CodePage);
    InitializeLineBreakComboBox(IDC_BS64_ORG_LINEBREAK_COMBO, m_LineBreak);
    InitializeLineLengthComboBox(m_CharsPerLine);
    SetStatus(0, 0, L"");
    m_menuView
        .Add(ResourceString(IDS_MENU_BS64), IDM_VIEW_BS64);
}


void Base64DialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName, 0, KEY_WRITE);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetDWORD(hKey, REGVAL_FORMAT, m_OriginalDataDisplayMode);
        RegistryValue::SetDWORD(hKey, REGVAL_CODEPAGE, m_CodePage);
        RegistryValue::SetDWORD(hKey, REGVAL_LINEBREAK, m_LineBreak);
        RegistryValue::SetDWORD(hKey, REGVAL_CHARSPERLINE, m_CharsPerLine);
        RegistryValue::SetSZ(hKey, REGVAL_ORGPATH, m_szOriginalPath);
        RegistryValue::SetSZ(hKey, REGVAL_ENCPATH, m_szEncodedPath);
    }
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

    after[IDC_BS64_STATUS_STATIC].right += cxDelta;

    ExtendVertically(after[IDC_BS64_ORG_EDIT], after[IDC_BS64_ENC_EDIT], cyDelta);
    LONG dy = after[IDC_BS64_ORG_EDIT].bottom - before[IDC_BS64_ORG_EDIT].bottom;
    MoveVertically(after[IDC_BS64_ENC_STATIC], dy);
    MoveVertically(after[IDC_BS64_ENC_COPY_BUTTON], dy);
    MoveVertically(after[IDC_BS64_ENC_DECODE_BUTTON], dy);
    MoveVertically(after[IDC_BS64_ENC_LINELENGTH_STATIC], dy);
    MoveVertically(after[IDC_BS64_ENC_LINELENGTH_COMBO], dy);

    MoveVertically(after[IDC_BS64_STATUS_STATIC], cyDelta);

    after.Apply();
}


void Base64DialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
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
    UpdateControlsState(1);
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
        if (Encode())
        {
            SetFocus(IDC_BS64_ENC_COPY_BUTTON);
        }
        break;
    case IDM_EDIT_COPYRESULT1:
        CopyAllText(IDC_BS64_ORG_EDIT);
        break;
    case IDM_EDIT_EXECUTE2:
        if (Decode())
        {
            SetFocus(IDC_BS64_ORG_COPY_BUTTON);
        }
        break;
    case IDM_EDIT_COPYRESULT2:
        CopyAllText(IDC_BS64_ENC_EDIT);
        break;
    case IDC_BS64_ORG_ENCODE_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            if (Encode())
            {
                SetFocus(IDC_BS64_ENC_COPY_BUTTON);
            }
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
            if (Decode())
            {
                SetFocus(IDC_BS64_ORG_COPY_BUTTON);
            }
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
            ChangeOriginalDataDisplayMode(ControlIdToDataDisplayMode(idChild));
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_BS64_ORG_CODEPAGE_COMBO:
        if (idNotif == CBN_SELCHANGE)
        {
            m_CodePage = ComboBoxGetSelection(IDC_BS64_ORG_CODEPAGE_COMBO, m_CodePage);
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_BS64_ORG_LINEBREAK_COMBO:
        if (idNotif == CBN_SELCHANGE)
        {
            m_LineBreak = static_cast<LineBreak>(ComboBoxGetSelection(IDC_BS64_ORG_LINEBREAK_COMBO, LineBreak::CRLF));
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_BS64_ENC_LINELENGTH_COMBO:
        if (idNotif == CBN_SELCHANGE)
        {
            ChangeLinesPerLine(ComboBoxGetSelection(IDC_BS64_ENC_LINELENGTH_COMBO, 0));
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


void Base64DialogBox::OnEditChanged(int id)
{
    MyDialogBox::OnEditChanged(id);
    switch (id)
    {
    case IDC_BS64_ORG_EDIT:
        SetStatus(0, MASK_PANE1 | MASK_STATUS, L"");
        break;
    case IDC_BS64_ENC_EDIT:
        SetStatus(0, MASK_PANE2 | MASK_STATUS, L"");
        break;
    default:
        return;
    }
    UpdateControlsState(id);
}


void Base64DialogBox::OnNew()
{
    SetStatus(0, MASK_PANE1 | MASK_PANE2 | MASK_STATUS, L"");
    SetTextAndNotify(IDC_BS64_ORG_EDIT);
    SetTextAndNotify(IDC_BS64_ENC_EDIT);
}


void Base64DialogBox::OnLoad1From()
{
    if (GetLoadFromFileName(m_szOriginalPath, ResourceString(IDS_TITLE_LOAD_ORIGINAL)))
    {
        ResourceString szLeader(IDS_LOADING_ORGDATA);
        try
        {
            SetStatus(FLAG_BUSY, MASK_STATUS, szLeader);
            FileMapper fm(m_szOriginalPath);
            SetOriginalData(fm.Ptr, fm.Len);
            SetStatus(FLAG_STATUS_SUCCESSFUL, MASK_PANE1, ResourceString(IDS_DONE_IN), szLeader, NumberOfBytes(fm.Len));
        }
        catch (Exception e)
        {
            SetStatus(FLAG_STATUS_ERROR, 0, ResourceString(IDS_W_FAILED_X), szLeader, e.Message);
        }
    }
}


void Base64DialogBox::OnSave1As()
{
    if (GetSaveAsFileName(m_szOriginalPath, ResourceString(IDS_TITLE_SAVE_ORIGINAL)))
    {
        ResourceString szLeader(IDS_SAVING_ORGDATA);
        try
        {
            SetStatus(FLAG_BUSY, MASK_STATUS, szLeader);
            ByteString original = GetOriginalData();
            FileWriter(m_szOriginalPath).Write(original.Ptr, original.Len);
            SetStatus(FLAG_STATUS_SUCCESSFUL, 0, ResourceString(IDS_DONE_OUT), szLeader, NumberOfBytes(original.Len));
        }
        catch (Exception e)
        {
            SetStatus(FLAG_STATUS_ERROR, 0, ResourceString(IDS_W_FAILED_X), szLeader, e.Message);
        }
    }
}


void Base64DialogBox::OnLoad2From()
{
    if (GetLoadFromFileName(m_szEncodedPath, ResourceString(IDS_TITLE_LOAD_ENCODED)))
    {
        ResourceString szLeader(IDS_LOADING_BASE64);
        try
        {
            SetStatus(FLAG_BUSY, MASK_STATUS, szLeader);
            FileMapper fm(m_szEncodedPath);
            SetText(IDC_BS64_ENC_EDIT, ByteString(fm.Ptr, fm.Len).ToString(CP_UTF8));
            SetStatus(FLAG_STATUS_SUCCESSFUL, MASK_PANE2, ResourceString(IDS_DONE_IN), szLeader, NumberOfBytes(fm.Len));
        }
        catch (Exception e)
        {
            SetStatus(FLAG_STATUS_ERROR, 0, ResourceString(IDS_W_FAILED_X), szLeader, e.Message);
        }
    }
}


void Base64DialogBox::OnSave2As()
{
    if (GetSaveAsFileName(m_szEncodedPath, ResourceString(IDS_TITLE_SAVE_ENCODED)))
    {
        ResourceString szLeader(IDS_SAVING_BASE64);
        try
        {
            SetStatus(FLAG_BUSY, MASK_STATUS, szLeader);
            ByteString encoded = GetEncodedData();
            FileWriter(m_szEncodedPath).Write(encoded.Ptr, encoded.Len);
            SetStatus(FLAG_STATUS_SUCCESSFUL, 0, ResourceString(IDS_DONE_OUT), szLeader, NumberOfBytes(encoded.Len));
        }
        catch (Exception e)
        {
            SetStatus(FLAG_STATUS_ERROR, 0, ResourceString(IDS_W_FAILED_X), szLeader, e.Message);
        }
    }
}


bool Base64DialogBox::Encode()
{
    DBGFNC(L"Base64DialogBox::Encode");
    ResourceString szLeader(IDS_ENCODING_ORGDATA);
    try
    {
        SetStatus(FLAG_BUSY, MASK_STATUS, szLeader);
        ByteString original = GetOriginalData();
        SetText(IDC_BS64_ENC_EDIT, original.ToBase64().Wrap(m_CharsPerLine));
        SetStatus(FLAG_STATUS_SUCCESSFUL | FLAG_PANE2_SUCCESSFUL, FLAG_PANE1_ERROR | FLAG_PANE2_ERROR,
            ResourceString(IDS_W_DONE_X_IN_Y_OUT), szLeader, NumberOfBytes(original.Len), NumberOfChars(GetTextLength(IDC_BS64_ENC_EDIT)));
        return true;
    }
    catch (Exception e)
    {
        SetStatus(FLAG_STATUS_ERROR | FLAG_PANE1_ERROR, FLAG_PANE2_SUCCESSFUL,
            ResourceString(IDS_W_FAILED_X), szLeader, e.Message);
        return false;
    }
}


bool Base64DialogBox::Decode()
{
    DBGFNC(L"Base64DialogBox::Decode");
    ResourceString szLeader(IDS_DECODING_BASE64);
    try
    {
        SetStatus(FLAG_BUSY, MASK_STATUS, szLeader);
        SetOriginalData(GetDecodedData());
        SetStatus(FLAG_STATUS_SUCCESSFUL | FLAG_PANE1_SUCCESSFUL, FLAG_PANE1_ERROR | FLAG_PANE2_ERROR,
            ResourceString(IDS_W_DONE_X_IN_Y_OUT), szLeader, NumberOfChars(GetTextLength(IDC_BS64_ENC_EDIT)), NumberOfBytes(GetOriginalData().Len));
        return true;
    }
    catch (Exception e)
    {
        SetStatus(FLAG_STATUS_ERROR | FLAG_PANE2_ERROR, FLAG_PANE1_SUCCESSFUL,
            ResourceString(IDS_W_FAILED_X), szLeader, e.Message);
        return false;
    }
}


void Base64DialogBox::ChangeOriginalDataDisplayMode(DataDisplayMode mode)
{
    DBGFNC(L"Base64DialogBox::ChangeOriginalDataDisplayMode(%d)", mode);
    if (mode != m_OriginalDataDisplayMode)
    {
        ResourceString szLeader(IDS_PROCESSING);
        try
        {
            SetStatus(FLAG_BUSY, MASK_STATUS, szLeader);
            SetOriginalData(GetOriginalData(), mode);
            SetStatus(0, 0, L"");
        }
        catch (Exception e)
        {
            ButtonCheck(IDC_BS64_ORG_HEX_RADIO, m_OriginalDataDisplayMode == DataDisplayMode::HEX ? TRUE : FALSE);
            ButtonCheck(IDC_BS64_ORG_TEXT_RADIO, m_OriginalDataDisplayMode == DataDisplayMode::TEXT ? TRUE : FALSE);
            SetStatus(FLAG_STATUS_ERROR, 0, ResourceString(IDS_W_FAILED_X), szLeader, e.Message);
        }
    }
}


void Base64DialogBox::ChangeLinesPerLine(UINT cch)
{
    DBGFNC(L"Base64DialogBox::ChangeLinesPerLine(%u)", cch);
    if (cch != m_CharsPerLine)
    {
        SetStatus(FLAG_BUSY, MASK_STATUS, ResourceString(IDS_PROCESSING));
        SetText(IDC_BS64_ENC_EDIT, GetText(IDC_BS64_ENC_EDIT).Replace(L"\r\n", L"").Wrap(cch));
        m_CharsPerLine = cch;
        SetStatus(0, 0, L"");
    }
}


ByteString Base64DialogBox::GetOriginalData() const
{
    String originalString = GetText(IDC_BS64_ORG_EDIT);
    return m_OriginalDataDisplayMode == DataDisplayMode::HEX
        ? ByteString::FromHex(originalString)
        : ByteString::FromString(originalString, m_CodePage, m_LineBreak);
}


void Base64DialogBox::SetOriginalData(const void* ptr, size_t len)
{
    SetOriginalData(ByteString(ptr, len));
}


void Base64DialogBox::SetOriginalData(const ByteString& bs)
{
    SetOriginalData(bs, m_OriginalDataDisplayMode);
}


void Base64DialogBox::SetOriginalData(const ByteString& bs, DataDisplayMode mode)
{

    SetText(IDC_BS64_ORG_EDIT,
        mode == DataDisplayMode::HEX
        ? bs.ToHex(StringOptions::UPPERCASE)
        : bs.ToString(m_CodePage).ChangeLineBreak(LineBreak::CRLF));
    m_OriginalDataDisplayMode = mode;
}


ByteString Base64DialogBox::GetEncodedData() const
{
    return ByteString::FromString(GetText(IDC_BS64_ENC_EDIT), CP_UTF8);
}


ByteString Base64DialogBox::GetDecodedData() const
{
    return ByteString::FromBase64(GetText(IDC_BS64_ENC_EDIT));
}


void Base64DialogBox::InitializeLineLengthComboBox(UINT cch) const
{
    ComboBoxAdd(IDC_BS64_ENC_LINELENGTH_COMBO, ResourceString(IDS_NO_LINE_BREAK), 0);
    ComboBoxAdd(IDC_BS64_ENC_LINELENGTH_COMBO, L"72", 72);
    ComboBoxAdd(IDC_BS64_ENC_LINELENGTH_COMBO, L"144", 144);
    ComboBoxAdd(IDC_BS64_ENC_LINELENGTH_COMBO, L"216", 216);
    ComboBoxSetSelection(IDC_BS64_ENC_LINELENGTH_COMBO, cch);
}


void Base64DialogBox::UpdateControlsState(int id)
{
    switch (id)
    {
    case 0:
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
    case 1:
        EditSetReadOnly(IDC_BS64_ORG_EDIT, FALSE);
        EditSetReadOnly(IDC_BS64_ENC_EDIT, FALSE);
        UpdateControlsState(IDC_BS64_ORG_EDIT);
        UpdateControlsState(IDC_BS64_ENC_EDIT);
        break;
    case IDC_BS64_ORG_EDIT:
    {
        BOOL bEnabled = GetTextLength(IDC_BS64_ORG_EDIT) > 0 ? TRUE : FALSE;
        EnableWindow(IDC_BS64_ORG_COPY_BUTTON, bEnabled);
        EnableWindow(IDC_BS64_ORG_ENCODE_BUTTON, bEnabled);
        EnableWindow(IDC_BS64_ORG_CODEPAGE_COMBO);
        EnableWindow(IDC_BS64_ORG_LINEBREAK_COMBO);
        m_menuEdit
            .Enable(IDM_EDIT_EXECUTE1, bEnabled ? MF_ENABLED : MF_DISABLED)
            .Enable(IDM_EDIT_COPYRESULT1, bEnabled ? MF_ENABLED : MF_DISABLED);
        break;
    }
    case IDC_BS64_ENC_EDIT:
    {
        BOOL bEnabled = GetTextLength(IDC_BS64_ENC_EDIT) > 0 ? TRUE : FALSE;
        EnableWindow(IDC_BS64_ENC_COPY_BUTTON, bEnabled);
        EnableWindow(IDC_BS64_ENC_DECODE_BUTTON, bEnabled);
        m_menuEdit
            .Enable(IDM_EDIT_EXECUTE2, bEnabled ? MF_ENABLED : MF_DISABLED)
            .Enable(IDM_EDIT_COPYRESULT2, bEnabled ? MF_ENABLED : MF_DISABLED);
        break;
    }
    default:
        break;
    }
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


int Base64DialogBox::DataDisplayModeToControlId(DataDisplayMode mode)
{
    switch (mode)
    {
    case DataDisplayMode::HEX:
    default:
        return IDC_BS64_ORG_HEX_RADIO;
    case DataDisplayMode::TEXT:
        return IDC_BS64_ORG_TEXT_RADIO;
    }
}


DataDisplayMode Base64DialogBox::ControlIdToDataDisplayMode(int id)
{
    switch (id)
    {
    case IDC_BS64_ORG_HEX_RADIO:
    default:
        return DataDisplayMode::HEX;
    case IDC_BS64_ORG_TEXT_RADIO:
        return DataDisplayMode::TEXT;
    }
}
