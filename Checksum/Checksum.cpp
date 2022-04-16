#include "pch.h"
#include "Checksum.h"
#include "hnrt/Exception.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ResourceString.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/ByteDataFeeder.h"
#include "hnrt/NumberText.h"
#include "hnrt/Buffer.h"
#include "hnrt/String.h"
#include "hnrt/ErrorMessage.h"


#pragma comment(lib, "Core")


#define LABEL_UTF8 L"UTF-8"
#define LABEL_UTF16LE L"UTF-16 LE"
#define LABEL_CP932 L"CP 932"

#define LABEL_CRLF L"CRLF"
#define LABEL_LF L"LF"


using namespace hnrt;


Checksum::Checksum()
    : m_iExitCode(EXIT_FAILURE)
    , m_hAccelTable(NULL)
    , m_hwnd(NULL)
    , m_hash()
    , m_uMethod(IDC_MD5)
    , m_Width(0)
    , m_Height(0)
    , m_PaddingRight(0)
    , m_PaddingBottom(0)
    , m_LastClientWidth(0)
    , m_LastClientHeight(0)
{
}


Checksum::~Checksum()
{
}


void Checksum::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(lpCmdLine);
    ResourceString::m_hInstance = hInstance;
    m_hwnd = CreateDialogParamW(hInstance, MAKEINTRESOURCE(IDD_CHECKSUM), NULL, ProcessMessage, reinterpret_cast<LPARAM>(this));
    if (!m_hwnd)
    {
        throw Exception(ResourceString(IDS_CRDLG_FAILURE));
    }
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}


void Checksum::Run()
{
    if (!m_hwnd)
    {
        return;
    }
    //m_hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(IDR_ACCELERATOR1));
    while (1)
    {
        MSG msg;
        BOOL bRet = GetMessageW(&msg, NULL, 0, 0);
        if (bRet == -1)
        {
            throw Exception(ResourceString(IDS_GETMSG_FAILURE));
        }
        else if (!bRet)
        {
            m_iExitCode = (int)msg.wParam;
            break;
        }
        /*if (TranslateAcceleratorW(m_hwnd, m_hAccelTable, &msg))
        {
            continue;
        }
        else*/ if (IsDialogMessage(m_hwnd, &msg))
        {
            continue;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}


bool Checksum::ProcessMessages()
{
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            PostQuitMessage(static_cast<int>(msg.wParam));
            return false;
        }
        /*if (TranslateAcceleratorW(m_hwnd, m_hAccelTable, &msg))
        {
            continue;
        }
        else*/ if (IsDialogMessage(m_hwnd, &msg))
        {
            continue;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return true;
}


void Checksum::Close()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
    }
}


INT_PTR CALLBACK Checksum::ProcessMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        reinterpret_cast<Checksum*>(lParam)->OnCreate(hDlg);
        break;

    case WM_DESTROY:
        GetInstance(hDlg)->OnDestory(hDlg);
        break;

    case WM_CLOSE:
        GetInstance(hDlg)->OnExit(hDlg);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_EXIT:
            GetInstance(hDlg)->OnExit(hDlg);
            break;

        case IDC_CALCULATE:
            GetInstance(hDlg)->OnCalculate(hDlg);
            break;

        case IDC_COPY:
            GetInstance(hDlg)->OnCopy(hDlg);
            break;

        case IDC_BROWSE:
            GetInstance(hDlg)->OnBrowse(hDlg);
            break;

        case IDC_FILE:
        case IDC_TEXT:
            GetInstance(hDlg)->OnSelectInput(hDlg, LOWORD(wParam));
            break;

        case IDC_MD5:
        case IDC_SHA1:
        case IDC_SHA256:
        case IDC_SHA384:
        case IDC_SHA512:
            GetInstance(hDlg)->OnSelectMethod(hDlg, LOWORD(wParam));
            break;

        default:
            return (INT_PTR)FALSE;
        }
        break;

    case WM_SIZE:
        GetInstance(hDlg)->OnSize(hDlg, wParam, lParam);
        break;

    default:
        return (INT_PTR)FALSE;
    }

    return (INT_PTR)TRUE;
}


