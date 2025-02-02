#include "pch.h"
#include "HashDialogBox.h"
#include "MyFileDataFeeder.h"
#include "MyToolbox.h"
#include "resource.h"
#include "hnrt/Menu.h"
#include "hnrt/Clipboard.h"
#include "hnrt/Exception.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ResourceString.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/ByteDataFeeder.h"
#include "hnrt/Buffer.h"
#include "hnrt/String.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Debug.h"


using namespace hnrt;


enum hnrt::HashSource
{
    FROM_FILE,
    FROM_TEXT
};


constexpr auto REGVAL_ALGORITHM = L"Algorithm";
constexpr auto REGVAL_SOURCE = L"Source";
constexpr auto REGVAL_LETTERCASE = L"Lettercase";


HashDialogBox::HashDialogBox()
    : MyDialogBox(IDD_HASH, L"Hash", IDC_HASH_VALUE_STATIC)
    , m_algorithm(HashAlgorithm::MD5)
    , m_hash()
    , m_source(HashSource::FROM_TEXT)
    , m_lettercase(StringOptions::UPPERCASE)
    , m_szTextPath()
{
}


void HashDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    HFONT hFont = GetApp<MyToolbox>().GetFontForData();
    SetFont(IDC_HASH_PATH_EDIT, hFont);
    SetFont(IDC_HASH_CONTENT_EDIT, hFont);
    SetFont(IDC_HASH_VALUE_STATIC, hFont);
    SetFont(IDC_HASH_VERIFY_EDIT, hFont);
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName);
    if (rc == ERROR_SUCCESS)
    {
        m_algorithm = HashAlgorithmHelpers::FromString(RegistryValue::GetSZ(hKey, REGVAL_ALGORITHM, HashAlgorithmHelpers::ToString(HashAlgorithm::MD5)));
        m_source = static_cast<HashSource>(RegistryValue::GetDWORD(hKey, REGVAL_SOURCE, HashSource::FROM_TEXT));
        m_lettercase = RegistryValue::GetDWORD(hKey, REGVAL_LETTERCASE, 1) == 1 ? StringOptions::UPPERCASE : StringOptions::LOWERCASE;
    }
    ButtonCheck(HashSourceToControlId(m_source));
    ButtonCheck(HashAlgorithmToControlId(m_algorithm));
    ButtonCheck(IDC_HASH_UPPERCASE_CHECK, m_lettercase == StringOptions::UPPERCASE);
    InitializeCodePageComboBox(IDC_HASH_CODEPAGE_COMBO);
    InitializeLineBreakComboBox(IDC_HASH_LINEBREAK_COMBO);
    ChangeSource(-HashSourceToControlId(m_source));
    ChangeHashAlgorithm(-HashAlgorithmToControlId(m_algorithm));
    OnNew();
    m_menuView
        .Add(ResourceString(IDS_MENU_HASH), IDM_VIEW_HASH);
}


void HashDialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, m_szRegistryKeyName);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetSZ(hKey, REGVAL_ALGORITHM, HashAlgorithmHelpers::ToString(m_algorithm));
        RegistryValue::SetDWORD(hKey, REGVAL_SOURCE, m_source);
        RegistryValue::SetDWORD(hKey, REGVAL_LETTERCASE, m_lettercase == StringOptions::UPPERCASE ? 1 : 0);
    }
    MyDialogBox::OnDestroy();
}


void HashDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_CALCULATE_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_COPY_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_VALUE_GROUP, 0, cyDelta, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_VALUE_STATIC, 0, cyDelta, cxDelta, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_VERIFY_GROUP, 0, cyDelta, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_VERIFY_EDIT, 0, cyDelta, cxDelta, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_ALGORITHM_GROUP, cxDelta, 0, 0, cyDelta, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_MD5_RADIO, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_SHA1_RADIO, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_SHA256_RADIO, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_SHA384_RADIO, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_SHA512_RADIO, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_FORMAT_GROUP, cxDelta, cyDelta, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_UPPERCASE_CHECK, cxDelta, cyDelta, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_RESULT_GROUP, cxDelta, cyDelta, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_RESULT_STATIC, cxDelta, cyDelta, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_SOURCE_GROUP, 0, 0, cxDelta, cyDelta);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_BROWSE_BUTTON, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_CODEPAGE_COMBO, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_LINEBREAK_COMBO, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_PATH_EDIT, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_CONTENT_EDIT, 0, 0, cxDelta, cyDelta);
}


void HashDialogBox::OnTabSelectionChanging()
{
    MyDialogBox::OnTabSelectionChanging();
    m_menuView
        .Enable(IDM_VIEW_HASH, MF_ENABLED);
}


void HashDialogBox::OnTabSelectionChanged()
{
    MyDialogBox::OnTabSelectionChanged();
    m_menuEdit
        .Insert(0, ResourceString(IDS_MENU_CALCULATE), IDM_EDIT_EXECUTE)
        .Insert(1, ResourceString(IDS_MENU_COPYRESULT), IDM_EDIT_COPYRESULT)
        .InsertSeparator(2);
    m_menuView
        .Enable(IDM_VIEW_HASH, MF_DISABLED);
    SwitchMenu();
}


INT_PTR HashDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (m_cProcessing)
    {
        return TRUE;
    }
    if (MyDialogBox::OnCommand(wParam, lParam))
    {
        return TRUE;
    }
    UINT idChild = LOWORD(wParam);
    UINT idNotif = HIWORD(wParam);
    switch (idChild)
    {
    case IDC_HASH_CALCULATE_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            OnExecute();
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_HASH_COPY_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            OnCopyResult();
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_HASH_BROWSE_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            OnBrowse();
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_HASH_FILE_RADIO:
    case IDC_HASH_TEXT_RADIO:
        if (idNotif == BN_CLICKED)
        {
            if (ButtonIsChecked(idChild))
            {
                ChangeSource(idChild);
            }
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_HASH_PATH_EDIT:
    case IDC_HASH_CONTENT_EDIT:
    case IDC_HASH_VERIFY_EDIT:
        return OnEditCommand(wParam, lParam);
    case IDC_HASH_MD5_RADIO:
    case IDC_HASH_SHA1_RADIO:
    case IDC_HASH_SHA256_RADIO:
    case IDC_HASH_SHA384_RADIO:
    case IDC_HASH_SHA512_RADIO:
        if (idNotif == BN_CLICKED)
        {
            ChangeHashAlgorithm(idChild);
        }
        else
        {
            return FALSE;
        }
        break;
    case IDC_HASH_UPPERCASE_CHECK:
        if (idNotif == BN_CLICKED)
        {
            OnUppercase();
        }
        else
        {
            return FALSE;
        }
        break;
    case IDM_SETTINGS_MD5:
    case IDM_SETTINGS_SHA1:
    case IDM_SETTINGS_SHA256:
    case IDM_SETTINGS_SHA384:
    case IDM_SETTINGS_SHA512:
    {
        int idRadio = 0;
        if (ApplyToHashAlgorithmSettingMenu(idChild, idRadio))
        {
            ButtonUncheck(HashAlgorithmToControlId(m_algorithm));
            ButtonCheck(idRadio);
            ChangeHashAlgorithm(idRadio);
        }
        else
        {
            return FALSE;
        }
        break;
    }
    case IDM_SETTINGS_UPPERCASE:
    case IDM_SETTINGS_LOWERCASE:
        if (ApplyToLettercase(idChild, m_lettercase))
        {
            ButtonCheck(IDC_HASH_UPPERCASE_CHECK, m_lettercase == StringOptions::UPPERCASE);
            ResetValueLetterCase();
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


INT_PTR HashDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
    INT_PTR ret = MyDialogBox::OnControlColorStatic(wParam, lParam);
    if (ret)
    {
        return ret;
    }
    HDC hdc = reinterpret_cast<HDC>(wParam);
    int id = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
    switch (id)
    {
    case IDC_HASH_RESULT_STATIC:
    {
        COLORREF colorText, colorBack;
        String s = GetText(IDC_HASH_RESULT_STATIC).Trim();
        if (s == ResourceString(IDS_MATCH))
        {
            colorText = RGB(240, 240, 240);
            colorBack = RGB_SUCCESSFUL;
        }
        else if (s == ResourceString(IDS_MISMATCH))
        {
            colorText = RGB(240, 240, 240);
            colorBack = RGB_ERROR;
        }
        else
        {
            colorText = GetSysColor(COLOR_WINDOWTEXT);
            colorBack = GetSysColor(COLOR_3DFACE);
        }
        SetTextColor(hdc, colorText);
        SetBkColor(hdc, colorBack);
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_3DFACE));
    }
    default:
        return 0;
    }
}


