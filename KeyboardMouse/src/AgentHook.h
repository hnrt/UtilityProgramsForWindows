#pragma once


#include "hnrt/SharedMemory.h"
#include "AgentBlock.h"


namespace hnrt
{
    class AgentHook
    {
    public:

        AgentHook(HMODULE hModule, HWND hwnd, DWORD dwThreadId);
        AgentHook(const AgentHook&) = delete;
        ~AgentHook();
        void operator =(const AgentHook&) = delete;
        const AgentBlock* get_BlockPtr() const;
        AgentBlock* get_BlockPtr();
        __declspec(property(get = get_BlockPtr)) AgentBlock* BlockPtr;

    private:

        SharedMemory<AgentBlock> m_pBlock;
        HHOOK m_hHook;
    };

    inline const AgentBlock* AgentHook::get_BlockPtr() const
    {
        return m_pBlock.operator->();
    }

    inline AgentBlock* AgentHook::get_BlockPtr()
    {
        return m_pBlock.operator->();
    }
}
