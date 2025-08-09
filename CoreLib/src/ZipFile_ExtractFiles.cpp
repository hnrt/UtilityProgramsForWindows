#include "pch.h"
#include "hnrt/ZipFile.h"
#include "hnrt/Path.h"
#include "ZipInternal.h"


using namespace hnrt;


void ZipFile::ExtractFiles(PCWSTR pszSource, PCWSTR pszFilePattern, PCWSTR pszDestination, IZipExtractCallbacks& callbacks)
{
    struct LocalContext
        : public IZipForEachCallback
    {
        ZipArchive* pZipArchive;
        PCWSTR pszFilePattern;
        String strDestination;
        IZipExtractCallbacks& callbacks;
        bool bExtract;

        LocalContext(PCWSTR pszSource, PCWSTR pszFilePattern_, PCWSTR pszDestination_, IZipExtractCallbacks& callbacks_)
            : pZipArchive(OpenRead(pszSource))
            , pszFilePattern(pszFilePattern_)
            , strDestination(pszDestination_)
            , callbacks(callbacks_)
            , bExtract(false)
        {
        }

        ~LocalContext()
        {
            delete pZipArchive;
        }

        virtual void ZipForEach(ZipArchiveEntry& entry)
        {
            if (entry.IsFolder)
            {
                bool bPrevious = bExtract;
                if (!bExtract && Path::WildcardMatch(pszFilePattern, entry.Name))
                {
                    bExtract = true;
                }
                String strOriginal = strDestination;
                strDestination = Path::Combine(strDestination, entry.Name);
                entry.ForEach(*this);
                strDestination = strOriginal;
                bExtract = bPrevious;
            }
            else if (bExtract || Path::WildcardMatch(pszFilePattern, entry.Name))
            {
                Path::CreateDirectory(strDestination);
                entry.ExtractTo(strDestination, callbacks);
            }
        }

    } ctx(pszSource, pszFilePattern, pszDestination, callbacks);

    ctx.pZipArchive->ForEach(ctx);
}
