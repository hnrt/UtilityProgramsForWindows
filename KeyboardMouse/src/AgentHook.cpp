#include "pch.h"
#include "AgentHook.h"
#include "hnrt/Debug.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Impersonator.h"
#include "Agent.h"


using namespace hnrt;


AgentHook::AgentHook(HMODULE hModule, HWND hwnd, DWORD dwThreadId)
    : m_pBlock()
    , m_hHook(nullptr)
{
    try
    {
        {
            // Temporarily impersonate the low integrity so that even a web browser in the protected mode can map this block.
            LowIntegrityImpersonator x;
            Agent::MapSharedBlock(m_pBlock, dwThreadId);
        }
        m_pBlock->dwHwnd = static_cast<DWORD>(reinterpret_cast<DWORD_PTR>(hwnd));
        m_pBlock->dwThreadId = dwThreadId;
        m_pBlock->dwFlags = 0;
        m_hHook = SetWindowsHookExW(WH_CALLWNDPROC, Agent::CallWndProcHook, hModule, dwThreadId);
        if (m_hHook)
        {
            DBGPUT(L"SetWindowsHook(CALLWNDPROC,wnd=%p/thread=%lu) return=%p", hwnd, dwThreadId, m_hHook);
        }
        else
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"SetWindowsHook(CALLWNDPROC,wnd=%p/thread=%lu) failed. error=%lu(%s)", hwnd, dwThreadId, dwError, ErrorMessage::Get(dwError));
        }
    }
    catch (...)
    {
        // ignore any error
    }
}


AgentHook::~AgentHook()
{
    if (m_hHook)
    {
        if (UnhookWindowsHookEx(m_hHook))
        {
            DBGPUT(L"UnhookWindowsHookEx(CALLWNDPROC::%p)", m_hHook);
        }
        else
        {
            DWORD dwError = GetLastError();
            Debug::Put(L"UnhookWindowsHookEx(CALLWNDPROC::%p) failed. error=%lu(%s)", m_hHook, dwError, ErrorMessage::Get(dwError));
        }
    }
    m_pBlock.Unmap();
}
