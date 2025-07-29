#pragma once

#include "hnrt/ZipArchiveEntry.h"
#include "hnrt/String.h"

namespace hnrt
{
    class ZipArchive
    {
    private:

        String m_szFileName;
        Folder* m_pFolder;

        ZipArchive(const String&, Folder*);

        friend class ZipFile;

    public:

        ZipArchive(const ZipArchive&);
        virtual ~ZipArchive();

        ZipArchive& Assign(const ZipArchive&);
        void ForEach(IZipForEachCallback&);

        ZipArchive& operator =(const ZipArchive&);

        String get_FileName() const;

        __declspec(property(get = get_FileName)) String FileName;
    };

    inline ZipArchive& ZipArchive::operator =(const ZipArchive& src)
    {
        return Assign(src);
    }

    inline String ZipArchive::get_FileName() const
    {
        return m_szFileName;
    }
}
