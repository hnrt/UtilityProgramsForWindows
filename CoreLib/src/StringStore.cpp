#include "pch.h"
#include "hnrt/StringStore.h"
#include "StringStoreInternal.h"
#include "hnrt/String.h"
#include "hnrt/MultibyteString.h"


using namespace hnrt;


String StringStore::Get(PCWSTR psz)
{
    return StringStoreInternal::Instance().Get(psz);
}


MultibyteString StringStore::Get(PCSTR psz)
{
    return StringStoreInternal::Instance().Get(psz);
}
