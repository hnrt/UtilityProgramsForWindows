#include "pch.h"
#include <Windows.h>
#include "hnrt/RegistryValue.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefStr.h"
#include "hnrt/RefBin.h"
#include "hnrt/Buffer.h"


using namespace hnrt;


LSTATUS RegistryValue::Query(HKEY hKey, PCWSTR pszName)
{
	Clear();
	RegistryValueHolder value = { 0 };
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
			else
			{
				return ERROR_INVALID_DATA;
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
			else
			{
				return ERROR_INVALID_DATA;
			}
		}
		else if (dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_LINK)
		{
			m_dwType = dwType;
			m_dwSize = dwSize;
			m_value.psz = RefStr::Create(reinterpret_cast<PWCHAR>(&value.qw), dwSize / sizeof(WCHAR));
			return ERROR_SUCCESS;
		}
		else if (dwType == REG_MULTI_SZ)
		{
			SIZE_T cch = dwSize / sizeof(WCHAR);
			PWCHAR psz = RefStr::Create(cch + 1);
			MemCpy(psz, reinterpret_cast<PWCHAR>(&value.qw), cch);
			psz[cch + 0] = L'\0'; // to guarantee the last string null-terminated
			psz[cch + 1] = L'\0'; // to guarantee the multi-string null-terminated
			m_dwType = dwType;
			m_dwSize = CountStrings(psz);
			m_value.psz = psz;
			return ERROR_SUCCESS;
		}
		else if (dwType == REG_BINARY)
		{
			m_dwType = dwType;
			m_dwSize = dwSize;
			m_value.ptr = reinterpret_cast<PBYTE>(RefBin::Create(&value.qw, dwSize));
			return ERROR_SUCCESS;
		}
		else
		{
			return ERROR_UNSUPPORTED_TYPE;
		}
	}
	while (status == ERROR_MORE_DATA)
	{
		Buffer<BYTE> buf(dwSize);
		status = RegQueryValueExW(hKey, pszName, nullptr, &dwType, &buf, &dwSize);
		if (status == ERROR_SUCCESS)
		{
			if (dwType == REG_DWORD)
			{
				if (dwSize == sizeof(m_value.dw))
				{
					m_dwType = dwType;
					m_dwSize = dwSize;
					m_value.dw = *reinterpret_cast<PDWORD>(buf.Ptr);
					return ERROR_SUCCESS;
				}
				else
				{
					return ERROR_INVALID_DATA;
				}
			}
			else if (dwType == REG_QWORD)
			{
				if (dwSize == sizeof(m_value.qw))
				{
					m_dwType = dwType;
					m_dwSize = dwSize;
					m_value.qw = *reinterpret_cast<PULONGLONG>(buf.Ptr);
					return ERROR_SUCCESS;
				}
				else
				{
					return ERROR_INVALID_DATA;
				}
			}
			else if (dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_LINK)
			{
				m_dwType = dwType;
				m_dwSize = dwSize;
				m_value.psz = RefStr::Create(reinterpret_cast<PWCHAR>(buf.Ptr), dwSize / sizeof(WCHAR));
				return ERROR_SUCCESS;
			}
			else if (dwType == REG_MULTI_SZ)
			{
				SIZE_T cch = dwSize / sizeof(WCHAR);
				PWCHAR psz = RefStr::Create(cch + 1);
				MemCpy(psz, reinterpret_cast<PWCHAR>(buf.Ptr), cch);
				psz[cch + 0] = L'\0'; // to guarantee the last string null-terminated
				psz[cch + 1] = L'\0'; // to guarantee the multi-string null-terminated
				m_dwType = dwType;
				m_dwSize = CountStrings(psz);
				m_value.psz = psz;
				return ERROR_SUCCESS;
			}
			else if (dwType == REG_BINARY)
			{
				m_dwType = dwType;
				m_dwSize = dwSize;
				m_value.ptr = reinterpret_cast<PBYTE>(RefBin::Create(buf.Ptr, dwSize));
				return ERROR_SUCCESS;
			}
			else
			{
				return ERROR_UNSUPPORTED_TYPE;
			}
		}
	}
	return status;
}
