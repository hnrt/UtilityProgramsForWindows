#include "pch.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Exception.h"


using namespace hnrt;


RegistryValue::operator ULONGLONG() const
{
	if (m_dwType == REG_QWORD)
	{
		return m_value.qw;
	}
	else if (m_dwType == REG_DWORD)
	{
		return static_cast<ULONGLONG>(m_value.dw);
	}
	else
	{
		throw Exception(L"RegistryValue::operator unsigned long: Bad type.");
	}
}
