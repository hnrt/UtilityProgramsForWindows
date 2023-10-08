#include "pch.h"
#include "hnrt/AnyApp.h"
#include "hnrt/Exception.h"
#include "hnrt/Interlocked.h"
#include "hnrt/CommandLine.h"


#pragma comment(lib,"Core")


using namespace hnrt;


AnyApp* AnyApp::m_pThis = nullptr;


AnyApp* AnyApp::GetAppPtr()
{
    return m_pThis;
}


AnyApp::AnyApp()
	: m_bExit(false)
    , m_iExitCode(EXIT_FAILURE)
    , m_pCommandLine(nullptr)
	, m_hAccelTable(nullptr)
{
    if (Interlocked<AnyApp*>::CompareExchangePointer(&m_pThis, this, nullptr) != nullptr)
    {
        throw Exception(L"Unable to create another AnyApp instance.");
    }
}


AnyApp::~AnyApp()
{
    Interlocked<AnyApp*>::CompareExchangePointer(&m_pThis, nullptr, this);
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
    while (!m_bExit)
    {
        MSG msg;
        BOOL bRet = GetMessageW(&msg, NULL, 0, 0);
        switch (bRet)
        {
        case 0: // WM_QUIT
            m_bExit = true;
            m_iExitCode = static_cast<int>(msg.wParam);
            break;
        case -1: // ERROR
            throw Exception(L"GetMessage failed.");
        default:
            ProcessMessage(&msg);
            break;
        }
    }
}


int AnyApp::TryProcessMessage()
{
    MSG msg;
    if (!PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        return 0;
    }
    else if (msg.message == WM_QUIT)
    {
        m_bExit = true;
        m_iExitCode = static_cast<int>(msg.wParam);
        return -1;
    }
    else
    {
        ProcessMessage(&msg);
        return 1;
    }
}
