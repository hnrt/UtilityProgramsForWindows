#pragma once


#include <ws2def.h>


namespace hnrt
{
    class InternetAddress
    {
    public:

        InternetAddress();
        InternetAddress(const InternetAddress& src);
        ~InternetAddress() = default;
        InternetAddress& operator =(const InternetAddress& src);
        struct sockaddr* operator &();
        int get_Len() const;
        int get_Port() const;
        void set_Port(int value);
        bool SetHost(const char* psz);

        __declspec(property(get = get_Len)) int Len;
        __declspec(property(get = get_Port, put = set_Port)) int Port;

    private:

        struct sockaddr_in m_a;
    };

    inline struct sockaddr* InternetAddress::operator &()
    {
        return reinterpret_cast<struct sockaddr*>(&m_a);
    }

    inline int InternetAddress::get_Len() const
    {
        return sizeof(m_a);
    }
}
