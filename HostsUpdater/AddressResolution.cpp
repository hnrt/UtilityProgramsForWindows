#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "AddressResolution.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"
#include "hnrt/String.h"
#include <stdio.h>
#include <stdlib.h>


using namespace hnrt;


AddressResolution::AddressResolution(PCWSTR pszAlias, PCWSTR pszHostName)
	: m_pszAlias(pszAlias)
	, m_pszHostName(pszHostName)
	, m_pAddresses(nullptr)
	, m_Size(0)
	, m_Count(0)
	, m_Error(0)
{
}


AddressResolution::AddressResolution(const AddressResolution& src)
	: m_pszAlias(src.m_pszAlias)
	, m_pszHostName(src.m_pszHostName)
	, m_pAddresses(nullptr)
	, m_Size(0)
	, m_Count(0)
	, m_Error(0)
{
	Copy(src);
}


AddressResolution::~AddressResolution()
{
	Clear();
}


AddressResolution& AddressResolution::operator =(const AddressResolution& src)
{
	Clear();
	Copy(src);
	return *this;
}


void AddressResolution::Clear()
{
	free(m_pAddresses);
	m_pAddresses = nullptr;
	m_Size = 0;
	m_Count = 0;
	m_Error = 0;
}


void AddressResolution::Copy(const AddressResolution& src)
{
	m_pszAlias = src.m_pszAlias;
	m_pszHostName = src.m_pszHostName;
	if (src.m_Count)
	{
		m_pAddresses = Allocate<PCWSTR>(src.m_Size);
		m_Size = src.m_Size;
		memcpy_s(m_pAddresses, m_Size * sizeof(PCWSTR), src.m_pAddresses, src.m_Size * sizeof(PCWSTR));
		m_Count = src.m_Count;
	}
}


void AddressResolution::Add(PCWSTR pszAddress)
{
	if (m_Count + 1 > m_Size)
	{
		m_Size += 8;
		m_pAddresses = Allocate<PCWSTR>(m_pAddresses, m_Size);
		memset(&m_pAddresses[m_Count], 0, (m_Size - m_Count) * sizeof(PCWSTR));
	}
	m_pAddresses[m_Count++] = pszAddress;
}


PCWSTR AddressResolution::operator[](int index) const
{
	if (0 <= index && index < static_cast<int>(m_Count))
	{
		return m_pAddresses[index];
	}
	else if (0 <= static_cast<int>(m_Count) + index && index < 0)
	{
		return m_pAddresses[static_cast<int>(m_Count) + index];
	}
	else
	{
		throw Exception(L"AddressResolution: Index out of range.");
	}
}


bool AddressResolution::Resolve()
{
	Clear();
	PADDRINFOW pAddrInfo = nullptr;
	m_Error = GetAddrInfoW(m_pszHostName, NULL, NULL, &pAddrInfo);
	if (m_Error)
	{
#ifdef _DEBUG
		wprintf(L"# GetAddrInfoW(%s) failed. %d\n", m_pszHostName, m_Error);
#endif
		return false;
	}
	for (PADDRINFOW pCur = pAddrInfo; pCur; pCur = pCur->ai_next)
	{
		if (pCur->ai_family == AF_INET)
		{
			WCHAR szIpAddress[46] = { 0 };
			DWORD dwIpAddress = _countof(szIpAddress);
			PSOCKADDR pSockAddr = reinterpret_cast<PSOCKADDR>(pCur->ai_addr);
			INT ret = WSAAddressToStringW(pSockAddr, static_cast<DWORD>(pCur->ai_addrlen), NULL, szIpAddress, &dwIpAddress);
			if (ret)
			{
#ifdef _DEBUG
				wprintf(L"# WSAAddressToString(%s) failed. %d\n", m_pszHostName, ret);
#endif
				m_Error = ret;
				continue;
			}
#ifdef _DEBUG
			wprintf(L"# %s=%s\n", m_pszHostName, szIpAddress);
#endif
			Add(String::Copy(szIpAddress));
		}
	}
	FreeAddrInfoW(pAddrInfo);
	return true;
}
