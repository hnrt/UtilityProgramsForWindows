#include "pch.h"
#include "hnrt/AdapterAddressCollection.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Heap.h"


#pragma comment(lib, "IPHLPAPI.lib")


using namespace hnrt;


AdapterAddressCollection::AdapterAddressCollection(ULONG family, ULONG flags)
    : m_p(nullptr)
    , m_pp(nullptr)
    , m_count(0)
{
    ULONG outBufLen = 512;

    m_p = (IP_ADAPTER_ADDRESSES*)Malloc(outBufLen);

    DWORD dwRet = GetAdaptersAddresses(family, flags, NULL, m_p, &outBufLen);

    if (dwRet == ERROR_BUFFER_OVERFLOW)
    {
        m_p = (IP_ADAPTER_ADDRESSES*)Realloc(m_p, outBufLen);

        dwRet = GetAdaptersAddresses(family, flags, NULL, m_p, &outBufLen);
    }

    if (dwRet != ERROR_SUCCESS)
    {
        throw Win32Exception(dwRet, L"Failed to obtain adapter addresses.");
    }

    IP_ADAPTER_ADDRESSES* p = m_p;
    while (p)
    {
        m_count++;
        p = p->Next;
    }

    m_pp = Allocate<IP_ADAPTER_ADDRESSES*>(static_cast<size_t>(m_count) + 1);

    IP_ADAPTER_ADDRESSES** pp = m_pp;
    p = m_p;
    while (p)
    {
        *pp++ = p;
        p = p->Next;
    }
    *pp = nullptr;
}


AdapterAddressCollection::~AdapterAddressCollection()
{
    free(m_p);
    free(m_pp);
}


unsigned long AdapterAddressCollection::get_Count() const
{
    return m_count;
}


const IP_ADAPTER_ADDRESSES* AdapterAddressCollection::operator [](ULONG index) const
{
    if (m_count <= index)
    {
        throw Exception(L"AdapterAddressCollection::operator []: Index out of range.");
    }
    return m_pp[index];
}


const IP_ADAPTER_ADDRESSES* AdapterAddressCollection::FindSameSubnetIPv4(PCWSTR pszIPv4)
{
    wchar_t* pStop = nullptr;
    ULONG a1 = wcstoul(pszIPv4, &pStop, 10);
    if (*pStop != L'.')
    {
        throw Exception(L"Malformed IPv4 address.");
    }
    a1 = (a1 << 8) | wcstoul(pStop + 1, &pStop, 10);
    if (*pStop != L'.')
    {
        throw Exception(L"Malformed IPv4 address.");
    }
    a1 = (a1 << 8) | wcstoul(pStop + 1, &pStop, 10);
    if (*pStop != L'.')
    {
        throw Exception(L"Malformed IPv4 address.");
    }
    a1 = (a1 << 8) | wcstoul(pStop + 1, &pStop, 10);
    if (*pStop != L'\0')
    {
        throw Exception(L"Malformed IPv4 address.");
    }
    for (ULONG index = 0; index < m_count; index++)
    {
        const IP_ADAPTER_ADDRESSES* p = m_pp[index];
        PIP_ADAPTER_UNICAST_ADDRESS_LH pUA = p->FirstUnicastAddress;
        while (pUA)
        {
            if (pUA->Address.lpSockaddr->sa_family == AF_INET)
            {
                struct sockaddr_in* pv4 = reinterpret_cast<struct sockaddr_in*>(pUA->Address.lpSockaddr);
                ULONG a2 = pv4->sin_addr.S_un.S_un_b.s_b1;
                a2 = (a2 << 8) | pv4->sin_addr.S_un.S_un_b.s_b2;
                a2 = (a2 << 8) | pv4->sin_addr.S_un.S_un_b.s_b3;
                a2 = (a2 << 8) | pv4->sin_addr.S_un.S_un_b.s_b4;
                ULONG mask = AdapterUnicastAddress::GetSubnetMask(pUA);
                if ((a2 & mask) == (a1 & mask))
                {
                    return p;
                }
            }
            pUA = pUA->Next;
        }
    }
    return nullptr;
}


ULONG AdapterUnicastAddress::GetSubnetMask(const IP_ADAPTER_UNICAST_ADDRESS_LH* pUnicastAddress)
{
    return ~((1UL << (32 - pUnicastAddress->OnLinkPrefixLength)) - 1UL);
}
