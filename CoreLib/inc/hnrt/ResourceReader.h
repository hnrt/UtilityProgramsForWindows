#pragma once


#include "Path.h"


namespace hnrt
{
    class ResourceReader;

    struct IResourceForEachCallback
    {
        virtual void ResourceForEach(PCWSTR, PCWSTR, ResourceReader&) = 0;
    };

    class ResourceReader
    {
    public:

        virtual ~ResourceReader();
        ResourceReader();
        ResourceReader(const ResourceReader&) = delete;
        void operator =(const ResourceReader&) = delete;
        bool Load(PCWSTR pszFileName);
        bool ForEach(IResourceForEachCallback&);
        bool ForEach(PCWSTR pszType, IResourceForEachCallback&);
        bool GetData(PCWSTR pszType, PCWSTR pszName, PVOID& pBlock, DWORD& dwSize);

    protected:

        PCWSTR m_pszFileName;
        HMODULE m_hModule;
    };
}
