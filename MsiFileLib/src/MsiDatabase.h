#pragma once


#include <Windows.h>
#include <Msi.h>
#include <map>
#include "hnrt/String.h"
#include "hnrt/IMsiDatabase.h"


namespace hnrt
{
    class MsiDatabase
        : public IMsiDatabase
    {
    public:

        MsiDatabase();
        MsiDatabase(const MsiDatabase&) = delete;
        virtual ~MsiDatabase();
        void operator =(const MsiDatabase&) = delete;
        operator MSIHANDLE () { return m_phDatabase; }
        virtual long AddRef();
        virtual long Release();
        virtual PCWSTR get_Path() const;
        virtual void set_Path(PCWSTR pszPath);
        virtual void Load();
        virtual IMsiTable* GetTable(PCWSTR pszName);
        virtual IMsiFileRowCollection* CreateFileRowCollection();
        virtual void AddByFileName(IMsiFileRowCollection* pCollection, PCWSTR pszPattern);
        virtual void AddByFeature(IMsiFileRowCollection* pCollection, PCWSTR pszFeature);
        virtual void ExtractFiles(IMsiFileRowCollection* pCollection, PCWSTR pszOutputDirectory, bool bFlat, IMsiDatabaseExtractFilesCallback* pCallback);
        virtual PCWSTR GetDirectoryPath(PCWSTR pszDirectory);

    private:

        typedef std::map<PCWSTR, IMsiTable*, StringLessThan> TableMap;
        typedef std::pair<PCWSTR, IMsiTable*> TableMapEntry;

        typedef std::map<PCWSTR, PCWSTR, StringLessThan> ValueMap;
        typedef std::pair<PCWSTR, PCWSTR> ValueMapEntry;

        long m_refs;
        PCWSTR m_pszPath;
        PMSIHANDLE m_phDatabase;
        TableMap m_tables;
        ValueMap m_dirPaths;
    };
}
