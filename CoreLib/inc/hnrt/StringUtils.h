#pragma once

#include "hnrt/String.h"
#include <vector>

namespace hnrt
{
	String JoinBy(const std::vector<DWORD>&, PCWSTR, PCWSTR = L"%lu");
}
