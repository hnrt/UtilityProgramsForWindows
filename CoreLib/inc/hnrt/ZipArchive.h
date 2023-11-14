#pragma once


#include "hnrt/ZipArchiveEntry.h"
#include "hnrt/String.h"


namespace hnrt
{
    class ZipArchive
    {
    public:

        ~ZipArchive();
        ZipArchive(const ZipArchive&);
        ZipArchive& operator =(const ZipArchive&);
        PCWSTR get_FileName() const { return m_FileName; }
        void ForEach(IZipForEachCallback&);

        __declspec(property(get = get_FileName)) PCWSTR FileName;

    private:

        friend class ZipFile;

        ZipArchive(PCWSTR, Folder*);

        String m_FileName;
        Folder* m_pFolder;
    };
}
