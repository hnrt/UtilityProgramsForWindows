#pragma once

#include "hnrt/String.h"
#include "hnrt/Array.h"

namespace hnrt
{
	String JoinBy(const Array<DWORD>&, PCWSTR, PCWSTR = L"%lu");
	Array<String> SplitBy(String, WCHAR);
	Array<String> SplitBy(PCWSTR, WCHAR);
}