Checksum* Checksum::GetInstance(HWND hDlg)
{
    return reinterpret_cast<Checksum*>(GetWindowLongPtrW(hDlg, GWLP_USERDATA));
}


void Checksum::OnCreate(HWND hDlg)
{
    SetWindowLongPtrW(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    SendDlgItemMessageW(hDlg, IDC_FILE, BM_SETCHECK, BST_CHECKED, 0);
    SendDlgItemMessageW(hDlg, m_uMethod, BM_SETCHECK, BST_CHECKED, 0);
    SendDlgItemMessageW(hDlg, IDC_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L""));
    SendDlgItemMessageW(hDlg, IDC_CHARSET, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(LABEL_UTF8));
    SendDlgItemMessageW(hDlg, IDC_CHARSET, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(LABEL_UTF16LE));
    SendDlgItemMessageW(hDlg, IDC_CHARSET, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(LABEL_CP932));
    SendDlgItemMessageW(hDlg, IDC_CHARSET, CB_SELECTSTRING, 0, reinterpret_cast<LPARAM>(LABEL_UTF8));
    SendDlgItemMessageW(hDlg, IDC_LINEBREAK, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(LABEL_CRLF));
    SendDlgItemMessageW(hDlg, IDC_LINEBREAK, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(LABEL_LF));
    SendDlgItemMessageW(hDlg, IDC_LINEBREAK, CB_SELECTSTRING, 0, reinterpret_cast<LPARAM>(LABEL_CRLF));
    EnableWindow(GetDlgItem(hDlg, IDC_COPY), FALSE);
    OnSelectInput(hDlg, IDC_FILE);
    RECT rectDlg;
    GetWindowRect(hDlg, &rectDlg);
    m_Width = rectDlg.right - rectDlg.left;
    m_Height = rectDlg.bottom - rectDlg.top;
    RECT rectClient;
    GetClientRect(hDlg, &rectClient);
    m_LastClientWidth = rectClient.right;
    m_LastClientHeight = rectClient.bottom;
    RECT rectExit;
    GetWindowRect(GetDlgItem(hDlg, IDC_EXIT), &rectExit);
    POINT pt = { rectExit.right, rectExit.bottom };
    ScreenToClient(hDlg, &pt);
    m_PaddingRight = m_LastClientWidth - pt.x;
    m_PaddingBottom = m_LastClientHeight - pt.y;
}


void Checksum::OnDestory(HWND hDlg)
{
    SetWindowLongPtrW(hDlg, GWLP_USERDATA, 0);
}


void Checksum::OnSize(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    GetWindowRect(hDlg, &rect);
    LONG cx = rect.right - rect.left;
    LONG cy = rect.bottom - rect.top;
    if (cx < m_Width || cy < m_Height)
    {
        SetWindowPos(hDlg, NULL, 0, 0, cx > m_Width ? cx : m_Width, cy > m_Height ? cy : m_Height, SWP_NOMOVE | SWP_NOZORDER);
        return;
    }
    LONG cxClient = LOWORD(lParam);
    LONG cyClient = HIWORD(lParam);
    LONG cxDelta = cxClient - m_LastClientWidth;
    LONG cyDelta = cyClient - m_LastClientHeight;
    m_LastClientWidth = cxClient;
    m_LastClientHeight = cyClient;
    UpdateLayout(hDlg, IDC_EXIT, cxDelta, cyDelta, 0, 0);
    UpdateLayout(hDlg, IDC_CALCULATE, cxDelta, 0, 0, 0);
    UpdateLayout(hDlg, IDC_COPY, cxDelta, 0, 0, 0);
    UpdateLayout(hDlg, IDC_RESULT_BOX, 0, cyDelta, cxDelta, 0);
    UpdateLayout(hDlg, IDC_RESULT, 0, cyDelta, cxDelta, 0, TRUE);
    UpdateLayout(hDlg, IDC_METHOD, cxDelta, 0, 0, cyDelta);
    UpdateLayout(hDlg, IDC_MD5, cxDelta, 0, 0, 0, TRUE);
    UpdateLayout(hDlg, IDC_SHA1, cxDelta, 0, 0, 0, TRUE);
    UpdateLayout(hDlg, IDC_SHA256, cxDelta, 0, 0, 0, TRUE);
    UpdateLayout(hDlg, IDC_SHA384, cxDelta, 0, 0, 0, TRUE);
    UpdateLayout(hDlg, IDC_SHA512, cxDelta, 0, 0, 0, TRUE);
    UpdateLayout(hDlg, IDC_SOURCE, 0, 0, cxDelta, cyDelta);
    UpdateLayout(hDlg, IDC_BROWSE, cxDelta, 0, 0, 0);
    UpdateLayout(hDlg, IDC_CHARSET, cxDelta, 0, 0, 0);
    UpdateLayout(hDlg, IDC_LINEBREAK, cxDelta, 0, 0, 0);
    UpdateLayout(hDlg, IDC_PATH, 0, 0, cxDelta, 0);
    UpdateLayout(hDlg, IDC_CONTENT, 0, 0, cxDelta, cyDelta);
}


