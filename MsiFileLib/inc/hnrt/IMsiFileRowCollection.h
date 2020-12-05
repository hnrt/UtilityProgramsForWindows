#pragma once


namespace hnrt
{
    struct IMsiFileRowCollection
    {
        virtual long AddRef() = 0;
        virtual long Release() = 0;
        virtual long get_Count() const = 0;
        virtual void Add(PCWSTR pszFile, long rowNo) = 0;
        virtual void Remove(PCWSTR pszFile) = 0;
        virtual long Find(PCWSTR pszFile) const = 0;

        __declspec(property(get = get_Count)) long Count;
    };
}
