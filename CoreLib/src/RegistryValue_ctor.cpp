#include "pch.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/RefStr.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"


using namespace hnrt;


RegistryValue::RegistryValue()
	: m_dwType(0)
	, m_dwSize(0)
	, m_value()
{
}


RegistryValue::RegistryValue(DWORD dwValue)
	: m_dwType(REG_DWORD)
	, m_dwSize(sizeof(dwValue))
	, m_value()
{
	m_value.dw = dwValue;
}


RegistryValue::RegistryValue(ULONGLONG qwValue)
	: m_dwType(REG_QWORD)
	, m_dwSize(sizeof(qwValue))
	, m_value()
{
	m_value.qw = qwValue;
}


RegistryValue::RegistryValue(PCWSTR pszValue, DWORD dwType)
	: m_dwType(dwType)
	, m_dwSize(0)
	, m_value()
{
	if (m_dwType == REG_SZ || m_dwType == REG_EXPAND_SZ || m_dwType == REG_LINK)
	{
		m_value.psz = RefStr::Create(pszValue);
		m_dwSize = static_cast<DWORD>((RefStr::Get(m_value.psz).Length + 1) * sizeof(WCHAR));
	}
	else
	{
		throw Exception(L"RegistryValue::ctor(PCWSTR): Bad type.");
	}
}
