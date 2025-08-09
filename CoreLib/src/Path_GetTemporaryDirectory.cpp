#include "pch.h"
#include "hnrt/Path.h"


using namespace hnrt;


String Path::GetTemporaryDirectory()
{
    WCHAR szPath[MAX_PATH] = { 0 };
    if (GetTempPathW(MAX_PATH, szPath))
    {
        return String(szPath);
    }
    else
    {
        return String(L".");
    }
}


String Path::GetTemporaryDirectory(PCWSTR psz)
{
    return Path::Combine(GetTemporaryDirectory(), psz);
}


String Path::GetTemporaryDirectory(PCWSTR psz1, PCWSTR psz2)
{
    return Path::Combine(GetTemporaryDirectory(), psz1, psz2);
}


String Path::GetTemporaryDirectory(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3)
{
    return Path::Combine(GetTemporaryDirectory(), psz1, psz2, psz3);
}


String Path::GetTemporaryDirectory(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4)
{
    return Path::Combine(GetTemporaryDirectory(), psz1, psz2, psz3, psz4);
}