void Checksum::UpdateLayout(HWND hDlg, UINT id, LONG dx, LONG dy, LONG dcx, LONG dcy, BOOL bInvalidate)
{
    HWND hwndChild = GetDlgItem(hDlg, id);
    UINT uFlags = SWP_NOZORDER;
    LONG x, y, cx, cy;
    RECT rect;
    GetWindowRect(hwndChild, &rect);
    if (dx || dy)
    {
        POINT pt = { rect.left, rect.top };
        ScreenToClient(hDlg, &pt);
        x = pt.x + dx;
        y = pt.y + dy;
    }
    else
    {
        x = 0;
        y = 0;
        uFlags |= SWP_NOMOVE;
    }
    if (dcx || dcy)
    {
        cx = rect.right - rect.left + dcx;
        cy = rect.bottom - rect.top + dcy;
    }
    else
    {
        cx = 0;
        cy = 0;
        uFlags |= SWP_NOSIZE;
    }
    SetWindowPos(hwndChild, NULL, x, y, cx, cy, uFlags);
    if (bInvalidate)
    {
        InvalidateRect(hwndChild, NULL, TRUE);
    }
}


void Checksum::OnExit(HWND hDlg)
{
    UNREFERENCED_PARAMETER(hDlg);
    PostQuitMessage(EXIT_SUCCESS);
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
        SendDlgItemMessageW(hDlg, IDC_PATH, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(szPath));
    }
}


void Checksum::OnCalculate(HWND hDlg)
{
    UINT uInput = isFile(hDlg) ? IDC_FILE : IDC_TEXT;
    OnSelectInput(hDlg, 0);
    OnSelectMethod(hDlg, 0);
    EnableWindow(GetDlgItem(hDlg, IDC_CALCULATE), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_COPY), FALSE);
    try
    {
        ULONGLONG nBytesIn;
        if (uInput == IDC_FILE)
        {
            WCHAR szPath[MAX_PATH] = { 0 };
            SendDlgItemMessageW(hDlg, IDC_PATH, WM_GETTEXT, MAX_PATH, reinterpret_cast<LPARAM>(szPath));
            FileDataFeederEx feeder(szPath, hDlg);
            Calculate(hDlg, feeder);
            nBytesIn = feeder.TotalLength;
        }
        else
        {
            int length2 = static_cast<int>(SendDlgItemMessageW(hDlg, IDC_CONTENT, WM_GETTEXTLENGTH, 0, 0));
            Buffer<WCHAR> buf2(length2 + 1);
            SendDlgItemMessageW(hDlg, IDC_CONTENT, WM_GETTEXT, static_cast<WPARAM>(buf2.Len), reinterpret_cast<LPARAM>(&buf2));
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
        SetResultHeader(hDlg, nBytesIn, m_hash.ValueLength);
        EnableWindow(GetDlgItem(hDlg, IDC_COPY), TRUE);
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
    EnableWindow(GetDlgItem(hDlg, IDC_CALCULATE), TRUE);
    OnSelectInput(hDlg, uInput);
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
    memcpy_s(GlobalLock(hMem), cbLen, m_hash.Text, cbLen);
    GlobalUnlock(hMem);
    OpenClipboard(hDlg);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, hMem);
    CloseClipboard();
}


