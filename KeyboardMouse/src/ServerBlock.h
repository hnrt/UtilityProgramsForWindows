#pragma once


#include <Windows.h>
#include "hnrt/KeyboardBitMap.h"


namespace hnrt
{
    struct ServerBlock
    {
        DWORD dwClientProcessId;
        DWORD dwProcessId64;
        DWORD hwndMessage64;
        DWORD dwProcessId32;
        DWORD hwndMessage32;
        BOOL bBlockKeybd;
        BOOL bBlockMouse;
        BOOL bBlockInputPaused;
        KeyboardBitMap ToggleKeySequence;
    };
}
