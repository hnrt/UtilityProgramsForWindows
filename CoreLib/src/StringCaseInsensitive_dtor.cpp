#include "pch.h"
#include <Windows.h>
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/Interlocked.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"


using namespace hnrt;


StringCaseInsensitive::~StringCaseInsensitive()
{
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, nullptr));
}
