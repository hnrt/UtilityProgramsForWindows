#include "pch.h"
#include "hnrt/Environment.h"
#include "hnrt/Buffer.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


String hnrt::GetEnvironmentVariable(PCWSTR pszName)
{
	WCHAR sz[MAX_PATH] = { 0 };
	DWORD dwReturn = ::GetEnvironmentVariableW(pszName, sz, _countof(sz));
	if (dwReturn == 0)
	{
		return String::Empty;
	}
	else if (dwReturn < _countof(sz))
	{
		return String(sz);
	}
	Buffer<WCHAR> buf(dwReturn);
	dwReturn = ::GetEnvironmentVariableW(pszName, buf, static_cast<DWORD>(buf.Len));
	if (dwReturn == 0)
	{
		return String::Empty;
	}
	else if (dwReturn < static_cast<DWORD>(buf.Len))
	{
		return String(buf);
	}
	buf.Resize(32767);
	dwReturn = ::GetEnvironmentVariableW(pszName, buf, static_cast<DWORD>(buf.Len));
	if (dwReturn == 0)
	{
		return String::Empty;
	}
	else if (dwReturn < static_cast<DWORD>(buf.Len))
	{
		return String(buf);
	}
	else
	{
		throw Win32Exception(GetLastError(), String::Format(L"GetEnvironmentVariable(%s) failed.", pszName));
	}
}
