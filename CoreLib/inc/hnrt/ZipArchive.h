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
        const String& get_FileName() const { return m_szFileName; }
        void ForEach(IZipForEachCallback&);

        __declspec(property(get = get_FileName)) const String& FileName;

    private:

        friend class ZipFile;

        ZipArchive(const String&, Folder*);

        String m_szFileName;
        Folder* m_pFolder;
    };
}
