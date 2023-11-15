#include "pch.h"
#include "Client.h"
#include "Server.h"
#include "ServerWindow.h"
#include "Agent.h"
#include "hnrt/KeyboardMouse.h"
#include "hnrt/CoreLibrary.h"
#include "hnrt/Debug.h"
#include "hnrt/Exception.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ComException.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/String.h"
#include "hnrt/WindowsPlatform.h"


#pragma comment(lib, "Core")


#define MSG_UNHANDLED_EXCEPTION L"ERROR: Unhandled exception caught."
#define MSG_OUT_OF_MEMORY L"ERROR: Out of memory."
#define FMT_EXCEPTION L"ERROR: %s"
#define FMT_WIN32EXCEPTION L"ERROR: %s (%ld: %s)"
#define FMT_COMEXCEPTION L"ERROR: %s (HRESULT: %08lx)"


using namespace hnrt;


static CoreLibrary theCore;


HMODULE g_hModule;
DWORD g_dwTlsIndex = TLS_OUT_OF_INDEXES;
WCHAR g_szDllFileName[MAX_PATH];
WCHAR g_szExeFileName[MAX_PATH];
Client* g_pClient;
Server* g_pServer;


static void ReleaseAgent()
{
    DBGFNC(L"KeyboardMouse%d::ReleaseAgent", BITNESS);
    Agent* pAgent = reinterpret_cast<Agent*>(TlsGetValue(g_dwTlsIndex));
    if (pAgent)
    {
        TlsSetValue(g_dwTlsIndex, nullptr);
        try
        {
            pAgent->Uninitialize();
            pAgent->Release();
        }
        catch (ComException ex)
        {
            Debug::Put(FMT_COMEXCEPTION, ex.Message, ex.Result);
        }
        catch (Win32Exception ex)
        {
            Debug::Put(FMT_WIN32EXCEPTION, ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
        }
        catch (Exception ex)
        {
            Debug::Put(FMT_EXCEPTION, ex.Message);
        }
        catch (std::bad_alloc)
        {
            Debug::Put(MSG_OUT_OF_MEMORY);
        }
        catch (...)
        {
            Debug::Put(MSG_UNHANDLED_EXCEPTION);
        }
    }
}


BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        g_hModule = hModule;
        g_dwTlsIndex = TlsAlloc();
        GetModuleFileNameW(hModule, g_szDllFileName, MAX_PATH);
        GetModuleFileNameW(NULL, g_szExeFileName, MAX_PATH);
        DBGFNC(L"KeyboardMouse%d::DLL_PROCESS_ATTACH", BITNESS);
        DBGPUT(L"DLL=%s", g_szDllFileName);
        DBGPUT(L"EXE=%s", g_szExeFileName);
        break;
    }

    case DLL_THREAD_ATTACH:
    {
        DBGFNC(L"KeyboardMouse%d::DLL_THREAD_ATTACH", BITNESS);
        break;
    }

    case DLL_THREAD_DETACH:
    {
        DBGFNC(L"KeyboardMouse%d::DLL_THREAD_DETACH", BITNESS);
        ReleaseAgent();
        break;
    }

    case DLL_PROCESS_DETACH:
    {
        DBGFNC(L"KeyboardMouse%d::DLL_PROCESS_DETACH", BITNESS);
        ReleaseAgent();
        delete g_pServer;
        delete g_pClient;
        TlsFree(g_dwTlsIndex);
        break;
    }
    }
    return TRUE;
}


