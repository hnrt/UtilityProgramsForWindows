﻿#include "pch.h"
#include "Checksum.h"
#include "FileDataFeederEx.h"
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
#include "hnrt/Debug.h"


#define REG_SUBKEY L"SOFTWARE\\hnrt\\Checksum"
#define REG_NAME_SOURCE L"Source"
#define REG_NAME_METHOD L"Method"
#define REG_NAME_UPPERCASE L"Uppercase"


#define LABEL_UTF8 L"UTF-8"
#define LABEL_UTF16LE L"UTF-16 LE"
#define LABEL_CP932 L"CP 932"

#define LABEL_CRLF L"CRLF"
#define LABEL_LF L"LF"


using namespace hnrt;


Checksum::Checksum()
    : DialogApp(IDD_CHECKSUM)
    , m_hash()
    , m_uSource(IDC_FILE)
    , m_uMethod(IDC_MD5)
    , m_bUppercase(FALSE)
{
}


void Checksum::OnCreate()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Open(HKEY_CURRENT_USER, REG_SUBKEY);
    if (rc == ERROR_SUCCESS)
    {
        RegistryValue value;
        m_uSource = value.GetDWORD(hKey, REG_NAME_SOURCE, 1) == 2 ? IDC_TEXT : IDC_FILE;
        m_uMethod = ((value.GetDWORD(hKey, REG_NAME_METHOD, 1) - 1) % 5) + IDC_MD5;
        m_bUppercase = value.GetDWORD(hKey, REG_NAME_UPPERCASE) ? TRUE : FALSE;
    }
    SendMessage(m_uSource, BM_SETCHECK, BST_CHECKED, 0);
    SendMessage(m_uMethod, BM_SETCHECK, BST_CHECKED, 0);
    SendMessage(IDC_UPPERCASE, BM_SETCHECK, m_bUppercase ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(IDC_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L""));
    SendMessage(IDC_CHARSET, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(LABEL_UTF8));
    SendMessage(IDC_CHARSET, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(LABEL_UTF16LE));
    SendMessage(IDC_CHARSET, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(LABEL_CP932));
    SendMessage(IDC_CHARSET, CB_SELECTSTRING, 0, reinterpret_cast<LPARAM>(LABEL_UTF8));
    SendMessage(IDC_LINEBREAK, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(LABEL_CRLF));
    SendMessage(IDC_LINEBREAK, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(LABEL_LF));
    SendMessage(IDC_LINEBREAK, CB_SELECTSTRING, 0, reinterpret_cast<LPARAM>(LABEL_CRLF));
    DisableWindow(IDC_COPY);
    OnSelectSource(hwnd, m_uSource);
}


void Checksum::OnDestroy()
{
    RegistryKey hKey;
    LSTATUS rc = hKey.Create(HKEY_CURRENT_USER, REG_SUBKEY);
    if (rc == ERROR_SUCCESS)
    {
        rc = RegistryValue::SetDWORD(hKey, REG_NAME_SOURCE, m_uSource == IDC_FILE ? 1 : 2);
        if (rc != ERROR_SUCCESS)
        {
            Debug::Put(L"Failed to set DWORD to HKCU\\%s\\%s: %s", REG_SUBKEY, REG_NAME_SOURCE, ErrorMessage::Get(rc));
        }
        rc = RegistryValue::SetDWORD(hKey, REG_NAME_METHOD, m_uMethod - IDC_MD5 + 1);
        if (rc != ERROR_SUCCESS)
        {
            Debug::Put(L"Failed to set DWORD to HKCU\\%s\\%s: %s", REG_SUBKEY, REG_NAME_METHOD, ErrorMessage::Get(rc));
        }
        rc = RegistryValue::SetDWORD(hKey, REG_NAME_UPPERCASE, m_bUppercase ? 1UL : 0UL);
        if (rc != ERROR_SUCCESS)
        {
            Debug::Put(L"Failed to set DWORD to HKCU\\%s\\%s: %s", REG_SUBKEY, REG_NAME_UPPERCASE, ErrorMessage::Get(rc));
        }
    }
    else
    {
        Debug::Put(L"Failed to create HKCU\\%s: %s", REG_SUBKEY, ErrorMessage::Get(rc));
    }
}


