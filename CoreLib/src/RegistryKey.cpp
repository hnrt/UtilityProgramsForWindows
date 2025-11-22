#include "pch.h"
#include <Windows.h>
#include "hnrt/RegistryKey.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


RegistryKey::RegistryKey()
	: m_h(nullptr)
{
}


RegistryKey::~RegistryKey()
{
	Close();
}


HKEY* RegistryKey::operator &()
{
	Close();
	return &m_h;
}


LSTATUS RegistryKey::Create(HKEY hKey, PCWSTR pszSubKey, DWORD dwOptions, REGSAM samDesired, const PSECURITY_ATTRIBUTES pSecurityAttributes, PDWORD pdwDisposition)
{
	Close();
	DWORD dwUnused = 0UL;
	return RegCreateKeyExW(hKey, pszSubKey, 0, NULL, dwOptions, samDesired, pSecurityAttributes, &m_h, pdwDisposition ? pdwDisposition : &dwUnused);
}


LSTATUS RegistryKey::Open(HKEY hKey, PCWSTR pszSubKey, DWORD dwOptions, REGSAM samDesired)
{
	Close();
	return RegOpenKeyExW(hKey, pszSubKey, dwOptions, samDesired, &m_h);
}


LSTATUS RegistryKey::Close()
{
	HKEY h = Interlocked<HKEY>::ExchangePointer(&m_h, nullptr);
	if (h)
	{
		return RegCloseKey(h);
	}
	else
	{
		return ERROR_SUCCESS;
	}
}
