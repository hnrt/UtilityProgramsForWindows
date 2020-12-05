#pragma once


#include <WinSock2.h>


namespace hnrt
{
    class Socket
    {
    public:

        Socket();
        Socket(const Socket&) = delete;
        ~Socket();
        Socket& operator =(const Socket&) = delete;
        operator SOCKET() const;
        int Create(int af, int type, int protocol = 0);
        int Close();

    private:

        SOCKET m_d;
    };

    inline Socket::operator SOCKET() const
    {
        return m_d;
    }
}
