#include "pch.h"
#include <Windows.h>
#include "hnrt/RegistryValue.h"
#include "hnrt/Exception.h"


using namespace hnrt;


RegistryValue::operator PCWSTR() const
{
	if (m_dwType == REG_SZ || m_dwType == REG_EXPAND_SZ || m_dwType == REG_LINK)
	{
		return m_value.psz;
	}
	else
	{
		throw Exception(L"RegistryValue::operator PCWSTR: Bad type.");
	}
}
