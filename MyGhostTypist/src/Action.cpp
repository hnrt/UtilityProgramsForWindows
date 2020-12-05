#include "Action.h"
#include <string.h>
#include "hnrt/String.h"
#include "hnrt/VirtualKey.h"


using namespace hnrt;


int Action::GetClass(PCWSTR psz)
{
    if (!wcsncmp(psz, L"VK_", 3))
    {
        return VirtualKey(psz).Value ? AC_TYPEKEY : AC_UNKNOWN;
    }
    else if (!wcsncmp(psz, L"unicode:", 8))
    {
        return AC_TYPEUNICODE;
    }
    else if (!wcsncmp(psz, L"username", 8))
    {
        return psz[8] == L'\0' || psz[8] == L':' ? AC_TYPEUSERNAME : AC_UNKNOWN;
    }
    else if (!wcsncmp(psz, L"password", 8))
    {
        return psz[8] == L'\0' || psz[8] == L':' ? AC_TYPEPASSWORD : AC_UNKNOWN;
    }
    else if (!wcscmp(psz, L"delete"))
    {
        return AC_TYPEDELETESEQUENCE;
    }
    else
    {
        return AC_UNKNOWN;
    }
}


PCWSTR Action::GetValue(PCWSTR psz)
{
    PCWSTR pColon = wcschr(psz, L':');
    return pColon ? pColon + 1 : nullptr;
}


PCWSTR Action::TypeUnicode(PCWSTR psz)
{
    return String::Format(L"unicode:%s", psz);
}


PCWSTR Action::TypeUsername(PCWSTR pszName)
{
    return String::Format(pszName && *pszName ? L"username:%s" : L"username", pszName);
}


PCWSTR Action::TypePassword(PCWSTR pszName)
{
    return String::Format(pszName && *pszName ? L"password:%s" : L"password", pszName);
}


PCWSTR Action::TypeDeleteSequence()
{
    return String::Copy(L"delete");
}
