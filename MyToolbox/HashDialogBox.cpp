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
#include "hnrt/NumberText.h"
#include "hnrt/Buffer.h"
#include "hnrt/String.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/RegistryKey.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/WindowHandle.h"
#include "hnrt/WhileInScope.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Debug.h"


static const WCHAR REGVAL_SOURCE[] = L"Source";
static const WCHAR REGVAL_METHOD[] = L"Method";
static const WCHAR REGVAL_LETTERCASE[] = L"Lettercase";


using namespace hnrt;


HashDialogBox::HashDialogBox()
    : MyDialogBox(IDD_HASH, L"Hash")
    , m_hash()
    , m_uSource(0)
    , m_uMethod(0)
    , m_uLettercase(StringOptions::UPPERCASE)
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
    UINT uSource = IDC_HASH_FILE_RADIO;
    UINT uMethod = IDC_HASH_MD5_RADIO;
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, m_szRegistryKeyName);
    if (rc == ERROR_SUCCESS)
    {
        uSource = RegistryValue::GetDWORD(hKey, REGVAL_SOURCE, 1) == 2 ? IDC_HASH_TEXT_RADIO : IDC_HASH_FILE_RADIO;
        uMethod = IDC_HASH_MD5_RADIO + (RegistryValue::GetDWORD(hKey, REGVAL_METHOD, 1) - 1) % 5;
        m_uLettercase = RegistryValue::GetDWORD(hKey, REGVAL_LETTERCASE, 1) == 1 ? StringOptions::UPPERCASE : StringOptions::LOWERCASE;
    }
    ButtonCheck(uSource);
    ButtonCheck(uMethod);
    ButtonCheck(IDC_HASH_UPPERCASE_CHECK, m_uLettercase == StringOptions::UPPERCASE);
    InitializeCodePageComboBox(IDC_HASH_CODEPAGE_COMBO);
    InitializeLineBreakComboBox(IDC_HASH_LINEBREAK_COMBO);
    OnSelectSource(uSource);
    OnSelectMethod(uMethod);
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
        RegistryValue::SetDWORD(hKey, REGVAL_SOURCE, m_uSource == IDC_HASH_FILE_RADIO ? 1 : 2);
        RegistryValue::SetDWORD(hKey, REGVAL_METHOD, m_uMethod - IDC_HASH_MD5_RADIO + 1);
        RegistryValue::SetDWORD(hKey, REGVAL_LETTERCASE, m_uLettercase == StringOptions::UPPERCASE ? 1 : 0);
    }
    SetFont(IDC_HASH_PATH_EDIT, NULL);
    SetFont(IDC_HASH_CONTENT_EDIT, NULL);
    SetFont(IDC_HASH_VALUE_STATIC, NULL);
    SetFont(IDC_HASH_VERIFY_EDIT, NULL);
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
                OnSelectSource(idChild);
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
            OnSelectMethod(idChild);
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
    default:
        return FALSE;
    }
    return TRUE;
}


INT_PTR HashDialogBox::OnControlColorStatic(WPARAM wParam, LPARAM lParam)
{
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
        if (HasValue() && GetTextLength(IDC_HASH_VERIFY_EDIT) > 0)
        {
            VerifyValue();
        }
        else
        {
            SetVerificationResult();
        }
    }
    else if (HasValue())
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
        LoadTextFromFile(IDC_HASH_CONTENT_EDIT, m_szTextPath);
    }
}


