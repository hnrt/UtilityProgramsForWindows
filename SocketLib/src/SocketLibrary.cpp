#include "pch.h"
#include "hnrt/SocketLibrary.h"
#include "hnrt/Debug.h"
#include "hnrt/Exception.h"


#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "Core")


using namespace hnrt;


SocketLibrary::SocketLibrary(unsigned short wMajorVersion, unsigned short wMinorVersion)
{
    DBGFNC(L"SocketLibrary::ctor(%u,%u)", wMajorVersion, wMinorVersion);

    memset(&m_wsaData, 0, sizeof(m_wsaData));

    int err = WSAStartup(MAKEWORD(wMajorVersion, wMinorVersion), &m_wsaData);
    if (err)
    {
        DBGPUT(L"WSAStartup failed: %d", err);
        throw Exception(L"WSAStartup failed. (Error code %d)", err);
    }
    DBGPUT(L"WSAStartup: major=%u minor=%u", MajorVersion, MinorVersion);
}


SocketLibrary::~SocketLibrary()
{
    DBGFNC(L"SocketLibrary::dtor");

    WSACleanup();
    DBGPUT(L"WSACleanup");
}


unsigned short SocketLibrary::get_MajorVersion() const
{
    return LOBYTE(m_wsaData.wVersion);
}


unsigned short SocketLibrary::get_MinorVersion() const
{
    return HIBYTE(m_wsaData.wHighVersion);
}
