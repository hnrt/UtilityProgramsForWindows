#pragma once


#include "hnrt/StringBuffer.h"
#include "hnrt/ByteString.h"


namespace hnrt
{
    class Base64Encoder
        : public StringBuffer
    {
    public:

        Base64Encoder();
        Base64Encoder(const Base64Encoder&) = delete;
        ~Base64Encoder() = default;
        void operator =(const Base64Encoder&) = delete;
        Base64Encoder& Append(const void* ptr, size_t len);
        Base64Encoder& End();

    protected:

        unsigned char m_Remaining;
        unsigned char m_Data[2];
    };

    class Base64Decoder
        : public ByteString
    {
    public:

        Base64Decoder();
        Base64Decoder(const Base64Decoder&) = delete;
        ~Base64Decoder() = default;
        void operator =(const Base64Decoder&) = delete;
        bool Parse(PCWSTR psz);
    };
}
