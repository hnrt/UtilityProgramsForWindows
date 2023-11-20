#include "pch.h"
#include "HashDialogBox.h"
#include "MyFileDataFeeder.h"
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
#include "hnrt/Debug.h"


#define REG_NAME_SOURCE L"Source"
#define REG_NAME_METHOD L"Method"
#define REG_NAME_LETTERCASE L"Lettercase"


#define UPPERCASE_LETTER 1
#define LOWERCASE_LETTER 2


using namespace hnrt;


HashDialogBox::HashDialogBox()
    : MyDialogBox(IDD_HASH)
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
    RegistryValue source;
    RegistryValue method;
    RegistryValue lettercase;
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REG_SUBKEY_(Hash));
    if (rc == ERROR_SUCCESS)
    {
        source.GetDWORD(hKey, REG_NAME_SOURCE, 1);
        method.GetDWORD(hKey, REG_NAME_METHOD, 1);
        lettercase.GetDWORD(hKey, REG_NAME_LETTERCASE, UPPERCASE_LETTER);
    }
    UINT uSource = source.Type == REG_DWORD ? ((DWORD)source == 2 ? IDC_HASH_TEXT : IDC_HASH_FILE) : IDC_HASH_FILE;
    UINT uMethod = method.Type == REG_DWORD ? (IDC_HASH_MD5 + ((DWORD)method - 1) % 5) : IDC_HASH_MD5;
    m_uLettercase = lettercase.Type == REG_DWORD ? (DWORD)lettercase : UPPERCASE_LETTER;
    CheckButton(uSource);
    CheckButton(uMethod);
    CheckButton(IDC_HASH_UPPERCASE, m_uLettercase == UPPERCASE_LETTER ? TRUE : FALSE);
    SetPath();
    SetResult();
    InitializeCodePageComboBox(IDC_HASH_ENCODING);
    InitializeLineBreakComboBox(IDC_HASH_LINEBREAK);
    OnSelectSource(uSource);
    OnSelectMethod(uMethod);
    m_menuView
        .Add(ResourceString(IDS_MENU_HASH), IDM_VIEW_HASH);
}


void HashDialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, REG_SUBKEY_(Hash));
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetDWORD(hKey, REG_NAME_SOURCE, m_uSource == IDC_HASH_FILE ? 1 : 2);
        RegistryValue::SetDWORD(hKey, REG_NAME_METHOD, m_uMethod - IDC_HASH_MD5 + 1);
        RegistryValue::SetDWORD(hKey, REG_NAME_LETTERCASE, m_uLettercase);
    }
    MyDialogBox::OnDestroy();
}


void HashDialogBox::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_CALCULATE, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_COPY, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_RESULT, 0, cyDelta, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_VALUE, 0, cyDelta, cxDelta, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_ALGORITHM, cxDelta, 0, 0, cyDelta, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_MD5, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_SHA1, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_SHA256, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_SHA384, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_SHA512, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_FORMAT, cxDelta, cyDelta, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_UPPERCASE, cxDelta, cyDelta, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_SOURCE, 0, 0, cxDelta, cyDelta);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_BROWSE, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_ENCODING, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_LINEBREAK, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_PATH, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_HASH_CONTENT, 0, 0, cxDelta, cyDelta);
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
    SwitchMenu();
    m_menuView
        .Enable(IDM_VIEW_HASH, MF_DISABLED);
}


INT_PTR HashDialogBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    UINT idChild = LOWORD(wParam);
    UINT idNotif = HIWORD(wParam);
    switch (idChild)
    {
    case IDC_HASH_CALCULATE:
        OnCalculate();
        break;

    case IDC_HASH_COPY:
        OnCopy();
        break;

    case IDC_HASH_BROWSE:
        OnBrowse();
        break;

    case IDC_HASH_FILE:
    case IDC_HASH_TEXT:
        if (idNotif == BN_CLICKED && GetButtonState(idChild) == BST_CHECKED)
        {
            OnSelectSource(idChild);
        }
        break;

    case IDC_HASH_PATH:
        if (idNotif == EN_CHANGE)
        {
            if (HasResult())
            {
                ClearResult();
            }
            bool bCanCalculate = CanCalculate();
            EnableWindow(IDC_HASH_CALCULATE, bCanCalculate ? TRUE : FALSE);
            m_menuEdit.Enable(IDM_EDIT_EXECUTE, bCanCalculate ? MF_ENABLED : MF_DISABLED);
        }
        break;

    case IDC_HASH_CONTENT:
        if (idNotif == EN_CHANGE)
        {
            if (HasResult())
            {
                ClearResult();
            }
        }
        break;

    case IDC_HASH_MD5:
    case IDC_HASH_SHA1:
    case IDC_HASH_SHA256:
    case IDC_HASH_SHA384:
    case IDC_HASH_SHA512:
        OnSelectMethod(idChild);
        break;

    case IDC_HASH_UPPERCASE:
        OnUppercase();
        break;

    default:
        return FALSE;
    }
    return TRUE;
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
        SetPath(szPath);
    }
}


