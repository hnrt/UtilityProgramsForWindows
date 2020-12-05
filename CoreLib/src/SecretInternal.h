#pragma once


#include <Windows.h>
#include <bcrypt.h>
#include "hnrt/Secret.h"
#include "hnrt/SecretBuffer.h"


namespace hnrt
{
    class SecretAlgHandle
    {
    public:

        SecretAlgHandle();
        SecretAlgHandle(const SecretAlgHandle&) = delete;
        ~SecretAlgHandle();
        void operator =(const SecretAlgHandle&) = delete;
        operator BCRYPT_ALG_HANDLE() const;
        BCRYPT_ALG_HANDLE* operator &();

    private:

        BCRYPT_ALG_HANDLE m_h;
    };

    inline SecretAlgHandle::SecretAlgHandle()
        : m_h(NULL)
    {
    }

    inline SecretAlgHandle::~SecretAlgHandle()
    {
        if (m_h)
        {
            BCryptCloseAlgorithmProvider(m_h, 0);
        }
    }

    inline SecretAlgHandle::operator BCRYPT_ALG_HANDLE() const
    {
        return m_h;
    }

    inline BCRYPT_ALG_HANDLE* SecretAlgHandle::operator &()
    {
        if (m_h)
        {
            BCryptCloseAlgorithmProvider(m_h, 0);
            m_h = NULL;
        }
        return &m_h;
    }

    class SecretKeyHandle
    {
    public:

        SecretKeyHandle();
        SecretKeyHandle(const SecretKeyHandle&) = delete;
        ~SecretKeyHandle();
        void operator =(const SecretKeyHandle&) = delete;
        operator BCRYPT_KEY_HANDLE() const;
        BCRYPT_KEY_HANDLE* operator &();

    private:

        BCRYPT_KEY_HANDLE m_h;
    };

    inline SecretKeyHandle::SecretKeyHandle()
        : m_h(NULL)
    {
    }

    inline SecretKeyHandle::~SecretKeyHandle()
    {
        if (m_h)
        {
            BCryptDestroyKey(m_h);
        }
    }

    inline SecretKeyHandle::operator BCRYPT_KEY_HANDLE() const
    {
        return m_h;
    }

    inline BCRYPT_KEY_HANDLE* SecretKeyHandle::operator &()
    {
        if (m_h)
        {
            BCryptDestroyKey(m_h);
            m_h = NULL;
        }
        return &m_h;
    }

    class SecretInternal
        : public Secret
    {
    public:

        SecretInternal();
        SecretInternal(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH]);
        SecretInternal(PCWSTR pszKey, PCWSTR pszIV);
        SecretInternal(const SecretInternal&) = delete;
        virtual ~SecretInternal();
        void operator =(const SecretInternal&) = delete;
        virtual void SetKey(const unsigned char* ptr, size_t len);
        virtual void SetIV(const unsigned char* ptr, size_t len);
        virtual void SetKey(PCWSTR psz);
        virtual void SetIV(PCWSTR psz);
        virtual void ClearBuffer();
        virtual void Encrypt(const void* ptr, size_t len);
        virtual void Decrypt(const void* ptr, size_t len);
        virtual const void* get_Ptr() const;
        virtual size_t get_Len() const;

    private:

        void Initialize();
        void InitializeKeyBlob(unsigned char key[SECRET_KEY_LENGTH]);
        void AddSalt(unsigned char* ptr, size_t len, unsigned char salt);

        unsigned char m_IV[SECRET_IV_LENGTH];
        SecretAlgHandle m_hAlg;
        SecretBuffer m_Object;
        SecretBuffer m_KeyBlob;
        SecretBuffer m_Processed;
    };
}
