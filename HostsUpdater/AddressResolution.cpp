#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "AddressResolution.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"
#include <stdlib.h>


using namespace hnrt;


AddressResolution::AddressResolution(PCWSTR pszAlias, PCWSTR pszHostName)
	: m_pszAlias(Clone(pszAlias))
	, m_pszHostName(Clone(pszHostName))
	, m_pAddresses(nullptr)
	, m_Count(0)
	, m_Error(0)
{
}


AddressResolution::AddressResolution(const AddressResolution& src)
	: m_pszAlias(nullptr)
	, m_pszHostName(nullptr)
	, m_pAddresses(nullptr)
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
	free(m_pszAlias);
	free(m_pszHostName);
	Free(m_pAddresses);
	m_pszAlias = nullptr;
	m_pszHostName = nullptr;
	m_pAddresses = nullptr;
	m_Count = 0;
	m_Error = 0;
}


void AddressResolution::Copy(const AddressResolution& src)
{
	m_pszAlias = Clone(src.m_pszAlias);
	m_pszHostName = Clone(src.m_pszHostName);
	m_pAddresses = Clone(src.m_pAddresses, src.m_Count, ArraySize(src.m_pAddresses));
	m_Count = src.m_Count;
	m_Error = 0;
}


void AddressResolution::Add(PCWSTR pszAddress)
{
	DWORD dwSize = ArraySize(m_pAddresses);
	if (m_Count + 1 > dwSize)
	{
		m_pAddresses = Resize(m_pAddresses, dwSize + 8);
	}
	m_pAddresses[m_Count++] = Clone(pszAddress);
}


PCWSTR AddressResolution::operator [](int index) const
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
	DBGFNC(L"AddressResolution::Resolve");
	m_pAddresses = Resize(m_pAddresses, 0);
	m_Count = 0;
	PADDRINFOW pAddrInfo = nullptr;
	m_Error = GetAddrInfoW(m_pszHostName, NULL, NULL, &pAddrInfo);
	if (m_Error)
	{
		DBGPUT(L"GetAddrInfoW(%s) failed. (%d: %s)", m_pszHostName, m_Error, ErrorMessage::Get(m_Error));
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
				DBGPUT(L"WSAAddressToString(%s) failed. (%d: %s)", m_pszHostName, ret, ErrorMessage::Get(ret));
				m_Error = ret;
				continue;
			}
			DBGPUT(L"%s=%s", m_pszHostName, szIpAddress);
			Add(szIpAddress);
		}
	}
	FreeAddrInfoW(pAddrInfo);
	return true;
}
