#include "pch.h"
#include "hnrt/AnyApp.h"
#include "hnrt/Exception.h"


#pragma comment(lib,"Core")


using namespace hnrt;


AnyApp::AnyApp()
	: m_iExitCode(EXIT_FAILURE)
	, m_hAccelTable(nullptr)
	, m_hwnd(nullptr)
{
}


void AnyApp::SetAccelerators(HINSTANCE hInstance, UINT id)
{
	m_hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(id));
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
