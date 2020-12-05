#include "pch.h"
#include <ws2tcpip.h>
#include "hnrt/InternetAddress.h"
#include "hnrt/Exception.h"


using namespace hnrt;


InternetAddress::InternetAddress()
{
    memset(&m_a, 0, sizeof(m_a));
    m_a.sin_family = AF_INET;
}


InternetAddress::InternetAddress(const InternetAddress& src)
{
    memcpy_s(&m_a, sizeof(m_a), &src.m_a, sizeof(src.m_a));
}


InternetAddress& InternetAddress::operator =(const InternetAddress& src)
{
    memcpy_s(&m_a, sizeof(m_a), &src.m_a, sizeof(src.m_a));
    return *this;
}


int InternetAddress::get_Port() const
{
    return static_cast<int>(ntohs(m_a.sin_port));
}


void InternetAddress::set_Port(int value)
{
    m_a.sin_port = htons(static_cast<u_short>(value));
}


bool InternetAddress::SetHost(const char* psz)
{
    int rc = inet_pton(AF_INET, psz, &m_a.sin_addr.s_addr);
    if (rc == -1)
    {
        throw Exception(L"inet_pton failed. (Error code %d)", WSAGetLastError());
    }
    return rc == 1 ? true : false;
}
