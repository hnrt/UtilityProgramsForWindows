#include "pch.h"
#include "hnrt/Path.h"
#include "hnrt/ZipFile.h"
#include "ZipInternal.h"


using namespace hnrt;


ZipArchive* ZipFile::OpenRead(PCWSTR pszPath)
{
    return new ZipArchive(pszPath, ZipInternal::GetFolder(pszPath));
}


void ZipFile::ExtractFiles(PCWSTR pszSource, PCWSTR pszFilePattern, PCWSTR pszDestination, IZipExtractCallbacks& callbacks)
{
    struct LocalContext
        : public IZipForEachCallback
    {
        ZipArchive* pZipArchive;
        PCWSTR pszFilePattern;
        PCWSTR pszDestination;
        IZipExtractCallbacks& callbacks;

        LocalContext(PCWSTR pszSource, PCWSTR pszFilePattern_, PCWSTR pszDestination_, IZipExtractCallbacks& callbacks_)
            : pZipArchive(OpenRead(pszSource))
            , pszFilePattern(pszFilePattern_)
            , pszDestination(pszDestination_)
            , callbacks(callbacks_)
        {
        }

        ~LocalContext()
        {
            delete pZipArchive;
        }

        virtual void ZipForEach(ZipArchiveEntry& entry)
        {
            if (Path::WildcardMatch(pszFilePattern, entry.Name))
            {
                entry.ExtractTo(pszDestination, callbacks);
            }
            else if (entry.IsFolder())
            {
                entry.ForEach(*this);
            }
        }

    } ctx(pszSource, pszFilePattern, pszDestination, callbacks);

    ctx.pZipArchive->ForEach(ctx);
}
