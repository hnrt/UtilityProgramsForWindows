#include "pch.h"
#include "ServerWindow.h"
#include "Server.h"
#include "Agent.h"
#include "KeyboardMouseConstants.h"
#include "hnrt/Debug.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/WindowsPlatform.h"
#include "hnrt/WindowHelper.h"


#define WINDOW_CLASSNAME L"KeyboardMouseServerMessageWindowClass"
#define WINDOW_NAME L"KeyboardMouseServerMessage"


using namespace hnrt;


ServerWindow::ServerWindow(HMODULE hModule, SharedMemory<ServerBlock>& pServerBlock)
    : m_hModule(hModule)
    , m_pServerBlock(pServerBlock)
    , m_atomWndClass(RegisterWndClass(hModule))
    , m_hwndMessage(nullptr)
    , m_hKeybdHook(nullptr)
    , m_hMouseHook(nullptr)
    , m_WM_AGENT_COMMAND(Agent::GetCommandMessage())
    , m_AgentHooks()
{
}


ServerWindow::~ServerWindow()
{
    StopAllAgents();
    UnregisterClassW(WINDOW_CLASSNAME, m_hModule);
}


ATOM ServerWindow::RegisterWndClass(HMODULE hModule)
{
    WNDCLASSEX wcx = { 0 };
    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.lpfnWndProc = WindowProc;
    wcx.hInstance = hModule;
    wcx.lpszClassName = WINDOW_CLASSNAME;
    return RegisterClassExW(&wcx);
}


