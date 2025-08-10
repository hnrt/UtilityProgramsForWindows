#pragma once

#include "hnrt/String.h"
#include "hnrt/FolderPtr.h"
#include "hnrt/FolderItemPtr.h"

namespace hnrt
{
    class ZipFile
    {
    private:

        String m_pszFileName;
        FolderPtr m_pFolder;

        ZipFile(const String&, const FolderPtr&);

    public:

        ZipFile(const ZipFile&);
        virtual ~ZipFile() = default;
        BOOL ForEach(IFolderItemForEachCallback&);
        void ExtractFiles(PCWSTR pszFilePattern, PCWSTR pszDestination, IFolderItemProcessCallbacks& callbacks);

        ZipFile& operator =(const ZipFile&);

        static ZipFile Open(const String& pszFileName);

        String get_FileName() const;

        __declspec(property(get = get_FileName)) String FileName;

    };

    inline String ZipFile::get_FileName() const
    {
        return m_pszFileName;
    }
}
