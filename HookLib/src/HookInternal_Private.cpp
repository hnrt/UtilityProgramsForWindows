#include "pch.h"
#include "HookInternal.h"
#include "hnrt/WindowsHandle.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Debug.h"
#include "hnrt/ErrorMessage.h"


#pragma comment(lib, "Dbghelp.lib")


using namespace hnrt;


void HookInternal::Install()
{
    WindowsHandle hSnapshot(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId()));
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        throw Win32Exception(GetLastError(), L"CreateToolhelp32Snapshot failed.");
    }
    MODULEENTRY32W entry = { 0 };
    entry.dwSize = static_cast<DWORD>(sizeof(entry));
    if (!Module32FirstW(hSnapshot, &entry))
    {
        throw Win32Exception(GetLastError(), L"Module32First failed.");
    }
    PBYTE pKERNEL32 = reinterpret_cast<PBYTE>(GetModuleHandleW(L"KERNEL32.DLL"));
    do
    {
        if (entry.modBaseAddr == pKERNEL32)
        {
            continue;
        }
        Install(entry.modBaseAddr, entry.szModule);
    } while (Module32NextW(hSnapshot, &entry));
    DWORD dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES)
    {
        throw Win32Exception(dwError, L"Module32Next failed.");
    }
}


void HookInternal::Install(PBYTE pBase, PCWSTR pszName)
{
    SpinLock x(m_Lock);
    ModuleOverrideMap::iterator iter = m_ModuleOverrideMap.find(pBase);
    if (iter != m_ModuleOverrideMap.end())
    {
        return;
    }
    ULONG size = 0UL;
    PIMAGE_IMPORT_DESCRIPTOR pDesc = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(ImageDirectoryEntryToData(pBase, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size));
    if (!pDesc)
    {
        DWORD dwError = GetLastError();
        if (dwError)
        {
            Debug::Put(L"Hook::install: ImageDirectoryEntryToData failed: %lu(%s)", dwError, ErrorMessage::Get(dwError));
        }
        return;
    }
    OverrideMap* pMap = new OverrideMap();
    m_ModuleOverrideMap.insert(ModuleOverrideEntry(pBase, pMap));
    while (pDesc->Name)
    {
        Install(pBase, pszName, pDesc, *pMap);
        pDesc++;
    }
}


void HookInternal::Install(PBYTE pBase, PCWSTR pszName, PIMAGE_IMPORT_DESCRIPTOR pDesc, OverrideMap& overrideMap)
{
    PIMAGE_THUNK_DATA pThunk = reinterpret_cast<PIMAGE_THUNK_DATA>(pBase + pDesc->FirstThunk);
    PIMAGE_THUNK_DATA pOrgThunk = reinterpret_cast<PIMAGE_THUNK_DATA>(pBase + pDesc->OriginalFirstThunk);
    while (pThunk->u1.Function)
    {
        Install(pBase, pszName, pDesc, pThunk, pOrgThunk, overrideMap);
        pThunk++;
        pOrgThunk++;
    }
}


