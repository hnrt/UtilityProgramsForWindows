#pragma once


#include "hnrt/Buffer.h"


namespace hnrt
{
    class SecretBuffer
        : public Buffer<unsigned char>
    {
    public:

        SecretBuffer(size_t len = 0);
        SecretBuffer(const SecretBuffer&) = delete;
        ~SecretBuffer();
        void operator =(const SecretBuffer&) = delete;
        void Resize(size_t len);
    };
}
