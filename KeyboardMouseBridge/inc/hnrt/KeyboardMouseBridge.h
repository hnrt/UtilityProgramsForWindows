#pragma once


#include <Windows.h>
#include "hnrt/KeyboardMouse.h"


namespace hnrt
{
    class KeyboardMouseBridge
    {
    public:

        KeyboardMouseBridge();
        KeyboardMouseBridge(const KeyboardMouseBridge&) = delete;
        ~KeyboardMouseBridge();
        void operator =(const KeyboardMouseBridge&) = delete;
        BOOL StartServer();
        void StopServer();
        DWORD GetBlockInputState();
        DWORD SetBlockInputState(DWORD dwFlags);
        BOOL SetToggleKeySequence(PCWSTR psz);
        BOOL StartAgent(HWND hwnd);
        BOOL StopAgent(HWND hwnd = nullptr);
        DWORD GetKeyboardState(HWND hwnd, DWORD dwMask);
        DWORD SetKeyboardState(HWND hwnd, DWORD dwState, DWORD dwMask);

    private:

        typedef BOOL(__stdcall* PFNSTARTSERVER)(void);
        typedef void(__stdcall* PFNSTOPSERVER)(void);
        typedef DWORD(__stdcall* PFNGETBLOCKINPUTSTATE)(void);
        typedef DWORD(__stdcall* PFNSETBLOCKINPUTSTATE)(DWORD);
        typedef BOOL(__stdcall* PFNSETTOGGLEKEYSEQUENCE)(PCWSTR);
        typedef BOOL(__stdcall* PFNSTARTAGENT)(HWND);
        typedef BOOL(__stdcall* PFNSTOPAGENT)(HWND);
        typedef DWORD(__stdcall* PFNGETKEYBOARDSTATE)(HWND, DWORD);
        typedef DWORD(__stdcall* PFNSETKEYBOARDSTATE)(HWND, DWORD, DWORD);

        HMODULE m_hmod;
        PFNSTARTSERVER m_pfnStartServer;
        PFNSTOPSERVER m_pfnStopServer;
        PFNGETBLOCKINPUTSTATE m_pfnGetBlockInputState;
        PFNSETBLOCKINPUTSTATE m_pfnSetBlockInputState;
        PFNSETTOGGLEKEYSEQUENCE m_pfnSetToggleKeySequence;
        PFNSTARTAGENT m_pfnStartAgent;
        PFNSTOPAGENT m_pfnStopAgent;
        PFNGETKEYBOARDSTATE m_pfnGetKeyboardState;
        PFNSETKEYBOARDSTATE m_pfnSetKeyboardState;
    };
}
