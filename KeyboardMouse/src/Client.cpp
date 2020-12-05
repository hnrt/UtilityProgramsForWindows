#include "pch.h"
#include "Client.h"
#include "Server.h"
#include "KeyboardMouseConstants.h"
#include "hnrt/Debug.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Guid.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/String.h"
#include "hnrt/WindowsPlatform.h"
#include "hnrt/WindowHelper.h"
#include "hnrt/Path.h"


using namespace hnrt;


Client::Client(HMODULE hModule)
    : ComLibrary(COINIT_APARTMENTTHREADED)
    , m_hModule(hModule)
    , m_pszGuid(Guid().Initialize().ToString(GUID_TOSTRING_NO_BRACE | GUID_TOSTRING_UPPERCASE))
    , m_pServerBlock()
{
    DBGFNC(L"KeyboardMouse%d::Client::ctor", BITNESS);
    DBGPUT(L"guid=%s", m_pszGuid);
    Server::MapSharedBlock(m_pServerBlock, m_pszGuid);
    m_pServerBlock->dwClientProcessId = GetCurrentProcessId();
}


Client::~Client()
{
    DBGFNC(L"KeyboardMouse%d::Client::dtor", BITNESS);
}


DWORD Client::StartServer()
{
    DBGFNC(L"KeyboardMouse%d::Client::StartServer", BITNESS);

    DWORD dwState = 0;

    if (m_pServerBlock->dwProcessId64)
    {
        WindowsHandle h = OpenProcess(SYNCHRONIZE, FALSE, m_pServerBlock->dwProcessId64);
        if (h)
        {
            dwState |= KEYBOARDMOUSE_SERVER64_RUNNING;
        }
        else
        {
            m_pServerBlock->dwProcessId64 = 0;
            m_pServerBlock->hwndMessage64 = 0;
            dwState |= StartServer(64) ? KEYBOARDMOUSE_SERVER64_START_PENDING : KEYBOARDMOUSE_SERVER64_FAILURE;
        }
    }
    else
    {
        dwState |= StartServer(64) ? KEYBOARDMOUSE_SERVER64_START_PENDING : KEYBOARDMOUSE_SERVER64_FAILURE;
    }

    if (m_pServerBlock->dwProcessId32)
    {
        WindowsHandle h = OpenProcess(SYNCHRONIZE, FALSE, m_pServerBlock->dwProcessId32);
        if (h)
        {
            dwState |= KEYBOARDMOUSE_SERVER32_RUNNING;
        }
        else
        {
            m_pServerBlock->dwProcessId32 = 0;
            m_pServerBlock->hwndMessage32 = 0;
            dwState |= StartServer(32) ? KEYBOARDMOUSE_SERVER32_START_PENDING : KEYBOARDMOUSE_SERVER32_FAILURE;
        }
    }
    else
    {
        dwState |= StartServer(32) ? KEYBOARDMOUSE_SERVER32_START_PENDING : KEYBOARDMOUSE_SERVER32_FAILURE;
    }

    for (int i = 0; (dwState & (KEYBOARDMOUSE_SERVER32_START_PENDING | KEYBOARDMOUSE_SERVER64_START_PENDING)) && i < START_TIMEOUT; i++)
    {
        Sleep(1);

        if ((dwState & KEYBOARDMOUSE_SERVER64_START_PENDING) && m_pServerBlock->dwProcessId64)
        {
            dwState &= ~KEYBOARDMOUSE_SERVER64_MASK;
            dwState |= KEYBOARDMOUSE_SERVER64_RUNNING;
        }

        if ((dwState & KEYBOARDMOUSE_SERVER32_START_PENDING) && m_pServerBlock->dwProcessId32)
        {
            dwState &= ~KEYBOARDMOUSE_SERVER32_MASK;
            dwState |= KEYBOARDMOUSE_SERVER32_RUNNING;
        }
    }

    DBGPUT(L"return=%08lx", dwState);

    return dwState;
}


