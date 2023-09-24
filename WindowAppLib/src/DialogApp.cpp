#include "pch.h"
#include "hnrt/DialogApp.h"
#include "hnrt/Exception.h"
#include "hnrt/CommandLine.h"
#include "hnrt/LogicalFont.h"
#include "hnrt/String.h"


using namespace hnrt;


DialogApp::DialogApp(UINT idTemplate)
    : DialogBox(idTemplate)
    , AnyApp()
{
}


static const WCHAR szFONT[] = { L"-font=" };


void DialogApp::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    AnyApp::Open(hInstance, lpCmdLine, nCmdShow);
    DialogBox::Open(hInstance);
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
            SetFont(
                LogicalFont()
                .SetFaceName(String::Copy(pszValue, pSep - pszValue))
                .SetHeight(pointSize, hwnd)
                .SetJapaneseCharSet()
                .Create());
            iter.RemoveNext();
        }
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
}


void DialogApp::SetText(PCWSTR psz)
{
    SetWindowTextW(hwnd, psz);
}


void DialogApp::ProcessMessage(MSG* pMsg)
{
    if (m_hAccelTable && TranslateAcceleratorW(hwnd, m_hAccelTable, pMsg))
    {
        // OK
    }
    else if (IsDialogMessageW(hwnd, pMsg))
    {
        // OK
    }
    else
    {
        TranslateMessage(pMsg);
        DispatchMessageW(pMsg);
    }
}


void DialogApp::OnClose()
{
    PostQuitMessage(EXIT_SUCCESS);
}
