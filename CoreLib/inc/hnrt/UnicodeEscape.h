#pragma once


#include <Windows.h>


namespace hnrt
{
	int FromNativeToAscii(PCWSTR psz, PWCHAR pOut = nullptr, bool bLowercase = false);
	int FromAsciiToNative(PCWSTR psz, PWCHAR pOut = nullptr);
}
