#pragma once


#include "hnrt/KeyValueMap.h"


namespace hnrt
{
    class GuestIntrinsicExchangeItems
        : public KeyValueMap
    {
    public:

        GuestIntrinsicExchangeItems(const KeyValueMap& exchangeComponentMap);
        GuestIntrinsicExchangeItems(const GuestIntrinsicExchangeItems&) = delete;
        ~GuestIntrinsicExchangeItems() = default;
        GuestIntrinsicExchangeItems& operator =(const GuestIntrinsicExchangeItems&) = delete;
        PCWSTR get_FullyQualifiedDomainName() const;
        PCWSTR get_IntegrationServicesVersion() const;
        PCWSTR get_NetworkAddressIPv4() const;
        PCWSTR get_NetworkAddressIPv6() const;
        PCWSTR get_OSBuildNumber() const;
        PCWSTR get_OSDistributionData() const;
        PCWSTR get_OSDistributionName() const;
        PCWSTR get_OSKernelVersion() const;
        PCWSTR get_OSMajorVersion() const;
        PCWSTR get_OSMinorVersion() const;
        PCWSTR get_OSName() const;
        PCWSTR get_OSPlatformId() const;
        PCWSTR get_OSVersion() const;
        PCWSTR get_ProcessorArchitecture() const;

        __declspec(property(get = get_FullyQualifiedDomainName)) PCWSTR FullyQualifiedDomainName;
        __declspec(property(get = get_IntegrationServicesVersion)) PCWSTR IntegrationServicesVersion;
        __declspec(property(get = get_NetworkAddressIPv4)) PCWSTR NetworkAddressIPv4;
        __declspec(property(get = get_NetworkAddressIPv6)) PCWSTR NetworkAddressIPv6;
        __declspec(property(get = get_OSBuildNumber)) PCWSTR OSBuildNumber;
        __declspec(property(get = get_OSDistributionData)) PCWSTR OSDistributionData;
        __declspec(property(get = get_OSDistributionName)) PCWSTR OSDistributionName;
        __declspec(property(get = get_OSKernelVersion)) PCWSTR OSKernelVersion;
        __declspec(property(get = get_OSMajorVersion)) PCWSTR OSMajorVersion;
        __declspec(property(get = get_OSMinorVersion)) PCWSTR OSMinorVersion;
        __declspec(property(get = get_OSName)) PCWSTR OSName;
        __declspec(property(get = get_OSPlatformId)) PCWSTR OSPlatformId;
        __declspec(property(get = get_OSVersion)) PCWSTR OSVersion;
        __declspec(property(get = get_ProcessorArchitecture)) PCWSTR ProcessorArchitecture;
    };

    inline PCWSTR GuestIntrinsicExchangeItems::get_FullyQualifiedDomainName() const
    {
        return GetValue(L"FullyQualifiedDomainName");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_IntegrationServicesVersion() const
    {
        return GetValue(L"IntegrationServicesVersion");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_NetworkAddressIPv4() const
    {
        return GetValue(L"NetworkAddressIPv4");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_NetworkAddressIPv6() const
    {
        return GetValue(L"NetworkAddressIPv6");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_OSBuildNumber() const
    {
        return GetValue(L"OSBuildNumber");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_OSDistributionData() const
    {
        return GetValue(L"OSDistributionData");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_OSDistributionName() const
    {
        return GetValue(L"OSDistributionName");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_OSKernelVersion() const
    {
        return GetValue(L"OSKernelVersion");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_OSMajorVersion() const
    {
        return GetValue(L"OSMajorVersion");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_OSMinorVersion() const
    {
        return GetValue(L"OSMinorVersion");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_OSName() const
    {
        return GetValue(L"OSName");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_OSPlatformId() const
    {
        return GetValue(L"OSPlatformId");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_OSVersion() const
    {
        return GetValue(L"OSVersion");
    }

    inline PCWSTR GuestIntrinsicExchangeItems::get_ProcessorArchitecture() const
    {
        return GetValue(L"ProcessorArchitecture");
    }
}
