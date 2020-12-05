#pragma once


#include <Windows.h>


namespace hnrt
{
    struct ErrorMessage
    {
        static PCWSTR Get(DWORD code);
    };
}
