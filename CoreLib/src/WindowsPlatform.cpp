#include "pch.h"
#include "hnrt/WindowsPlatform.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/WindowsHandle.h"


typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);


using namespace hnrt;


bool WindowsPlatform::Is64bit()
{
    return Is64bitProcess() || IsWow64Process();
}


bool WindowsPlatform::Is32bit()
{
    return Is32bitProcess() && !IsWow64Process();
}


bool WindowsPlatform::Is64bitProcess()
{
    return sizeof(void*) == 8;
}


bool WindowsPlatform::Is32bitProcess()
{
    return sizeof(void*) == 4;
}


bool WindowsPlatform::IsWow64Process()
{
    bool bIsWow64;
    DWORD dwError = IsWow64Process(GetCurrentProcess(), bIsWow64);
    if (dwError != ERROR_SUCCESS)
    {
        throw Win32Exception(dwError, L"IsWow64Process(CurrentProcess): Failed.");
    }
    return bIsWow64;
}


DWORD WindowsPlatform::Is64bitProcess(HANDLE hProcess, bool& bRet)
{
    if (Is32bit())
    {
        bRet = false;
        return ERROR_SUCCESS;
    }
    else
    {
        DWORD dwError = IsWow64Process(hProcess, bRet);
        if (dwError == ERROR_SUCCESS)
        {
            bRet = !bRet;
        }
        return dwError;
    }
}


DWORD WindowsPlatform::Is32bitProcess(HANDLE hProcess, bool& bRet)
{
    if (Is32bit())
    {
        bRet = true;
        return ERROR_SUCCESS;
    }
    else
    {
        return IsWow64Process(hProcess, bRet);
    }
}


DWORD WindowsPlatform::IsWow64Process(HANDLE hProcess, bool& bRet)
{
    HMODULE hKernel32 = GetModuleHandleA("kernel32");
    if (!hKernel32)
    {
        throw Win32Exception(GetLastError(), L"GetModuleHandle(kernel32) failed.");
    }

    //IsWow64Process is not available on all supported versions of Windows.
    //Use GetModuleHandle to get a handle to the DLL that contains the function
    //and GetProcAddress to get a pointer to the function if available.

    LPFN_ISWOW64PROCESS fnIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(GetProcAddress(hKernel32, "IsWow64Process"));
    if (fnIsWow64Process != NULL)
    {
        BOOL bIsWow64 = FALSE;
        if (!fnIsWow64Process(hProcess, &bIsWow64))
        {
            DWORD dwError = GetLastError();
            if (dwError == ERROR_SUCCESS)
            {
                throw Exception(L"IsWow64Process failed withERROR_SUCCESS.");
            }
            return dwError;
        }
        bRet = bIsWow64 ? true : false;
    }
    else
    {
        bRet = false;
    }

    return ERROR_SUCCESS;
}


int WindowsPlatform::Bitness()
{
    return Is64bit() ? 64 : 32;
}


int WindowsPlatform::ProcessBitness()
{
    return Is64bitProcess() ? 64 : 32;
}


int WindowsPlatform::ProcessBitness(DWORD dwProcessId)
{
    int bitness = 0;
    WindowsHandle hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (hProcess != NULL)
    {
        bool bIs64bit;
        DWORD dwRet = WindowsPlatform::Is64bitProcess(hProcess, bIs64bit);
        if (dwRet == ERROR_SUCCESS)
        {
            bitness = bIs64bit ? 64 : 32;
        }
    }
    return bitness;
}
