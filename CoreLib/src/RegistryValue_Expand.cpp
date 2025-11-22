#include "pch.h"
#include <Windows.h>
#include "hnrt/RegistryValue.h"
#include "hnrt/Interlocked.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"
#include "hnrt/Buffer.h"


using namespace hnrt;


PCWSTR RegistryValue::Expand()
{
	if (m_dwType == REG_SZ || m_dwType == REG_EXPAND_SZ)
	{
		Buffer<WCHAR> buf(MAX_PATH);
		DWORD dwRet = ExpandEnvironmentStringsW(m_value.psz, buf, static_cast<DWORD>(buf.Len));
		if (dwRet > buf.Len)
		{
			buf.Resize(dwRet);
			dwRet = ExpandEnvironmentStringsW(m_value.psz, buf, static_cast<DWORD>(buf.Len));
		}
		if (0 < dwRet && dwRet <= buf.Len)
		{
			StringRelease(Interlocked<PWCHAR>::ExchangePointer(&m_value.psz, RefStr::Create(buf)));
		}
		return m_value.psz;
	}
	else
	{
		return nullptr;
	}
}