// Creates a process of RUNDLL32 to run RunServer function, which is the main procedure of the server. 
bool Client::StartServer(int bitness)
{
    STARTUPINFOW si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(STARTUPINFOW);

    WCHAR szCommandLine[MAX_PATH * 2];

    if (!BuildCommandLine(bitness, szCommandLine))
    {
        return false;
    }

    if (CreateProcessW(NULL, szCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    else
    {
        DWORD dwError = GetLastError();
        Debug::Put(L"CreateProcess(%s) failed. error=%lu(%s)", szCommandLine, dwError, ErrorMessage::Get(dwError));
        return false;
    }
}


// Builds the command line string to get RUNDLL32 to run RunServer function.
// Passes the GUID to RunServer as the 1st argument.
// The path of this DLL needs to be a short path name to exclude any spaces in it.
bool Client::BuildCommandLine(int bitness, PWCHAR pCommandLine)
{
    WCHAR szPath[MAX_PATH];
    if (!GetModuleFileNameW(m_hModule, szPath, MAX_PATH))
    {
        DWORD dwError = GetLastError();
        Debug::Put(L"GetModuleFileName(KeyboardMouse%d.dll) failed. error=%lu(%s)", BITNESS, dwError, ErrorMessage::Get(dwError));
        return false;
    }

    if (sizeof(void*) * 8 != static_cast<size_t>(bitness))
    {
        WCHAR szPath2[MAX_PATH];
        if (!GetLongPathNameW(szPath, szPath2, MAX_PATH))
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"GetLongPathName(KeyboardMouse%d.dll) failed. error=%lu(%s)", BITNESS, dwError, ErrorMessage::Get(dwError));
            return false;
        }
        PWCHAR pEnd = szPath2 + wcslen(szPath2);
        if (towupper(pEnd[-1]) == L'L'
            && towupper(pEnd[-2]) == L'L'
            && towupper(pEnd[-3]) == L'D'
            && towupper(pEnd[-4]) == L'.'
            && iswdigit(pEnd[-5])
            && iswdigit(pEnd[-6]))
        {
            pEnd[-6] = L'\0';
        }
        else
        {
            DBGPUT(L"The file name of the current module is not the expected one.");
            return false;
        }
        _snwprintf_s(szPath, _TRUNCATE, L"%s%d.dll", szPath2, bitness);
    }

    WCHAR szShortPath[MAX_PATH];
    if (!GetShortPathNameW(szPath, szShortPath, MAX_PATH))
    {
        DWORD dwError = GetLastError();
        Debug::Put(L"GetShortPathName(%s) failed. error=%lu(%s)", szPath, dwError, ErrorMessage::Get(dwError));
        return false;
    }

    WCHAR szRundll[MAX_PATH];
    _snwprintf_s(szRundll, MAX_PATH, _TRUNCATE, L"%s\\System32\\rundll32.exe", Path::GetKnownFolder(FOLDERID_Windows));

    _snwprintf_s(pCommandLine, MAX_PATH * 2, _TRUNCATE, L"%s %s,RunServer %s", szRundll, szShortPath, m_pszGuid);

    return true;
}