void HookInternal::Install(PBYTE pBase, PCWSTR pszName, PIMAGE_IMPORT_DESCRIPTOR pDesc, PIMAGE_THUNK_DATA pThunk, PIMAGE_THUNK_DATA pOrgThunk, OverrideMap& overrideMap)
{
    char* szModuleName = reinterpret_cast<char*>(pBase + pDesc->Name);
    if (IMAGE_SNAP_BY_ORDINAL(pOrgThunk->u1.Ordinal))
    {
        return;
    }
    PIMAGE_IMPORT_BY_NAME pImg = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(pBase + pOrgThunk->u1.AddressOfData);
    void* pfn = getReplacement(pImg->Name);
    if (!pfn)
    {
        DBGPUT(L"Hook::Install: %s::%hs::%hs", pszName, szModuleName, pImg->Name);
        return;
    }
    DWORD dwOrgProtect = 0UL;
    if (!VirtualProtect(&pThunk->u1.Function, sizeof(pThunk->u1.Function), PAGE_READWRITE, &dwOrgProtect))
    {
        DWORD dwError = GetLastError();
        Debug::Put(L"Hook::Install: %s::%hs::%hs: VirtualProtect(READWRITE) failed: %lu(%s)", pszName, szModuleName, pImg->Name, dwError, ErrorMessage::Get(dwError));
        return;
    }
    overrideMap.insert(OverrideEntry(&pThunk->u1.Function, _InterlockedExchange(&pThunk->u1.Function, reinterpret_cast<ULONGLONG>(pfn))));
    DBGPUT(L"Hook::Install: %s::%hs::%hs [HOOKED]", pszName, szModuleName, pImg->Name);
    DWORD dwNewProtect = 0UL;
    if (!VirtualProtect(&pThunk->u1.Function, sizeof(pThunk->u1.Function), dwOrgProtect, &dwNewProtect))
    {
        DWORD dwError = GetLastError();
        Debug::Put(L"Hook::Install: %s::%hs::%hs: VirtualProtect(0x%lX) failed: %lu(%s)", pszName, szModuleName, pImg->Name, dwOrgProtect, dwError, ErrorMessage::Get(dwError));
    }
}


void HookInternal::Uninstall()
{
    WindowsHandle hSnapshot(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId()));
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        throw Win32Exception(GetLastError(), L"CreateToolhelp32Snapshot failed.");
    }
    MODULEENTRY32W entry = { 0 };
    entry.dwSize = static_cast<DWORD>(sizeof(entry));
    if (!Module32FirstW(hSnapshot, &entry))
    {
        throw Win32Exception(GetLastError(), L"Module32First failed.");
    }
    PBYTE pKERNEL32 = reinterpret_cast<PBYTE>(GetModuleHandleW(L"KERNEL32.DLL"));
    do
    {
        if (entry.modBaseAddr == pKERNEL32)
        {
            continue;
        }
        Uninstall(entry.modBaseAddr, entry.szModule);
    } while (Module32NextW(hSnapshot, &entry));
    DWORD dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES)
    {
        throw Win32Exception(dwError, L"Module32Next failed.");
    }
}


void HookInternal::Uninstall(PBYTE pBase, PCWSTR pszName)
{
    SpinLock x(m_Lock);
    ModuleOverrideMap::iterator iter = m_ModuleOverrideMap.find(pBase);
    if (iter == m_ModuleOverrideMap.end())
    {
        return;
    }
    OverrideMap* pMap = iter->second;
    m_ModuleOverrideMap.erase(iter);
    Uninstall(pBase, pszName, *pMap);
    delete pMap;
}


void HookInternal::Uninstall(PBYTE pBase, PCWSTR pszName, const OverrideMap& map) const
{
    for (OverrideMap::const_iterator iter = map.cbegin(); iter != map.cend(); iter++)
    {
        ULONGLONG* pFunction = iter->first;
        ULONGLONG aFunction = iter->second;
        DWORD dwOrgProtect = 0UL;
        if (!VirtualProtect(pFunction, sizeof(ULONGLONG), PAGE_READWRITE, &dwOrgProtect))
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"Hook::Uninstall: %s::%p VirtualProtect(READWRITE) failed: %lu(%s)", pszName, pFunction, dwError, ErrorMessage::Get(dwError));
            continue;
        }
        *pFunction = aFunction;
        DBGPUT(L"Hook::Uninstall: %s::%p [UNHOOKED]", pszName, pFunction);
        DWORD dwNewProtect = 0UL;
        if (!VirtualProtect(pFunction, sizeof(ULONGLONG), dwOrgProtect, &dwNewProtect))
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"Hook::Uninstall: %s::%p VirtualProtect(0x%lX) failed: %lu(%s)", pszName, pFunction, dwOrgProtect, dwError, ErrorMessage::Get(dwError));
        }
    }
}
