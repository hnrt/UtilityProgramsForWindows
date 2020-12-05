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
    public:

        ~ZipArchiveEntry();
        ZipArchiveEntry();
        ZipArchiveEntry(FolderItem*);
        ZipArchiveEntry(const ZipArchiveEntry&);
        ZipArchiveEntry& operator =(const ZipArchiveEntry&);
        operator IDispatch* () { return m_pFolderItem; }
        PCWSTR get_Name() const { return m_bstrName; }
        PCWSTR get_Type() const { return m_bstrType; }
        LONG get_Size() const { return m_Size; }
        bool IsFolder() const { return m_bIsFolder ? true : false; }
        void ForEach(IZipForEachCallback&);
        void ExtractTo(PCWSTR, IZipExtractCallbacks&);

        __declspec(property(get = get_Name)) PCWSTR Name;
        __declspec(property(get = get_Type)) PCWSTR Type;
        __declspec(property(get = get_Size)) LONG Size;

    private:

        FolderItem* m_pFolderItem;
        BSTR m_bstrName;
        BSTR m_bstrType;
        LONG m_Size;
        VARIANT_BOOL m_bIsFolder;
    };
}
