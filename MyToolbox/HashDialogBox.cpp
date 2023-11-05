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
#define REG_NAME_UPPERCASE L"Uppercase"


#define LABEL_UTF8 L"UTF-8"
#define LABEL_UTF16LE L"UTF-16 LE"
#define LABEL_CP932 L"CP 932"

#define LABEL_CRLF L"CRLF"
#define LABEL_LF L"LF"


using namespace hnrt;


HashDialogBox::HashDialogBox()
    : MyDialogBox(IDD_HASH)
    , m_hash()
    , m_uSource(IDC_HASH_FILE)
    , m_uMethod(IDC_HASH_MD5)
    , m_bUppercase(FALSE)
    , m_szTextPath()
{
}


void HashDialogBox::OnCreate()
{
    MyDialogBox::OnCreate();
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REG_SUBKEY_(Hash));
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue value;
        m_uSource = value.GetDWORD(hKey, REG_NAME_SOURCE, 1) == 2 ? IDC_HASH_TEXT : IDC_HASH_FILE;
        m_uMethod = ((value.GetDWORD(hKey, REG_NAME_METHOD, 1) - 1) % 5) + IDC_HASH_MD5;
        m_bUppercase = value.GetDWORD(hKey, REG_NAME_UPPERCASE) ? TRUE : FALSE;
    }
    CheckButton(m_uSource);
    CheckButton(m_uMethod);
    CheckButton(IDC_HASH_UPPERCASE, m_bUppercase);
    SetText(IDC_HASH_VALUE, L"");
    AddStringToComboBox(IDC_HASH_ENCODING, LABEL_UTF8);
    AddStringToComboBox(IDC_HASH_ENCODING, LABEL_UTF16LE);
    AddStringToComboBox(IDC_HASH_ENCODING, LABEL_CP932);
    SetComboBoxSelection(IDC_HASH_ENCODING, LABEL_UTF8);
    AddStringToComboBox(IDC_HASH_LINEBREAK, LABEL_CRLF);
    AddStringToComboBox(IDC_HASH_LINEBREAK, LABEL_LF);
    SetComboBoxSelection(IDC_HASH_LINEBREAK, LABEL_CRLF);
    DisableWindow(IDC_HASH_COPY);
    OnSelectSource(m_uSource);
    m_menuView
        .Add(ResourceString(IDS_HASH_TABLABEL), IDM_VIEW_HASH);
}


void HashDialogBox::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, REG_SUBKEY_(Hash));
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue::SetDWORD(hKey, REG_NAME_SOURCE, m_uSource == IDC_HASH_FILE ? 1 : 2);
        RegistryValue::SetDWORD(hKey, REG_NAME_METHOD, m_uMethod - IDC_HASH_MD5 + 1);
        RegistryValue::SetDWORD(hKey, REG_NAME_UPPERCASE, m_bUppercase ? 1UL : 0UL);
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
    UINT uTextFlags = GetButtonState(IDC_HASH_TEXT) == BST_CHECKED ? MF_ENABLED : MF_DISABLED;
    m_menuFile
        .RemoveAll()
        .Add(ResourceString(IDS_LOADFROM), IDM_FILE_LOADFROM)
        .Add(ResourceString(IDS_SAVEAS), IDM_FILE_SAVEAS, uTextFlags)
        .AddSeparator()
        .Add(ResourceString(IDS_EXIT), IDM_FILE_EXIT);
    m_menuEdit
        .RemoveAll()
        .Add(ResourceString(IDS_COPY), IDM_EDIT_COPY, MF_DISABLED)
        .AddSeparator()
        .Add(ResourceString(IDS_CUT), IDM_EDIT_CUT, uTextFlags)
        .Add(ResourceString(IDS_PASTE), IDM_EDIT_PASTE, uTextFlags)
        .AddSeparator()
        .Add(ResourceString(IDS_SELECTALL), IDM_EDIT_SELECTALL, uTextFlags)
        .AddSeparator()
        .Add(ResourceString(IDS_CLEAR), IDM_EDIT_CLEAR, uTextFlags);
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
        SetText(IDC_HASH_PATH, szPath);
    }
}


