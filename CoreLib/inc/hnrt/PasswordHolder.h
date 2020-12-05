#pragma once


#include "hnrt/Secret.h"
#include "hnrt/Buffer.h"


namespace hnrt
{
    class PasswordHolder
    {
    public:

        PasswordHolder(const unsigned char key[SECRET_KEY_LENGTH], const unsigned char iv[SECRET_IV_LENGTH]);
        PasswordHolder(const PasswordHolder&) = delete;
        ~PasswordHolder();
        void operator =(const PasswordHolder&) = delete;
        PCWSTR get_Encrypted() const;
        void set_Encrypted(PCWSTR psz);
        PCWSTR get_PlainText() const;
        void set_PlainText(const wchar_t *psz);
        void ClearPlainText();
        __declspec(property(get = get_Encrypted, put = set_Encrypted)) PCWSTR Encrypted;
        __declspec(property(get = get_PlainText, put = set_PlainText)) PCWSTR PlainText;

    private:

        RefPtr<Secret> m_pSecret;
        Buffer<wchar_t> m_Encrypted;
        Buffer<wchar_t> m_PlainText;
    };
}
