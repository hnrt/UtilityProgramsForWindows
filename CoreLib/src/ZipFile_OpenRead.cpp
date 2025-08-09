#include "pch.h"
#include "hnrt/ZipFile.h"
#include "hnrt/Path.h"
#include "ZipInternal.h"


using namespace hnrt;


ZipArchive* ZipFile::OpenRead(PCWSTR pszPath)
{
    return new ZipArchive(pszPath, ZipInternal::GetFolder(pszPath));
}
