#include "pch.h"
#include <Windows.h>
#include "hnrt/RegistryValue.h"
#include "hnrt/Exception.h"


using namespace hnrt;


RegistryValue::operator DWORD() const
{
	if (m_dwType == REG_DWORD)
	{
		return m_value.dw;
	}
	else if (m_dwType == REG_QWORD)
	{
		return static_cast<DWORD>(m_value.qw);
	}
	else
	{
		throw Exception(L"RegistryValue::operator unsigned long: Bad type.");
	}
}