void Checksum::OnSelectInput(HWND hDlg, UINT uInput)
{
    BOOL bRadio = uInput ? TRUE : FALSE;
    BOOL bFile = uInput == IDC_FILE ? TRUE : FALSE;
    BOOL bText = uInput == IDC_TEXT ? TRUE : FALSE;
    EnableWindow(GetDlgItem(hDlg, IDC_FILE), bRadio);
    EnableWindow(GetDlgItem(hDlg, IDC_PATH), bFile);
    EnableWindow(GetDlgItem(hDlg, IDC_BROWSE), bFile);
    EnableWindow(GetDlgItem(hDlg, IDC_TEXT), bRadio);
    EnableWindow(GetDlgItem(hDlg, IDC_CONTENT), bText);
    EnableWindow(GetDlgItem(hDlg, IDC_CHARSET), bText);
    EnableWindow(GetDlgItem(hDlg, IDC_LINEBREAK), bText);
}


void Checksum::OnSelectMethod(HWND hDlg, UINT uMethod)
{
    BOOL bRadio = uMethod ? TRUE : FALSE;
    EnableWindow(GetDlgItem(hDlg, IDC_MD5), bRadio);
    EnableWindow(GetDlgItem(hDlg, IDC_SHA1), bRadio);
    EnableWindow(GetDlgItem(hDlg, IDC_SHA256), bRadio);
    EnableWindow(GetDlgItem(hDlg, IDC_SHA384), bRadio);
    EnableWindow(GetDlgItem(hDlg, IDC_SHA512), bRadio);
    if (uMethod && uMethod != m_uMethod)
    {
        m_uMethod = uMethod;
        m_hash.Close();
        SetResultHeader(hDlg);
        SetResult(hDlg);
    }
}


void Checksum::Calculate(HWND hDlg, DataFeeder& rDataFeeder)
{
    if (isMD5(hDlg))
    {
        MD5Hash hash(rDataFeeder);
        SetResult(hDlg, hash);
    }
    else if (isSHA1(hDlg))
    {
        SHA1Hash hash(rDataFeeder);
        SetResult(hDlg, hash);
    }
    else if (isSHA256(hDlg))
    {
        SHA256Hash hash(rDataFeeder);
        SetResult(hDlg, hash);
    }
    else if (isSHA384(hDlg))
    {
        SHA384Hash hash(rDataFeeder);
        SetResult(hDlg, hash);
    }
    else if (isSHA512(hDlg))
    {
        SHA512Hash hash(rDataFeeder);
        SetResult(hDlg, hash);
    }
}


bool Checksum::isFile(HWND hDlg) const
{
    return SendDlgItemMessageW(hDlg, IDC_FILE, BM_GETCHECK, 0, 0) == BST_CHECKED;
}


bool Checksum::isMD5(HWND hDlg) const
{
    return SendDlgItemMessageW(hDlg, IDC_MD5, BM_GETCHECK, 0, 0) == BST_CHECKED;
}


bool Checksum::isSHA1(HWND hDlg) const
{
    return SendDlgItemMessageW(hDlg, IDC_SHA1, BM_GETCHECK, 0, 0) == BST_CHECKED;
}


bool Checksum::isSHA256(HWND hDlg) const
{
    return SendDlgItemMessageW(hDlg, IDC_SHA256, BM_GETCHECK, 0, 0) == BST_CHECKED;
}


bool Checksum::isSHA384(HWND hDlg) const
{
    return SendDlgItemMessageW(hDlg, IDC_SHA384, BM_GETCHECK, 0, 0) == BST_CHECKED;
}