void ServerWindow::MainLoop(HANDLE hExitEvent)
{
    DBGFNC(L"KeyboardMouse%d::ServerWindow::MainLoop", BITNESS);
    if (CreateWindowExW(0, WINDOW_CLASSNAME, WINDOW_NAME, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, m_hModule, this))
    {
        while (true)
        {
            DWORD dwRet = MsgWaitForMultipleObjects(1, &hExitEvent, FALSE, INFINITE, QS_ALLINPUT);
            if (dwRet == WAIT_OBJECT_0)
            {
                DBGPUT(L"Signaled by ExitEvent.");
                break;
            }
            else if (dwRet == WAIT_OBJECT_0 + 1)
            {
                MSG msg;
                BOOL bRet = GetMessageW(&msg, NULL, 0, 0);
                if (bRet > 0)
                {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
                else if (!bRet)
                {
                    break;
                }
                else //if (bRet < 0)
                {
                    DWORD dwError = GetLastError();
                    Debug::Put(L"GetMessage failed. error=%lu(%s)", dwError, ErrorMessage::Get(dwError));
                    break;
                }
            }
            else
            {
                DWORD dwError = GetLastError();
                Debug::Put(L"MsgWaitForMultipleObjects failed. error=%lu(%s)", dwError, ErrorMessage::Get(dwError));
                break;
            }
        }
        DestroyWindow(m_hwndMessage);
    }
}


LRESULT CALLBACK ServerWindow::WindowProc(
    _In_ HWND   hwnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    ServerWindow* pThis = reinterpret_cast<ServerWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (pThis)
    {
        switch (uMsg)
        {
        case WM_DESTROY:
            pThis->OnDestroy();
            return 0;
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_APP_START_AGENT:
            return pThis->StartAgent(reinterpret_cast<HWND>(wParam));
        case WM_APP_STOP_AGENT:
            return pThis->StopAgent(reinterpret_cast<HWND>(wParam));
        case WM_APP_GET_LANGUAGE:
            return pThis->GetLanguage(reinterpret_cast<HWND>(wParam));
        case WM_APP_SET_LANGUAGE:
            return pThis->SetLanguage(reinterpret_cast<HWND>(wParam), static_cast<LANGID>(lParam));
        case WM_APP_GET_OPENCLOSE:
            return pThis->GetOpenClose(reinterpret_cast<HWND>(wParam));
        case WM_APP_SET_OPENCLOSE:
            return pThis->SetOpenClose(reinterpret_cast<HWND>(wParam), static_cast<LONG>(lParam));
        case WM_APP_GET_INPUTMODE:
            return pThis->GetInputMode(reinterpret_cast<HWND>(wParam));
        case WM_APP_SET_INPUTMODE:
            return pThis->SetInputMode(reinterpret_cast<HWND>(wParam), static_cast<LONG>(lParam));
        default:
            break;
        }
    }
    else if (uMsg == WM_CREATE)
    {
        LPCREATESTRUCTW pCS = reinterpret_cast<LPCREATESTRUCTW>(lParam);
        pThis = reinterpret_cast<ServerWindow*>(pCS->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        pThis->OnCreate(hwnd);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}


void ServerWindow::OnCreate(HWND hwnd)
{
    m_hwndMessage = hwnd;
    if (WindowsPlatform::ProcessBitness(m_pServerBlock->dwClientProcessId) == BITNESS)
    {
        InstallKeybdHook();
        InstallMouseHook();
    }
#if BITNESS == 64
    InterlockedExchange(&m_pServerBlock->dwProcessId64, GetCurrentProcessId());
    InterlockedExchange(&m_pServerBlock->hwndMessage64, static_cast<DWORD>(reinterpret_cast<DWORD_PTR>(hwnd)));
#elif BITNESS == 32
    InterlockedExchange(&m_pServerBlock->dwProcessId32, GetCurrentProcessId());
    InterlockedExchange(&m_pServerBlock->hwndMessage32, static_cast<DWORD>(reinterpret_cast<DWORD_PTR>(hwnd)));
#endif
}


void ServerWindow::OnDestroy()
{
    m_hwndMessage = nullptr;
    StopAgent(nullptr);
    if (WindowsPlatform::ProcessBitness(m_pServerBlock->dwClientProcessId) == BITNESS)
    {
        UninstallMouseHook();
        UninstallKeybdHook();
    }
#if BITNESS == 64
    InterlockedExchange(&m_pServerBlock->dwProcessId64, 0);
    InterlockedExchange(&m_pServerBlock->hwndMessage64, 0);
#elif BITNESS == 32
    InterlockedExchange(&m_pServerBlock->dwProcessId32, 0);
    InterlockedExchange(&m_pServerBlock->hwndMessage32, 0);
#endif
}


void ServerWindow::InstallKeybdHook()
{
    if (!m_hKeybdHook)
    {
        m_hKeybdHook = SetWindowsHookExW(WH_KEYBOARD_LL, Server::LowLevelKeyboardHook, m_hModule, 0);
        if (m_hKeybdHook)
        {
            DBGPUT(L"SetWindowsHookEx(WH_KEYBOARD_LL)");
        }
        else
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"SetWindowsHookEx(WH_KEYBOARD_LL): Failed. error=%lu", dwError);
        }
    }
}


void ServerWindow::InstallMouseHook()
{
    if (!m_hMouseHook)
    {
        m_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, Server::LowLevelMouseHook, m_hModule, 0);
        if (m_hMouseHook)
        {
            DBGPUT(L"SetWindowsHookEx(WH_MOUSE_LL)");
        }
        else
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"SetWindowsHookEx(WH_MOUSE_LL): Failed. error=%lu", dwError);
        }
    }
}


void ServerWindow::UninstallKeybdHook()
{
    if (m_hKeybdHook)
    {
        if (UnhookWindowsHookEx(m_hKeybdHook))
        {
            DBGPUT(L"UnhookWindowsHookEx(WH_KEYBOARD_LL)");
        }
        else
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"UnhookWindowsHookEx(WH_KEYBOARD_LL): Failed. error=%lu", dwError);
        }
        m_hKeybdHook = NULL;
    }
}


void ServerWindow::UninstallMouseHook()
{
    if (m_hMouseHook)
    {
        if (UnhookWindowsHookEx(m_hMouseHook))
        {
            DBGPUT(L"UnhookWindowsHookEx(WH_MOUSE_LL)");
        }
        else
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"UnhookWindowsHookEx(WH_MOUSE_LL): Failed. error=%lu", dwError);
        }
        m_hMouseHook = NULL;
    }
}