void HashDialogBox::OnCalculate()
{
    OnSelectSource(0);
    OnSelectMethod(0);
    DisableWindow(IDC_HASH_CALCULATE);
    DisableWindow(IDC_HASH_COPY);
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
        EnableWindow(IDC_HASH_COPY);
    }
    catch (Win32Exception e)
    {
        static const WCHAR szCreateFile[] = L"CreateFile";
        static const WCHAR szReadFile[] = L"ReadFile";
        if (!wcsncmp(e.Message, szCreateFile, wcslen(szCreateFile)))
        {
            MessageBox(hwnd, String::Format(ResourceString(IDS_OPENFILE_FAILURE), ErrorMessage::Get(e.Error)), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        }
        else if (!wcsncmp(e.Message, szReadFile, wcslen(szReadFile)))
        {
            MessageBox(hwnd, String::Format(ResourceString(IDS_READFILE_FAILURE), ErrorMessage::Get(e.Error)), ResourceString(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
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
    EnableWindow(IDC_HASH_CALCULATE);
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
        LoadTextFromFile(IDC_HASH_CONTENT, m_szTextPath, MAX_PATH);
    }
}


void HashDialogBox::OnSaveAs()
{
    if (GetButtonState(IDC_HASH_TEXT) == BST_CHECKED)
    {
        SaveTextAsFile(IDC_HASH_CONTENT, m_szTextPath, MAX_PATH);
    }
}


void HashDialogBox::OnCut()
{
    if (GetButtonState(IDC_HASH_TEXT) == BST_CHECKED)
    {
        CutText(IDC_HASH_CONTENT);
    }
}


void HashDialogBox::OnCopy()
{
    if (!m_hash.Value)
    {
        return;
    }
    SIZE_T cch = wcslen(m_hash.Text);
    Buffer<WCHAR> buf(cch + 1);
    wcscpy_s(buf, buf.Len, m_hash.Text);
    if (m_bUppercase)
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
        wmemset(m_szTextPath, L'\0', MAX_PATH);
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
    if (uSource && m_uSource != uSource)
    {
        m_uSource = uSource;
        m_hash.Close();
        SetResultHeader();
        SetResult();
        DisableWindow(IDC_HASH_COPY);
    }
    m_menuFile
        .Modify(
            IDM_FILE_SAVEAS, bText ? MF_ENABLED : MF_DISABLED,
            IDM_FILE_SAVEAS, ResourceString(IDS_SAVEAS));
    m_menuEdit
        .Modify(
            IDM_EDIT_COPY, GetTextLength(IDC_HASH_VALUE) > 0 ? MF_ENABLED : MF_DISABLED,
            IDM_EDIT_COPY, ResourceString(IDS_COPY))
        .Modify(
            IDM_EDIT_CUT, bText ? MF_ENABLED : MF_DISABLED,
            IDM_EDIT_CUT, ResourceString(IDS_CUT))
        .Modify(
            IDM_EDIT_PASTE, bText ? MF_ENABLED : MF_DISABLED,
            IDM_EDIT_PASTE, ResourceString(IDS_PASTE))
        .Modify(
            IDM_EDIT_SELECTALL, bText ? MF_ENABLED : MF_DISABLED,
            IDM_EDIT_SELECTALL, ResourceString(IDS_SELECTALL))
        .Modify(
            IDM_EDIT_CLEAR, bText ? MF_ENABLED : MF_DISABLED,
            IDM_EDIT_CLEAR, ResourceString(IDS_CLEAR));
}


void HashDialogBox::OnSelectMethod(UINT uMethod)
{
    BOOL bRadio = uMethod ? TRUE : FALSE;
    EnableWindow(IDC_HASH_MD5, bRadio);
    EnableWindow(IDC_HASH_SHA1, bRadio);
    EnableWindow(IDC_HASH_SHA256, bRadio);
    EnableWindow(IDC_HASH_SHA384, bRadio);
    EnableWindow(IDC_HASH_SHA512, bRadio);
    if (uMethod && uMethod != m_uMethod)
    {
        m_uMethod = uMethod;
        m_hash.Close();
        SetResultHeader();
        SetResult();
    }
}


void HashDialogBox::OnUppercase()
{
    UINT value = GetButtonState(IDC_HASH_UPPERCASE);
    if (value == BST_CHECKED)
    {
        m_bUppercase = TRUE;
        ResetResultCase();
    }
    else if (value == BST_UNCHECKED)
    {
        m_bUppercase = FALSE;
        ResetResultCase();
    }
    else
    {
        Debug::Put(L"SendDlgItemMessage(UPPERCASE,BM_GETCHECK) unexpectedly returned %ld.", value);
    }
}


void HashDialogBox::Calculate(DataFeeder& rDataFeeder)
{
    SetResult(L"");
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


UINT HashDialogBox::GetCodePage()
{
    int selected = GetComboBoxSelection(IDC_HASH_ENCODING);
    UINT length = GetListBoxTextLength(IDC_HASH_ENCODING, selected);
    Buffer<WCHAR> charset(length + 1);
    GetListBoxText(IDC_HASH_ENCODING, selected, charset);
    if (!wcscmp(charset, LABEL_UTF8))
    {
        return CP_UTF8;
    }
    else if (!wcscmp(charset, LABEL_UTF16LE))
    {
        return 1200;
    }
    else if (!wcscmp(charset, LABEL_CP932))
    {
        return 932;
    }
    else
    {
        return CP_UTF8;
    }
}


UINT HashDialogBox::GetLineBreak()
{
    int selected = GetComboBoxSelection(IDC_HASH_LINEBREAK);
    UINT length = GetListBoxTextLength(IDC_HASH_LINEBREAK, selected);
    Buffer<WCHAR> lineBreak(length + 1);
    GetListBoxText(IDC_HASH_LINEBREAK, selected, lineBreak);
    if (!wcscmp(lineBreak, LABEL_CRLF))
    {
        return 0x0d0a;
    }
    else if (!wcscmp(lineBreak, LABEL_LF))
    {
        return 0x0a;
    }
    else
    {
        return 0x0d0a;
    }
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


void HashDialogBox::SetResultHeader()
{
    SetText(IDC_HASH_RESULT, ResourceString(IDS_RESULT));
}


void HashDialogBox::SetResultHeader(ULONGLONG nBytesIn)
{
    WCHAR szBuf[260];
    _snwprintf_s(szBuf, _TRUNCATE, ResourceString(IDS_RESULT_IN), NumberText(nBytesIn).Ptr);
    SetText(IDC_HASH_RESULT, szBuf);
}


void HashDialogBox::SetResultHeader(ULONGLONG nBytesIn, ULONG nBytesOut)
{
    ULONG nBitsOut = nBytesOut * 8;
    WCHAR szBuf[260];
    _snwprintf_s(szBuf, _TRUNCATE, ResourceString(IDS_RESULT_IN_OUT), NumberText(nBytesIn).Ptr, NumberText(nBytesOut).Ptr, NumberText(nBitsOut).Ptr);
    SetText(IDC_HASH_RESULT, szBuf);
}


void HashDialogBox::SetResult(PCWSTR psz)
{
    SetText(IDC_HASH_VALUE, psz);
}


void HashDialogBox::SetResult(Hash& rHash)
{
    m_hash = rHash;
    Buffer<WCHAR> buf(wcslen(m_hash.Text) + 1);
    wcscpy_s(buf, buf.Len, m_hash.Text);
    if (m_bUppercase)
    {
        _wcsupr_s(buf, buf.Len);
    }
    else
    {
        _wcslwr_s(buf, buf.Len);
    }
    SetResult(buf);
}


void HashDialogBox::ResetResultCase()
{
    UINT length = GetTextLength(IDC_HASH_VALUE);
    Buffer<WCHAR> buf(length + 1);
    GetText(IDC_HASH_VALUE, buf, buf.Len);
    if (m_bUppercase)
    {
        _wcsupr_s(buf, buf.Len);
    }
    else
    {
        _wcslwr_s(buf, buf.Len);
    }
    SetResult(buf);
}