// Posts a WM_CLOSE to each server's message-only window to end the main procedure of the server.
// Monitors the termination of each server process for SHUTDOWN_TIMEOUT milliseconds at most.
// Finally, force-terminates the servers that are not ended.
void Client::StopServer()
{
    DBGFNC(L"KeyboardMouse%d::Client::StopServer", BITNESS);

    WindowsHandle h64;
    WindowsHandle h32;
    DWORD dwCount = 0;
    HANDLE hh[2] = { 0 };
    DWORD ii[2] = { 0 };

    if (m_pServerBlock->dwProcessId64)
    {
        *(&h64) = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, m_pServerBlock->dwProcessId64);
        if (h64)
        {
            hh[dwCount] = h64;
            ii[dwCount] = m_pServerBlock->dwProcessId64;
            dwCount++;
        }
        else
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"OpenProcess(KeyboardMouse64) failed. error=%lu(%s)", dwError, ErrorMessage::Get(dwError));
        }
    }

    if (m_pServerBlock->dwProcessId32)
    {
        *(&h32) = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, m_pServerBlock->dwProcessId32);
        if (h32)
        {
            hh[dwCount] = h32;
            ii[dwCount] = m_pServerBlock->dwProcessId32;
            dwCount++;
        }
        else
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"OpenProcess(KeyboardMouse32) failed. error=%lu(%s)", dwError, ErrorMessage::Get(dwError));
        }
    }

    if (m_pServerBlock->hwndMessage64)
    {
        PostMessageW(reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage64)), WM_CLOSE, 0, 0);
    }

    if (m_pServerBlock->hwndMessage32)
    {
        PostMessageW(reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage32)), WM_CLOSE, 0, 0);
    }

    for (int i = SHUTDOWN_TIMEOUT / SHUTDOWN_WAIT_INTERVAL; dwCount > 0 && i > 0; i--)
    {
        DWORD dwRet = WaitForMultipleObjects(dwCount, hh, FALSE, SHUTDOWN_WAIT_INTERVAL);
        if (dwRet >= WAIT_OBJECT_0 + 0 && dwRet < WAIT_OBJECT_0 + dwCount)
        {
            DWORD dwIndex = dwRet - WAIT_OBJECT_0;
            if (!dwIndex && dwCount == 2)
            {
                hh[0] = hh[1];
                ii[0] = ii[1];
            }
            dwCount--;
        }
        else if (dwRet == WAIT_TIMEOUT)
        {
            continue;
        }
        else
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"WaitForMultipleObjects failed. error=%lu(%s)", dwError, ErrorMessage::Get(dwError));
            break;
        }
    }

    for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
    {
        if (TerminateProcess(hh[dwIndex], 127))
        {
            DBGPUT(L"TerminateProcess(PID=%lu)", ii[dwIndex]);
        }
        else
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"TerminateProcess(PID=%lu) failed. error=%lu(%s)", ii[dwIndex], dwError, ErrorMessage::Get(dwError));
        }
    }
}


DWORD Client::GetBlockInputFlags()
{
    return (m_pServerBlock->bBlockKeybd ? KEYBOARD_FLAG_BLOCK : 0) | (m_pServerBlock->bBlockMouse ? MOUSE_FLAG_BLOCK : 0);
}


DWORD Client::SetBlockInputFlags(DWORD dwFlags)
{
    DWORD dwFlagsPrev = GetBlockInputFlags();
    m_pServerBlock->bBlockKeybd = (dwFlags & KEYBOARD_FLAG_BLOCK) ? TRUE : FALSE;
    m_pServerBlock->bBlockMouse = (dwFlags & MOUSE_FLAG_BLOCK) ? TRUE : FALSE;
    return dwFlagsPrev;
}


BOOL Client::SetToggleKeySequence(PCWSTR psz)
{
    int errors = -1;
    int parsed = m_pServerBlock->ToggleKeySequence.Parse(psz, &errors);
    return parsed > 0 && errors == 0 ? TRUE : FALSE;
}


BOOL Client::StartAgent(HWND hwnd)
{
    LRESULT lRet = FALSE;
    if (hwnd)
    {
        int bitness = WindowsPlatform::ProcessBitness(WindowHelper(hwnd).ProcessId);
        if (bitness == 64)
        {
            lRet = SendMessageW(reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage64)), WM_APP_START_AGENT, reinterpret_cast<WPARAM>(hwnd), 0);
        }
        else if (bitness == 32)
        {
            lRet = SendMessageW(reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage32)), WM_APP_START_AGENT, reinterpret_cast<WPARAM>(hwnd), 0);
        }
    }
    return lRet ? TRUE : FALSE;
}


BOOL Client::StopAgent(HWND hwnd)
{
    LRESULT lRet = FALSE;
    if (hwnd)
    {
        int bitness = WindowsPlatform::ProcessBitness(WindowHelper(hwnd).ProcessId);
        if (bitness == 64 && m_pServerBlock->hwndMessage64)
        {
            lRet = SendMessageW(reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage64)), WM_APP_STOP_AGENT, reinterpret_cast<WPARAM>(hwnd), 0);
        }
        else if (bitness == 32 && m_pServerBlock->hwndMessage32)
        {
            lRet = SendMessageW(reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage32)), WM_APP_STOP_AGENT, reinterpret_cast<WPARAM>(hwnd), 0);
        }
    }
    else
    {
        if (m_pServerBlock->hwndMessage64)
        {
            lRet = SendMessageW(reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage64)), WM_APP_STOP_AGENT, 0, 0);
        }
        if (m_pServerBlock->hwndMessage32)
        {
            lRet = SendMessageW(reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage32)), WM_APP_STOP_AGENT, 0, 0);
        }
    }
    return lRet ? TRUE : FALSE;
}