void HashDialogBox::OnCalculate()
{
    OnSelectSource(0);
    OnSelectMethod(0);
    try
    {
        ULONGLONG nBytesIn;
        if (m_uSource == IDC_HASH_FILE)
        {
            WCHAR szPath[MAX_PATH] = { 0 };
            GetText(IDC_HASH_PATH, szPath, MAX_PATH);
            MyFileDataFeeder feeder;
            feeder.Open(szPath);
            Calculate(feeder);
            nBytesIn = feeder.TotalLength;
        }
        else
        {
            int length2 = static_cast<int>(GetTextLength(IDC_HASH_CONTENT));
            Buffer<WCHAR> buf2(length2 + 1);
            GetText(IDC_HASH_CONTENT, buf2, buf2.Len);
            UINT uLineBreak = GetLineBreak();
            if (uLineBreak == 0x0a && length2 > 0)
            {
                length2 = ConvertToLF(&buf2, length2);
            }
            UINT uCodePage = GetCodePage();
            if (uCodePage == 1200)
            {
                length2 *= sizeof(WCHAR);
                ByteDataFeeder feeder(&buf2, length2);
                Calculate(feeder);
                nBytesIn = length2;
            }
            else
            {
                int length1 = static_cast<int>(length2 * 4);
                Buffer<CHAR> buf1(length1);
                length1 = WideCharToMultiByte(uCodePage, 0, &buf2, length2, &buf1, length1, NULL, NULL);
                ByteDataFeeder feeder(&buf1, length1);
                Calculate(feeder);
                nBytesIn = length1;
            }
        }
        SetResultHeader(nBytesIn, m_hash.ValueLength);
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


void HashDialogBox::OnLoadFrom()
{
    if (GetButtonState(IDC_HASH_FILE) == BST_CHECKED)
    {
        OnBrowse();
    }
    else if (GetButtonState(IDC_HASH_TEXT) == BST_CHECKED)
    {
        LoadTextFromFile(IDC_HASH_CONTENT, m_szTextPath);
    }
}


void HashDialogBox::OnSaveAs()
{
    if (GetButtonState(IDC_HASH_TEXT) == BST_CHECKED)
    {
        SaveTextAsFile(IDC_HASH_CONTENT, m_szTextPath);
    }
}


void HashDialogBox::OnCut()
{
    if (GetButtonState(IDC_HASH_TEXT) == BST_CHECKED)
    {
        CutText(IDC_HASH_CONTENT);
        ClearResult();
    }
}


void HashDialogBox::OnCopy()
{
    if (!HasResult())
    {
        CopyText(IDC_HASH_CONTENT);
        return;
    }
    if (!m_hash.Value)
    {
        return;
    }
    SIZE_T cch = wcslen(m_hash.Text);
    Buffer<WCHAR> buf(cch + 1);
    wcscpy_s(buf, buf.Len, m_hash.Text);
    if (m_uLettercase == UPPERCASE_LETTER)
    {
        _wcsupr_s(buf, buf.Len);
    }
    else
    {
        _wcslwr_s(buf, buf.Len);
    }
    if (!Clipboard::Copy(hwnd, buf, cch))
    {
        MessageBoxW(hwnd, ResourceString(IDS_MSG_CLIPBOARD_COPY_ERROR), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
    }
}


void HashDialogBox::OnPaste()
{
    if (GetButtonState(IDC_HASH_TEXT) == BST_CHECKED)
    {
        PasteText(IDC_HASH_CONTENT);
        ClearResult();
    }
}


void HashDialogBox::OnSelectAll()
{
    if (GetButtonState(IDC_HASH_TEXT) == BST_CHECKED)
    {
        SelectAllText(IDC_HASH_CONTENT);
    }
}


void HashDialogBox::OnClear()
{
    if (GetButtonState(IDC_HASH_TEXT) == BST_CHECKED)
    {
        ClearEdit(IDC_HASH_CONTENT);
        m_szTextPath = String::Empty;
        ClearResult();
    }
}


void HashDialogBox::OnExecute()
{
    OnCalculate();
}


void HashDialogBox::OnSettingChanged(UINT uId)
{
    if (ApplyToInputCodePage(uId))
    {
        return;
    }
    if (ApplyToOutputCodePage(uId))
    {
        return;
    }
    UINT uMethod = 0;
    if (ApplyToHashAlgorithm(uId, uMethod, IDC_HASH_MD5))
    {
        UncheckButton(m_uMethod);
        CheckButton(uMethod);
        OnSelectMethod(uMethod);
        return;
    }
    if (ApplyToLettercase(uId, m_uLettercase, UPPERCASE_LETTER))
    {
        CheckButton(IDC_HASH_UPPERCASE, m_uLettercase == UPPERCASE_LETTER ? TRUE : FALSE);
        ResetResultCase();
        return;
    }
}


void HashDialogBox::OnFeederNotify(ULONGLONG nBytesIn)
{
    SetResultHeader(nBytesIn);
}


void HashDialogBox::OnSelectSource(UINT uSource)
{
    BOOL bRadio = uSource ? TRUE : FALSE;
    BOOL bFile = uSource == IDC_HASH_FILE ? TRUE : FALSE;
    BOOL bText = uSource == IDC_HASH_TEXT ? TRUE : FALSE;
    EnableWindow(IDC_HASH_FILE, bRadio);
    EnableWindow(IDC_HASH_PATH, bFile);
    EnableWindow(IDC_HASH_BROWSE, bFile);
    EnableWindow(IDC_HASH_TEXT, bRadio);
    SetReadOnlyEdit(IDC_HASH_CONTENT, bText ? FALSE : TRUE);
    EnableWindow(IDC_HASH_ENCODING, bText);
    EnableWindow(IDC_HASH_LINEBREAK, bText);
    if (uSource)
    {
        if (m_uSource != uSource)
        {
            m_uSource = uSource;
            ClearResult();
        }
        else
        {
            SwitchMenu();
        }
    }
    EnableWindow(IDC_HASH_CALCULATE, uSource == IDC_HASH_FILE && GetTextLength(IDC_HASH_PATH) > 0 || uSource == IDC_HASH_TEXT ? TRUE : FALSE);
}


void HashDialogBox::OnSelectMethod(UINT uMethod)
{
    BOOL bRadio = uMethod ? TRUE : FALSE;
    EnableWindow(IDC_HASH_MD5, bRadio);
    EnableWindow(IDC_HASH_SHA1, bRadio);
    EnableWindow(IDC_HASH_SHA256, bRadio);
    EnableWindow(IDC_HASH_SHA384, bRadio);
    EnableWindow(IDC_HASH_SHA512, bRadio);
    if (uMethod)
    {
        if (uMethod != m_uMethod)
        {
            ApplyToHashAlgorithm(IDM_SETTINGS_MD5 + uMethod - IDC_HASH_MD5, m_uMethod, IDC_HASH_MD5);
            m_hash.Close();
            SetResultHeader();
            SetResult();
        }
        if (m_uSource == IDC_HASH_FILE)
        {
            if (GetTextLength(IDC_HASH_PATH) > 0)
            {
                EnableWindow(IDC_HASH_CALCULATE);
                m_menuEdit.Enable(IDM_EDIT_EXECUTE, MF_ENABLED);
            }
            else
            {
                DisableWindow(IDC_HASH_CALCULATE);
                m_menuEdit.Enable(IDM_EDIT_EXECUTE, MF_DISABLED);
            }
        }
        else if (m_uSource == IDC_HASH_TEXT)
        {
            EnableWindow(IDC_HASH_CALCULATE);
            m_menuEdit.Enable(IDM_EDIT_EXECUTE, MF_ENABLED);
        }
    }
    else
    {
        DisableWindow(IDC_HASH_CALCULATE);
        m_menuEdit.Enable(IDM_EDIT_EXECUTE, MF_DISABLED);
    }
}


void HashDialogBox::OnUppercase()
{
    UINT value = GetButtonState(IDC_HASH_UPPERCASE);
    if (value == BST_CHECKED)
    {
        ApplyToLettercase(IDM_SETTINGS_UPPERCASE, m_uLettercase, UPPERCASE_LETTER);
        ResetResultCase();
    }
    else if (value == BST_UNCHECKED)
    {
        ApplyToLettercase(IDM_SETTINGS_LOWERCASE, m_uLettercase, UPPERCASE_LETTER);
        ResetResultCase();
    }
    else
    {
        Debug::Put(L"SendDlgItemMessage(UPPERCASE,BM_GETCHECK) unexpectedly returned %ld.", value);
    }
}


void HashDialogBox::SwitchMenu()
{
    if (m_uSource == IDC_HASH_FILE)
    {
        m_menuFile
            .RemoveAll()
            .Add(ResourceString(IDS_MENU_LOADFROM), IDM_FILE_LOADFROM)
            .AddSeparator()
            .Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
        m_menuEdit
            .RemoveAll()
            .Add(ResourceString(IDS_MENU_CALCULATE), IDM_EDIT_EXECUTE, CanCalculate() ? MF_ENABLED : MF_DISABLED)
            .Add(ResourceString(IDS_MENU_COPYRESULT), IDM_EDIT_COPY, HasResult() ? MF_ENABLED : MF_DISABLED);
        m_menuSettings
            .RemoveAll();
    }
    else if (m_uSource == IDC_HASH_TEXT)
    {
        m_menuFile
            .RemoveAll()
            .Add(ResourceString(IDS_MENU_LOADFROM), IDM_FILE_LOADFROM)
            .Add(ResourceString(IDS_MENU_SAVEAS), IDM_FILE_SAVEAS)
            .AddSeparator()
            .Add(ResourceString(IDS_MENU_EXIT), IDM_FILE_EXIT);
        if (HasResult())
        {
            m_menuEdit
                .RemoveAll()
                .Add(ResourceString(IDS_MENU_CALCULATE), IDM_EDIT_EXECUTE)
                .Add(ResourceString(IDS_MENU_COPYRESULT), IDM_EDIT_COPY)
                .AddSeparator()
                .Add(ResourceString(IDS_MENU_CUT), IDM_EDIT_CUT)
                .Add(ResourceString(IDS_MENU_PASTE), IDM_EDIT_PASTE)
                .Add(ResourceString(IDS_MENU_DELETE), IDM_EDIT_DELETE)
                .AddSeparator()
                .Add(ResourceString(IDS_MENU_SELECTALL), IDM_EDIT_SELECTALL)
                .AddSeparator()
                .Add(ResourceString(IDS_MENU_COPYALL), IDM_EDIT_COPYALL)
                .AddSeparator()
                .Add(ResourceString(IDS_MENU_CLEAR), IDM_EDIT_CLEAR);
        }
        else
        {
            m_menuEdit
                .RemoveAll()
                .Add(ResourceString(IDS_MENU_CALCULATE), IDM_EDIT_EXECUTE)
                .AddSeparator()
                .Add(ResourceString(IDS_MENU_CUT), IDM_EDIT_CUT)
                .Add(ResourceString(IDS_MENU_COPY), IDM_EDIT_COPY)
                .Add(ResourceString(IDS_MENU_PASTE), IDM_EDIT_PASTE)
                .Add(ResourceString(IDS_MENU_DELETE), IDM_EDIT_DELETE)
                .AddSeparator()
                .Add(ResourceString(IDS_MENU_SELECTALL), IDM_EDIT_SELECTALL)
                .AddSeparator()
                .Add(ResourceString(IDS_MENU_COPYALL), IDM_EDIT_COPYALL)
                .AddSeparator()
                .Add(ResourceString(IDS_MENU_CLEAR), IDM_EDIT_CLEAR);
        }
        m_menuSettings
            .RemoveAll();
        AddInputCodePageSettingMenus();
        AddOutputCodePageSettingMenus();
    }
    AddHashAlgorithmSettingMenus(IDM_SETTINGS_MD5 + m_uMethod - IDC_HASH_MD5);
    AddLettercaseSettingMenus(IDM_SETTINGS_UPPERCASE + m_uLettercase - UPPERCASE_LETTER);
}


void HashDialogBox::Calculate(DataFeeder& rDataFeeder)
{
    SetResult();
    switch (m_uMethod)
    {
    case IDC_HASH_MD5:
    {
        MD5Hash hash(rDataFeeder);
        SetResult(hash);
        break;
    }
    case IDC_HASH_SHA1:
    {
        SHA1Hash hash(rDataFeeder);
        SetResult(hash);
        break;
    }
    case IDC_HASH_SHA256:
    {
        SHA256Hash hash(rDataFeeder);
        SetResult(hash);
        break;
    }
    case IDC_HASH_SHA384:
    {
        SHA384Hash hash(rDataFeeder);
        SetResult(hash);
        break;
    }
    case IDC_HASH_SHA512:
    {
        SHA512Hash hash(rDataFeeder);
        SetResult(hash);
        break;
    }
    default:
        break;
    }
}


void HashDialogBox::SetPath(PCWSTR psz)
{
    SetText(IDC_HASH_PATH, psz);
    SetResultHeader();
    SetResult();
}


bool HashDialogBox::CanCalculate() const
{
    if (m_uSource == IDC_HASH_FILE)
    {
        int cch = GetTextLength(IDC_HASH_PATH) + 1;
        if (cch == 1)
        {
            return false;
        }
        Buffer<WCHAR> path(cch);
        GetText(IDC_HASH_PATH, path, path.Len);
        DWORD dwAttr = GetFileAttributesW(path);
        return dwAttr != INVALID_FILE_ATTRIBUTES && dwAttr != FILE_ATTRIBUTE_DIRECTORY;
    }
    else
    {
        return m_uSource == IDC_HASH_TEXT;
    }
}


bool HashDialogBox::HasResult() const
{
    return GetTextLength(IDC_HASH_VALUE) > 0;
}


void HashDialogBox::ClearResult()
{
    m_hash.Close();
    SetResultHeader();
    SetResult();
    SwitchMenu();
}


void HashDialogBox::SetResultHeader() const
{
    SetText(IDC_HASH_RESULT, ResourceString(IDS_RESULT));
}


void HashDialogBox::SetResultHeader(ULONGLONG nBytesIn) const
{
    SetText(IDC_HASH_RESULT, String(PRINTF, ResourceString(IDS_RESULT_IN), NumberText(nBytesIn).Ptr));
}


void HashDialogBox::SetResultHeader(ULONGLONG nBytesIn, ULONG nBytesOut) const
{
    ULONG nBitsOut = nBytesOut * 8;
    SetText(IDC_HASH_RESULT, String(PRINTF, ResourceString(IDS_RESULT_IN_OUT), NumberText(nBytesIn).Ptr, NumberText(nBytesOut).Ptr, NumberText(nBitsOut).Ptr));
}


void HashDialogBox::SetResult(PCWSTR psz)
{
    SetText(IDC_HASH_VALUE, psz);
    EnableWindow(IDC_HASH_COPY, *psz ? TRUE : FALSE);
    m_menuEdit.Enable(IDM_EDIT_COPY, *psz ? MF_ENABLED : MF_DISABLED);
}


void HashDialogBox::SetResult(Hash& rHash)
{
    m_hash = rHash;
    String szValue = m_hash.Text;
    if (m_uLettercase == UPPERCASE_LETTER)
    {
        szValue.Uppercase();
    }
    else
    {
        szValue.Lowercase();
    }
    SetResult(szValue);
}


void HashDialogBox::ResetResultCase()
{
    String szValue = GetText(IDC_HASH_VALUE);
    if (m_uLettercase == UPPERCASE_LETTER)
    {
        szValue.Uppercase();
    }
    else
    {
        szValue.Lowercase();
    }
    SetResult(szValue);
}


UINT HashDialogBox::GetCodePage() const
{
    return GetComboBoxSelection(IDC_HASH_ENCODING, CP_UTF8);
}


UINT HashDialogBox::GetLineBreak() const
{
    return GetComboBoxSelection(IDC_HASH_LINEBREAK, 0x0d0a);
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