void Checksum::UpdateLayout(HWND hDlg, LONG cxDelta, LONG cyDelta)
{
    WindowLayout::UpdateLayout(hDlg, IDC_EXIT, cxDelta, cyDelta, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_CALCULATE, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_COPY, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_RESULT_BOX, 0, cyDelta, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_RESULT, 0, cyDelta, cxDelta, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_METHOD, cxDelta, 0, 0, cyDelta, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_MD5, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_SHA1, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_SHA256, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_SHA384, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_SHA512, cxDelta, 0, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_FORMAT, cxDelta, cyDelta, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_UPPERCASE, cxDelta, cyDelta, 0, 0, TRUE);
    WindowLayout::UpdateLayout(hDlg, IDC_SOURCE, 0, 0, cxDelta, cyDelta);
    WindowLayout::UpdateLayout(hDlg, IDC_BROWSE, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_CHARSET, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_LINEBREAK, cxDelta, 0, 0, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_PATH, 0, 0, cxDelta, 0);
    WindowLayout::UpdateLayout(hDlg, IDC_CONTENT, 0, 0, cxDelta, cyDelta);
}


INT_PTR Checksum::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    UINT idChild = LOWORD(wParam);
    switch (idChild)
    {
    case IDC_EXIT:
        OnClose();
        break;

    case IDC_CALCULATE:
        OnCalculate(hwnd);
        break;

    case IDC_COPY:
        OnCopy(hwnd);
        break;

    case IDC_BROWSE:
        OnBrowse(hwnd);
        break;

    case IDC_FILE:
    case IDC_TEXT:
        OnSelectSource(hwnd, idChild);
        break;

    case IDC_MD5:
    case IDC_SHA1:
    case IDC_SHA256:
    case IDC_SHA384:
    case IDC_SHA512:
        OnSelectMethod(hwnd, idChild);
        break;

    case IDC_UPPERCASE:
        OnUppercase(hwnd);
        break;

    default:
        return FALSE;
    }
    return TRUE;
}


void Checksum::OnBrowse(HWND hDlg)
{
    WCHAR szPath[MAX_PATH] = { 0 };
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hDlg;
    ofn.lpstrFile = szPath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameW(&ofn))
    {
        SendMessage(IDC_PATH, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(szPath));
    }
}