void HashDialogBox::OnSaveAs()
{
    if (ButtonIsChecked(IDC_HASH_TEXT_RADIO))
    {
        SaveTextAsFile(IDC_HASH_CONTENT_EDIT, m_szTextPath);
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
    if (HasValue())
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
    OnSelectSource(0);
    OnSelectMethod(0);
    try
    {
        ULONGLONG nBytesIn;
        if (m_uSource == IDC_HASH_FILE_RADIO)
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
    OnSelectSource(m_uSource);
    OnSelectMethod(m_uMethod);
}


void HashDialogBox::OnCopyResult()
{
    if (m_hash.ValueLength)
    {
        String sz = ByteString(m_hash.Value, m_hash.ValueLength).ToHex(m_uLettercase);
        if (!Clipboard::Write(hwnd, sz))
        {
            MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
    }
}


void HashDialogBox::OnSettingChanged(UINT uId)
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (ApplyToInputCodePage(uId))
    {
        return;
    }
    if (ApplyToOutputCodePage(uId))
    {
        return;
    }
    UINT uMethod = 0;
    if (ApplyToHashAlgorithm(uId, uMethod, IDC_HASH_MD5_RADIO))
    {
        ButtonUncheck(m_uMethod);
        ButtonCheck(uMethod);
        OnSelectMethod(uMethod);
        return;
    }
    if (ApplyToLettercase(uId, m_uLettercase))
    {
        ButtonCheck(IDC_HASH_UPPERCASE_CHECK, m_uLettercase == StringOptions::UPPERCASE);
        ResetValueLetterCase();
        return;
    }
}


void HashDialogBox::OnFeederNotify(ULONGLONG nBytesIn)
{
    SetValueHeader(nBytesIn);
}


void HashDialogBox::OnSelectSource(UINT uSource)
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    BOOL bRadio = uSource ? TRUE : FALSE;
    BOOL bFile = uSource == IDC_HASH_FILE_RADIO ? TRUE : FALSE;
    BOOL bText = uSource == IDC_HASH_TEXT_RADIO ? TRUE : FALSE;
    EnableWindow(IDC_HASH_FILE_RADIO, bRadio);
    EnableWindow(IDC_HASH_PATH_EDIT, bFile);
    EnableWindow(IDC_HASH_BROWSE_BUTTON, bFile);
    EnableWindow(IDC_HASH_TEXT_RADIO, bRadio);
    EditSetReadOnly(IDC_HASH_CONTENT_EDIT, bText ? FALSE : TRUE);
    EnableWindow(IDC_HASH_CODEPAGE_COMBO, bText);
    EnableWindow(IDC_HASH_LINEBREAK_COMBO, bText);
    if (uSource)
    {
        if (m_uSource != uSource)
        {
            m_uSource = uSource;
            ClearValue();
            SwitchMenu();
        }
        else
        {
        }
    }
    EnableWindow(IDC_HASH_CALCULATE_BUTTON, uSource == IDC_HASH_FILE_RADIO && GetTextLength(IDC_HASH_PATH_EDIT) > 0 || uSource == IDC_HASH_TEXT_RADIO ? TRUE : FALSE);
}


void HashDialogBox::OnSelectMethod(UINT uMethod)
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    BOOL bRadio = uMethod ? TRUE : FALSE;
    EnableWindow(IDC_HASH_MD5_RADIO, bRadio);
    EnableWindow(IDC_HASH_SHA1_RADIO, bRadio);
    EnableWindow(IDC_HASH_SHA256_RADIO, bRadio);
    EnableWindow(IDC_HASH_SHA384_RADIO, bRadio);
    EnableWindow(IDC_HASH_SHA512_RADIO, bRadio);
    if (uMethod)
    {
        if (uMethod != m_uMethod)
        {
            ApplyToHashAlgorithm(IDM_SETTINGS_MD5 + uMethod - IDC_HASH_MD5_RADIO, m_uMethod, IDC_HASH_MD5_RADIO);
            m_hash.Close();
            SetValueHeader();
            SetValue();
        }
        if (m_uSource == IDC_HASH_FILE_RADIO)
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
        else if (m_uSource == IDC_HASH_TEXT_RADIO)
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
}


void HashDialogBox::OnUppercase()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    if (ButtonIsChecked(IDC_HASH_UPPERCASE_CHECK))
    {
        ApplyToLettercase(IDM_SETTINGS_UPPERCASE, m_uLettercase);
        ResetValueLetterCase();
    }
    else
    {
        ApplyToLettercase(IDM_SETTINGS_LOWERCASE, m_uLettercase);
        ResetValueLetterCase();
    }
}


void HashDialogBox::SwitchMenu()
{
    if (m_uSource == IDC_HASH_FILE_RADIO)
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
            .Enable(IDM_EDIT_COPYRESULT, HasValue() ? MF_ENABLED : MF_DISABLED);
        m_menuSettings
            .RemoveAll();
    }
    else if (m_uSource == IDC_HASH_TEXT_RADIO)
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
            .Enable(IDM_EDIT_COPYRESULT, HasValue() ? MF_ENABLED : MF_DISABLED);
        m_menuSettings
            .RemoveAll();
        AddInputCodePageSettingMenus();
        AddOutputCodePageSettingMenus();
    }
    AddHashAlgorithmSettingMenus(IDM_SETTINGS_MD5 + m_uMethod - IDC_HASH_MD5_RADIO);
    AddLettercaseSettingMenus(m_uLettercase == StringOptions::UPPERCASE ? IDM_SETTINGS_UPPERCASE : IDM_SETTINGS_LOWERCASE);
}


void HashDialogBox::Calculate(RefPtr<DataFeeder> pDataFeeder)
{
    SetValue();
    switch (m_uMethod)
    {
    case IDC_HASH_MD5_RADIO:
    {
        MD5Hash hash(pDataFeeder);
        SetValue(hash);
        break;
    }
    case IDC_HASH_SHA1_RADIO:
    {
        SHA1Hash hash(pDataFeeder);
        SetValue(hash);
        break;
    }
    case IDC_HASH_SHA256_RADIO:
    {
        SHA256Hash hash(pDataFeeder);
        SetValue(hash);
        break;
    }
    case IDC_HASH_SHA384_RADIO:
    {
        SHA384Hash hash(pDataFeeder);
        SetValue(hash);
        break;
    }
    case IDC_HASH_SHA512_RADIO:
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
    if (m_uSource == IDC_HASH_FILE_RADIO)
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
        return m_uSource == IDC_HASH_TEXT_RADIO;
    }
}


bool HashDialogBox::HasValue() const
{
    return GetTextLength(IDC_HASH_VALUE_STATIC) > 0;
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
    SetText(IDC_HASH_VALUE_GROUP, String(PRINTF, ResourceString(IDS_VALUE_IN), NumberText(nBytesIn).Ptr));
}


void HashDialogBox::SetValueHeader(ULONGLONG nBytesIn, ULONG nBytesOut) const
{
    ULONG nBitsOut = nBytesOut * 8;
    SetText(IDC_HASH_VALUE_GROUP, String(PRINTF, ResourceString(IDS_VALUE_IN_OUT), NumberText(nBytesIn).Ptr, NumberText(nBytesOut).Ptr, NumberText(nBitsOut).Ptr));
}


void HashDialogBox::SetValue(PCWSTR psz)
{
    SetText(IDC_HASH_VALUE_STATIC, psz);
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
    SetValue(ByteString(m_hash.Value, m_hash.ValueLength).ToHex(m_uLettercase));
    if (GetTextLength(IDC_HASH_VERIFY_EDIT) > 0)
    {
        VerifyValue();
    }
}


void HashDialogBox::ResetValueLetterCase()
{
    SetValue(GetText(IDC_HASH_VALUE_STATIC).Lettercase(m_uLettercase));
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
