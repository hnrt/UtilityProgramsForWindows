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
		SIZE_T cch = wcslen(pszValue) + 1;
		m_value.psz = Allocate<WCHAR>(cch);
		wmemcpy_s(m_value.psz, cch, pszValue, cch);
		m_dwSize = static_cast<DWORD>(cch * sizeof(WCHAR));
	}
	else
	{
		throw Exception(L"RegistryValue::ctor(PCWSTR): Bad type.");
	}
}


RegistryValue::~RegistryValue()
{
	Clear();
}


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


PCWSTR RegistryValue::operator [](SIZE_T index) const
{
	if (m_dwType == REG_MULTI_SZ)
	{
		if (index < m_dwSize)
		{
			PWCHAR pCur = m_value.psz;
			for (SIZE_T iCur = 0; iCur < index; iCur++)
			{
				pCur += wcslen(pCur) + 1;
			}
			return pCur;
		}
		return nullptr;
	}
	else
	{
		throw Exception(L"RegistryValue::operator []: Bad type.");
	}
}


RegistryValue::operator const void*() const
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
			free(m_value.psz);
			break;
		case REG_BINARY:
			free(m_value.ptr);
			break;
		default:
			break;
		}
		m_dwType = 0;
		m_dwSize = 0;
		m_value.qw = 0;
	}
}


static DWORD CountMultiStrings(PWCHAR pszz)
{
	DWORD dwCount = 0;
	PWCHAR pCur = pszz;
	do
	{
		dwCount++;
		pCur += wcslen(pCur) + 1;
	}
	while (*pCur);
	return dwCount;
}


#pragma warning(disable:28182)
LSTATUS RegistryValue::Query(HKEY hKey, PCWSTR pszName)
{
	Clear();
	RegistryValueHolder value = { 0 };
	value.qw = 0;
	DWORD dwType = 0;
	DWORD dwSize = sizeof(value.qw);
	LSTATUS status = RegQueryValueExW(hKey, pszName, nullptr, &dwType, reinterpret_cast<LPBYTE>(&value.qw), &dwSize);
	if (status == ERROR_SUCCESS)
	{
		if (dwType == REG_DWORD)
		{
			if (dwSize == sizeof(m_value.dw))
			{
				m_dwType = dwType;
				m_dwSize = dwSize;
				m_value.dw = value.dw;
				return ERROR_SUCCESS;
			}
		}
		else if (dwType == REG_QWORD)
		{
			if (dwSize == sizeof(m_value.qw))
			{
				m_dwType = dwType;
				m_dwSize = dwSize;
				m_value.qw = value.qw;
				return ERROR_SUCCESS;
			}
		}
		else if (dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_LINK)
		{
			SIZE_T cch = dwSize / sizeof(WCHAR);
			PWCHAR psz = Allocate<WCHAR>(cch + 1);
			wmemcpy_s(psz, cch, reinterpret_cast<PWCHAR>(&value.qw), cch);
			psz[cch] = L'\0'; // to guarantee the string null-terminated
			m_dwType = dwType;
			m_dwSize = dwSize;
			m_value.psz = psz;
			return ERROR_SUCCESS;
		}
		else if (dwType == REG_MULTI_SZ)
		{
			SIZE_T cch = dwSize / sizeof(WCHAR);
			PWCHAR psz = Allocate<WCHAR>(cch + 2);
			wmemcpy_s(psz, cch, reinterpret_cast<PWCHAR>(&value.qw), cch);
			psz[cch + 0] = L'\0'; // to guarantee the last string null-terminated
			psz[cch + 1] = L'\0'; // to guarantee the multi-string null-terminated
			m_dwType = dwType;
			m_dwSize = CountMultiStrings(psz);
			m_value.psz = psz;
			return ERROR_SUCCESS;
		}
		else if (dwType == REG_BINARY)
		{
			PBYTE ptr = Allocate<BYTE>(dwSize);
			memcpy_s(ptr, m_dwSize, &value.qw, dwSize);
			m_dwType = dwType;
			m_dwSize = dwSize;
			m_value.ptr = ptr;
			return ERROR_SUCCESS;
		}
		status = ERROR_UNSUPPORTED_TYPE;
	}
	else if (status == ERROR_MORE_DATA)
	{
		PBYTE ptr = Allocate<BYTE>(dwSize + sizeof(WCHAR) * 2);
		status = RegQueryValueExW(hKey, pszName, nullptr, &dwType, ptr, &dwSize);
		if (status == ERROR_SUCCESS)
		{
			if (dwType == REG_DWORD)
			{
				if (dwSize == sizeof(m_value.dw))
				{
					m_dwType = dwType;
					m_dwSize = dwSize;
					memcpy_s(&m_value.dw, sizeof(m_value.dw), ptr, sizeof(m_value.dw));
					free(ptr);
					return ERROR_SUCCESS;
				}
			}
			else if (dwType == REG_QWORD)
			{
				if (dwSize == sizeof(m_value.qw))
				{
					m_dwType = dwType;
					m_dwSize = dwSize;
					memcpy_s(&m_value.dw, sizeof(m_value.qw), ptr, sizeof(m_value.qw));
					free(ptr);
					return ERROR_SUCCESS;
				}
			}
			else if (dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_LINK)
			{
				SIZE_T cch = dwSize / sizeof(WCHAR);
				PWCHAR psz = reinterpret_cast<PWCHAR>(ptr);
				psz[cch] = L'\0'; // to guarantee the string null-terminated
				m_dwType = dwType;
				m_dwSize = dwSize;
				m_value.psz = psz;
				return ERROR_SUCCESS;
			}
			else if (dwType == REG_MULTI_SZ)
			{
				SIZE_T cch = dwSize / sizeof(WCHAR);
				PWCHAR psz = reinterpret_cast<PWCHAR>(ptr);
				psz[cch + 0] = L'\0'; // to guarantee the last string null-terminated
				psz[cch + 1] = L'\0'; // to guarantee the multi-string null-terminated
				m_dwType = dwType;
				m_dwSize = CountMultiStrings(psz);
				m_value.psz = psz;
				return ERROR_SUCCESS;
			}
			else if (dwType == REG_BINARY)
			{
				m_dwType = dwType;
				m_dwSize = dwSize;
				m_value.ptr = ptr;
				return ERROR_SUCCESS;
			}
			status = ERROR_UNSUPPORTED_TYPE;
		}
		free(ptr);
	}
	return status;
}
#pragma warning(default:28182)


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
			free(m_value.psz);
			m_value.psz = _wcsdup(buf);
		}
		return m_value.psz;
	}
	else
	{
		return nullptr;
	}
}
