#include "pch.h"
#include "hnrt/Socket.h"
#include "hnrt/Debug.h"


using namespace hnrt;


Socket::Socket()
    : m_d(INVALID_SOCKET)
{
    DBGFNC(L"Socket::ctor");
}


Socket::~Socket()
{
    DBGFNC(L"Socket::dtor");

    if (m_d != INVALID_SOCKET)
    {
        closesocket(m_d);
        DBGPUT(L"closesocket(%zu)", m_d);
    }
}


int Socket::Create(int af, int type, int protocol)
{
    DBGFNC(L"Socket::Create");

    if (m_d != INVALID_SOCKET)
    {
        closesocket(m_d);
        DBGPUT(L"closesocket(%zu)", m_d);
    }

    m_d = socket(af, type, protocol);
    DBGPUT(L"socket(%d,%d,%d): return=%zu", af, type, protocol, m_d);
    if (m_d == INVALID_SOCKET)
    {
        return WSAGetLastError();
    }

    return 0;
}


int Socket::Close()
{
    DBGFNC(L"Socket::Close");

    if (m_d != INVALID_SOCKET)
    {
        int rc = closesocket(m_d);
        DBGPUT(L"closesocket(%zu): return=%d", m_d, rc);
        m_d = INVALID_SOCKET;
        if (rc == SOCKET_ERROR)
        {
            return WSAGetLastError();
        }
    }

    return 0;
}