void HashDialogBox::UpdateControlsState(int id)
{
    DBGFNC(L"HashDialogBox::UpdateControlsState(%d)", id);
    if (id == IDC_HASH_VERIFY_EDIT)
    {
        if ((m_dwFlags & FLAG_STATUS_SUCCESSFUL) && GetTextLength(IDC_HASH_VERIFY_EDIT) > 0)
        {
            VerifyValue();
        }
        else
        {
            SetVerificationResult();
        }
    }
    else if (GetTextLength(IDC_HASH_VALUE_STATIC) > 0)
    {
        ClearValue();
    }
    if (id == IDC_HASH_PATH_EDIT)
    {
        bool bCanCalculate = CanCalculate();
        EnableWindow(IDC_HASH_CALCULATE_BUTTON, bCanCalculate ? TRUE : FALSE);
        m_menuEdit.Enable(IDM_EDIT_EXECUTE, bCanCalculate ? MF_ENABLED : MF_DISABLED);
    }
}


void HashDialogBox::OnBrowse()
{
    WCHAR szPath[MAX_PATH] = { 0 };
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szPath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameW(&ofn))
    {
        SetText(IDC_HASH_PATH_EDIT, szPath);
    }
}


void HashDialogBox::OnLoadFrom()
{
    if (ButtonIsChecked(IDC_HASH_FILE_RADIO))
    {
        OnBrowse();
    }
    else if (ButtonIsChecked(IDC_HASH_TEXT_RADIO))
    {
        try
        {
            LoadTextFromFile(IDC_HASH_CONTENT_EDIT, ResourceString(IDS_LOADING_TEXT), m_szTextPath);
        }
        catch (Win32Exception e)
        {
            SetError(String(PRINTF, L"%s: %s", e.Message, ErrorMessage::Get(e.Error)));
        }
        catch (Exception e)
        {
            SetError(e.Message);
        }
    }
}


void HashDialogBox::OnSaveAs()
{
    if (ButtonIsChecked(IDC_HASH_TEXT_RADIO))
    {
        try
        {
            SaveTextAsFile(IDC_HASH_CONTENT_EDIT, ResourceString(IDS_SAVING_TEXT), m_szTextPath);
        }
        catch (Win32Exception e)
        {
            SetError(String(PRINTF, L"%s: %s", e.Message, ErrorMessage::Get(e.Error)));
        }
        catch (Exception e)
        {
            SetError(e.Message);
        }
    }
}


void HashDialogBox::OnClear()
{
    MyDialogBox::OnClear();
    if (ButtonIsChecked(IDC_HASH_FILE_RADIO))
    {
        if (m_CurrentEdit == 0)
        {
            EditClear(IDC_HASH_PATH_EDIT);
        }
    }
    else if (ButtonIsChecked(IDC_HASH_TEXT_RADIO))
    {
        if (m_CurrentEdit == 0)
        {
            EditClear(IDC_HASH_CONTENT_EDIT);
        }
        m_szTextPath = String::Empty;
    }
    if (GetTextLength(IDC_HASH_VALUE_STATIC) > 0)
    {
        ClearValue();
    }
}


