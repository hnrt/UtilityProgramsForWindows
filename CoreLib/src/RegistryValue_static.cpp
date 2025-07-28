#include "pch.h"
#include "hnrt/RegistryValue.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"
#include "hnrt/Buffer.h"


using namespace hnrt;


LSTATUS RegistryValue::SetDWORD(HKEY hKey, PCWSTR pszName, DWORD dwValue)
{
	return RegSetValueExW(hKey, pszName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwValue), sizeof(dwValue));
}


LSTATUS RegistryValue::SetQWORD(HKEY hKey, PCWSTR pszName, ULONGLONG qwValue)
{
	return RegSetValueExW(hKey, pszName, 0, REG_QWORD, reinterpret_cast<const BYTE*>(&qwValue), sizeof(qwValue));
}


LSTATUS RegistryValue::SetSZ(HKEY hKey, PCWSTR pszName, PCWSTR pszValue)
{
	return RegSetValueExW(hKey, pszName, 0, REG_SZ, reinterpret_cast<const BYTE*>(pszValue), static_cast<DWORD>((wcslen(pszValue) + 1) * sizeof(WCHAR)));
}


LSTATUS RegistryValue::SetEXPANDSZ(HKEY hKey, PCWSTR pszName, PCWSTR pszValue)
{
	return RegSetValueExW(hKey, pszName, 0, REG_EXPAND_SZ, reinterpret_cast<const BYTE*>(pszValue), static_cast<DWORD>((wcslen(pszValue) + 1) * sizeof(WCHAR)));
}


LSTATUS RegistryValue::SetMULTISZ(HKEY hKey, PCWSTR pszName, PCWSTR pszValues[], SIZE_T nValues)
{
	if (nValues)
	{
		SIZE_T cch = 0;
		for (SIZE_T index = 0; index < nValues; index++)
		{
			cch += wcslen(pszValues[index]) + 1;
		}
		cch++;
		Buffer<WCHAR> buf(cch);
		PWCHAR pCur = &buf;
		for (SIZE_T index = 0; index < nValues; index++)
		{
			cch = wcslen(pszValues[index]) + 1;
			wmemcpy_s(pCur, cch, pszValues[index], cch);
			pCur += cch;
		}
		*pCur = L'\0';
		return RegSetValueExW(hKey, pszName, 0, REG_MULTI_SZ, reinterpret_cast<const BYTE*>(&buf), static_cast<DWORD>(buf.Len * sizeof(WCHAR)));
	}
	else
	{
		WCHAR buf[2] = { L'\0', L'\0' };
		return RegSetValueExW(hKey, pszName, 0, REG_MULTI_SZ, reinterpret_cast<const BYTE*>(buf), static_cast<DWORD>(2 * sizeof(WCHAR)));
	}
}


LSTATUS RegistryValue::SetBINARY(HKEY hKey, PCWSTR pszName, const void* pValue, SIZE_T cbValue)
{
	return RegSetValueExW(hKey, pszName, 0, REG_BINARY, reinterpret_cast<const BYTE*>(pValue), static_cast<DWORD>(cbValue));
}


DWORD RegistryValue::GetDWORD(HKEY hKey, PCWSTR pszName, DWORD dwDefaultValue)
{
	RegistryValue value;
	return value.Query(hKey, pszName) == ERROR_SUCCESS && value.Type == REG_DWORD ? (DWORD)value : dwDefaultValue;
}


ULONGLONG RegistryValue::GetQWORD(HKEY hKey, PCWSTR pszName, ULONGLONG qwDefaultValue)
{
	RegistryValue value;
	return value.Query(hKey, pszName) == ERROR_SUCCESS && value.Type == REG_QWORD ? (ULONGLONG)value : qwDefaultValue;
}


String RegistryValue::GetSZ(HKEY hKey, PCWSTR pszName, const String& szDefaultValue)
{
	RegistryValue value;
	return value.Query(hKey, pszName) == ERROR_SUCCESS && value.Type == REG_SZ ? String((PCWSTR)value) : String(szDefaultValue);
}


String RegistryValue::GetEXPANDSZ(HKEY hKey, PCWSTR pszName, const String& szDefaultValue)
{
	RegistryValue value;
	return value.Query(hKey, pszName) == ERROR_SUCCESS && value.Type == REG_EXPAND_SZ ? String((PCWSTR)value) : String(szDefaultValue);
}


String RegistryValue::GetString(HKEY hKey, PCWSTR pszName, const String& szDefaultValue)
{
	RegistryValue value;
	return value.Query(hKey, pszName) != ERROR_SUCCESS
		? String(szDefaultValue)
		: value.Type == REG_EXPAND_SZ ? String(value.Expand())
		: value.Type == REG_SZ ? String((PCWSTR)value)
		: String(szDefaultValue);
}
