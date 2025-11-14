#pragma once

#include "hnrt/String.h"

#ifdef GetEnvironmentVariable
#undef GetEnvironmentVariable
#endif

namespace hnrt
{
	String GetEnvironmentVariable(PCWSTR);
}
