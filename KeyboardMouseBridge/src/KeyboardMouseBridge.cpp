#include "pch.h"
#include "hnrt/KeyboardMouseBridge.h"
#include "hnrt/WindowsPlatform.h"
#include "hnrt/String.h"


#pragma comment(lib, "Core")


using namespace hnrt;


KeyboardMouseBridge::KeyboardMouseBridge()
    : m_hmod(LoadLibraryW(String::Format(L"KeyboardMouse%d", WindowsPlatform::ProcessBitness())))
    , m_pfnStartServer(nullptr)
    , m_pfnStopServer(nullptr)
    , m_pfnGetBlockInputState(nullptr)
    , m_pfnSetBlockInputState(nullptr)
    , m_pfnSetToggleKeySequence(nullptr)
    , m_pfnStartAgent(nullptr)
    , m_pfnStopAgent(nullptr)
    , m_pfnGetKeyboardState(nullptr)
    , m_pfnSetKeyboardState(nullptr)
{
    if (m_hmod)
    {
        m_pfnStartServer = (PFNSTARTSERVER)GetProcAddress(m_hmod, "StartServer");
        m_pfnStopServer = (PFNSTOPSERVER)GetProcAddress(m_hmod, "StopServer");
        m_pfnGetBlockInputState = (PFNGETBLOCKINPUTSTATE)GetProcAddress(m_hmod, "GetBlockInputState");
        m_pfnSetBlockInputState = (PFNSETBLOCKINPUTSTATE)GetProcAddress(m_hmod, "SetBlockInputState");
        m_pfnSetToggleKeySequence = (PFNSETTOGGLEKEYSEQUENCE)GetProcAddress(m_hmod, "SetToggleKeySequence");
        m_pfnStartAgent = (PFNSTARTAGENT)GetProcAddress(m_hmod, "StartAgent");
        m_pfnStopAgent = (PFNSTOPAGENT)GetProcAddress(m_hmod, "StopAgent");
        m_pfnGetKeyboardState = (PFNGETKEYBOARDSTATE)GetProcAddress(m_hmod, "GetWindowKeyboardState");
        m_pfnSetKeyboardState = (PFNSETKEYBOARDSTATE)GetProcAddress(m_hmod, "SetWindowKeyboardState");
    }
}


KeyboardMouseBridge::~KeyboardMouseBridge()
{
    if (m_hmod)
    {
        FreeLibrary(m_hmod);
    }
}


BOOL KeyboardMouseBridge::StartServer()
{
    if (m_pfnStartServer)
    {
        return (*m_pfnStartServer)();
    }
    else
    {
        return FALSE;
    }
}


void KeyboardMouseBridge::StopServer()
{
    if (m_pfnStopServer)
    {
        (*m_pfnStopServer)();
    }
}


DWORD KeyboardMouseBridge::GetBlockInputState()
{
    if (m_pfnGetBlockInputState)
    {
        return (*m_pfnGetBlockInputState)();
    }
    else
    {
        return 0;
    }
}


DWORD KeyboardMouseBridge::SetBlockInputState(DWORD dwFlags)
{
    if (m_pfnSetBlockInputState)
    {
        return (*m_pfnSetBlockInputState)(dwFlags);
    }
    else
    {
        return 0;
    }
}


BOOL KeyboardMouseBridge::SetToggleKeySequence(PCWSTR psz)
{
    if (m_pfnSetToggleKeySequence)
    {
        return (*m_pfnSetToggleKeySequence)(psz);
    }
    else
    {
        return FALSE;
    }
}


BOOL KeyboardMouseBridge::StartAgent(HWND hwnd)
{
    if (m_pfnStartAgent)
    {
        return (*m_pfnStartAgent)(hwnd);
    }
    else
    {
        return FALSE;
    }
}


BOOL KeyboardMouseBridge::StopAgent(HWND hwnd)
{
    if (m_pfnStopAgent)
    {
        return (*m_pfnStopAgent)(hwnd);
    }
    else
    {
        return FALSE;
    }
}


DWORD KeyboardMouseBridge::GetKeyboardState(HWND hwnd, DWORD dwMask)
{
    if (m_pfnGetKeyboardState)
    {
        return (*m_pfnGetKeyboardState)(hwnd, dwMask);
    }
    else
    {
        return 0;
    }
}


DWORD KeyboardMouseBridge::SetKeyboardState(HWND hwnd, DWORD dwState, DWORD dwMask)
{
    if (m_pfnSetKeyboardState)
    {
        return (*m_pfnSetKeyboardState)(hwnd, dwState, dwMask);
    }
    else
    {
        return 0;
    }
}
