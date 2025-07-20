#include "pch.h"
#include "String_internal.h"
#include "hnrt/MultibyteString.h"


using namespace hnrt;


MultibyteString String::ToAcp() const
{
	return MultibyteString(CP_ACP, Ptr);
}


MultibyteString String::ToUTF8() const
{
	return MultibyteString(CP_UTF8, Ptr);
}


MultibyteString String::ToMultibyteString(UINT cp) const
{
	return MultibyteString(cp, Ptr);
}