void HashDialogBox::OnNew()
{
    SetText(IDC_HASH_PATH_EDIT);
    SetTextAndNotify(IDC_HASH_CONTENT_EDIT);
    SetText(IDC_HASH_VERIFY_EDIT);
    m_szTextPath = String::Empty;
}


void HashDialogBox::OnExecute()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    ChangeSource(0);
    ChangeHashAlgorithm(0);
    try
    {
        ULONGLONG nBytesIn;
        if (m_source == HashSource::FROM_FILE)
        {
            RefPtr<MyFileDataFeeder> pFeeder(new MyFileDataFeeder());
            pFeeder->Open(GetText(IDC_HASH_PATH_EDIT));
            Calculate(RefPtr<DataFeeder>::Cast(pFeeder.Ptr));
            nBytesIn = pFeeder->TotalLength;
        }
        else
        {
            String szContent = GetText(IDC_HASH_CONTENT_EDIT);
            UINT uCodePage = GetCodePage();
            LineBreak uLineBreak = GetLineBreak();
            ByteString data = ByteString::FromString(szContent, uCodePage, uLineBreak);
            Calculate(RefPtr<DataFeeder>(new ByteDataFeeder(data, data.Len)));
            nBytesIn = data.Len;
        }
        SetValueHeader(nBytesIn, m_hash.ValueLength);
    }
    catch (Win32Exception e)
    {
        static const WCHAR szCreateFile[] = L"CreateFile";
        static const WCHAR szReadFile[] = L"ReadFile";
        if (!wcsncmp(e.Message, szCreateFile, wcslen(szCreateFile)))
        {
            MessageBox(hwnd, String(PRINTF, ResourceString(IDS_OPENFILE_FAILURE), ErrorMessage::Get(e.Error)), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
        else if (!wcsncmp(e.Message, szReadFile, wcslen(szReadFile)))
        {
            MessageBox(hwnd, String(PRINTF, ResourceString(IDS_READFILE_FAILURE), ErrorMessage::Get(e.Error)), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
        else
        {
            MessageBox(hwnd, ErrorMessage::Get(e.Error), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
    }
    catch (Exception e)
    {
        MessageBox(hwnd, e.Message, ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
    ChangeSource(HashSourceToControlId(m_source));
    ChangeHashAlgorithm(HashAlgorithmToControlId(m_algorithm));
}


void HashDialogBox::OnCopyResult()
{
    if (m_hash.ValueLength)
    {
        String sz = ByteString(m_hash.Value, m_hash.ValueLength).ToHex(m_lettercase);
        if (!Clipboard::Write(hwnd, sz))
        {
            MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
    }
}


void HashDialogBox::OnFeederNotify(ULONGLONG nBytesIn)
{
    SetValueHeader(nBytesIn);
}


void HashDialogBox::ChangeSource(int id)
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (id)
    {
        if (id != HashSourceToControlId(m_source))
        {
            if (id < 0)
            {
                id *= -1;
            }
            switch (id)
            {
            case IDC_HASH_FILE_RADIO:
                m_source = HashSource::FROM_FILE;
                break;
            case IDC_HASH_TEXT_RADIO:
            default:
                m_source = HashSource::FROM_TEXT;
                break;
            }
            ClearValue();
            SwitchMenu();
        }
        else
        {
        }
    }
    BOOL bRadio = id ? TRUE : FALSE;
    BOOL bFile = id == IDC_HASH_FILE_RADIO ? TRUE : FALSE;
    BOOL bText = id == IDC_HASH_TEXT_RADIO ? TRUE : FALSE;
    EnableWindow(IDC_HASH_FILE_RADIO, bRadio);
    EnableWindow(IDC_HASH_PATH_EDIT, bFile);
    EnableWindow(IDC_HASH_BROWSE_BUTTON, bFile);
    EnableWindow(IDC_HASH_TEXT_RADIO, bRadio);
    EditSetReadOnly(IDC_HASH_CONTENT_EDIT, bText ? FALSE : TRUE);
    EnableWindow(IDC_HASH_CODEPAGE_COMBO, bText);
    EnableWindow(IDC_HASH_LINEBREAK_COMBO, bText);
    EnableWindow(IDC_HASH_CALCULATE_BUTTON, id == IDC_HASH_FILE_RADIO && GetTextLength(IDC_HASH_PATH_EDIT) > 0 || id == IDC_HASH_TEXT_RADIO ? TRUE : FALSE);
}


void HashDialogBox::ChangeHashAlgorithm(int id)
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (id)
    {
        if (id != HashAlgorithmToControlId(m_algorithm))
        {
            if (id < 0)
            {
                id *= -1;
            }
            switch (id)
            {
            case IDC_HASH_MD5_RADIO:
                m_algorithm = HashAlgorithm::MD5;
                break;
            case IDC_HASH_SHA1_RADIO:
                m_algorithm = HashAlgorithm::SHA1;
                break;
            case IDC_HASH_SHA256_RADIO:
                m_algorithm = HashAlgorithm::SHA256;
                break;
            case IDC_HASH_SHA384_RADIO:
                m_algorithm = HashAlgorithm::SHA384;
                break;
            case IDC_HASH_SHA512_RADIO:
                m_algorithm = HashAlgorithm::SHA512;
                break;
            default:
                throw Exception(L"HashDialogBox::OnSelectMethod: Bad value(%d)", id);
            }
            ApplyToHashAlgorithmSettingMenu(IDM_SETTINGS_MD5 + id - IDC_HASH_MD5_RADIO, id);
            m_hash.Close();
            SetValueHeader();
            SetValue();
        }
        if (m_source == HashSource::FROM_FILE)
        {
            if (GetTextLength(IDC_HASH_PATH_EDIT) > 0)
            {
                EnableWindow(IDC_HASH_CALCULATE_BUTTON);
                m_menuEdit.Enable(IDM_EDIT_EXECUTE, MF_ENABLED);
            }
            else
            {
                DisableWindow(IDC_HASH_CALCULATE_BUTTON);
                m_menuEdit.Enable(IDM_EDIT_EXECUTE, MF_DISABLED);
            }
        }
        else if (m_source == HashSource::FROM_TEXT)
        {
            EnableWindow(IDC_HASH_CALCULATE_BUTTON);
            m_menuEdit.Enable(IDM_EDIT_EXECUTE, MF_ENABLED);
        }
    }
    else
    {
        DisableWindow(IDC_HASH_CALCULATE_BUTTON);
        m_menuEdit.Enable(IDM_EDIT_EXECUTE, MF_DISABLED);
    }
    BOOL bRadio = id ? TRUE : FALSE;
    EnableWindow(IDC_HASH_MD5_RADIO, bRadio);
    EnableWindow(IDC_HASH_SHA1_RADIO, bRadio);
    EnableWindow(IDC_HASH_SHA256_RADIO, bRadio);
    EnableWindow(IDC_HASH_SHA384_RADIO, bRadio);
    EnableWindow(IDC_HASH_SHA512_RADIO, bRadio);
}


void HashDialogBox::OnUppercase()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (ButtonIsChecked(IDC_HASH_UPPERCASE_CHECK))
    {
        ApplyToLettercase(IDM_SETTINGS_UPPERCASE, m_lettercase);
        ResetValueLetterCase();
    }
    else
    {
        ApplyToLettercase(IDM_SETTINGS_LOWERCASE, m_lettercase);
        ResetValueLetterCase();
    }
}


void HashDialogBox::SwitchMenu()
{
    if (m_source == HashSource::FROM_FILE)
    {
        m_menuFile
            .RemoveAll()
            .Add(ResourceString(IDS_MENU_NEW), IDM_FILE_NEW)
            .AddSeparator()
            .Add(ResourceString(IDS_MENU_LOADFROM), IDM_FILE_LOADFROM)
            .AddSeparator()
            .Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
        m_menuEdit
            .Enable(IDM_EDIT_EXECUTE, CanCalculate() ? MF_ENABLED : MF_DISABLED)
            .Enable(IDM_EDIT_COPYRESULT, (m_dwFlags & FLAG_STATUS_SUCCESSFUL) ? MF_ENABLED : MF_DISABLED);
        m_menuSettings
            .RemoveAll();
    }
    else if (m_source == HashSource::FROM_TEXT)
    {
        m_menuFile
            .RemoveAll()
            .Add(ResourceString(IDS_MENU_NEW), IDM_FILE_NEW)
            .AddSeparator()
            .Add(ResourceString(IDS_MENU_LOADFROM), IDM_FILE_LOADFROM)
            .Add(ResourceString(IDS_MENU_SAVEAS), IDM_FILE_SAVEAS)
            .AddSeparator()
            .Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
        m_menuEdit
            .Enable(IDM_EDIT_EXECUTE, MF_ENABLED)
            .Enable(IDM_EDIT_COPYRESULT, (m_dwFlags & FLAG_STATUS_SUCCESSFUL) ? MF_ENABLED : MF_DISABLED);
        m_menuSettings
            .RemoveAll();
        AddInputCodePageSettingMenus();
        AddOutputCodePageSettingMenus();
    }
    AddHashAlgorithmSettingMenus(IDM_SETTINGS_MD5 + HashAlgorithmToControlId(m_algorithm) - IDC_HASH_MD5_RADIO);
    AddLettercaseSettingMenus(m_lettercase == StringOptions::UPPERCASE ? IDM_SETTINGS_UPPERCASE : IDM_SETTINGS_LOWERCASE);
}


void HashDialogBox::Calculate(RefPtr<DataFeeder> pDataFeeder)
{
    SetValue();
    switch (m_algorithm)
    {
    case HashAlgorithm::MD5:
    {
        MD5Hash hash(pDataFeeder);
        SetValue(hash);
        break;
    }
    case HashAlgorithm::SHA1:
    {
        SHA1Hash hash(pDataFeeder);
        SetValue(hash);
        break;
    }
    case HashAlgorithm::SHA256:
    {
        SHA256Hash hash(pDataFeeder);
        SetValue(hash);
        break;
    }
    case HashAlgorithm::SHA384:
    {
        SHA384Hash hash(pDataFeeder);
        SetValue(hash);
        break;
    }
    case HashAlgorithm::SHA512:
    {
        SHA512Hash hash(pDataFeeder);
        SetValue(hash);
        break;
    }
    default:
        break;
    }
}


bool HashDialogBox::CanCalculate() const
{
    if (m_source == HashSource::FROM_FILE)
    {
        String szPath = GetText(IDC_HASH_PATH_EDIT).Trim();
        if (!szPath.Len)
        {
            return false;
        }
        DWORD dwAttr = GetFileAttributesW(szPath);
        return dwAttr != INVALID_FILE_ATTRIBUTES && dwAttr != FILE_ATTRIBUTE_DIRECTORY;
    }
    else
    {
        return m_source == HashSource::FROM_TEXT;
    }
}


void HashDialogBox::ClearValue()
{
    m_hash.Close();
    SetValueHeader();
    SetValue();
    SwitchMenu();
}


void HashDialogBox::SetValueHeader() const
{
    SetText(IDC_HASH_VALUE_GROUP, ResourceString(IDS_VALUE));
}


void HashDialogBox::SetValueHeader(ULONGLONG nBytesIn) const
{
    SetText(IDC_HASH_VALUE_GROUP, String(PRINTF, ResourceString(IDS_VALUE_IN), NumberOfBytes(nBytesIn)));
}


void HashDialogBox::SetValueHeader(ULONGLONG nBytesIn, ULONG nBytesOut) const
{
    ULONG nBitsOut = nBytesOut * 8;
    SetText(IDC_HASH_VALUE_GROUP, String(PRINTF, ResourceString(IDS_VALUE_IN_OUT), NumberOfBytes(nBytesIn), NumberOfBytes(nBytesOut), NumberOfBits(nBitsOut)));
}


void HashDialogBox::SetValue(PCWSTR psz)
{
    SetStatus(FLAG_STATUS_SUCCESSFUL, FLAG_STATUS_ERROR, L"%s", psz);
    EnableWindow(IDC_HASH_COPY_BUTTON, *psz ? TRUE : FALSE);
    m_menuEdit.Enable(IDM_EDIT_COPYRESULT, *psz ? MF_ENABLED : MF_DISABLED);
    if (!*psz)
    {
        SetVerificationResult();
    }
}


void HashDialogBox::SetValue(Hash& rHash)
{
    m_hash = rHash;
    SetValue(ByteString(m_hash.Value, m_hash.ValueLength).ToHex(m_lettercase));
    if (GetTextLength(IDC_HASH_VERIFY_EDIT) > 0)
    {
        VerifyValue();
    }
}


void HashDialogBox::ResetValueLetterCase()
{
    SetValue(GetText(IDC_HASH_VALUE_STATIC).Lettercase(m_lettercase));
}


void HashDialogBox::SetError(PCWSTR psz)
{
    SetStatus(FLAG_STATUS_ERROR, FLAG_STATUS_SUCCESSFUL, L"ERROR:  %s", psz);
    DisableWindow(IDC_HASH_COPY_BUTTON);
    m_menuEdit.Enable(IDM_EDIT_COPYRESULT, MF_DISABLED);
}


void HashDialogBox::VerifyValue()
{
    try
    {
        ByteString bs = ByteString::FromHex(GetText(IDC_HASH_VERIFY_EDIT));
        SetVerificationResult(String(PRINTF, L" %s ",
            ResourceString((bs.Len == m_hash.ValueLength && !memcmp(bs, m_hash.Value, m_hash.ValueLength)) ? IDS_MATCH : IDS_MISMATCH)));
    }
    catch (...)
    {
        SetVerificationResult(L"?");
    }
}


void HashDialogBox::SetVerificationResult(PCWSTR psz)
{
    SetText(IDC_HASH_RESULT_STATIC, psz);
}


UINT HashDialogBox::GetCodePage() const
{
    return ComboBoxGetSelection(IDC_HASH_CODEPAGE_COMBO, CP_UTF8);
}


LineBreak HashDialogBox::GetLineBreak() const
{
    return static_cast<LineBreak>(ComboBoxGetSelection(IDC_HASH_LINEBREAK_COMBO, LineBreak::CRLF));
}


void HashDialogBox::AddHashAlgorithmSettingMenus(int idMenuItem)
{
    m_menuSettings
        .Add(ResourceString(IDS_MENU_ALGORITHM), Menu()
        .Add(HashAlgorithmHelpers::ToString(HashAlgorithm::MD5), IDM_SETTINGS_MD5, idMenuItem == IDM_SETTINGS_MD5 ? MF_CHECKED : MF_UNCHECKED)
        .Add(HashAlgorithmHelpers::ToString(HashAlgorithm::SHA1), IDM_SETTINGS_SHA1, idMenuItem == IDM_SETTINGS_SHA1 ? MF_CHECKED : MF_UNCHECKED)
        .Add(HashAlgorithmHelpers::ToString(HashAlgorithm::SHA256), IDM_SETTINGS_SHA256, idMenuItem == IDM_SETTINGS_SHA256 ? MF_CHECKED : MF_UNCHECKED)
        .Add(HashAlgorithmHelpers::ToString(HashAlgorithm::SHA384), IDM_SETTINGS_SHA384, idMenuItem == IDM_SETTINGS_SHA384 ? MF_CHECKED : MF_UNCHECKED)
        .Add(HashAlgorithmHelpers::ToString(HashAlgorithm::SHA512), IDM_SETTINGS_SHA512, idMenuItem == IDM_SETTINGS_SHA512 ? MF_CHECKED : MF_UNCHECKED));
}


bool HashDialogBox::ApplyToHashAlgorithmSettingMenu(int idMenuItem, int& idRadio)
{
    switch (idMenuItem)
    {
    case IDM_SETTINGS_MD5:
        idRadio = IDC_HASH_MD5_RADIO;
        break;
    case IDM_SETTINGS_SHA1:
        idRadio = IDC_HASH_SHA1_RADIO;
        break;
    case IDM_SETTINGS_SHA256:
        idRadio = IDC_HASH_SHA256_RADIO;
        break;
    case IDM_SETTINGS_SHA384:
        idRadio = IDC_HASH_SHA384_RADIO;
        break;
    case IDM_SETTINGS_SHA512:
        idRadio = IDC_HASH_SHA512_RADIO;
        break;
    default:
        return false;
    }
    m_menuSettings[ResourceString(IDS_MENU_ALGORITHM)]
        .Modify(
            IDM_SETTINGS_MD5, idMenuItem == IDM_SETTINGS_MD5 ? MF_CHECKED : MF_UNCHECKED,
            IDM_SETTINGS_MD5, HashAlgorithmHelpers::ToString(HashAlgorithm::MD5))
        .Modify(
            IDM_SETTINGS_SHA1, idMenuItem == IDM_SETTINGS_SHA1 ? MF_CHECKED : MF_UNCHECKED,
            IDM_SETTINGS_SHA1, HashAlgorithmHelpers::ToString(HashAlgorithm::SHA1))
        .Modify(
            IDM_SETTINGS_SHA256, idMenuItem == IDM_SETTINGS_SHA256 ? MF_CHECKED : MF_UNCHECKED,
            IDM_SETTINGS_SHA256, HashAlgorithmHelpers::ToString(HashAlgorithm::SHA256))
        .Modify(
            IDM_SETTINGS_SHA384, idMenuItem == IDM_SETTINGS_SHA384 ? MF_CHECKED : MF_UNCHECKED,
            IDM_SETTINGS_SHA384, HashAlgorithmHelpers::ToString(HashAlgorithm::SHA384))
        .Modify(
            IDM_SETTINGS_SHA512, idMenuItem == IDM_SETTINGS_SHA512 ? MF_CHECKED : MF_UNCHECKED,
            IDM_SETTINGS_SHA512, HashAlgorithmHelpers::ToString(HashAlgorithm::SHA512));
    return true;
}


int HashDialogBox::HashAlgorithmToControlId(HashAlgorithm ha)
{
    switch (ha)
    {
    case HashAlgorithm::MD5: return IDC_HASH_MD5_RADIO;
    case HashAlgorithm::SHA1: return IDC_HASH_SHA1_RADIO;
    case HashAlgorithm::SHA256: return IDC_HASH_SHA256_RADIO;
    case HashAlgorithm::SHA384: return IDC_HASH_SHA384_RADIO;
    case HashAlgorithm::SHA512: return IDC_HASH_SHA512_RADIO;
    default: return IDC_HASH_MD5_RADIO;
    }
}


int HashDialogBox::HashSourceToControlId(HashSource src)
{
    switch (src)
    {
    case HashSource::FROM_FILE: return IDC_HASH_FILE_RADIO;
    case HashSource::FROM_TEXT: return IDC_HASH_TEXT_RADIO;
    default: return IDC_HASH_TEXT_RADIO;
    }
}
