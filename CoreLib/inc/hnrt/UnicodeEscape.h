#pragma once


#include "hnrt/String.h"


namespace hnrt
{
	String FromNativeToAscii(PCWSTR psz, bool bLowercase = false);
	String FromAsciiToNative(PCWSTR psz);
}