bool Checksum::isSHA512(HWND hDlg) const
{
    return SendDlgItemMessageW(hDlg, IDC_SHA512, BM_GETCHECK, 0, 0) == BST_CHECKED;
}


UINT Checksum::GetCodePage(HWND hDlg)
{
    LRESULT SelectedIndex = SendDlgItemMessageW(hDlg, IDC_CHARSET, CB_GETCURSEL, 0, 0);
    if (SelectedIndex == CB_ERR)
    {
        SelectedIndex = 0;
    }
    WCHAR szCharset[64] = { 0 };
    if (SendDlgItemMessageW(hDlg, IDC_CHARSET, CB_GETLBTEXT, SelectedIndex, reinterpret_cast<LPARAM>(szCharset)) == CB_ERR)
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
    LRESULT SelectedIndex = SendDlgItemMessageW(hDlg, IDC_LINEBREAK, CB_GETCURSEL, 0, 0);
    if (SelectedIndex == CB_ERR)
    {
        SelectedIndex = 0;
    }
    WCHAR szLineBreak[64] = { 0 };
    if (SendDlgItemMessageW(hDlg, IDC_LINEBREAK, CB_GETLBTEXT, SelectedIndex, reinterpret_cast<LPARAM>(szLineBreak)) == CB_ERR)
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


void Checksum::SetResultHeader(HWND hDlg)
{
    SendDlgItemMessageW(m_hwnd, IDC_RESULT_BOX, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(ResourceString(IDS_RESULT).Ptr));
}


void Checksum::SetResultHeader(HWND hDlg, ULONGLONG nBytesIn)
{
    WCHAR szBuf[260];
    _snwprintf_s(szBuf, _TRUNCATE, ResourceString(IDS_RESULT_IN), NumberText(nBytesIn).Ptr);
    SendDlgItemMessageW(m_hwnd, IDC_RESULT_BOX, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(szBuf));
}


void Checksum::SetResultHeader(HWND hDlg, ULONGLONG nBytesIn, ULONG nBytesOut)
{
    ULONG nBitsOut = nBytesOut * 8;
    WCHAR szBuf[260];
    _snwprintf_s(szBuf, _TRUNCATE, ResourceString(IDS_RESULT_IN_OUT), NumberText(nBytesIn).Ptr, NumberText(nBytesOut).Ptr, NumberText(nBitsOut).Ptr);
    SendDlgItemMessageW(m_hwnd, IDC_RESULT_BOX, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(szBuf));
}


void Checksum::SetResult(HWND hDlg)
{
    SendDlgItemMessageW(hDlg, IDC_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L""));
}


void Checksum::SetResult(HWND hDlg, Hash& rHash)
{
    m_hash = rHash;
    SendDlgItemMessageW(hDlg, IDC_RESULT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(m_hash.Text));
}


FileDataFeederEx::FileDataFeederEx(PCWSTR pszFileName, HWND hDlg)
    : FileDataFeeder(pszFileName)
    , m_hDlg(hDlg)
    , m_LastTick(0ULL)
{
}


bool FileDataFeederEx::HasNext()
{
    bool bRet = FileDataFeeder::HasNext();
    ULONGLONG tick = GetTickCount64();
    if (m_LastTick + 100 <= tick || !bRet)
    {
        m_LastTick = tick;
        Checksum& app = *Checksum::GetInstance(m_hDlg);
        app.SetResultHeader(m_hDlg, TotalLength);
        if (!app.ProcessMessages())
        {
            return false;
        }
    }
    return bRet;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    Checksum app;
    try
    {
        _wsetlocale(LC_ALL, L"");
        app.Open(hInstance, lpCmdLine, nCmdShow);
        app.Run();
        app.Close();
    }
    catch (Exception e)
    {
        MessageBoxW(NULL, e.Message, ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        MessageBoxW(NULL, ResourceString(IDS_UNKNOWN_FAILURE), ResourceString(IDS_CAPTION), MB_OK | MB_ICONERROR);
    }
    return app.ExitCode;
}
