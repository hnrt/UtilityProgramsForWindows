#pragma once


#include <Windows.h>


namespace hnrt
{
    struct AgentBlock
    {
        DWORD dwHwnd;
        DWORD dwThreadId;
        DWORD dwFlags;
        LANGID Language;
        LONG KeyboardOpenClose;
        LONG InputModeConversion;
        LANGID PreferredLanguage;
    };
}
