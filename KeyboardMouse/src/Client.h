#pragma once


#include "hnrt/ComLibrary.h"
#include "hnrt/SharedMemory.h"
#include "ServerBlock.h"


namespace hnrt
{
    class Client
        : public ComLibrary
    {
    public:

        Client(HMODULE hModule);
        Client(const Client&) = delete;
        ~Client();
        void operator =(const Client&) = delete;
        DWORD StartServer();
        void StopServer();
        DWORD GetBlockInputFlags();
        DWORD SetBlockInputFlags(DWORD dwFlags);
        BOOL SetToggleKeySequence(PCWSTR psz);
        BOOL StartAgent(HWND hwnd);
        BOOL StopAgent(HWND hwnd);
        DWORD GetKeyboardState(HWND hwnd, DWORD dwMask);
        DWORD SetKeyboardState(HWND hwnd, DWORD dwState, DWORD dwMask);

    private:

        bool StartServer(int bitness);
        bool BuildCommandLine(int bitness, PWCHAR pCommandLine);

        HMODULE m_hModule;
        PCWSTR m_pszGuid;
        SharedMemory<ServerBlock> m_pServerBlock;
    };
}