DWORD Client::GetKeyboardState(HWND hwnd, DWORD dwMask)
{
    if (hwnd)
    {
        HWND hwndServer;
        int bitness = WindowsPlatform::ProcessBitness(WindowHelper(hwnd).ProcessId);
        if (bitness == 64)
        {
            hwndServer = reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage64));
        }
        else if (bitness == 32)
        {
            hwndServer = reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage32));
        }
        else
        {
            return 0;
        }
        LRESULT language;
        if ((dwMask & KEYBOARD_FLAG_LANGUAGE))
        {
            language = SendMessageW(hwndServer, WM_APP_GET_LANGUAGE, reinterpret_cast<WPARAM>(hwnd), 0);
            if (language == -1)
            {
                language = 0;
            }
        }
        else
        {
            language = 0;
        }
        LRESULT openclose;
        if ((dwMask & KEYBOARD_FLAG_OPENCLOSE))
        {
            openclose = SendMessageW(hwndServer, WM_APP_GET_OPENCLOSE, reinterpret_cast<WPARAM>(hwnd), 0);
            if (openclose == -1)
            {
                openclose = 0;
            }
        }
        else
        {
            openclose = 0;
        }
        LRESULT inputmode;
        if ((dwMask & KEYBOARD_FLAG_INPUTMODE))
        {
            inputmode = SendMessageW(hwndServer, WM_APP_GET_INPUTMODE, reinterpret_cast<WPARAM>(hwnd), 0);
            if (inputmode == -1)
            {
                inputmode = 0;
            }
        }
        else
        {
            inputmode = 0;
        }
        return KEYBOARD_STATE_PACK(DWORD, language, openclose, inputmode);
    }
    else
    {
        return 0;
    }
}


DWORD Client::SetKeyboardState(HWND hwnd, DWORD dwState, DWORD dwMask)
{
    if (hwnd)
    {
        HWND hwndServer;
        int bitness = WindowsPlatform::ProcessBitness(WindowHelper(hwnd).ProcessId);
        if (bitness == 64)
        {
            hwndServer = reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage64));
        }
        else if (bitness == 32)
        {
            hwndServer = reinterpret_cast<HWND>(static_cast<DWORD_PTR>(m_pServerBlock->hwndMessage32));
        }
        else
        {
            return 0;
        }
        LRESULT language;
        if ((dwMask & KEYBOARD_FLAG_LANGUAGE))
        {
            language = SendMessageW(hwndServer, WM_APP_SET_LANGUAGE, reinterpret_cast<WPARAM>(hwnd), KEYBOARD_STATE_UNPACK_LANGUAGE(dwState));
            if (language == -1)
            {
                language = 0;
            }
        }
        else
        {
            language = 0;
        }
        LRESULT openclose;
        if ((dwMask & KEYBOARD_FLAG_OPENCLOSE))
        {
            openclose = SendMessageW(hwndServer, WM_APP_SET_OPENCLOSE, reinterpret_cast<WPARAM>(hwnd), KEYBOARD_STATE_UNPACK_OPENCLOSE(dwState));
            if (openclose == -1)
            {
                openclose = 0;
            }
        }
        else
        {
            openclose = 0;
        }
        LRESULT inputmode;
        if ((dwMask & KEYBOARD_FLAG_INPUTMODE))
        {
            inputmode = SendMessageW(hwndServer, WM_APP_SET_INPUTMODE, reinterpret_cast<WPARAM>(hwnd), KEYBOARD_STATE_UNPACK_INPUTMODE(dwState));
            if (inputmode == -1)
            {
                inputmode = 0;
            }
        }
        else
        {
            inputmode = 0;
        }
        return KEYBOARD_STATE_PACK(DWORD, language, openclose, inputmode);
    }
    else
    {
        return 0;
    }
}
