#pragma once


#include "hnrt/ComLibrary.h"
#include "hnrt/SharedMemory.h"
#include "hnrt/WindowsHandle.h"
#include "hnrt/String.h"
#include "ServerBlock.h"
#include "ServerWindow.h"


namespace hnrt
{
    class Server
        : public ComLibrary
    {
    public:

        static void MapSharedBlock(SharedMemory<ServerBlock>& sm, PCWSTR psz);

        Server(HMODULE hModule, PCWSTR pszGuid);
        Server(const Server&) = delete;
        ~Server();
        void operator =(const Server&) = delete;
        void Run();

        static LRESULT CALLBACK LowLevelKeyboardHook(
            _In_ int    nCode,
            _In_ WPARAM wParam,
            _In_ LPARAM lParam);
        static LRESULT CALLBACK LowLevelMouseHook(
            _In_ int    nCode,
            _In_ WPARAM wParam,
            _In_ LPARAM lParam);

        bool OnKeybdHook(UINT, const KBDLLHOOKSTRUCT*);
        bool OnMouseHook(UINT, const MSLLHOOKSTRUCT*);

    private:

        void Initialize();
        void Uninitialize();
        void RegisterClient(DWORD dwProcessId);
        static DWORD WINAPI WatchClientProcess(_In_ LPVOID lpParameter);
        bool CanChangeByToggleKeySequence();

        HMODULE m_hModule;
        String m_Guid;
        SharedMemory<ServerBlock> m_pServerBlock;
        WindowsHandle m_hExitEvent;
        WindowsHandle m_hClientProcess;
        WindowsHandle m_hClientProcessWatcher;
        ServerWindow m_ServerWindow;
        KeyboardBitMap m_KeysPressed;
        ULONGLONG m_LastProcessed;
    };
}
