#pragma once

#include "hnrt/String.h"
#include <vector>

namespace hnrt
{
	class StringHelpers
	{
	public:

		static String JoinBy(const std::vector<DWORD>&, PCWSTR, PCWSTR = L"%lu");
	};
}
