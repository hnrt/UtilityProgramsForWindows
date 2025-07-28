#include "pch.h"
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/RefStr.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


StringCaseInsensitive::~StringCaseInsensitive()
{
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, nullptr));
}
