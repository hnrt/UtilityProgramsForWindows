#pragma once


#include <map>
#include "hnrt/StringLessThan.h"
#include "hnrt/IMsiFileRowCollection.h"


namespace hnrt
{
    class MsiFileRowCollection
        : public IMsiFileRowCollection
    {
    public:

        MsiFileRowCollection();
        MsiFileRowCollection(const MsiFileRowCollection&) = delete;
        virtual ~MsiFileRowCollection() = default;
        void operator =(const MsiFileRowCollection&) = delete;
        virtual long AddRef();
        virtual long Release();
        virtual long get_Count() const;
        virtual void Add(PCWSTR pszFile, long rowNo);
        virtual void Remove(PCWSTR pszFile);
        virtual long Find(PCWSTR pszFile) const;

    private:

        typedef std::map<PCWSTR, long, StringLessThan> Map;
        typedef std::pair<PCWSTR, long> MapEntry;

        long m_refs;
        Map m_map;
    };
}
