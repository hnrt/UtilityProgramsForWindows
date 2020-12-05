#include "pch.h"
#include "hnrt/MsiDatabaseFactory.h"
#include "MsiDatabase.h"


using namespace hnrt;


IMsiDatabase* MsiDatabaseFactory::OpenRead(PCWSTR pszPath)
{
    MsiDatabase* pDatabase = new MsiDatabase();
    pDatabase->Path = pszPath;
    pDatabase->Load();
    return pDatabase;
}
