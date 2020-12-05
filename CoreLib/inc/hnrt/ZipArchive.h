#pragma once


#include "hnrt/ZipArchiveEntry.h"


namespace hnrt
{
    class ZipArchive
    {
    public:

        ~ZipArchive();
        ZipArchive(const ZipArchive&);
        ZipArchive& operator =(const ZipArchive&);
        PCWSTR get_FileName() const { return m_pszFileName; }
        void ForEach(IZipForEachCallback&);

        __declspec(property(get = get_FileName)) PCWSTR FileName;

    private:

        friend class ZipFile;

        ZipArchive(PCWSTR, Folder*);

        PCWSTR m_pszFileName;
        Folder* m_pFolder;
    };
}
