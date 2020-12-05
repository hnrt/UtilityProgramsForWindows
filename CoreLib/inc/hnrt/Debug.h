#pragma once


#include <Windows.h>
#include <stdarg.h>


namespace hnrt
{
    class Debug
    {
    public:

        static void Put(PCWSTR pszFormat, ...);
        static void VaPut(PCWSTR pszFormat, va_list argList);
    };

    class DebugFunc
    {
    public:

        DebugFunc(PCWSTR pszFormat, ...);
        DebugFunc(const DebugFunc&) = delete;
        ~DebugFunc();
        DebugFunc& operator =(const DebugFunc&) = delete;
    };

    #ifdef _DEBUG
    #define DBGPUT(f,...) Debug::Put(f,__VA_ARGS__)
    #define DBGFNC(f,...) DebugFunc func_790F6FA8_AFC7_4A8B_AF30_5838D03ED1FA(f,__VA_ARGS__)
    #else //_DEBUG
    #define DBGPUT(f,...) (void)0
    #define DBGFNC(f,...) (void)0
    #endif //_DEBUG
}
