#pragma once

#include <Windows.h>
#include <ShlDisp.h>
#include <shellapi.h>

namespace hnrt
{
    class ZipArchiveEntry;

    struct IZipForEachCallback
    {
        virtual void ZipForEach(ZipArchiveEntry&) = 0;
    };

    struct IZipExtractCallbacks
    {
        virtual void ZipExtractOnStart(PCWSTR, LONG) = 0;
        virtual void ZipExtractOnSuccess(PCWSTR) = 0;
        virtual void ZipExtractOnFailure(PCWSTR, HRESULT) = 0;
    };

    class ZipArchiveEntry
    {
    private:

        FolderItem* m_pFolderItem;
        BSTR m_bstrName;
        BSTR m_bstrType;
        LONG m_Size;
        VARIANT_BOOL m_bIsFolder;

    public:

        ZipArchiveEntry();
        ZipArchiveEntry(FolderItem*);
        ZipArchiveEntry(const ZipArchiveEntry&);
        virtual ~ZipArchiveEntry();

        ZipArchiveEntry& Assign(const ZipArchiveEntry&);
        void ForEach(IZipForEachCallback&);
        void ExtractTo(PCWSTR, IZipExtractCallbacks&);

        ZipArchiveEntry& operator =(const ZipArchiveEntry&);

        operator IDispatch* ();
        PCWSTR get_Name() const;
        PCWSTR get_Type() const;
        LONG get_Size() const;
        bool IsFolder() const;

        __declspec(property(get = get_Name)) PCWSTR Name;
        __declspec(property(get = get_Type)) PCWSTR Type;
        __declspec(property(get = get_Size)) LONG Size;
    };

    inline ZipArchiveEntry& ZipArchiveEntry::operator =(const ZipArchiveEntry& src)
    {
        return Assign(src);
    }

    inline ZipArchiveEntry::operator IDispatch*()
    {
        return m_pFolderItem;
    }

    inline PCWSTR ZipArchiveEntry::get_Name() const
    {
        return m_bstrName;
    }

    inline PCWSTR ZipArchiveEntry::get_Type() const
    {
        return m_bstrType;
    }

    inline LONG ZipArchiveEntry::get_Size() const
    {
        return m_Size;
    }

    inline bool ZipArchiveEntry::IsFolder() const
    {
        return m_bIsFolder ? true : false;
    }
}
