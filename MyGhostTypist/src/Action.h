#pragma once


#include <Windows.h>


#define AC_UNKNOWN 0
#define AC_TYPEKEY 1
#define AC_TYPEUNICODE 2
#define AC_TYPEUSERNAME 3
#define AC_TYPEPASSWORD 4
#define AC_TYPEDELETESEQUENCE 5


namespace hnrt
{
    class Action
    {
    public:

        static int GetClass(PCWSTR psz);
        static PCWSTR GetValue(PCWSTR psz);
        static PCWSTR TypeUnicode(PCWSTR psz);
        static PCWSTR TypeUsername(PCWSTR pszName = nullptr);
        static PCWSTR TypePassword(PCWSTR pszName = nullptr);
        static PCWSTR TypeDeleteSequence();
    };
}
