#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "AddressResolution.h"
#include "hnrt/Exception.h"
#include "hnrt/ErrorMessage.h"
#include "hnrt/Debug.h"
#include <stdlib.h>


using namespace hnrt;


AddressResolution::AddressResolution(const String& szAlias, const String& szHostName)
	: m_szAlias(szAlias)
	, m_szHostName(szHostName)
	, m_Addresses()
	, m_Error(0)
{
}


AddressResolution::AddressResolution(const AddressResolution& src)
	: m_szAlias(src.m_szAlias)
	, m_szHostName(src.m_szHostName)
	, m_Addresses(src.m_Addresses)
	, m_Error(0)
{
}


AddressResolution& AddressResolution::operator =(const AddressResolution& src)
{
	m_szAlias = src.m_szAlias;
	m_szHostName = src.m_szHostName;
	m_Addresses = src.m_Addresses;
	m_Error = 0;
	return *this;
}


PCWSTR AddressResolution::operator [](int index) const
{
	int count = static_cast<int>(Count);
	if (0 <= index && index < count)
	{
		return m_Addresses.at(index);
	}
	else if (0 <= count + index && index < 0)
	{
		return m_Addresses.at(static_cast<INT_PTR>(count) + static_cast<INT_PTR>(index));
	}
	else
	{
		throw Exception(L"AddressResolution: Index out of range.");
	}
}


bool AddressResolution::Resolve()
{
	DBGFNC(L"AddressResolution::Resolve");
	m_Addresses.clear();
	PADDRINFOW pAddrInfo = nullptr;
	m_Error = GetAddrInfoW(m_szHostName, NULL, NULL, &pAddrInfo);
	if (m_Error)
	{
		DBGPUT(L"GetAddrInfoW(%s) failed. (%d: %s)", m_szHostName.Ptr, m_Error, ErrorMessage::Get(m_Error));
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
				DBGPUT(L"WSAAddressToString(%s) failed. (%d: %s)", m_szHostName.Ptr, ret, ErrorMessage::Get(ret));
				m_Error = ret;
				continue;
			}
			DBGPUT(L"%s=%s", m_szHostName.Ptr, szIpAddress);
			m_Addresses.push_back(String(szIpAddress));
		}
	}
	FreeAddrInfoW(pAddrInfo);
	return true;
}