extern "C" BOOL __stdcall StartServer(void)
{
    DBGFNC(L"KeyboardMouse%d::StartServer", BITNESS);
    BOOL bRet = FALSE;
    try
    {
        if (!g_pClient)
        {
            g_pClient = new Client(g_hModule);
        }
        DWORD dwState = g_pClient->StartServer();
        bRet = (dwState & (KEYBOARDMOUSE_SERVER32_RUNNING | KEYBOARDMOUSE_SERVER64_RUNNING)) ? TRUE : FALSE;
    }
    catch (ComException ex)
    {
        Debug::Put(FMT_COMEXCEPTION, ex.Message, ex.Result);
    }
    catch (Win32Exception ex)
    {
        Debug::Put(FMT_WIN32EXCEPTION, ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
    }
    catch (Exception ex)
    {
        Debug::Put(FMT_EXCEPTION, ex.Message);
    }
    catch (std::bad_alloc)
    {
        Debug::Put(MSG_OUT_OF_MEMORY);
    }
    catch (...)
    {
        Debug::Put(MSG_UNHANDLED_EXCEPTION);
    }
    DBGPUT(L"return=%d", bRet);
    return bRet;
}


extern "C" void __stdcall StopServer(void)
{
    DBGFNC(L"KeyboardMouse%d::StopServer", BITNESS);
    try
    {
        if (g_pClient)
        {
            g_pClient->StopServer();
        }
    }
    catch (ComException ex)
    {
        Debug::Put(FMT_COMEXCEPTION, ex.Message, ex.Result);
    }
    catch (Win32Exception ex)
    {
        Debug::Put(FMT_WIN32EXCEPTION, ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
    }
    catch (Exception ex)
    {
        Debug::Put(FMT_EXCEPTION, ex.Message);
    }
    catch (std::bad_alloc)
    {
        Debug::Put(MSG_OUT_OF_MEMORY);
    }
    catch (...)
    {
        Debug::Put(MSG_UNHANDLED_EXCEPTION);
    }
}


extern "C" DWORD __stdcall GetBlockInputState(void)
{
    DBGFNC(L"KeyboardMouse%d::GetBlockInputState", BITNESS);
    DWORD dwRet = 0;
    if (g_pClient)
    {
        dwRet = g_pClient->GetBlockInputFlags();
    }
    DBGPUT(L"return=%08lx", dwRet);
    return dwRet;
}


extern "C" DWORD __stdcall SetBlockInputState(DWORD dwFlags)
{
    DBGFNC(L"KeyboardMouse%d::SetBlockInputState", BITNESS);
    DBGPUT(L"state=%08lx", dwFlags);
    DWORD dwRet = 0;
    if (g_pClient)
    {
        dwRet = g_pClient->SetBlockInputFlags(dwFlags);
    }
    DBGPUT(L"return=%08lx", dwRet);
    return dwRet;
}


extern "C" BOOL __stdcall SetToggleKeySequence(PCWSTR psz)
{
    DBGFNC(L"KeyboardMouse%d::SetToggleKeySequence", BITNESS);
    DBGPUT(L"psz=%s", psz);
    BOOL bRet = FALSE;
    if (g_pClient)
    {
        bRet = g_pClient->SetToggleKeySequence(psz);
    }
    DBGPUT(L"return=%d", bRet);
    return bRet;
}


extern "C" BOOL __stdcall StartAgent(HWND hwnd)
{
    DBGFNC(L"KeyboardMouse%d::StartAgent", BITNESS);
    BOOL bRet = FALSE;
    try
    {
        if (g_pClient)
        {
            bRet = g_pClient->StartAgent(hwnd);
        }
    }
    catch (ComException ex)
    {
        Debug::Put(FMT_COMEXCEPTION, ex.Message, ex.Result);
    }
    catch (Win32Exception ex)
    {
        Debug::Put(FMT_WIN32EXCEPTION, ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
    }
    catch (Exception ex)
    {
        Debug::Put(FMT_EXCEPTION, ex.Message);
    }
    catch (std::bad_alloc)
    {
        Debug::Put(MSG_OUT_OF_MEMORY);
    }
    catch (...)
    {
        Debug::Put(MSG_UNHANDLED_EXCEPTION);
    }
    DBGPUT(L"return=%d", bRet);
    return bRet;
}


extern "C" BOOL __stdcall StopAgent(HWND hwnd)
{
    DBGFNC(L"KeyboardMouse%d::StopAgent", BITNESS);
    BOOL bRet = FALSE;
    try
    {
        if (g_pClient)
        {
            bRet = g_pClient->StopAgent(hwnd);
        }
    }
    catch (ComException ex)
    {
        Debug::Put(FMT_COMEXCEPTION, ex.Message, ex.Result);
    }
    catch (Win32Exception ex)
    {
        Debug::Put(FMT_WIN32EXCEPTION, ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
    }
    catch (Exception ex)
    {
        Debug::Put(FMT_EXCEPTION, ex.Message);
    }
    catch (std::bad_alloc)
    {
        Debug::Put(MSG_OUT_OF_MEMORY);
    }
    catch (...)
    {
        Debug::Put(MSG_UNHANDLED_EXCEPTION);
    }
    DBGPUT(L"return=%d", bRet);
    return bRet;
}


extern "C" DWORD __stdcall GetWindowKeyboardState(HWND hwnd, DWORD dwMask)
{
    DBGFNC(L"KeyboardMouse%d::GetWindowKeyboardState", BITNESS);
    DBGPUT(L"hwnd=%p mask=%08lx", hwnd, dwMask);
    DWORD dwRet = 0;
    if (g_pClient)
    {
        dwRet = g_pClient->GetKeyboardState(hwnd, dwMask);
    }
    DBGPUT(L"return=%08lx", dwRet);
    return dwRet;
}


extern "C" DWORD __stdcall SetWindowKeyboardState(HWND hwnd, DWORD dwState, DWORD dwMask)
{
    DBGFNC(L"KeyboardMouse%d::SetWindowKeyboardState", BITNESS);
    DBGPUT(L"hwnd=%p state=%08lx mask=%08lx", hwnd, dwState, dwMask);
    DWORD dwRet = 0;
    if (g_pClient)
    {
        dwRet = g_pClient->SetKeyboardState(hwnd, dwState, dwMask);
    }
    DBGPUT(L"return=%08lx", dwRet);
    return dwRet;
}


// This is the server main procedure to be passed to RUNDLL32.
extern "C" void CALLBACK RunServer(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
    DBGFNC(L"KeyboardMouse%d::RunServer", BITNESS);
    DBGPUT(L"CmdLine=\"%hs\"", lpszCmdLine);
    try
    {
        g_pServer = new Server(g_hModule, String(TRIM, String(lpszCmdLine)));
        g_pServer->Run();
    }
    catch (ComException ex)
    {
        Debug::Put(FMT_COMEXCEPTION, ex.Message, ex.Result);
    }
    catch (Win32Exception ex)
    {
        Debug::Put(FMT_WIN32EXCEPTION, ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
    }
    catch (Exception ex)
    {
        Debug::Put(FMT_EXCEPTION, ex.Message);
    }
    catch (std::bad_alloc)
    {
        Debug::Put(MSG_OUT_OF_MEMORY);
    }
    catch (...)
    {
        Debug::Put(MSG_UNHANDLED_EXCEPTION);
    }
}


LRESULT CALLBACK Server::LowLevelKeyboardHook(
    _In_ int    nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        try
        {
            if (g_pServer->OnKeybdHook(static_cast<UINT>(wParam), reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam)))
            {
                return TRUE;
            }
        }
        catch (ComException ex)
        {
            Debug::Put(FMT_COMEXCEPTION, ex.Message, ex.Result);
        }
        catch (Win32Exception ex)
        {
            Debug::Put(FMT_WIN32EXCEPTION, ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
        }
        catch (Exception ex)
        {
            Debug::Put(FMT_EXCEPTION, ex.Message);
        }
        catch (std::bad_alloc)
        {
            Debug::Put(MSG_OUT_OF_MEMORY);
        }
        catch (...)
        {
            Debug::Put(MSG_UNHANDLED_EXCEPTION);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


LRESULT CALLBACK Server::LowLevelMouseHook(
    _In_ int    nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        try
        {
            if (g_pServer->OnMouseHook(static_cast<UINT>(wParam), reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)))
            {
                return TRUE;
            }
        }
        catch (ComException ex)
        {
            Debug::Put(FMT_COMEXCEPTION, ex.Message, ex.Result);
        }
        catch (Win32Exception ex)
        {
            Debug::Put(FMT_WIN32EXCEPTION, ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
        }
        catch (Exception ex)
        {
            Debug::Put(FMT_EXCEPTION, ex.Message);
        }
        catch (std::bad_alloc)
        {
            Debug::Put(MSG_OUT_OF_MEMORY);
        }
        catch (...)
        {
            Debug::Put(MSG_UNHANDLED_EXCEPTION);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


LRESULT CALLBACK Agent::CallWndProcHook(
    _In_ int    nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        try
        {
            if (g_dwTlsIndex != TLS_OUT_OF_INDEXES)
            {
                Agent* pAgent = reinterpret_cast<Agent*>(TlsGetValue(g_dwTlsIndex));
                if (!pAgent)
                {
                    DBGFNC(L"KeyboardMouse%d::Agent::CallWndProcHook<pAgent=null>", BITNESS);
                    pAgent = new Agent();
                    TlsSetValue(g_dwTlsIndex, pAgent);
                }
                CWPSTRUCT* pcwps = reinterpret_cast<CWPSTRUCT*>(lParam);
                if (!pAgent->OnCall(pcwps->hwnd, pcwps->message, pcwps->wParam, pcwps->lParam))
                {
                    DBGFNC(L"KeyboardMouse%d::Agent::CallWndProcHook<OnCall=false>", BITNESS);
                    pAgent->Release();
                    TlsSetValue(g_dwTlsIndex, nullptr);
                }
            }
        }
        catch (ComException ex)
        {
            Debug::Put(FMT_COMEXCEPTION, ex.Message, ex.Result);
        }
        catch (Win32Exception ex)
        {
            Debug::Put(FMT_WIN32EXCEPTION, ex.Message, ex.Error, ErrorMessage::Get(ex.Error));
        }
        catch (Exception ex)
        {
            Debug::Put(FMT_EXCEPTION, ex.Message);
        }
        catch (std::bad_alloc)
        {
            Debug::Put(MSG_OUT_OF_MEMORY);
        }
        catch (...)
        {
            Debug::Put(MSG_UNHANDLED_EXCEPTION);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
