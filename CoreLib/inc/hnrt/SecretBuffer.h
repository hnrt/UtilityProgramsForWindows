#pragma once

#include "hnrt/RefObj.h"
#include "hnrt/Buffer.h"

namespace hnrt
{
    class SecretBuffer
        : public RefObj
        , public Buffer<unsigned char>
    {
    public:

        SecretBuffer(size_t = 0);
        SecretBuffer(const SecretBuffer&) = delete;
        virtual ~SecretBuffer();
        void operator =(const SecretBuffer&) = delete;
        void Resize(size_t);
    };
}