void Checksum::OnCalculate(HWND hDlg)
{
    OnSelectSource(hDlg, 0);
    OnSelectMethod(hDlg, 0);
    DisableWindow(IDC_CALCULATE);
    DisableWindow(IDC_COPY);
    try
    {
        ULONGLONG nBytesIn;
        if (m_uSource == IDC_FILE)
        {
            WCHAR szPath[MAX_PATH] = { 0 };
            SendMessage(IDC_PATH, WM_GETTEXT, MAX_PATH, reinterpret_cast<LPARAM>(szPath));
            FileDataFeederEx feeder(szPath, hDlg);
            Calculate(hDlg, feeder);
            nBytesIn = feeder.TotalLength;
        }
        else
        {
            int length2 = static_cast<int>(SendMessage(IDC_CONTENT, WM_GETTEXTLENGTH, 0, 0));
            Buffer<WCHAR> buf2(length2 + 1);
            SendMessage(IDC_CONTENT, WM_GETTEXT, static_cast<WPARAM>(buf2.Len), reinterpret_cast<LPARAM>(&buf2));
            UINT uLineBreak = GetLineBreak(hDlg);
            if (uLineBreak == 0x0a && length2 > 0)
            {
                length2 = ConvertToLF(&buf2, length2);
            }
            UINT uCodePage = GetCodePage(hDlg);
            if (uCodePage == 1200)
            {
                length2 *= sizeof(WCHAR);
                ByteDataFeeder feeder(&buf2, length2);
                Calculate(hDlg, feeder);
                nBytesIn = length2;
            }
            else
            {
                int length1 = static_cast<int>(length2 * 4);
                Buffer<CHAR> buf1(length1);
                length1 = WideCharToMultiByte(uCodePage, 0, &buf2, length2, &buf1, length1, NULL, NULL);
                ByteDataFeeder feeder(&buf1, length1);
                Calculate(hDlg, feeder);
                nBytesIn = length1;
            }
        }
        SetResultHeader(nBytesIn, m_hash.ValueLength);
        EnableWindow(IDC_COPY);
    }
    catch (Win32Exception e)
    {
        static const WCHAR szCreateFile[] = L"CreateFile";
        static const WCHAR szReadFile[] = L"ReadFile";
        if (!wcsncmp(e.Message, szCreateFile, wcslen(szCreateFile)))
        {
            MessageBox(hDlg, String::Format(ResourceString(IDS_OPENFILE_FAILURE), ErrorMessage::Get(e.Error)), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
        }
        else if (!wcsncmp(e.Message, szReadFile, wcslen(szReadFile)))
        {
            MessageBox(hDlg, String::Format(ResourceString(IDS_READFILE_FAILURE), ErrorMessage::Get(e.Error)), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
        }
        else
        {
            MessageBox(hDlg, ErrorMessage::Get(e.Error), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
        }
    }
    catch (Exception e)
    {
        MessageBox(hDlg, e.Message, ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    EnableWindow(IDC_CALCULATE);
    OnSelectSource(hDlg, m_uSource);
    OnSelectMethod(hDlg, m_uMethod);
}


void Checksum::OnCopy(HWND hDlg)
{
    SIZE_T cbLen = (wcslen(m_hash.Text) + 1) * sizeof(WCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, cbLen);
    if (hMem == NULL)
    {
        MessageBoxW(hDlg, L"Failed to copy text to Clipboard.", ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
        return;
    }
    Buffer<WCHAR> buf(cbLen / sizeof(WCHAR));
    wcscpy_s(buf, buf.Len, m_hash.Text);
    if (m_bUppercase)
    {
        _wcsupr_s(buf, buf.Len);
    }
    else
    {
        _wcslwr_s(buf, buf.Len);
    }
    memcpy_s(GlobalLock(hMem), cbLen, buf.Ptr, cbLen);
    GlobalUnlock(hMem);
    OpenClipboard(hDlg);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, hMem);
    CloseClipboard();
}


void Checksum::OnSelectSource(HWND hDlg, UINT uSource)
{
    BOOL bRadio = uSource ? TRUE : FALSE;
    BOOL bFile = uSource == IDC_FILE ? TRUE : FALSE;
    BOOL bText = uSource == IDC_TEXT ? TRUE : FALSE;
    EnableWindow(IDC_FILE, bRadio);
    EnableWindow(IDC_PATH, bFile);
    EnableWindow(IDC_BROWSE, bFile);
    EnableWindow(IDC_TEXT, bRadio);
    EnableWindow(IDC_CONTENT, bText);
    EnableWindow(IDC_CHARSET, bText);
    EnableWindow(IDC_LINEBREAK, bText);
    if (uSource)
    {
        m_uSource = uSource;
    }
}


void Checksum::OnSelectMethod(HWND hDlg, UINT uMethod)
{
    BOOL bRadio = uMethod ? TRUE : FALSE;
    EnableWindow(IDC_MD5, bRadio);
    EnableWindow(IDC_SHA1, bRadio);
    EnableWindow(IDC_SHA256, bRadio);
    EnableWindow(IDC_SHA384, bRadio);
    EnableWindow(IDC_SHA512, bRadio);
    if (uMethod && uMethod != m_uMethod)
    {
        m_uMethod = uMethod;
        m_hash.Close();
        SetResultHeader();
        SetResult();
    }
}


void Checksum::OnUppercase(HWND hDlg)
{
    LRESULT value = SendMessage(IDC_UPPERCASE, BM_GETCHECK, 0, 0);
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


void Checksum::Calculate(HWND hDlg, DataFeeder& rDataFeeder)
{
    switch (m_uMethod)
    {
    case IDC_MD5:
    {
        MD5Hash hash(rDataFeeder);
        SetResult(hash);
        break;
    }
    case IDC_SHA1:
    {
        SHA1Hash hash(rDataFeeder);
        SetResult(hash);
        break;
    }
    case IDC_SHA256:
    {
        SHA256Hash hash(rDataFeeder);
        SetResult(hash);
        break;
    }
    case IDC_SHA384:
    {
        SHA384Hash hash(rDataFeeder);
        SetResult(hash);
        break;
    }
    case IDC_SHA512:
    {
        SHA512Hash hash(rDataFeeder);
        SetResult(hash);
        break;
    }
    default:
        break;
    }
}


UINT Checksum::GetCodePage(HWND hDlg)
{
    LRESULT SelectedIndex = SendMessage(IDC_CHARSET, CB_GETCURSEL, 0, 0);
    if (SelectedIndex == CB_ERR)
    {
        SelectedIndex = 0;
    }
    WCHAR szCharset[64] = { 0 };
    if (SendMessage(IDC_CHARSET, CB_GETLBTEXT, SelectedIndex, reinterpret_cast<LPARAM>(szCharset)) == CB_ERR)
    {
        wcscpy_s(szCharset, LABEL_UTF8);
    }
    if (!wcscmp(szCharset, LABEL_UTF8))
    {
        return CP_UTF8;
    }
    else if (!wcscmp(szCharset, LABEL_UTF16LE))
    {
        return 1200;
    }
    else if (!wcscmp(szCharset, LABEL_CP932))
    {
        return 932;
    }
    else
    {
        return CP_UTF8;
    }
}


UINT Checksum::GetLineBreak(HWND hDlg)
{
    LRESULT SelectedIndex = SendMessage(IDC_LINEBREAK, CB_GETCURSEL, 0, 0);
    if (SelectedIndex == CB_ERR)
    {
        SelectedIndex = 0;
    }
    WCHAR szLineBreak[64] = { 0 };
    if (SendMessage(IDC_LINEBREAK, CB_GETLBTEXT, SelectedIndex, reinterpret_cast<LPARAM>(szLineBreak)) == CB_ERR)
    {
        wcscpy_s(szLineBreak, LABEL_UTF8);
    }
    if (!wcscmp(szLineBreak, LABEL_CRLF))
    {
        return 0x0d0a;
    }
    else if (!wcscmp(szLineBreak, LABEL_LF))
    {
        return 0x0a;
    }
    else
    {
        return 0x0d0a;
    }
}


UINT Checksum::ConvertToLF(PWCHAR pStart, UINT uLength)
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


void Checksum::SetResultHeader()
{
    SendDlgItemMessageW(hwnd, IDC_RESULT_BOX, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(ResourceString(IDS_RESULT).Ptr));
}


void Checksum::SetResultHeader(ULONGLONG nBytesIn)
{
    WCHAR szBuf[260];
    _snwprintf_s(szBuf, _TRUNCATE, ResourceString(IDS_RESULT_IN), NumberText(nBytesIn).Ptr);
    SendDlgItemMessageW(hwnd, IDC_RESULT_BOX, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(szBuf));
}


void Checksum::SetResultHeader(ULONGLONG nBytesIn, ULONG nBytesOut)
{
    ULONG nBitsOut = nBytesOut * 8;
    WCHAR szBuf[260];
    _snwprintf_s(szBuf, _TRUNCATE, ResourceString(IDS_RESULT_IN_OUT), NumberText(nBytesIn).Ptr, NumberText(nBytesOut).Ptr, NumberText(nBitsOut).Ptr);
    SendDlgItemMessageW(hwnd, IDC_RESULT_BOX, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(szBuf));
}


void Checksum::SetResult(PCWSTR psz)
{
    SendMessage(IDC_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(psz));
}


void Checksum::SetResult(Hash& rHash)
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


void Checksum::ResetResultCase()
{
    LRESULT length = SendMessage(IDC_RESULT, WM_GETTEXTLENGTH, 0, 0);
    if (length <= 0)
    {
        return;
    }
    Buffer<WCHAR> buf(length + 1);
    SendMessage(IDC_RESULT, WM_GETTEXT, buf.Len, reinterpret_cast<LPARAM>(buf.Ptr));
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
