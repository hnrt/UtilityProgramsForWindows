#include "pch.h"
#include "hnrt/MagicPacket.h"
#include "hnrt/MacAddress.h"


using namespace hnrt;


MagicPacket::MagicPacket(const MacAddress& mac)
{
    memset(m_data, 0xFF, 6);
    for (int i = 0; i < 16; i++)
    {
#pragma warning(suppress : 26451)
        memcpy_s(m_data + 6 + i * 6, 6, &mac, 6);
    }
}


MagicPacket::MagicPacket(const MagicPacket& src)
{
    memcpy_s(m_data, sizeof(m_data), src.m_data, sizeof(src.m_data));
}


MagicPacket& MagicPacket::operator =(const MagicPacket& src)
{
    memcpy_s(m_data, sizeof(m_data), src.m_data, sizeof(src.m_data));
    return *this;
}
