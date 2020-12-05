#pragma once


#include <map>
#include "hnrt/SharedMemory.h"
#include "ServerBlock.h"
#include "AgentHook.h"


namespace hnrt
{
    class ServerWindow
    {
    public:

        ServerWindow(HMODULE hModule, SharedMemory<ServerBlock>& pServerBlock);
        ServerWindow(const ServerWindow&) = delete;
        ~ServerWindow();
        void operator =(const ServerWindow&) = delete;
        void MainLoop(HANDLE hExitEvent);

    private:

        static ATOM RegisterWndClass(HMODULE hModule);
        static LRESULT CALLBACK WindowProc(
            _In_ HWND   hwnd,
            _In_ UINT   uMsg,
            _In_ WPARAM wParam,
            _In_ LPARAM lParam
        );
        void OnCreate(HWND hwnd);
        void OnDestroy();
        void InstallKeybdHook();
        void InstallMouseHook();
        void UninstallKeybdHook();
        void UninstallMouseHook();
        LRESULT StartAgent(HWND hwnd);
        LRESULT StopAgent(HWND hwnd);
        void StopAllAgents();
        LRESULT GetLanguage(HWND hwnd);
        LRESULT SetLanguage(HWND hwnd, LANGID langid);
        LRESULT GetOpenClose(HWND hwnd);
        LRESULT SetOpenClose(HWND hwnd, LONG value);
        LRESULT GetInputMode(HWND hwnd);
        LRESULT SetInputMode(HWND hwnd, LONG value);
        AgentHook* AddAgentHook(HWND hwnd);
        AgentHook* GetAgentHook(HWND hwnd);
        AgentHook* RemoveAgentHook(HWND hwnd);

        HMODULE m_hModule;
        SharedMemory<ServerBlock>& m_pServerBlock;
        ATOM m_atomWndClass;
        HWND m_hwndMessage;
        HHOOK m_hKeybdHook;
        HHOOK m_hMouseHook;
        UINT m_WM_AGENT_COMMAND;
        std::map<DWORD, AgentHook*> m_AgentHooks;
    };
}
