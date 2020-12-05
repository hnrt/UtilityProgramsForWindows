#pragma once


#include <WinSock2.h>


namespace hnrt
{
    class SocketLibrary
    {
    public:

        SocketLibrary(unsigned short wMajorVersion = 2, unsigned short wMinorVersion = 2);
        SocketLibrary(const SocketLibrary&) = delete;
        ~SocketLibrary();
        SocketLibrary& operator =(const SocketLibrary&) = delete;
        unsigned short get_MajorVersion() const;
        unsigned short get_MinorVersion() const;

        __declspec(property(get = get_MajorVersion)) unsigned short MajorVersion;
        __declspec(property(get = get_MinorVersion)) unsigned short MinorVersion;

    private:

        WSADATA m_wsaData;
    };
}
