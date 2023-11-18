#pragma once


#include "hnrt/String.h"
#include "hnrt/IMsiTable.h"
#include "hnrt/IMsiFileRowCollection.h"


namespace hnrt
{
    struct IMsiDatabaseExtractFilesCallback
    {
        virtual bool OnMsiDatabaseExtract(PCWSTR pszFile, PCWSTR pszPath, unsigned long status) = 0;
    };

    struct IMsiDatabase
    {
        virtual long AddRef() = 0;
        virtual long Release() = 0;
        virtual PCWSTR get_Path() const = 0;
        virtual void set_Path(PCWSTR pszPath) = 0;
        virtual void Load() = 0;
        virtual IMsiTable* GetTable(PCWSTR pszName) = 0;
        virtual IMsiFileRowCollection* CreateFileRowCollection() = 0;
        virtual void AddByFileName(IMsiFileRowCollection* pCollection, PCWSTR pszPattern) = 0;
        virtual void AddByFeature(IMsiFileRowCollection* pCollection, PCWSTR pszFeature) = 0;
        virtual void ExtractFiles(IMsiFileRowCollection* pCollection, PCWSTR pszOutputDirectory, bool bFlat, IMsiDatabaseExtractFilesCallback* pCallback) = 0;
        virtual String GetDirectoryPath(PCWSTR pszDirectory) = 0;

        __declspec(property(get = get_Path, put = set_Path)) PCWSTR Path;
    };
}
