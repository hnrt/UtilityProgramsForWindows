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


#define REGVAL_SOURCE L"Source"
#define REGVAL_METHOD L"Method"
#define REGVAL_LETTERCASE L"Lettercase"


#define UPPERCASE_LETTER 1
#define LOWERCASE_LETTER 2


#define HASH_TIMER1000MS 10100


using namespace hnrt;


HashDialogBox::HashDialogBox()
    : MyDialogBox(IDD_HASH, L"Hash")
    , m_hash()
    , m_uSource(0)
    , m_uMethod(0)
    , m_uLettercase(0)
    , m_szTextPath()
{
}


void HashDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    m_EditControls.Add(IDC_HASH_PATH_EDIT);
    m_EditControls.Add(IDC_HASH_CONTENT_EDIT);
    m_EditControls.Add(IDC_HASH_VERIFY_EDIT);
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
        m_uLettercase = RegistryValue::GetDWORD(hKey, REGVAL_LETTERCASE, UPPERCASE_LETTER);
    }
    else
    {
        m_uLettercase = UPPERCASE_LETTER;
    }
    ButtonCheck(uSource);
    ButtonCheck(uMethod);
    ButtonCheck(IDC_HASH_UPPERCASE_CHECK, m_uLettercase == UPPERCASE_LETTER);
    SetPath();
    SetValue();
    InitializeCodePageComboBox(IDC_HASH_CODEPAGE_COMBO);
    InitializeLineBreakComboBox(IDC_HASH_LINEBREAK_COMBO);
    OnSelectSource(uSource);
    OnSelectMethod(uMethod);
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
        RegistryValue::SetDWORD(hKey, REGVAL_LETTERCASE, m_uLettercase);
    }
    SetFont(IDC_HASH_PATH_EDIT, NULL);
    SetFont(IDC_HASH_CONTENT_EDIT, NULL);
    SetFont(IDC_HASH_VALUE_STATIC, NULL);
    SetFont(IDC_HASH_VERIFY_EDIT, NULL);
    m_EditControls.RemoveAll();
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
    KillTimer(hwnd, HASH_TIMER1000MS);
    m_menuView
        .Enable(IDM_VIEW_HASH, MF_ENABLED);
}


void HashDialogBox::OnTabSelectionChanged()
{
    MyDialogBox::OnTabSelectionChanged();
    SwitchMenu();
    m_menuView
        .Enable(IDM_VIEW_HASH, MF_DISABLED);
    SetTimer(hwnd, HASH_TIMER1000MS, 1000, NULL);
}


INT_PTR HashDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
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
        break;

    case IDC_HASH_COPY_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            OnCopyResult();
        }
        break;

    case IDC_HASH_BROWSE_BUTTON:
        if (idNotif == BN_CLICKED)
        {
            OnBrowse();
        }
        break;

    case IDC_HASH_FILE_RADIO:
    case IDC_HASH_TEXT_RADIO:
        if (idNotif == BN_CLICKED && ButtonIsChecked(idChild))
        {
            OnSelectSource(idChild);
        }
        break;

    case IDC_HASH_MD5_RADIO:
    case IDC_HASH_SHA1_RADIO:
    case IDC_HASH_SHA256_RADIO:
    case IDC_HASH_SHA384_RADIO:
    case IDC_HASH_SHA512_RADIO:
        if (idNotif == BN_CLICKED)
        {
            OnSelectMethod(idChild);
        }
        break;

    case IDC_HASH_UPPERCASE_CHECK:
        if (idNotif == BN_CLICKED)
        {
            OnUppercase();
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}


INT_PTR HashDialogBox::OnTimer(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case HASH_TIMER1000MS:
        UpdateEditControlMenus(m_CurrentEdit);
        break;
    default:
        break;
    }
    return 0;
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
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
    WCHAR szPath[MAX_PATH] = { 0 };
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szPath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameW(&ofn))
    {
        SetPath(szPath);
    }
}


void HashDialogBox::OnLoadFrom()
{
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
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
    WhileInScope<int> wis(m_cProcessing, m_cProcessing + 1, m_cProcessing);
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
            UINT uLineBreak = GetLineBreak();
            if (uLineBreak == 0x0a && szContent.Len > 0)
            {
                szContent.Len = ConvertToLF(szContent.Buf, static_cast<UINT>(szContent.Len));
            }
            UINT uCodePage = GetCodePage();
            if (uCodePage == CP_UTF16)
            {
                Calculate(RefPtr<DataFeeder>(new ByteDataFeeder(szContent, szContent.Len * sizeof(WCHAR))));
                nBytesIn = szContent.Len * sizeof(WCHAR);
            }
            else
            {
                Buffer<CHAR> buf(szContent.Len * 4ULL);
                int length = WideCharToMultiByte(uCodePage, 0, szContent, static_cast<int>(szContent.Len), &buf, static_cast<int>(buf.Len), NULL, NULL);
                Calculate(RefPtr<DataFeeder>(new ByteDataFeeder(&buf, length)));
                nBytesIn = length;
            }
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
        String sz = String::ToHex(m_hash.Value, m_hash.ValueLength, m_uLettercase == UPPERCASE_LETTER ? UPPERCASE : LOWERCASE);
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
    if (ApplyToLettercase(uId, m_uLettercase, UPPERCASE_LETTER))
    {
        ButtonCheck(IDC_HASH_UPPERCASE_CHECK, m_uLettercase == UPPERCASE_LETTER);
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
        ApplyToLettercase(IDM_SETTINGS_UPPERCASE, m_uLettercase, UPPERCASE_LETTER);
        ResetValueLetterCase();
    }
    else
    {
        ApplyToLettercase(IDM_SETTINGS_LOWERCASE, m_uLettercase, UPPERCASE_LETTER);
        ResetValueLetterCase();
    }
}


