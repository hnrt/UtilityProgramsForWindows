#pragma once

#include "hnrt/BCryptHandle.h"
#include "hnrt/RefPtr.h"
#include "hnrt/SecretBuffer.h"

namespace hnrt
{
    class BCryptAlgHandle;

    class BCryptKeyHandle
        : protected BCryptHandle
    {
    public:

        BCryptKeyHandle();
        BCryptKeyHandle(const BCryptKeyHandle&) = delete;
        virtual ~BCryptKeyHandle();
        void operator =(const BCryptKeyHandle&) = delete;
        operator BCRYPT_KEY_HANDLE() const;
        void Generate(const BCryptAlgHandle&, void*, size_t);
        void Import(const BCryptAlgHandle&, RefPtr<SecretBuffer>);
        void Close();
        RefPtr<SecretBuffer> Export() const;
        RefPtr<SecretBuffer> Encrypt(void*, size_t, void*, size_t);
        RefPtr<SecretBuffer> Decrypt(void*, size_t, void*, size_t);
        DWORD get_KeyLength() const;

        __declspec(property(get = get_KeyLength)) DWORD KeyLength;

        PUCHAR m_pObject;
    };

    inline BCryptKeyHandle::BCryptKeyHandle()
        : BCryptHandle()
        , m_pObject(nullptr)
    {
    }

    inline BCryptKeyHandle::~BCryptKeyHandle()
    {
        if (m_h || m_pObject)
        {
            Close();
        }
    }

    inline BCryptKeyHandle::operator BCRYPT_KEY_HANDLE() const
    {
        return m_h;
    }

    inline DWORD BCryptKeyHandle::get_KeyLength() const
    {
        return GetPropertyDWORD(BCRYPT_KEY_LENGTH);
    }
}
