#include "pch.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Exception.h"


using namespace hnrt;


RegistryValue::operator const void* () const
{
	if (m_dwType == REG_BINARY)
	{
		return m_value.ptr;
	}
	else
	{
		throw Exception(L"RegistryValue::operator const BYTE*: Bad type.");
	}
}
