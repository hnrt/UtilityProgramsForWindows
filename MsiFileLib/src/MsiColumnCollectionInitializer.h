#pragma once


#include <Msi.h>
#include <map>
#include "hnrt/String.h"
#include "MsiColumn.h"


namespace hnrt
{
    class MsiColumnCollectionInitializer
    {
    public:

        MsiColumnCollectionInitializer();
        MsiColumnCollectionInitializer(const MsiColumnCollectionInitializer&) = delete;
        ~MsiColumnCollectionInitializer() = default;
        void operator =(const MsiColumnCollectionInitializer&) = delete;
        void Run(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count) const;

    private:

        static void InitGeneric(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count);
        static void Init_Tables(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count);
        static void Init_Columns(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count);
        static void Init_Streams(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count);
        static void Init_Storages(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count);
        static void Init_TransformView(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count);

        typedef void (*Initialize)(MSIHANDLE hDatabase, PCWSTR pszTableName, MsiColumn*& columns, long& count);
        typedef std::map<PCWSTR, Initialize, StringLessThan> InitMap;
        typedef std::pair<PCWSTR, Initialize> InitMapEntry;

        InitMap m_map;
    };
}