LRESULT ServerWindow::StartAgent(HWND hwnd)
{
    AgentHook* pAgentHook = AddAgentHook(hwnd);
    if (pAgentHook)
    {
        SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_INITIALIZE, 0, SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        if ((pAgentHook->BlockPtr->dwFlags & AGENT_FLAG_ENABLED))
        {
            return TRUE;
        }
    }
    return FALSE;
}


LRESULT ServerWindow::StopAgent(HWND hwnd)
{
    if (hwnd)
    {
        AgentHook* pAgentHook = RemoveAgentHook(hwnd);
        if (pAgentHook)
        {
            SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_UNINITIALIZE, 0, SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
            LRESULT lRet = (pAgentHook->BlockPtr->dwFlags & AGENT_FLAG_ENABLED) ? FALSE : TRUE;
            delete pAgentHook;
            return lRet;
        }
        return FALSE;
    }
    else
    {
        StopAllAgents();
        return TRUE;
    }
}


void ServerWindow::StopAllAgents()
{
    for (std::map<DWORD, AgentHook*>::iterator iter = m_AgentHooks.begin(); iter != m_AgentHooks.end(); iter++)
    {
        AgentHook* pAgentHook = iter->second;
        HWND hwnd = reinterpret_cast<HWND>(static_cast<DWORD_PTR>(pAgentHook->BlockPtr->dwHwnd));
        if (hwnd)
        {
            SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_UNINITIALIZE, 0, SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        }
        delete pAgentHook;
        iter->second = nullptr;
    }
    m_AgentHooks.clear();
}


LRESULT ServerWindow::GetLanguage(HWND hwnd)
{
    AgentHook* pAgentHook = GetAgentHook(hwnd);
    if (pAgentHook)
    {
        SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_GET_LANGUAGE, 0, SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        if ((pAgentHook->BlockPtr->dwFlags & KEYBOARD_FLAG_LANGUAGE))
        {
            //DBGPUT(L"GetLanguage: return=%04x", pAgentHook->BlockPtr->Language);
            return pAgentHook->BlockPtr->Language;
        }
    }
    //DBGPUT(L"GetLanguage: return=-1");
    return -1;
}


LRESULT ServerWindow::SetLanguage(HWND hwnd, LANGID langid)
{
    AgentHook* pAgentHook = GetAgentHook(hwnd);
    if (pAgentHook)
    {
        SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_SET_LANGUAGE, static_cast<LPARAM>(langid), SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        if ((pAgentHook->BlockPtr->dwFlags & KEYBOARD_FLAG_LANGUAGE))
        {
            //DBGPUT(L"SetLanguage: return=%04x", pAgentHook->BlockPtr->Language);
            return pAgentHook->BlockPtr->Language;
        }
        SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_GET_LANGUAGE, 0, SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        if ((pAgentHook->BlockPtr->dwFlags & KEYBOARD_FLAG_LANGUAGE))
        {
            //DBGPUT(L"SetLanguage: return=%04x", pAgentHook->BlockPtr->Language);
            return pAgentHook->BlockPtr->Language;
        }
    }
    //DBGPUT(L"SetLanguage: return=-1");
    return -1;
}


LRESULT ServerWindow::GetOpenClose(HWND hwnd)
{
    AgentHook* pAgentHook = GetAgentHook(hwnd);
    if (pAgentHook)
    {
        SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_GET_OPENCLOSE, 0, SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        if ((pAgentHook->BlockPtr->dwFlags & KEYBOARD_FLAG_OPENCLOSE))
        {
            //DBGPUT(L"GetOpenClose: return=%ld", pAgentHook->BlockPtr->KeyboardOpenClose);
            return pAgentHook->BlockPtr->KeyboardOpenClose;
        }
    }
    //DBGPUT(L"GetOpenClose: return=-1");
    return -1;
}


