#pragma once


#include "hnrt/Array.h"
#include "hnrt/String.h"


namespace hnrt
{
    class StringCollection
        : public Array<String>
    {
    public:

        StringCollection();
        StringCollection(const StringCollection&);
        ~StringCollection() = default;
        StringCollection& operator =(const StringCollection&);
        StringCollection& Clear();
        StringCollection& Add(PCWSTR psz);
        StringCollection& Add(PCWSTR psz, size_t cch);
        StringCollection& RemoveAt(DWORD dwIndex);
        StringCollection& Split(PCWSTR psz, WCHAR sep, bool bAll = false);
    };
}
