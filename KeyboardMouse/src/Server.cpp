#include "pch.h"
#include "Server.h"
#include "KeyboardMouseConstants.h"
#include "hnrt/Debug.h"
#include "hnrt/SpinLock.h"
#include "hnrt/ComException.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/String.h"
#include "hnrt/WindowsPlatform.h"
#include "hnrt/Path.h"
#include "hnrt/ErrorMessage.h"


using namespace hnrt;


void Server::MapSharedBlock(SharedMemory<ServerBlock>& sm, PCWSTR psz)
{
    DWORD dwError = sm.Map(String::Format2(L"Local\\SERVER_%s", psz));
    if (dwError)
    {
        throw Win32Exception(dwError, L"Failed to map the server block.");
    }
}


Server::Server(HMODULE hModule, PCWSTR pszGuid)
    : ComLibrary(COINIT_APARTMENTTHREADED)
    , m_hModule(hModule)
    , m_Guid(pszGuid)
    , m_pServerBlock()
    , m_hExitEvent(CreateEventW(NULL, TRUE, FALSE, NULL))
    , m_hClientProcess()
    , m_hClientProcessWatcher()
    , m_ServerWindow(hModule, m_pServerBlock)
    , m_KeysPressed()
    , m_LastProcessed(0)
{
    DBGFNC(L"KeyboardMouse%d::Server::ctor", BITNESS);
    if (!m_hExitEvent)
    {
        throw Exception(L"Event handle unavailable.");
    }
}


Server::~Server()
{
    DBGFNC(L"KeyboardMouse%d::Server::dtor", BITNESS);
}


void Server::Run()
{
    DBGFNC(L"KeyboardMouse%d::Server::Run", BITNESS);

    Initialize();

    m_ServerWindow.MainLoop(m_hExitEvent);

    Uninitialize();
}


void Server::Initialize()
{
    MapSharedBlock(m_pServerBlock, m_Guid);

    RegisterClient(m_pServerBlock->dwClientProcessId);
}


void Server::Uninitialize()
{
    if (m_hExitEvent)
    {
        SetEvent(m_hExitEvent);

        if (m_hClientProcessWatcher)
        {
            WaitForSingleObject(m_hClientProcessWatcher, 1000);
        }
    }

    m_pServerBlock.Unmap();
}


void Server::RegisterClient(DWORD dwProcessId)
{
    *(&m_hClientProcess) = OpenProcess(SYNCHRONIZE, FALSE, dwProcessId);
    if (!m_hClientProcess)
    {
        throw Win32Exception(GetLastError(), L"OpenProcess(%lu) failed.", dwProcessId);
    }

    *(&m_hClientProcessWatcher) = CreateThread(NULL, 0, WatchClientProcess, this, 0, NULL);
    if (!m_hClientProcessWatcher)
    {
        throw Win32Exception(GetLastError(), L"OpenProcess(%lu) failed.", dwProcessId);
    }
}


DWORD WINAPI Server::WatchClientProcess(_In_ LPVOID lpParameter)
{
    DBGFNC(L"KeyboardMouse%d::Server::WatchClientProcess", BITNESS);
    Server* pThis = reinterpret_cast<Server*>(lpParameter);
    HANDLE hh[2] =
    {
        pThis->m_hExitEvent,
        pThis->m_hClientProcess
    };
    DWORD dwRet = WaitForMultipleObjects(2, hh, FALSE, INFINITE);
    if (dwRet == WAIT_OBJECT_0 + 0)
    {
        DBGPUT(L"Server is going down.");
        return 0;
    }
    else if (dwRet == WAIT_OBJECT_0 + 1)
    {
        DBGPUT(L"Client exited.");
        SetEvent(pThis->m_hExitEvent);
        return 1;
    }
    else
    {
        DWORD dwError = GetLastError();
        Debug::Put(L"WaitForMultipleObjects failed. return=%lu error=%lu(%s)", dwRet, dwError, ErrorMessage::Get(dwError));
        return 2;
    }
}


bool Server::OnKeybdHook(UINT uMsg, const KBDLLHOOKSTRUCT* pKHS)
{
    if ((pKHS->flags & (LLKHF_INJECTED | LLKHF_LOWER_IL_INJECTED)) != 0)
    {
        //DBGPUT(L"Injected.");
        return false;
    }
    if ((pKHS->flags & LLKHF_UP) == 0)
    {
        // KEY DOWN
        m_KeysPressed.Set(pKHS->vkCode);
        if (m_pServerBlock->bBlockKeybd || m_pServerBlock->bBlockMouse)
        {
            if (!m_pServerBlock->ToggleKeySequence.IsZero() && m_KeysPressed.IsSet(m_pServerBlock->ToggleKeySequence))
            {
                if (CanChangeByToggleKeySequence())
                {
                    DBGPUT(L"Toggle key sequence detected.");
                    m_pServerBlock->bBlockInputPaused ^= 1;
                }
            }
            if (m_pServerBlock->bBlockKeybd && !m_pServerBlock->bBlockInputPaused)
            {
                DBGPUT(L"Blocked.");
                return true;
            }
        }
    }
    else
    {
        // KEY UP
        m_KeysPressed.Reset(pKHS->vkCode);
    }
    //DBGPUT(L"Allowed.");
    return false;
}


bool Server::OnMouseHook(UINT uMsg, const MSLLHOOKSTRUCT* pMHS)
{
    if ((pMHS->flags & (LLMHF_INJECTED | LLMHF_LOWER_IL_INJECTED)) != 0)
    {
        //DBGPUT(L"Injected.");
        return false;
    }
    if (m_pServerBlock->bBlockMouse && !m_pServerBlock->bBlockInputPaused)
    {
        DBGPUT(L"Blocked.");
        return true;
    }
    //DBGPUT(L"Allowed.");
    return false;
}


bool Server::CanChangeByToggleKeySequence()
{
    static LARGE_INTEGER freq; // initialized by zeroes
    if (freq.QuadPart == 0)
    {
        QueryPerformanceFrequency(&freq);
    }
    ULONGLONG last = m_LastProcessed;
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    if (last + (MIN_MILLISECONDS_TO_CHANGE * freq.QuadPart / 1000ULL) <= static_cast<ULONGLONG>(current.QuadPart))
    {
        if (InterlockedCompareExchange(&m_LastProcessed, current.QuadPart, last) == last)
        {
            // guarantees the exclusive change.
            return true;
        }
    }
    return false;
}
