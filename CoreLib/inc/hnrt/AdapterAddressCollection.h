#pragma once


#include <winsock2.h>
#include <Windows.h>
#include <iphlpapi.h>


namespace hnrt
{
    class AdapterAddressCollection
    {
    public:

        AdapterAddressCollection(ULONG family = AF_UNSPEC, ULONG flags = GAA_FLAG_INCLUDE_PREFIX);
        AdapterAddressCollection(const AdapterAddressCollection&) = delete;
        ~AdapterAddressCollection();
        ULONG get_Count() const;
        const IP_ADAPTER_ADDRESSES* operator [](ULONG index) const;
        const IP_ADAPTER_ADDRESSES* FindSameSubnetIPv4(PCWSTR pszIPv4);

        __declspec(property(get = get_Count)) unsigned long Count;

    private:

        IP_ADAPTER_ADDRESSES* m_p;
        IP_ADAPTER_ADDRESSES** m_pp;
        ULONG m_count;
    };

    class AdapterUnicastAddress
    {
    public:

        static ULONG GetSubnetMask(const IP_ADAPTER_UNICAST_ADDRESS_LH* pUnicastAddress);
    };
}
