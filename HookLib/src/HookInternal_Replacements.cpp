#include "pch.h"
#include "HookInternal.h"
#include "hnrt/String.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Debug.h"


using namespace hnrt;


HookInternal& HookInternal::Instance()
{
    return reinterpret_cast<HookInternal&>(Hook::Instance());
}


////////////////////////////////////////////////////////////////////////////////
//
//   L O A D L I B R A R Y   I N T E R F A C E
//
////////////////////////////////////////////////////////////////////////////////


typedef HMODULE(WINAPI* PFNLOADLIBRARYA)(LPCSTR);
typedef HMODULE(WINAPI* PFNLOADLIBRARYW)(LPCWSTR);
typedef HMODULE(WINAPI* PFNLOADLIBRARYEXA)(LPCSTR, HANDLE, DWORD);
typedef HMODULE(WINAPI* PFNLOADLIBRARYEXW)(LPCWSTR, HANDLE, DWORD);


HMODULE WINAPI HookInternal::hookLoadLibraryA(LPCSTR lpFileName)
{
    DBGPUT(L"hookLoadLibraryA(%hs)", lpFileName);
    PFNLOADLIBRARYA pfn = reinterpret_cast<PFNLOADLIBRARYA>(Instance().getOriginal("LoadLibraryA"));
    HMODULE hModule = (*pfn)(lpFileName);
    if (hModule)
    {
        Instance().Install(reinterpret_cast<PBYTE>(hModule), String(CP_ACP, lpFileName));
    }
    return hModule;
}


HMODULE WINAPI HookInternal::hookLoadLibraryW(LPCWSTR lpFileName)
{
    DBGPUT(L"hookLoadLibraryW(%s)", lpFileName);
    PFNLOADLIBRARYW pfn = reinterpret_cast<PFNLOADLIBRARYW>(Instance().getOriginal("LoadLibraryW"));
    HMODULE hModule = (*pfn)(lpFileName);
    if (hModule)
    {
        Instance().Install(reinterpret_cast<PBYTE>(hModule), lpFileName);
    }
    return hModule;
}


HMODULE WINAPI HookInternal::hookLoadLibraryExA(LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
    DBGPUT(L"hookLoadLibraryExA(%hs,%p,%xh)", lpFileName, hFile, dwFlags);
    PFNLOADLIBRARYEXA pfn = reinterpret_cast<PFNLOADLIBRARYEXA>(Instance().getOriginal("LoadLibraryExA"));
    HMODULE hModule = (*pfn)(lpFileName, hFile, dwFlags);
    if (hModule && (dwFlags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE)) == 0)
    {
        Instance().Install(reinterpret_cast<PBYTE>(hModule), String(CP_ACP, lpFileName));
    }
    return hModule;
}


HMODULE WINAPI HookInternal::hookLoadLibraryExW(LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
    DBGPUT(L"hookLoadLibraryExW(%s,%p,%xh)", lpFileName, hFile, dwFlags);
    PFNLOADLIBRARYEXW pfn = reinterpret_cast<PFNLOADLIBRARYEXW>(Instance().getOriginal("LoadLibraryExW"));
    HMODULE hModule = (*pfn)(lpFileName, hFile, dwFlags);
    if (hModule && (dwFlags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE)) == 0)
    {
        Instance().Install(reinterpret_cast<PBYTE>(hModule), lpFileName);
    }
    return hModule;
}


////////////////////////////////////////////////////////////////////////////////
//
//   T I M E   I N T E R F A C E
//
////////////////////////////////////////////////////////////////////////////////


typedef void (WINAPI *PFNGETSYSTEMTIMEASFILETIME)(LPFILETIME);


void WINAPI HookInternal::hookGetSystemTimeAsFileTime(LPFILETIME pFT)
{
    PFNGETSYSTEMTIMEASFILETIME pfn = reinterpret_cast<PFNGETSYSTEMTIMEASFILETIME>(Instance().getOriginal("GetSystemTimeAsFileTime"));
    (*pfn)(pFT);
    Instance().ApplyTimeDisplacementTo(*pFT);
    DBGPUT(L"hookGetSystemTimeAsFileTime: %08lX%08lX", pFT->dwHighDateTime, pFT->dwLowDateTime);
}


void WINAPI HookInternal::hookGetSystemTime(LPSYSTEMTIME pST)
{
    FILETIME ft = { 0 };
    PFNGETSYSTEMTIMEASFILETIME pfn = reinterpret_cast<PFNGETSYSTEMTIMEASFILETIME>(Instance().getOriginal("GetSystemTimeAsFileTime"));
    (*pfn)(&ft);
    Instance().ApplyTimeDisplacementTo(ft);
    FileTimeToSystemTime(&ft, pST);
    DBGPUT(L"hookGetSystemTime: %d-%02d-%02d %02d:%02d:%02d.%03d", pST->wYear, pST->wMonth, pST->wDay, pST->wHour, pST->wMinute, pST->wSecond, pST->wMilliseconds);
}


void WINAPI HookInternal::hookGetLocalTime(LPSYSTEMTIME pST)
{
    FILETIME ft = { 0 };
    PFNGETSYSTEMTIMEASFILETIME pfn = reinterpret_cast<PFNGETSYSTEMTIMEASFILETIME>(Instance().getOriginal("GetSystemTimeAsFileTime"));
    (*pfn)(&ft);
    Instance().ApplyTimeDisplacementTo(ft);
    FileTimeToSystemTime(&ft, pST);
    Instance().ApplyTimeZoneTo(*pST);
    DBGPUT(L"hookGetLocalTime: %d-%02d-%02d %02d:%02d:%02d.%03d", pST->wYear, pST->wMonth, pST->wDay, pST->wHour, pST->wMinute, pST->wSecond, pST->wMilliseconds);
}


////////////////////////////////////////////////////////////////////////////////
//
//   M A P   I N I T I A L I Z A T I O N
//
////////////////////////////////////////////////////////////////////////////////


void HookInternal::InitializeMaps()
{
    m_Replacements.insert(Entry("LoadLibraryA", &hookLoadLibraryA));
    m_Replacements.insert(Entry("LoadLibraryW", &hookLoadLibraryW));
    m_Replacements.insert(Entry("LoadLibraryExA", &hookLoadLibraryExA));
    m_Replacements.insert(Entry("LoadLibraryExW", &hookLoadLibraryExW));
    m_Replacements.insert(Entry("GetSystemTimeAsFileTime", &hookGetSystemTimeAsFileTime));
    m_Replacements.insert(Entry("GetSystemTime", &hookGetSystemTime));
    m_Replacements.insert(Entry("GetLocalTime", &hookGetLocalTime));
    HMODULE hKERNEL32 = GetModuleHandleW(L"KERNEL32.dll");
    if (!hKERNEL32)
    {
        throw Win32Exception(GetLastError(), L"GetModuleHandle(KERNEL32) failed.");
    }
    for (Map::const_iterator iter = m_Replacements.cbegin(); iter != m_Replacements.cend(); iter++)
    {
        FARPROC pfn = GetProcAddress(hKERNEL32, iter->first);
        if (!pfn)
        {
            throw Win32Exception(GetLastError(), L"GetProcAddress(KERNEL32::%hs) failed.", iter->first);
        }
        m_Originals.insert(Entry(iter->first, pfn));
    }
}
