#pragma once


#include "hnrt/RefObj.h"


#define SECRET_KEY_LENGTH 32
#define SECRET_IV_LENGTH 16


namespace hnrt
{
    class Secret
        : public RefObj
    {
    public:

        Secret(const Secret&) = delete;
        virtual ~Secret() = default;
        void operator =(const Secret&) = delete;
        virtual void SetKey(const unsigned char* ptr, size_t len = SECRET_KEY_LENGTH) = 0;
        virtual void SetIV(const unsigned char* ptr, size_t len = SECRET_IV_LENGTH) = 0;
        virtual void SetKey(PCWSTR psz) = 0;
        virtual void SetIV(PCWSTR psz) = 0;
        virtual void ClearBuffer() = 0;
        virtual void Encrypt(const void* ptr, size_t len) = 0;
        virtual void Decrypt(const void* ptr, size_t len) = 0;
        virtual const void* get_Ptr() const = 0;
        virtual size_t get_Len() const = 0;
        __declspec(property(get = get_Ptr)) const void* Ptr;
        __declspec(property(get = get_Len)) size_t Len;

    protected:

        Secret();
    };

    inline Secret::Secret()
        : RefObj()
    {
    }
}
