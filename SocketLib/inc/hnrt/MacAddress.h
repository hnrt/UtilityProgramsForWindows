#pragma once


#include "hnrt/MultibyteString.h"


namespace hnrt
{
    class MacAddress
    {
    public:

        MacAddress();
        MacAddress(const MacAddress& src);
        ~MacAddress() = default;
        MacAddress& operator =(const MacAddress& src);
        const char* operator &() const;
        MultibyteString ToAcp() const;
        bool Parse(const char* psz);

    private:

        unsigned char m_data[6];
    };

    inline const char* MacAddress::operator &() const
    {
        return reinterpret_cast<const char*>(m_data);
    }
}