void HashDialogBox::SwitchMenu()
{
    if (m_uSource == IDC_HASH_FILE_RADIO)
    {
        m_menuFile
            .RemoveAll()
            .Add(ResourceString(IDS_MENU_LOADFROM), IDM_FILE_LOADFROM)
            .AddSeparator()
            .Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
        m_menuEdit
            .RemoveAll()
            .Add(ResourceString(IDS_MENU_CALCULATE), IDM_EDIT_EXECUTE, CanCalculate() ? MF_ENABLED : MF_DISABLED)
            .Add(ResourceString(IDS_MENU_COPYRESULT), IDM_EDIT_COPYRESULT, HasValue() ? MF_ENABLED : MF_DISABLED)
            .AddSeparator();
        AddEditControlMenus(IDC_HASH_PATH_EDIT);
        m_menuSettings
            .RemoveAll();
    }
    else if (m_uSource == IDC_HASH_TEXT_RADIO)
    {
        m_menuFile
            .RemoveAll()
            .Add(ResourceString(IDS_MENU_LOADFROM), IDM_FILE_LOADFROM)
            .Add(ResourceString(IDS_MENU_SAVEAS), IDM_FILE_SAVEAS)
            .AddSeparator()
            .Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
        m_menuEdit
            .RemoveAll()
            .Add(ResourceString(IDS_MENU_CALCULATE), IDM_EDIT_EXECUTE)
            .Add(ResourceString(IDS_MENU_COPYRESULT), IDM_EDIT_COPYRESULT, HasValue() ? MF_ENABLED : MF_DISABLED)
            .AddSeparator();
        AddEditControlMenus(IDC_HASH_CONTENT_EDIT);
        m_menuEdit
            .AddSeparator()
            .Add(ResourceString(IDS_MENU_COPYALL), IDM_EDIT_COPYALL)
            .AddSeparator()
            .Add(ResourceString(IDS_MENU_CLEAR), IDM_EDIT_CLEAR);
        m_menuSettings
            .RemoveAll();
        AddInputCodePageSettingMenus();
        AddOutputCodePageSettingMenus();
    }
    AddHashAlgorithmSettingMenus(IDM_SETTINGS_MD5 + m_uMethod - IDC_HASH_MD5_RADIO);
    AddLettercaseSettingMenus(IDM_SETTINGS_UPPERCASE + m_uLettercase - UPPERCASE_LETTER);
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


void HashDialogBox::SetPath(PCWSTR psz)
{
    SetText(IDC_HASH_PATH_EDIT, psz);
    ClearValue();
    EnableWindow(IDC_HASH_CALCULATE_BUTTON, TRUE);
    m_menuEdit.Enable(IDM_EDIT_EXECUTE, MF_ENABLED);
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
    SetValue(String::ToHex(m_hash.Value, m_hash.ValueLength, m_uLettercase == UPPERCASE_LETTER ? UPPERCASE : LOWERCASE));
    if (GetTextLength(IDC_HASH_VERIFY_EDIT) > 0)
    {
        VerifyValue();
    }
}


void HashDialogBox::ResetValueLetterCase()
{
    String szValue = GetText(IDC_HASH_VALUE_STATIC);
    if (m_uLettercase == UPPERCASE_LETTER)
    {
        szValue.Uppercase();
    }
    else
    {
        szValue.Lowercase();
    }
    SetValue(szValue);
}


void HashDialogBox::VerifyValue()
{
    try
    {
        ByteString bs = ByteString::FromHex(GetText(IDC_HASH_VERIFY_EDIT));
        if (bs.Len == m_hash.ValueLength && !memcmp(bs, m_hash.Value, m_hash.ValueLength))
        {
            SetVerificationResult(String(PRINTF, L" %s ", ResourceString(IDS_MATCH)));
        }
        else
        {
            SetVerificationResult(String(PRINTF, L" %s ", ResourceString(IDS_MISMATCH)));
        }
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


UINT HashDialogBox::GetLineBreak() const
{
    return ComboBoxGetSelection(IDC_HASH_LINEBREAK_COMBO, 0x0d0a);
}


UINT HashDialogBox::ConvertToLF(PWCHAR pStart, UINT uLength)
{
    PWCHAR pCur = wmemchr(pStart, L'\r', uLength);
    if (!pCur)
    {
        return uLength;
    }
    PWCHAR pDst = pCur++;
    PWCHAR pEnd = pStart + uLength;
    while (pCur < pEnd)
    {
        PWCHAR pStop = wmemchr(pCur, L'\r', pEnd - pCur);
        size_t d = (pStop ? pStop : pEnd) - pCur;
        wmemmove_s(pDst, pEnd - pDst, pCur, d);
        pDst += d;
        pCur += d + 1;
    }
    return static_cast<UINT>(pDst - pStart);
}
