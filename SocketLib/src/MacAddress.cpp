#include "pch.h"
#include "hnrt/MacAddress.h"
#include "hnrt/String.h"
#include "hnrt/MultibyteString.h"


using namespace hnrt;


MacAddress::MacAddress()
{
    memset(&m_data, 0, sizeof(m_data));
}


MacAddress::MacAddress(const MacAddress& src)
{
    memcpy_s(m_data, sizeof(m_data), src.m_data, sizeof(src.m_data));
}


MacAddress& MacAddress::operator =(const MacAddress& src)
{
    memcpy_s(m_data, sizeof(m_data), src.m_data, sizeof(src.m_data));
    return *this;
}


MultibyteString MacAddress::ToAcp() const
{
    return ::ToAcp(String::Format(L"%02X:%02X:%02X:%02X:%02X:%02X", m_data[0], m_data[1], m_data[2], m_data[3], m_data[4], m_data[5]));
}


bool MacAddress::Parse(const char* psz)
{
    for (int i = 0; i < 5; i++)
    {
        int j = i * 3;
        if (!isxdigit(psz[j + 0])) return false;
        if (!isxdigit(psz[j + 1])) return false;
        if (psz[j + 2] != ':' && psz[j + 2] != '-') return false;
        m_data[i] = static_cast<unsigned char>(strtoul(&psz[j], NULL, 16));
    }
    if (!isxdigit(psz[15])) return false;
    if (!isxdigit(psz[16])) return false;
    if (psz[17] != '\0') return false;
    m_data[5] = static_cast<unsigned char>(strtoul(&psz[15], NULL, 16));
    return true;
}
