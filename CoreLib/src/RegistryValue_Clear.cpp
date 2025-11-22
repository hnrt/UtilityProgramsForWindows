#include "pch.h"
#include <Windows.h>
#include "hnrt/RegistryValue.h"
#include "hnrt/Interlocked.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"
#include "hnrt/RefBin.h"


using namespace hnrt;


void RegistryValue::Clear()
{
	if (m_dwType)
	{
		switch (m_dwType)
		{
		case REG_SZ:
		case REG_EXPAND_SZ:
		case REG_LINK:
		case REG_MULTI_SZ:
			StringRelease(Interlocked<PWCHAR>::ExchangePointer(&m_value.psz, nullptr));
			break;
		case REG_BINARY:
			BinRelease(Interlocked<PBYTE>::ExchangePointer(&m_value.ptr, nullptr));
			break;
		default:
			m_value.qw = 0;
			break;
		}
		m_dwSize = 0;
		m_dwType = 0;
	}
}
