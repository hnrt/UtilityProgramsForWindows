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


PCWSTR StringStore::Set(PWSTR psz)
{
    return StringStoreInternal::Instance().Set(psz);
}


PCSTR StringStore::Set(PSTR psz)
{
    return StringStoreInternal::Instance().Set(psz);
}


PCWSTR StringStore::Set(StringBuffer& buf)
{
    return StringStoreInternal::Instance().Set(buf);
}
