#pragma once


#include "hnrt/Secret.h"
#include "hnrt/ByteString.h"
#include "hnrt/BCryptAlgHandle.h"
#include "hnrt/BCryptKeyHandle.h"


namespace hnrt
{
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
        virtual void SetKey(const unsigned char* ptr, size_t len) override;
        virtual void SetIV(const unsigned char* ptr, size_t len) override;
        virtual void SetKey(PCWSTR psz) override;
        virtual void SetIV(PCWSTR psz) override;
        virtual void ClearBuffer() override;
        virtual void Encrypt(const void* ptr, size_t len) override;
        virtual void Decrypt(const void* ptr, size_t len) override;
        virtual const void* get_Ptr() const override;
        virtual size_t get_Len() const override;

    private:

        void AddSalt(unsigned char* ptr, size_t len, unsigned char salt);

        ByteString m_IV;
        BCryptAlgHandle m_hAlg;
        BCryptKeyHandle m_hKey;
        ByteString m_Processed;
    };
}