LRESULT ServerWindow::SetOpenClose(HWND hwnd, LONG value)
{
    AgentHook* pAgentHook = GetAgentHook(hwnd);
    if (pAgentHook)
    {
        SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_SET_OPENCLOSE, static_cast<LPARAM>(value), SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        if ((pAgentHook->BlockPtr->dwFlags & KEYBOARD_FLAG_OPENCLOSE))
        {
            //DBGPUT(L"SetOpenClose: return=%ld", pAgentHook->BlockPtr->KeyboardOpenClose);
            return pAgentHook->BlockPtr->KeyboardOpenClose;
        }
        SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_GET_OPENCLOSE, 0, SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        if ((pAgentHook->BlockPtr->dwFlags & KEYBOARD_FLAG_OPENCLOSE))
        {
            //DBGPUT(L"SetOpenClose: return=%ld", pAgentHook->BlockPtr->KeyboardOpenClose);
            return pAgentHook->BlockPtr->KeyboardOpenClose;
        }
    }
    //DBGPUT(L"SetOpenClose: return=-1");
    return -1;
}


LRESULT ServerWindow::GetInputMode(HWND hwnd)
{
    AgentHook* pAgentHook = GetAgentHook(hwnd);
    if (pAgentHook)
    {
        SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_GET_INPUTMODE, 0, SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        if ((pAgentHook->BlockPtr->dwFlags & KEYBOARD_FLAG_INPUTMODE))
        {
            //DBGPUT(L"GetInputMode: return=%04lx", pAgentHook->BlockPtr->InputModeConversion);
            return pAgentHook->BlockPtr->InputModeConversion;
        }
    }
    //DBGPUT(L"GetInputMode: return=-1");
    return -1;
}


LRESULT ServerWindow::SetInputMode(HWND hwnd, LONG value)
{
    AgentHook* pAgentHook = GetAgentHook(hwnd);
    if (pAgentHook)
    {
        SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_SET_INPUTMODE, static_cast<LPARAM>(value), SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        if ((pAgentHook->BlockPtr->dwFlags & KEYBOARD_FLAG_INPUTMODE))
        {
            //DBGPUT(L"SetInputMode: return=%04lx", pAgentHook->BlockPtr->InputModeConversion);
            return pAgentHook->BlockPtr->InputModeConversion;
        }
        SendMessageTimeoutW(hwnd, m_WM_AGENT_COMMAND, AGENT_GET_INPUTMODE, 0, SMTO_NOTIMEOUTIFNOTHUNG, AGENT_COMMAND_TIMEOUT, NULL);
        if ((pAgentHook->BlockPtr->dwFlags & KEYBOARD_FLAG_INPUTMODE))
        {
            //DBGPUT(L"SetInputMode: return=%04lx", pAgentHook->BlockPtr->InputModeConversion);
            return pAgentHook->BlockPtr->InputModeConversion;
        }
    }
    //DBGPUT(L"SetInputMode: return=-1");
    return -1;
}


AgentHook* ServerWindow::AddAgentHook(HWND hwnd)
{
    WindowHelper w(hwnd);
    if (w.ThreadId)
    {
        std::map<DWORD, AgentHook*>::iterator iter = m_AgentHooks.find(w.ThreadId);
        if (iter != m_AgentHooks.end())
        {
            return iter->second;
        }
        else
        {
            AgentHook* pAgentHook = new AgentHook(m_hModule, hwnd, w.ThreadId);
            m_AgentHooks.insert(std::pair<DWORD, AgentHook*>(w.ThreadId, pAgentHook));
            return pAgentHook;
        }
    }
    else
    {
        return nullptr;
    }
}


AgentHook* ServerWindow::GetAgentHook(HWND hwnd)
{
    std::map<DWORD, AgentHook*>::iterator iter = m_AgentHooks.find(WindowHelper(hwnd).ThreadId);
    return iter != m_AgentHooks.end() ? iter->second : nullptr;
}


AgentHook* ServerWindow::RemoveAgentHook(HWND hwnd)
{
    std::map<DWORD, AgentHook*>::iterator iter = m_AgentHooks.find(WindowHelper(hwnd).ThreadId);
    if (iter != m_AgentHooks.end())
    {
        AgentHook* pAgentHook = iter->second;
        m_AgentHooks.erase(iter);
        return pAgentHook;
    }
    return nullptr;
}
