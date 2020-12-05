#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/ResourceReader.h"


using namespace hnrt;


ResourceReader::~ResourceReader()
{
    if (m_hModule)
    {
        FreeLibrary(m_hModule);
    }
}


ResourceReader::ResourceReader()
    : m_pszFileName(nullptr)
    , m_hModule(nullptr)
{

}


bool ResourceReader::Load(PCWSTR pszFileName)
{
    if (m_hModule)
    {
        FreeLibrary(m_hModule);
    }
    m_pszFileName = String::Copy(pszFileName);
    m_hModule = LoadLibraryExW(m_pszFileName, NULL, LOAD_LIBRARY_AS_DATAFILE);
    return m_hModule ? true : false;
}


bool ResourceReader::ForEach(IResourceForEachCallback& callback)
{
    struct LocalContext
    {
        ResourceReader& Owner;
        IResourceForEachCallback& Callback;

        LocalContext(ResourceReader& owner, IResourceForEachCallback& callback)
            : Owner(owner)
            , Callback(callback)
        {
        }

        static BOOL CALLBACK EnumResType(HMODULE hModule, LPWSTR lpszType, LONG_PTR lParam)
        {
            if (!EnumResourceNamesW(hModule, lpszType, LocalContext::EnumResName, lParam))
            {
                throw Win32Exception(GetLastError(), L"EnumResourceNamesW failed.");
            }

            return TRUE;
        }

        static BOOL CALLBACK EnumResName(HMODULE hModule, LPCWSTR lpszType, LPWSTR lpszName, LONG_PTR lParam)
        {
            LocalContext& ctx = *reinterpret_cast<LocalContext*>(lParam);

            ctx.Callback.ResourceForEach(lpszType, lpszName, ctx.Owner);

            return TRUE;
        }

    } ctx(*this, callback);

    try
    {
        return EnumResourceTypesW(m_hModule, LocalContext::EnumResType, reinterpret_cast<LONG_PTR>(&ctx)) ? true : false;
    }
    catch (Win32Exception ex)
    {
        SetLastError(ex.Error);
        return false;
    }
}


bool ResourceReader::ForEach(PCWSTR pszType, IResourceForEachCallback& callback)
{
    struct LocalContext
    {
        ResourceReader& Owner;
        IResourceForEachCallback& Callback;
        PCWSTR pszType;

        LocalContext(ResourceReader& owner, PCWSTR pszType_, IResourceForEachCallback& callback)
            : Owner(owner)
            , pszType(pszType_)
            , Callback(callback)
        {
        }

        static BOOL CALLBACK EnumResType(HMODULE hModule, LPWSTR lpszType, LONG_PTR lParam)
        {
            LocalContext& ctx = *reinterpret_cast<LocalContext*>(lParam);

            if (TypeMatch(lpszType, ctx.pszType))
            {
                if (!EnumResourceNamesW(hModule, lpszType, LocalContext::EnumResName, lParam))
                {
                    throw Win32Exception(GetLastError(), L"EnumResourceNamesW failed.");
                }
            }

            return TRUE;
        }

        static BOOL CALLBACK EnumResName(HMODULE hModule, LPCWSTR lpszType, LPWSTR lpszName, LONG_PTR lParam)
        {
            LocalContext& ctx = *reinterpret_cast<LocalContext*>(lParam);

            ctx.Callback.ResourceForEach(lpszType, lpszName, ctx.Owner);

            return TRUE;
        }

        static bool TypeMatch(PCWSTR pszType1, PCWSTR pszType2)
        {
            if (IS_INTRESOURCE(pszType1))
            {
                if (IS_INTRESOURCE(pszType1))
                {
                    return reinterpret_cast<INT_PTR>(pszType1) == reinterpret_cast<INT_PTR>(pszType2);
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return !wcscmp(pszType1, pszType2);
            }
        }

    } ctx(*this, pszType, callback);

    try
    {
        return EnumResourceTypesW(m_hModule, LocalContext::EnumResType, reinterpret_cast<LONG_PTR>(&ctx)) ? true : false;
    }
    catch (Win32Exception ex)
    {
        SetLastError(ex.Error);
        return false;
    }
}


bool ResourceReader::GetData(PCWSTR pszType, PCWSTR pszName, PVOID& pBlock, DWORD& dwSize)
{
    HRSRC hResource = FindResourceW(m_hModule, pszName, pszType);
    if (!hResource)
    {
        return false;
    }

    dwSize = SizeofResource(m_hModule, hResource);
    if (!dwSize)
    {
        return false;
    }

    HGLOBAL hBlock = LoadResource(m_hModule, hResource);
    if (!hBlock)
    {
        return false;
    }

    pBlock = LockResource(hBlock);
    return pBlock ? true : false;
}
