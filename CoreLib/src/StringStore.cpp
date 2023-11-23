#include "pch.h"
#include "hnrt/StringStore.h"
#include "StringStoreInternal.h"


using namespace hnrt;


PCWSTR StringStore::Get(PCWSTR psz)
{
    return StringStoreInternal::Instance().Get(psz);
}


PCSTR StringStore::Get(PCSTR psz)
{
    return StringStoreInternal::Instance().Get(psz);
}
