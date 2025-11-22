#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/Interlocked.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"


using namespace hnrt;


String::~String()
{
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, nullptr));
}
