#include "pch.h"
#include "hnrt/Exception.h"
#include "hnrt/CommandLine.h"
#include "hnrt/AnyApp.h"


#pragma comment(lib,"Core")


using namespace hnrt;


AnyApp::AnyApp()
	: m_iExitCode(EXIT_FAILURE)
    , m_pCommandLine(nullptr)
	, m_hAccelTable(nullptr)
	, m_hwnd(nullptr)
{
}


AnyApp::~AnyApp()
{
    delete m_pCommandLine;
}


void AnyApp::SetAccelerators(HINSTANCE hInstance, UINT id)
{
	m_hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(id));
}


static const WCHAR szDPIAWARENESS[] = { L"-dpiawareness=" };


void AnyApp::Open(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(nCmdShow);
    m_pCommandLine = new CommandLine(lpCmdLine);
    CommandLineIterator iter(*m_pCommandLine);
    while (iter.HasNext)
    {
        // accepts the manifest dpiAwareness value
        if (!wcsncmp(iter.Next, szDPIAWARENESS, wcslen(szDPIAWARENESS)))
        {
            static bool specified = false;
            if (specified)
            {
                throw Exception(L"Bad command line: %.*s: Specified more than once.", (int)(wcslen(szDPIAWARENESS) - 1), szDPIAWARENESS);
            }
            specified = true;
            PCWSTR pszValue = iter.Next + wcslen(szDPIAWARENESS);
            if (!_wcsicmp(pszValue, L"unaware"))
            {
                SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED);
            }
            else if (!_wcsicmp(pszValue, L"system"))
            {
                SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
            }
            else if (!_wcsicmp(pszValue, L"permonitor"))
            {
                SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
            }
            else if (!_wcsicmp(pszValue, L"permonitorv2"))
            {
                SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            }
            else
            {
                throw Exception(L"Bad command line: %.*s: Bad value.", (int)(wcslen(szDPIAWARENESS) - 1), szDPIAWARENESS);
            }
            iter.RemoveNext();
        }
    }
}


void AnyApp::Run()
{
    if (!m_hwnd)
    {
        return;
    }
    while (1)
    {
        MSG msg;
        BOOL bRet = GetMessageW(&msg, NULL, 0, 0);
        if (bRet == -1)
        {
            throw Exception(L"GetMessage failed.");
        }
        else if (!bRet)
        {
            m_iExitCode = static_cast<int>(msg.wParam);
            break;
        }
        ProcessMessage(&msg);
    }
}


int AnyApp::TryProcessMessage()
{
    MSG msg;
    if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            PostQuitMessage(static_cast<int>(msg.wParam));
            return -1;
        }
        ProcessMessage(&msg);
        return 1;
    }
    else
    {
        return 0;
    }
}
