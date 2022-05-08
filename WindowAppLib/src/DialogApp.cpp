#include "pch.h"
#include "hnrt/ResourceString.h"
#include "hnrt/Exception.h"
#include "hnrt/Interlocked.h"
#include "hnrt/CommandLine.h"
#include "hnrt/LogicalFont.h"
#include "hnrt/String.h"
#include "hnrt/DialogApp.h"


using namespace hnrt;


DialogApp::DialogApp(UINT idTemplate)
    : AnyApp()
    , WindowSize()
    , WindowLayout()
    , m_idTemplate(idTemplate)
    , m_hFont()
{
}


static const WCHAR szFONT[] = { L"-font=" };


static BOOL CALLBACK ApplyFontToChild(HWND hwnd, LPARAM lParam)
{
    SendMessageW(hwnd, WM_SETFONT, static_cast<WPARAM>(lParam), TRUE);
    return TRUE;
}


void DialogApp::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    AnyApp::Open(hInstance, lpCmdLine, nCmdShow);
    m_hwnd = CreateDialogParamW(hInstance, MAKEINTRESOURCE(m_idTemplate), NULL, MessageCallback, reinterpret_cast<LPARAM>(this));
    if (!m_hwnd)
    {
        throw Exception(L"CreateDialog failed.");
    }
    CommandLineIterator iter(*m_pCommandLine);
    while (iter.HasNext)
    {
        if (!wcsncmp(iter.Next, szFONT, wcslen(szFONT)))
        {
            PCWSTR pszValue = iter.Next + wcslen(szFONT);
            const WCHAR* pSep = wcschr(pszValue, L':');
            if (!pSep)
            {
                throw Exception(L"Bad command line: %.*s: No point size.", (int)(wcslen(szFONT) - 1), szFONT);
            }
            if (pSep <= pszValue)
            {
                throw Exception(L"Bad command line: %.*s: No face name.", (int)(wcslen(szFONT) - 1), szFONT);
            }
            int pointSize = (int)wcstol(pSep + 1, nullptr, 10);
            if (pointSize < 6 || 100 < pointSize)
            {
                throw Exception(L"Bad command line: %.*s: Bad point size.", (int)(wcslen(szFONT) - 1), szFONT);
            }
            m_hFont = LogicalFont()
                .SetFaceName(String::Copy(pszValue, pSep - pszValue))
                .SetHeight(pointSize, m_hwnd)
                .SetJapaneseCharSet()
                .Create();
            if (m_hFont)
            {
                EnumChildWindows(m_hwnd, ApplyFontToChild, reinterpret_cast<LPARAM>((HFONT)m_hFont));
            }
            iter.RemoveNext();
        }
    }
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}


void DialogApp::Close()
{
    HWND hwnd = Interlocked<HWND>::ExchangePointer(&m_hwnd, nullptr);
    if (hwnd)
    {
        DestroyWindow(hwnd);
    }
}


void DialogApp::ProcessMessage(MSG* pMsg)
{
    if (m_hAccelTable && TranslateAcceleratorW(m_hwnd, m_hAccelTable, pMsg))
    {
        // OK
    }
    else if (IsDialogMessageW(m_hwnd, pMsg))
    {
        // OK
    }
    else
    {
        TranslateMessage(pMsg);
        DispatchMessageW(pMsg);
    }
}


INT_PTR CALLBACK DialogApp::MessageCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        DialogApp* pThis = reinterpret_cast<DialogApp*>(lParam);
        SetWindowLongPtrW(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        pThis->InitializeSize(hDlg);
        pThis->OnCreate(hDlg);
        break;
    }
    case WM_DESTROY:
        GetInstance(hDlg)->OnDestroy(hDlg);
        SetWindowLongPtrW(hDlg, GWLP_USERDATA, 0);
        break;
    case WM_CLOSE:
        GetInstance(hDlg)->OnClose(hDlg);
        break;
    case WM_SIZE:
        GetInstance(hDlg)->OnSize(hDlg, wParam, lParam);
        break;
    case WM_COMMAND:
        return GetInstance(hDlg)->OnCommand(hDlg, wParam, lParam);
    case WM_TIMER:
        return GetInstance(hDlg)->OnTimer(hDlg, wParam, lParam);
    default:
        return FALSE;
    }
    return TRUE;
}


DialogApp* DialogApp::GetInstance(HWND hDlg)
{
    return reinterpret_cast<DialogApp*>(GetWindowLongPtrW(hDlg, GWLP_USERDATA));
}


void DialogApp::OnCreate(HWND hDlg)
{
}


void DialogApp::OnDestroy(HWND hDlg)
{
}


void DialogApp::OnClose(HWND hDlg)
{
    UNREFERENCED_PARAMETER(hDlg);
    PostQuitMessage(EXIT_SUCCESS);
}


void DialogApp::OnSize(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    WindowSize::OnSize(hDlg, wParam, lParam, *this);
}


INT_PTR DialogApp::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}


INT_PTR DialogApp::OnTimer(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}
