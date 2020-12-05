#pragma once


namespace hnrt
{
    class MacAddress;

    class MagicPacket
    {
    public:

        MagicPacket(const MacAddress& mac);
        MagicPacket(const MagicPacket& src);
        ~MagicPacket() = default;
        MagicPacket& operator =(const MagicPacket& src);
        const char* operator &() const;
        int get_Len() const;

        __declspec(property(get = get_Len)) int Len;

    private:

        unsigned char m_data[6 + 6 * 16];
    };

    inline const char* MagicPacket::operator &() const
    {
        return reinterpret_cast<const char*>(m_data);
    }

    inline int MagicPacket::get_Len() const
    {
        return static_cast<int>(sizeof(m_data));
    }
}
